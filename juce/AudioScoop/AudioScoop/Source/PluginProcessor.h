/*
  ==============================================================================

    PluginProcessor.h
    Created: 13 May 2021 11:21      (Tevka support)
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "WaverData.h"

//==============================================================================
/**
*/
class WaverAudioProcessor  : public juce::AudioProcessor
{
public:
    
    t_WaverDataStruct*      getLocalWaverData() { return &mLocalWaverData; };
    void                    resetWaverData(bool inClearBuffer);
    
private:
    
    t_WaverDataStruct       mLocalWaverData;
    
    float*                  mTriggerBuffer      { nullptr };
    int                     mTriggerBufferIndex { 0 };
    int                     mTriggerBufferSize  { 0 };
    
    bool                    mIsPrepared { false };
    bool                    mIsReleased { false };
    
    //==============================================================================
public:
    WaverAudioProcessor();
    ~WaverAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaverAudioProcessor)
};
