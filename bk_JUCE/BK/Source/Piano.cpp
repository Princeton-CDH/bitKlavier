/*
  ==============================================================================

    Piano.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(BKSynthesiser *ms,
             BKSynthesiser *res,
             BKSynthesiser *ham,
             Keymap::Ptr keymap,
             int Id):
currentPMap(PreparationMap::Ptr()),
activePMaps(PreparationMap::CSPtrArr()),
prepMaps(PreparationMap::CSPtrArr()),
Id(Id)
{
    //initialize pianomaps, keymaps, preparations, and processors
    for (int i = 0; i < aMaxNumPreparationKeymaps; i++)
    {
        prepMaps.add(new PreparationMap(ms, res, ham,
                                        keymap,
                                        i));
    }
}

Piano::~Piano()
{
    
}

void Piano::prepareToPlay(double sampleRate)
{
    for (int i = 0; i < aMaxNumPreparationKeymaps; i++)
        prepMaps[i]->prepareToPlay(sampleRate);
}

 // Create and return value tree representing current Piano state, for use in writing Pianos.
ValueTree* Piano::getPianoValueTree(void)
{
   
    return &vt;
}

