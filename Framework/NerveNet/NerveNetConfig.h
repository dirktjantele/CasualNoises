/*
  ==============================================================================

    NerveNetConfig.h

	NerveNet related configuration settings

    Created: 09/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NERVENET_THREAD

#pragma once

#include "queue.h"

namespace CasualNoises
{

class AudioProcessor;

// Arguments expected when starting the thread
typedef struct
{
	uint16_t				NerveNetThreadNo;
	GPIO_TypeDef*			NerveNet_REQ_Port;
	uint16_t				NerveNet_REQ_Pin;
	GPIO_TypeDef*			NerveNet_ACK_Port;
	uint16_t				NerveNet_ACK_Pin;
	GPIO_TypeDef*			NerveNet_RESET_Port;
	uint16_t				NerveNet_RESET_Pin;
	SPI_HandleTypeDef*		NerveNet_SPI_Ptr;
	QueueHandle_t*			NerveNetThreadQueue;
	bool*					NerveNetRunningFlagPtr;		// Optional pointer to a flag to be set if the thread is running
	AudioProcessor*			AudioProcessorPtr;			// Optional pointer to the AudioProcessor to be used
	TIM_HandleTypeDef*		PerformanceTestTimerPtr;	// Optional pointer to a timer to measure performance
	uint32_t*				PerformanceResultPtr;		// Optional pointer to store the performance mesurment
} sNerveNetThreadData;

} // namespace CasualNoises

#endif
