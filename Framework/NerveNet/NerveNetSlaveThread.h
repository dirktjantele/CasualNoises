/*
  ==============================================================================

    NerveNetSlaveThread.h

    NerveNet communication slave side

    Created: 09/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#ifdef CASUALNOISES_NERVENET_THREAD

#include "FreeRTOS.h"
#include "semphr.h"

#include "NerveNetMessage.h"
#include "NerveNetSlaveProcessor.h"

namespace CasualNoises
{

enum class eNerveNetSlaveThreadState
{
	awaitingReset,
	idle,
	awaitingMessage
};

class AudioProcessor;

class NerveNetSlaveThread final
{
public:

	 NerveNetSlaveThread () = default;
	~NerveNetSlaveThread () = default;

	void mainNerveNetSlaveThread ( void* pvParameters );

	bool sendMessage ( const void* messagePtr, uint32_t size ) noexcept;

	bool GPIO_EXTI_Callback ( uint16_t GPIO_Pin );

	void setNerveNetSlaveProcessorPtr ( NerveNetSlaveProcessor* ptr ) noexcept { mNerveNetSlaveProcessorPtr = ptr; };

private:

	// Ignore all interrupts until the thread is ready
	bool					mIgnoreInterrupts			{ true };

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

	// Flag used to indicate that the thread is ready, thread will not respond to reset signals if not ready
	bool					mThreadReady				{ false };

	// Devices to be used
	GPIO_TypeDef*			mNerveNet_REQ_Port			{ nullptr };
	uint16_t				mNerveNet_REQ_Pin			{ 0 };
	GPIO_TypeDef*			mNerveNet_ACK_Port			{ nullptr };
	uint16_t				mNerveNet_ACK_Pin			{ 0 };
	GPIO_TypeDef*			mNerveNet_RESET_Port		{ nullptr };
	uint16_t				mNerveNet_RESET_Pin			{ 0 };
	SPI_HandleTypeDef*		mNerveNet_SPI_Ptr			{ nullptr };
	AudioProcessor* 		mSouthSideAudioProcessorPtr	{ nullptr };				// ToDo uniform NerveNet message handler
	NerveNetSlaveProcessor* mNerveNetSlaveProcessorPtr	{ nullptr };
	TIM_HandleTypeDef*		mPerformanceTestTimerPtr	{ nullptr };
	uint32_t*				mPerformanceResultPtr		{ nullptr };

	// Thread number
	uint32_t				mTheadNumber				{ 0xffffffff };

	// Current thread state
	eNerveNetSlaveThreadState	mThreadState			{ eNerveNetSlaveThreadState::awaitingReset };

	// Binary semaphore used for interrupt/task synchronisation
	SemaphoreHandle_t		mStartCycleSemaphore;

	// Semaphore used to make sendMessage() thread save
	SemaphoreHandle_t		mSyncSemaphoreHandle		{ nullptr };

};

// Start thread function
BaseType_t startNerveNetSlaveThread(CasualNoises::NerveNetSlaveThread* threadPtr, void *argument, TaskHandle_t* xHandlePtr) noexcept;

} // namespace CasualNoises

extern CasualNoises::NerveNetSlaveThread* gNerveNetSlaveThreadPtr[MAX_NO_OF_NERVENET_SLAVE_THREADS];

// Pointers to slave threads

#endif
