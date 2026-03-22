/*
  ==============================================================================

    LabelGroup.h
    Created: 19/03/2026

    Group labels in a list

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include "Component.h"

namespace CasualNoises
{

class Label;

class LabelGroup final : public Component
{
public:
	 LabelGroup () = delete;
	 LabelGroup ( String name );
	~LabelGroup ();

	void addLabel ( Label* label );
	void takeOwnership ( bool flag )	{ mDeleteElements = flag; }

	uint32_t getHeight ();

	virtual void setBounds ( int32_t x, int32_t y, int32_t w, int32_t h ) noexcept override;
	virtual void setBounds ( Rectangle<int32_t>& bounds ) noexcept override
	{
		setBounds ( bounds.getX (), bounds.getY (), bounds.getWidth (), bounds.getHeight() );
	};

	void paint ( Graphics& g ) noexcept override;

private:
	std::vector<Label*> 	mLabelGroup;
	bool 					mDeleteElements		{ true };

	bool					mElementBoundsSet	{ false };
};

} // namespace CasualNoises

#endif
