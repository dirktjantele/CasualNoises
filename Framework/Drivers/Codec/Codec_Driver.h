/*
 * Codec_Driver.h
 *
 *  Created on: Oct 23, 2024
 *      Author: dirktjantele
 */

#pragma once

namespace CasualNoises
{

class Codec_Driver
{
public:
	 Codec_Driver() = default;
	~Codec_Driver() = default;

	virtual HAL_StatusTypeDef initializeCodec() = 0;

protected:

};

} // namespace CasualNoises
