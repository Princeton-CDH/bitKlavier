/*
  ==============================================================================

    Piano.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(BKAudioProcessor& p):
processor(p)
{
    // Set pointers to Preparations and Keymaps here, based on Processor state. 
}

Piano::~Piano()
{
    
}

 // Create and return value tree representing current Piano state, for use in writing Pianos.
ValueTree* Piano::getPianoValueTree(void)
{
   
    return &vt;
}

