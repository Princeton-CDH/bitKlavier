/*
  ==============================================================================

    PreparationMap.cpp
    Created: 17 Dec 2016 12:35:30pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "PreparationMap.h"

PreparationMap::PreparationMap(Keymap::Ptr km,
                               int Id):
isActive(false),
Id(Id),
pKeymap(km),
sustainPedalIsDepressed(false)
{

}

PreparationMap::~PreparationMap()
{
    
}

void PreparationMap::prepareToPlay (double sr)
{
    sampleRate = sr;
}

void PreparationMap::setKeymap(Keymap::Ptr km)
{
    pKeymap = km;
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

void PreparationMap::deactivateIfNecessary()
{
    if (pKeymap == nullptr)
    {
        isActive = false;
    }
    else if(sprocessor.size() == 0 &&
       nprocessor.size() == 0 &&
       dprocessor.size() == 0 &&
       tprocessor.size() == 0 &&
       mprocessor.size() == 0)
    {
        isActive = false;
    }
    else
    {
        isActive = true;
    }
}


void PreparationMap::processBlock(int numSamples, int midiChannel, bool onlyNostalgic)
{
    if(onlyNostalgic) {
        for (auto nproc : nprocessor)
            nproc->processBlock(numSamples, midiChannel);
    }

    else
    {
        for (auto dproc : dprocessor)
            dproc->processBlock(numSamples, midiChannel);
        
        for (auto sproc : sprocessor)
            sproc->processBlock(numSamples, midiChannel);
        
        for (auto nproc : nprocessor)
            nproc->processBlock(numSamples, midiChannel);
        
        for (auto tproc : tprocessor)
            tproc->processBlock(numSamples);
        
        for (auto mproc : mprocessor)
            mproc->processBlock(numSamples, midiChannel);
    }
}

//not sure why some of these have Channel and some don't; should rectify?
void PreparationMap::keyPressed(int noteNumber, float velocity, int channel)
{
    if (pKeymap->containsNote(noteNumber))
    {
        
        if(sustainPedalIsDepressed)
        {
            DBG("removing sustained note " + String(noteNumber));
            
            for(int i=0; i<sustainedNotes.size(); i++)
            {
                if(sustainedNotes.getUnchecked(i).noteNumber == noteNumber)
                    sustainedNotes.remove(i);
            }
        }
        
        for (auto proc : dprocessor)
            proc->keyPressed(noteNumber, velocity, channel);
        
        for (auto proc : nprocessor)
            proc->keyPressed(noteNumber, velocity, channel);
        
        for (auto proc : sprocessor)
            proc->keyPressed(noteNumber, velocity);
        
        for (auto proc : tprocessor)
            proc->keyPressed(noteNumber);
        
        for (auto proc : mprocessor)
            proc->keyPressed(noteNumber, velocity);
    }
}


void PreparationMap::keyReleased(int noteNumber, float velocity, int channel)
{
    
    if(sustainPedalIsDepressed && pKeymap->containsNote(noteNumber))
    {
        SustainedNote newNote;
        newNote.noteNumber = noteNumber;
        newNote.velocity = velocity;
        newNote.channel = channel;
        DBG("storing sustained note " + String(noteNumber));
        
        sustainedNotes.add(newNote);
    }
    else
    {
        if (pKeymap->containsNote(noteNumber))
        {
            for (auto proc : dprocessor)
            {
                proc->keyReleased(noteNumber, velocity, channel);
            }
            
            for (auto proc : nprocessor)
            {
                proc->keyReleased(noteNumber, velocity);
            }
            
            for (auto proc : sprocessor)
            {
                proc->keyReleased(noteNumber, velocity, channel);
            }
            
            for (auto proc : mprocessor)
            {
                proc->keyReleased(noteNumber, velocity);
            }
            
            /* // need this ???
             for (int i = tuning.size(); --i >= 0; )
             {
             if (pKeymap->containsNote(noteNumber))
             tuning[i]->processor->keyReleased(noteNumber, channel);
             }
             */
        }
    }
}

void PreparationMap::sustainPedalReleased()
{
    sustainPedalIsDepressed = false;
    
    //do all keyReleased calls now
    for(int n=0; n<sustainedNotes.size(); n++)
    {
        SustainedNote releaseNote = sustainedNotes.getUnchecked(n);
        
        for (auto proc : dprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel);
        }
        
        for (auto proc : sprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel);
        }
        
        for (auto proc : nprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.channel);
        }
    }
    
    sustainedNotes.clearQuick();
}

void PreparationMap::postRelease(int noteNumber, float velocity, int channel)
{
    if (pKeymap->containsNote(noteNumber))
    {
        for (auto proc : dprocessor)
        {
            proc->keyReleased(noteNumber, velocity, channel);
        }
        
        for (auto proc : nprocessor)
        {
            proc->keyReleased(noteNumber, velocity, true);
        }
        
        for (auto proc : mprocessor)
        {
            proc->keyReleased(noteNumber, velocity);
        }
    }
}
