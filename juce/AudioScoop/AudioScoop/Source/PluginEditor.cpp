/*
  ==============================================================================

    PluginEditor.cpp
    Created: 13 May 2021 11:21      (Tevka support)
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "../../Common Sources/FileTypes.h"
#include "../../Common Sources/ProjectDescription.h"

//==============================================================================
//          WaverAudioProcessorEditor()     /       ~WaverAudioProcessorEditor
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
WaverAudioProcessorEditor::WaverAudioProcessorEditor (WaverAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      mSamplerControlComponent  (this),
      mSamplerProgressComponent (this),
      mSamplerScoopComponent    (this),
      mSamplerInfoComponent     (this),
      mSaveSamplerDataComponent (this)
{
    
    int width  = SamplerScoopComponent::s_getPreferredWidth() +
                 SamplerControlComponent::s_getPreferredWidth() +
                 (cMainX_Indent * 3);
    int height = SamplerScoopComponent::s_getPreferredHeight() +
                 (cMainY_Indent * 2);
    setSize (width, height);
    
    startTimer (100);

    addAndMakeVisible(&mSamplerScoopComponent);
    addAndMakeVisible(&mSamplerControlComponent);
    addAndMakeVisible(&mSamplerProgressComponent);
    addAndMakeVisible(&mSamplerInfoComponent);
    addAndMakeVisible(&mSaveSamplerDataComponent);
    addAndMakeVisible(&mCasualNoisesComponent);

}

WaverAudioProcessorEditor::~WaverAudioProcessorEditor()
{
}

//==============================================================================
//          timerCallback()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void WaverAudioProcessorEditor::timerCallback ()
{
}

//==============================================================================
//          paint()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void WaverAudioProcessorEditor::paint (juce::Graphics& /*g*/)
{
}

//==============================================================================
//          resized()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void WaverAudioProcessorEditor::resized()
{
    
    mSamplerScoopComponent.setBounds(cMainX_Indent, cMainY_Indent,
                                     SamplerScoopComponent::s_getPreferredWidth(),
                                     SamplerScoopComponent::s_getPreferredHeight());
    
    auto x = SamplerScoopComponent::s_getPreferredWidth() + (cMainX_Indent * 2);
    auto y = cMainY_Indent;
    auto ys = y * 2;
    
    y = ys;
    mSamplerInfoComponent.setBounds(x, y,
                                    SamplerInfoComponent::s_getPreferredWidth(),
                                    SamplerInfoComponent::s_getPreferredHeight());
    y += ys + SamplerInfoComponent::s_getPreferredHeight();
    
    mSamplerControlComponent.setBounds(x, y,
                                      SamplerControlComponent::s_getPreferredWidth(),
                                      SamplerControlComponent::s_getPreferredHeight());
    y += ys + SamplerControlComponent::s_getPreferredHeight();
    
    mSamplerProgressComponent.setBounds(x, y,
                                        SamplerProgressComponent::s_getPreferredWidth(),
                                        SamplerProgressComponent::s_getPreferredHeight());
    y += ys + SamplerProgressComponent::s_getPreferredHeight();
    
    mSaveSamplerDataComponent.setBounds(x, y,
                                        SaveSamplerDataComponent::s_getPreferredWidth(),
                                        SaveSamplerDataComponent::s_getPreferredHeight());
    y += ys + SaveSamplerDataComponent::s_getPreferredHeight();
    
    mCasualNoisesComponent.setBounds(x, y,
                                     CasualNoisesComponent::s_getPreferredWidth(),
                                     CasualNoisesComponent::s_getPreferredHeight());
    
}

//==============================================================================
//          SaveSamplerDataComponent()     /       ~SaveSamplerDataComponent
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
static juce::String sTargetFileName   { "-" };
static juce::String sTargetFolderName { "-" };

SaveSamplerDataComponent::SaveSamplerDataComponent (WaverAudioProcessorEditor* ptr)
    : mEditorPtr (ptr),
      mSelectFolderButton ( "Select" ),
      mSaveSampleButton ( "Save")
{
    
    mNameTextLabel.setText("File:", juce::dontSendNotification);
    mNameTextLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(&mNameTextLabel);
    
    mNameEditLabel.setEnabled(true);
    mNameEditLabel.setEditable(true);
    mNameEditLabel.onTextChange = [this] { handleNameEditLabelChange(); };
    mNameEditLabel.setText(sTargetFileName, juce::sendNotification);
    mNameEditLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&mNameEditLabel);

    if (sTargetFolderName != "-") {
        juce::File current(sTargetFolderName);
        if (!current.exists()) {
            sTargetFolderName = (juce::File::getSpecialLocation(juce::File::userDesktopDirectory)).getFullPathName();
        }
    } else {
        sTargetFolderName = (juce::File::getSpecialLocation(juce::File::userDesktopDirectory)).getFullPathName();
    }
    
    mFolderTextLabel.setText("Folder:", juce::dontSendNotification);
    mFolderTextLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(&mFolderTextLabel);
    
    mFolderNameLabel.setText(sTargetFolderName, juce::dontSendNotification);
    mFolderNameLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&mFolderNameLabel);
    
    mSelectFolderButton.onClick = [this]{ handleSelectFolderButtonClicked(); };
    mSelectFolderButton.setEnabled(true);
    addAndMakeVisible(&mSelectFolderButton);

    mSaveSampleButton.onClick = [this]{ handleSaveSampleButtonClicked(); };
    mSaveSampleButton.setEnabled(true);
    addAndMakeVisible(&mSaveSampleButton);

    startTimer (100);

}

SaveSamplerDataComponent::~SaveSamplerDataComponent()
{
}

//==============================================================================
//          timerCallback()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void SaveSamplerDataComponent::timerCallback ()
{
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    bool canSave = ((sTargetFileName != "-") &&
                    (!dataPtr->AP_UI_SamplingInProgress) &&
                    (dataPtr->noOfAvailableSamples > 0));
    
    if (sTargetFileName != "-") {
        juce::String pathFileName = sTargetFolderName + "/" + sTargetFileName + ".aif";
        juce::File current(pathFileName);
        mSaveSampleButton.setEnabled(canSave);
        if (current.exists()) mSaveSampleButton.setButtonText("Replace");
        else                  mSaveSampleButton.setButtonText("Save");
    } else {
        mSaveSampleButton.setEnabled(false);
    }
    
    
}

//==============================================================================
//          handleNameEditLabelChange()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SaveSamplerDataComponent::handleNameEditLabelChange()
{
    sTargetFileName = mNameEditLabel.getText();
}

//==============================================================================
//          handleSelectFolderButtonClicked()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SaveSamplerDataComponent::handleSelectFolderButtonClicked()
{
    juce::FileChooser chooser("Select destination folder", sTargetFolderName, "");
//    chooser.browseForDirectory();
    juce::File dir = chooser.getResult();
    sTargetFolderName = dir.getFullPathName();
    mFolderNameLabel.setText(sTargetFolderName, juce::dontSendNotification);
}

//==============================================================================
//          createCharmProject()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SaveSamplerDataComponent::createCharmProject()
{

    // Init project description
    ProjectDescription::updateProjectDescriptionPathName(sTargetFolderName);
    ProjectDescription::updateProjectDescriptionFileName(sTargetFileName);
    ProjectDescription::createBlankProject();

    // Prepare for use
    t__GlobalProjectDataStruct* globals = ProjectDescription::getGlobalProjectDataStruct();
    globals->categoryNo = t__CategoryNo::CasualNoises;
    auto initialSize = globals->noOfMultiSampleParts;
    while (initialSize > 1) {
        ProjectDescription::deletePartFromProjectDescription(0);
        initialSize = globals->noOfMultiSampleParts;
    }
    
    // Fill in the wave description
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    juce::int32 sampleStartPoint = 0;
    juce::int32 sampleEndPoint = (juce::int32)dataPtr->noOfAvailableSamples - 1;
    t__MultiSamplePartStruct* part = ProjectDescription::getMultiSamplePartStruct(0);
    part->partNo             = 0;
    part->startNote          = t__MIDI_NoteNameNo::cm2;
    part->endNote            = t__MIDI_NoteNameNo::fp8s;
    part->isPitched          = false;
    part->isWaveTable        = false;
    part->noOfCycles         = 1.0f;
    part->referenceNote      = t__MIDI_NoteNameNo::fp3;
    part->pitchCorrection    = 0.99655;
    part->waveTableOffset    = 0;
    part->sampleStartPoint   = sampleStartPoint;
    part->sampleEndPoint     = sampleEndPoint;
    part->loopStartPoint     = part->sampleStartPoint;
    part->loopEndPoint       = part->sampleEndPoint;
    part->focusPoint         = (sampleStartPoint + sampleEndPoint) / 2;
    part->scoopGainSetting   = 1;
    part->scoopScaleSetting  = 40;
    part->scoopOffsetSetting = part->focusPoint;
        
    // Save the resulting project
    globals->focusedPartNo = 0;
    ProjectDescription::saveProjectDescription(false, false);

}

//==============================================================================
//          createAudioFile()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SaveSamplerDataComponent::createAudioFile()
{
    
    // Create '.aif' file
    juce::String pathFileName = sTargetFolderName + "/" + sTargetFileName + ".aif";
    juce::File file(pathFileName);
    if (file.exists()) {
        file.deleteFile();
    }
    file.create();

    // Prepare some stuff...
    juce::FileOutputStream* output = new juce::FileOutputStream(file);
    juce::AiffAudioFormat format;
    juce::AudioFormatWriter* writer = format.createWriterFor(output, 44100.0f, 1, 24, {}, 0);
    juce::AudioBuffer<float> buffer;
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    buffer.setSize(1, (int)dataPtr->noOfAvailableSamples);
    
    // Fill the buffer
    float* ptr = buffer.getWritePointer(0);
    for (auto i = 0; i < buffer.getNumSamples(); i++) {
        ptr[i] = dataPtr->buffer[i];
    }
    
    // Write buffer to file
    writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());

    // We no longer need these guy's
    delete writer;

}

//==============================================================================
//          handleSaveSampleButtonClicked()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SaveSamplerDataComponent::handleSaveSampleButtonClicked()
{
    createCharmProject();
    createAudioFile();
}

//==============================================================================
//          paint()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SaveSamplerDataComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<int> rect = getBounds();

    g.setColour (juce::Colours::white);
    g.drawRoundedRectangle(0.0f, 0.0f, 
				(float)rect.getWidth(), (float)rect.getHeight(), cCornerSize, 1.0f);
    
}

//==============================================================================
//          resized()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SaveSamplerDataComponent::resized()
{
    juce::Rectangle<int> rect = getBounds();
    
    auto x1 = cMainX_Indent;
    auto x2 = x1 + cMainX_Indent + cLabelWidthRegular;
    auto y = cMainY_Indent;
    
    mNameTextLabel.setBounds(x1, y, cLabelWidthRegular, cLabelHeight);
    mNameEditLabel.setBounds(x2, y, cLabelWidthRegular, cLabelHeight);
    y += cLabelHeight;
    
    auto w = rect.getWidth() - (cMainX_Indent * 2);
    mFolderTextLabel.setBounds(x1, y, cLabelWidthRegular, cLabelHeight);
    y += cLabelHeight;
    mFolderNameLabel.setBounds(x1, y, w, cLabelHeight);
    y += cLabelHeight;
    
    mSelectFolderButton.setBounds(x1, y, cButtonWidthRegular, cLabelHeight);
    x2 = rect.getWidth() - cButtonWidthRegular - x1;
    mSaveSampleButton.setBounds(x2, y, cButtonWidthRegular, cLabelHeight);
    
}

//==============================================================================
//          SamplerControlComponent()     /       ~SamplerControlComponent
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
SamplerControlComponent::SamplerControlComponent (WaverAudioProcessorEditor* ptr)
    : mEditorPtr (ptr)
{
    
    mSamplerModeComboBox.addItem("Single Shot", t__SamplerModes::sm_SingleShot);
    mSamplerModeComboBox.addItem("Triggered",   t__SamplerModes::sm_Triggered);
    mSamplerModeComboBox.addItem("Contiguous",  t__SamplerModes::sm_Contiguous);
    mSamplerModeComboBox.setSelectedId(t__SamplerModes::sm_SingleShot);
    mSamplerModeComboBox.onChange = [this] { handleSamplerModeComboBoxChange(); };
    addAndMakeVisible(&mSamplerModeComboBox);
    mSamplerMode = sm_SingleShot;

    mStartSamplingButton.setButtonText("Start");
    mStartSamplingButton.onClick = [this]{ handleStartSamplingButtonClicked(); };
    addAndMakeVisible(&mStartSamplingButton);

    mStopSamplingButton.setButtonText("Stop");
    mStopSamplingButton.onClick = [this]{ handleStopSamplingButtonClicked(); };
    addAndMakeVisible(&mStopSamplingButton);
    
    mTriggerLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false,
                                        cSliderTextWidthRegular, cSliderTextHeightRegular);
    juce::Range<double> range = { 1.0f, 100.0f };
    mTriggerLevelSlider.setRange(range, 1.0f);
    mTriggerLevelSlider.addListener(this);
    mTriggerLevelSlider.setValue(0.0f);
    addChildComponent(&mTriggerLevelSlider);
    
    mTriggerLevel.setText("-", juce::dontSendNotification);
    mTriggerLevel.setJustificationType(juce::Justification::centred);
    addChildComponent(&mTriggerLevel);
    
    mContiguousTriggeredModeCheckBox.onClick = [this] { handleContiguousTriggeredModeCheckBoxClicked(); };
    mContiguousTriggeredModeCheckBox.setColours(juce::Colours::white, juce::Colours::white);
    addChildComponent(&mContiguousTriggeredModeCheckBox);
    handleContiguousTriggeredModeCheckBoxClicked();
    
    mTriggeredPolarityCheckBox.onClick = [this] { handleTriggeredPolarityCheckBoxClicked(); };
    mTriggeredPolarityCheckBox.setColours(juce::Colours::white, juce::Colours::white);
    addChildComponent(&mTriggeredPolarityCheckBox);
    handleTriggeredPolarityCheckBoxClicked();

    addAndMakeVisible(&mSamplerStateIndication);
    
    startTimer (100);

}

SamplerControlComponent::~SamplerControlComponent()
{
}

//==============================================================================
//          handleContiguousTriggeredModeCheckBoxClicked()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SamplerControlComponent::handleContiguousTriggeredModeCheckBoxClicked()
{
    mSamplingStarted = false;
    mContigousTriggeredMode = mContiguousTriggeredModeCheckBox.isChecked();
    if (mContigousTriggeredMode)
        mContiguousTriggeredModeCheckBox.setCheckBoxContents(t_checked);
    else
        mContiguousTriggeredModeCheckBox.setCheckBoxContents(t_blank);
}

//==============================================================================
//          handleTriggeredPolarityCheckBoxClicked()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void SamplerControlComponent::handleTriggeredPolarityCheckBoxClicked()
{
    mPositivePolarity = !mTriggeredPolarityCheckBox.isChecked();
    if (mPositivePolarity) {
        mTriggeredPolarityCheckBox.setColours(juce::Colours::white, juce::Colours::red);
        mTriggeredPolarityCheckBox.setCheckBoxContents(t_plus);
    } else {
        mTriggeredPolarityCheckBox.setColours(juce::Colours::white, juce::Colours::white);
        mTriggeredPolarityCheckBox.setCheckBoxContents(t_minus);
    }
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    dataPtr->UI_AP_UsePositiveLevel = mPositivePolarity;

}
    
//==============================================================================
//          handleStartSamplingButtonClicked()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void SamplerControlComponent::handleStartSamplingButtonClicked()
{
    handleStopSamplingButtonClicked();          // Fake it
    mSamplingStarted = true;
    
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    dataPtr->noOfAvailableSamples = 0;
    
    for (auto i = 0; i < dataPtr->audioBufferSize; i++) {
        dataPtr->buffer[i] = 0.0f;
    }
    
    mSamplerMode = (t__SamplerModes)mSamplerModeComboBox.getSelectedId();
    switch (mSamplerMode) {
        case sm_Triggered:
            dataPtr->UI_AP_StartSampling      = true;
            dataPtr->UI_AP_WaitForLevelEvent  = true;
            break;
        case sm_Contiguous:
            dataPtr->UI_AP_StartSampling      = true;
            dataPtr->UI_AP_ContiguousMode     = true;
            break;
        case sm_SingleShot:
            dataPtr->UI_AP_StartSampling      = true;
            break;
        default:
            jassertfalse;
    };

}

//==============================================================================
//          handleStopSamplingButtonClicked()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void SamplerControlComponent::handleStopSamplingButtonClicked()
{
    mSamplingStarted = false;
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    while (dataPtr->AP_UI_SamplingInProgress) {
        dataPtr->UI_AP_AbortSampling = true;
    }
    dataPtr->UI_AP_WaitForLevelEvent  = false;
    dataPtr->UI_AP_TriggerMode        = false;
    dataPtr->UI_AP_ContiguousMode     = false;
//    mContigousTriggeredMode           = false;
    while (dataPtr->UI_AP_AbortSampling) { }
}

//==============================================================================
//          handleSamplerModeComboBoxChange()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void SamplerControlComponent::handleSamplerModeComboBoxChange()
{
    handleStopSamplingButtonClicked();          // Fake this one
    
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    dataPtr->UI_AP_TriggerMode    = false;
    dataPtr->noOfAvailableSamples = 0;
    
    for (auto i = 0; i < dataPtr->audioBufferSize; i++) {
        dataPtr->buffer[i] = 0.0f;
    }
    
    mTriggerLevelSlider.setVisible(false);
    mTriggerLevel.setVisible(false);
    mContiguousTriggeredModeCheckBox.setVisible(false);
    mTriggeredPolarityCheckBox.setVisible(false);
    mSamplerMode = (t__SamplerModes)mSamplerModeComboBox.getSelectedId();
    switch (mSamplerMode) {
        case t__SamplerModes::sm_SingleShot:
            break;
        case t__SamplerModes::sm_Triggered:
            dataPtr->UI_AP_TriggerMode = true;
            mTriggerLevelSlider.setVisible(true);
            mTriggeredPolarityCheckBox.setVisible(true);
            mTriggerLevel.setVisible(true);
            mContiguousTriggeredModeCheckBox.setVisible(true);
            break;
        case t__SamplerModes::sm_Contiguous:
            break;
    }
    resized();
}

//==============================================================================
//          sliderValueChanged()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void SamplerControlComponent::sliderValueChanged(juce::Slider* inSlider)
{
    jassert(inSlider == &mTriggerLevelSlider);
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    dataPtr->UI_AP_LevelSliderValue = mTriggerLevelSlider.getValue();
    mTriggerLevel.setText(juce::String(dataPtr->UI_AP_LevelSliderValue), juce::dontSendNotification);
}

//==============================================================================
//          timerCallback()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
static t__SamplerStates s_SamplerState = (t__SamplerStates)-1;
void SamplerControlComponent::timerCallback ()
{
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    
    bool finished = (dataPtr->UI_AP_WaitForLevelEvent  == false)
                 && (dataPtr->UI_AP_StartSampling      == false)
                 && (dataPtr->AP_UI_SamplingInProgress == false);

    if (finished) {
        if ((mSamplerModeComboBox.getSelectedId() == t__SamplerModes::sm_Triggered) &&
            mContigousTriggeredMode && mSamplingStarted) {
            dataPtr->UI_AP_StartSampling      = true;
            dataPtr->UI_AP_WaitForLevelEvent  = true;
        } else {
            mSamplerState = st_SamplerReady;
        }
    } else if (dataPtr->UI_AP_WaitForLevelEvent) {
        mSamplerState = st_AwaitingTrigger;
    } else if (dataPtr->AP_UI_SamplingInProgress) {
        mSamplerState = st_SamplerBusy;
    }
    
    if (mSamplerState != s_SamplerState) {
        s_SamplerState = mSamplerState;
        repaint();
    }
}

//==============================================================================
//          paint()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void SamplerControlComponent::paint (juce::Graphics& g)
{

    juce::Rectangle<int> rect = getBounds();

    g.setColour (juce::Colours::white);
    g.drawRoundedRectangle(0.0f, 0.0f, 
			(float)rect.getWidth(), (float)rect.getHeight(), cCornerSize, 1.0f);
    
    switch (mSamplerState) {
        case st_SamplerReady:    g.setColour (juce::Colours::green);
            break;
        case st_SamplerBusy:     g.setColour (juce::Colours::blue);
            break;
        case st_AwaitingTrigger: g.setColour (juce::Colours::red);
            break;
        default:
            jassertfalse;
    }
    g.fillEllipse((float)mSamplerStateIndication.getX(),     (float)mSamplerStateIndication.getY(),
				  (float)mSamplerStateIndication.getWidth(), (float)mSamplerStateIndication.getHeight());
  
}

//==============================================================================
//          resized()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void SamplerControlComponent::resized()
{
    juce::Rectangle<int> rect = getBounds();

    mSamplerModeComboBox.setBounds(cMainX_Indent, cMainY_Indent, cComboBoxWidthRegular, cComboBoxheight);
    
    mStartSamplingButton.setBounds(cComboBoxWidthRegular + (cMainX_Indent * 2), cMainY_Indent, cButtonWidthRegular, cButtonHeightRegular);
    mStopSamplingButton.setBounds(cComboBoxWidthRegular + (cMainX_Indent * 2),
                                  cButtonHeightRegular + (cMainY_Indent * 2),
                                  cButtonWidthRegular, cButtonHeightRegular);
    if (mSamplerModeComboBox.getSelectedId() == sm_Triggered) {
        mStartSamplingButton.setBounds(mStartSamplingButton.getBounds().getX(),
                                       mStartSamplingButton.getBounds().getY() + cButtonHeightRegular + cMainY_Indent,
                                       mStartSamplingButton.getBounds().getWidth(),
                                       mStartSamplingButton.getBounds().getHeight());
        mStopSamplingButton.setBounds(mStartSamplingButton.getBounds().getX() + cButtonWidthRegular + cMainX_Indent,
                                      mStartSamplingButton.getBounds().getY(),
                                      mStopSamplingButton.getBounds().getWidth(),
                                      mStopSamplingButton.getBounds().getHeight());
        mTriggerLevelSlider.setBounds(mStartSamplingButton.getBounds().getX(),
                                      cMainY_Indent,
                                      cComboBoxWidthRegular,
                                      cComboBoxheight);
        mTriggerLevel.setBounds(cMainX_Indent + cCheckBoxSize + cSpacingX,
                                mStartSamplingButton.getBounds().getY(),
                                cLabelWidthRegular,
                                cLabelHeight);
        mContiguousTriggeredModeCheckBox.setBounds(cMainX_Indent,
                                                   mStartSamplingButton.getBounds().getY() + cButtonHeightRegular - cCheckBoxSize,
                                                   cCheckBoxSize,
                                                   cCheckBoxSize);
        mTriggeredPolarityCheckBox.setBounds(cMainX_Indent,
                                             mStartSamplingButton.getBounds().getY(),
                                             cCheckBoxSize,
                                             cCheckBoxSize);
    }
            
    mSamplerStateIndication.setBounds(rect.getWidth() - cBusyBoxSize - cMainX_Indent, cMainY_Indent, cBusyBoxSize, cBusyBoxSize);

}

//==============================================================================
//          SamplerProgressComponent()     /       ~SamplerProgressComponent
//
//  CasualNoises    15/05/2021  First implementation
//==============================================================================
SamplerProgressComponent::SamplerProgressComponent (WaverAudioProcessorEditor* ptr)
    : mEditorPtr (ptr)
{
    startTimer (100);
}

SamplerProgressComponent::~SamplerProgressComponent()
{
}

//==============================================================================
//          timerCallback()
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
void SamplerProgressComponent::timerCallback ()
{
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    long count = dataPtr->bufferFillIndex;
    if (count != mCurrentProgress) {
        mCurrentProgress = count;
        repaint();
    }
}

//==============================================================================
//          paint()
//
//  CasualNoises    15/05/2021  First implementation
//==============================================================================
void SamplerProgressComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<int> rect = getBounds();

    g.setColour (juce::Colours::white);
    g.drawRoundedRectangle(0.0f, 0.0f, 
		(float)rect.getWidth(), (float)rect.getHeight(), cCornerSize, 1.0f);
    
    g.setColour (juce::Colours::grey);
    g.fillRect(4, 4, rect.getWidth() - 8, rect.getHeight() - 8);
    g.setColour (juce::Colours::lightgreen);
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    if (mCurrentProgress > 0) {
        float tmp = (float)dataPtr->bufferFillIndex / (float)dataPtr->audioBufferSize;
        int fill  = (int)((float)(rect.getWidth() - 8) * tmp);
        g.fillRect(4, 4, fill, rect.getHeight() - 8);
    }
    
    g.setColour (juce::Colours::darkgreen);
    g.drawRect(4, 4, rect.getWidth() - 8, rect.getHeight() - 8, 2);
    
}

//==============================================================================
//          resized()
//
//  CasualNoises    15/05/2021  First implementation
//==============================================================================
void SamplerProgressComponent::resized()
{
}

//==============================================================================
//          SamplerScoopComponent()     /       ~SamplerScoopComponent
//
//  CasualNoises    15/05/2021  First implementation
//==============================================================================
SamplerScoopComponent::SamplerScoopComponent (WaverAudioProcessorEditor* ptr)
    : mEditorPtr (ptr)
{
    mWaveDisplay.resetWaveDisplay();
    addAndMakeVisible(&mWaveDisplay);
    startTimer (100);
}

SamplerScoopComponent::~SamplerScoopComponent()
{
}

//==============================================================================
//          timerCallback()
//
//  CasualNoises    21/05/2021  First implementation
//==============================================================================
void SamplerScoopComponent::timerCallback ()
{
    SamplerControlComponent& cntl = mEditorPtr->getSamplerControlComponent();
    t__WaveDisplaySettings* settings = mWaveDisplay.getDisplaySettings();
    settings->samplerState        = cntl.getSamplerState();
    settings->samplerMode         = cntl.getSamplerMode();
    
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    settings->waveToDisplayPtr    = dataPtr->buffer;
    settings->waveBufferSize      = dataPtr->audioBufferSize;
    settings->waveSampleSize      = dataPtr->bufferFillIndex;
    settings->minInputLevel       = dataPtr->AP_UI_MinInputLevel;
    settings->maxInputLevel       = dataPtr->AP_UI_MaxInputLevel;
    settings->minTotalInputLevel  = dataPtr->AP_UI_TotalMinInputLevel;
    settings->maxTotalInputLevel  = dataPtr->AP_UI_TotalMaxInputLevel;
    settings->triggerLevel        = dataPtr->UI_AP_LevelSliderValue / 100.0f;
    if (!dataPtr->UI_AP_UsePositiveLevel) settings->triggerLevel *= -1.0f;
    settings->firstSamplePosition = dataPtr->firstSamplePosition;
}

//==============================================================================
//          paint()
//
//  CasualNoises    15/05/2021  First implementation
//==============================================================================
void SamplerScoopComponent::paint (juce::Graphics& /*g*/)
{
}

//==============================================================================
//          resized()
//
//  CasualNoises    16/05/2021  First implementation
//  CasualNoises    21/05/2021  WaveDisplay implementation
//==============================================================================
void SamplerScoopComponent::resized()
{
    juce::Rectangle<int> rect = getBounds();
    rect.setX(0);
    rect.setY(0);
    mWaveDisplay.setBounds(rect);
}

//==============================================================================
//          SamplerInfoComponent()     /       ~SamplerInfoComponent
//
//  CasualNoises    16/05/2021  First implementation
//==============================================================================
SamplerInfoComponent::SamplerInfoComponent (WaverAudioProcessorEditor* ptr)
    : mEditorPtr (ptr),
      mWaveBufferSize  ("mWaveBufferSize",  "-"),
      mAudioBufferSize ("mAudioBufferSize", "-"),
      mNoOfChannels    ("mNoOfChannels",    "-")
{
    startTimer (100);
    
    mWaveBufferSizeText.setText("Size:", juce::dontSendNotification);
    mWaveBufferSizeText.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(&mWaveBufferSizeText);
    
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();
    
    auto duration = dataPtr->audioBufferSize / dataPtr->AP_UI_SampleRate;
    juce::String text(duration);
    text += " Sec.";
    mDurationText.setText(text, juce::dontSendNotification);
    mWaveBufferSizeText.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&mDurationText);
    
    mAudioBufferSizeText.setText("Buffer:", juce::dontSendNotification);
    mAudioBufferSizeText.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(&mAudioBufferSizeText);
    
    mNoOfChannelsText.setText("Channels:", juce::dontSendNotification);
    mNoOfChannelsText.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(&mNoOfChannelsText);
    
    mWaveBufferSize.setEditable(true);
    mWaveBufferSize.setEnabled(true);
    mWaveBufferSize.onTextChange = [this] { handleWaveBufferSizeChange(); };
    mWaveBufferSize.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(&mWaveBufferSize);

    mAudioBufferSize.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(&mAudioBufferSize);
    mNoOfChannels.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(&mNoOfChannels);

}

SamplerInfoComponent::~SamplerInfoComponent()
{
}

//==============================================================================
//          handleWaveBufferSizeChange()
//
//  CasualNoises    16/05/2021  First implementation
//==============================================================================
void SamplerInfoComponent::handleWaveBufferSizeChange ()
{
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();

    juce::String text = mWaveBufferSize.getText();
    int value = text.getIntValue();
    if (value != dataPtr->audioBufferSize) {
        value = juce::jmax<int>(value, ((int)dataPtr->numSamples * 2));
        value = juce::jmin<int>(value, (int)dataPtr->maxAudioBufferSize);
        dataPtr->audioBufferSize = value;
        
        auto duration = dataPtr->audioBufferSize / dataPtr->AP_UI_SampleRate;
        juce::String _text(duration);
        _text += " Sec.";
        mDurationText.setText(_text, juce::dontSendNotification);
        
        SamplerControlComponent& control = mEditorPtr->getSamplerControlComponent();
        control.stopSampling();
        
        for (auto i = 0; i < dataPtr->audioBufferSize; i++) {
            dataPtr->buffer[i] = 0.0f;
        }
        
    }
      
}

//==============================================================================
//          timerCallback()
//
//  CasualNoises    16/05/2021  First implementation
//==============================================================================
void SamplerInfoComponent::timerCallback ()
{
    t_WaverDataStruct* dataPtr = mEditorPtr->getLocalWaverData();

    if (mWaveBufferSize.getText().getIntValue() != dataPtr->audioBufferSize) {
        mWaveBufferSize.setText(juce::String(dataPtr->audioBufferSize), juce::sendNotification);
    }
    
    if (mAudioBufferSize.getText().getIntValue() != dataPtr->numSamples) {
        mAudioBufferSize.setText(juce::String(dataPtr->numSamples), juce::dontSendNotification);
    }
 
    if (mNoOfChannels.getText().getIntValue() != dataPtr->numChannels) {
        mNoOfChannels.setText(juce::String(dataPtr->numChannels), juce::dontSendNotification);
    }

}

//==============================================================================
//          paint()
//
//  CasualNoises    16/05/2021  First implementation
//==============================================================================
void SamplerInfoComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<int> rect = getBounds();
    g.setColour (juce::Colours::white);
    g.drawRoundedRectangle(0.0f, 0.0f, 
		(float)rect.getWidth(), (float)rect.getHeight(), cCornerSize, 1.0f);
}

//==============================================================================
//          resized()
//
//  CasualNoises    16/05/2021  First implementation
//==============================================================================
void SamplerInfoComponent::resized()
{
    auto x = cMainX_Indent;
    auto y = cMainY_Indent;
    
    mWaveBufferSizeText.setBounds(x, y, cLabelWidthRegular, cLabelHeight);
    y += cLabelHeight;
    mAudioBufferSizeText.setBounds(x, y, cLabelWidthRegular, cLabelHeight);
    y += cLabelHeight;
    mNoOfChannelsText.setBounds(x, y, cLabelWidthRegular, cLabelHeight);
    
    x += cLabelWidthRegular + cMainX_Indent;
    y = cMainY_Indent;
    mWaveBufferSize.setBounds(x, y, cLabelWidthRegular, cLabelHeight);
    mDurationText.setBounds(x + cLabelWidthRegular + cMainX_Indent, y, cLabelWidthRegular, cLabelHeight);
    y += cLabelHeight;
    mAudioBufferSize.setBounds(x, y, cLabelWidthRegular, cLabelHeight);
    y += cLabelHeight;
    mNoOfChannels.setBounds(x, y, cLabelWidthRegular, cLabelHeight);
    
}

//==============================================================================
//          CasualNoisesComponent()     /       ~CasualNoisesComponent
//
//  CasualNoises    13/05/2021  First implementation
//==============================================================================
CasualNoisesComponent::CasualNoisesComponent ()
{
    mCasualNoisesLabel.setText("CasualNoises", juce::dontSendNotification);
    mCasualNoisesLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&mCasualNoisesLabel);
    startTimer(500);
}

CasualNoisesComponent::~CasualNoisesComponent()
{
}

//==============================================================================
//          timerCallback()
//
//  CasualNoises    22/05/2021  First implementation
//==============================================================================
void CasualNoisesComponent::timerCallback ()
{
    static bool flag = false;
    static juce::String text_1("AudioScoop #0.2");
    static juce::String text_2(" CasualNoises  ");
    static juce::String text_3("");
    static int index = -1;
    mCasualNoisesLabel.setText(text_3, juce::dontSendNotification);
    if (index++ >= text_1.length()) {
        index = 0;
        flag = !flag;
        startTimer(10000);
        text_3 = "";
    } else {
        startTimer(50);
    }
    auto c = text_1[index];
    if (flag) c = text_2[index];
    text_3 += c;
}

//==============================================================================
//          paint()
//
//  CasualNoises    20/05/2021  First implementation
//==============================================================================
void CasualNoisesComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<int> rect = getBounds();
    g.setColour (juce::Colours::white);
    g.drawRoundedRectangle(0.0f, 0.0f, 
		(float)rect.getWidth(), (float)rect.getHeight(), cCornerSize, 1.0f);
}

//==============================================================================
//          resized()
//
//  CasualNoises    16/05/2021  First implementation
//==============================================================================
void CasualNoisesComponent::resized()
{
    juce::Rectangle<int> rect = getBounds();
    mCasualNoisesLabel.setBounds(cSpacingX, cSpacingY,
                                 rect.getWidth()  - (cSpacingX * 2),
                                 rect.getHeight() - (cSpacingY * 2));
}
