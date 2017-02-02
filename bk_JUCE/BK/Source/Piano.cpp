/*
  ==============================================================================

    Piano.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(SynchronicPreparation::CSPtrArr sPrep,
             NostalgicPreparation::CSPtrArr nPrep,
             DirectPreparation::CSPtrArr dPrep,
             BKSynthesiser *ms,
             BKSynthesiser *res,
             BKSynthesiser *ham,
             Keymap::Ptr keymap,
             int Id):
currentPMap(PreparationMap::Ptr()),
activePMaps(PreparationMap::CSPtrArr()),
prepMaps(PreparationMap::CSPtrArr()),
activeSPrep(sPrep),
activeNPrep(nPrep),
activeDPrep(dPrep),
numPMaps(0),
Id(Id),
synth(ms),
resonanceSynth(res),
hammerSynth(ham),
initialKeymap(keymap)
{
    pianoMap.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)   pianoMap.set(i, 0);
    
    modMap.ensureStorageAllocated(1);
    
    prepMaps.ensureStorageAllocated(12);
}

Piano::~Piano()
{
    
}

// Add preparation map, return its Id.
int Piano::addPreparationMap(void)
{
    prepMaps.add(new PreparationMap(synth, resonanceSynth, hammerSynth,
                                    initialKeymap,
                                    numPMaps));
    
    prepMaps[numPMaps]->prepareToPlay(sampleRate);
    
    activePMaps.addIfNotAlreadyThere(prepMaps[numPMaps]);
    
    ++numPMaps;
    
    
    return numPMaps-1;
}

// Add preparation map, return its Id.
int Piano::removeLastPreparationMap(void)
{
    for (int i = activePMaps.size(); --i >= 0;)
    {
        if (activePMaps[i]->getId() == (numPMaps-1))
        {
            activePMaps.remove(i);
        }
    }
    
    prepMaps.remove((numPMaps-1));
    
    --numPMaps;

    return numPMaps;
}


void Piano::prepareToPlay(double sr)
{
    sampleRate = sr;

}


void Piano::storeCurrentPiano()
{
    //copy currentPrepMaps to storedPrepMaps
}

void Piano::recallCurrentPiano()
{
    //copy storedPrepMaps to currentPrepMaps
}

