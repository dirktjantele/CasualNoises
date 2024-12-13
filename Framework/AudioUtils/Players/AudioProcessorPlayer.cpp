/*
  ==============================================================================

    AudioProcessorPlayer.cpp
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "AudioProcessorPlayer.h"
#include "AudioBasics/Buffers/AudioBuffer.h"

#include "SystemConfig.h"

#include "Utilities/ReportFault.h"
#include "main.h"

namespace CasualNoises
{

// Space for statics
AudioProcessorPlayer	AudioProcessorPlayer::mAudioProcessorPlayer;
bool					AudioProcessorPlayer::mIsAllocated = false;
AudioProcessor* 		AudioProcessorPlayer::mAudioProcessorPtr = nullptr;
//I2S_HandleTypeDef*  	AudioProcessorPlayer::m_hi2sHandlePtr = nullptr;						// ToDo

// Binary semaphore used for interrupt/task synchronisation
static SemaphoreHandle_t xAudioProcessorPlayerSemaphore = nullptr;

// Codec input and output buffers
static int16_t _rawAudioBuffer[AUDIO_BUFFER_SIZE];
//static int16_t _rawmAudioBuffer[AUDIO_BUFFER_SIZE];
//static int16_t* audioBufferPtr  = _rawAudioBuffer;
//static int16_t* audioOutBufferPtr = _rawAudioOutputBuffer;

// This flag is set if an interrupt is received an cleared when processing the audio data is completed
//  so it should always be false at interrupt otherwise audio processing has taken to much time
static bool doGenerateAudio = false;
static bool checkForOverRun = false;

// Count how many times audio is processed for info only
static int64_t loopCounter = 0;

// interrupt callback for buffer half transfer completed
/*void HAL_I2SEx_TxRxHalfCpltCallback (I2S_HandleTypeDef * hi2s)								// ToDo
{
	if (doGenerateAudio && checkForOverRun) CN_ReportFault(1);
	doGenerateAudio = true;
	toggleBenchmarkPin(BENCHMARK_OUT_1_Pin);
	toggleBenchmarkPin(BENCHMARK_OUT_2_Pin);
	audioBufferPtr = &_rawAudioBuffer[0];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( xAudioProcessorPlayerSemaphore, &xHigherPriorityTaskWoken );
	toggleBenchmarkPin(BENCHMARK_OUT_1_Pin);
	toggleBenchmarkPin(BENCHMARK_OUT_2_Pin);
}*/

// interrupt callback for buffer full transfer completed
/*void HAL_I2SEx_TxRxCpltCallback (I2S_HandleTypeDef * hi2s)									// ToDo
{
	if (doGenerateAudio && checkForOverRun) CN_ReportFault(1);
	doGenerateAudio = true;
	toggleBenchmarkPin(BENCHMARK_OUT_1_Pin);
	toggleBenchmarkPin(BENCHMARK_OUT_2_Pin);
	audioBufferPtr = &_rawAudioBuffer[AUDIO_BUFFER_SIZE / 2];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( xAudioProcessorPlayerSemaphore, &xHigherPriorityTaskWoken );
	toggleBenchmarkPin(BENCHMARK_OUT_1_Pin);
	toggleBenchmarkPin(BENCHMARK_OUT_2_Pin);
}*/

//==============================================================================
//          runAudioProcessor
//
// This is the main audio processor loop, this method never return
//
//  CasualNoises    24/07/2023  First implementation
//==============================================================================
void AudioProcessorPlayer::runAudioProcessor(AudioProcessor* audioProcessor,
		AudioBuffer* audioBufferPtr)
{

	// Create a binary semaphore for task/interrupt synchronisation
	xAudioProcessorPlayerSemaphore = xSemaphoreCreateBinary();
	if (xAudioProcessorPlayerSemaphore == nullptr)
		CN_ReportFault(1);

	// Info used to fill the AudioBuffer
	static uint32_t numSamples = audioBufferPtr->getNumSamples();
	static sAudioBufferPtrs* pointers = audioBufferPtr->getAudioBufferPtrs();

	// Start DMA on I2S3
//	if ( HAL_I2SEx_TransmitReceive_DMA (m_hi2sHandlePtr, (uint16_t *)_rawAudioBuffer, (uint16_t *)_rawAudioBuffer, AUDIO_BUFFER_SIZE) != HAL_OK ) CN_ReportFault(1);	// ToDo

	// Prepare the audioprocessor
	audioProcessor->prepareToPlay(SAMPLE_FREQUENCY, numSamples);

	// Process incoming audio data block when a DMA interrupt is received
	for (;;)
	{

		// Wait for next interrupt
		BaseType_t res = xSemaphoreTake( xAudioProcessorPlayerSemaphore, portMAX_DELAY );
		if (res != pdPASS) CN_ReportFault(1);

//		toggleBenchmarkPin(BENCHMARK_OUT_2_Pin);							// ToDo

		// Counting is done for testing info only...
		++loopCounter;

		// Convert incoming audio data to float format and fill the AudioBuffer
		const float factor = 0x7fff;
		for (uint16_t i = 0, j = 0; i < numSamples; i += 2, ++j)
		{
			pointers->audioBuffer1[j] = (float)_rawAudioBuffer[i] / factor;
			pointers->audioBuffer2[j] = (float)_rawAudioBuffer[i + 1] / factor;
		}

		// Process the incoming audio data and generate new audio to send to the codec
		audioProcessor->processBlock(*audioBufferPtr);

		// Convert generated audio data back to int16_t format
		for (uint16_t i = 0, j = 0; i < numSamples; i += 2, ++j)
		{
			_rawAudioBuffer[i]     = (int16_t)(pointers->audioBuffer1[j] * factor);
			_rawAudioBuffer[i + 1] = (int16_t)(pointers->audioBuffer2[j] * factor);
		}

		// Processing is done, start checking for overruns
		doGenerateAudio = false;
		checkForOverRun = true;

//		toggleBenchmarkPin(BENCHMARK_OUT_2_Pin);							// ToDo

	}
}

} // namespace CasualNoises
