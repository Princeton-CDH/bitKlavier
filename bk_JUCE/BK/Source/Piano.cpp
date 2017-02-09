/*
  ==============================================================================

    Piano.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(Synchronic::PtrArr synchronic,
             Nostalgic::PtrArr nostalgic,
             Direct::PtrArr direct,
             Keymap::Ptr keymap,
             int Id):
currentPMap(PreparationMap::Ptr()),
activePMaps(PreparationMap::CSPtrArr()),
prepMaps(PreparationMap::CSPtrArr()),
synchronic(synchronic),
nostalgic(nostalgic),
direct(direct),
numPMaps(0),
Id(Id),
initialKeymap(keymap)
{
    pianoMap.ensureStorageAllocated(128);
    
    modMap = OwnedArray<ModificationMap>();
    modMap.ensureStorageAllocated(128);
    
    for (int i = 0; i < 128; i++)
    {
        pianoMap.set(i, 0);
        modMap.add(new ModificationMap());
    }
    
    prepMaps.ensureStorageAllocated(12);
}


Piano::~Piano()
{
    
}

// Add preparation map, return its Id.
int Piano::addPreparationMap(void)
{
    prepMaps.add(new PreparationMap(initialKeymap,
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

