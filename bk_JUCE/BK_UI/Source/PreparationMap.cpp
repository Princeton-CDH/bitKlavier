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

void PreparationMap::setSynchronic(Synchronic::PtrArr s)
{
    synchronic = s;
    deactivateIfNecessary();
}

void PreparationMap::addSynchronic(Synchronic::Ptr p)
{
    synchronic.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}

void PreparationMap::removeSynchronic(Synchronic::Ptr p)
{
    for (int i = synchronic.size(); --i >= 0;)
    {
        if (synchronic[i] == p)
        {
            synchronic.remove(i);
            break;
        }
    }
    deactivateIfNecessary();
}

void PreparationMap::setNostalgic(Nostalgic::PtrArr n)
{
    nostalgic = n;
    deactivateIfNecessary();
}

void PreparationMap::addNostalgic(Nostalgic::Ptr p)
{
    nostalgic.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}

void PreparationMap::removeNostalgic(Nostalgic::Ptr p)
{
    for (int i = nostalgic.size(); --i >= 0;)
    {
        if (nostalgic[i] == p)
        {
            nostalgic.remove(i);
            break;
        }
    }
    deactivateIfNecessary();
}

void PreparationMap::setDirect(Direct::PtrArr d)
{
    direct = d;
    deactivateIfNecessary();
}

void PreparationMap::addDirect(Direct::Ptr p)
{
    direct.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}


bool PreparationMap::contains(Direct::Ptr thisOne)
{
    for (auto p : direct)
    {
        if (p->getId() == thisOne->getId())
        {
            return true;
        }
    }
    return false;
}

bool PreparationMap::contains(Tuning::Ptr thisOne)
{
    bool isThere = false;
    for (auto p : tuning)
    {
        if (p->getId() == thisOne->getId())
        {
            isThere = true;
            break;
        }
    }
    
    return isThere;
}

bool PreparationMap::contains(Tempo::Ptr thisOne)
{
    bool isThere = false;
    for (auto p : tempo)
    {
        if (p->getId() == thisOne->getId())
        {
            isThere = true;
            break;
        }
    }
    
    return isThere;
}

bool PreparationMap::contains(Synchronic::Ptr thisOne)
{
    bool isThere = false;
    for (auto p : synchronic)
    {
        if (p->getId() == thisOne->getId())
        {
            isThere = true;
            break;
        }
    }
    
    return isThere;
}

bool PreparationMap::contains(Nostalgic::Ptr thisOne)
{
    bool isThere = false;
    for (auto p : nostalgic)
    {
        if (p->getId() == thisOne->getId())
        {
            isThere = true;
            break;
        }
    }
    
    return isThere;
}

void PreparationMap::removeDirect(Direct::Ptr p)
{
    for (int i = direct.size(); --i >= 0;)
    {
        if (direct[i] == p)
        {
            direct.remove(i);
            break;
        }
    }
    deactivateIfNecessary();
}

void PreparationMap::setTempo(Tempo::PtrArr d)
{
    tempo = d;
    deactivateIfNecessary();
}

void PreparationMap::addTempo(Tempo::Ptr p)
{
    tempo.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}

void PreparationMap::removeTempo(Tempo::Ptr p)
{
    for (int i = tempo.size(); --i >= 0;)
    {
        if (tempo[i] == p)
        {
            tempo.remove(i);
            break;
        }
    }
    deactivateIfNecessary();
}

void PreparationMap::setTuning(Tuning::PtrArr d)
{
    tuning = d;
    deactivateIfNecessary();
}

void PreparationMap::addTuning(Tuning::Ptr p)
{
    tuning.addIfNotAlreadyThere(p);
    deactivateIfNecessary();
}

void PreparationMap::removeTuning(Tuning::Ptr p)
{
    for (int i = tuning.size(); --i >= 0;)
    {
        if (tuning[i] == p)
        {
            tuning.remove(i);
            break;
        }
    }
    deactivateIfNecessary();
}


Synchronic::PtrArr PreparationMap::getSynchronic(void)
{
    return synchronic;
}

Nostalgic::PtrArr PreparationMap::getNostalgic(void)
{
    return nostalgic;
}

Direct::PtrArr PreparationMap::getDirect(void)
{
    return direct;
}

Tuning::PtrArr PreparationMap::getTuning(void)
{
    return tuning;
}


Tempo::PtrArr PreparationMap::getTempo(void)
{
    return tempo;
}


void PreparationMap::removeAllPreparations()
{
    synchronic.clearQuick();
    nostalgic.clearQuick();
    direct.clearQuick();
    tempo.clearQuick();
    tuning.clearQuick();
    isActive = false;
}

void PreparationMap::removeKeymap(void)
{
    pKeymap = nullptr;
    deactivateIfNecessary();
}



void PreparationMap::deactivateIfNecessary()
{
    if (pKeymap == nullptr)
    {
        isActive = false;
    }
    else if(synchronic.size() == 0 &&
       nostalgic.size() == 0 &&
       direct.size() == 0 &&
       tuning.size() == 0 &&
       tempo.size() == 0)
    {
        isActive = false;
    }
    else
    {
        isActive = true;
    }
}


void PreparationMap::processBlock(int numSamples, int midiChannel)
{
    for (int i = tempo.size(); --i >= 0; )
        tempo[i]->processor->processBlock(numSamples, midiChannel);
    
    for (int i = tuning.size(); --i >= 0; )
        tuning[i]->processor->processBlock(numSamples);
  
    for (int i = synchronic.size(); --i >= 0; )
        synchronic[i]->processor->processBlock(numSamples, midiChannel);
    
    for (int i = nostalgic.size(); --i >= 0; )
        nostalgic[i]->processor->processBlock(numSamples, midiChannel);
    
    for (int i = direct.size(); --i >= 0; )
        direct[i]->processor->processBlock(numSamples, midiChannel);

}

//not sure why some of these have Channel and some don't; should rectify?
void PreparationMap::keyPressed(int noteNumber, float velocity, int channel)
{
    for (int i = tempo.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            tempo[i]->processor->keyPressed(noteNumber, velocity);
    }
    
    for (int i = tuning.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            tuning[i]->processor->keyPressed(noteNumber);
    }
  
    for (int i = synchronic.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            synchronic[i]->processor->keyPressed(noteNumber, velocity);
    }
    
    for (int i = nostalgic.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            nostalgic[i]->processor->keyPressed(noteNumber, velocity, channel);
    }
    
    for (int i = direct.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            direct[i]->processor->keyPressed(noteNumber, velocity, channel);
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
    else {
        for (int i = synchronic.size(); --i >= 0; )
        {
            if (pKeymap->containsNote(noteNumber))
                synchronic[i]->processor->keyReleased(noteNumber, velocity, channel);
        }
        
        for (int i = nostalgic.size(); --i >= 0; )
        {
            if (pKeymap->containsNote(noteNumber))
                nostalgic[i]->processor->keyReleased(noteNumber, channel);
        }
        
        for (int i = direct.size(); --i >= 0; )
        {
            if (pKeymap->containsNote(noteNumber))
                direct[i]->processor->keyReleased(noteNumber, velocity, channel);
        }
        
        for (int i = tempo.size(); --i >= 0; )
        {
            if (pKeymap->containsNote(noteNumber))
                tempo[i]->processor->keyReleased(noteNumber, channel);
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

void PreparationMap::sustainPedalReleased()
{
    sustainPedalIsDepressed = false;
    
    //do all keyReleased calls now
    for(int n=0; n<sustainedNotes.size(); n++)
    {
        SustainedNote releaseNote = sustainedNotes.getUnchecked(n);
        
        for (int i = synchronic.size(); --i >= 0; )
            synchronic[i]->processor->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel);
        
        for (int i = nostalgic.size(); --i >= 0; )
            nostalgic[i]->processor->keyReleased(releaseNote.noteNumber, releaseNote.channel);
    }
    
    sustainedNotes.clearQuick();
}

void PreparationMap::postRelease(int noteNumber, float velocity, int channel)
{
    for (int i = direct.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            direct[i]->processor->keyReleased(noteNumber, velocity, channel);
    }
    
    for (int i = nostalgic.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            nostalgic[i]->processor->keyReleased(noteNumber, channel);
    }
    
    for (int i = tempo.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            tempo[i]->processor->keyReleased(noteNumber, channel);
    }
    
}
