/*
  ==============================================================================

    PianoMap.h
    Created: 31 Jan 2017 3:15:28pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PIANOMAP_H_INCLUDED
#define PIANOMAP_H_INCLUDED


#include "BKUtilities.h"

#include"Keymap.h"

class PianoMap
{
public:
    PianoMap(void);
    ~PianoMap(void);
    
    Array<int>          keyPianoMap;
    
    
    Keymap::PtrArr      keymapPianoMap;
    
private:
    
    JUCE_LEAK_DETECTOR(PianoMap)
};



#endif  // PIANOMAP_H_INCLUDED
