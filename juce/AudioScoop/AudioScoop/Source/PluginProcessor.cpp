/*
  ==============================================================================

    PluginProcessor.cpp
    Created: 13 May 2021 11:21      (Tevka support)
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//          WaverAudioProcessor()     /       ~WaverAudioProcessor()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
WaverAudioProcessor::WaverAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    
    // Local data initialization
    mLocalWaverData.maxAudioBufferSize  = 44100 * 60;                                  // 60 sec of audio
    mLocalWaverData.audioBufferSize     = mLocalWaverData.maxAudioBufferSize / 10;     // Buffer size to use during fill
    mLocalWaverData.buffer = (float*)malloc(sizeof(float) * mLocalWaverData.maxAudioBufferSize);
    mLocalWaverData.AP_UI_TotalMinInputLevel =  9.0f;
    mLocalWaverData.AP_UI_TotalMaxInputLevel = -9.0f;

    // Start sampling of wathever appears on the audio input channels
    resetWaverData(true);

}

WaverAudioProcessor::~WaverAudioProcessor()
{
    free(mLocalWaverData.buffer);
}

//==============================================================================
//          prepareToPlay()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void WaverAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mLocalWaverData.AP_UI_SampleRate = sampleRate;
    mTriggerBufferSize = 2 * samplesPerBlock;
    mTriggerBuffer = (float*)malloc(mTriggerBufferSize * sizeof(float));
    mLocalWaverData.numSamples = samplesPerBlock;
    mIsPrepared = true;
}

//==============================================================================
//          releaseResources()
//
//  CasualNoises    13/05/2021  First implementation
//  CasualNoises    05/06/2021  Handle mulpiple release calls
//==============================================================================
void WaverAudioProcessor::releaseResources()
{
    if (!mIsPrepared || mIsReleased) return;
    
    delete(mTriggerBuffer);
    mTriggerBuffer = nullptr;

    mIsReleased = true;
    
}

//==============================================================================
//          resetWaverData()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void WaverAudioProcessor::resetWaverData(bool inClearBuffer)
{
    if (inClearBuffer) {
        mLocalWaverData.bufferFillIndex      = 0;
        mLocalWaverData.noOfAvailableSamples = 0;
        mLocalWaverData.firstSamplePosition  = 0;
    }
    mLocalWaverData.UI_AP_StartSampling      = false;
    mLocalWaverData.UI_AP_AbortSampling      = false;
    mLocalWaverData.AP_UI_SamplingAborted    = false;
    mLocalWaverData.AP_UI_SamplingInProgress = false;
    mLocalWaverData.UI_AP_WaitForLevelEvent  = false;
    mLocalWaverData.UI_AP_TriggerMode        = false;
    mLocalWaverData.UI_AP_ContiguousMode     = false;
}

//==============================================================================
//          processBlock()
//
//  CasualNoises    13/05/2021  First implementation
//  CasualNoises    21/05/2021  Trigger mode implemented
//==============================================================================
void WaverAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    jassert(buffer.getNumSamples() == mLocalWaverData.numSamples);
    
    // Save info
    mLocalWaverData.numSamples  = buffer.getNumSamples();
    mLocalWaverData.numChannels = buffer.getNumChannels();
    if (buffer.getNumChannels() == 0) return;
    mLocalWaverData.RMSLevel = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    if (buffer.getNumChannels() > 1) {
        mLocalWaverData.RMSLevel += buffer.getRMSLevel(1, 0, buffer.getNumSamples());
        mLocalWaverData.RMSLevel /= 2;
    }
    
    // Abort sampling?
    if (mLocalWaverData.UI_AP_AbortSampling) {
        resetWaverData(false);
        mLocalWaverData.AP_UI_SamplingAborted = true;
        mLocalWaverData.numSamples  = buffer.getNumSamples();
        mLocalWaverData.numChannels = buffer.getNumChannels();
    }
 
    // Ready to start sampling
    if (mLocalWaverData.UI_AP_StartSampling) {
        mLocalWaverData.UI_AP_StartSampling      = false;
        mLocalWaverData.UI_AP_AbortSampling      = false;
        mLocalWaverData.AP_UI_SamplingInProgress = true;
        mLocalWaverData.bufferFillIndex          = 0;
        mLocalWaverData.noOfAvailableSamples     = 0;
        mLocalWaverData.firstSamplePosition      = 0;
        mLocalWaverData.AP_UI_TotalMinInputLevel =  9.0f;
        mLocalWaverData.AP_UI_TotalMaxInputLevel = -9.0f;
    }
    
    // Wait for level event
    auto readerIndex = 0;
    const float* readPtrL = buffer.getReadPointer(0);
    const float* readPtrR = buffer.getReadPointer(1);
    if (mLocalWaverData.UI_AP_WaitForLevelEvent) {
        mLocalWaverData.AP_UI_SamplingInProgress = false;
        
        // Make room for more samples
        if (mTriggerBufferIndex >= mTriggerBufferSize) {
            auto cnt = mTriggerBufferSize - mLocalWaverData.numSamples;
            for (auto i = 0; cnt > 0; i++, cnt--) {
                mTriggerBuffer[i] =  mTriggerBuffer[i + mLocalWaverData.numSamples];
            }
            mTriggerBufferIndex = mTriggerBufferSize - mLocalWaverData.numSamples;               // cnt;
        }
        
        // Copy incomming samples into trigger buffer
        for (auto i = 0; i < mLocalWaverData.numSamples; i++, readerIndex++) {
            mTriggerBuffer[mTriggerBufferIndex++] =  (readPtrL[readerIndex] + readPtrR[readerIndex]) / 2;
        }
        
        // Search for trigger point in trigger buffer
        if (mTriggerBufferIndex == mTriggerBufferSize) {
            
            // No trigger level set...
            auto triggerFound = false;
            double level = mLocalWaverData.UI_AP_LevelSliderValue / 100.0f;
            if (level <= 0.02f) {
                triggerFound = true;
                auto i = 0;
                for (; i < mTriggerBufferIndex; i++) {
                    mLocalWaverData.buffer[i] = mTriggerBuffer[i];
                }
                mTriggerBufferIndex = 0;
                mLocalWaverData.AP_UI_SamplingInProgress = true;
                mLocalWaverData.UI_AP_WaitForLevelEvent  = false;
                mLocalWaverData.noOfAvailableSamples     = i;
                mLocalWaverData.bufferFillIndex          = i;
            
            // Search trigger buffer...
            } else {
                
                // Look for correct slope...
                auto numSamples = mLocalWaverData.numSamples;
                auto start = numSamples / 2;
                auto end = start + (mTriggerBufferSize - numSamples);
                auto i = start;
                auto minInputLevel =  9.0f;
                auto maxInputLevel = -9.0f;
                for (; i < end; i++) {
                    if (mTriggerBuffer[i] < minInputLevel) minInputLevel = mTriggerBuffer[i];
                    if (mTriggerBuffer[i] > maxInputLevel) maxInputLevel = mTriggerBuffer[i];
                    if (mLocalWaverData.UI_AP_UsePositiveLevel) {
                        if ((mTriggerBuffer[i] >=  level) &&
                            (mTriggerBuffer[i + 1] >= mTriggerBuffer[i]) &&
                            (mTriggerBuffer[i + 2] >= mTriggerBuffer[i + 1])) {
                            triggerFound = true;
                            break;
                            }
                    } else {
                        if ((mTriggerBuffer[i] <=  -level) &&
                            (mTriggerBuffer[i + 1] <= mTriggerBuffer[i]) &&
                            (mTriggerBuffer[i + 2] <= mTriggerBuffer[i + 1])) {
                            triggerFound = true;
                            break;
                        }
                    }
                }
                mLocalWaverData.AP_UI_MinInputLevel = minInputLevel;
                mLocalWaverData.AP_UI_MaxInputLevel = maxInputLevel;
                
                // Copy trigger buffer in sample buffer when trigger found
                if (triggerFound) {
                    i -= start;
                    auto cnt = mTriggerBufferSize - i;
                    for (auto j = 0; j < cnt; i++, j++) {
                        mLocalWaverData.buffer[j] = mTriggerBuffer[i];
                    }
                    mTriggerBufferIndex = 0;
                    mLocalWaverData.AP_UI_SamplingInProgress = true;
                    mLocalWaverData.UI_AP_WaitForLevelEvent  = false;
                    mLocalWaverData.bufferFillIndex          = cnt;
                    mLocalWaverData.noOfAvailableSamples     = cnt;
                    mLocalWaverData.firstSamplePosition      = start;
                }
                
            }
        }
    }
    
    // Get next audio sample pack, if we are sampling
    if (mLocalWaverData.AP_UI_SamplingInProgress) {
        
        if (mLocalWaverData.UI_AP_AbortSampling) {
            mLocalWaverData.UI_AP_AbortSampling      = false;
            mLocalWaverData.AP_UI_SamplingInProgress = false;
            mLocalWaverData.UI_AP_WaitForLevelEvent  = false;
            mLocalWaverData.UI_AP_ContiguousMode     = false;
        } else {
            auto noOfSamples = buffer.getNumSamples();
            auto minInputLevel =  9.0f;
            auto maxInputLevel = -9.0f;
            for (; readerIndex < noOfSamples; readerIndex++, mLocalWaverData.bufferFillIndex++) {
                if (mLocalWaverData.bufferFillIndex >= mLocalWaverData.audioBufferSize) {
                    if (mLocalWaverData.UI_AP_ContiguousMode) {
                        mLocalWaverData.bufferFillIndex      = 0;
                        mLocalWaverData.noOfAvailableSamples = mLocalWaverData.audioBufferSize;
                    } else {
                        mLocalWaverData.AP_UI_SamplingInProgress = false;
                        break;
                    }
                }
                mLocalWaverData.buffer[mLocalWaverData.bufferFillIndex] =
                            (readPtrL[readerIndex] + readPtrR[readerIndex]) / 2;
                if (mLocalWaverData.buffer[mLocalWaverData.bufferFillIndex] < minInputLevel)
                    minInputLevel = mLocalWaverData.buffer[mLocalWaverData.bufferFillIndex];
                if (mLocalWaverData.buffer[mLocalWaverData.bufferFillIndex] > maxInputLevel)
                    maxInputLevel = mLocalWaverData.buffer[mLocalWaverData.bufferFillIndex];
            }
            mLocalWaverData.noOfAvailableSamples += readerIndex;
            if (mLocalWaverData.noOfAvailableSamples > mLocalWaverData.audioBufferSize) {
                mLocalWaverData.noOfAvailableSamples = mLocalWaverData.audioBufferSize;
            }
            if (mLocalWaverData.AP_UI_SamplingInProgress) {
                mLocalWaverData.AP_UI_MinInputLevel = minInputLevel;
                mLocalWaverData.AP_UI_MaxInputLevel = maxInputLevel;
            }

            // Update total min's and max
            if (mLocalWaverData.AP_UI_MinInputLevel < mLocalWaverData.AP_UI_TotalMinInputLevel)
                mLocalWaverData.AP_UI_TotalMinInputLevel = mLocalWaverData.AP_UI_MinInputLevel;
            if (mLocalWaverData.AP_UI_MaxInputLevel > mLocalWaverData.AP_UI_TotalMaxInputLevel)
                mLocalWaverData.AP_UI_TotalMaxInputLevel = mLocalWaverData.AP_UI_MaxInputLevel;

        }
    } else
    
    // Not sampling, only measure min and max input levels
    {
        auto minInputLevel =  9.0f;
        auto maxInputLevel = -9.0f;
        for (auto readerIndex = 0; readerIndex < mLocalWaverData.numSamples; readerIndex++) {
            auto level = (readPtrL[readerIndex] + readPtrR[readerIndex]) / 2;
            if (level < minInputLevel) minInputLevel = level;
            if (level > maxInputLevel) maxInputLevel = level;
        }
        mLocalWaverData.AP_UI_MinInputLevel = minInputLevel;
        mLocalWaverData.AP_UI_MaxInputLevel = maxInputLevel;
    }
       
}

//==============================================================================
void WaverAudioProcessor::getStateInformation (juce::MemoryBlock& /*destData*/)
{
}

void WaverAudioProcessor::setStateInformation (const void* /*data*/, int /*sizeInBytes*/)
{
}

//==============================================================================
const juce::String WaverAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WaverAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WaverAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WaverAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WaverAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WaverAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WaverAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WaverAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WaverAudioProcessor::getProgramName (int index)
{
    return {};
}

void WaverAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaverAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

//==============================================================================
bool WaverAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WaverAudioProcessor::createEditor()
{
    return new WaverAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaverAudioProcessor();
}
