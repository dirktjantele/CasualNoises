/*
  ==============================================================================

    CharacterFunctions.h
    Created: 26/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

class CharPointerType;

class CharacterFunctions final
{
public:
	CharacterFunctions () = delete;

	static bool isWhitespace ( const char character ) noexcept
	{
		return character <= ' ';
	}

	static bool isLetterOrDigit (char character) noexcept;

    static int indexOf ( CharPointerType textToSearch, const CharPointerType substringToLookFor ) noexcept;
    static int compareUpTo ( CharPointerType s1, CharPointerType s2, int maxChars) noexcept;
    static int compare (char char1, char char2) noexcept;

};

} // namespace CasualNoises
