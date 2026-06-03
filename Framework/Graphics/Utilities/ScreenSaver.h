/*
  ==============================================================================

    ScreenSaver.h
    Created: 21/04/2026

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

class SSD1309_Driver;
enum class eBitOperations;

class MovingTriangle;

class ScreenSaver final
{
public:
	 ScreenSaver () = delete;
	 ScreenSaver ( SSD1309_Driver* ptr );
	~ScreenSaver ();

	void refresh () const noexcept;

private:
	SSD1309_Driver*		mSSD1309_DriverPtr		{ nullptr };

	MovingTriangle*		mMovingTrianglePtr1		{ nullptr };
	MovingTriangle*		mMovingTrianglePtr2		{ nullptr };

};

class MovingPoint final
{
public:
	 MovingPoint () = delete;
	 MovingPoint ( int minX, int minY, int maxX, int maxY );
	~MovingPoint () = default;

	void 	doMove ( ) noexcept;

	int		getPosX	()		{ return mPosX; };
	int		getPosY	()		{ return mPosY; };

private:
	int				mMinX		{ 0 };
	int				mMinY		{ 0 };
	int				mMaxX		{ 0 };
	int				mMaxY		{ 0 };

	int				mDeltaX		{ 0 };
	int				mDeltaY		{ 0 };

	int				mPosX		{ 0 };
	int				mPosY		{ 0 };

};

class MovingTriangle final
{
public:
	 MovingTriangle () = delete;
	 MovingTriangle ( int minX, int minY, int maxX, int maxY );
	~MovingTriangle () = default;

	void paintTriangle 	( SSD1309_Driver* SSD1309_DriverPtr, eBitOperations op ) noexcept;
	void doMove 		( ) noexcept;

private:
	int				mMinX		{ 0 };
	int				mMinY		{ 0 };
	int				mMaxX		{ 0 };
	int				mMaxY		{ 0 };

	MovingPoint		mPoint1		{ mMinX, mMinY, mMaxX, mMaxY };
	MovingPoint		mPoint2		{ mMinX, mMinY, mMaxX, mMaxY };
	MovingPoint		mPoint3		{ mMinX, mMinY, mMaxX, mMaxY };

};

} // namespace CasualNoises
