/*
  ==============================================================================

    PotentiometerThread.cpp

    Handles potentiometers events

    Created: 25 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "PotentiometerThread.h"

#include "SystemConfig.h"

namespace CasualNoises
{

//==============================================================================
//          PotentiometerConvCpltCallback
//
// Handle adc conversion complete call-backs for potentiometer adc's
//
//  CasualNoises    09/11/2024  First implementation
//==============================================================================

constexpr uint32_t noOfPotConvertions	= 3;
constexpr uint32_t potAverageCount		= 10;			// This will generate new data at 10Hz

ADC_HandleTypeDef* Potentiometer_adc;
volatile uint16_t potData[noOfPotConvertions];			// Raw ADC data from DMA
uint32_t potDataRunning[noOfPotConvertions];			// Running average values
uint32_t potConvCnt = 0;
bool	 newPotDataAvailable = false;

// Should be called at a rate of about 100Hz
void PotentiometerConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if ((hadc == Potentiometer_adc) && ! newPotDataAvailable)
	{
//		toggleMarker_1();		// ToDo remove this

		// Update running average
		for (uint32_t i = 0; i < noOfPotConvertions; ++i)
			potDataRunning[i] += potData[i];
		potConvCnt += 1;

		// Potentiometer data available?
		if (potConvCnt == potAverageCount)
		{
			potConvCnt = 0;
			newPotDataAvailable = true;
		}

	}
}

//==============================================================================
//          potentiometerThread
//
// Handle all potentiometer events
//
//  CasualNoises    01/11/2024  First implementation
//==============================================================================
void potentiometerThread(void* pvParameters)
{
	float    potDataAverage[noOfPotConvertions];			// Average values in %
	float	 prevPotDataAverage[noOfPotConvertions];		// Previous potentiometer values

	// Clear conversion data buffers
	for (uint32_t i = 0; i < noOfPotConvertions; ++i)
	{
		potData[i] 				= 0;
		potDataRunning[i]		= 0;
		potDataAverage[i]		= 0.0f;
		prevPotDataAverage[i]	= 0.0f;
	}

	// Get arguments
	sPotentiometerThreadData* arguments = (sPotentiometerThreadData*)pvParameters;
	Potentiometer_adc 				= arguments->hadc;
	TIM_HandleTypeDef htim			= arguments->htim;
	QueueHandle_t clientQueueHandle = arguments->clientQueueHandle;

	// Register a callback for the potentiometer adc
	add_ADC_ConvCpltCallback(PotentiometerConvCpltCallback);

	// Calibrate the ADC converter used and start in DMA mode then start the timer to trigger conversions
	HAL_StatusTypeDef res = HAL_ADCEx_Calibration_Start(Potentiometer_adc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	if (res != HAL_OK) CN_ReportFault(1);
	res = HAL_ADC_Start_DMA(Potentiometer_adc, (uint32_t *)potData, noOfPotConvertions);
	if (res != HAL_OK) CN_ReportFault(1);
	HAL_TIM_Base_Start(&htim);
	if (res != HAL_OK) CN_ReportFault(1);

	// Loop here awaiting new potentiometer values
	for (;;)
	{

		// New data available and average difference is larger than 1% -> send message
		if (newPotDataAvailable)
		{
			for (uint32_t i = 0; i < noOfPotConvertions; ++i)
			{
				potDataRunning[i] /= potAverageCount;
				potDataAverage[i] = (float)(potDataRunning[i] * 100) / 65535.0f;
				float diff = potDataAverage[i] - prevPotDataAverage[i];
				if ((diff > 1.0f) || (diff < -1.0f))
				{
					sPotentiometerEventStruct event;
					event.potentiometerId = i;
					event.potentiometerValue = potDataAverage[i];
					BaseType_t res = xQueueSendToBack(clientQueueHandle, (void*)&event, 10);
					if (res != pdPASS) CN_ReportFault(4);
					prevPotDataAverage[i] = potDataAverage[i];
				}
				potDataRunning[i] = 0;
			}

			newPotDataAvailable = false;
		}

		// Wait x ticks
		osDelay(10);

	}

}

//==============================================================================
//          StartPotentiometerThread
//
// Handle all potentiometer events
//
//  CasualNoises    01/11/2024  First implementation
//==============================================================================
BaseType_t startPotentiometerThread(void *argument)
{

	// Create the thread to scan the potentiometers
	TaskHandle_t xHandle;
	BaseType_t res = xTaskCreate(potentiometerThread,	"PotentiometerThread", DEFAULT_STACK_SIZE / 2, argument,
			UI_THREAD_PRIORITY,	&xHandle);
	return res;

}

} // namespace CasualNoises

