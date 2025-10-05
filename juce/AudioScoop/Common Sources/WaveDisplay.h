/*
  ==============================================================================

    WaveDisplay.h
    Created: 21 May 2021 4:08:18pm
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
typedef enum t_SamplerStates {
    st_SamplerReady,
    st_SamplerBusy,
    st_AwaitingTrigger
} t__SamplerStates;

typedef enum t_SamplerModes {
    sm_SingleShot = 1,
    sm_Triggered,
    sm_Contiguous
} t__SamplerModes;

typedef enum t_TriggerModes {
    tm_MIDI_NoteOn = 1,
    tm_LevelBased
} t__TriggerModes;

//==============================================================================
typedef struct t_WaveDisplaySettings {
    
    // Data from the sampler
    float*      waveToDisplayPtr;
    long        waveBufferSize;
    long        waveSampleSize;
    double      minInputLevel;
    double      maxInputLevel;
    double      minTotalInputLevel;
    double      maxTotalInputLevel;
    
    // Trigger data
    double      triggerLevel;
    long        firstSamplePosition;

    // Sampler engine state
    t__SamplerModes  samplerMode;
    t__SamplerStates samplerState;
    
    
    // UI data
    double      gainSliderValue;
    double      scaleSliderValue;
    double      offsetSliderValue;

    // Scoop settings
    double      scoopGain;
    double      pixelToSampleScale;
    double      scoopSampleStart;
    int         scoopPixelWidth;
    
} t__WaveDisplaySettings;

//==============================================================================
class WaveDisplayLabel  : public  juce::Label
{
public:
    WaveDisplayLabel(juce::String prefix);

    void paint (juce::Graphics&) override;
    
    void setValue (double inValue) { mValue = inValue; };

private:
    juce::String    mLabelPrefix { "-" };
    double          mValue { 0 };
    
};

//==============================================================================
class WaveDisplay;
class ScoopWaveForm  : public  juce::Component
{
public:
    ScoopWaveForm(WaveDisplay* waveDisplay) { mWaveDisplay = waveDisplay; };

    void paint (juce::Graphics&) override;
    
private:
    WaveDisplay*    mWaveDisplay;
    
};

//==============================================================================
class ScoopRaster  : public  juce::Component
{
public:
    ScoopRaster(WaveDisplay* waveDisplay) { mWaveDisplay = waveDisplay; };

    void paint (juce::Graphics&) override;
    
private:
    WaveDisplay*    mWaveDisplay;
    
};

//==============================================================================
class ScoopParameters : public  juce::Component
{
public:
    ScoopParameters(WaveDisplay* waveDisplay);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    WaveDisplay*        mWaveDisplay;
    
    WaveDisplayLabel    mNoOfSamplesText   { "#Samples: " };
    WaveDisplayLabel    mGainValueText     { "Gain: " };
    WaveDisplayLabel    mScaleValueText    { "Scale: " };
    WaveDisplayLabel    mOffsetValueText   { "Offset: " };
    
    WaveDisplayLabel    mTotalMinValueText { "Min:  " };
    WaveDisplayLabel    mTotalMaxValueText { "Max: " };

    juce::Label         mSamplerStateLabel;

    void        paintLocationMarker(juce::Graphics& g);
    void        paintInputLevel(juce::Graphics& g);

};

//==============================================================================
class WaveDisplay  : public  juce::Component,
                     private juce::Timer,
                     private juce::Slider::Listener
{
public:
    WaveDisplay();
    ~WaveDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void    resetWaveDisplay();

    void    timerCallback () override;
    
    t__WaveDisplaySettings*     getDisplaySettings() { return &mWaveDisplaySettings; };
    
private:
    
    t__WaveDisplaySettings      mWaveDisplaySettings;

    juce::Slider                mGainSlider;
    juce::Slider                mScaleSlider;
    juce::Slider                mOffsetSlider;
    
    ScoopWaveForm               mScoopWaveForm;
    ScoopRaster                 mScoopRaster;
    ScoopParameters             mScoopParameters;
    
    void            sliderValueChanged(juce::Slider* inSlider) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveDisplay)
};
