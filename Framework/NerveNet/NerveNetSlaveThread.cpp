/*
  ==============================================================================

    NerveNetSlaveThread.cpp

    NerveNet communication slave side

    Created: 09/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NERVENET_THREAD

#include <string.h>

#include "NerveNetSlaveThread.h"
#include "NerveNetConfig.h"

#include "AudioBasics/Buffers/AudioBuffer.h"
#include "AudioProcessors/Processors/AudioProcessor.h"

#include "Utilities/ReportFault.h"

#include "Core/Maths/MathsFunctions.h"

#include "main.h"

CasualNoises::NerveNetSlaveThread* gNerveNetSlaveThreadPtr[MAX_NO_OF_NERVENET_SLAVE_THREADS];

namespace CasualNoises
{

struct sRunNerveNetSlaveThreadParams
{
	sNerveNetThreadData*				nerveNetThreadDataPtr;
	CasualNoises::NerveNetSlaveThread*  threadPtr;
};

//==============================================================================
//          sendMessage()
//
// Add a new message to the current filling Tx message buffer
// Return false if there was not enough room left in the buffer
//
//  CasualNoises    18/07/2025  First implementation
//  CasualNoises	25/07/2025	Made function thread save
//==============================================================================
bool NerveNetSlaveThread::sendMessage(const void* messagePtr, uint32_t size)
{

	// Block if another thread is using this function
	BaseType_t rtosRes = xSemaphoreTake(mSyncSemaphoreHandle, portMAX_DELAY);
	if (rtosRes != pdTRUE)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

	// Enough remaining buffer space?
	if (mRemainingSpace < size)
		return false;

	// Mark construction buffer busy
	mConstructionBufferBusy = true;

	// Add message to the construction buffer
	uint8_t* ptr = (uint8_t*)messagePtr;
	for (uint32_t i = 0; i < size; ++i)
		*mConstructionDataPtr++ = *ptr++;
	*mConstructionBufferSizePtr += size;
	mRemainingSpace -= size;

	// Release construction buffer
	mConstructionBufferBusy = false;

	// Release function for other threads
	rtosRes = xSemaphoreGive(mSyncSemaphoreHandle);
	if (rtosRes != pdTRUE)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

	return true;

}

//==============================================================================
//          GPIO_EXTI_Callback()
//
// Handle external interrupts
//
//  CasualNoises    11/07/2025  First implementation
//==============================================================================
bool NerveNetSlaveThread::GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	// Reset signal?
	if (GPIO_Pin == mNerveNet_RESET_Pin)
	{
		if (mThreadReady)
		{
			HAL_GPIO_WritePin(mNerveNet_ACK_Port, mNerveNet_ACK_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mNerveNet_ACK_Port, mNerveNet_ACK_Pin, GPIO_PIN_RESET);
			mThreadState = eNerveNetSlaveThreadState::idle;
		}
		return true;
	}

	// REQ signal
	if (GPIO_Pin == mNerveNet_REQ_Pin)
	{
		GPIO_PinState state = HAL_GPIO_ReadPin(mNerveNet_REQ_Port, GPIO_Pin);
		if (state == GPIO_PIN_SET)
		{

			// Start next cycle
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			xSemaphoreGiveFromISR(mStartCycleSemaphore, &xHigherPriorityTaskWoken);

		} else
		{

			// Rotate Tx and Rx buffers
			uint32_t temp = mTxToBeSentBufferIndex;
			mTxToBeSentBufferIndex = mTxWaitingBufferIndex;
			mTxWaitingBufferIndex  = mTxFillingBufferIndex;
			mTxFillingBufferIndex  = temp;
			temp = mRxReceivingBufferIndex;
			mRxReceivingBufferIndex  = mRxProcessingBufferIndex;
			mRxProcessingBufferIndex = temp;

			// End of NerveNet cycle, the thread should be awaitingMessage here
			if (mThreadState != eNerveNetSlaveThreadState::awaitingMessage)
				CN_ReportFault(eErrorCodes::NerveNetThread_Error);

			// We are ready for a new cycle
			mThreadState = eNerveNetSlaveThreadState::idle;

			// Set ACK line low
			HAL_GPIO_WritePin(mNerveNet_ACK_Port, mNerveNet_ACK_Pin, GPIO_PIN_RESET);

		}

		return true;
	}

	return false;
}

//==============================================================================
//          mainNerveNetSlaveThread()
//
// NerveNet slave thread
//
//  CasualNoises    11/07/2025  First implementation
//  CasualNoises    15/07/2025  First implementation implemented performance measurement
//==============================================================================
void NerveNetSlaveThread::mainNerveNetSlaveThread(void* pvParameters)
{

	// Get parameters
	sRunNerveNetSlaveThreadParams* nerveNetThreadDataPtr = (sRunNerveNetSlaveThreadParams*)pvParameters;
	mNerveNet_REQ_Port		 	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_REQ_Port;
	mNerveNet_REQ_Pin		 	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_REQ_Pin;
	mNerveNet_ACK_Port		 	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_ACK_Port;
	mNerveNet_ACK_Pin		 	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_ACK_Pin;
	mNerveNet_RESET_Port	 	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_RESET_Port;
	mNerveNet_RESET_Pin		 	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_RESET_Pin;
	mNerveNet_SPI_Ptr		 	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_SPI_Ptr;
	mSouthSideAudioProcessorPtr = nerveNetThreadDataPtr->nerveNetThreadDataPtr->AudioProcessorPtr;
	mPerformanceTestTimerPtr	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->PerformanceTestTimerPtr;
	mPerformanceResultPtr		= nerveNetThreadDataPtr->nerveNetThreadDataPtr->PerformanceResultPtr;

	// Create sendMessage() blocking semaphore
	vSemaphoreCreateBinary(mSyncSemaphoreHandle);
	if (mSyncSemaphoreHandle == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

	// Global pointer to be used by the HAL_GPIO_EXTI_Callback()
	mTheadNumber = nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNetThreadNo;
	gNerveNetSlaveThreadPtr[mTheadNumber] = this;

	// Create Tx and Rx buffers
	for (uint32_t i = 0; i < cNoOfTxMessageBuffers; ++i)
	{
		mTxMessageBuffers[i] = new sNerveNetMessage;
		memset(mTxMessageBuffers[i], 0x00, sizeof(sNerveNetMessage));
		mTxMessageBuffers[i]->header.messageSourceID = eNerveNetSourceId::eFellhornSouthSide;
		mTxMessageBuffers[i]->header.messageNumber = 0;
	}
	for (uint32_t i = 0; i < cNoOfRxMessageBuffers; ++i)
	{
		mRxMessageBuffers[i] = new sNerveNetMessage;
		memset(mRxMessageBuffers[i], 0x00, sizeof(sNerveNetMessage));
	}

	// Buffer used to build up data to be send on next exchange
	mConstructionBufferPtr 		 = new sNerveNetData;
	memset(mConstructionBufferPtr, 0, sizeof(sNerveNetData));
	mConstructionBufferSizePtr	 = &mConstructionBufferPtr->size;
	mConstructionDataPtr		 = &mConstructionBufferPtr->data[0];
	mRemainingSpace 			 = NERVENET_DATA_SIZE;
	mConstructionBufferBusy 	 = false;

	// Create a binary semaphore for task/interrupt synchronisation
	mStartCycleSemaphore = xSemaphoreCreateBinary();
	if (mStartCycleSemaphore == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

	// Audio buffers to be used in AudioProcessor::processBlock() call
#ifdef USE_AUDIO_BUFFER
	AudioBuffer* inBufferPtr  = new AudioBuffer();
	AudioBuffer* outBufferPtr = new AudioBuffer();
#else
	AudioBuffer* inBufferPtr  = nullptr;
	AudioBuffer* outBufferPtr = nullptr;
#endif

	// Thread is ready
	mThreadReady = true;

	// Start performance timer, if any
	if (mPerformanceTestTimerPtr != nullptr)
	{
		BaseType_t res = HAL_TIM_Base_Start(mPerformanceTestTimerPtr);
		if (res != HAL_OK)
			CN_ReportFault(eErrorCodes::NerveNetThread_Error);
		mPerformanceTestTimerPtr->Instance->CNT = 0;
	}

	// Theoretical cycle time in micro sec
#ifdef SAMPLE_FREQUENCY
	int32_t cycleTime = 1000000 / (SAMPLE_FREQUENCY / NUM_SAMPLES);
#else
	int32_t cycleTime = 0;
#endif

	// Main thread loop
	for (;;)
	{

		// Have to start an SPI txRx cycle?
		BaseType_t res = xSemaphoreTake(mStartCycleSemaphore, 0);
		if (res == pdPASS)
		{

			// Start of cycle time
			uint32_t startCycleTime = 0;
			if (mPerformanceTestTimerPtr != nullptr)
			{
				startCycleTime = mPerformanceTestTimerPtr->Instance->CNT;
			}

			// Start of NerveNet cycle, the thread should be idle here
			if (mThreadState != eNerveNetSlaveThreadState::idle)
				CN_ReportFault(eErrorCodes::NerveNetThread_Error);

			// If there is any data in the construction buffer, added it to current Tx message
			mTxMessageBuffers[mTxToBeSentBufferIndex]->data.size = 0;
			if (( ! mConstructionBufferBusy) && (*mConstructionBufferSizePtr > 0))
			{
				memcpy(&mTxMessageBuffers[mTxToBeSentBufferIndex]->data, mConstructionBufferPtr, *mConstructionBufferSizePtr + sizeof(uint32_t));
				*mConstructionBufferSizePtr  = 0;
				mConstructionDataPtr		 = &mConstructionBufferPtr->data[0];
				mRemainingSpace 			 = NERVENET_DATA_SIZE;
			}

			// Start SPI DMA in slave mode
			uint32_t size = sizeof(sNerveNetMessage);
			mTxMessageBuffers[mTxToBeSentBufferIndex]->header.messageNumber = ++mTxMessageNumber;
			HAL_StatusTypeDef res = HAL_SPI_TransmitReceive_DMA(mNerveNet_SPI_Ptr,
					(uint8_t *)mTxMessageBuffers[mTxToBeSentBufferIndex],
					(uint8_t *)mRxMessageBuffers[mRxReceivingBufferIndex],
					size);
			if (res != HAL_OK)
				CN_ReportFault(eErrorCodes::NerveNetThread_Error);

			// Start of AudioProcessor::processBlock() call
			uint32_t startOfProcessBlockTime = 0;
			UNUSED(startOfProcessBlockTime);
			if (mPerformanceTestTimerPtr != nullptr)
			{
				startOfProcessBlockTime = mPerformanceTestTimerPtr->Instance->CNT;
			}

			setTimeMarker_2();

			// Process any NerveNet data
			if (mRxMessageBuffers[mRxProcessingBufferIndex]->data.size > 0)
			{
				mSouthSideAudioProcessorPtr->processNerveNetData(
						mTheadNumber,
						mRxMessageBuffers[mRxProcessingBufferIndex]->data.size,
						&mRxMessageBuffers[mRxProcessingBufferIndex]->data.data[0]);
			}

			// Generate new audio in the current filling buffer using audio from the current processing buffer
#ifdef USE_AUDIO_BUFFER
			inBufferPtr->importAudio(mRxMessageBuffers[mRxProcessingBufferIndex]->audio.audioData);
			mSouthSideAudioProcessorPtr->processBlock(*inBufferPtr, *outBufferPtr);
			outBufferPtr->exportAudio(mTxMessageBuffers[mTxFillingBufferIndex]->audio.audioData);
#endif

			resetTimeMarker_2();

			// End of AudioProcessor::processBlock() call
			uint32_t endOfProcessBlockTime = 0;
			UNUSED(endOfProcessBlockTime);
			if (mPerformanceTestTimerPtr != nullptr)
			{
				endOfProcessBlockTime = mPerformanceTestTimerPtr->Instance->CNT;
			}

			// Wait for new message
			mThreadState = eNerveNetSlaveThreadState::awaitingMessage;

			// Set ACK line high
			HAL_GPIO_WritePin(mNerveNet_ACK_Port, mNerveNet_ACK_Pin, GPIO_PIN_SET);

			// end of cycle time
			uint32_t endCycleTime = 0;
			if (mPerformanceTestTimerPtr != nullptr)
			{
				endCycleTime = mPerformanceTestTimerPtr->Instance->CNT;
			}

			// Calculate cpu load
			int32_t elapsedlCycleTime = endCycleTime - startCycleTime;
			if (elapsedlCycleTime > 0)
				*mPerformanceResultPtr = jmap(elapsedlCycleTime, (int32_t)0, cycleTime, (int32_t)0, (int32_t)100);

		} else
		{
			vTaskDelay(pdMS_TO_TICKS(1));
		}

	}
}

//==============================================================================
//          runNerveNeSlaveThread()
//
// Run the thread inside a given NerveNetSlaveThread object
//
//  CasualNoises    11/07/2025  First implementation
//==============================================================================
void runNerveNetSlaveThread(void* pvParameters)
{
	sRunNerveNetSlaveThreadParams* params = (sRunNerveNetSlaveThreadParams*)pvParameters;
	params->threadPtr->mainNerveNetSlaveThread(pvParameters);
}

//==============================================================================
//          startNerveNetSlaveThread()
//
// Start NerveNet slave thread
//
//  CasualNoises    11/07/2025  First implementation
//==============================================================================
BaseType_t startNerveNetSlaveThread(CasualNoises::NerveNetSlaveThread* threadPtr, void *argument, TaskHandle_t* xHandlePtr)
{

	static sRunNerveNetSlaveThreadParams params;
	params.nerveNetThreadDataPtr = (sNerveNetThreadData*)argument;
	params.threadPtr = threadPtr;

	// Create the thread to scan the ADC convertions
	BaseType_t res = xTaskCreate(runNerveNetSlaveThread, "NerveNetSlaveThread", DEFAULT_STACK_SIZE / 2, &params,
			NERVENET_THRAD_PRIORITY, xHandlePtr);
	return res;

}

} // namespace CasualNoises

#endif

