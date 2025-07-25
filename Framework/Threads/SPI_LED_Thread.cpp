/*
  ==============================================================================

    SPI_LED_Thread.cpp

    This thread handles led's connected to 74HC595 shift registers
    Communication is done using SPI using BDMA (Basic DMA)
    Current implementation supports up to 4 shift registers for a total of 32 led's

    Created: 04/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_LED_THREAD_SPI

#include "SPI_LED_Thread.h"

#include "YellowPages.h"

#include "Utilities/ReportFault.h"

#include "../Utilities/CallbackHandlers.h"

namespace CasualNoises
{

//==============================================================================
//          Global data for internal use by the thread
//==============================================================================
SPI_HandleTypeDef* 		LT_spiPtr;
GPIO_TypeDef* 			LT_latchPort;
uint16_t 				LT_latchPin;

constexpr uint32_t stateDepth = 25;
__attribute__((section(".RAM_D3"))) uint32_t state[stateDepth];

uint32_t				LT_CurrentStateNo = 0;

bool					LT_TxReady = false;

//==============================================================================
//          Handle_LED_RefreshInterrupts()
//
// Timer interrupt handler used to reresh the led's at a constant rate to support
//	led dimming and brightness control
// Called from HAL_TIM_PeriodElapsedCallback() in main.cpp
//
//  CasualNoises    04/07/2025  First implementation
//==============================================================================
void Handle_LED_RefreshInterrupts ()
{

	// Previous DMA request should be ready
	if ( ! LT_TxReady)
		return;

	// Start a new dma transfert
	HAL_StatusTypeDef res = HAL_SPI_Transmit_DMA(LT_spiPtr, (uint8_t*)&state[LT_CurrentStateNo], sizeof(uint32_t));
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::UI_ThreadError);
	LT_TxReady = false;

	// Goto next state entry
	LT_CurrentStateNo += 1;
	if (LT_CurrentStateNo >= stateDepth)
		LT_CurrentStateNo = 0;

}


//==============================================================================
//          handle_LT_DMA_Cplt()
//
// Handle dma complete callback
//
//  CasualNoises    05/07/2025  First implementation
//==============================================================================
bool handle_LT_DMA_Cplt(SPI_HandleTypeDef* hspi)
{
	if (hspi == LT_spiPtr)
	{

		// Pending dma transfert is completed
		LT_TxReady = true;

		// Latch the shift register
		HAL_GPIO_WritePin(LT_latchPort, LT_latchPin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LT_latchPort, LT_latchPin, GPIO_PIN_RESET);

		return true;
	} else
	{
		return false;
	}
}

//==============================================================================
//          LED_Thread()
//
// Main led driver thread
//
//  CasualNoises    04/07/2025  First implementation
//==============================================================================
void LED_Thread(void* pvParameters)
{

	// Get initial data
	sSPI_LED_ThreadData* dataPtr = (sSPI_LED_ThreadData*)pvParameters;
	TIM_HandleTypeDef* timerPtr	 = dataPtr->TimerHandle;
	LT_spiPtr 	 				 = dataPtr->LED_SPI_Handle;
	LT_latchPort 				 = dataPtr->SER_OUT_RCLK_GPIO__Port;
	LT_latchPin		 			 = dataPtr->SER_OUT_RCLK_GPIO_Pin;

	// Subscribe to DMA complete call-backs
	add_HAL_SPI_TxCpltCallback(handle_LT_DMA_Cplt);

	// Initialize the state buffer
	for (uint32_t i = 0; i < stateDepth; ++i)
		state[i] =  0xffffffff;

	// Start first dma transfert
	HAL_StatusTypeDef res =  HAL_SPI_Transmit_DMA(LT_spiPtr, (uint8_t*)&state, sizeof(uint32_t));
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::UI_ThreadError);
	LT_TxReady = false;
	HAL_GPIO_WritePin(LT_latchPort, LT_latchPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LT_latchPort, LT_latchPin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LT_latchPort, LT_latchPin, GPIO_PIN_RESET);

	// Create a message queue
	const uint32_t cQueueLength = 10;
	QueueHandle_t xUI_ThreadQueue = xQueueCreate( cQueueLength, sizeof(sLED_Event) );
	if (xUI_ThreadQueue == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	gYellowPages.gLED_ThreadQueueHandle = xUI_ThreadQueue;

	// Start refresh interrupt timer
	res = HAL_TIM_Base_Start_IT (timerPtr);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::UI_ThreadError);

	// Report UI thread ready for duty
	gYellowPages.gLED_ThreadRunning = true;

	// Main thread loop
	for (;;)
	{

		sLED_Event event;
		BaseType_t res = xQueueReceive(xUI_ThreadQueue, &event, portMAX_DELAY);
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::UI_ThreadError);

		uint32_t ledBitNum = event.ledBitNum << 8;

		uint32_t noOfOnTicks = (event.ledIntensity * stateDepth) / 100;
		uint32_t indx = 0;
		for ( ; indx < noOfOnTicks; ++indx)
			state[indx] &= ~ledBitNum;
		for ( ; indx < stateDepth; ++indx)
			state[indx] |= ledBitNum;

	}

}

//==============================================================================
//          start_LED_Thread()
//
// Start the thread with given parameters and return a handle to it
//
//  CasualNoises    04/07/2025  First implementation
//==============================================================================
BaseType_t start_LED_Thread(void *argument, TaskHandle_t* xHandlePtr)
{

	// Create the thread to drive the led's
	BaseType_t res = xTaskCreate(LED_Thread, "LED_Thread", DEFAULT_STACK_SIZE / 2, argument,
			UI_THREAD_PRIORITY,	xHandlePtr);
	gYellowPages.gLED_ThreadTaskHandle = *xHandlePtr;
	return res;

}

} // namespace CasualNoises

#endif
