/*
  ==============================================================================

    AudioProcessorPlayer.cpp
    Created: 24 July 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_PROCESSOR_PLAYER

//#include <arm_math.h>

#include "AudioProcessorPlayer.h"
#include "AudioBasics/Buffers/AudioBuffer.h"

#include "SystemConfig.h"

#include "Utilities/ReportFault.h"
#include "main.h"

// Space for statics
CasualNoises::AudioProcessorPlayer	CasualNoises::AudioProcessorPlayer::mAudioProcessorPlayer;
bool								CasualNoises::AudioProcessorPlayer::mIsAllocated 			= false;
CasualNoises::AudioProcessor* 		CasualNoises::AudioProcessorPlayer::mAudioProcessorPtr 		= nullptr;
I2S_HandleTypeDef*  				CasualNoises::AudioProcessorPlayer::m_hi2sHandlePtr 		= nullptr;
void*								CasualNoises::AudioProcessorPlayer::mSynthesiserParamsPtr	= nullptr;

// Binary semaphore used for interrupt/task synchronisation
static SemaphoreHandle_t xAudioProcessorPlayerSemaphore = nullptr;

// Codec input and output buffers
static int32_t rx_rawAudioBuffer[FULL_AUDIO_BUFFER_SIZE];
static int32_t* rx_audioDataPtr  = rx_rawAudioBuffer;
static int32_t tx_rawAudioBuffer[FULL_AUDIO_BUFFER_SIZE];
static int32_t* tx_audioDataPtr  = tx_rawAudioBuffer;

// This flag is set if an interrupt is received an cleared when processing the audio data is completed
//  so it should always be false at interrupt otherwise audio processing has taken to much time
static bool doGenerateAudio = false;
static bool checkForOverRun = false;

// Count how many times audio is processed for info only...
static int64_t loopCounter = 0;

// Interrupt callback for buffer half transfer completed
void HAL_I2SEx_TxRxHalfCpltCallback (I2S_HandleTypeDef * hi2s)
{
	if (doGenerateAudio && checkForOverRun)
		CN_ReportFault(eErrorCodes::runtimeError);
	doGenerateAudio = true;
	checkForOverRun = false;
	tx_audioDataPtr = &tx_rawAudioBuffer[0];
	rx_audioDataPtr = &rx_rawAudioBuffer[0];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( xAudioProcessorPlayerSemaphore, &xHigherPriorityTaskWoken );
}

// Interrupt callback for buffer full transfer completed
void HAL_I2SEx_TxRxCpltCallback (I2S_HandleTypeDef * hi2s)
{
	if (doGenerateAudio && checkForOverRun)
		CN_ReportFault(eErrorCodes::runtimeError);
	doGenerateAudio = true;
	checkForOverRun = false;
	tx_audioDataPtr = &tx_rawAudioBuffer[FULL_AUDIO_BUFFER_SIZE / 2];
	rx_audioDataPtr = &rx_rawAudioBuffer[FULL_AUDIO_BUFFER_SIZE / 2];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( xAudioProcessorPlayerSemaphore, &xHigherPriorityTaskWoken );
}

namespace CasualNoises
{

//==============================================================================
//          runAudioProcessor
//
// This is the main audio processor loop, this method never return
//
//  CasualNoises    24/07/2023  First implementation
// 	CasualNoises    10/11/2024  Resuscitated
//==============================================================================
void AudioProcessorPlayer::runAudioProcessor(
		/*AudioProcessor* audioProcessor,*/
		AudioBuffer* audioBufferPtr)
{

	// Create a binary semaphore for task/interrupt synchronisation
	xAudioProcessorPlayerSemaphore = xSemaphoreCreateBinary();
	if (xAudioProcessorPlayerSemaphore == nullptr)
		CN_ReportFault(eErrorCodes::runtimeError);

	// Clear buffers
	for (uint32_t i = 0; i < FULL_AUDIO_BUFFER_SIZE; ++i)
	{
		tx_rawAudioBuffer[i] = 0; 	//i << 16;
		rx_rawAudioBuffer[i] = 0;		//i << 16;
	}

	// Start DMA on I2Sx (note: size is the no of 16 bit words, so double the no of 32 bit words!!!)
	HAL_StatusTypeDef res =  HAL_I2SEx_TransmitReceive_DMA (m_hi2sHandlePtr,
									(uint16_t *)tx_rawAudioBuffer,
									(uint16_t *)rx_rawAudioBuffer,
									FULL_AUDIO_BUFFER_SIZE * 1);
	if (res != HAL_OK )
		CN_ReportFault(eErrorCodes::runtimeError);

	// Info used to fill the AudioBuffer
	uint32_t 			numSamples 	= audioBufferPtr->getNumSamples();
	sAudioBufferPtrs* 	pointers 	= audioBufferPtr->getAudioBufferPtrs();

	// Prepare the audio processor
	mAudioProcessorPtr->prepareToPlay(SAMPLE_FREQUENCY, numSamples, mSynthesiserParamsPtr);

	// Process incoming audio data block when a DMA interrupt is received
	float scale = static_cast<float>(0x7fffff00);
	for (;;)
	{

		// Wait for next interrupt
		BaseType_t res = xSemaphoreTake( xAudioProcessorPlayerSemaphore, portMAX_DELAY );
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::runtimeError);

		// Mark start of audio processing
		setTimeMarker_2();

		// Counting is done for testing info only...
		++loopCounter;

		// Convert incoming audio data to float format and fill the AudioBuffer
		for (uint32_t i = 0, j = 0; i < numSamples; i += 2, ++j)
		{
			uint32_t isample = rx_audioDataPtr[i];
			float fsample = static_cast<float>(rx_audioDataPtr[i]);
			fsample /= scale;
			pointers->audioBuffer1[j] = (float)rx_audioDataPtr[i] / scale;
			pointers->audioBuffer2[j] = (float)rx_audioDataPtr[i + 1] / scale;
		}

		// Process the incoming audio data and generate new audio to send to the codec
		mAudioProcessorPtr->processBlock(*audioBufferPtr);

		// Convert generated audio data back to int32_t format
		float* lptr = pointers->audioBuffer1;
		float* rptr = pointers->audioBuffer2;
		for (uint32_t i = 0; i < (numSamples * 2); i += 2)
		{
			tx_audioDataPtr[i]     = static_cast<int32_t>(*lptr++ * scale);
			tx_audioDataPtr[i + 1] = static_cast<int32_t>(*rptr++ * scale);
		}

		// Processing is done, start checking for overruns
		doGenerateAudio = false;
		checkForOverRun = true;

		// Mark end of audio processing
		resetTimeMarker_2();

	}
}

} // namespace CasualNoises

#endif
