/*
  ==============================================================================

    MyCheckBox.cpp
    Created: 20 May 2021 10:59:56am
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MyCheckBox.h"

#include "../../Common Sources/Constants.h"

//==============================================================================
//          MyCheckBox()     /       ~MyCheckBox
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
MyCheckBox::MyCheckBox()
{
}

MyCheckBox::~MyCheckBox()
{
}

//==============================================================================
//          paint()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
void MyCheckBox::paint (juce::Graphics& g)
{
    g.setColour (mBorderColour);
    g.drawRect(0, 0, cCheckBoxSize, cCheckBoxSize);
    float x1 = 3; float x2 = cCheckBoxSize - 3; float xm = (x2 + x1) / 2;
    float y1 = 3; float y2 = cCheckBoxSize - 3; float ym = (y2 + y1) / 2;
    g.setColour (mMarkerColour);
    switch (mCheckBoxContents) {
        case t_blank:
            break;
        case t_checked: {
            juce::Line<float> line1(x1, y1, x2, y2);
            g.drawLine(line1);
            juce::Line<float> line2(x2, y1, x1, y2);
            g.drawLine(line2);
            break;
        }
        case t_plus: {
            juce::Line<float> line1(xm, y1, xm, y2);
            g.drawLine(line1);
            // Fall through
        }
        case t_minus: {
            juce::Line<float> line1(x1, ym, x2, ym);
            g.drawLine(line1);
            break;
        }
    }
}

//==============================================================================
//          resized()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void MyCheckBox::resized()
{
}

//==============================================================================
//          mouseDown()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void MyCheckBox::mouseDown(const juce::MouseEvent& /*event*/)
{
    mClickCount += 1;
    if (onClick != nullptr)
        onClick();
}

