/*
  ==============================================================================

    Piano.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(PreparationsMap::CSPtrArr activePrepMaps):
currentPrepMaps(activePrepMaps)
{
    storedPrepMaps.ensureStorageAllocated(aMaxNumPreparationKeymaps);
}

Piano::~Piano()
{
    
}

void Piano::storeCurrentPiano()
{
    //copy currentPrepMaps to storedPrepMaps
}

void Piano::recallCurrentPiano()
{
    //copy storedPrepMaps to currentPrepMaps
}

/*
 // Create and return value tree representing current Piano state, for use in writing Pianos.
ValueTree* Piano::getPianoValueTree(void)
{
   
    return &vt;
}

*/
