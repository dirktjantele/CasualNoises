/*
  ==============================================================================

    AudioThread.cpp
    Created: 31 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

 #ifdef CASUALNOISES_AUDIO_THREAD

#include "AudioThread.h"

#include "main.h"
#include "SystemConfig.h"

#include "CasualNoises.h"

#ifdef OpFour_Hardware							// ToDo: abstract this out
#include "OpFourAudioProcessor.h"
#endif

namespace CasualNoises
{

//==============================================================================
//          AudioThread
//
// Just create an AudioProcessorPlayer and let it do the work
//
//  CasualNoises    10/11/2024  First implementation
//==============================================================================
void AudioThread(void* pvParameters)
{

	// Create an audio buffer
	static AudioBuffer* audioBufferPtr = new AudioBuffer();

	// Create an AudioProcessor according to the hardware platform
	AudioProcessor* audioProcessorPtr = nullptr;
#ifdef OpFour_Hardware
	void* ptr = OpFourAudioProcessor::getOpFourAudioProcessor();
	audioProcessorPtr = dynamic_cast<AudioProcessor*>((AudioProcessor*)ptr);
#endif
	if (audioProcessorPtr == nullptr) CN_ReportFault(1);

	// Create an audio player and start it, this call should never return
	AudioProcessorPlayer* player = AudioProcessorPlayer::getAudioProcessorPlayer(audioProcessorPtr/*, audioBufferPtr*/);
	sAudioThreadInitData* params = (sAudioThreadInitData*) pvParameters;
	player->sethi2sHandlePtr(params->hi2sHandlePtr);
	player->setSynthesiserParamsPtr(params->synthesizerParamsPtr);
	player->runAudioProcessor(/*audioProcessorPtr,*/ audioBufferPtr);

	// We should never come here
	CN_ReportFault(1);

}

//==============================================================================
//          StartAudioThread
//
// Create an audio thread and run it
//
//  CasualNoises    10/11/2024  First implementation
//==============================================================================
BaseType_t StartAudioThread(void *argument)
{

	// Create the thread to run
	TaskHandle_t xHandle;
	BaseType_t res = xTaskCreate(AudioThread, "AudioThread", DEFAULT_STACK_SIZE, argument,
			AUDIO_THREAD_PRIORITY,	&xHandle);
	return res;

}

} // namespace CasualNoises

#endif
