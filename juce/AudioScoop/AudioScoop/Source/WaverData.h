/*
  ==============================================================================

    WaverData.h
    Created: 13 May 2021 12:13:30pm
    Author:  Dirk Tjantele

  ==============================================================================
*/

typedef struct t_WaverDataStruct {
    
    // Working buffer
    long        maxAudioBufferSize;         // Buffer size
    long        audioBufferSize;            // Buffer size
    float*      buffer;                     // Pointer to the buffer
    
    // Info from DAW
    int         numSamples;                 // Size of a buffer
    int         numChannels;
    float       RMSLevel;
    
    // Filling the buffer
    long        bufferFillIndex;
    long        noOfAvailableSamples;       // In contiguous mode this is not set to 0 after one cycle
    long        firstSamplePosition;        // Position of first sample in trigger mode
    
    // Process flags
    bool        UI_AP_StartSampling;
    bool        UI_AP_AbortSampling;
    bool        AP_UI_SamplingAborted;
    bool        AP_UI_SamplingInProgress;
    bool        UI_AP_WaitForLevelEvent;
    bool        UI_AP_TriggerMode;
    bool        UI_AP_ContiguousMode;

    // Used during trigger mode
    double      UI_AP_LevelSliderValue;
    bool        UI_AP_UsePositiveLevel;
    
    // Contigous measured
    double      AP_UI_MinInputLevel;        // Calculated for each incomming buffer
    double      AP_UI_MaxInputLevel;
    double      AP_UI_TotalMinInputLevel;   // Calculated when sampling
    double      AP_UI_TotalMaxInputLevel; 
    
    double      AP_UI_SampleRate;

} t__WaverDataStruct;

#pragma once
