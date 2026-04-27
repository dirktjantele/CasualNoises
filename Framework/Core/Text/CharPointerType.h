/*
  ==============================================================================

    CharPointerType.h
    Created: 26/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <string.h>

#include "CharacterFunctions.h"

namespace CasualNoises
{

class CharPointerType final
{
public:
    explicit CharPointerType ( const char* rawPointer ) noexcept
        : mDataPtr ( const_cast<char*> ( rawPointer ) )
    {
    }

    CharPointerType ( const CharPointerType& other ) = default;

    CharPointerType& operator= ( const CharPointerType& other ) noexcept = default;

    CharPointerType& operator= ( const char* text ) noexcept
    {
    	mDataPtr = const_cast<char*> ( text );
        return *this;
    }

    bool isEmpty() const noexcept
    {
    	return *mDataPtr == 0;
    }

    CharPointerType& operator++() noexcept
    {
        assert ( *mDataPtr != 0 ); // trying to advance past the end of the string?
        mDataPtr += 1;
        return *this;
    }
    char operator*() const noexcept
    {
    	return *mDataPtr;
    }

    char operator[] ( int characterIndex ) const noexcept
    {
        auto p = mDataPtr;
        p += characterIndex;
        return *p;
    }

    bool operator== (CharPointerType other) const noexcept      { return mDataPtr == other.mDataPtr; }
    bool operator!= (CharPointerType other) const noexcept      { return mDataPtr != other.mDataPtr; }
    bool operator<= (CharPointerType other) const noexcept      { return mDataPtr <= other.mDataPtr; }
    bool operator<  (CharPointerType other) const noexcept      { return mDataPtr <  other.mDataPtr; }
    bool operator>= (CharPointerType other) const noexcept      { return mDataPtr >= other.mDataPtr; }
    bool operator>  (CharPointerType other) const noexcept      { return mDataPtr >  other.mDataPtr; }

    const char& operator+= ( int numToSkip ) noexcept
    {
        if ( numToSkip < 0 )
        {
            while ( ++numToSkip <= 0 )
                --mDataPtr;
        }
        else
        {
            while ( --numToSkip >= 0 )
                ++mDataPtr;
        }

        return *mDataPtr;
    }

    void incrementToEndOfWhitespace () noexcept
    {
    	while ( CharacterFunctions::isWhitespace ( *mDataPtr ) )
    		mDataPtr += 1;
    }

    int indexOf ( const char* stringToFind ) const noexcept
    {
    	CharPointerType target ( stringToFind );
        return CharacterFunctions::indexOf ( *this, target );
    }

    int length () const
    {
    	return strlen ( mDataPtr );
    }

    int compareUpTo ( CharPointerType other, const int maxChars) noexcept
    {
        return CharacterFunctions::compareUpTo ( *this, other, maxChars );
    }

    char getAndAdvance () noexcept
    {
    	char current = *mDataPtr++;
    	return current;
    }

private:
    char* 		mDataPtr		{ nullptr };

};

} // namespace CasualNoises

