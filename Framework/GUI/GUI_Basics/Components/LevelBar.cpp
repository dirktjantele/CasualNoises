/*
  ==============================================================================

    LevelBar.cpp
    Created: 09/04/2026

    Display a level bar with target, min and max values

    Author:  Dirk Tjantele

  ==============================================================================
*/
#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "LevelBar.h"

#include <Graphics/Contexts/Graphics.h>

namespace CasualNoises
{

//==============================================================================
//          LevelBar() & ~LevelBar()
//
//  CasualNoises    09/04/2026  First implementation
//==============================================================================
LevelBar::LevelBar(String name) :
	Component (name)
{
}

//==============================================================================
//          paint()
//
//  CasualNoises    09/04/2026  First implementation
//==============================================================================
void LevelBar::paint ( Graphics& g ) noexcept
{

	// Draw center line
	SSD1309_Driver* screen = g.getScreen ();
	Rectangle<int32_t> bounds = getLocalBounds ();
	int32_t posY = bounds.getGlobalCentreY ();
	screen->drawLine ( bounds.getX (), posY, bounds.getX () + bounds.getWidth (), posY );

	// Draw target marker
	const sFont* fontPtr = &font_7x10;
	float midX = ( ( 2 * bounds.getX () ) + bounds.getWidth () ) / 2;
	float halfWidth = ( ( float ) bounds.getWidth () ) / 2.0f;
    if ( ( mTargetLevel >= -1.0f ) && ( mTargetLevel <= 1.0f ) )
	{
		int32_t tX = midX + ( halfWidth * mTargetLevel);
		screen->drawCharacter ( tX - 3, posY - 10, 'v', fontPtr );
	}

    // Draw level 1 marker
    if ( ( mLevel_1 >= -1.0f ) && ( mLevel_1 <= 1.0f ) )
	{
		int32_t tX = midX + ( halfWidth * mLevel_1);
		screen->drawCharacter ( tX - 3, posY + 2, '^', fontPtr );
	} else if ( mLevel_1 > 1.0f )
	{
		int32_t tX = midX + halfWidth;
		screen->drawCharacter ( tX - 3, posY + 2, '>', fontPtr );
	} else
	{
		int32_t tX = midX - halfWidth;
		screen->drawCharacter ( tX - 3, posY + 2, '<', fontPtr );
	}

    // Draw level 2 marker
    if ( mLevel_2 != mLevel_1 )
    {
		if ( ( mLevel_2 >= -1.0f ) && ( mLevel_2 <= 1.0f ) )
		{
			int32_t tX = midX + ( halfWidth * mLevel_2);
			screen->drawCharacter ( tX - 3, posY + 2, '^', fontPtr );
		} else if ( mLevel_2 > 1.0f )
		{
			int32_t tX = midX + halfWidth;
			screen->drawCharacter ( tX - 3, posY + 2, '>', fontPtr );
		} else
		{
			int32_t tX = midX - halfWidth;
			screen->drawCharacter ( tX - 3, posY + 2, '<', fontPtr );
		}
    }

}

} // namespace CasualNoises

#endif
