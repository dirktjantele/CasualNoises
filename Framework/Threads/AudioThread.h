/*
  ==============================================================================

    AudioThread.h
    Created: 31 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_AUDIO_THREAD

#pragma once

#include "main.h"

#include "AudioProcessors/Processors/AudioProcessor.h"
#include "../NerveNet/NerveNetMessage.h"

namespace CasualNoises
{

typedef struct
{
	AudioProcessor*		audioProcessorPtr;
	I2S_HandleTypeDef*  hi2sHandlePtr;
	void (**nerveNetCallBackPtr)(CasualNoises::sNerveNetData*);
//	void*				synthesizerParamsPtr;
} sAudioThreadInitData;

BaseType_t StartAudioThread(void *argument);

} // namespace CasualNoises

#endif
