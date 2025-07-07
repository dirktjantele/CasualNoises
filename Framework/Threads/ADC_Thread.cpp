/*
  ==============================================================================

    ADC_Thread.cpp

    Handles potentiometers events

    Created: 18/04/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_ADC_THREAD

#include "ADC_Thread.h"
#include "Utilities/CallbackHandlers.h"
#include "Utilities/ReportFault.h"
#include "SystemConfig.h"

namespace CasualNoises
{

constexpr uint32_t noOfADC_Convertions	= 4;
volatile uint16_t ADC_Data[noOfADC_Convertions];			// Raw ADC data from DMA

ADC_HandleTypeDef* ADC_adc;

//==============================================================================
//          ADC_ConvCpltCallback()
//
// Conversion complete callback
// Should be called at a rate of about 100KHz
//
//  CasualNoises    18/04/2025  First implementation
//==============================================================================
bool ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc == ADC_adc)
	{
//		setTimeMarker_4();

//		resetTimeMarker_4();
		return true;
	} else
	{
		return false;
	}
}

//==============================================================================
//          ADC_Thread()
//
// Handle all ADC events
//
//  CasualNoises    18/04/2025  First implementation
//==============================================================================
void ADC_Thread(void* pvParameters)
{

	// Get arguments
	sADC_ThreadData* arguments = (sADC_ThreadData*)pvParameters;
	ADC_adc 							= arguments->hadc;
	TIM_HandleTypeDef htim				= arguments->htim;
	QueueHandle_t clientQueueHandle 	= arguments->clientQueueHandle;

	// Register a callback for the potentiometer adc
	add_ADC_ConvCpltCallback(ADC_ConvCpltCallback);

	// Calibrate the ADC converter used and start in DMA mode then start the timer to trigger conversions
	HAL_StatusTypeDef res = HAL_ADCEx_Calibration_Start(ADC_adc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);
	res = HAL_ADC_Start_DMA(ADC_adc, (uint32_t *)ADC_Data, noOfADC_Convertions);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);
	res = HAL_TIM_Base_Start(&htim);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);

	// Loop here awaiting new potentiometer values
	for (;;)
	{

		// ToDo: use a semaphore to wake-up the thread
		// Wait x ticks
		osDelay(10);

	}

}

//==============================================================================
//          startADC_Thread()
//
// Handle all ADC events
//
//  CasualNoises    18/04/2025  First implementation
//==============================================================================
BaseType_t startADC_Thread(void *argument, TaskHandle_t* xHandlePtr)
{

	// Create the thread to scan the ADC convertions
	BaseType_t res = xTaskCreate(ADC_Thread, "ADC_Thread", DEFAULT_STACK_SIZE / 2, argument,
			UI_THREAD_PRIORITY,	xHandlePtr);
	return res;

}

} // namespace CasualNoises

#endif
