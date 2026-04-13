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

#ifdef ADC_NERVENET_SUPPORT				// ToDo: remove this on Fellhorn rev 2

#include "SynthEngineMessage.h"
#include "NerveNet/NerveNetMasterThread.h"
#include "../../NorthSideEngine/YellowPages.h"

#endif

#include "semphr.h"

namespace CasualNoises
{

// ADC conversion buffers
ADC_HandleTypeDef* 	gADC_adc;
uint16_t 			gADC_Data  [ TOTAL_NUM_CV_INPUTS ];			// Raw ADC data from DMA

// Handler for adc data
ADC_DataHandler*	gADC_DataHandlerPtr		= nullptr;

//#ifdef CASUALNOISES_ADC_NERVENET_SUPPORT

SemaphoreHandle_t	gADC_SemaphoreHandle 	= nullptr;

//#endif

//==============================================================================
//          ADC_ConvCpltCallback()
//
// Conversion complete callback
// Should be called at a rate of about 100KHz
//
//  CasualNoises    18/04/2025  First implementation
//==============================================================================
bool ADC_ConvCpltCallback ( ADC_HandleTypeDef* hadc )
{
	if ( hadc == gADC_adc )
	{

		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		if ( gADC_SemaphoreHandle != nullptr )
			xSemaphoreGiveFromISR( gADC_SemaphoreHandle, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

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
void ADC_Thread ( void* pvParameters )
{

	// Get arguments
	sADC_ThreadData* arguments = (sADC_ThreadData*)pvParameters;
	gADC_adc 							= arguments->hadc;
	gADC_DataHandlerPtr					= arguments->ADC_DataHandlerPtr;

	// Register a callback for the potentiometer adc
	add_ADC_ConvCpltCallback ( ADC_ConvCpltCallback );
	vTaskDelay ( pdMS_TO_TICKS ( 10 ) );

	// Create a binary semaphore to awake thread when new data is available
	gADC_SemaphoreHandle = xSemaphoreCreateBinary();
	if ( gADC_SemaphoreHandle == nullptr )
		CN_ReportFault ( eErrorCodes::adcThreadError );

	// Calibrate the ADC converter used and start in DMA mode then start the timer to trigger conversions
	HAL_StatusTypeDef res = HAL_ADCEx_Calibration_Start ( gADC_adc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED );
	if ( res != HAL_OK )
		CN_ReportFault ( eErrorCodes::adcThreadError );
	res = HAL_ADC_Start_DMA ( gADC_adc, (uint32_t *)gADC_Data, TOTAL_NUM_CV_INPUTS );
	if ( res != HAL_OK )
		CN_ReportFault ( eErrorCodes::adcThreadError );

	// Main event loop
	for (;;)
	{

		// Wait for new data
		BaseType_t res = xSemaphoreTake ( gADC_SemaphoreHandle, portMAX_DELAY );
		if (res != pdTRUE)
			CN_ReportFault(eErrorCodes::adcThreadError);

		// Process incoming ADC data
		if ( gADC_DataHandlerPtr != nullptr )
			gADC_DataHandlerPtr->handle_ADC_Data ( TOTAL_NUM_CV_INPUTS, gADC_Data );

		// Launch new conversion cycle
		res = HAL_ADC_Start_DMA ( gADC_adc, (uint32_t *)gADC_Data, TOTAL_NUM_CV_INPUTS );
		if ( res != HAL_OK )
			CN_ReportFault ( eErrorCodes::adcThreadError );

	}

	// We should never reach this point
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
