/*
  ==============================================================================

    ADSR.h
    Created: 1 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

class ADSR
{
public:
	 ADSR() = delete;
	~ADSR() = default;

	ADSR(float sampleRate)
	: mSampleRate (sampleRate)
	{};

private:
	float	mSampleRate { 0.0f };

};

} // namespace CasualNoises
