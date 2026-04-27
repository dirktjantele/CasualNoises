/*
  ==============================================================================

    CharacterFunctions.cpp
    Created: 26/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "CharacterFunctions.h"
#include "CharPointerType.h"

namespace CasualNoises
{

//==============================================================================
//          isLetterOrDigit ()
//
//  CasualNoises    27/04/2026  First implementation
//==============================================================================
bool CharacterFunctions::isLetterOrDigit (const char character) noexcept
{
    return (character >= 'a' && character <= 'z')
        || (character >= 'A' && character <= 'Z')
        || (character >= '0' && character <= '9');
}

//==============================================================================
//          indexOf ()
//
//  CasualNoises    26/04/2026  First implementation
//==============================================================================
int CharacterFunctions::indexOf ( CharPointerType textToSearch, const CharPointerType substringToLookFor ) noexcept
{
    int index = 0;
    auto substringLength = substringToLookFor.length ();
    for (;;)
    {
        if ( textToSearch.compareUpTo ( substringToLookFor, substringLength ) == 0 )
            return index;
        if ( textToSearch.getAndAdvance () == 0 )
            return -1;
        ++index;
    }
}

//==============================================================================
//          compareUpTo ()
//
//  CasualNoises    26/04/2026  First implementation
//==============================================================================
int CharacterFunctions::compareUpTo ( CharPointerType s1, CharPointerType s2, int maxChars ) noexcept
{
    while ( --maxChars >= 0 )
    {
        auto c1 = s1.getAndAdvance ();

        if ( auto diff = compare ( c1, s2.getAndAdvance() ) )
            return diff;

        if (c1 == 0)
            break;

    }

    return 0;
}

//==============================================================================
//          compareUpTo ()
//
//  CasualNoises    26/04/2026  First implementation
//==============================================================================
int CharacterFunctions::compare (char char1, char char2) noexcept
{
    if (auto diff = static_cast<int> (char1) - static_cast<int> (char2))
        return diff < 0 ? -1 : 1;

    return 0;
}

} // namespace CasualNoises
