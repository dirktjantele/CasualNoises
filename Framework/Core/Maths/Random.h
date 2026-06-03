/*
  ==============================================================================

    Random.h
    Created: 21/04/20206
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <random>

namespace CasualNoises
{

int32_t getRandomNumber( int min, int max )
{
    static std::mt19937 gen ( 123456 );
    std::uniform_int_distribution<> dist ( min, max );
    return dist ( gen );
}

} // namespace CasualNoises
