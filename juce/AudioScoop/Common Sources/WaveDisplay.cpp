/*
  ==============================================================================

    WaveDisplay.cpp
    Created: 21 May 2021 4:08:18pm
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveDisplay.h"
#include "Constants.h"

//==============================================================================
//          WaveDisplayLabel()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
WaveDisplayLabel::WaveDisplayLabel(juce::String prefix)
{
    mLabelPrefix = prefix;
    setText(juce::String(mLabelPrefix + juce::String(mValue)), juce::dontSendNotification);
    setJustificationType(juce::Justification::centredLeft);
}

//==============================================================================
//          paint()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
void WaveDisplayLabel::paint (juce::Graphics& g)
{
    setColour(Label::textColourId, juce::Colours::lightgrey);
    setFont(12.0f);
    setText(juce::String(mLabelPrefix + juce::String(mValue)), juce::dontSendNotification);
    juce::Label::paint(g);
}
    
//==============================================================================
//          paint()
//
//  CasualNoises    24/05/2021  First implementation
//==============================================================================
void ScoopWaveForm::paint (juce::Graphics& g)
{
    
    // Buffer still empty?
    t__WaveDisplaySettings* settings = mWaveDisplay->getDisplaySettings();
    if ((settings->waveBufferSize == 0) || (settings->waveSampleSize == 0)) {
        return;
    }
    
    // Draw the waveform
    juce::Rectangle<int> rect = getBounds();
    float mid_y = (float)rect.getHeight() / 2;
    juce::Point<float> p1(0, mid_y);
    double scoopXIncrement = 1 / settings->pixelToSampleScale;
    if (scoopXIncrement < 1.0f) {
        scoopXIncrement = 1.0f;
    }
    for (double i = -scoopXIncrement; i < settings->scoopPixelWidth; i += scoopXIncrement) {
        g.setColour (juce::Colours::white);
        int sampleIndex = settings->scoopSampleStart + (i * settings->pixelToSampleScale);
        if (i < 0.0f) {
            sampleIndex = settings->scoopSampleStart;
        }
        float sample = settings->waveToDisplayPtr[(int)sampleIndex];
        if ((sample >= 1.0f) || (sample <= -1.0f)) {
            g.setColour (juce::Colours::red);
        }
        sample = juce::jlimit<float>(-1.0f, 1.0f, sample * settings->scoopGain);
        sample = juce::jmap<float>(sample, -1.0f, 1.0f, rect.getHeight(), 0);
        juce::Point<float> p2(i, sample);
        if (i >= 0.0f) {
            if (scoopXIncrement >= 2.0f) {
                p1.setX(i);
                p1.setY(mid_y);
                juce::Line<float> line(p1, p2);
                g.drawLine (line, 1.0f);
            } else {
                juce::Line<float> line(p1, p2);
                g.drawLine (line, 1.0f);
            }
        } else {
            p2.setX(0.0f);
        }
        p1 = p2;
    }

}

//==============================================================================
//          paint()
//
//  CasualNoises    23/05/2021  First implementation
//==============================================================================
void ScoopRaster::paint (juce::Graphics& g)
{
    
    // Paint a border
    juce::Rectangle<int> rect = getBounds();
    g.setColour (juce::Colours::lightgreen);
    g.drawRoundedRectangle(0, 0, rect.getWidth(), rect.getHeight(), cCornerSize, 1.0f);

    // Draw middle line
    auto y = rect.getHeight() / 2;
    juce::Line<float> line(0, y, rect.getWidth(), y);
    g.drawLine (line, 0.5f);

 //   t__WaveDisplaySettings* settings = mWaveDisplay->getDisplaySettings();

    
}

//==============================================================================
//          ScoopParameters()
//
//  CasualNoises    23/05/2021  First implementation
//==============================================================================
ScoopParameters::ScoopParameters (WaveDisplay* waveDisplay)
{
    mWaveDisplay = waveDisplay;
    
    addAndMakeVisible(&mNoOfSamplesText);
    addAndMakeVisible(&mGainValueText);
    addAndMakeVisible(&mScaleValueText);
    addAndMakeVisible(&mOffsetValueText);
    
    addAndMakeVisible(&mTotalMinValueText);
    addAndMakeVisible(&mTotalMaxValueText);

    mSamplerStateLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    mSamplerStateLabel.setFont(12.0f);
    mSamplerStateLabel.setText("UNKNOWN", juce::dontSendNotification);
    mSamplerStateLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(&mSamplerStateLabel);
    
}

//==============================================================================
//          paintLocationMarker()
//
//  CasualNoises    25/05/2021  First implementation
//==============================================================================
void ScoopParameters::paintLocationMarker(juce::Graphics& g)
{
    t__WaveDisplaySettings* settings = mWaveDisplay->getDisplaySettings();
    if (settings->waveBufferSize == 0) return;

    auto width = cLabelWidthRegular;
    auto height = cLabelHeightSmall;
    
    auto x = (settings->scoopPixelWidth / 2) - (width / 2);
    auto y = height / 4;
    g.setColour(juce::Colours::darkgreen);
    g.drawRect(x, y, width, height);

    y += 2;
    width  -= 4;
    height -= 4;
    
    auto scale = (settings->scoopPixelWidth * settings->pixelToSampleScale) / settings->waveBufferSize;
    auto length = width * scale;
    if (length < 2) length = 2;
    int dx = ((float)width / (float)settings->waveBufferSize) * (float)settings->scoopSampleStart;
    g.setColour(juce::Colours::lightgreen);
    g.fillRect(x + dx + 2, y, length, height);

}

//==============================================================================
//          paintInputLevel()
//
//  CasualNoises    25/05/2021  First implementation
//==============================================================================
void ScoopParameters::paintInputLevel(juce::Graphics& g)
{
    t__WaveDisplaySettings* settings = mWaveDisplay->getDisplaySettings();
    juce::Rectangle<int> rect = getBounds();

    juce::Colour colour = juce::Colours::lightgreen;
    auto min_y =  settings->minInputLevel * settings->scoopGain;
    min_y = juce::jmap<float>(min_y, -1.0f, 1.0f, rect.getHeight(), 0);
    auto l_min_y = juce::jlimit<float>(0, rect.getHeight(), min_y);
    if (l_min_y != min_y) colour = juce::Colours::mediumvioletred;
    auto max_y =  settings->maxInputLevel * settings->scoopGain;
    max_y = juce::jmap<float>(max_y, -1.0f, 1.0f, rect.getHeight(), 0);
    auto l_max_y = juce::jlimit<float>(0, rect.getHeight(), max_y);
    if (l_max_y != l_max_y) colour = juce::Colours::mediumvioletred;
    
    juce::Point<float> p1(cMainX_Indent - 2, l_min_y);
    juce::Point<float> p2(cMainX_Indent + 2, l_min_y);
    juce::Line<float> line(p1, p2);
    g.setColour(colour);
    g.drawLine (line, 1.0f);
    
    if (p1 != p2) {
        
        p1.setY(l_max_y);
        p2.setY(l_max_y);
        juce::Line<float> line(p1, p2);
        g.drawLine (line, 1.0f);
        
        p1.setX(cMainX_Indent);
        p2.setX(cMainX_Indent);
        p2.setY(l_min_y);
        line.setStart(p1);
        line.setEnd(p2);
        g.drawLine (line, 1.0f);
        
    }

}

//==============================================================================
//          paint()
//
//  CasualNoises    23/05/2021  First implementation
//==============================================================================
void ScoopParameters::paint (juce::Graphics& g)
{
    
    // Update labels
    t__WaveDisplaySettings* settings = mWaveDisplay->getDisplaySettings();
    mNoOfSamplesText.setValue(settings->waveSampleSize);
    auto scoopGain = settings->gainSliderValue;
    scoopGain = juce::jmap<double>(scoopGain, 0.0f, 100.0f, 10000.0f, 1.0f);
    scoopGain = log10(scoopGain);
    scoopGain = juce::jmap<double>(scoopGain, 0.0f, 4.0f, 4.0f, 0.9f);
    settings->scoopGain = scoopGain;
    mGainValueText.setValue(settings->scoopGain);
    mScaleValueText.setValue(settings->scaleSliderValue);
    mOffsetValueText.setValue(settings->offsetSliderValue);
    
    // Total min & max labels
    if (settings->minTotalInputLevel < 9.0f)
        mTotalMinValueText.setValue(settings->minTotalInputLevel);
    else
        mTotalMinValueText.setValue(0.0f);
    if (settings->maxTotalInputLevel > -9.0f)
        mTotalMaxValueText.setValue(settings->maxTotalInputLevel);
    else
        mTotalMaxValueText.setValue(0.0f);

    // Update state label
    switch (settings->samplerState) {
        case st_SamplerReady:
            mSamplerStateLabel.setText("State = Ready", juce::dontSendNotification);
            break;
        case st_SamplerBusy:
            mSamplerStateLabel.setText("State = Sampling", juce::dontSendNotification);
            break;
        case st_AwaitingTrigger:
            mSamplerStateLabel.setText("State = Waiting", juce::dontSendNotification);
            break;
        default:
            mSamplerStateLabel.setText("-", juce::dontSendNotification);
    }
    
    // Draw level line when awaiting trigger level
    if (settings->samplerMode == sm_Triggered) {
        juce::Rectangle<int> rect = getBounds();
        auto level = (settings->triggerLevel * settings->scoopGain) * (rect.getHeight() / 2);
        float y = (rect.getHeight() / 2) - level;
        g.setColour(juce::Colours::lightblue);
        g.drawLine(0.0f, y, rect.getWidth(), y, 1.0f);
    }
    
    // Paint graphics
    paintLocationMarker(g);
    paintInputLevel(g);
    
}

//==============================================================================
//          resized()
//
//  CasualNoises    23/05/2021  First implementation
//==============================================================================
void ScoopParameters::resized ()
{
    auto ys = (cLabelHeightSmall / 2) + 2;
    
    auto x = cSpacingX;
    auto y = cSpacingY;
    mNoOfSamplesText.setBounds(x, y, cLabelWidthLarge * 2, cLabelHeightSmall);
    
    x = cSpacingX;
    y += ys;
    mSamplerStateLabel.setBounds(x, y, cLabelWidthLarge * 2, cLabelHeightSmall);
    
    x = cSpacingX;
    y += cLabelHeightSmall;
    mTotalMaxValueText.setBounds(x, y, cLabelWidthLarge * 2, cLabelHeightSmall);
    y += ys;
    mTotalMinValueText.setBounds(x, y, cLabelWidthLarge * 2, cLabelHeightSmall);

    juce::Rectangle<int> rect = getBounds();
    x = cSpacingX;
    y = rect.getHeight() - cLabelHeightSmall - cSpacingY;
    mGainValueText.setBounds(x, y, cLabelWidthRegular, cLabelHeightSmall);
    x += cLabelWidthRegular + (cSpacingX * 2);
    mScaleValueText.setBounds(x, y, cLabelWidthRegular, cLabelHeightSmall);
    x += cLabelWidthRegular + (cSpacingX * 2);
    mOffsetValueText.setBounds(x, y, cLabelWidthRegular, cLabelHeightSmall);

}

//==============================================================================
//          WaveDisplay()     /       ~WaveDisplay
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
WaveDisplay::WaveDisplay() : mScoopWaveForm(this),
                             mScoopRaster(this),
                             mScoopParameters(this)
{
    mWaveDisplaySettings.waveToDisplayPtr = nullptr;
    mWaveDisplaySettings.waveBufferSize   = 0;
    mWaveDisplaySettings.waveSampleSize   = 0;
    
    mGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true,
                                        cSliderTextWidthRegular, cSliderTextHeightRegular);
    mGainSlider.setSliderStyle(juce::Slider::LinearVertical );
    mGainSlider.setRange(0.0f, 100.0f, 0.0f);
    mGainSlider.addListener(this);
    addAndMakeVisible(&mGainSlider);
    mWaveDisplaySettings.gainSliderValue = 0.0f;

    mScaleSlider.setTextBoxStyle(juce::Slider::NoTextBox, true,
                                        cSliderTextWidthRegular, cSliderTextHeightRegular);
    mScaleSlider.setSliderStyle(juce::Slider::LinearHorizontal );
    mScaleSlider.setRange(1.0f, 100.0f, 1.0f);
    mScaleSlider.setValue(1.0f);
    mScaleSlider.addListener(this);
    addAndMakeVisible(&mScaleSlider);
    mWaveDisplaySettings.scaleSliderValue = 1.0f;

    mOffsetSlider.setTextBoxStyle(juce::Slider::NoTextBox, true,
                                        cSliderTextWidthRegular, cSliderTextHeightRegular);
    mOffsetSlider.setSliderStyle(juce::Slider::LinearHorizontal );
    mOffsetSlider.setRange(1.0f, 100.0f, 1.0f);
    mOffsetSlider.setValue(1.0f);
    mOffsetSlider.addListener(this);
    addAndMakeVisible(&mOffsetSlider);
    mWaveDisplaySettings.offsetSliderValue = 1.0f;
    
    addAndMakeVisible(&mScoopRaster);
    addAndMakeVisible(&mScoopWaveForm);
    addAndMakeVisible(&mScoopParameters);
    
    startTimer (10);
    
}

WaveDisplay::~WaveDisplay()
{
}

//==============================================================================
//          resetWaveDisplay()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
void WaveDisplay::resetWaveDisplay ()
{
}

//==============================================================================
//          timerCallback()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
void WaveDisplay::timerCallback ()
{
    if (mWaveDisplaySettings.waveToDisplayPtr == nullptr) {
        return;
    }
    mOffsetSlider.setEnabled(mScaleSlider.getValue() > 1.0f);
    juce::Range<double> range = mOffsetSlider.getRange();
    if ((mWaveDisplaySettings.waveBufferSize > 0) &&
        (range.getEnd() != mWaveDisplaySettings.waveBufferSize)) {
        range.setEnd(mWaveDisplaySettings.waveBufferSize);
        double value = mOffsetSlider.getValue();
        if (value > range.getEnd()) {
            value = range.getEnd();
        }
        mOffsetSlider.setRange(range, 1.0f);
        mOffsetSlider.setValue(value);
        mWaveDisplaySettings.offsetSliderValue = value;
    }
    repaint();
}

//==============================================================================
//          sliderValueChanged()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
void WaveDisplay::sliderValueChanged(juce::Slider* inSlider)
{
    if (inSlider == &mGainSlider) {
        mWaveDisplaySettings.gainSliderValue   = mGainSlider.getValue();
    } else if (inSlider == &mScaleSlider) {
        mWaveDisplaySettings.scaleSliderValue  = mScaleSlider.getValue();
    } else if (inSlider == &mOffsetSlider) {
        mWaveDisplaySettings.offsetSliderValue = mOffsetSlider.getValue();
    } else jassertfalse;
    repaint();
}

//==============================================================================
//          paint()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
void WaveDisplay::paint (juce::Graphics& g)
{

    // Paint a border
    juce::Rectangle<int> rect = getBounds();
    g.setColour (juce::Colours::white);
    g.drawRoundedRectangle(0, 0, rect.getWidth(), rect.getHeight(), cCornerSize, 1.0f);

    // Calculate the scaling from pixel space to sample space
    double width = (double)mWaveDisplaySettings.scoopPixelWidth;
    double scaling = mWaveDisplaySettings.waveBufferSize / width;
    mWaveDisplaySettings.pixelToSampleScale = juce::jmap<double>(mWaveDisplaySettings.scaleSliderValue,
                                                      1.0f, 100.0f, scaling, 0.1f);
 
    // Calculate the location of the first sample in the display
    double scoopSampleRange = width * mWaveDisplaySettings.pixelToSampleScale;
    double scoopSampleStart = mWaveDisplaySettings.offsetSliderValue - (scoopSampleRange / 2);
    if (scoopSampleStart < 0) {
        scoopSampleStart = 0.0f;
    } else if ((scoopSampleStart + scoopSampleRange) > mWaveDisplaySettings.waveBufferSize) {
        scoopSampleStart = mWaveDisplaySettings.waveBufferSize - scoopSampleRange;
    }
    mWaveDisplaySettings.scoopSampleStart = scoopSampleStart;
    
}

//==============================================================================
//          resized()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
void WaveDisplay::resized()
{

    // Place gain slider left of the scoop
    juce::Rectangle<int> rect = getBounds();
    auto length = (rect.getHeight() * 2) / 3;
    auto y = (rect.getHeight() - length) / 2;
    mGainSlider.setBounds(cSpacingX, y, cSliderTextHeightRegular, length);
    
    // Scale and offset sliders below the scoop
    auto spacing = rect.getWidth() / 7;
    length = spacing * 2;
    y = rect.getHeight() - cSliderTextHeightRegular - (cSpacingY);
    mScaleSlider.setBounds(spacing, y, length, cSliderTextHeightRegular);
    mOffsetSlider.setBounds((spacing * 2) + length, y, length, cSliderTextHeightRegular);

    // The scoop layers
    juce::Rectangle<int> scoopRect{(cSpacingX * 2) + cSliderTextHeightRegular, cMainY_Indent,
        rect.getWidth()  - (cSpacingX * 4) - cSliderTextHeightRegular,
        rect.getHeight() - (cSpacingY * 4) - cSliderTextHeightRegular};
    mWaveDisplaySettings.scoopPixelWidth = scoopRect.getWidth();
    mScoopWaveForm.setBounds(scoopRect);
    mScoopRaster.setBounds(scoopRect);
    mScoopParameters.setBounds(scoopRect);

}
