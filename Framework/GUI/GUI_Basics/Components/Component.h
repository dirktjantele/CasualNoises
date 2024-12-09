/*
  ==============================================================================

    Component.h
    Created: 18 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#pragma once

#include <vector>

#include "../../../Graphics/Contexts/GraphicsContext.h"

namespace CasualNoises
{

class Component
{
public:
	 Component() = default;
	~Component() = default;

	virtual void	repaint() 		{ mNeedsRepainting = true; }

	virtual void	paint(Graphics& g);

	void 	addChildComponent (Component* child);
	void	setBounds (int x, int y, int width, int height)
	{
		mX = x; mY = y; mWidth = width; mHeight = height;
	}

protected:
	int		mX 		{ 0 };
	int		mY 		{ 0 };
	int		mWidth 	{ 0 };
	int		mHeight	{ 0 };

private:
	bool			mNeedsRepainting { false };

	std::vector<Component*>		mChildren;

};

} // namespace CasualNoises

#endif
