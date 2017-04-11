/*
  ==============================================================================

    Reset.h
    Created: 11 Apr 2017 11:32:50am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef RESET_H_INCLUDED
#define RESET_H_INCLUDED

#include "BKUtilities.h"

#include "Keymap.h"

class Reset
{
public:
    Reset(BKPreparationType type, Keymap::Ptr keymap, Array<int> preparations):
    type(type),
    keymap(keymap),
    preparations(preparations)
    {
        
    }
    
    Reset():
    type(BKPreparationTypeNil),
    keymap(new Keymap())
    {
        
    }
    
    inline void setType(BKPreparationType t) { type = t; }
    
    inline void setKeymap(Keymap::Ptr k)
    {
        keymap.copy(k);
    }
    
    inline void addKeymap(Keymap::Ptr k)
    {
        keymap.addKeymap(k);
    }
    
    inline void setPreparations(Array<int> p) { preparations = p; }
    inline void addPreparations(Array<int> preps) { for (auto p : preps) preparations.add(p); }
    inline void addPreparation(int p)       { preparations.add(p);}
    
    inline BKPreparationType getType(void) { return type; }
    inline Array<int> getKeys(void) { return keymap.keys(); }
    inline Array<int> getPreparations(void) { return preparations; }
    
private:
    BKPreparationType type;
    Keymap keymap;
    Array<int> preparations;
    
    JUCE_LEAK_DETECTOR(Reset)
};



#endif  // RESET_H_INCLUDED
