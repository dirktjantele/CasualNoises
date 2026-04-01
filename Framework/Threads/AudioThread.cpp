/*
  ==============================================================================

    AudioThread.cpp
    Created: 31 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_AUDIO_THREAD

#include "AudioThread.h"

#include "AudioBasics/Buffers/AudioBuffer.h"
#include "AudioUtils/Players/AudioProcessorPlayer.h"

#include "Utilities/ReportFault.h"

namespace CasualNoises
{

//#include "main.h"
//#include "SystemConfig.h"

//#include "CasualNoises.h"

//#ifdef debugging

//==============================================================================
//          AudioThread
//
// Just create an AudioProcessorPlayer and let it do the work
//
//  CasualNoises    10/11/2024  First implementation
//==============================================================================
void AudioThread ( void* pvParameters )
{

	// Create an audio buffer
	static AudioBuffer* audioBufferPtr = new AudioBuffer ();

	// Create an audio player and start it, this call should never return
	sAudioThreadInitData* params = static_cast<sAudioThreadInitData*> ( pvParameters );
	AudioProcessorPlayer* player = AudioProcessorPlayer::getAudioProcessorPlayer ( params->audioProcessorPtr );
	I2S_HandleTypeDef* ptr = static_cast<I2S_HandleTypeDef*> ( params->hi2sHandlePtr );
	player->sethi2sHandlePtr ( ptr );
	void (**nerveNetCallBackPtr)(CasualNoises::sNerveNetData*) = params->nerveNetCallBackPtr;
	player->runAudioProcessor( audioBufferPtr );

	// We should never come here
	CN_ReportFault(eErrorCodes::AudioThreadError);

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

//#endif

} // namespace CasualNoises

#endif
