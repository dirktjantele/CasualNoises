/*
  ==============================================================================

    ProjectDescription.cpp
    Created: 22 Apr 2021 2:55:49pm
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ProjectDescription.h"

#include "FileTypes.h"

//==============================================================================
//          Static stuff
//==============================================================================

static juce::String         sProjectFileName             = "Untitled";
static juce::String         sProjectPathName             = "...Drop audio file...";
static bool                 sDescriptionExits            = false;
static juce::String         sSampleSourceFilePathName    = "Untitled";

static bool                 sProjectLoaded               = false;
static juce::String         sLastUsedProjectFilePathName = sSampleSourceFilePathName;

static t__GlobalProjectDataStruct  sGlobalProjectDataStruct;
static std::vector<t__MultiSamplePartStruct*> cSamplePartPtrVector;

//==============================================================================
//          getGlobalProjectDataStruct()
//
//  CasualNoises    25/04/2021  First implementation
//==============================================================================
t__GlobalProjectDataStruct* ProjectDescription::getGlobalProjectDataStruct()
{
    return &sGlobalProjectDataStruct;
}

//==============================================================================
//          getMultiSamplePartStruct()
//
//  CasualNoises    25/04/2021  First implementation
//==============================================================================
t__MultiSamplePartStruct* ProjectDescription::getMultiSamplePartStruct(int index)
{
    auto size = cSamplePartPtrVector.size();
    if (index >= size) {
        std::cout << "*** ProjectDescription::getMultiSamplePartStruct(), INVALID INDEX: " << index << "\n";
        return nullptr;
    }
    return cSamplePartPtrVector[index];
}

//==============================================================================
//          updateSampleSourceFilePathName()
//
//  CasualNoises    25/04/2021  First implementation
//==============================================================================
void ProjectDescription::updateSampleSourceFilePathName(juce::String inSampleSourceFilePathName)
{
    sSampleSourceFilePathName = inSampleSourceFilePathName;
}

//==============================================================================
//          updateProjectDescriptionPath()
//
//  CasualNoises    23/04/2021  First implementation
//==============================================================================
void ProjectDescription::updateProjectDescriptionPathName(juce::String inProjectPathName)
{
    sProjectPathName = inProjectPathName;
}

//==============================================================================
//          updateProjectDescriptionFileName()
//
//  CasualNoises    23/04/2021  First implementation
//==============================================================================
void ProjectDescription::updateProjectDescriptionFileName(juce::String inProjectFileName)
{
    sProjectFileName = inProjectFileName;
}

//==============================================================================
//          updateProjectDescriptionFilePathName()
//
//  CasualNoises    24/04/2021  First implementation
//==============================================================================
void ProjectDescription::updateProjectDescriptionFilePathName(juce::String inProjectFilePathName)
{
    juce::File current(inProjectFilePathName);
    sDescriptionExits = current.exists();
    updateProjectDescriptionPathName(current.getParentDirectory().getFullPathName());
    updateProjectDescriptionFileName(current.getFileNameWithoutExtension());
}

//==============================================================================
//          getProjectDescriptionFilePathName()
//
//  CasualNoises    24/04/2021  First implementation
//  CasualNoises    25/04/2021  Use containing folder when exists
//==============================================================================
juce::String ProjectDescription::getProjectDescriptionFilePathName(bool inCreateFolderFlag)
{
    juce::String name = sProjectPathName;
    name += "/";
    if (inCreateFolderFlag) {
        name += sProjectFileName;
        name += "/";
        juce::File current(name);
        if (current.exists() == false) {
            juce::Result res = current.createDirectory();
        }
    } else {
        juce::String folderName = name;
        folderName += sProjectFileName;
        folderName += "/";
        juce::File current(folderName);
        if (current.exists() && current.isDirectory()) {
            name = folderName;
        }
    }
    name += sProjectFileName;
    name += PROJECT_DESCRIPTION_FILE_EXTENSION;
    juce::File current(name);
    sDescriptionExits = current.exists();
    return name;
}
 
//==============================================================================
//          clearCurrentProject()
//
//  Delete any data that is created for previous project
//
//  CasualNoises    25/04/2021  First implementation
//==============================================================================
void clearCurrentProject()
{
    auto size = cSamplePartPtrVector.size();
    for (auto i = 0; i < size; i++) {
        t__MultiSamplePartStruct* ptr = cSamplePartPtrVector[i];
        free(ptr);
    }
    cSamplePartPtrVector.clear();
}

//==============================================================================
//          setDefaultPartSettings()
//
//  CasualNoises    25/04/2021  First implementation
//==============================================================================
void setDefaultPartSettings(t__MultiSamplePartStruct* inPartPtr, int inPartNo)
{
    inPartPtr->partNo               = inPartNo;
    inPartPtr->startNote            = t__MIDI_NoteNameNo::cm2;
    inPartPtr->endNote              = t__MIDI_NoteNameNo::fp8s;
    inPartPtr->isPitched            = true;
    inPartPtr->isWaveTable          = false;
    inPartPtr->noOfCycles           = 1;
    inPartPtr->referenceNote        = inPartPtr->startNote;
    inPartPtr->pitchCorrection      = 0.0f;
    inPartPtr->waveTableOffset      = -1;
    inPartPtr->sampleStartPoint     = -1;
    inPartPtr->sampleEndPoint       = -1;
    inPartPtr->loopStartPoint       = -1;
    inPartPtr->loopEndPoint         = -1;
    inPartPtr->focusPoint           = -1;
    inPartPtr->scoopGainSetting     = 1.0f;
    inPartPtr->scoopScaleSetting    = 1.0f;
    inPartPtr->scoopOffsetSetting   = 0.0f;
    inPartPtr->notePlaying          = t__MIDI_NoteNameNo::ap3;
    inPartPtr->reserved_1           = 0x27111127;
    inPartPtr->reserved_2           = 0x27222227;
    inPartPtr->reserved_3           = 0x27333327;
    
    juce::String partName("CasualNoises");
    auto i = 0;
    for (; i < partName.length(); i++) {
        inPartPtr->partName[i] = partName[i];
    }
    inPartPtr->partName[i] = 0x00;
    
}

//==============================================================================
//          createBlankProject()
//
//  Create a blank project, delete any data that is created for previous project
//
//  CasualNoises    25/04/2021  First implementation
//==============================================================================
const int cInitialNoOfParts = 2;
void ProjectDescription::createBlankProject()
{
    
    // Clear current project
    clearCurrentProject();
    
    // Default project info
    sGlobalProjectDataStruct.categoryNo = t__CategoryNo::INITIAL;
    sGlobalProjectDataStruct.noOfMultiSampleParts = cInitialNoOfParts;
    sGlobalProjectDataStruct.focusedPartNo = 0;
    
    // Create as many parts as required by default
    for (auto i = 0; i < cInitialNoOfParts; i++) {
    
        // Create a single multi-sample part
        auto size = sizeof(t__MultiSamplePartStruct);
        t__MultiSamplePartStruct* ptr = (t__MultiSamplePartStruct*)malloc(size);
        cSamplePartPtrVector.push_back(ptr);
        
        // Fill in some default values here...
        t__MultiSamplePartStruct* partPtr = ProjectDescription::getMultiSamplePartStruct(i);
        setDefaultPartSettings(partPtr, i);
        
    }
    
}

//==============================================================================
//          loadProjectDescription()
//
//  Load the project description if it exists, otherwise create a blank one
//
//  CasualNoises    24/04/2021  First implementation
//==============================================================================
bool ProjectDescription::projectDescriptionFileExists(bool inCreateFolderFlag)
{
    juce::String projectFilePathName = getProjectDescriptionFilePathName(inCreateFolderFlag);
    juce::File current(projectFilePathName);
    return current.exists();
}

//==============================================================================
//          loadProjectDescription()
//
//  Load the project description if it exists, otherwise create a blank one
//
//  CasualNoises    24/04/2021  First implementation
//==============================================================================
void ProjectDescription::loadProjectDescription(bool inCreateFolderFlag)
{
    juce::String projectFilePathName = getProjectDescriptionFilePathName(inCreateFolderFlag);
    
    // Ignore uninitialized project names
    if (projectFilePathName.contains("...Drop audio file...")) {
        std::cout << "*** ProjectDescription::loadProjectDescription, REQUEST DENIED, INVALID PROJECT NAME: " <<
                                            projectFilePathName << "!!!!\n";
        return;
    }
    
    // Load, or create project
    juce::File current(projectFilePathName);
    if (current.exists()) {
        
        // Remember this file for later use
        sProjectLoaded = true;
        sLastUsedProjectFilePathName = projectFilePathName;
        
        // Get rid of all pending data
        clearCurrentProject();
        
        // Get data for this project
        juce::MemoryBlock data;
        bool res = current.loadFileAsData(data);
        if (res == false) {
            std::cout << "*** ProjectDescription::loadProjectDescription(), LOAD FAILED!!!\n";
            return;
        }
        char* dataPtr = (char*)data.getData();
        
        // Update global project data
        auto size = sizeof(sGlobalProjectDataStruct);
        memcpy(&sGlobalProjectDataStruct, dataPtr, size);
        dataPtr += size;
        
        // Get all multi-parts
        auto numOfParts = sGlobalProjectDataStruct.noOfMultiSampleParts;
        while (numOfParts-- > 0) {
            auto size = sizeof(t__MultiSamplePartStruct);
            t__MultiSamplePartStruct* ptr = (t__MultiSamplePartStruct*)malloc(size);
            cSamplePartPtrVector.push_back(ptr);
            memcpy(ptr, dataPtr, size);
            dataPtr += size;
        }

    } else {
        createBlankProject();
    }
    
}

//==============================================================================
//          saveProjectDescription()
//
//  Save the project description
//
//  CasualNoises    25/04/2021  First implementation
//==============================================================================
void ProjectDescription::saveProjectDescription(bool inCreateFolderFlag, bool inCopySampleFlag)
{
    
    // Create the file on disk, overwrite any existing one
    juce::String projectDescriptionFilePathName = getProjectDescriptionFilePathName(inCreateFolderFlag);
    juce::File projectFile(projectDescriptionFilePathName);
    if (projectFile.exists()) {
        projectFile.deleteFile();
    }
    projectFile.create();
    sLastUsedProjectFilePathName = projectDescriptionFilePathName;

    // Write the global project information
    auto size = sizeof(sGlobalProjectDataStruct);
    projectFile.appendData(&sGlobalProjectDataStruct, size);
    
    // Write all muli-parts
    auto noOfMultiSampleParts = sGlobalProjectDataStruct.noOfMultiSampleParts;
    for (auto i = 0; i < noOfMultiSampleParts; i++) {
        auto size = sizeof(t__MultiSamplePartStruct);
        t__MultiSamplePartStruct* partPtr = ProjectDescription::getMultiSamplePartStruct(i);
        projectFile.appendData(partPtr, size);
    }
    
    // Write a stop signal;
    juce::String endStamp("CasualNoises 09/05/2021");
    juce::String::CharPointerType ptr = endStamp.getCharPointer();
    projectFile.appendData(ptr, endStamp.length());
    
    // Copy source sample data
    if (inCopySampleFlag) {
        juce::File source(sSampleSourceFilePathName);
        if (source.exists()) {
            juce::File folder(projectFile.getParentDirectory());
            juce::String destinationName = folder.getFullPathName();
            destinationName += "/";
            destinationName += projectFile.getFileNameWithoutExtension();
            juce::String extension = source.getFileExtension();
            destinationName += extension;
            juce::File destination(destinationName);
            if (destination.exists()) {
                destination.deleteFile();
            }
            bool res = source.copyFileTo(destination);
            if (res == false) {
                std::cout << "*** ProjectDescription::saveProjectDescription(), sample data file copy failed, UNABLE TO COPY!!!\n";
            }
        } else {
            std::cout << "*** ProjectDescription::saveProjectDescription(), sample data file copy failed, SOURCE NOT FOUND!!!\n";
        }
        
    }
    
}

//==============================================================================
//          compareProjectData()
//
//  CasualNoises    29/04/2021  First implementation
//==============================================================================
bool compareProjectData(char* pt1, char* pt2, size_t size)
{
    while (size-- > 0)
        if (*pt1++ != *pt2++)
            return false;
    return true;
}

//==============================================================================
//          isProjectDescriptionDurty()
//
//  Check to see if the actual project data equals the one on disk
//
//  CasualNoises    29/04/2021  First implementation
//==============================================================================
bool ProjectDescription::isProjectDescriptionDurty()
{

    // If no file loaded, consider this project durty
    if (sProjectLoaded == false) {
        return true;
    }

    // If no file exists, consider this project durty
    juce::File projectFile(sLastUsedProjectFilePathName);
    if (projectFile.exists() == false) {
        return true;
    }

    // Compare the gloabal project data
    juce::MemoryBlock data;
    bool res = projectFile.loadFileAsData(data);
    if (res == false) {
        std::cout << "*** ProjectDescription::isProjectDescriptionDurty, LOAD FAILED!!!\n";
        return false;
    }
    char* dataPtr = (char*)data.getData();
    auto size = sizeof(t__GlobalProjectDataStruct);
    if (compareProjectData((char*)(&sGlobalProjectDataStruct), dataPtr, size) == false) {
        return true;
    }
    dataPtr += size;

    // Compare parts one by one
    auto noOfParts = sGlobalProjectDataStruct.noOfMultiSampleParts;
    auto index = 0;
    size = sizeof(t_MultiSamplePartStruct);
    while (noOfParts-- > 0) {
        if (compareProjectData((char*)getMultiSamplePartStruct(index++), dataPtr, size) == false) {
            return true;
        }
        dataPtr += size;
    }

    // All clear
    return false;
    
}
    
//==============================================================================
//          deletePartFromProjectDescription()
//
//  CasualNoises    29/04/2021  First implementation
//==============================================================================
void  ProjectDescription::deletePartFromProjectDescription(int index)
{
    
    // Delete valid?
    if (cSamplePartPtrVector.size() > 0) { 
        
        // Delete part from the vector
        t__MultiSamplePartStruct* ptr = cSamplePartPtrVector[index];
        free(ptr);
        auto it = cSamplePartPtrVector.begin();
        cSamplePartPtrVector.erase(it + index);
        sGlobalProjectDataStruct.noOfMultiSampleParts = cSamplePartPtrVector.size();
        
        // Update focused part no
        if ((sGlobalProjectDataStruct.focusedPartNo == index) ||
            (sGlobalProjectDataStruct.focusedPartNo >= sGlobalProjectDataStruct.noOfMultiSampleParts)) {
            sGlobalProjectDataStruct.focusedPartNo -= 1;
        } 
        
    }
}
    
//==============================================================================
//          insertPartAfterProjectDescription()
//
//  CasualNoises    29/04/2021  First implementation
//  CasualNoises    09/05/2021  Handle empty projects
//==============================================================================
void  ProjectDescription::insertPartAfterProjectDescription(int index)
{
    
    // Insert new part into the vector
    auto size = sizeof(t__MultiSamplePartStruct);
    t__MultiSamplePartStruct* ptr = (t__MultiSamplePartStruct*)malloc(size);
    setDefaultPartSettings(ptr, -1);
    auto it = cSamplePartPtrVector.begin();
    cSamplePartPtrVector.insert(it + index + 1, ptr);
    sGlobalProjectDataStruct.noOfMultiSampleParts = cSamplePartPtrVector.size();
    
    // Make a copy of previous part, if any
    if (index >= 0) {
        t__MultiSamplePartStruct* sourcePtr = getMultiSamplePartStruct(index);
        memcpy(ptr, sourcePtr, size);
    }
    
    // Update focused part no
    sGlobalProjectDataStruct.focusedPartNo = index + 1;
    
}
    
//==============================================================================
//          GetCategoryName()
//
//  CasualNoises    07/04/2021  First implementation
//==============================================================================
juce::String ProjectDescription::GetCategoryName(t__CategoryNo inCategoryNo)
{
    switch (inCategoryNo) {
        case t__CategoryNo::Attack :        {juce::String name("Attack");       return name;}
        case t__CategoryNo::Synth :         {juce::String name("Synth");        return name;}
        case t__CategoryNo::WaveForm :      {juce::String name("WaveForm");     return name;}
        case t__CategoryNo::BellMallet :    {juce::String name("Bell/Mallet");  return name;}
        case t__CategoryNo::BrassEns :      {juce::String name("Brass Ens");    return name;}
        case t__CategoryNo::BrassSolo :     {juce::String name("Brass Solo");   return name;}
        case t__CategoryNo::StringEns :     {juce::String name("String Ens");   return name;}
        case t__CategoryNo::StringSolo :    {juce::String name("String Solo");  return name;}
        case t__CategoryNo::Woodwind :      {juce::String name("Woodwind");     return name;}
        case t__CategoryNo::AcPiano :       {juce::String name("Ac Piano");     return name;}
        case t__CategoryNo::EPiano :        {juce::String name("E. Piano");     return name;}
        case t__CategoryNo::Keyboard :      {juce::String name("Keyboard");     return name;}
        case t__CategoryNo::Organ :         {juce::String name("Organ");        return name;}
        case t__CategoryNo::Vocal :         {juce::String name("Vocal");        return name;}
        case t__CategoryNo::AcGuitar :      {juce::String name("Ac Guitar");    return name;}
        case t__CategoryNo::EGuitar :       {juce::String name("E. Guitar");    return name;}
        case t__CategoryNo::GuitarFX :      {juce::String name("Guitar FX");    return name;}
        case t__CategoryNo::Plucked :       {juce::String name("Plucked");      return name;}
        case t__CategoryNo::AcBass :        {juce::String name("Ac Bass");      return name;}
        case t__CategoryNo::EBass :         {juce::String name("E. Bass");      return name;}
        case t__CategoryNo::BassFX :        {juce::String name("Bass FX");      return name;}
        case t__CategoryNo::Hits :          {juce::String name("Hits");         return name;}
        case t__CategoryNo::Kick :          {juce::String name("Kick");         return name;}
        case t__CategoryNo::Snare :         {juce::String name("Drum");         return name;}
        case t__CategoryNo::Drum :          {juce::String name("Vocal");        return name;}
        case t__CategoryNo::Cymbal :        {juce::String name("Cymbal");       return name;}
        case t__CategoryNo::Percussion :    {juce::String name("Percussion");   return name;}
        case t__CategoryNo::Effects :       {juce::String name("Effects");      return name;}
        case t__CategoryNo::WaveTable :     {juce::String name("WaveTable");    return name;}
        case t__CategoryNo::CasualNoises :  {juce::String name("CasualNoises"); return name;}
        default:                            {juce::String name("JaWatteDade");  return name;}
    }
}

//==============================================================================
//          GetMIDI_NoteName()
//
//  CasualNoises    10/04/2021  First implementation
//==============================================================================
juce::String ProjectDescription::GetMIDI_NoteName(t__MIDI_NoteNameNo inMIDI_NoteName)
{
    switch (inMIDI_NoteName) {
        case t__MIDI_NoteNameNo::cm2 :        {juce::String name("C-2");      return name;}
        case t__MIDI_NoteNameNo::cm2s :       {juce::String name("C#-2");     return name;}
        case t__MIDI_NoteNameNo::dm2 :        {juce::String name("D-2");      return name;}
        case t__MIDI_NoteNameNo::dm2s :       {juce::String name("D#-2");     return name;}
        case t__MIDI_NoteNameNo::em2 :        {juce::String name("E-2");      return name;}
        case t__MIDI_NoteNameNo::fm2 :        {juce::String name("F-2");      return name;}
        case t__MIDI_NoteNameNo::fm2s :       {juce::String name("F#-2");     return name;}
        case t__MIDI_NoteNameNo::gm2 :        {juce::String name("G-2");      return name;}
        case t__MIDI_NoteNameNo::gm2s :       {juce::String name("G#-2");     return name;}
        case t__MIDI_NoteNameNo::am2 :        {juce::String name("A-2");      return name;}
        case t__MIDI_NoteNameNo::am2s :       {juce::String name("A#-2");     return name;}
        case t__MIDI_NoteNameNo::bm2 :        {juce::String name("B-2");      return name;}
        case t__MIDI_NoteNameNo::cm1 :        {juce::String name("C-1");      return name;}
        case t__MIDI_NoteNameNo::cm1s :       {juce::String name("C#-1");     return name;}
        case t__MIDI_NoteNameNo::dm1 :        {juce::String name("D-1");      return name;}
        case t__MIDI_NoteNameNo::dm1s :       {juce::String name("D#-1");     return name;}
        case t__MIDI_NoteNameNo::em1 :        {juce::String name("E-1");      return name;}
        case t__MIDI_NoteNameNo::fm1 :        {juce::String name("F-1");      return name;}
        case t__MIDI_NoteNameNo::fm1s :       {juce::String name("F#-1");     return name;}
        case t__MIDI_NoteNameNo::gm1 :        {juce::String name("G-1");      return name;}
        case t__MIDI_NoteNameNo::gm1s :       {juce::String name("G#-1");     return name;}
        case t__MIDI_NoteNameNo::am1 :        {juce::String name("A-1");      return name;}
        case t__MIDI_NoteNameNo::am1s :       {juce::String name("A#-1");     return name;}
        case t__MIDI_NoteNameNo::bm1 :        {juce::String name("B-1");      return name;}
        case t__MIDI_NoteNameNo::c0 :         {juce::String name("C");        return name;}
        case t__MIDI_NoteNameNo::c0s :        {juce::String name("C#");       return name;}
        case t__MIDI_NoteNameNo::d0 :         {juce::String name("D");        return name;}
        case t__MIDI_NoteNameNo::d0s :        {juce::String name("D#");       return name;}
        case t__MIDI_NoteNameNo::e0 :         {juce::String name("E");        return name;}
        case t__MIDI_NoteNameNo::f0 :         {juce::String name("F");        return name;}
        case t__MIDI_NoteNameNo::f0s :        {juce::String name("F#");       return name;}
        case t__MIDI_NoteNameNo::g0 :         {juce::String name("G");        return name;}
        case t__MIDI_NoteNameNo::g0s :        {juce::String name("G#");       return name;}
        case t__MIDI_NoteNameNo::a0 :         {juce::String name("A");        return name;}
        case t__MIDI_NoteNameNo::a0s :        {juce::String name("A#");       return name;}
        case t__MIDI_NoteNameNo::b0 :         {juce::String name("B");        return name;}
        case t__MIDI_NoteNameNo::cp1 :        {juce::String name("C1");       return name;}
        case t__MIDI_NoteNameNo::cp1s :       {juce::String name("C#1");      return name;}
        case t__MIDI_NoteNameNo::dp1 :        {juce::String name("D1");       return name;}
        case t__MIDI_NoteNameNo::dp1s :       {juce::String name("D#1");      return name;}
        case t__MIDI_NoteNameNo::ep1 :        {juce::String name("E1");       return name;}
        case t__MIDI_NoteNameNo::fp1 :        {juce::String name("F1");       return name;}
        case t__MIDI_NoteNameNo::fp1s :       {juce::String name("F#1");      return name;}
        case t__MIDI_NoteNameNo::gp1 :        {juce::String name("G1");       return name;}
        case t__MIDI_NoteNameNo::gp1s :       {juce::String name("G#1");      return name;}
        case t__MIDI_NoteNameNo::ap1 :        {juce::String name("A1");       return name;}
        case t__MIDI_NoteNameNo::ap1s :       {juce::String name("A#1");      return name;}
        case t__MIDI_NoteNameNo::bp1 :        {juce::String name("B1");       return name;}
        case t__MIDI_NoteNameNo::cp2 :        {juce::String name("C2");       return name;}
        case t__MIDI_NoteNameNo::cp2s :       {juce::String name("C#2");      return name;}
        case t__MIDI_NoteNameNo::dp2 :        {juce::String name("D2");       return name;}
        case t__MIDI_NoteNameNo::dp2s :       {juce::String name("D#2");      return name;}
        case t__MIDI_NoteNameNo::ep2 :        {juce::String name("E2");       return name;}
        case t__MIDI_NoteNameNo::fp2 :        {juce::String name("F2");       return name;}
        case t__MIDI_NoteNameNo::fp2s :       {juce::String name("F#2");      return name;}
        case t__MIDI_NoteNameNo::gp2 :        {juce::String name("G2");       return name;}
        case t__MIDI_NoteNameNo::gp2s :       {juce::String name("G#2");      return name;}
        case t__MIDI_NoteNameNo::ap2 :        {juce::String name("A2");       return name;}
        case t__MIDI_NoteNameNo::ap2s :       {juce::String name("A#2");      return name;}
        case t__MIDI_NoteNameNo::bp2 :        {juce::String name("B2");       return name;}
        case t__MIDI_NoteNameNo::cp3 :        {juce::String name("C3");       return name;}
        case t__MIDI_NoteNameNo::cp3s :       {juce::String name("C#3");      return name;}
        case t__MIDI_NoteNameNo::dp3 :        {juce::String name("D3");       return name;}
        case t__MIDI_NoteNameNo::dp3s :       {juce::String name("D#3");      return name;}
        case t__MIDI_NoteNameNo::ep3 :        {juce::String name("E3");       return name;}
        case t__MIDI_NoteNameNo::fp3 :        {juce::String name("F3");       return name;}
        case t__MIDI_NoteNameNo::fp3s :       {juce::String name("F#3");      return name;}
        case t__MIDI_NoteNameNo::gp3 :        {juce::String name("G3");       return name;}
        case t__MIDI_NoteNameNo::gp3s :       {juce::String name("G#3");      return name;}
        case t__MIDI_NoteNameNo::ap3 :        {juce::String name("A3");       return name;}
        case t__MIDI_NoteNameNo::ap3s :       {juce::String name("A#3");      return name;}
        case t__MIDI_NoteNameNo::bp3 :        {juce::String name("B3");       return name;}
        case t__MIDI_NoteNameNo::cp4 :        {juce::String name("C4");       return name;}
        case t__MIDI_NoteNameNo::cp4s :       {juce::String name("C#4");      return name;}
        case t__MIDI_NoteNameNo::dp4 :        {juce::String name("D4");       return name;}
        case t__MIDI_NoteNameNo::dp4s :       {juce::String name("D#4");      return name;}
        case t__MIDI_NoteNameNo::ep4 :        {juce::String name("E4");       return name;}
        case t__MIDI_NoteNameNo::fp4 :        {juce::String name("F4");       return name;}
        case t__MIDI_NoteNameNo::fp4s :       {juce::String name("F#4");      return name;}
        case t__MIDI_NoteNameNo::gp4 :        {juce::String name("G4");       return name;}
        case t__MIDI_NoteNameNo::gp4s :       {juce::String name("G#4");      return name;}
        case t__MIDI_NoteNameNo::ap4 :        {juce::String name("A4");       return name;}
        case t__MIDI_NoteNameNo::ap4s :       {juce::String name("A#4");      return name;}
        case t__MIDI_NoteNameNo::bp4 :        {juce::String name("B4");       return name;}
        case t__MIDI_NoteNameNo::cp5 :        {juce::String name("C5");       return name;}
        case t__MIDI_NoteNameNo::cp5s :       {juce::String name("C#5");      return name;}
        case t__MIDI_NoteNameNo::dp5 :        {juce::String name("D5");       return name;}
        case t__MIDI_NoteNameNo::dp5s :       {juce::String name("D#5");      return name;}
        case t__MIDI_NoteNameNo::ep5 :        {juce::String name("E5");       return name;}
        case t__MIDI_NoteNameNo::fp5 :        {juce::String name("F5");       return name;}
        case t__MIDI_NoteNameNo::fp5s :       {juce::String name("F#5");      return name;}
        case t__MIDI_NoteNameNo::gp5 :        {juce::String name("G5");       return name;}
        case t__MIDI_NoteNameNo::gp5s :       {juce::String name("G#5");      return name;}
        case t__MIDI_NoteNameNo::ap5 :        {juce::String name("A5");       return name;}
        case t__MIDI_NoteNameNo::ap5s :       {juce::String name("A#5");      return name;}
        case t__MIDI_NoteNameNo::bp5 :        {juce::String name("B5");       return name;}
        case t__MIDI_NoteNameNo::cp6 :        {juce::String name("C6");       return name;}
        case t__MIDI_NoteNameNo::cp6s :       {juce::String name("C#6");      return name;}
        case t__MIDI_NoteNameNo::dp6 :        {juce::String name("D6");       return name;}
        case t__MIDI_NoteNameNo::dp6s :       {juce::String name("D#6");      return name;}
        case t__MIDI_NoteNameNo::ep6 :        {juce::String name("E6");       return name;}
        case t__MIDI_NoteNameNo::fp6 :        {juce::String name("F6");       return name;}
        case t__MIDI_NoteNameNo::fp6s :       {juce::String name("F#6");      return name;}
        case t__MIDI_NoteNameNo::gp6 :        {juce::String name("G6");       return name;}
        case t__MIDI_NoteNameNo::gp6s :       {juce::String name("G#6");      return name;}
        case t__MIDI_NoteNameNo::ap6 :        {juce::String name("A6");       return name;}
        case t__MIDI_NoteNameNo::ap6s :       {juce::String name("A#6");      return name;}
        case t__MIDI_NoteNameNo::bp6 :        {juce::String name("B6");       return name;}
        case t__MIDI_NoteNameNo::cp7 :        {juce::String name("C7");       return name;}
        case t__MIDI_NoteNameNo::cp7s :       {juce::String name("C#7");      return name;}
        case t__MIDI_NoteNameNo::dp7 :        {juce::String name("D7");       return name;}
        case t__MIDI_NoteNameNo::dp7s :       {juce::String name("D#7");      return name;}
        case t__MIDI_NoteNameNo::ep7 :        {juce::String name("E7");       return name;}
        case t__MIDI_NoteNameNo::fp7 :        {juce::String name("F7");       return name;}
        case t__MIDI_NoteNameNo::fp7s :       {juce::String name("F#7");      return name;}
        case t__MIDI_NoteNameNo::gp7 :        {juce::String name("G7");       return name;}
        case t__MIDI_NoteNameNo::gp7s :       {juce::String name("G#7");      return name;}
        case t__MIDI_NoteNameNo::ap7 :        {juce::String name("A7");       return name;}
        case t__MIDI_NoteNameNo::ap7s :       {juce::String name("A#7");      return name;}
        case t__MIDI_NoteNameNo::bp7 :        {juce::String name("B7");       return name;}
        case t__MIDI_NoteNameNo::cp8 :        {juce::String name("C8");       return name;}
        case t__MIDI_NoteNameNo::cp8s :       {juce::String name("C#8");      return name;}
        case t__MIDI_NoteNameNo::dp8 :        {juce::String name("D8");       return name;}
        case t__MIDI_NoteNameNo::dp8s :       {juce::String name("D#8");      return name;}
        case t__MIDI_NoteNameNo::ep8 :        {juce::String name("E8");       return name;}
        case t__MIDI_NoteNameNo::fp8 :        {juce::String name("F8");       return name;}
        case t__MIDI_NoteNameNo::fp8s :       {juce::String name("F#8");      return name;}
        case t__MIDI_NoteNameNo::gp8 :        {juce::String name("G8");       return name;}
        default:                              {juce::String name("oops");     return name;}
    }
}
