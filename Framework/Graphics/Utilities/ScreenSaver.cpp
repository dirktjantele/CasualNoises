/*
  ==============================================================================

    ScreenSaver.cpph
    Created: 21/04/2026

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_SSD1309_DRIVER

#include "ScreenSaver.h"

#include "Drivers/OLED/SSD1309_Driver.h"

#include "Core/Maths/Random.h"

namespace CasualNoises
{

/****************************** ScreenSaver ******************************/

//==============================================================================
//          ScreenSaver () / ~ScreenSaver ()
//
//  CasualNoises    21/04/2026  First implementation
//==============================================================================
ScreenSaver::ScreenSaver ( SSD1309_Driver* ptr ) :
	mSSD1309_DriverPtr ( ptr )
{

	mMovingTrianglePtr1 = new MovingTriangle (
			0, 0,
			mSSD1309_DriverPtr->getDisplayWidth (),
			mSSD1309_DriverPtr->getDisplayHeight () );
	mMovingTrianglePtr2 = new MovingTriangle (
			0, 0,
			mSSD1309_DriverPtr->getDisplayWidth (),
			mSSD1309_DriverPtr->getDisplayHeight () );
}

ScreenSaver::~ScreenSaver ()
{
	if ( mMovingTrianglePtr1 != nullptr )	delete mMovingTrianglePtr1;
	if ( mMovingTrianglePtr2 != nullptr )	delete mMovingTrianglePtr2;
}

//==============================================================================
//          refresh ()
//
//  CasualNoises    21/04/2026  First implementation
//==============================================================================
void ScreenSaver::refresh () const noexcept
{
	mMovingTrianglePtr1->paintTriangle ( mSSD1309_DriverPtr, eBitOperations::ClearBitOp );
	mMovingTrianglePtr1->doMove ();
	mMovingTrianglePtr1->paintTriangle ( mSSD1309_DriverPtr, eBitOperations::SetBitOp );
	mMovingTrianglePtr2->paintTriangle ( mSSD1309_DriverPtr, eBitOperations::ClearBitOp );
	mMovingTrianglePtr2->doMove ();
	mMovingTrianglePtr2->paintTriangle ( mSSD1309_DriverPtr, eBitOperations::SetBitOp );
	mSSD1309_DriverPtr->refreshDisplay( true );
}

/****************************** MovingPoint ******************************/

//==============================================================================
//          MovingPoint ()
//
//  CasualNoises    21/04/2026  First implementation
//==============================================================================
MovingPoint::MovingPoint ( int minX, int minY, int maxX, int maxY ) :
		mMinX ( minX ), mMinY ( minY ), mMaxX ( maxX ), mMaxY ( maxY )
{
	mDeltaX = getRandomNumber ( -4, 4 );
	mDeltaY = getRandomNumber ( -4, 4 );
	mPosX   = getRandomNumber ( mMinX, mMaxX );
	mPosY   = getRandomNumber ( mMinY, mMaxY );
}

//==============================================================================
//          doMove ()
//
//  CasualNoises    21/04/2026  First implementation
//==============================================================================
void MovingPoint::doMove () noexcept
{
	mPosX += mDeltaX;
	if ( mPosX < mMinX )
	{
		mDeltaX *= -1.0f;
		mPosX = mMinX;
	} else if  ( mPosX > mMaxX )
	{
		mDeltaX *= -1.0f;
		mPosX = mMaxX;
	}
	mPosY += mDeltaY;
	if ( mPosY < mMinY )
	{
		mDeltaY *= -1.0f;
		mPosY = mMinY;
	} else if  ( mPosY > mMaxY )
	{
		mDeltaY *= -1.0f;
		mPosY = mMaxY;
	}
}

/****************************** MovingTriangle ******************************/

//==============================================================================
//          MovingTriangle ()
//
//  CasualNoises    21/04/2026  First implementation
//==============================================================================
MovingTriangle::MovingTriangle ( int minX, int minY, int maxX, int maxY ) :
		mMinX ( minX ), mMinY ( minY ), mMaxX ( maxX ), mMaxY ( maxY )
{
}

//==============================================================================
//          paintTriangle ()
//
//  CasualNoises    21/04/2026  First implementation
//==============================================================================
void MovingTriangle::paintTriangle ( SSD1309_Driver* SSD1309_DriverPtr, eBitOperations op ) noexcept
{
	SSD1309_DriverPtr->drawLine( mPoint1.getPosX (), mPoint1.getPosY (),
								 mPoint2.getPosX (), mPoint2.getPosY (), op);
	SSD1309_DriverPtr->drawLine( mPoint2.getPosX (), mPoint2.getPosY (),
								 mPoint3.getPosX (), mPoint3.getPosY (), op);
	SSD1309_DriverPtr->drawLine( mPoint3.getPosX (), mPoint3.getPosY (),
								 mPoint1.getPosX (), mPoint1.getPosY (), op);
}

//==============================================================================
//          doMove ()
//
//  CasualNoises    21/04/2026  First implementation
//==============================================================================
void MovingTriangle::doMove () noexcept
{
	mPoint1.doMove ();
	mPoint2.doMove ();
	mPoint3.doMove ();
}

} // namespace CasualNoises

#endif
