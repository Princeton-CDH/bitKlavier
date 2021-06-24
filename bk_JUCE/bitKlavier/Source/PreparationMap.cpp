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
        for (int i = 0; i < bprocessor.size(); ++i)
        {
            if (bprocessor[i]->getId() == thisId)
            {
                bprocessor[i]->addKeymap(getKeymap(keymapId));
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

BlendronicProcessor::PtrArr     PreparationMap::getBlendronicProcessors(void)
{
    return bprocessor;
}

BlendronicProcessor::Ptr        PreparationMap::getBlendronicProcessor(int Id)
{
    for (auto p : bprocessor)
    {
        if (p->getId() == Id) return p;
    }
    
    return nullptr;
}

void PreparationMap::setBlendronicProcessors(BlendronicProcessor::PtrArr p)
{
    bprocessor = p;
    deactivateIfNecessary();
}

void PreparationMap::addBlendronicProcessor(BlendronicProcessor::Ptr p)
{
    bprocessor.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}


bool PreparationMap::contains(BlendronicProcessor::Ptr thisOne)
{
    for (auto p : bprocessor)
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
       bprocessor.size() == 0)
    {
        isActive = false;
    }
    else
    {
        isActive = true;
    }
}


void PreparationMap::processBlock(AudioSampleBuffer& buffer, int numSamples, int midiChannel, BKSampleLoadType type, bool onlyNostalgic)
{
    sampleType = type;
    if(onlyNostalgic) {
        for (auto nproc : nprocessor)
            nproc->processBlock(numSamples, midiChannel, sampleType);
    }
    
    else
    {
        for (auto dproc : dprocessor)
            dproc->processBlock(numSamples, midiChannel, sampleType);
        
        for (auto sproc : sprocessor)
            sproc->processBlock(numSamples, midiChannel, sampleType);
        
        for (auto nproc : nprocessor)
            nproc->processBlock(numSamples, midiChannel, sampleType);
        
        for (auto tproc : tprocessor)
            tproc->processBlock(numSamples);
        
        for (auto mproc : mprocessor)
            mproc->processBlock(numSamples, midiChannel);
        
        for (auto bproc : bprocessor)
            bproc->processBlock(numSamples, midiChannel);
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
                if (km->isInverted()) checkForSustain = true;
                else checkForReattack = true;
            }
        }
    }
    if (checkForSustain) attemptSustain(noteNumber, velocity, channel, mappedFrom, true, soundfont, source);
    if (checkForReattack) attemptReattack(noteNumber, mappedFrom, source);
    
    //put ignoreSustain condition in front of keyRelease calls
    
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
        proc->keyPressed(noteNumber, pressTargetVelocities);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities);
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
        proc->keyPressed(noteNumber, pressTargetVelocities);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : bprocessor)
    {
        bool ignoreSustain = !sustainPedalIsDepressed;
        // For each keymap
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
        proc->keyPressed(noteNumber, pressTargetVelocities);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : dprocessor)
    {
        bool ignoreSustain = !sustainPedalIsDepressed;
        proc->resetLastVelocity();
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                if (km->getTargetStates()[TargetTypeDirect])
                {
                    if (km->isInverted())
                    {
                        // Filter here so that we only take the max within the filtered range
                        float v = proc->filterVelocity(jmax(releaseTargetVelocities.getUnchecked(TargetTypeDirect),
                                                            km->applyVelocityCurve(velocity)));
                        releaseTargetVelocities.set(TargetTypeDirect, v);
                    }
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
        proc->keyPressed(noteNumber, pressTargetVelocities, true);
        pressTargetVelocities.fill(-1.f);
        
        proc->playReleaseSample(noteNumber, releaseTargetVelocities, true, soundfont);
        if (ignoreSustain && !noteDown)
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
    
    // PERFORM MODIFICATION STUFF
}


void PreparationMap::keyReleased(int noteNumber, float velocity, int channel, int mappedFrom, bool noteDown, bool soundfont, String source)
{
    //DBG("PreparationMap::keyReleased : " + String(noteNumber));
    
    /*iterate through keymaps inside each processor loop
     set local flag to true if any ignore sustain
     check against that flag or sustain pedal isn't depressed (then release key)*/
    
    Array<float> pressTargetVelocities;
    Array<float> releaseTargetVelocities;
    
    // Initialize the targets as being disabled
    for (int i = 0; i < TargetTypeNil; i++)
    {
        pressTargetVelocities.add(-1.f);
        releaseTargetVelocities.add(-1.f);
    }
    
    bool foundReattack = false;
    bool foundSustain = false;
    
    for (auto km : keymaps)
    {
        if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
        {
            if (km->isInverted()) foundReattack = true;
            else foundSustain = true;
        }
    }
    if (foundSustain) attemptSustain(noteNumber, velocity, channel, mappedFrom, false, soundfont, source);
    if (foundReattack) attemptReattack(noteNumber, mappedFrom, source);
    
    for (auto proc : dprocessor)
    {
        bool ignoreSustain = !sustainPedalIsDepressed;
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
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        proc->playReleaseSample(noteNumber, releaseTargetVelocities, false, soundfont);
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : tprocessor)
    {
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
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        
        proc->keyPressed(noteNumber, pressTargetVelocities);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : sprocessor)
    {
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
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : nprocessor)
    {
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
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities, false);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities, false);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : bprocessor)
    {
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
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities);
        releaseTargetVelocities.fill(-1.f);
    }
    
    for (auto proc : mprocessor)
    {
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
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        proc->keyPressed(noteNumber, pressTargetVelocities);
        pressTargetVelocities.fill(-1.f);
        
        if (ignoreSustain && !noteDown)
            proc->keyReleased(noteNumber, releaseTargetVelocities);
        releaseTargetVelocities.fill(-1.f);
    }
}

void PreparationMap::sustainPedalReleased(OwnedArray<HashMap<String, int>>& keysThatAreDepressed, bool post)
{
    sustainPedalIsDepressed = false;
    
    Array<float> targetStates;
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(-1.f);
    }
    
    //do all keyReleased calls now
    for(int n = 0; n < sustainedNotes.size(); ++n)
    {
        Note note = sustainedNotes.getUnchecked(n);
        int noteNumber = note.noteNumber;
        float velocity = note.velocity;
        //        int channel = note.channel;
        int mappedFrom = note.mappedFrom;
        String source = note.source;
        
        bool keyIsDepressed = keysThatAreDepressed.getUnchecked(noteNumber)->size() > 0;
        
        /*for each processor loop, add a keymap loop
         if the keymap contains the note and the source, if any aren't ignoring sustain, for that note/source, set flag to true
         check against that flag for release*/
        
        
        for (auto proc : dprocessor)
        {
            bool allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    if (km->getTargetStates()[TargetTypeDirect])
                    {
                        float v = km->applyVelocityCurve(velocity);
                        targetStates.set(TargetTypeDirect, v);
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            //local flag for keymap that isn't ignoring sustain
            if (!keyIsDepressed && !allIgnoreSustain)
                //don't turn off note if key is down!
                proc->keyReleased(noteNumber, targetStates, false);
            targetStates.fill(-1.f);
        }
        
        for (auto proc : tprocessor)
        {
            bool allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    if (km->getTargetStates()[TargetTypeTuning])
                    {
                        float v = km->applyVelocityCurve(velocity);
                        targetStates.set(TargetTypeTuning, v);
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain)
                proc->keyReleased(noteNumber, targetStates);
            targetStates.fill(-1.f);
        }
        
        for (auto proc : mprocessor)
        {
            bool allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    if (km->getTargetStates()[TargetTypeTempo])
                    {
                        float v = km->applyVelocityCurve(velocity);
                        targetStates.set(TargetTypeTempo, v);
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain)
                proc->keyReleased(noteNumber, targetStates);
            targetStates.fill(-1.f);
        }
        
        for (auto proc : sprocessor)
        {
            bool allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    for (int i = TargetTypeSynchronic; i <= TargetTypeSynchronicRotate; i++)
                    {
                        if (km->getTargetStates()[i])
                        {
                            float v = km->applyVelocityCurve(velocity);
                            targetStates.set(i, v);
                        }
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain)
                proc->keyReleased(noteNumber, targetStates, false);
            targetStates.fill(-1.f);
        }
        
        for (auto proc : nprocessor)
        {
            bool allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    for (int i = TargetTypeNostalgic; i <= TargetTypeNostalgicClear; i++)
                    {
                        if (km->getTargetStates()[i])
                        {
                            float v = km->applyVelocityCurve(velocity);
                            targetStates.set(i, v);
                        }
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain)
                proc->keyReleased(noteNumber, targetStates, post);
            targetStates.fill(-1.f);
        }
        
        for (auto proc : bprocessor)
        {
            bool allIgnoreSustain = true;
            for (auto km : proc->getKeymaps())
            {
                if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
                {
                    for (int i = TargetTypeBlendronicPatternSync; i <= TargetTypeBlendronicOpenCloseOutput; i++)
                    {
                        if (km->getTargetStates()[i])
                        {
                            float v = km->applyVelocityCurve(velocity);
                            targetStates.set(i, v);
                        }
                    }
                    if (!km->getIgnoreSustain()) allIgnoreSustain = false;
                }
            }
            if (!keyIsDepressed && !allIgnoreSustain)
                proc->keyReleased(noteNumber, targetStates);
            targetStates.fill(-1.f);
        }
    }
    
    sustainedNotes.clearQuick();
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
    DBG("PreparationMap::postRelease " + String(noteNumber));
    
    Array<float> targetStates;
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(-1.f);
    }
    
    if (sustainPedalIsDepressed)
    {
        Note newNote;
        newNote.noteNumber = noteNumber;
        newNote.velocity = velocity;
        newNote.channel = channel;
        newNote.mappedFrom = mappedFrom;
        newNote.source = source;
        DBG("storing sustained note " + String(noteNumber));
        
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
                    targetStates.set(TargetTypeDirect, v);
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain))
        {
            DBG("PreparationMap::postRelease releasing noteNumnber: " + String(noteNumber));
            proc->keyReleased(noteNumber, targetStates, false);
        }
        targetStates.fill(-1.f);
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
                    targetStates.set(TargetTypeDirect, v);
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain)) proc->keyReleased(noteNumber, targetStates);
        targetStates.fill(-1.f);
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
                        targetStates.set(i, v);
                    }
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain)) proc->keyReleased(noteNumber, targetStates, true);
        targetStates.fill(-1.f);
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
                    targetStates.set(TargetTypeTempo, v);
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        }
        if ((!sustainPedalIsDepressed) || (sustainPedalIsDepressed && ignoreSustain))
            proc->keyReleased(noteNumber, targetStates);
        targetStates.fill(-1.f);
    }
}

void PreparationMap::attemptReattack(int noteNumber, int mappedFrom, String source)
{
    if(sustainPedalIsDepressed)
    {
        //DBG("removing sustained note " + String(noteNumber));
        
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
    if(sustainPedalIsDepressed)
    {
        Note newNote;
        newNote.noteNumber = noteNumber;
        newNote.velocity = velocity;
        newNote.channel = channel;
        newNote.mappedFrom = mappedFrom;
        newNote.source = source;
        //DBG("storing sustained note " + String(noteNumber));
        
        sustainedNotes.add(newNote);
        
        if (!soundfont)
        {
            Array<float> targetStates;
            for (int i = 0; i < TargetTypeNil; i++)
            {
                targetStates.add(-1.f);
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
                                           targetStates.getUnchecked(TargetTypeDirect));
                            targetStates.setUnchecked(TargetTypeDirect, v);
                        }
                    }
                }
                proc->playReleaseSample(noteNumber, targetStates, fromPress);
                targetStates.fill(-1.f);
            }
        }
    }
}

