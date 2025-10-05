/*
  ==============================================================================

    MyCheckBox.h
    Created: 20 May 2021 10:59:56am     Tevka Support
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
typedef enum t_CheckBoxContents {
    t_blank,
    t_checked,
    t_plus,
    t_minus
} t__CheckBoxContents;

//==============================================================================
class MyCheckBox  : public juce::Component
{
public:
    MyCheckBox();
    ~MyCheckBox() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void    mouseDown(const juce::MouseEvent& event) override;
    std::function<void()> onClick;
    
    bool    isChecked() { return (mClickCount & 0x01); };
    
    void    setCheckBoxContents(t__CheckBoxContents in) { mCheckBoxContents = in;
                                                          repaint(); 
                                                        };
    void    setColours(juce::Colour inBorder, juce::Colour inMarker) {
                mBorderColour = inBorder;   mMarkerColour = inMarker;
                repaint();
                                                                    };

private:
    
    t__CheckBoxContents     mCheckBoxContents { t_blank };
    juce::uint32            mClickCount { 0 };
    
    juce::Colour            mBorderColour { juce::Colours::black };
    juce::Colour            mMarkerColour { juce::Colours::white };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyCheckBox)
};
