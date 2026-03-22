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

	// The component is not owned by the ViwPort
	void setContent ( Component* componentPtr )
	{
		mContentsComponentPtr = componentPtr;
	}

	void applyVerticalScrolling ( int32_t amount );

	void paint ( Graphics& g ) noexcept override;

private:

	Component*		mContentsComponentPtr	{ nullptr };

	int32_t			mOffset					{ 0 };

};

} // namespace CasualNoises

#endif
