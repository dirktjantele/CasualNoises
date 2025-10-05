/*
  ==============================================================================

    PluginEditor.cpp
    Created: 13 May 2021 11:21      (Tevka support)
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "../../Common Sources/Constants.h"
#include "../../Common Sources/MyCheckBox.h"
#include "../../Common Sources/WaveDisplay.h"

//==============================================================================

class WaverAudioProcessorEditor;

//==============================================================================
class SamplerControlComponent  : public juce::Component,
                                 private juce::Timer,
                                 private juce::Slider::Listener
{
    
public:
    SamplerControlComponent (WaverAudioProcessorEditor*);
    ~SamplerControlComponent() override;

    void    paint (juce::Graphics&) override;
    void    resized() override;
    
    void    timerCallback () override;
    
    void    stopSampling()             { handleStopSamplingButtonClicked(); };
    
    t__SamplerStates getSamplerState() { return mSamplerState; };
    t__SamplerModes  getSamplerMode()  { return mSamplerMode; };

    static int  s_getPreferredWidth()  { return (cComboBoxWidthRegular * 2) + (cMainX_Indent * 4) + cBusyBoxSize; };
    static int  s_getPreferredHeight() { return (cComboBoxheight * 2) + (cMainX_Indent * 3); };

private:
    WaverAudioProcessorEditor*    mEditorPtr;
    
    juce::Component     mSamplerStateIndication;
    
    juce::ComboBox      mSamplerModeComboBox;
    juce::TextButton    mStartSamplingButton;
    juce::TextButton    mStopSamplingButton;
    
    bool                mSamplingStarted { false };
    
    juce::Slider        mTriggerLevelSlider;
    juce::Label         mTriggerLevel;
    
    MyCheckBox          mContiguousTriggeredModeCheckBox;
    bool                mContigousTriggeredMode { false };
    void                handleContiguousTriggeredModeCheckBoxClicked();

    MyCheckBox          mTriggeredPolarityCheckBox;
    bool                mPositivePolarity { true };
    void                handleTriggeredPolarityCheckBoxClicked();

    t__SamplerStates    mSamplerState { st_SamplerReady };
    t__SamplerModes     mSamplerMode  { sm_SingleShot   };
    t__TriggerModes     mTriggerMode  { tm_MIDI_NoteOn  };

    void                handleSamplerModeComboBoxChange();
    void                handleStartSamplingButtonClicked();
    void                handleStopSamplingButtonClicked();
    
    void                sliderValueChanged(juce::Slider* inSlider) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerControlComponent)
};

//==============================================================================
class SamplerInfoComponent  : public juce::Component,
                              private juce::Timer
{
    
public:
    SamplerInfoComponent (WaverAudioProcessorEditor*);
    ~SamplerInfoComponent() override;

    void    paint (juce::Graphics&) override;
    void    resized() override;
    
    void    timerCallback () override;
    
    static int  s_getPreferredWidth()  { return SamplerControlComponent::s_getPreferredWidth(); };
    static int  s_getPreferredHeight() { return (cLabelHeight * 3) + (cMainX_Indent * 3); };

private:
    WaverAudioProcessorEditor*    mEditorPtr;
    
    juce::Label             mWaveBufferSizeText  { "mWaveBufferSizeText" };
    juce::Label             mWaveBufferSize;
    juce::Label             mDurationText        { "mDurationText" };
    juce::Label             mAudioBufferSizeText { "mAudioBufferSizeText" };
    juce::Label             mAudioBufferSize;
    juce::Label             mNoOfChannelsText    { "mNoOfChannelsText" };
    juce::Label             mNoOfChannels;
    
    void    handleWaveBufferSizeChange ();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerInfoComponent)
};


//==============================================================================
class SamplerProgressComponent  : public juce::Component,
                                  private juce::Timer
{
    
public:
    SamplerProgressComponent (WaverAudioProcessorEditor*);
    ~SamplerProgressComponent() override;

    void    paint (juce::Graphics&) override;
    void    resized() override;
    
    void    timerCallback () override;
    
    static int  s_getPreferredWidth()  { return SamplerControlComponent::s_getPreferredWidth(); };
    static int  s_getPreferredHeight() { return cComboBoxheight + (cMainX_Indent * 2); };

private:
    WaverAudioProcessorEditor*  mEditorPtr;
    
    long                        mCurrentProgress { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerProgressComponent)
};

//==============================================================================
class SaveSamplerDataComponent  : public juce::Component,
                                  private juce::Timer
{
    
public:
    SaveSamplerDataComponent (WaverAudioProcessorEditor*);
    ~SaveSamplerDataComponent() override;

    void    paint (juce::Graphics&) override;
    void    resized() override;
    
    void    timerCallback () override;
    
    static int  s_getPreferredWidth()  { return SamplerControlComponent::s_getPreferredWidth(); };
    static int  s_getPreferredHeight() { return (cLabelHeight * 3) + cButtonHeightRegular + (cSpacingY * 4); };

private:
    WaverAudioProcessorEditor*    mEditorPtr;
    
    juce::Label         mNameTextLabel;
    juce::Label         mNameEditLabel;
    juce::Label         mFolderTextLabel;
    juce::Label         mFolderNameLabel;
    juce::TextButton    mSelectFolderButton;
    juce::TextButton    mSaveSampleButton;
    
    void    handleNameEditLabelChange();
    void    handleSelectFolderButtonClicked();
    void    handleSaveSampleButtonClicked();
    
    void    createCharmProject();
    void    createAudioFile();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaveSamplerDataComponent)
};

//==============================================================================
class CasualNoisesComponent  : public  juce::Component,
                               private juce::Timer
{
    
public:

    CasualNoisesComponent ();
    ~CasualNoisesComponent() override;

    void    timerCallback () override;

    void    paint (juce::Graphics&) override;
    void    resized() override;

    static int  s_getPreferredWidth()  { return SamplerControlComponent::s_getPreferredWidth(); };
    static int  s_getPreferredHeight() { return cLabelHeight + (cSpacingY * 2); };

private:
    juce::Label         mCasualNoisesLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CasualNoisesComponent)
};

//==============================================================================
class SamplerScoopComponent  : public  juce::Component,
                               private juce::Timer
{
    
public:
    SamplerScoopComponent (WaverAudioProcessorEditor*);
    ~SamplerScoopComponent() override;

    void    timerCallback () override;
    
    void    paint (juce::Graphics&) override;
    void    resized() override;
    
    static int  s_getPreferredWidth()  { return 500; };
    static int  s_getPreferredHeight() { return SamplerInfoComponent::s_getPreferredHeight() +
                                                SamplerControlComponent::s_getPreferredHeight() +
                                                SamplerProgressComponent::s_getPreferredHeight() +
                                                SaveSamplerDataComponent::s_getPreferredHeight() +
                                                CasualNoisesComponent::s_getPreferredHeight() +
                                                (cMainY_Indent * 11); };

private:
    WaverAudioProcessorEditor*  mEditorPtr;
    
    WaveDisplay                 mWaveDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerScoopComponent)
};

//==============================================================================
class WaverAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                   private juce::Timer
{
public:
    WaverAudioProcessorEditor (WaverAudioProcessor&);
    ~WaverAudioProcessorEditor() override;

    void    paint (juce::Graphics&) override;
    void    resized() override;
    
    void    timerCallback () override;
    
    SamplerControlComponent&    getSamplerControlComponent()  { return mSamplerControlComponent; };
    SamplerProgressComponent&   getSamplerProgressComponent() { return mSamplerProgressComponent; };
    SamplerScoopComponent&      getSamplerScoopComponent()    { return mSamplerScoopComponent; };
    SaveSamplerDataComponent&   getSaveSamplerDataComponent() { return mSaveSamplerDataComponent; };
    
    WaverAudioProcessor*        getWaverAudioProcessor()      { return &audioProcessor; };
    
    t_WaverDataStruct*          getLocalWaverData()           { return audioProcessor.getLocalWaverData(); };

private:
    WaverAudioProcessor&     audioProcessor;

    SamplerControlComponent  mSamplerControlComponent;
    SamplerProgressComponent mSamplerProgressComponent;
    SamplerScoopComponent    mSamplerScoopComponent;
    SamplerInfoComponent     mSamplerInfoComponent;
    SaveSamplerDataComponent mSaveSamplerDataComponent;
    CasualNoisesComponent    mCasualNoisesComponent;
 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaverAudioProcessorEditor)
};

