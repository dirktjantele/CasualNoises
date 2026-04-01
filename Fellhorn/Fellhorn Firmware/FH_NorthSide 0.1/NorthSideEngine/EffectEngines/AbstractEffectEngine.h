/*
  ==============================================================================

    AbstractEffectEngine.h
    Created: 11/03/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <AudioProcessors/Processors/AudioProcessor.h>
#include <CommonDefinitions.h>

#include <SynthEngineMessage.h>

namespace CasualNoises
{

typedef struct {
	float	_1V_OCT_1;						// 1V/OCT range from 0.0 to 10.0
	float	_1V_OCT_2;						// ...
	float	CV_Inputs [ NUM_CV_INPUTS ];	// Control voltage inputs range from -1.0 to 1.0
} sControlVoltages;

class AbstractEffectEngine : public AudioProcessor
{
public:
	 AbstractEffectEngine() = default;
	~AbstractEffectEngine() = default;

	virtual void applyControlVoltages ( sControlVoltages* voltages ) = 0;
	virtual void applyControlVoltages (  uint32_t noOfEntries, uint16_t* adcDataPtr  ) final;

	virtual void processNerveNetMessage ( tInitMessage* messagePtr ) final;
	virtual void applyPotentiometerValue ( ePotentioMeterId potId, float value ) = 0;

private:

};

} // namespace CasualNoises

