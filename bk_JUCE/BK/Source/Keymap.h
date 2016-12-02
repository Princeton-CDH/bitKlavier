/*
  ==============================================================================

    Keymap.h
    Created: 2 Dec 2016 12:23:33pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef KEYMAP_H_INCLUDED
#define KEYMAP_H_INCLUDED

#include "BKUtilities.h"

typedef enum OctatonicType
{
    OctatonicOne,
    OctatonicTwo,
    OctatonicThree,
    OctatonicNil
} OctatonicType;

class Keymap
{
public:
    Keymap()
    {
        keymap = Array<int>();
        keymap.ensureStorageAllocated(128);
        
        for (int i = 0; i < 128; i++)
        {
            keymap.set(i,false);
        }
    }
    
    ~Keymap()
    {
        
    }
    
    // Returns true if added, false if removed.
    bool toggleNoteInKeymap(int noteNumber)
    {
        bool ret = !keymap[noteNumber];
        
        keymap[noteNumber] = ret;
        
        return ret;
    }
    
    // Returns true if added. Returns false if not added (because it's already there).
    bool addNoteToKeymap(int noteNumber)
    {
        bool ret = !keymap[noteNumber];
        
        keymap[noteNumber] = true;
        
        return ret;
    }
    
    // Returns true if removed. Returns false if not removed (because it's not there to begin with).
    bool removeNoteFromKeymap(int noteNumber)
    {
        bool ret = keymap[noteNumber];
        
        keymap[noteNumber] = false;
        
        return ret;
    }
    
    // Clears keymap.
    void clearKeymap(void)
    {
        for (int note = 0; note < 128; note++)
        {
            keymap[note] = false;
        }
    }
    
    void setAllWhiteKeys(void);
    void setAllBlackKeys(void);
    void setAllOctatonicKeys(OctatonicType type);
    
    
private:
    Array<bool> keymap;
    
    JUCE_LEAK_DETECTOR (Keymap)
};



#endif  // KEYMAP_H_INCLUDED
