/*
  ==============================================================================

    Rectangle.h
    Created: 30 mar 2024
    Author:  Dirk Tjantele

    Used Juce as source for this stuff!!

  ==============================================================================
*/

/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

template <typename ValueType>
class Rectangle
{
public:

    /** Creates a rectangle of zero size.
        The default coordinates will be (0, 0, 0, 0).
    */
    Rectangle() = default;

    /** Creates a copy of another rectangle. */
    Rectangle (const Rectangle&) = default;

    /** Creates a rectangle with a given position and size. */
    Rectangle (ValueType initialX, ValueType initialY,
               ValueType width, ValueType height) noexcept
      : pos (initialX, initialY),
        w (width), h (height)
    {
    }

    /** Creates a rectangle with a given size, and a position of (0, 0). */
    Rectangle (ValueType width, ValueType height) noexcept
      : w (width), h (height)
    {
    }

    /** Creates a Rectangle from the positions of two opposite corners. */
    Rectangle (Point<ValueType> corner1, Point<ValueType> corner2) noexcept
      : pos (jmin (corner1.x, corner2.x),
             jmin (corner1.y, corner2.y)),
        w (corner1.x - corner2.x),
        h (corner1.y - corner2.y)
    {
        if (w < ValueType()) w = -w;
        if (h < ValueType()) h = -h;
    }

    /** Creates a Rectangle from a set of left, right, top, bottom coordinates.
        The right and bottom values must be larger than the left and top ones, or the resulting
        rectangle will have a negative size.
    */
    static Rectangle leftTopRightBottom (ValueType left, ValueType top,
                                         ValueType right, ValueType bottom) noexcept
    {
        return { left, top, right - left, bottom - top };
    }

    /** Creates a copy of another rectangle. */
    Rectangle& operator= (const Rectangle&) = default;

    /** Destructor. */
    ~Rectangle() = default;

    /** Returns true if the rectangle's width or height are zero or less */
    bool isEmpty() const noexcept                                   { return w <= ValueType() || h <= ValueType(); }

    /** Returns true if the rectangle's values are all finite numbers, i.e. not NaN or infinity. */
    inline bool isFinite() const noexcept                           { return pos.isFinite() && juce_isfinite (w) && juce_isfinite (h); }

    /** Returns the x coordinate of the rectangle's left-hand-side. */
    inline ValueType getX() const noexcept                          { return pos.x; }

    /** Returns the y coordinate of the rectangle's top edge. */
    inline ValueType getY() const noexcept                          { return pos.y; }

    /** Returns the width of the rectangle. */
    inline ValueType getWidth() const noexcept                      { return w; }

    /** Returns the height of the rectangle. */
    inline ValueType getHeight() const noexcept                     { return h; }

    /** Returns the x coordinate of the rectangle's right-hand-side. */
    inline ValueType getRight() const noexcept                      { return pos.x + w; }

    /** Returns the y coordinate of the rectangle's bottom edge. */
    inline ValueType getBottom() const noexcept                     { return pos.y + h; }

    /** Returns the x coordinate of the rectangle's centre. */
    ValueType getCentreX() const noexcept                           { return pos.x + w / (ValueType) 2; }

    /** Returns the y coordinate of the rectangle's centre. */
    ValueType getCentreY() const noexcept                           { return pos.y + h / (ValueType) 2; }

    /** Returns the centre point of the rectangle. */
    Point<ValueType> getCentre() const noexcept                     { return { pos.x + w / (ValueType) 2,
                                                                               pos.y + h / (ValueType) 2 }; }

    /** Returns the aspect ratio of the rectangle's width / height.
        If widthOverHeight is true, it returns width / height; if widthOverHeight is false,
        it returns height / width. */
    ValueType getAspectRatio (bool widthOverHeight = true) const noexcept                           { return widthOverHeight ? w / h : h / w; }

    /** Returns the rectangle's top-left position as a Point. */
    inline Point<ValueType> getPosition() const noexcept                                            { return pos; }

    /** Changes the position of the rectangle's top-left corner (leaving its size unchanged). */
    inline void setPosition (Point<ValueType> newPos) noexcept                                      { pos = newPos; }

    /** Changes the position of the rectangle's top-left corner (leaving its size unchanged). */
    inline void setPosition (ValueType newX, ValueType newY) noexcept                               { pos.setXY (newX, newY); }

    /** Returns the rectangle's top-left position as a Point. */
    Point<ValueType> getTopLeft() const noexcept                                                    { return pos; }

    /** Returns the rectangle's top-right position as a Point. */
    Point<ValueType> getTopRight() const noexcept                                                   { return { pos.x + w, pos.y }; }

    /** Returns the rectangle's bottom-left position as a Point. */
    Point<ValueType> getBottomLeft() const noexcept                                                 { return { pos.x, pos.y + h }; }

    /** Returns the rectangle's bottom-right position as a Point. */
    Point<ValueType> getBottomRight() const noexcept                                                { return { pos.x + w, pos.y + h }; }

    /** Returns the rectangle's left and right positions as a Range. */
    Range<ValueType> getHorizontalRange() const noexcept                                            { return Range<ValueType>::withStartAndLength (pos.x, w); }

    /** Returns the rectangle's top and bottom positions as a Range. */
    Range<ValueType> getVerticalRange() const noexcept                                              { return Range<ValueType>::withStartAndLength (pos.y, h); }

    /** Changes the rectangle's size, leaving the position of its top-left corner unchanged. */
    void setSize (ValueType newWidth, ValueType newHeight) noexcept                                 { w = newWidth; h = newHeight; }

    /** Changes all the rectangle's coordinates. */
    void setBounds (ValueType newX, ValueType newY,
                    ValueType newWidth, ValueType newHeight) noexcept                               { pos.x = newX; pos.y = newY; w = newWidth; h = newHeight; }

    /** Changes the rectangle's X coordinate */
    inline void setX (ValueType newX) noexcept                                                      { pos.x = newX; }

    /** Changes the rectangle's Y coordinate */
    inline void setY (ValueType newY) noexcept                                                      { pos.y = newY; }

    /** Changes the rectangle's width */
    inline void setWidth (ValueType newWidth) noexcept                                              { w = newWidth; }

    /** Changes the rectangle's height */
    inline void setHeight (ValueType newHeight) noexcept                                            { h = newHeight; }

    /** Changes the position of the rectangle's centre (leaving its size unchanged). */
    inline void setCentre (ValueType newCentreX, ValueType newCentreY) noexcept                     { pos.x = newCentreX - w / (ValueType) 2;
                                                                                                      pos.y = newCentreY - h / (ValueType) 2; }

    /** Changes the position of the rectangle's centre (leaving its size unchanged). */
    inline void setCentre (Point<ValueType> newCentre) noexcept                                     { setCentre (newCentre.x, newCentre.y); }

    /** Changes the position of the rectangle's left and right edges. */
    void setHorizontalRange (Range<ValueType> range) noexcept                                       { pos.x = range.getStart(); w = range.getLength(); }

    /** Changes the position of the rectangle's top and bottom edges. */
    void setVerticalRange (Range<ValueType> range) noexcept                                         { pos.y = range.getStart(); h = range.getLength(); }

    /** Moves the x position, adjusting the width so that the right-hand edge remains in the same place.
        If the x is moved to be on the right of the current right-hand edge, the width will be set to zero.
        @see withLeft
    */
    void setLeft (ValueType newLeft) noexcept                   { w = jmax (ValueType(), pos.x + w - newLeft); pos.x = newLeft; }

    /** Returns a new rectangle with a different x position, but the same right-hand edge as this one.
        If the new x is beyond the right of the current right-hand edge, the width will be set to zero.
        @see setLeft
    */
    [[nodiscard]] Rectangle withLeft (ValueType newLeft) const noexcept       { return { newLeft, pos.y, jmax (ValueType(), pos.x + w - newLeft), h }; }

    /** Moves the y position, adjusting the height so that the bottom edge remains in the same place.
        If the y is moved to be below the current bottom edge, the height will be set to zero.
        @see withTop
    */
    void setTop (ValueType newTop) noexcept                     { h = jmax (ValueType(), pos.y + h - newTop); pos.y = newTop; }

    /** Returns a new rectangle with a different y position, but the same bottom edge as this one.
        If the new y is beyond the bottom of the current rectangle, the height will be set to zero.
        @see setTop
    */
    [[nodiscard]] Rectangle withTop (ValueType newTop) const noexcept         { return { pos.x, newTop, w, jmax (ValueType(), pos.y + h - newTop) }; }

    /** Adjusts the width so that the right-hand edge of the rectangle has this new value.
        If the new right is below the current X value, the X will be pushed down to match it.
        @see getRight, withRight
    */
    void setRight (ValueType newRight) noexcept                 { pos.x = jmin (pos.x, newRight); w = newRight - pos.x; }

    /** Returns a new rectangle with a different right-hand edge position, but the same left-hand edge as this one.
        If the new right edge is below the current left-hand edge, the width will be set to zero.
        @see setRight
    */
    [[nodiscard]] Rectangle withRight (ValueType newRight) const noexcept     { return { jmin (pos.x, newRight), pos.y, jmax (ValueType(), newRight - pos.x), h }; }

    /** Adjusts the height so that the bottom edge of the rectangle has this new value.
        If the new bottom is lower than the current Y value, the Y will be pushed down to match it.
        @see getBottom, withBottom
    */
    void setBottom (ValueType newBottom) noexcept               { pos.y = jmin (pos.y, newBottom); h = newBottom - pos.y; }

    /** Returns a new rectangle with a different bottom edge position, but the same top edge as this one.
        If the new y is beyond the bottom of the current rectangle, the height will be set to zero.
        @see setBottom
    */
    [[nodiscard]] Rectangle withBottom (ValueType newBottom) const noexcept   { return { pos.x, jmin (pos.y, newBottom), w, jmax (ValueType(), newBottom - pos.y) }; }

    /** Returns a version of this rectangle with the given amount removed from its left-hand edge. */
    [[nodiscard]] Rectangle withTrimmedLeft (ValueType amountToRemove) const noexcept     { return withLeft (pos.x + amountToRemove); }

    /** Returns a version of this rectangle with the given amount removed from its right-hand edge. */
    [[nodiscard]] Rectangle withTrimmedRight (ValueType amountToRemove) const noexcept    { return withWidth (w - amountToRemove); }

    /** Returns a version of this rectangle with the given amount removed from its top edge. */
    [[nodiscard]] Rectangle withTrimmedTop (ValueType amountToRemove) const noexcept      { return withTop (pos.y + amountToRemove); }

    /** Returns a version of this rectangle with the given amount removed from its bottom edge. */
    [[nodiscard]] Rectangle withTrimmedBottom (ValueType amountToRemove) const noexcept   { return withHeight (h - amountToRemove); }

    /** Moves the rectangle's position by adding amount to its x and y coordinates. */
     void translate (ValueType deltaX,
                     ValueType deltaY) noexcept
     {
         pos.x += deltaX;
         pos.y += deltaY;
     }

     /** Returns a rectangle which is the same as this one moved by a given amount. */
     [[nodiscard]] Rectangle translated (ValueType deltaX,
                                         ValueType deltaY) const noexcept
     {
         return { pos.x + deltaX, pos.y + deltaY, w, h };
     }

     /** Returns a rectangle which is the same as this one moved by a given amount. */
     Rectangle operator+ (Point<ValueType> deltaPosition) const noexcept
     {
         return { pos.x + deltaPosition.x, pos.y + deltaPosition.y, w, h };
     }

     /** Moves this rectangle by a given amount. */
     Rectangle& operator+= (Point<ValueType> deltaPosition) noexcept
     {
         pos += deltaPosition;
         return *this;
     }

     /** Returns a rectangle which is the same as this one moved by a given amount. */
     Rectangle operator- (Point<ValueType> deltaPosition) const noexcept
     {
         return { pos.x - deltaPosition.x, pos.y - deltaPosition.y, w, h };
     }

     /** Moves this rectangle by a given amount. */
     Rectangle& operator-= (Point<ValueType> deltaPosition) noexcept
     {
         pos -= deltaPosition;
         return *this;
     }

     /** Returns a rectangle that has been scaled by the given amount, centred around the origin.
         Note that if the rectangle has int coordinates and it's scaled by a
         floating-point amount, then the result will be converted back to integer
         coordinates using getSmallestIntegerContainer().
     */
     template <typename FloatType>
     Rectangle operator* (FloatType scaleFactor) const noexcept
     {
         Rectangle r (*this);
         r *= scaleFactor;
         return r;
     }

     /** Scales this rectangle by the given amount, centred around the origin.
         Note that if the rectangle has int coordinates and it's scaled by a
         floating-point amount, then the result will be converted back to integer
         coordinates using getSmallestIntegerContainer().
     */
     template <typename FloatType>
     Rectangle operator*= (FloatType scaleFactor) noexcept
     {
         Rectangle<FloatType> ((FloatType) pos.x * scaleFactor,
                               (FloatType) pos.y * scaleFactor,
                               (FloatType) w * scaleFactor,
                               (FloatType) h * scaleFactor).copyWithRounding (*this);
         return *this;
     }

     /** Scales this rectangle by the given X and Y factors, centred around the origin.
         Note that if the rectangle has int coordinates and it's scaled by a
         floating-point amount, then the result will be converted back to integer
         coordinates using getSmallestIntegerContainer().
     */
     template <typename FloatType>
     Rectangle operator*= (Point<FloatType> scaleFactor) noexcept
     {
         Rectangle<FloatType> ((FloatType) pos.x * scaleFactor.x,
                               (FloatType) pos.y * scaleFactor.y,
                               (FloatType) w * scaleFactor.x,
                               (FloatType) h * scaleFactor.y).copyWithRounding (*this);
         return *this;
     }

     /** Scales this rectangle by the given amount, centred around the origin. */
     template <typename FloatType>
     Rectangle operator/ (FloatType scaleFactor) const noexcept
     {
         Rectangle r (*this);
         r /= scaleFactor;
         return r;
     }

     /** Scales this rectangle by the given amount, centred around the origin. */
     template <typename FloatType>
     Rectangle operator/= (FloatType scaleFactor) noexcept
     {
         Rectangle<FloatType> ((FloatType) pos.x / scaleFactor,
                               (FloatType) pos.y / scaleFactor,
                               (FloatType) w / scaleFactor,
                               (FloatType) h / scaleFactor).copyWithRounding (*this);
         return *this;
     }

     /** Scales this rectangle by the given X and Y factors, centred around the origin. */
     template <typename FloatType>
     Rectangle operator/= (Point<FloatType> scaleFactor) noexcept
     {
         Rectangle<FloatType> ((FloatType) pos.x / scaleFactor.x,
                               (FloatType) pos.y / scaleFactor.y,
                               (FloatType) w / scaleFactor.x,
                               (FloatType) h / scaleFactor.y).copyWithRounding (*this);
         return *this;
     }

     /** Expands the rectangle by a given amount.

         Effectively, its new size is (x - deltaX, y - deltaY, w + deltaX * 2, h + deltaY * 2).
         @see expanded, reduce, reduced
     */
     void expand (ValueType deltaX,
                  ValueType deltaY) noexcept
     {
         auto nw = jmax (ValueType(), w + deltaX * 2);
         auto nh = jmax (ValueType(), h + deltaY * 2);
         setBounds (pos.x - deltaX, pos.y - deltaY, nw, nh);
     }

     /** Returns a rectangle that is larger than this one by a given amount.

         Effectively, the rectangle returned is (x - deltaX, y - deltaY, w + deltaX * 2, h + deltaY * 2).
         @see expand, reduce, reduced
     */
     [[nodiscard]] Rectangle expanded (ValueType deltaX,
                                       ValueType deltaY) const noexcept
     {
         auto nw = jmax (ValueType(), w + deltaX * 2);
         auto nh = jmax (ValueType(), h + deltaY * 2);
         return { pos.x - deltaX, pos.y - deltaY, nw, nh };
     }

     /** Returns a rectangle that is larger than this one by a given amount.

         Effectively, the rectangle returned is (x - delta, y - delta, w + delta * 2, h + delta * 2).
         @see expand, reduce, reduced
     */
     [[nodiscard]] Rectangle expanded (ValueType delta) const noexcept
     {
         return expanded (delta, delta);
     }

     /** Shrinks the rectangle by a given amount.

         Effectively, its new size is (x + deltaX, y + deltaY, w - deltaX * 2, h - deltaY * 2).
         @see reduced, expand, expanded
     */
     void reduce (ValueType deltaX,
                  ValueType deltaY) noexcept
     {
         expand (-deltaX, -deltaY);
     }

     /** Returns a rectangle that is smaller than this one by a given amount.

         Effectively, the rectangle returned is (x + deltaX, y + deltaY, w - deltaX * 2, h - deltaY * 2).
         @see reduce, expand, expanded
     */
     [[nodiscard]] Rectangle reduced (ValueType deltaX,
                                      ValueType deltaY) const noexcept
     {
         return expanded (-deltaX, -deltaY);
     }

     /** Returns a rectangle that is smaller than this one by a given amount.

         Effectively, the rectangle returned is (x + delta, y + delta, w - delta * 2, h - delta * 2).
         @see reduce, expand, expanded
     */
     [[nodiscard]] Rectangle reduced (ValueType delta) const noexcept
     {
         return reduced (delta, delta);
     }

     /** Removes a strip from the top of this rectangle, reducing this rectangle
         by the specified amount and returning the section that was removed.

         E.g. if this rectangle is (100, 100, 300, 300) and amountToRemove is 50, this will
         return (100, 100, 300, 50) and leave this rectangle as (100, 150, 300, 250).

         If amountToRemove is greater than the height of this rectangle, it'll be clipped to
         that value.
     */
     Rectangle removeFromTop (ValueType amountToRemove) noexcept
     {
         const Rectangle r (pos.x, pos.y, w, jmin (amountToRemove, h));
         pos.y += r.h; h -= r.h;
         return r;
     }

     /** Removes a strip from the left-hand edge of this rectangle, reducing this rectangle
         by the specified amount and returning the section that was removed.

         E.g. if this rectangle is (100, 100, 300, 300) and amountToRemove is 50, this will
         return (100, 100, 50, 300) and leave this rectangle as (150, 100, 250, 300).

         If amountToRemove is greater than the width of this rectangle, it'll be clipped to
         that value.
     */
     Rectangle removeFromLeft (ValueType amountToRemove) noexcept
     {
         const Rectangle r (pos.x, pos.y, jmin (amountToRemove, w), h);
         pos.x += r.w; w -= r.w;
         return r;
     }

     /** Removes a strip from the right-hand edge of this rectangle, reducing this rectangle
         by the specified amount and returning the section that was removed.

         E.g. if this rectangle is (100, 100, 300, 300) and amountToRemove is 50, this will
         return (350, 100, 50, 300) and leave this rectangle as (100, 100, 250, 300).

         If amountToRemove is greater than the width of this rectangle, it'll be clipped to
         that value.
     */
     Rectangle removeFromRight (ValueType amountToRemove) noexcept
     {
         amountToRemove = jmin (amountToRemove, w);
         const Rectangle r (pos.x + w - amountToRemove, pos.y, amountToRemove, h);
         w -= amountToRemove;
         return r;
     }

     /** Removes a strip from the bottom of this rectangle, reducing this rectangle
         by the specified amount and returning the section that was removed.

         E.g. if this rectangle is (100, 100, 300, 300) and amountToRemove is 50, this will
         return (100, 350, 300, 50) and leave this rectangle as (100, 100, 300, 250).

         If amountToRemove is greater than the height of this rectangle, it'll be clipped to
         that value.
     */
     Rectangle removeFromBottom (ValueType amountToRemove) noexcept
     {
         amountToRemove = jmin (amountToRemove, h);
         const Rectangle r (pos.x, pos.y + h - amountToRemove, w, amountToRemove);
         h -= amountToRemove;
         return r;
     }

     /** Returns the nearest point to the specified point that lies within this rectangle. */
     Point<ValueType> getConstrainedPoint (Point<ValueType> point) const noexcept
     {
         return { jlimit (pos.x, pos.x + w, point.x),
                  jlimit (pos.y, pos.y + h, point.y) };
     }

     /** Returns a point within this rectangle, specified as proportional coordinates.
         The relative X and Y values should be between 0 and 1, where 0 is the left or
         top of this rectangle, and 1 is the right or bottom. (Out-of-bounds values
         will return a point outside the rectangle).
     */
     template <typename FloatType>
     Point<ValueType> getRelativePoint (FloatType relativeX, FloatType relativeY) const noexcept
     {
         return { pos.x + static_cast<ValueType> ((FloatType) w * relativeX),
                  pos.y + static_cast<ValueType> ((FloatType) h * relativeY) };
     }

     /** Returns a proportion of the width of this rectangle. */
     template <typename FloatType>
     ValueType proportionOfWidth (FloatType proportion) const noexcept
     {
         return static_cast<ValueType> ((FloatType) w * proportion);
     }

     /** Returns a proportion of the height of this rectangle. */
     template <typename FloatType>
     ValueType proportionOfHeight (FloatType proportion) const noexcept
     {
         return static_cast<ValueType> ((FloatType) h * proportion);
     }

     /** Returns a rectangle based on some proportional coordinates relative to this one.
         So for example getProportion ({ 0.25f, 0.25f, 0.5f, 0.5f }) would return a rectangle
         of half the original size, with the same centre.
     */
     template <typename FloatType>
     Rectangle getProportion (Rectangle<FloatType> proportionalRect) const noexcept
     {
         return { pos.x + static_cast<ValueType> (w * proportionalRect.pos.x),
                  pos.y + static_cast<ValueType> (h * proportionalRect.pos.y),
                  proportionOfWidth  (proportionalRect.w),
                  proportionOfHeight (proportionalRect.h) };
     }

     /** Returns true if the two rectangles are identical. */
     bool operator== (const Rectangle& other) const noexcept
     {
    	 return pos == other.pos && w == other.w && h == other.h;
     }

     /** Returns true if the two rectangles are not identical. */
     bool operator!= (const Rectangle& other) const noexcept     { return ! operator== (other); }

     /** Returns true if this coordinate is inside the rectangle. */
     bool contains (ValueType xCoord, ValueType yCoord) const noexcept
     {
         return xCoord >= pos.x && yCoord >= pos.y && xCoord < pos.x + w && yCoord < pos.y + h;
     }

     /** Returns true if this coordinate is inside the rectangle. */
     bool contains (Point<ValueType> point) const noexcept
     {
         return point.x >= pos.x && point.y >= pos.y && point.x < pos.x + w && point.y < pos.y + h;
     }

     /** Returns true if this other rectangle is completely inside this one. */
     bool contains (Rectangle other) const noexcept
     {
         return pos.x <= other.pos.x && pos.y <= other.pos.y
             && pos.x + w >= other.pos.x + other.w && pos.y + h >= other.pos.y + other.h;
     }

     /** Returns true if any part of another rectangle overlaps this one. */
     bool intersects (Rectangle other) const noexcept
     {
         return pos.x + w > other.pos.x
             && pos.y + h > other.pos.y
             && pos.x < other.pos.x + other.w
             && pos.y < other.pos.y + other.h
             && w > ValueType() && h > ValueType()
             && other.w > ValueType() && other.h > ValueType();
     }

     /** Returns true if any part of the given line lies inside this rectangle. */
     bool intersects (const Line<ValueType>& line) const noexcept
     {
         return contains (line.getStart()) || contains (line.getEnd())
                 || line.intersects (Line<ValueType> (getTopLeft(),     getTopRight()))
                 || line.intersects (Line<ValueType> (getTopRight(),    getBottomRight()))
                 || line.intersects (Line<ValueType> (getBottomRight(), getBottomLeft()))
                 || line.intersects (Line<ValueType> (getBottomLeft(),  getTopLeft()));
     }

     /** Returns the region that is the overlap between this and another rectangle.
         If the two rectangles don't overlap, the rectangle returned will be empty.
     */
     Rectangle getIntersection (Rectangle other) const noexcept
     {
         auto nx = jmax (pos.x, other.pos.x);
         auto ny = jmax (pos.y, other.pos.y);
         auto nw = jmin (pos.x + w, other.pos.x + other.w) - nx;

         if (nw >= ValueType())
         {
             auto nh = jmin (pos.y + h, other.pos.y + other.h) - ny;

             if (nh >= ValueType())
                 return { nx, ny, nw, nh };
         }

         return {};
     }

     /** Clips a set of rectangle coordinates so that they lie only within this one.
         This is a non-static version of intersectRectangles().
         Returns false if the two rectangles didn't overlap.
     */
     bool intersectRectangle (ValueType& otherX, ValueType& otherY, ValueType& otherW, ValueType& otherH) const noexcept
     {
         auto maxX = jmax (otherX, pos.x);
         otherW = jmin (otherX + otherW, pos.x + w) - maxX;

         if (otherW > ValueType())
         {
             auto maxY = jmax (otherY, pos.y);
             otherH = jmin (otherY + otherH, pos.y + h) - maxY;

             if (otherH > ValueType())
             {
                 otherX = maxX; otherY = maxY;
                 return true;
             }
         }

         return false;
     }

     /** Clips a rectangle so that it lies only within this one.
         Returns false if the two rectangles didn't overlap.
     */
     bool intersectRectangle (Rectangle<ValueType>& rectangleToClip) const noexcept
     {
         return intersectRectangle (rectangleToClip.pos.x, rectangleToClip.pos.y,
                                    rectangleToClip.w,     rectangleToClip.h);
     }

     /** Returns the smallest rectangle that contains both this one and the one passed-in.

         If either this or the other rectangle are empty, they will not be counted as
         part of the resulting region.
     */
     Rectangle getUnion (Rectangle other) const noexcept
     {
         if (other.isEmpty())  return *this;
         if (isEmpty())        return other;

         auto newX = jmin (pos.x, other.pos.x);
         auto newY = jmin (pos.y, other.pos.y);

         return { newX, newY,
                  jmax (pos.x + w, other.pos.x + other.w) - newX,
                  jmax (pos.y + h, other.pos.y + other.h) - newY };
     }

     /** If this rectangle merged with another one results in a simple rectangle, this
         will set this rectangle to the result, and return true.

         Returns false and does nothing to this rectangle if the two rectangles don't overlap,
         or if they form a complex region.
     */
     bool enlargeIfAdjacent (Rectangle other) noexcept
     {
         if (exactlyEqual (pos.x, other.pos.x) && exactlyEqual (getRight(), other.getRight())
              && (other.getBottom() >= pos.y && other.pos.y <= getBottom()))
         {
             auto newY = jmin (pos.y, other.pos.y);
             h = jmax (getBottom(), other.getBottom()) - newY;
             pos.y = newY;
             return true;
         }

         if (exactlyEqual (pos.y, other.pos.y) && exactlyEqual (getBottom(), other.getBottom())
              && (other.getRight() >= pos.x && other.pos.x <= getRight()))
         {
             auto newX = jmin (pos.x, other.pos.x);
             w = jmax (getRight(), other.getRight()) - newX;
             pos.x = newX;
             return true;
         }

         return false;
     }

     /** If after removing another rectangle from this one the result is a simple rectangle,
         this will set this object's bounds to be the result, and return true.

         Returns false and does nothing to this rectangle if the two rectangles don't overlap,
         or if removing the other one would form a complex region.
     */
     bool reduceIfPartlyContainedIn (Rectangle other) noexcept
     {
         int inside = 0;
         auto otherR = other.getRight();
         if (pos.x >= other.pos.x && pos.x < otherR) inside = 1;
         auto otherB = other.getBottom();
         if (pos.y >= other.pos.y && pos.y < otherB) inside |= 2;
         auto r = pos.x + w;
         if (r >= other.pos.x && r < otherR) inside |= 4;
         auto b = pos.y + h;
         if (b >= other.pos.y && b < otherB) inside |= 8;

         switch (inside)
         {
             case 1 + 2 + 8:  w = r - otherR; pos.x = otherR; return true;
             case 1 + 2 + 4:  h = b - otherB; pos.y = otherB; return true;
             case 2 + 4 + 8:  w = other.pos.x - pos.x;        return true;
             case 1 + 4 + 8:  h = other.pos.y - pos.y;        return true;
             default:         break;
         }

         return false;
     }

     /** Tries to fit this rectangle within a target area, returning the result.

         If this rectangle is not completely inside the target area, then it'll be
         shifted (without changing its size) so that it lies within the target. If it
         is larger than the target rectangle in either dimension, then that dimension
         will be reduced to fit within the target.
     */
     Rectangle constrainedWithin (Rectangle areaToFitWithin) const noexcept
     {
         auto newPos = areaToFitWithin.withSize (areaToFitWithin.getWidth() - w,
                                                 areaToFitWithin.getHeight() - h)
                         .getConstrainedPoint (pos);

         return { newPos.x, newPos.y,
                  jmin (w, areaToFitWithin.getWidth()),
                  jmin (h, areaToFitWithin.getHeight()) };
     }

     /** Returns the smallest rectangle that can contain the shape created by applying
         a transform to this rectangle.

         This should only be used on floating point rectangles.
     */
/*     [[nodiscard]] Rectangle transformedBy (const AffineTransform& transform) const noexcept
     {
         using FloatType = TypeHelpers::SmallestFloatType<ValueType>;

         auto x1 = static_cast<FloatType> (pos.x),     y1 = static_cast<FloatType> (pos.y);
         auto x2 = static_cast<FloatType> (pos.x + w), y2 = static_cast<FloatType> (pos.y);
         auto x3 = static_cast<FloatType> (pos.x),     y3 = static_cast<FloatType> (pos.y + h);
         auto x4 = static_cast<FloatType> (x2),        y4 = static_cast<FloatType> (y3);

         transform.transformPoints (x1, y1, x2, y2);
         transform.transformPoints (x3, y3, x4, y4);

         auto rx1 = jmin (x1, x2, x3, x4);
         auto rx2 = jmax (x1, x2, x3, x4);
         auto ry1 = jmin (y1, y2, y3, y4);
         auto ry2 = jmax (y1, y2, y3, y4);

         Rectangle r;
         Rectangle<FloatType> (rx1, ry1, rx2 - rx1, ry2 - ry1).copyWithRounding (r);
         return r;
     }
*/
     /** Returns the smallest integer-aligned rectangle that completely contains this one.
         This is only relevant for floating-point rectangles, of course.
         @see toFloat(), toNearestInt(), toNearestIntEdges()
     */
/*
     Rectangle<int> getSmallestIntegerContainer() const noexcept
     {
         return Rectangle<int>::leftTopRightBottom (detail::floorAsInt (pos.x),
                                                    detail::floorAsInt (pos.y),
                                                    detail::ceilAsInt  (pos.x + w),
                                                    detail::ceilAsInt  (pos.y + h));
     }
*/
     /** Casts this rectangle to a Rectangle<int>.
         This uses roundToInt to snap x, y, width and height to the nearest integer (losing precision).
         If the rectangle already uses integers, this will simply return a copy.
         @see getSmallestIntegerContainer(), toNearestIntEdges()
     */
     Rectangle<int> toNearestInt() const noexcept
     {
         return { roundToInt (pos.x), roundToInt (pos.y),
                  roundToInt (w),     roundToInt (h) };
     }

     /** Casts this rectangle to a Rectangle<int>.
         This uses roundToInt to snap top, left, right and bottom to the nearest integer (losing precision).
         If the rectangle already uses integers, this will simply return a copy.
         @see getSmallestIntegerContainer(), toNearestInt()
     */
     Rectangle<int> toNearestIntEdges() const noexcept
     {
         return Rectangle<int>::leftTopRightBottom (roundToInt (pos.x),       roundToInt (pos.y),
                                                    roundToInt (getRight()),  roundToInt (getBottom()));
     }

     /** Casts this rectangle to a Rectangle<float>.
         @see getSmallestIntegerContainer
     */
     Rectangle<float> toFloat() const noexcept
     {
         return { static_cast<float> (pos.x), static_cast<float> (pos.y),
                  static_cast<float> (w),     static_cast<float> (h) };
     }

     /** Casts this rectangle to a Rectangle<double>.
         @see getSmallestIntegerContainer
     */
     Rectangle<double> toDouble() const noexcept
     {
         return { static_cast<double> (pos.x), static_cast<double> (pos.y),
                  static_cast<double> (w),     static_cast<double> (h) };
     }

     /** Casts this rectangle to a Rectangle with the given type.
         If the target type is a conversion from float to int, then the conversion
         will be done using getSmallestIntegerContainer().
     */
     template <typename TargetType>
     Rectangle<TargetType> toType() const noexcept
     {
         Rectangle<TargetType> r;
         copyWithRounding (r);
         return r;
     }

     /** Returns the smallest Rectangle that can contain a set of points. */
     static Rectangle findAreaContainingPoints (const Point<ValueType>* points, int numPoints) noexcept
     {
         if (numPoints <= 0)
             return {};

         auto minX = points[0].x;
         auto maxX = minX;
         auto minY = points[0].y;
         auto maxY = minY;

         for (int i = 1; i < numPoints; ++i)
         {
             minX = jmin (minX, points[i].x);
             maxX = jmax (maxX, points[i].x);
             minY = jmin (minY, points[i].y);
             maxY = jmax (maxY, points[i].y);
         }

         return { minX, minY, maxX - minX, maxY - minY };
     }

private:
    Point<ValueType> pos;
    ValueType w {}, h {};

};

}
