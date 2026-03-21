/*
  ==============================================================================

    Justification.h
    Created: 04/01/2026

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

enum class eJustificationFlags  : uint32_t {
	none				= 0,
	left 				= 1 << 0,
	right				= 1 << 1,
	centred				= 1 << 2,
	horizontallyCentred	= 1 << 3,
	verticallyCentred	= 1 << 4,
  /*
  left = 1 , right = 2 , horizontallyCentred = 4 , top = 8 ,
  bottom = 16 , verticallyCentred = 32 , horizontallyJustified = 64 , centred = 36 ,
  centredLeft = 33 , centredRight = 34 , centredTop = 12 , centredBottom = 20 ,
  topLeft = 9 , topRight = 10 , bottomLeft = 17 , bottomRight = 18
  */
};

inline constexpr eJustificationFlags operator| ( eJustificationFlags a, eJustificationFlags b ) {
    return static_cast <eJustificationFlags> (
        static_cast <unsigned int>(a) |
        static_cast <unsigned int>(b)
    );
}

inline constexpr eJustificationFlags& operator|= (eJustificationFlags& a, eJustificationFlags b ) {
    a = a | b;
    return a;
}
/*
inline constexpr eJustificationFlags operator& ( eJustificationFlags a, eJustificationFlags b ) {
    return static_cast <eJustificationFlags> (
        static_cast <unsigned int>(a) &
        static_cast <unsigned int>(b)
    );
*/
class Justification final
{
public:
	 Justification() = default;
	 Justification(eJustificationFlags flags)	{ mFlags = flags; };
	~Justification() = default;

	void setJustificationType (eJustificationFlags flags)
	{
		mFlags = flags;
	}

	eJustificationFlags getJustificationType()
	{
		return mFlags;
	}

private:
	eJustificationFlags mFlags;

};

} // namespace CasualNoises
