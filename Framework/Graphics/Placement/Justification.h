/*
  ==============================================================================

    Justification.h
    Created: 04/01/2026

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

enum class eJustificationFlags {
  left = 1 , right = 2 , horizontallyCentred = 4 , top = 8 ,
  bottom = 16 , verticallyCentred = 32 , horizontallyJustified = 64 , centred = 36 ,
  centredLeft = 33 , centredRight = 34 , centredTop = 12 , centredBottom = 20 ,
  topLeft = 9 , topRight = 10 , bottomLeft = 17 , bottomRight = 18
};

class Justification final
{
public:
	 Justification() = default;
	 Justification(eJustificationFlags flags)	{ mFlags = flags; };
	~Justification() = default;

	void setJustificationType (eJustificationFlags flags)
	{
		mFlags = flags;
	}

	void setJustificationType (eJustificationFlags flags1, eJustificationFlags flags2)
	{
		mFlags = (eJustificationFlags)((uint32_t)flags1 | (uint32_t)flags2);
	}

	eJustificationFlags getJustificationType()
	{
		return mFlags;
	}

private:
	eJustificationFlags mFlags;

};

} // namespace CasualNoises
