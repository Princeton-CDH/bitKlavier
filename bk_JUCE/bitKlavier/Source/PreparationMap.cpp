/*
 ==============================================================================
 
 PreparationMap.cpp
 Created: 17 Dec 2016 12:35:30pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "PreparationMap.h"

PreparationMap::PreparationMap():
isActive(false),
sustainPedalIsDepressed(false)
{
    
}

PreparationMap::~PreparationMap()
{
    
}

void PreparationMap::prepareToPlay (double sr)
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
    if (thisType == PreparationTypeDirect)
    {
        for (int i = 0; i < dprocessor.size(); ++i)
        {
            if (dprocessor[i]->getId() == thisId)
            {
                dprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        for (int i = 0; i < sprocessor.size(); ++i)
        {
            if (sprocessor[i]->getId() == thisId)
            {
                sprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        for (int i = 0; i < nprocessor.size(); ++i)
        {
            if (nprocessor[i]->getId() == thisId)
            {
                nprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeBlendronic)
    {
        for (int i = 0; i < eprocessor.size(); ++i)
        {
            if (eprocessor[i]->getType() == EffectType::EffectBlendronic)
            {
                if (eprocessor[i]->getId() == thisId)
                {
                    eprocessor[i]->addKeymap(getKeymap(keymapId));
                }
            }
        }
    }
    else if (thisType == PreparationTypeTempo)
    {
        for (int i = 0; i < mprocessor.size(); ++i)
        {
            if (mprocessor[i]->getId() == thisId)
            {
                mprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeTuning)
    {
        for (int i = 0; i < tprocessor.size(); ++i)
        {
            if (tprocessor[i]->getId() == thisId)
            {
                tprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeResonance)
    {
        for (int i = 0; i < rprocessor.size(); ++i)
        {
            if (rprocessor[i]->getId() == thisId)
            {
                rprocessor[i]->addKeymap(getKeymap(keymapId));
            }
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

DirectProcessor::PtrArr     PreparationMap::getDirectProcessors(void)
{
    return dprocessor;
}

DirectProcessor::Ptr        PreparationMap::getDirectProcessor(int Id)
{
    for (auto p : dprocessor)
    {
        if (p->getId() == Id) return p;
    }
    
    return nullptr;
}

void PreparationMap::setDirectProcessors(DirectProcessor::PtrArr p)
{
    dprocessor = p;
    deactivateIfNecessary();
}

void PreparationMap::addDirectProcessor(DirectProcessor::Ptr p)
{
    dprocessor.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}


bool PreparationMap::contains(DirectProcessor::Ptr thisOne)
{
    for (auto p : dprocessor)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
}

NostalgicProcessor::PtrArr     PreparationMap::getNostalgicProcessors(void)
{
    return nprocessor;
}

NostalgicProcessor::Ptr        PreparationMap::getNostalgicProcessor(int Id)
{
    for (auto p : nprocessor)
    {
        if (p->getId() == Id) return p;
    }
    
    return nullptr;
}

void PreparationMap::setNostalgicProcessors(NostalgicProcessor::PtrArr p)
{
    nprocessor = p;
    deactivateIfNecessary();
}

void PreparationMap::addNostalgicProcessor(NostalgicProcessor::Ptr p)
{
    nprocessor.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}


bool PreparationMap::contains(NostalgicProcessor::Ptr thisOne)
{
    for (auto p : nprocessor)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
}

SynchronicProcessor::PtrArr     PreparationMap::getSynchronicProcessors(void)
{
    return sprocessor;
}

SynchronicProcessor::Ptr        PreparationMap::getSynchronicProcessor(int Id)
{
    for (auto p : sprocessor)
    {
        if (p->getId() == Id) return p;
    }
    
    return nullptr;
}

void PreparationMap::setSynchronicProcessors(SynchronicProcessor::PtrArr p)
{
    sprocessor = p;
    deactivateIfNecessary();
}

void PreparationMap::addSynchronicProcessor(SynchronicProcessor::Ptr p)
{
    sprocessor.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}

bool PreparationMap::contains(SynchronicProcessor::Ptr thisOne)
{
    for (auto p : sprocessor)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
}

TuningProcessor::PtrArr     PreparationMap::getTuningProcessors(void)
{
    return tprocessor;
}

TuningProcessor::Ptr        PreparationMap::getTuningProcessor(int Id)
{
    for (auto p : tprocessor)
    {
        if (p->getId() == Id) return p;
    }
    
    return nullptr;
}

void PreparationMap::setTuningProcessors(TuningProcessor::PtrArr p)
{
    tprocessor = p;
    deactivateIfNecessary();
}

void PreparationMap::addTuningProcessor(TuningProcessor::Ptr p)
{
    tprocessor.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}


bool PreparationMap::contains(TuningProcessor::Ptr thisOne)
{
    for (auto p : tprocessor)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
}

TempoProcessor::PtrArr     PreparationMap::getTempoProcessors(void)
{
    return mprocessor;
}

TempoProcessor::Ptr        PreparationMap::getTempoProcessor(int Id)
{
    for (auto p : mprocessor)
    {
        if (p->getId() == Id) return p;
    }
    
    return nullptr;
}

void PreparationMap::setTempoProcessors(TempoProcessor::PtrArr p)
{
    mprocessor = p;
    deactivateIfNecessary();
}

void PreparationMap::addTempoProcessor(TempoProcessor::Ptr p)
{
    mprocessor.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}


bool PreparationMap::contains(TempoProcessor::Ptr thisOne)
{
    for (auto p : mprocessor)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
    
    
}

EffectProcessor::PtrArr     PreparationMap::getEffectProcessors(void)
{
    return eprocessor;
}

EffectProcessor::Ptr        PreparationMap::getEffectProcessor(int Id, EffectType type)
{
    for (auto p : eprocessor)
    {
        if (p->getType() == type)
        {
            if (p->getId() == Id) return p;
        }
    }
    
    return nullptr;
}

//void PreparationMap::setBlendronicProcessors(BlendronicProcessor::PtrArr p)
//{
//    bprocessor = p;
//    deactivateIfNecessary();
//}
//
void PreparationMap::addEffectProcessor(EffectProcessor::Ptr p)
{
    eprocessor.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}


bool PreparationMap::contains(EffectProcessor::Ptr thisOne)
{
    for (auto p : eprocessor)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
}

void PreparationMap::addResonanceProcessor(ResonanceProcessor::Ptr p)
{
    rprocessor.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}

void PreparationMap::setResonanceProcessors(ResonanceProcessor::PtrArr p)
{
    rprocessor = p;
    deactivateIfNecessary();
}

ResonanceProcessor::PtrArr PreparationMap::getResonanceProcessors(void)
{
    return rprocessor;
}

ResonanceProcessor::Ptr PreparationMap::getResonanceProcessor(int Id)
{
    for (auto p : rprocessor)
    {
        if (p->getId() == Id) return p;
    }

    return nullptr;
}

bool PreparationMap::contains(ResonanceProcessor::Ptr thisOne)
{
    for (auto p : rprocessor)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
}

void PreparationMap::deactivateIfNecessary()
{
    if(keymaps.size() == 0 &&
    sprocessor.size() == 0 &&
    nprocessor.size() == 0 &&
    dprocessor.size() == 0 &&
    tprocessor.size() == 0 &&
    mprocessor.size() == 0 &&
    eprocessor.size() == 0 &&
    rprocessor.size() == 0)
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
        for (auto nproc : nprocessor)
            nproc->processBlock(buffer, midiMessages, numSamples, midiChannel, sampleType);
    }
    
    else
    {
        for (auto dproc : dprocessor)
            dproc->processBlock(buffer, midiMessages, numSamples, midiChannel, sampleType);
        
        for (auto sproc : sprocessor)
            sproc->processBlock(buffer, midiMessages, numSamples, midiChannel, sampleType);
//
        for (auto nproc : nprocessor)
            nproc->processBlock(buffer, midiMessages, numSamples, midiChannel, sampleType);

//        for (auto tproc : tprocessor)
//            tproc->processBlock(numSamples);
//
//        for (auto mproc : mprocessor)
//            mproc->processBlock(numSamples, midiChannel);
//
////		for (auto bproc : bprocessor)
////			bproc->processBlock(numSamples, midiChannel);
//
//        for (auto rproc : rprocessor)
//            rproc->processBlock(numSamples, midiChannel);

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
    
    for (auto proc : tprocessor)
    {
        bool ignoreSustain = !sustainPedalIsDepressed;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeTuning])
                {
                    // tuning doesn't use velocity, so no need to apply curve
                    if (km->isInverted())
                    {
                        releaseTargetVelocities.setUnchecked(TargetTypeTuning, velocity);
                    }
                    else
                    {
                        pressTargetVelocities.setUnchecked(TargetTypeTuning, velocity);
                    }
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, true);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, true);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : mprocessor)
    {
        bool ignoreSustain = !sustainPedalIsDepressed;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeTuning])
                {
                    // tempo doesn't use velocity, so no need to apply curve
                    if (km->isInverted())
                    {
                        releaseTargetVelocities.setUnchecked(TargetTypeTempo, velocity);
                    }
                    else
                    {
                        pressTargetVelocities.setUnchecked(TargetTypeTempo, velocity);
                    }
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, true);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, true);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : eprocessor)
    {
        if (proc->getType() == EffectBlendronic)
        {
            bool ignoreSustain = !sustainPedalIsDepressed;
            for (auto km : proc->getKeymaps())
            {
                // Check that the the keymap contains the pressed note and uses the midi source of the note
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    // For each target
                    for (int i = TargetTypeBlendronicPatternSync; i <= TargetTypeBlendronicOpenCloseOutput; i++)
                    {
                        // Check that the target is enabled
                        if (km->getTargetStates()[i])
                        {
                            // If the keymap is inverted, this is a release
                            if (km->isInverted())
                            {
                                // Apply the curve and take the max velocity of any redundant cases
                                float v = jmax(releaseTargetVelocities.getUnchecked(i),
                                               km->applyVelocityCurve(velocity));
                                releaseTargetVelocities.setUnchecked(i, v);
                            }
                            // Otherwise a press
                            else
                            {
                                // Apply the curve and take the max velocity of any redundant cases
                                float v = jmax(pressTargetVelocities.getUnchecked(i),
                                               km->applyVelocityCurve(velocity));
                                pressTargetVelocities.setUnchecked(i, v);
                            }
                        }
                    }
                    if (km->getIgnoreSustain()) ignoreSustain = true;
                }
            }
            proc->keyPressed(noteNumber, pressTargetVelocities, true);
            pressTargetVelocities.fill(-1.f);
            
            if (ignoreSustain && !noteDown)
                proc->keyReleased(noteNumber, releaseTargetVelocities, true);
            releaseTargetVelocities.fill(-1.f);
        }
    }
    
    for (auto proc : dprocessor)
    {
        bool ignoreSustain = !sustainPedalIsDepressed;
        proc->resetLastVelocity();

        for (auto km : proc->getKeymaps())
        {
            // Check that the keymap contains this note mapped from this harmonizer note
            // and that it uses this midi source
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                // Check that Direct is enabled as a target (should always be true until we add other targets)
                if (km->getTargetStates()[TargetTypeDirect])
                {
                    // Collect inverted keymaps into one velocities array
                    if (km->isInverted())
                    {
                        // Apply curve, take the max of any overlapping cases, then filter
                        float v = proc->filterVelocity(jmax(releaseTargetVelocities.getUnchecked(TargetTypeDirect),
                                                            km->applyVelocityCurve(velocity)));
                        releaseTargetVelocities.set(TargetTypeDirect, v);
                    }
                    // Collect normal keymaps into another velocity array
                    else
                    {
                        float v = proc->filterVelocity(jmax(pressTargetVelocities.getUnchecked(TargetTypeDirect),
                                                            km->applyVelocityCurve(velocity)));
                        pressTargetVelocities.set(TargetTypeDirect, v);
                    }
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        // Send the processor the velocities collected from normal keymaps and let it know this is a keypress
        proc->keyPressed(noteNumber, pressTargetVelocities, true);
        pressTargetVelocities.fill(-1.f);
        
        // Send the processor the velocities collected from inverted keymaps and let it know this is a keypress
        proc->playReleaseSample(noteNumber, releaseTargetVelocities, true, soundfont);
        if (ignoreSustain && !noteDown) // Don't keyrelease if we're sustaining or the note isn't down
            proc->keyReleased(noteNumber, releaseTargetVelocities, true);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : sprocessor)
    {
        bool ignoreSustain = !sustainPedalIsDepressed;
        proc->resetLastVelocity();
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeSynchronic; i <= TargetTypeSynchronicRotate; i++)
                {
                    if (km->getTargetStates()[i])
                    {
                        if (km->isInverted())
                        {
                            float v = proc->filterVelocity(jmax(releaseTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity)));
                            releaseTargetVelocities.setUnchecked(i, v);
                        }
                        else
                        {
                            float v = proc->filterVelocity(jmax(pressTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity)));
                            pressTargetVelocities.setUnchecked(i, v);
                        }
                    }
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, true);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, true);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : nprocessor)
    {
        bool ignoreSustain = !sustainPedalIsDepressed;
        proc->resetLastVelocity();
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeNostalgic; i <= TargetTypeNostalgicClear; i++)
                {
                    if (km->getTargetStates()[i])
                    {
                        if (km->isInverted())
                        {
                            float v = proc->filterVelocity(jmax(releaseTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity)));
                            releaseTargetVelocities.setUnchecked(i, v);
                        }
                        else
                        {
                            float v = proc->filterVelocity(jmax(pressTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity)));
                            pressTargetVelocities.setUnchecked(i, v);
                        }
                    }
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, true);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, true);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : rprocessor)
    {
        //bool toggleRelease = false;
        bool ignoreSustain = !sustainPedalIsDepressed;
        // For each Keymap in each resonance processor
        for (auto km : proc->getKeymaps())
        {
            // Check that the keymap contains this note mapped from this harmonizer note
            // and that it uses this midi source
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeResonanceAdd; i <= TargetTypeResonanceRing; i++ ){
                    // Check that resonance is enabled as a target (should always be true until we add other targets)
                    if (km->getTargetStates()[i])
                    {
                        // Collect inverted keymaps into one velocities array
                        if (km->isInverted())
                        {
                            // Apply curve, take the max of any overlapping cases, then filter
                            float v = jmax(releaseTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity));
                            releaseTargetVelocities.set(i, v);
                        }
                        // Collect normal keymaps into another velocity array
                        else
                        {
                            float v = jmax(pressTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity));
                            pressTargetVelocities.set(i, v);
                        }
                    }
                }
            
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        // Send the processor the velocities collected from normal keymaps and let it know this is a keypress
        proc->keyPressed(noteNumber, pressTargetVelocities, true);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown) // Don't keyrelease if we're sustaining or the note isn't down
            proc->keyReleased(noteNumber, releaseTargetVelocities, true);
        releaseTargetVelocities.fill(-1.f);
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
    
    for (auto proc : dprocessor)
    {
        
        // figure out whether this note should cut off or sustain, and do a bunch of other housekeeping
        cutOffNote = keyReleasedByProcess(proc, noteNumber, velocity, mappedFrom, source, noteDown, soundfont, TargetTypeDirect, TargetTypeDirect, pressTargetVelocities, releaseTargetVelocities);
        
        // we're always going to play the release sample in Direct, regardless of sustain state, since hammer will release regardless
        proc->playReleaseSample(noteNumber, releaseTargetVelocities, false, soundfont);
        
        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown) proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
        
        
        /*
        // will be true if the note should cut off, not sustained
        //      as determined by sustain and sostenuto pedals, and/or whether this keymap is in ignoreSustain() mode
        bool cutOffNote;
        
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeDirect])
                {
                    if (km->isInverted())
                    {
                        // Don't filter release velocities because they are not reliable.
                        // The processor will internally ignore this release if the
                        // last key press was filtered out.
                        float v = jmax(pressTargetVelocities.getUnchecked(TargetTypeDirect),
                                       km->applyVelocityCurve(velocity));
                        pressTargetVelocities.setUnchecked(TargetTypeDirect, v);
                    }
                    else
                    {
                        float v = jmax(releaseTargetVelocities.getUnchecked(TargetTypeDirect),
                                       km->applyVelocityCurve(velocity));
                        releaseTargetVelocities.setUnchecked(TargetTypeDirect, v);
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
        
        // we're always going to play the release sample in Direct, regardless of sustain state, since hammer will release regardless
        proc->playReleaseSample(noteNumber, releaseTargetVelocities, false, soundfont);
        
        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);

        // store release velocities
        releaseTargetVelocities.fill(-1.f);
         */
         

    }
    
    for (auto proc : tprocessor)
    {
        cutOffNote = keyReleasedByProcess(proc, noteNumber, velocity, mappedFrom, source, noteDown, soundfont, TargetTypeTuning, TargetTypeTuning, pressTargetVelocities, releaseTargetVelocities);
        
        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown) proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
        
        /*
        bool ignoreSustain = !sustainPedalIsDepressed;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeTuning])
                {
                    if (km->isInverted())
                    {
                        pressTargetVelocities.setUnchecked(TargetTypeTuning, velocity);
                    }
                    else
                    {
                        releaseTargetVelocities.setUnchecked(TargetTypeTuning, velocity);
                    }
                }
                // if (km->getIgnoreSustain()) ignoreSustain = true;
                if (km->getIgnoreSustain() || (km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed))) ignoreSustain = true;
            }
        }
        
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
         */
    }
    
    for (auto proc : sprocessor)
    {
        cutOffNote = keyReleasedByProcess(proc, noteNumber, velocity, mappedFrom, source, noteDown, soundfont, TargetTypeSynchronic, TargetTypeSynchronicRotate, pressTargetVelocities, releaseTargetVelocities);
        
        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown) proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
        
        /*
        bool ignoreSustain = !sustainPedalIsDepressed;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeSynchronic; i <= TargetTypeSynchronicRotate; i++)
                {
                    if (km->getTargetStates()[i])
                    {
                        if (km->isInverted())
                        {
                            float v = jmax(pressTargetVelocities.getUnchecked(i),
                                           km->applyVelocityCurve(velocity));
                            pressTargetVelocities.setUnchecked(i, v);
                        }
                        else
                        {
                            float v = jmax(releaseTargetVelocities.getUnchecked(i),
                                           km->applyVelocityCurve(velocity));
                            releaseTargetVelocities.setUnchecked(i, v);
                        }
                    }
                }
                // if (km->getIgnoreSustain()) ignoreSustain = true;
                if (km->getIgnoreSustain() || (km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed))) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
         */
    }
    
    for (auto proc : nprocessor)
    {
        cutOffNote = keyReleasedByProcess(proc, noteNumber, velocity, mappedFrom, source, noteDown, soundfont, TargetTypeNostalgic, TargetTypeNostalgicClear, pressTargetVelocities, releaseTargetVelocities);
        
        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown) proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
        
        /*
        bool ignoreSustain = !sustainPedalIsDepressed;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeNostalgic; i <= TargetTypeNostalgicClear; i++)
                {
                    if (km->getTargetStates()[i])
                    {
                        if (km->isInverted())
                        {
                            float v = jmax(pressTargetVelocities.getUnchecked(i),
                                           km->applyVelocityCurve(velocity));
                            pressTargetVelocities.setUnchecked(i, v);
                        }
                        else
                        {
                            float v = jmax(releaseTargetVelocities.getUnchecked(i),
                                           km->applyVelocityCurve(velocity));
                            releaseTargetVelocities.setUnchecked(i, v);
                        }
                    }
                }
                // if (km->getIgnoreSustain()) ignoreSustain = true;
                if (km->getIgnoreSustain() || (km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed))) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
         */
    }
    
    for (auto proc : eprocessor)
    {
        cutOffNote = keyReleasedByProcess(proc, noteNumber, velocity, mappedFrom, source, noteDown, soundfont, TargetTypeBlendronicPatternSync, TargetTypeBlendronicOpenCloseOutput, pressTargetVelocities, releaseTargetVelocities);

        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown) proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
        
        /*
        bool ignoreSustain = !sustainPedalIsDepressed;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeBlendronicPatternSync; i <= TargetTypeBlendronicOpenCloseOutput; i++)
                {
                    if (km->getTargetStates()[i])
                    {
                        if (km->isInverted())
                        {
                            float v = jmax(pressTargetVelocities.getUnchecked(i),
                                           km->applyVelocityCurve(velocity));
                            pressTargetVelocities.setUnchecked(i, v);
                        }
                        else
                        {
                            float v = jmax(releaseTargetVelocities.getUnchecked(i),
                                           km->applyVelocityCurve(velocity));
                            releaseTargetVelocities.setUnchecked(i, v);
                        }
                    }
                }
                // if (km->getIgnoreSustain()) ignoreSustain = true;
                if (km->getIgnoreSustain() || (km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed))) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
         */
    }
    
    for (auto proc : mprocessor)
    {
        cutOffNote = keyReleasedByProcess(proc, noteNumber, velocity, mappedFrom, source, noteDown, soundfont, TargetTypeTempo, TargetTypeTempo, pressTargetVelocities, releaseTargetVelocities);

        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown) proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
        
        /*
        bool ignoreSustain = !sustainPedalIsDepressed;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeTempo])
                {
                    if (km->isInverted())
                    {
                        pressTargetVelocities.setUnchecked(TargetTypeTempo, velocity);
                    }
                    else
                    {
                        releaseTargetVelocities.setUnchecked(TargetTypeTempo, velocity);
                    }
                }
                // if (km->getIgnoreSustain()) ignoreSustain = true;
                if (km->getIgnoreSustain() || (km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed))) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
         */
    }
    
    for (auto proc : rprocessor)
    {
        cutOffNote = keyReleasedByProcess(proc, noteNumber, velocity, mappedFrom, source, noteDown, soundfont, TargetTypeResonanceAdd, TargetTypeResonanceRing, pressTargetVelocities, releaseTargetVelocities);
        
        // now, do the actual release/damping, as determined by cutOffNote
        if (cutOffNote && !noteDown) proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
        
        
        /*
        bool ignoreSustain = !sustainPedalIsDepressed;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeResonanceAdd; i <= TargetTypeResonanceRing; i++ ){
                    // Check that Direct is enabled as a target (should always be true until we add other targets)
                    if (km->getTargetStates()[i])
                    {
                        // Collect inverted keymaps into one velocities array
                        if (km->isInverted())
                        {
                            // Don't filter release velocities because they are not reliable.
                            // The processor will internally ignore this release if the
                            // last key press was filtered out.
                            float v = jmax(releaseTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity));
                            pressTargetVelocities.setUnchecked(i, v);
                        }
                        // Collect normal keymaps into another velocity array
                        else
                        {
                            float v = jmax(pressTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity));
                            releaseTargetVelocities.setUnchecked(i, v);
                        }
                    }
                }
                //if (km->getIgnoreSustain()) ignoreSustain = true;
                if (km->getIgnoreSustain() || (km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed))) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
         */
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
            /*
            if (km->getTargetStates()[targetType])
            {
                hasActiveTarget = true;
                float v = km->applyVelocityCurve(velocity);
                targetVelocities.set(targetType, v);
            }
             */
            
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
        
        for (auto proc : dprocessor)
        {
            pedalReleaseByProcess(proc, noteNumber, velocity, mappedFrom, source, keyIsDepressed, false, TargetTypeDirect, TargetTypeDirect);
            
            /*
            hasActiveTarget = false;
            allIgnoreSustain = true;
            isActiveSostenutoNote = false;
            
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    if (km->getTargetStates()[TargetTypeDirect])
                    {
                        hasActiveTarget = true;
                        float v = km->applyVelocityCurve(velocity);
                        targetVelocities.set(TargetTypeDirect, v);
                    }
                    
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                    if (km->getIsSostenuto()) km->deactivateSostenuto();
                    isActiveSostenutoNote = !km->isUnsustainingNote(noteNumber, sostenutoPedalIsDepressed, sustainPedalIsDepressed);
                }
            }

            if (!keyIsDepressed && !allIgnoreSustain && hasActiveTarget && !isActiveSostenutoNote)
                proc->keyReleased(noteNumber, targetVelocities, false);
            
            targetVelocities.fill(-1.f);
             */
        }
        
        for (auto proc : tprocessor)
        {
            pedalReleaseByProcess(proc, noteNumber, velocity, mappedFrom, source, keyIsDepressed, false, TargetTypeTuning, TargetTypeTuning);
            
            /*
            hasActiveTarget = false;
            allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    if (km->getTargetStates()[TargetTypeTuning])
                    {
                        hasActiveTarget = true;
                        float v = km->applyVelocityCurve(velocity);
                        targetVelocities.set(TargetTypeTuning, v);
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain && hasActiveTarget)
                proc->keyReleased(noteNumber, targetVelocities, false);
            targetVelocities.fill(-1.f);
             */
        }
        
        for (auto proc : mprocessor)
        {
            pedalReleaseByProcess(proc, noteNumber, velocity, mappedFrom, source, keyIsDepressed, false, TargetTypeTempo, TargetTypeTempo);
            
            /*
            hasActiveTarget = false;
            allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    if (km->getTargetStates()[TargetTypeTempo])
                    {
                        hasActiveTarget = true;
                        float v = km->applyVelocityCurve(velocity);
                        targetVelocities.set(TargetTypeTempo, v);
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain && hasActiveTarget)
                proc->keyReleased(noteNumber, targetVelocities, false);
            targetVelocities.fill(-1.f);
             */
        }
        
        for (auto proc : sprocessor)
        {
            
            pedalReleaseByProcess(proc, noteNumber, velocity, mappedFrom, source, keyIsDepressed, false, TargetTypeSynchronic, TargetTypeSynchronicRotate);
            
            /*
            hasActiveTarget = false;
            allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    for (int i = TargetTypeSynchronic; i <= TargetTypeSynchronicRotate; i++)
                    {
                        if (km->getTargetStates()[i])
                        {
                            hasActiveTarget = true;
                            float v = km->applyVelocityCurve(velocity);
                            targetVelocities.set(i, v);
                        }
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain && hasActiveTarget)
                proc->keyReleased(noteNumber, targetVelocities, false);
            targetVelocities.fill(-1.f);
             */
        }
        
        for (auto proc : nprocessor)
        {
            pedalReleaseByProcess(proc, noteNumber, velocity, mappedFrom, source, keyIsDepressed, post, TargetTypeNostalgic, TargetTypeNostalgicClear);
            
            /*
            hasActiveTarget = false;
            allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    for (int i = TargetTypeNostalgic; i <= TargetTypeNostalgicClear; i++)
                    {
                        if (km->getTargetStates()[i])
                        {
                            hasActiveTarget = true;
                            float v = km->applyVelocityCurve(velocity);
                            targetVelocities.set(i, v);
                        }
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain && hasActiveTarget)
                proc->keyReleased(noteNumber, targetVelocities, post);
            targetVelocities.fill(-1.f);
             */
        }
        
        for (auto proc : eprocessor)
        {
            pedalReleaseByProcess(proc, noteNumber, velocity, mappedFrom, source, keyIsDepressed, false, TargetTypeBlendronicPatternSync, TargetTypeBlendronicOpenCloseOutput);
            
            /*
            hasActiveTarget = false;
            allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    for (int i = TargetTypeBlendronicPatternSync; i <= TargetTypeBlendronicOpenCloseOutput; i++)
                    {
                        if (km->getTargetStates()[i])
                        {
                            hasActiveTarget = true;
                            float v = km->applyVelocityCurve(velocity);
                            targetVelocities.set(i, v);
                        }
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain && hasActiveTarget)
                proc->keyReleased(noteNumber, targetVelocities, false);
            targetVelocities.fill(-1.f);
             */
        }
        
        for (auto proc : rprocessor)
        {
            pedalReleaseByProcess(proc, noteNumber, velocity, mappedFrom, source, keyIsDepressed, false, TargetTypeResonanceAdd, TargetTypeResonanceRing);
            
            /*
            hasActiveTarget = false;
            allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    for (int i = TargetTypeResonanceAdd; i <= TargetTypeResonanceRing; i++)
                    {
                        if (km->getTargetStates()[i])
                        {
                            hasActiveTarget = true;
                            float v = km->applyVelocityCurve(velocity);
                            targetVelocities.set(i, v);
                        }
                        if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                    }
                }
            }
            //local flag for keymap that isn't ignoring sustain
            if (!keyIsDepressed && !allIgnoreSustain && hasActiveTarget)
                //don't turn off note if key is down!
                proc->keyReleased(noteNumber, targetVelocities, false);
            targetVelocities.fill(-1.f);
             */
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
    /*
    if (!fromSostenutoRelease) sustainedNotes.clearQuick(); // need to NOT clear currently active sostenutoNotes!
    else {
        for (auto km : keymaps)
        {
            for(int n = sustainedNotes.size() - 1; n >= 0; n--)
            {
                Note note = sustainedNotes.getUnchecked(n);
                int noteNumber = note.noteNumber;
                if (km->isUnsustainingSostenutoNote(noteNumber, false, sustainPedalIsDepressed) && !sustainPedalIsDepressed)
                {
                    sustainedNotes.remove(n);
                    DBG("removing sustained note " + String(noteNumber));
                }
            }
        }
    }
     */
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
    
    for (auto proc : dprocessor)
    {
        bool ignoreSustain = false;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeDirect])
                {
                    float v = km->applyVelocityCurve(velocity);
                    targetVelocities.set(TargetTypeDirect, v);
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
                // check sostenutoState here, set ignoreSustain = true if km->doSostenuto && km->activeSostenutoNotesInclude(noteNumber)
            }
        }
        proc->playReleaseSample(noteNumber, targetVelocities, false);
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain))
        {
            // DBG("PreparationMap::postRelease releasing noteNumnber: " + String(noteNumber));
            proc->keyReleased(noteNumber, targetVelocities, false);
        }
        targetVelocities.fill(-1.f);
    }
    
    for (auto proc : tprocessor)
    {
        bool ignoreSustain = false;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeTuning])
                {
                    float v = km->applyVelocityCurve(velocity);
                    targetVelocities.set(TargetTypeDirect, v);
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain)) proc->keyReleased(noteNumber, targetVelocities, false);
        targetVelocities.fill(-1.f);
    }
    
    for (auto proc : nprocessor)
    {
        bool ignoreSustain = false;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeNostalgic; i <= TargetTypeNostalgicClear; i++)
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
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain)) proc->keyReleased(noteNumber, targetVelocities, true);
        targetVelocities.fill(-1.f);
    }
    
    for (auto proc : rprocessor)
    {
        bool ignoreSustain = false;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = TargetTypeResonanceAdd; i <= TargetTypeResonanceRing; i++)
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
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain))
        {
            proc->keyReleased(noteNumber, targetVelocities, false);
        }
        targetVelocities.fill(-1.f);
    }
    
    for (auto proc : mprocessor)
    {
        bool ignoreSustain = false;
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeTempo])
                {
                    float v = km->applyVelocityCurve(velocity);
                    targetVelocities.set(TargetTypeTempo, v);
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain))
            proc->keyReleased(noteNumber, targetVelocities, false);
        targetVelocities.fill(-1.f);
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
            for (auto proc : dprocessor)
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
                proc->playReleaseSample(noteNumber, targetVelocities, fromPress);
                targetVelocities.fill(-1.f);
            }
        }
    }
}

