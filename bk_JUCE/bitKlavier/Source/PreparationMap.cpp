/*
 ==============================================================================
 
 PreparationMap.cpp
 Created: 17 Dec 2016 12:35:30pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "PreparationMap.h"
#include "Direct.h"
PreparationMap::PreparationMap():
isActive(false),
sustainPedalIsDepressed(false)
{
    for (int i = 0; i < PreparationTypeKeymap; i ++)
    {
        processors.add(new ReferenceCountedArray<GenericProcessor>());
    }
    
}

PreparationMap::~PreparationMap()
{
    
}


Keymap::PtrArr     PreparationMap::getKeymaps(void)
{
    return keymaps;
}

Keymap::Ptr        PreparationMap::getKeymap(int Id)
{
    for (auto p : keymaps)
    {
        if (p->getId() == Id) return p;
    }
    
    return nullptr;
}

void PreparationMap::setKeymaps(Keymap::PtrArr p)
{
    keymaps = p;
    deactivateIfNecessary();
}

void PreparationMap::addKeymap(Keymap::Ptr p)
{
    keymaps.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}

void PreparationMap::linkKeymapToPreparation(int keymapId, BKPreparationType thisType, int thisId)
{
    for(auto proc : *processors[thisType])
    {
        if(proc->getId() == thisId)
        {
            proc->addKeymap(getKeymap(keymapId));
            return;
        }
    }
}

bool PreparationMap::contains(Keymap::Ptr thisOne)
{
    for (auto p : keymaps)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
}

 ReferenceCountedArray<GenericProcessor>*    PreparationMap::getProcessorsOfType(BKPreparationType type)
{
     return processors.getUnchecked(type);
}

GenericProcessor::Ptr        PreparationMap::getProcessorOfType(int Id, BKPreparationType type)
{
    for (auto p : *processors.getUnchecked(type))
    {
        if (p->getType() == type)
        if (p->getId() == Id) return p;
    }
    
    return nullptr;
}


bool PreparationMap::contains(GenericProcessor::Ptr thisOne, BKPreparationType type)
{
    return processors.getUnchecked(type)->contains(thisOne);
}


void PreparationMap::deactivateIfNecessary()
{
    if(keymaps.size() == 0 &&
    processors.size() == 0)
    {
        isActive = false;
    }
    else
    {
        isActive = true;
    }
}


void PreparationMap::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages, int numSamples, int midiChannel, BKSampleLoadType type, bool onlyNostalgic)
{
    sampleType = type;
    if(onlyNostalgic) {
        for (auto nproc : *processors[PreparationTypeNostalgic])
            nproc->processBlock(buffer, midiMessages, numSamples, midiChannel, sampleType);
    }
    
    else
    {
        for (auto proc : processors)
            for(auto p : *proc)
                p->processBlock(buffer, midiMessages, numSamples, midiChannel, sampleType);

    }
}

void PreparationMap::clearKey(int noteNumber)
{
    if(sustainPedalIsDepressed)
    {
        for(int i=0; i<sustainedNotes.size(); i++)
        {
            if(sustainedNotes.getUnchecked(i).noteNumber == noteNumber)
                sustainedNotes.remove(i);
        }
    }
}

//not sure why some of these have Channel and some don't; should rectify?
void PreparationMap::keyPressed(int noteNumber, float velocity, int channel, int mappedFrom, bool noteDown, bool soundfont, String source)
{
    Array<float> pressTargetVelocities;
    Array<float> releaseTargetVelocities;
    
    // Initialize the target states as being disabled
    for (int i = 0; i < TargetTypeNil; i++)
    {
        pressTargetVelocities.add(-1.f);
        releaseTargetVelocities.add(-1.f);
    }
    
    bool checkForReattack = false;
    bool checkForSustain = false;
    
    for (auto km : keymaps)
    {
        if (km->getAllMidiInputIdentifiers().contains(source))
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom))
            {
                // store velocity, for velocity curve handling
                km->addVelocity(noteNumber, velocity);
                
                // any pressed note is a potential sostenuto note
                Note newNote;
                newNote.noteNumber = noteNumber;
                newNote.velocity = velocity;
                newNote.channel = channel;
                newNote.mappedFrom = mappedFrom;
                newNote.source = source;
                km->addToPotentialSostenutoNotes(newNote);
                
                // sort out isInverted (where noteOffs and noteOns swap roles)
                if (km->isInverted()) checkForSustain = true;
                else checkForReattack = true;
            }
        }
    }
    
    // these functions deal with the complications of km->isInverted()
    if (checkForSustain) attemptSustain(noteNumber, velocity, channel, mappedFrom, true, soundfont, source);
    if (checkForReattack) attemptReattack(noteNumber, mappedFrom, source);
    

    for (auto procArr : processors){
        for (auto proc : *procArr)
        {
            
            bool ignoreSustain = !sustainPedalIsDepressed;
            proc->resetLastVelocity();
            for (auto km : proc->getKeymaps())
            {
                fillVelocities(pressTargetVelocities, releaseTargetVelocities, proc->targetTypeStart, proc->targetTypeEnd,
                               velocity, km, noteNumber, mappedFrom, source, ignoreSustain, proc);
            }
            proc->keyPressed(noteNumber, pressTargetVelocities, true);
            pressTargetVelocities.fill(-1.f);
            
            if (ignoreSustain && !noteDown)
                proc->keyReleased(noteNumber, releaseTargetVelocities, true);
            releaseTargetVelocities.fill(-1.f);
            if( proc->getType() == PreparationTypeDirect)
                dynamic_cast<DirectProcessor*>(proc)->playReleaseSample(noteNumber, releaseTargetVelocities, true, soundfont);
        }
    }
    
 
    
    // PERFORM MODIFICATION STUFF
}

template<typename P>
bool PreparationMap::keyReleasedByProcess(P proc, int noteNumber, float velocity, int mappedFrom, String source, bool noteDown, bool soundfont, KeymapTargetType targetTypeStart, KeymapTargetType targetTypeEnd, Array<float>& pressTargetVelocities, Array<float>& releaseTargetVelocities)
{
    // will be true if the note should cut off, not sustained
    //      as determined by sustain and sostenuto pedals, and/or whether this keymap is in ignoreSustain() mode
    bool cutOffNote;
    
    for (auto km : proc->getKeymaps())
    {
        if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
        {
            for (int i = targetTypeStart; i <= targetTypeEnd; i++)
            {
                if (km->getTargetStates()[i])
                {
                    if (km->isInverted())
                    {
                        // Don't filter release velocities because they are not reliable.
                        // The processor will internally ignore this release if the
                        // last key press was filtered out.
                        float v = jmax(pressTargetVelocities.getUnchecked(i), km->applyVelocityCurve(velocity));
                        pressTargetVelocities.setUnchecked(i, v);
                    }
                    else
                    {
                        float v = jmax(releaseTargetVelocities.getUnchecked(i), km->applyVelocityCurve(velocity));
                        releaseTargetVelocities.setUnchecked(i, v);
                        // DBG("setting releaseTargetVelocities " + String(i) + " " + String(v));
                    }
                }
            }

            // determine whether this note should be sustaining or not, either because of the sustain or sostenuto pedals
            //      factors include:
            //          - whether this keymap is set to ignore the sustain pedal,
            //          - whether this keymap is set so the sustain pedal behaves like a sostenuto pedal
            //          - the somewhat confusing interaction between sustain and sostenuto pedals!
            cutOffNote = km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed);
            if (km->getIgnoreSustain() || cutOffNote) cutOffNote = true;
            else cutOffNote = false;

        }
    }
    
    // last argument indicates this is NOT from an actual key press
    proc->keyPressed(noteNumber, pressTargetVelocities, false);
    pressTargetVelocities.fill(-1.f);

    return cutOffNote;
}

void PreparationMap::keyReleased(int noteNumber, float velocity, int channel, int mappedFrom, bool noteDown, bool soundfont, String source)
{
    /*
     iterate through keymaps inside each processor loop
     check sustain status (not could sustain even when key is released, due to pedaling or ignoreSustain setting
     
     see Keymap.h for notes about sostenuto implementation
    */

    Array<float> pressTargetVelocities;
    Array<float> releaseTargetVelocities;
    
    // Initialize the targets as being disabled
    for (int i = 0; i < TargetTypeNil; i++)
    {
        pressTargetVelocities.add(-1.f);
        releaseTargetVelocities.add(-1.f);
    }
    
    // for keeping track of km->isInverted() stuff (when noteOn and noteOff swap roles)
    bool foundReattack = false;
    bool foundSustain = false;
    bool cutOffNote;
    
    for (auto km : keymaps)
    {
        if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
        {
            km->removeVelocity(noteNumber);
            
            // set states in case in noteOn/Off inverted mode
            if (km->isInverted()) foundReattack = true;
            else foundSustain = true;
            
            // need to remove this note from potential sostenuto notes
            //      at the moment, we're not considering isInverted when thinking about sostenuto
            //      might address later
            Note newNote;
            newNote.noteNumber = noteNumber;
            newNote.velocity = velocity;
            newNote.channel = channel;
            newNote.mappedFrom = mappedFrom;
            newNote.source = source;
            km->removeFromPotentialSostenutoNotes(newNote);
        }
    }
    
    // sort out whether this is an actual release, or actually a noteOn becuase of isInverted()
    if (foundSustain) attemptSustain(noteNumber, velocity, channel, mappedFrom, false, soundfont, source);
    if (foundReattack) attemptReattack(noteNumber, mappedFrom, source);
    for (auto procArr : processors){
        for (auto proc : *procArr)
        {
        // figure out whether this note should cut off or sustain, and do a bunch of other housekeeping
        cutOffNote = keyReleasedByProcess(proc, noteNumber, velocity, mappedFrom, source, noteDown, soundfont, proc->targetTypeStart, proc->targetTypeEnd, pressTargetVelocities, releaseTargetVelocities);
        
        // we're always going to play the release sample in Direct, regardless of sustain state, since hammer will release regardless
        if( proc->getType() == PreparationTypeDirect)
                dynamic_cast<DirectProcessor*>(proc)->playReleaseSample(noteNumber, releaseTargetVelocities, false, soundfont);
        
        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown) proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
        
        }
    }
}

void PreparationMap::sostenutoPedalPressed() { // take sourcedNotesOn
    if(!sostenutoPedalIsDepressed) {
        // DBG("sostenutoPedalPressed");
        sostenutoPedalIsDepressed = true;
        
        for (auto km : keymaps)
        {
            // tell each keymap to copy their potentialSostenutoNotes to activeSostenutoNotes
            km->activateSostenuto();
        }
    }
}

void PreparationMap::sostenutoPedalReleased(OwnedArray<HashMap<String, int>>& keysThatArePressed) { 
    if(sostenutoPedalIsDepressed) {
        // DBG("sostenutoPedalReleased");
        sostenutoPedalIsDepressed = false;
        
        for (auto km : keymaps) {
            km->deactivateSostenuto();
        }
        
        pedalReleaseHandler(keysThatArePressed, false, true); // last true indicates this is a sostenuto release
    }
}

void PreparationMap::sustainPedalPressed()
{
    sustainPedalIsDepressed = true;
    for (auto d : *processors[PreparationTypeDirect])
    {
        d->sustainPedalPressed();
    }
    for (auto km : keymaps)
    {
        // if a Keymap is in sostenuto mode, copy potentialSostenutoNotes to activeSostenutoNotes
        //  not relevant to an actual sostenuto pedal
        if (km->getIsSostenuto()) km->activateSostenuto(); 
    }
    
}

template<typename P>
void PreparationMap::pedalReleaseByProcess(P proc, int noteNumber, float velocity, int mappedFrom, String source, bool keyIsDepressed, bool post, KeymapTargetType targetTypeStart, KeymapTargetType targetTypeEnd)
{
    bool hasActiveTarget = false;
    bool allIgnoreSustain = true;
    bool isActiveSostenutoNote = false;
    
    Array<float> targetVelocities;
    for (int i = 0; i < TargetTypeNil; i++) targetVelocities.add(-1.f);

    for (auto km : proc->getKeymaps())
    {
        if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
        {
            
            for (int i = targetTypeStart; i <= targetTypeEnd; i++)
            {
                if (km->getTargetStates()[i])
                {
                    hasActiveTarget = true;
                    float v = km->applyVelocityCurve(velocity);
                    targetVelocities.set(i, v);
                }
            }
            
            if (!km->getIgnoreSustain()) allIgnoreSustain = false;
            if (km->getIsSostenuto()) km->deactivateSostenuto();
            isActiveSostenutoNote = !km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed);
        }
    }

    if (!keyIsDepressed && !allIgnoreSustain && hasActiveTarget && !isActiveSostenutoNote)
        proc->keyReleased(noteNumber, targetVelocities, post);
}

// handles the sometimes complicated behaviors of the sustain and sostenuto pedal releases
void PreparationMap::pedalReleaseHandler(OwnedArray<HashMap<String, int>>& keysThatAreDepressed, bool post, bool fromSostenutoRelease)
{
    if (!fromSostenutoRelease) sustainPedalIsDepressed = false;
    
    Array<float> targetVelocities;
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetVelocities.add(-1.f);
    }
    
    
    
    //do all keyReleased calls now
    // DBG("sustainPedalReleased: sustainedNotes.size() = " + String(sustainedNotes.size()));
    for(int n = 0; n < sustainedNotes.size(); ++n)
    {
        Note note = sustainedNotes.getUnchecked(n);
        int noteNumber = note.noteNumber;
        float velocity = note.velocity;
        // int channel = note.channel;
        int mappedFrom = note.mappedFrom;
        String source = note.source;
        
        bool keyIsDepressed = keysThatAreDepressed.getUnchecked(noteNumber)->size() > 0;
        //bool isActiveSostenutoNote = false;
        //bool allIgnoreSustain = true;
        //bool hasActiveTarget;
        
        /*
         for each processor loop, add a keymap loop
         if the keymap contains the note and the source, if any aren't ignoring sustain, for that note/source, set flag to true
         check against that flag for release
        */
        
        for (auto procArr : processors){
            for (auto proc : *procArr)
            {
                pedalReleaseByProcess(proc, noteNumber, velocity, mappedFrom, source, keyIsDepressed, false, proc->targetTypeStart, proc->targetTypeEnd);
                //turn off pedal down resonance
             //HACK (CODE CLEANUP) DAVIS
                if (proc->getType() == PreparationTypeDirect)
                {
                    dynamic_cast<DirectProcessor*>(proc)->getPedalSynth()->keyOff(1,
                                      PedalNote,
                                      0,
                                      0,
                                      21,
                                      21,
                                      1.,
                                      1.,
                                      nullptr,
                                      true);
                    //play pedalUp sample
                    dynamic_cast<DirectProcessor*>(proc)->getPedalSynth()->keyOn(1,
                                     //synthNoteNumber,
                                     22,
                                     22,
                                     0,
                                     0.03, //gain
                                     1.,
                                     Forward,
                                     Normal, //FixedLength,
                                     PedalNote,
                                     0,
                                     0,
                                     0,
                                     2000,
                                     3,
                                     3 );
                }
            }
        }
        
        
    
       
    }
    
    for (auto km : keymaps)
    {
        for(int n = sustainedNotes.size() - 1; n >= 0; n--)
        {
            Note note = sustainedNotes.getUnchecked(n);
            int noteNumber = note.noteNumber;
            if (km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed))
            {
                sustainedNotes.remove(n);
                DBG("removing sustained note " + String(noteNumber));
            }
        }
    }
   
}

void PreparationMap::sustainPedalReleased(bool post)
{
    OwnedArray<HashMap<String, int>> keysThatAreDepressed;
    keysThatAreDepressed.ensureStorageAllocated(128);
    for (int i = 0; i < 128; ++i)
        keysThatAreDepressed.set(i, new HashMap<String, int>);
    sustainPedalReleased(keysThatAreDepressed, post);
}

void PreparationMap::postRelease(int noteNumber, float velocity, int channel, int mappedFrom, String source)
{
    // DBG("PreparationMap::postRelease " + String(noteNumber));
    
    Array<float> targetVelocities;
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetVelocities.add(-1.f);
    }
    
    if (sustainPedalIsDepressed)
    {
        Note newNote;
        newNote.noteNumber = noteNumber;
        newNote.velocity = velocity;
        newNote.channel = channel;
        newNote.mappedFrom = mappedFrom;
        newNote.source = source;
        // DBG("storing sustained note " + String(noteNumber));
        
        sustainedNotes.add(newNote);
    }

    for (auto procArr : processors){
        for (auto proc : *procArr)
        {
            bool ignoreSustain = false;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    for (int i = proc->targetTypeStart; i <= proc->targetTypeEnd; i++)
                    {
                        if (km->getTargetStates()[i])
                        {
                            float v = km->applyVelocityCurve(velocity);
                            targetVelocities.set(i, v);
                        }
                    }
                    if (km->getIgnoreSustain()) ignoreSustain = true;
                }
            }
            if(proc->getType() == PreparationTypeDirect)
            {
                dynamic_cast<DirectProcessor*>(proc)->playReleaseSample(noteNumber, targetVelocities, false);
            }
            if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain))
            {
                proc->keyReleased(noteNumber, targetVelocities, false);
            }
            targetVelocities.fill(-1.f);
        }
    }
}

void PreparationMap::attemptReattack(int noteNumber, int mappedFrom, String source)
{
    //if(sustainPedalIsDepressed)
    if(sustainPedalIsDepressed || sostenutoPedalIsDepressed)
    {
        // DBG("removing sustained note " + String(noteNumber));
        
        for(int i = 0; i < sustainedNotes.size(); ++i)
        {
            if(sustainedNotes.getUnchecked(i).noteNumber == noteNumber &&
               (sustainedNotes.getUnchecked(i).source == source) &&
               (sustainedNotes.getUnchecked(i).mappedFrom == mappedFrom))
                sustainedNotes.remove(i);
        }
    }
}

void PreparationMap::attemptSustain(int noteNumber, float velocity, int channel, int mappedFrom,
                                    bool fromPress, bool soundfont, String source)
{
    //if(sustainPedalIsDepressed)
    if(sustainPedalIsDepressed || sostenutoPedalIsDepressed)
    {
        Note newNote;
        newNote.noteNumber = noteNumber;
        newNote.velocity = velocity;
        newNote.channel = channel;
        newNote.mappedFrom = mappedFrom;
        newNote.source = source;
        // DBG("storing sustained note " + String(noteNumber));
        
        sustainedNotes.add(newNote);
        
        if (!soundfont)
        {
            Array<float> targetVelocities;
            for (int i = 0; i < TargetTypeNil; i++)
            {
                targetVelocities.add(-1.f);
            }
            
            //play hammers and resonance when keys are released, even with pedal down
            for (auto proc : *processors[PreparationTypeDirect])
            {
                for (auto km : proc->getKeymaps())
                {
                    if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                    {
                        if (km->getTargetStates()[TargetTypeDirect])
                        {
                            float v = jmax(km->applyVelocityCurve(velocity),
                                           targetVelocities.getUnchecked(TargetTypeDirect));
                            targetVelocities.setUnchecked(TargetTypeDirect, v);
                        }
                    }
                }
                dynamic_cast<DirectProcessor*>(proc)->playReleaseSample(noteNumber, targetVelocities, fromPress);
                targetVelocities.fill(-1.f);
            }
        }
    }
}

