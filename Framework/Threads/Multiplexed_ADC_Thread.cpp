/*
  ==============================================================================

    Multiplexed_ADC_Thread.cpp

    Handles ADC events using a multiplexer to handle multiple physical inputs
    	using a single MCU ADC input

    Created: 16/12/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_MULT_ADC_THREAD

#include "Multiplexed_ADC_Thread.h"
#include "Utilities/CallbackHandlers.h"
#include "Utilities/ReportFault.h"

#include "semphr.h"

namespace CasualNoises
{

namespace multiplexed_adc
{

// no of ADC values to use to calculate average values

ADC_HandleTypeDef* gMultiplexed_adc;										// Pointer to ADC interface handle
const uint32_t cNoOfMultiplexerInputs 		= 8;							// No of analog multiplexer inputs (8 on a 74HC4051)

int32_t gNoOfMultiplexers 					= 0;							// Number of analogue multiplexer chips (74HC4051)
sADC_MultiplexerSignature* gSignatures		= nullptr;						// Signature: port and pins for channel selection

const uint32_t 	cAverageSize				= 4;							// No of samples used to calculate the average value
uint16_t gRawAdcData[cNoOfMultiplexerInputs];								// Buffer for incoming ADC DMA data
uint16_t gAverageAdcData[cNoOfMultiplexerInputs][cNoOfMultiplexerInputs];	// Buffer for average values
uint16_t gPreviousAdcData[cNoOfMultiplexerInputs][cNoOfMultiplexerInputs];	//   "     "    "       "

SemaphoreHandle_t	gSemaphoreHandle 		= nullptr;

} // namespace multiplexed_adc

using namespace multiplexed_adc;

//==============================================================================
//          multiplexed_ADC_ConvCpltCallback()
//
// Conversion complete callback
//
//  CasualNoises    20/12/2025  First implementation
//==============================================================================
bool multiplexed_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

	static 		 uint32_t sum[cNoOfMultiplexerInputs];
	static		 uint32_t cnt				= 0;
	static const uint32_t bitShiftCnt		= 2; //__builtin_ctz(cAverageSize);

	static		 uint32_t multiplexChannel	= 0;

	// Only do this for the given ADC
	if (hadc == gMultiplexed_adc)
	{
		setTimeMarker_4();				// ToDo remove this line

		// Process ADC data, calculate average
		for (auto i = 0; i < gNoOfMultiplexers; ++i)
		{
			sum[i] += gRawAdcData[i];
		}
		++cnt;
		if (cnt == cAverageSize)
		{

			// Calculate average
			for (auto i = 0; i < gNoOfMultiplexers; ++i)
			{
				gAverageAdcData[i][multiplexChannel] = sum[i] >> bitShiftCnt;
				sum[i] = 0;
			}
			cnt = 0;

			// Proceed to next multiplexer channel
			++multiplexChannel;
			if (multiplexChannel == cNoOfMultiplexerInputs)
			{

				// Signal thread that one cycle over all inputs has completed
				BaseType_t xHigherPriorityTaskWoken = pdFALSE;
				if (gSemaphoreHandle != nullptr)
					xSemaphoreGiveFromISR(gSemaphoreHandle, &xHigherPriorityTaskWoken);

				// Select next multiplexer channel
				multiplexChannel = 0;

			}

			// Select next channel
			for (auto i = 0; i < gNoOfMultiplexers; ++i)
			{
				uint32_t mask = 0x00000001;
				for (auto c = 0; c < 3; ++c)
				{
					GPIO_PinState state;
					state = GPIO_PIN_RESET;
					if (multiplexChannel & mask)
						state = GPIO_PIN_SET;
					switch(c)
					{
					case 0: HAL_GPIO_WritePin(gSignatures[i].SO_Port, gSignatures[i].SO_Pin, state);
					break;
					case 1: HAL_GPIO_WritePin(gSignatures[i].S1_Port, gSignatures[i].S1_Pin, state);
					break;
					case 2: HAL_GPIO_WritePin(gSignatures[i].S2_Port, gSignatures[i].S2_Pin, state);
					break;
					}
					mask <<= 1;
				}
			}

		}

		// Restart ADC DMA cycle
		HAL_StatusTypeDef res;
		res = HAL_ADC_Start_DMA(gMultiplexed_adc, (uint32_t *)gRawAdcData, gNoOfMultiplexers);
		if (res != HAL_OK)
			CN_ReportFault(eErrorCodes::adcThreadError);

		resetTimeMarker_4();			// ToDo remove this line

		return true;
	} else
	{
		return false;
	}

}

//==============================================================================
//          multiplexed_ADC_Thread()
//
// Handle all multiplexed ADC events
//
//  CasualNoises    17/12/2025  First implementation
//==============================================================================
void multiplexed_ADC_Thread(void* pvParameters)
{

	// Get arguments
	sADC_MultiplexerThreadData* arguments 	= (sADC_MultiplexerThreadData*)pvParameters;
	gMultiplexed_adc 			= arguments->hadc;
	gNoOfMultiplexers			= arguments->noOfMultiplexers;
	gSignatures					= arguments->multiplexerSignatureArray;
	QueueHandle_t clientQueue 	= arguments->clientQueue;

	// Register a callback for the multiplexed adc
	add_ADC_ConvCpltCallback(multiplexed_ADC_ConvCpltCallback);
	vTaskDelay(pdMS_TO_TICKS(10));

	// Select first input for all multiplexers
	for (int32_t i = 0; i < gNoOfMultiplexers; ++i)
	{
		HAL_GPIO_WritePin(gSignatures[i].SO_Port, gSignatures[i].SO_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(gSignatures[i].S1_Port, gSignatures[i].S1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(gSignatures[i].S2_Port, gSignatures[i].S2_Pin, GPIO_PIN_RESET);
	}

	// Create a binary semaphore to awake thread when new data is available
	gSemaphoreHandle = xSemaphoreCreateBinary();
	if (gSemaphoreHandle == nullptr)
		CN_ReportFault(eErrorCodes::adcThreadError);

	// Calibrate the ADC converter used and start in DMA mode then start the timer to trigger conversions
	HAL_StatusTypeDef res;
	res = HAL_ADCEx_Calibration_Start(gMultiplexed_adc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);
	res = HAL_ADC_Start_DMA(gMultiplexed_adc, (uint32_t *)gRawAdcData, gNoOfMultiplexers);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);

	// Clear previous value buffer
	for (uint32_t i = 0; i < cNoOfMultiplexerInputs; ++i)
		for (uint32_t j= 0; j < cNoOfMultiplexerInputs; ++j)
			gPreviousAdcData[i][j] = 0x0000;

	// Main event loop

	auto foo = 0;

	auto treschp = 300;					// Deviation should exceed this value before sending events
	auto treschn = treschp * -1;
	for (;;)
	{

		// Wait for cycle completion semaphore
		BaseType_t res = xSemaphoreTake(gSemaphoreHandle, portMAX_DELAY);
		if (res != pdTRUE)
			CN_ReportFault(eErrorCodes::adcThreadError);

		// Compare new averages with previous ones
		for (uint32_t i = 0; i < cNoOfMultiplexerInputs; ++i)
		{
			uint8_t mask = gSignatures[i].mask;
			for (uint32_t j= 0; j < cNoOfMultiplexerInputs; ++j)
			{
				int32_t dev = gAverageAdcData[i][j] - gPreviousAdcData[i][j];
				if (((dev > treschp) || (dev < treschn)) &&
						(mask & 0x01))
				{

					if ((i == 0) && (std::abs(dev) > foo))													// ToDo remove test
						foo = std::abs(dev);

					// Send event to the client
					if (clientQueue != nullptr)
					{
						sMultiplexed_ADC_Event event;
						event.eventSourceID 		= eEventSourceID::multiplexerADC_ThreadSourceID;
						event.multiplexerNo			= i;
						event.multiplexerChannelNo	= j;
						event.value					= gAverageAdcData[i][j];
						event.deviation				= dev;
						BaseType_t res = xQueueSendToBack(clientQueue, (void*)&event, 10);
						if (res != pdPASS)
							CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
					}

					// Update previous value
					gPreviousAdcData[i][j] = gAverageAdcData[i][j];

				}
				mask >>= 1;
			}
		}
	}

}

//==============================================================================
//          startMultiplexed_ADC_Thread()
//
// Handle all multiplexed ADC events
//
//  CasualNoises    17/12/2025  First implementation
//==============================================================================
BaseType_t startMultiplexed_ADC_Thread(void *argument, TaskHandle_t* xHandlePtr)
{

	// Create the thread to scan the multiplexed ADC convertions
	BaseType_t res = xTaskCreate(multiplexed_ADC_Thread, "multiplexed_ADC_Thread", DEFAULT_STACK_SIZE / 2, argument,
			POT_THREAD_PRIORITY, xHandlePtr);
	return res;

}

} // namespace CasualNoises

#endif
