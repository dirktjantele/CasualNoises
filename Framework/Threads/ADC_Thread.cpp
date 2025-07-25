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

#include "semphr.h"

namespace CasualNoises
{

constexpr uint32_t 	cNoOfADC_Convertions	= 4;

// ADC conversion buffers
ADC_HandleTypeDef* 	ADC_adc;
volatile uint16_t 	ADC_Data[cNoOfADC_Convertions];			// Raw ADC data from DMA

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
		setTimeMarker_4();

		// Process adc data
		// ToDo implement this

		resetTimeMarker_4();

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
//  CasualNoises    23/07/2025  Using semaphores to synchronise events
//==============================================================================
void ADC_Thread(void* pvParameters)
{

	// Get arguments
	sADC_ThreadData* arguments = (sADC_ThreadData*)pvParameters;
	ADC_adc 							= arguments->hadc;
	TIM_HandleTypeDef* htim				= arguments->htim;

	// Register a callback for the potentiometer adc
	add_ADC_ConvCpltCallback(ADC_ConvCpltCallback);
	vTaskDelay(pdMS_TO_TICKS(10));

	// Calibrate the ADC converter used and start in DMA mode then start the timer to trigger conversions
	HAL_StatusTypeDef res = HAL_ADCEx_Calibration_Start(ADC_adc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);
	res = HAL_ADC_Start_DMA(ADC_adc, (uint32_t *)ADC_Data, cNoOfADC_Convertions);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);
	res = HAL_TIM_Base_Start(htim);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::adcThreadError);

	// Suspend this task
	vTaskSuspend(nullptr);

	// We should never reache this
	CN_ReportFault(eErrorCodes::runtimeError);

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
			CV_IN_THREAD_PRIORITY, xHandlePtr);
	return res;

}

} // namespace CasualNoises

#endif
