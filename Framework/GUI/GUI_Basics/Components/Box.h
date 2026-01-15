/*
  ==============================================================================

    Box.h
    Created: 29/12/2025

    Paints a box around other components

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include "Component.h"

namespace CasualNoises
{

class Box : public Component
{
public:
	 	 	  Box () = delete;
	 	 	  Box ( String name );
	 virtual ~Box () {};

	void paint ( Graphics& g ) noexcept override;

	bool handleUI_event ( sIncommingUI_Event* uiEvent, bool altState, Graphics& g ) override { return false; }

private:

};

class IndexBox final : public Box
{
public:
			 IndexBox () = delete;
			 IndexBox ( String name);
			 IndexBox ( String name, uint8_t noOfTabs, uint8_t currentTab );
	virtual ~IndexBox () = default;

	void setCurrentTab ( uint8_t tab ) noexcept { mCurrentTab = tab; };

	void paint(Graphics& g) noexcept override;

private:
	uint8_t		mNoOfTabs		{ 0 };
	uint8_t		mCurrentTab		{ 0 };

};

} // namespace CasualNoises

#endif
