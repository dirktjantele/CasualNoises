/*
  ==============================================================================

    BoxComponent.h
    Created: 19 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#pragma once

#include <GUI/GUI_Basics/Components OBSOLETE/Component.h>

namespace CasualNoises
{

class BoxComponent : public Component
{
public:
	 BoxComponent() = delete;
	~BoxComponent() = default;

	BoxComponent(Component* contents) : mContents(contents) {}

	void paint(Graphics& g) override
	{
		mContents->setBounds(mX, mY, mWidth, mHeight);
		mContents->paint(g);
	};

private:
	Component*		mContents { nullptr };

};

} // namespace CasualNoises

#endif
