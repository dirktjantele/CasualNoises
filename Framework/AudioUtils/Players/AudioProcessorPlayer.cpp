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

#include "NerveNet/NerveNetMasterThread.h"

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

// Global NerveNet master thread pointers
extern CasualNoises::NerveNetMasterThread* gNerveNetMasterThreadPtr[MAX_NO_OF_NERVENET_MASTER_THREADS];

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
		AudioBuffer* audioBufferPtr,
		void (**nerveNetCallBackPtr)(CasualNoises::sNerveNetData*))
{

	// Create a binary semaphore for task/interrupt synchronisation
	xAudioProcessorPlayerSemaphore = xSemaphoreCreateBinary();
	if (xAudioProcessorPlayerSemaphore == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

	// Clear buffers
	for (uint32_t i = 0; i < FULL_AUDIO_BUFFER_SIZE; ++i)
	{
		tx_rawAudioBuffer[i] = 0;
		rx_rawAudioBuffer[i] = 0;
	}

	// Start DMA on I2Sx (note: size is the no of 16 bit words, so double the no of 32 bit words!!!)
	HAL_StatusTypeDef res =  HAL_I2SEx_TransmitReceive_DMA (m_hi2sHandlePtr,
									(uint16_t *)tx_rawAudioBuffer,
									(uint16_t *)rx_rawAudioBuffer,
									FULL_AUDIO_BUFFER_SIZE * 1);
	if (res != HAL_OK )
		CN_ReportFault(eErrorCodes::runtimeError);

	// Info used to fill the AudioBuffer
	uint32_t 			numSamples 		= audioBufferPtr->getNumSamples();
	sAudioBufferPtrs 	audioInPointers;
	audioBufferPtr->getAudioBufferPtrs(&audioInPointers);

	AudioBuffer* 		NN_audioBufferPtr  = nullptr;
#ifdef CASUALNOISES_NERVENET_THREAD
	// Create an audio buffer to hold audio coming through NerveNet
	NN_audioBufferPtr = new AudioBuffer();
	NN_audioBufferPtr->clearAudioBuffer();
	sAudioBufferPtrs 	NN_audioInPointers;
	NN_audioBufferPtr->getAudioBufferPtrs(&NN_audioInPointers);
#endif

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

#ifdef CASUALNOISES_NERVENET_THREAD
		// Start a new NerveNet data exchange
		sNerveNetMessage* nerveNetMessagePtr = gNerveNetMasterThreadPtr[AUDIO_NERVENET_THREAD_NO]->startNewDataExchange();

		// Handle NerveNet message
		if ((nerveNetMessagePtr != nullptr) && (nerveNetMessagePtr->header.messageNumber != 0))
		{

			// Do we have a call back pointer to handle the incoming NerveNet data?
			if (nerveNetCallBackPtr != nullptr)
			{
				(**nerveNetCallBackPtr)(&nerveNetMessagePtr->data);
			}

			// Fill NerveNet audio buffer with incoming audio
			float* ptr = nerveNetMessagePtr->audio.audioData;
			for (uint32_t i = 0; i < numSamples; ++i)
			{
				NN_audioInPointers.audioBuffer1[i] = *ptr++;
				NN_audioInPointers.audioBuffer2[i] = *ptr++;
			}

		}
#endif

		// Convert incoming audio data to float format and fill the AudioBuffer
		for (uint32_t i = 0, j = 0; i < (numSamples * 2); i += 2, ++j)
		{
			float fsample = static_cast<float>(rx_audioDataPtr[i]);
			fsample /= scale;
			audioInPointers.audioBuffer1[j] = (float)rx_audioDataPtr[i] / scale;
			audioInPointers.audioBuffer2[j] = (float)rx_audioDataPtr[i + 1] / scale;
		}

		// Process the incoming audio data and generate new audio to send to the codec
		mAudioProcessorPtr->processBlock(*audioBufferPtr, *NN_audioBufferPtr);

		// Convert generated audio data back to int32_t format
		float* lptr = audioInPointers.audioBuffer1;
		float* rptr = audioInPointers.audioBuffer2;
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
