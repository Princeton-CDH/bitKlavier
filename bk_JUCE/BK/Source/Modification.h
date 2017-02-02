/*
  ==============================================================================

    Modification.h
    Created: 1 Feb 2017 5:32:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef MODIFICATION_H_INCLUDED
#define MODIFICATION_H_INCLUDED


#include "BKUtilities.h"

class Modification : public ReferenceCountedObject

{
public:
    typedef ReferenceCountedObjectPtr<Modification>   Ptr;
    typedef Array<Modification::Ptr>                  PtrArr;
    typedef Array<Modification::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Modification>                  Arr;
    typedef OwnedArray<Modification, CriticalSection> CSArr;
    
    Modification(BKModificationType whichMod, int whichPrep, int            val);
    Modification(BKModificationType whichMod, int whichPrep, float          val);
    Modification(BKModificationType whichMod, int whichPrep, bool           val);
    Modification(BKModificationType whichMod, int whichPrep, Array<int>     val);
    Modification(BKModificationType whichMod, int whichPrep, Array<float>   val);
    

    ~Modification(void);
    
    int             prepId;
    
    int             modInt;
    float           modFloat;
    bool            modBool;
    Array<int>      modIntArr;
    Array<float>    modFloatArr;
    
    BKModificationType type;
    
private:
    
    
    JUCE_LEAK_DETECTOR(Modification)
};



#endif  // MODIFICATION_H_INCLUDED
