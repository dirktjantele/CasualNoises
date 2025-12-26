/*
  ==============================================================================

    NerveNetMasterThread.h

    NerveNet communication master side

    Created: 09/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NERVENET_THREAD

#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "NerveNetMessage.h"
#include "NerveNetMessageHeader.h"

namespace CasualNoises
{

// Current thread state
enum class eNerveNetMasterThreadState
{
	resetSlave,
	idle,
	awaitingAck,
	awaitingDmaCplt,
	awaitingNotAck,
};

class NerveNetMasterThread final
{
public:

	 NerveNetMasterThread() = default;
	~NerveNetMasterThread() = default;

	void mainNerveNetMasterThread(void* pvParameters);

	bool GPIO_EXTI_Callback(uint16_t GPIO_Pin);
	bool SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);

	bool sendMessage(const void* messagePtr, uint32_t size) noexcept;

	sNerveNetMessage* startNewDataExchange() noexcept;

private:

	// Tx and Rx buffers & indexes
	sNerveNetMessage* 		mTxMessageBuffers[cNoOfTxMessageBuffers];		// Tx buffers
	sNerveNetMessage* 		mRxMessageBuffers[cNoOfRxMessageBuffers];		// Rx buffers
	uint32_t	 			mTxToBeSentBufferIndex		{ 0 };				// Indexes to Tx & Rx buffers
	uint32_t				mTxWaitingBufferIndex		{ 1 };
	uint32_t				mTxFillingBufferIndex		{ 2 };
	uint32_t				mRxReceivingBufferIndex		{ 0 };
	uint32_t				mRxProcessingBufferIndex	{ 1 };

	// Message number counter
	uint32_t				mTxMessageNumber			{ 0 };

	// Buffer used to build up message data to be send with next message exchange
	sNerveNetData*			mConstructionBufferPtr		{ nullptr };
	uint32_t*				mConstructionBufferSizePtr	{ nullptr };
	uint8_t*				mConstructionDataPtr		{ nullptr };
	uint32_t				mRemainingSpace				{ 0 };
	bool					mConstructionBufferBusy		{ false };

	// Devices to be used
	GPIO_TypeDef*			mNerveNet_REQ_Port			{ nullptr };
	uint16_t				mNerveNet_REQ_Pin			{ 0 };
	GPIO_TypeDef*			mNerveNet_ACK_Port			{ nullptr };
	uint16_t				mNerveNet_ACK_Pin			{ 0 };
	GPIO_TypeDef*			mNerveNet_RESET_Port		{ nullptr };
	uint16_t				mNerveNet_RESET_Pin			{ 0 };
	SPI_HandleTypeDef*		mNerveNet_SPI_Ptr			{ nullptr };

	// Current thread state
	eNerveNetMasterThreadState	mThreadState			{ eNerveNetMasterThreadState::resetSlave };

	// Semaphore used to make sendMessage() thread save
	SemaphoreHandle_t		mSyncSemaphoreHandle		{ nullptr };

};

// Start thread function
BaseType_t startNerveNetMasterThread(CasualNoises::NerveNetMasterThread* threadPtr, void *argument, TaskHandle_t* xHandlePtr) noexcept;

} // namespace CasualNoises

extern CasualNoises::NerveNetMasterThread* gNerveNetMasterThreadPtr[MAX_NO_OF_NERVENET_MASTER_THREADS];

#endif
