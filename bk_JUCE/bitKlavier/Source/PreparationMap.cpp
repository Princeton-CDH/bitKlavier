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
keymaps(Keymap::PtrArr(km)),
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
    else if (thisType == PreparationTypeBlendronomer)
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

BlendronomerProcessor::PtrArr     PreparationMap::getBlendronomerProcessors(void)
{
	return bprocessor;
}

BlendronomerProcessor::Ptr        PreparationMap::getBlendronomerProcessor(int Id)
{
	for (auto p : bprocessor)
	{
		if (p->getId() == Id) return p;
	}

	return nullptr;
}

void PreparationMap::setBlendronomerProcessors(BlendronomerProcessor::PtrArr p)
{
	bprocessor = p;
	deactivateIfNecessary();
}

void PreparationMap::addBlendronomerProcessor(BlendronomerProcessor::Ptr p)
{
    bprocessor.addIfNotAlreadyThere(p);
	deactivateIfNecessary();
}


bool PreparationMap::contains(BlendronomerProcessor::Ptr thisOne)
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
        {
            dproc->processBlock(numSamples, midiChannel, sampleType);
        }
        
        for (auto sproc : sprocessor)
        {
            sproc->processBlock(numSamples, midiChannel, sampleType);
        }
        
        for (auto nproc : nprocessor)
        {
            nproc->processBlock(numSamples, midiChannel, sampleType);
        }
        
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
void PreparationMap::keyPressed(int noteNumber, float velocity, int channel, bool soundfont)
{
    Array<KeymapTargetState> targetStates;
    targetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    for (auto km : keymaps)
    {
        if (km->containsNote(noteNumber))
        {
            reattack(noteNumber);
            break;
        }
    }
    
    for (auto proc : tprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
               if (km->getTargetStates()[TargetTypeTuning] == TargetStateEnabled)
                   targetStates.set(TargetTypeTuning, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyPressed(noteNumber);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : dprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeDirect] == TargetStateEnabled)
                    targetStates.set(TargetTypeDirect, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyPressed(noteNumber, velocity, channel);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : sprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeSynchronicSync] == TargetStateEnabled)
                    targetStates.set(TargetTypeSynchronicSync, TargetStateEnabled);
                if (km->getTargetStates()[TargetTypeSynchronicCluster] == TargetStateEnabled)
                    targetStates.set(TargetTypeSynchronicCluster, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyPressed(noteNumber, velocity, targetStates);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : nprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeNostalgic] == TargetStateEnabled)
                    targetStates.set(TargetTypeNostalgic, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyPressed(noteNumber, velocity, channel);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : bprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeBlendronicSync] == TargetStateEnabled)
                    targetStates.set(TargetTypeBlendronicSync, TargetStateEnabled);
                if (km->getTargetStates()[TargetTypeBlendronicClear] == TargetStateEnabled)
                    targetStates.set(TargetTypeBlendronicClear, TargetStateEnabled);
                if (km->getTargetStates()[TargetTypeBlendronicOpen] == TargetStateEnabled)
                    targetStates.set(TargetTypeBlendronicOpen, TargetStateEnabled);
                if (km->getTargetStates()[TargetTypeBlendronicClose] == TargetStateEnabled)
                    targetStates.set(TargetTypeBlendronicClose, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyPressed(noteNumber, velocity, channel, targetStates);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : mprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeTempo] == TargetStateEnabled)
                    targetStates.set(TargetTypeTempo, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyPressed(noteNumber, velocity);
        targetStates.fill(TargetStateNil);
    }
        // PERFORM MODIFICATION STUFF
}


void PreparationMap::keyReleased(int noteNumber, float velocity, int channel, bool soundfont)
{
    
    //DBG("PreparationMap::keyReleased : " + String(noteNumber));
    
    Array<KeymapTargetState> targetStates;
    targetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    for (auto km : keymaps)
    {
        if (km->containsNote(noteNumber))
        {
            if(sustainPedalIsDepressed)
            {
                SustainedNote newNote;
                newNote.noteNumber = noteNumber;
                newNote.velocity = velocity;
                newNote.channel = channel;
                //DBG("storing sustained note " + String(noteNumber));
                
                sustainedNotes.add(newNote);
                
                if (!soundfont)
                {
                    //play hammers and resonance when keys are released, even with pedal down
                    for (auto proc : dprocessor)
                    {
                        proc->playReleaseSample(noteNumber, velocity, channel);
                    }
                }
            }
            break;
        }
    }
    
    for (auto proc : dprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeDirect] == TargetStateEnabled)
                    targetStates.set(TargetTypeDirect, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled))
        {
            proc->playReleaseSample(noteNumber, velocity, channel, soundfont);
            proc->keyReleased(noteNumber, velocity, channel, soundfont);
            targetStates.fill(TargetStateNil);
        }
    }
    
    for (auto proc : tprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeTuning] == TargetStateEnabled)
                    targetStates.set(TargetTypeTuning, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyReleased(noteNumber);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : sprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeSynchronicSync] == TargetStateEnabled)
                    targetStates.set(TargetTypeSynchronicSync, TargetStateEnabled);
                if (km->getTargetStates()[TargetTypeSynchronicCluster] == TargetStateEnabled)
                    targetStates.set(TargetTypeSynchronicCluster, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyReleased(noteNumber, velocity, channel, targetStates);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : nprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeNostalgic] == TargetStateEnabled)
                    targetStates.set(TargetTypeNostalgic, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyReleased(noteNumber, velocity, channel);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : bprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeBlendronicSync] == TargetStateEnabled)
                    targetStates.set(TargetTypeBlendronicSync, TargetStateEnabled);
                if (km->getTargetStates()[TargetTypeBlendronicClear] == TargetStateEnabled)
                    targetStates.set(TargetTypeBlendronicClear, TargetStateEnabled);
                if (km->getTargetStates()[TargetTypeBlendronicOpen] == TargetStateEnabled)
                    targetStates.set(TargetTypeBlendronicOpen, TargetStateEnabled);
                if (km->getTargetStates()[TargetTypeBlendronicClose] == TargetStateEnabled)
                    targetStates.set(TargetTypeBlendronicClose, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyReleased(noteNumber, velocity, channel, targetStates);
        targetStates.fill(TargetStateNil);
    }
    
    for (auto proc : mprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber))
            {
                if (km->getTargetStates()[TargetTypeTempo] == TargetStateEnabled)
                    targetStates.set(TargetTypeTempo, TargetStateEnabled);
            }
        }
        if (targetStates.contains(TargetStateEnabled)) proc->keyReleased(noteNumber, velocity);
        targetStates.fill(TargetStateNil);
    }
}

void PreparationMap::sustainPedalReleased(Array<bool> keysThatAreDepressed, bool post)
{
    sustainPedalIsDepressed = false;
    
    Array<KeymapTargetState> targetStates;
    targetStates.ensureStorageAllocated(TargetStateNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    //do all keyReleased calls now
    for(int n=0; n<sustainedNotes.size(); n++)
    {
        SustainedNote releaseNote = sustainedNotes.getUnchecked(n);
        
        DBG(releaseNote.noteNumber);
        
        targetStates.fill(TargetStateNil);
        
        for (auto km : keymaps)
        {
            if (km->containsNote(releaseNote.noteNumber))
            {
                for (auto state : km->getTargetStates()) if (state == TargetStateEnabled) targetStates = state;
            }
        }
        
        for (auto proc : dprocessor)
        {
            if(!keysThatAreDepressed.getUnchecked(releaseNote.noteNumber)) //don't turn off note if key is down!
                proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel);
        }
        
        for (auto proc : tprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber);
        }
        
        for (auto proc : sprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel, targetStates);
        }
        
        for (auto proc : nprocessor)
        {
            //DBG("nostalgic sustainPedalReleased " + String((int)post));
            proc->keyReleased(releaseNote.noteNumber, releaseNote.channel, post);
        }
        
        for (auto proc : bprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel, targetStates);
        }
    }
    
    sustainedNotes.clearQuick();
}

void PreparationMap::sustainPedalReleased(bool post)
{
    sustainPedalIsDepressed = false;
   
    Array<KeymapTargetState> targetStates;
    targetStates.ensureStorageAllocated(TargetStateNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    //do all keyReleased calls now
    for(int n=0; n<sustainedNotes.size(); n++)
    {
        SustainedNote releaseNote = sustainedNotes.getUnchecked(n);
        
        targetStates.fill(TargetStateNil);
        
        for (auto km : keymaps)
        {
            if (km->containsNote(releaseNote.noteNumber))
            {
                for (auto state : km->getTargetStates()) if (state == TargetStateEnabled) targetStates = state;
            }
        }

        for (auto proc : dprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel);
        }
        
        for (auto proc : tprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber);
        }
        
        for (auto proc : sprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel, targetStates);
        }
        
        for (auto proc : nprocessor)
        {
            //DBG("nostalgic sustainPedalReleased " + String((int)post));
            proc->keyReleased(releaseNote.noteNumber, releaseNote.channel, post);
        }
        
        for (auto proc : bprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel, targetStates);
        }
    }
    
    sustainedNotes.clearQuick();
}

void PreparationMap::postRelease(int noteNumber, float velocity, int channel)
{
    DBG("PreparationMap::postRelease " + String(noteNumber));
    
    Array<KeymapTargetState> targetStates;
    targetStates.ensureStorageAllocated(TargetStateNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    for (auto km : keymaps)
    {
        if (km->containsNote(noteNumber))
        {
            for (auto state : km->getTargetStates()) if (state == TargetStateEnabled) targetStates = state;
        }
    }
    
    if(sustainPedalIsDepressed && targetStates.contains(TargetStateEnabled))
    {
        SustainedNote newNote;
        newNote.noteNumber = noteNumber;
        newNote.velocity = velocity;
        newNote.channel = channel;
        DBG("storing sustained note " + String(noteNumber));
        
        sustainedNotes.add(newNote);
    }
    
    if (targetStates.contains(TargetStateEnabled))
    {
        for (auto proc : dprocessor)
        {
            if (!sustainPedalIsDepressed) proc->keyReleased(noteNumber, velocity, channel);
            //proc->keyReleased(noteNumber, velocity, channel);
        }
        
        for (auto proc : tprocessor)
        {
            if (!sustainPedalIsDepressed) proc->keyReleased(noteNumber);
        }
        
        for (auto proc : nprocessor)
        {
            if (!sustainPedalIsDepressed) proc->keyReleased(noteNumber, velocity, true);
        }
        
        for (auto proc : mprocessor)
        {
            proc->keyReleased(noteNumber, velocity);
        }
    }
}

void PreparationMap::reattack(int noteNumber)
{
    if(sustainPedalIsDepressed)
    {
        //DBG("removing sustained note " + String(noteNumber));
        
        for(int i=0; i<sustainedNotes.size(); i++)
        {
            if(sustainedNotes.getUnchecked(i).noteNumber == noteNumber)
                sustainedNotes.remove(i);
        }
    }
}

void PreparationMap::merge(PreparationMap::Ptr thatMap)
{
    keymaps.addArray(thatMap->getKeymaps());
    dprocessor.addArray(thatMap->getDirectProcessors());
    sprocessor.addArray(thatMap->getSynchronicProcessors());
    nprocessor.addArray(thatMap->getNostalgicProcessors());
    bprocessor.addArray(thatMap->getBlendronomerProcessors());
    mprocessor.addArray(thatMap->getTempoProcessors());
    tprocessor.addArray(thatMap->getTuningProcessors());
}
