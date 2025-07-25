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
	 ADSR ( const ADSR& ) = default;
	 ADSR ( ADSR&& ) = default;
	~ADSR() = default;

	//==============================================================================
	//          ADSR()
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	ADSR(float sampleRate)
	: mSampleRate (sampleRate)
	{};

private:
	float	mSampleRate { 0.0f };

};

} // namespace CasualNoises
