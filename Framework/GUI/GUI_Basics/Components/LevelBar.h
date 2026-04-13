/*
  ==============================================================================

    LevelBar.h
    Created: 09/04/2026

    Display a level bar with target, min and max values

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include "Component.h"

namespace CasualNoises
{

class LevelBar final : public Component
{
public:
	 	 	 LevelBar() = delete;
	 	 	 LevelBar(String name);
	virtual ~LevelBar() = default;

	void paint(Graphics& g) noexcept;

	void setlevels ( float target, float l1 = 2.0f, float l2 = 2.0f )
	{
		mTargetLevel	= target;
		mLevel_1		= l1;
		mLevel_2		= l2;
	}

private:

	float		mTargetLevel		{  0.0f };
	float		mLevel_1			{ -1.0f };
	float		mLevel_2			{  1.0f  };

};

} // namespace CasualNoises

#endif
