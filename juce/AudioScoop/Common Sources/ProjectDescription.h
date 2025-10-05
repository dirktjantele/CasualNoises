/*
  ==============================================================================

    ProjectDescription.h
    Created: 22 Apr 2021 2:55:49pm
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
//  MIDI Note names   (m = '-', p = '+', s = '#')
//
//  CasualNoises    22/04/2021  First implementation
//==============================================================================
typedef enum t_MIDI_NoteNameNo { Nan = 0,
    cm2, cm2s, dm2, dm2s, em2, fm2, fm2s, gm2, gm2s, am2, am2s, bm2,
    cm1, cm1s, dm1, dm1s, em1, fm1, fm1s, gm1, gm1s, am1, am1s, bm1,
    c0,  c0s,  d0,  d0s,  e0,  f0,  f0s,  g0,  g0s,  a0,  a0s,  b0,
    cp1, cp1s, dp1, dp1s, ep1, fp1, fp1s, gp1, gp1s, ap1, ap1s, bp1,
    cp2, cp2s, dp2, dp2s, ep2, fp2, fp2s, gp2, gp2s, ap2, ap2s, bp2,
    cp3, cp3s, dp3, dp3s, ep3, fp3, fp3s, gp3, gp3s, ap3, ap3s, bp3,
    cp4, cp4s, dp4, dp4s, ep4, fp4, fp4s, gp4, gp4s, ap4, ap4s, bp4,
    cp5, cp5s, dp5, dp5s, ep5, fp5, fp5s, gp5, gp5s, ap5, ap5s, bp5,
    cp6, cp6s, dp6, dp6s, ep6, fp6, fp6s, gp6, gp6s, ap6, ap6s, bp6,
    cp7, cp7s, dp7, dp7s, ep7, fp7, fp7s, gp7, gp7s, ap7, ap7s, bp7,
    cp8, cp8s, dp8, dp8s, ep8, fp8, fp8s, gp8
} t__MIDI_NoteNameNo;

//==============================================================================
//  Category names (taken from the Wavestate)
//
//  CasualNoises    22/04/2021  First implementation
//==============================================================================
typedef enum t_CategoryNo { INITIAL,
                            Attack,
                            Synth,
                            WaveForm,
                            BellMallet,
                            BrassEns,
                            BrassSolo,
                            StringEns,
                            StringSolo,
                            Woodwind,
                            AcPiano,
                            EPiano,
                            Keyboard,
                            Organ,
                            Vocal,
                            AcGuitar,
                            EGuitar,
                            GuitarFX,
                            Plucked,
                            AcBass,
                            EBass,
                            BassFX,
                            Hits,
                            Kick,
                            Snare,
                            Drum,
                            Cymbal,
                            Percussion,
                            Effects,
                            WaveTable,
                            CasualNoises
} t__CategoryNo;

//==============================================================================
//  Structure of the global project info
//
//  CasualNoises    24/04/2021  First implementation
//==============================================================================
typedef struct t_GlobalProjectDataStruct {
    
    // Sample data
    t__CategoryNo       categoryNo;
    juce::int16         noOfMultiSampleParts;
    
    // UI related
    juce::int16         focusedPartNo;

} t__GlobalProjectDataStruct;

//==============================================================================
//  Structure of a multi-sample part
//
//  CasualNoises    25/04/2021  First implementation
//  CasualNoises    07/05/2021  Pitched samples introduced
//
// For non-pitched samples, the sample will be played with a increment of 1 in the
//      sample data, so, a sample of 256 words will give a frequancy of
//          44,100Hz / 256  ->  175.265625Hz (+/- F2 at 174.614Hz))
//==============================================================================
#define     PART_NAME_LENGTH        32
typedef struct t_MultiSamplePartStruct {

    // Sample data
    juce::int16         partNo;
    t__MIDI_NoteNameNo  startNote;          // Defines the MIDI note range for the multi-samples
    t__MIDI_NoteNameNo  endNote;            // Range includes both note no's
    bool                isPitched;
    bool                isWaveTable;        // Used for wavetables      *TODO*
    double              noOfCycles;         // Used to derive the reference note
    t__MIDI_NoteNameNo  referenceNote;      // Note played when using corrected sample increment
    double              pitchCorrection;    // Used to play a sample at the reference picth
    juce::int16         waveTableOffset;    // Used for wavetables      *TODO*
    juce::int64         reserved_1;
    
    // Settings for the sample points editor
    juce::int32         sampleStartPoint;
    juce::int32         sampleEndPoint;
    juce::int32         loopStartPoint;
    juce::int32         loopEndPoint;
    juce::int32         focusPoint;
    juce::int64         reserved_2;

    // UI related
    double              scoopGainSetting;
    double              scoopScaleSetting;
    double              scoopOffsetSetting;
    t__MIDI_NoteNameNo  notePlaying;
    juce::int64         reserved_3;
    
    juce::int8          partName[PART_NAME_LENGTH];   // Used to export a part

} t__MultiSamplePartStruct;

//==============================================================================
class ProjectDescription
{
public:
    
    static t__GlobalProjectDataStruct* getGlobalProjectDataStruct();
    static t__MultiSamplePartStruct* getMultiSamplePartStruct(int index);
    
    static void         updateSampleSourceFilePathName(juce::String inSampleSourceFilePathName);
    static void         updateProjectDescriptionPathName(juce::String inProjectPathName);
    static void         updateProjectDescriptionFileName(juce::String inProjectFileName);
    static void         updateProjectDescriptionFilePathName(juce::String inProjectFilePathName); 
    static juce::String getProjectDescriptionFilePathName(bool inCreateFolderFlag);

    static bool         projectDescriptionFileExists(bool inCreateFolderFlag);
    
    static void         loadProjectDescription(bool inCreateFolderFlag);
    static void         saveProjectDescription(bool inCreateFolderFlag, bool inCopySampleFlag);
    
    static bool         isProjectDescriptionDurty();
    
    static juce::String GetCategoryName(t__CategoryNo inCategoryNo);
    static juce::String GetMIDI_NoteName(t__MIDI_NoteNameNo inMIDI_NoteName);
    
    static void         createBlankProject();
    static void         deletePartFromProjectDescription(int index);
    static void         insertPartAfterProjectDescription(int index);

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectDescription)
};
