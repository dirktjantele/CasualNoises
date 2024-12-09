/*
  ==============================================================================

    AudioProcessorPlayer.cpp
    Created: 24 July 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_PROCESSOR

//#include <arm_math.h>

#include "AudioProcessorPlayer.h"
#include "AudioBasics/Buffers/AudioBuffer.h"

#include "SystemConfig.h"

#include "Utilities/ReportFault.h"
#include "main.h"

// Space for statics
CasualNoises::AudioProcessorPlayer	CasualNoises::AudioProcessorPlayer::mAudioProcessorPlayer;
bool								CasualNoises::AudioProcessorPlayer::mIsAllocated 		= false;
CasualNoises::AudioProcessor* 		CasualNoises::AudioProcessorPlayer::mAudioProcessorPtr 	= nullptr;
I2S_HandleTypeDef*  				CasualNoises::AudioProcessorPlayer::m_hi2sHandlePtr 	= nullptr;

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
	if (doGenerateAudio && checkForOverRun) CasualNoises::CN_ReportFault(1);
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
	if (doGenerateAudio && checkForOverRun) CasualNoises::CN_ReportFault(1);
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
		CN_ReportFault(1);

	// Clear buffers
	for (uint32_t i = 0; i < FULL_AUDIO_BUFFER_SIZE; ++i)
	{
		tx_rawAudioBuffer[i] = 0; 	//i << 16;
		rx_rawAudioBuffer[i] = 0;		//i << 16;
	}
/*
	// Test pattern
	tx_rawAudioBuffer[0]   = 0xffffff00;
	tx_rawAudioBuffer[64]  = 0x3fffff00;
	tx_rawAudioBuffer[256] = 0x7fffff00;
	tx_rawAudioBuffer[1]   = 0xffffff00;
//	tx_rawAudioBuffer[128 + 32] = 0x7fffff00;
//	tx_rawAudioBuffer[128 + 96] = 0xffffff00;

	// Fill the audio buffer with a sine wave	float angle = 2.0f * pi * static_cast<float>(i) / static_cast<float>(arraySize);
	//constexpr float pi = 3.14159265358979323846f;
	uint32_t bufferSize = NUM_SAMPLES * 2;		// Stereo double buffering
	float angle = 0.0f;
	float step  = (2 * pi) / static_cast<float>(bufferSize);
	float scale = static_cast<float>(0x7fffffff);
	for (uint32_t i = 0; i < FULL_AUDIO_BUFFER_SIZE; i += 2)
	{
		float sine = sin(angle) + 1.0f;
		angle += step;
		uint32_t i_sine =  static_cast<uint32_t>(sine * scale);
		tx_rawAudioBuffer[i]   = i_sine;
		tx_rawAudioBuffer[i+1] = i_sine;
	}
*/
	// Start DMA on I2Sx (note: size is the no of 16 bit words, so double the no of 32 bit words!!!)
	HAL_StatusTypeDef res =  HAL_I2SEx_TransmitReceive_DMA (m_hi2sHandlePtr, (uint16_t *)tx_rawAudioBuffer, (uint16_t *)rx_rawAudioBuffer, FULL_AUDIO_BUFFER_SIZE * 1);
	if (res != HAL_OK )	CN_ReportFault(1);

	// Info used to fill the AudioBuffer
	uint32_t 			numSamples 	= audioBufferPtr->getNumSamples();
	sAudioBufferPtrs* 	pointers 	= audioBufferPtr->getAudioBufferPtrs();

	// Prepare the audio processor
	mAudioProcessorPtr->prepareToPlay(SAMPLE_FREQUENCY, numSamples);

	// Process incoming audio data block when a DMA interrupt is received
	for (;;)
	{

		// Wait for next interrupt
		BaseType_t res = xSemaphoreTake( xAudioProcessorPlayerSemaphore, portMAX_DELAY );
		if (res != pdPASS) CN_ReportFault(1);

		setTrigger_2();

		// Counting is done for testing info only...
		++loopCounter;

		// Convert incoming audio data to float format and fill the AudioBuffer
		float scale = static_cast<float>(0x7fffffff); // / 2.0f;
/*
		const float factor = (float)0x7fffffff;
		for (uint32_t i = 0, j = 0; i < numSamples; i += 2, ++j)
		{
			pointers->audioBuffer1[j] = (float)rx_audioDataPtr[i] / factor;
			pointers->audioBuffer2[j] = (float)rx_audioDataPtr[i + 1] / factor;
		}
*/
		// Process the incoming audio data and generate new audio to send to the codec
		mAudioProcessorPtr->processBlock(*audioBufferPtr);

		// Convert generated audio data back to int32_t format
		float* lptr = pointers->audioBuffer1;
		float* rptr = pointers->audioBuffer2;
		for (uint32_t i = 0; i < (numSamples * 2); i += 2)
		{
			tx_audioDataPtr[i]     = static_cast<uint32_t>((*lptr++ + 1.0f) * scale);
			tx_audioDataPtr[i + 1] = static_cast<uint32_t>((*rptr++ + 1.0f) * scale);
		}

		// Processing is done, start checking for overruns
		doGenerateAudio = false;
		checkForOverRun = true;

		resetTrigger_2();

	}
}

} // namespace CasualNoises

#endif
