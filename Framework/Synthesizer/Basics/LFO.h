/*
  ==============================================================================

    LFO.h
    Created: 1 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

class LFO
{
public:
	 LFO() = delete;
	~LFO() = default;

	LFO(float sampleRate)
	: mSampleRate (sampleRate)
	{};

private:
	float	mSampleRate { 0.0f };

};

} // namespace CasualNoises



