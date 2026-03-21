/*
  ==============================================================================

    ViewPort.h
    Created: 17/03/2026

    Holds a single component and allow for vertical scrolling when the component is
      higher than the given bounds

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include "Component.h"

namespace CasualNoises
{

class ViewPort : public Component
{
public:
			 ViewPort () = delete;
			 ViewPort ( String name );
	virtual ~ViewPort () {};

	// The component is not  owned by the ViwPort
	void setContent ( Component* componentPtr )
	{
		mContentsComponentPtr = componentPtr;
	}

	void paint ( Graphics& g ) noexcept override;

	bool handleUI_event ( sIncommingUI_Event* uiEvent,
						  bool altState, Graphics& g ) override
		{ return false; }

private:

	Component*		mContentsComponentPtr				{ nullptr };

};

} // namespace CasualNoises

#endif
