/*
  ==============================================================================

    PotentiometerThread.cpp

    Handles potentiometers events

    Created: 25/10/2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_POTENTIOMETER_THREAD

#include "FreeRTOS.h"

#include "PotentiometerThread.h"
#include "Utilities/CallbackHandlers.h"
#include "Utilities/ReportFault.h"
#include "SystemConfig.h"

#include "semphr.h"

namespace CasualNoises
{

//constexpr uint32_t 	cNoOfPotConvertions		= 4;
constexpr uint32_t 	cPotAverageCount		= 10;				// This will generate new data at 10Hz

ADC_HandleTypeDef* 	gPotentiometer_adc;
volatile uint16_t 	gPotData[NUM_POT_INPUTS];					// Raw ADC data from DMA
uint32_t 			gPotDataRunning[NUM_POT_INPUTS];		// Running average values
uint32_t 			gPotConvCnt 			= 0;
bool	 			gNewPotDataAvailable 	= false;
SemaphoreHandle_t	gPotSemaphoreHandle 	= nullptr;

//==============================================================================
//          PotentiometerConvCpltCallback()
//
// Conversion complete callback
// Should be called at a rate of about 1KHz
//
//  CasualNoises    01/11/2024  First implementation
//==============================================================================
bool PotentiometerConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if ((hadc == gPotentiometer_adc) && ! gNewPotDataAvailable)
	{

		// Update running average
		for (uint32_t i = 0; i < NUM_POT_INPUTS; ++i)
			gPotDataRunning[i] += gPotData[i];
		gPotConvCnt += 1;

		// Potentiometer data available?
		if (gPotConvCnt == cPotAverageCount)
		{
			gPotConvCnt 						= 0;
			gNewPotDataAvailable 				= true;
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			xSemaphoreGiveFromISR(gPotSemaphoreHandle, &xHigherPriorityTaskWoken);
		}

		return true;
	} else
	{
		return false;
	}

}

//==============================================================================
//          potentiometerThread()
//
// Handle all potentiometer events
//
//  CasualNoises    01/11/2024  First implementation
//  CasualNoises    21/07/2025  Using semaphores to synchronise events
//==============================================================================
void potentiometerThread(void* pvParameters)
{
	float    potDataAverage[NUM_POT_INPUTS];			// Average values in %
	float	 prevPotDataAverage[NUM_POT_INPUTS];		// Previous potentiometer values

	// Clear conversion data buffers
	for (uint32_t i = 0; i < NUM_POT_INPUTS; ++i)
	{
		gPotData[i] 			= 0;
		gPotDataRunning[i]		= 0;
		potDataAverage[i]		= 0.0f;
		prevPotDataAverage[i]	= 0.0f;
	}

	// Get arguments
	sPotentiometerThreadData* arguments = (sPotentiometerThreadData*)pvParameters;
	gPotentiometer_adc 				= arguments->hadc;
	TIM_HandleTypeDef htim			= arguments->htim;
	QueueHandle_t clientQueueHandle = arguments->clientQueueHandle;

	// Register a callback for the potentiometer adc
	add_ADC_ConvCpltCallback(PotentiometerConvCpltCallback);

	// Calibrate the ADC converter used and start in DMA mode then start the timer to trigger conversions
	HAL_StatusTypeDef res = HAL_ADCEx_Calibration_Start(gPotentiometer_adc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);
	res = HAL_ADC_Start_DMA(gPotentiometer_adc, (uint32_t *)gPotData, NUM_POT_INPUTS);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);
	HAL_TIM_Base_Start(&htim);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);

	// Create a binary semaphore to awake thread when new data is available
	gPotSemaphoreHandle = xSemaphoreCreateBinary();
	if (gPotSemaphoreHandle == nullptr)
		CN_ReportFault(eErrorCodes::adcThreadError);

	// Loop here awaiting new potentiometer values
	for (;;)
	{

		// Wait for new data
		BaseType_t res = xSemaphoreTake(gPotSemaphoreHandle, portMAX_DELAY);
		if (res != pdTRUE)
			CN_ReportFault(eErrorCodes::adcThreadError);

		// New data available and average difference is larger than 1% -> send message
		for (uint32_t i = 0; i < NUM_POT_INPUTS; ++i)
		{
			gPotDataRunning[i] /= cPotAverageCount;
			potDataAverage[i] = (float)(gPotDataRunning[i] * 100) / 65535.0f;
			float diff = potDataAverage[i] - prevPotDataAverage[i];
			if ((diff > 1.0f) || (diff < -1.0f))
			{
				sPotentiometerEventStruct event;
				event.eventSourceID = eEventSourceID::potentiometerThreadSourceID;
				event.potentiometerId = i;
				event.potentiometerValue = potDataAverage[i];
				if (clientQueueHandle != nullptr)
				{
					BaseType_t res = xQueueSendToBack(clientQueueHandle, (void*)&event, 10);
					if (res != pdPASS)
						CN_ReportFault(eErrorCodes::adcThreadError);
				}
				prevPotDataAverage[i] = potDataAverage[i];
			}
			gPotDataRunning[i] = 0;
		}
		gNewPotDataAvailable = false;

	}

}

//==============================================================================
//          StartPotentiometerThread()
//
// Handle all potentiometer events
//
//  CasualNoises    01/11/2024  First implementation
//==============================================================================
BaseType_t startPotentiometerThread(void *argument, TaskHandle_t* xHandlePtr)
{

	// Create the thread to scan the potentiometers
	BaseType_t res = xTaskCreate(potentiometerThread, "PotentiometerThread", DEFAULT_STACK_SIZE / 2, argument,
			UI_THREAD_PRIORITY,	xHandlePtr);
	return res;

}

} // namespace CasualNoises

#endif
