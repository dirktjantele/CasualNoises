/*
  ==============================================================================

    CasualNoises.h
    Created: 31 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "AudioBasics/Buffers/AudioBuffer.h"
#include "AudioBasics/Wavetable/WaveTable.h"

#include "Core/Maths/MathsFunctions.h"
#include "Core/Maths/Range.h"

#include "Core/Text/String.h"

#include "Drivers/I2C/I2C_SoftwareDriver.h"
#include "Drivers/IO_Drivers/PCF8574_Driver.h"
#include "Drivers/ILI9341/ILI9341_Driver.h"
#include "Drivers/Codec/CS4270_Driver.h"

#include "Graphics/Contexts/GraphicsContext.h"

#include "Graphics/Geometry/Line.h"
#include "Graphics/Geometry/Point.h"
#include "Graphics/Geometry/Rectangle.h"

#include "GUI/GUI_Basics/Components/Component.h"
#include "GUI/GUI_Basics/Components/BoxComponent.h"

#include "GUI/GUI_Utilities/WaveView.h"

#include "Synthesizer/Basics/ADSR.h"
#include "Synthesizer/Basics/LFO.h"

#include "Threads/EncoderThread.h"
#include "Threads/PotentiometerThread.h"

#include "Utilities/ReportFault.h"
#include "Utilities/CallbackHandlers.h"

