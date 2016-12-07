/*
  ==============================================================================

    Preset.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Preset.h"

Preset::Preset(BKAudioProcessor& p):
processor(p)
{
    // Set pointers to Preparations and Keymaps here, based on Processor state. 
}

Preset::~Preset()
{
    
}

 // Create and return value tree representing current preset state, for use in writing presets.
ValueTree* Preset::getPresetValueTree(void)
{
   
    return &vt;
}

