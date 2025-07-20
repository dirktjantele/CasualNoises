/*
  ==============================================================================

    NerveNetMasterThread.cpp

    NerveNet communication master side

    Created: 09/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NERVENET_THREAD

#include <string.h>			// Required for memset()

#include "NerveNetMasterThread.h"
#include "NerveNetConfig.h"

#include "Utilities/ReportFault.h"

#include "FreeRTOS.h"

CasualNoises::NerveNetMasterThread* gNerveNetMasterThreadPtr[MAX_NO_OF_NERVENET_MASTER_THREADS];

namespace CasualNoises
{

struct sRunNerveNetMasterThreadParams
{
	sNerveNetThreadData*				nerveNetThreadDataPtr;
	CasualNoises::NerveNetMasterThread* threadPtr;
};

//==============================================================================
//          sendMessage()
//
// Add a new message to the current filling Tx message buffer
// Return false if there was not enough room left in the buffer
//
//  CasualNoises    18/07/2025  First implementation
//==============================================================================

// ToDo make this method thread safe, currently only used by the UI thread

bool NerveNetMasterThread::sendMessage(const void* messagePtr, uint32_t size)
{

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

	return true;

}

//==============================================================================
//          startNewDataExchange()
//
// Start new data exchange with the NerveNet slave device
//	This method is called from AudioProcessorPlayer::runAudioProcessor()
// Returns a pointer to the previous received NerveNet message
//
//  CasualNoises    11/07/2025  First implementation
//==============================================================================
sNerveNetMessage* NerveNetMasterThread::startNewDataExchange()
{

	// Ignore request while reseting slave thread
	if (mThreadState == eNerveNetMasterThreadState::resetSlave)
		return mRxMessageBuffers[mRxProcessingBufferIndex];

	// Thread should be idle in order to start a new cycle
	if (mThreadState != eNerveNetMasterThreadState::idle)
		return nullptr;

	// Swap Rx buffers
	uint32_t temp = mRxReceivingBufferIndex;
	mRxReceivingBufferIndex = mRxProcessingBufferIndex;
	mRxProcessingBufferIndex = temp;

	// Set Request line high to indicate the get attention from the slave thread
	mThreadState = eNerveNetMasterThreadState::awaitingAck;
	HAL_GPIO_WritePin(mNerveNet_REQ_Port, mNerveNet_REQ_Pin, GPIO_PIN_SET);

	// Return message to be processed by the AudioProcessorPlayer next
	return mRxMessageBuffers[mRxProcessingBufferIndex];

}

//==============================================================================
//          GPIO_EXTI_Callback()
//
// Handle external interrupts
//
//  CasualNoises    11/07/2025  First implementation
//==============================================================================
bool NerveNetMasterThread::GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == mNerveNet_ACK_Pin)
	{
		switch (mThreadState)
		{

		// ACK while idle
		case eNerveNetMasterThreadState::idle:
			break;

		// ACK on reset request
		case eNerveNetMasterThreadState::resetSlave:
			mThreadState = eNerveNetMasterThreadState::idle;
			break;

		// ACK on transfer request
		case eNerveNetMasterThreadState::awaitingAck:
		{

			// Next state is awaiting DMA completion
			mThreadState = eNerveNetMasterThreadState::awaitingDmaCplt;

			// If there is any data in the construction buffer, added it to current Tx message
			mTxMessageBuffers[mTxToBeSentBufferIndex]->data.size = 0;
			if (( ! mConstructionBufferBusy) && (*mConstructionBufferSizePtr > 0))
			{
				memcpy(&mTxMessageBuffers[mTxToBeSentBufferIndex]->data, mConstructionBufferPtr, *mConstructionBufferSizePtr + sizeof(uint32_t));
				*mConstructionBufferSizePtr  = 0;
				mConstructionDataPtr		 = &mConstructionBufferPtr->data[0];
				mRemainingSpace 			 = NERVENET_DATA_SIZE;
			}

			// Start data exchange
			uint32_t size = sizeof(sNerveNetMessage);
			mTxMessageBuffers[mTxToBeSentBufferIndex]->header.messageNumber = ++mTxMessageNumber;
			HAL_StatusTypeDef res = HAL_SPI_TransmitReceive_DMA(mNerveNet_SPI_Ptr,
					(uint8_t *)mTxMessageBuffers[mTxToBeSentBufferIndex],
					(uint8_t *)mRxMessageBuffers[mRxReceivingBufferIndex],
					size);
			if (res != HAL_OK)
				CN_ReportFault(eErrorCodes::NerveNetThread_Error);

			// Rotate Tx buffers
			uint32_t temp = mTxToBeSentBufferIndex;
			mTxToBeSentBufferIndex = mTxWaitingBufferIndex;
			mTxWaitingBufferIndex  = mTxFillingBufferIndex;
			mTxFillingBufferIndex  = temp;

		}
			break;

		// not ACK to signal that slave is idle again
		case eNerveNetMasterThreadState::awaitingNotAck:
			mThreadState = eNerveNetMasterThreadState::idle;
			break;

		default:
			CN_ReportFault(eErrorCodes::NerveNetThread_Error);
		}
		return true;
	}
	return false;
}

//==============================================================================
//          SPI_TxRxCpltCallback()
//
// Handle SPI TxRx complete callbacks
//
//  CasualNoises    12/07/2025  First implementation
//==============================================================================
bool NerveNetMasterThread::SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == mNerveNet_SPI_Ptr)
	{
		if (mThreadState == eNerveNetMasterThreadState::awaitingDmaCplt)
		{

			// Next state is awaiting ACK line to go low
			mThreadState = eNerveNetMasterThreadState::awaitingNotAck;

			// Set REQ line low
			HAL_GPIO_WritePin(mNerveNet_REQ_Port, mNerveNet_REQ_Pin, GPIO_PIN_RESET);

		} else
		{
			CN_ReportFault(eErrorCodes::NerveNetThread_Error);
		}

		return true;
	}
	return false;
}

//==============================================================================
//          mainNerveNetMasterThread()
//
// NerveNet master thread
//
//  CasualNoises    09/07/2025  First implementation
//==============================================================================
void NerveNetMasterThread::mainNerveNetMasterThread(void* pvParameters)
{

	// Get parameters
	sRunNerveNetMasterThreadParams* nerveNetThreadDataPtr = (sRunNerveNetMasterThreadParams*)pvParameters;
	mNerveNet_REQ_Port		= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_REQ_Port;
	mNerveNet_REQ_Pin		= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_REQ_Pin;
	mNerveNet_ACK_Port		= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_ACK_Port;
	mNerveNet_ACK_Pin		= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_ACK_Pin;
	mNerveNet_RESET_Port	= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_RESET_Port;
	mNerveNet_RESET_Pin		= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_RESET_Pin;
	mNerveNet_SPI_Ptr		= nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNet_SPI_Ptr;

	// Global pointer to be used by the HAL_GPIO_EXTI_Callback()
	gNerveNetMasterThreadPtr[nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNetThreadNo] = this;

	// Create Tx and Rx buffers
	for (uint32_t i = 0; i < cNoOfTxMessageBuffers; ++i)
	{
		mTxMessageBuffers[i] = new sNerveNetMessage;
		memset(mTxMessageBuffers[i], 0, sizeof(sNerveNetMessage));
		mTxMessageBuffers[i]->header.messageSourceID = eNerveNetSourceId::eFellhornNortSide;
		mTxMessageBuffers[i]->header.messageNumber = 0;
	}
	for (uint32_t i = 0; i < cNoOfRxMessageBuffers; ++i)
	{
		mRxMessageBuffers[i] = new sNerveNetMessage;
		mRxMessageBuffers[i]->header.messageNumber = 0;
	}

	// Buffer used to build up data to be send on next exchange
	mConstructionBufferPtr 		 = new sNerveNetData;
	memset(mConstructionBufferPtr, 0, sizeof(sNerveNetData));
	mConstructionBufferSizePtr	 = &mConstructionBufferPtr->size;
	mConstructionDataPtr		 = &mConstructionBufferPtr->data[0];
	mRemainingSpace 			 = NERVENET_DATA_SIZE;
	mConstructionBufferBusy 	 = false;

	// Reset NerveNet slave
	while (mThreadState == eNerveNetMasterThreadState::resetSlave)
	{
		HAL_GPIO_WritePin(mNerveNet_RESET_Port, mNerveNet_RESET_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(mNerveNet_RESET_Port, mNerveNet_RESET_Pin, GPIO_PIN_RESET);
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	// ToDo: create a queue for incoming message data
	if (nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNetThreadQueue != nullptr)
	{

	}

	// Signal that thread is up and running
	if (nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNetRunningFlagPtr != nullptr)
	{
		*nerveNetThreadDataPtr->nerveNetThreadDataPtr->NerveNetRunningFlagPtr = true;
	}

	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}

//==============================================================================
//          runNerveNetMasterThread()
//
// Run the thread inside a given NerveNetMasterThread object
//
//  CasualNoises    09/07/2025  First implementation
//==============================================================================
void runNerveNetMasterThread(void* pvParameters)
{
	sRunNerveNetMasterThreadParams* params = (sRunNerveNetMasterThreadParams*)pvParameters;
	params->threadPtr->mainNerveNetMasterThread(pvParameters);
}

//==============================================================================
//          startNerveNetMasterThread()
//
// Start NerveNet master thread
//
//  CasualNoises    09/07/2025  First implementation
//==============================================================================
BaseType_t startNerveNetMasterThread(CasualNoises::NerveNetMasterThread* threadPtr, void *argument, TaskHandle_t* xHandlePtr)
{
	assert(MAX_NO_OF_NERVENET_MASTER_THREADS == 1);

	static sRunNerveNetMasterThreadParams params;
	params.nerveNetThreadDataPtr = (sNerveNetThreadData*)argument;
	params.threadPtr = threadPtr;

	// Create the thread to scan the ADC convertions
	BaseType_t res = xTaskCreate(runNerveNetMasterThread, "NerveNetMasterThread", DEFAULT_STACK_SIZE / 2, &params,
			NERVENET_THRAD_PRIORITY, xHandlePtr);
	return res;

}

} // namespace CasualNoises

#endif

