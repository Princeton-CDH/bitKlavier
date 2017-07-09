/*
  ==============================================================================

    BKUpdateState.h
    Created: 21 Feb 2017 6:52:07pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef BKUPDATESTATE_H_INCLUDED
#define BKUPDATESTATE_H_INCLUDED

#include "BKUtilities.h"

class BKUpdateState : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<BKUpdateState>   Ptr;
    typedef Array<BKUpdateState::Ptr>                  PtrArr;
    typedef Array<BKUpdateState::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<BKUpdateState>                  Arr;
    typedef OwnedArray<BKUpdateState, CriticalSection> CSArr;
   
    BKUpdateState()
    {
        for (int i = 0; i < BKPreparationTypeNil; i++)
        {
            active.add(Array<int>());
        }
        
    };
    
    ~BKUpdateState()
    {
    };
    
    inline void removeActive(BKPreparationType type, int Id)
    {
        if (type == BKPreparationTypeNil) return;
        
        Array<int> theseActive = active.getUnchecked(type);

        for (int i = theseActive.size(); --i>=0;)
        {
            if (theseActive[i] == Id) theseActive.remove(i);
        }
        
        active.set(type, theseActive);
        
        DBG("active: " + arrayIntArrayToString(active));

    }
    
    inline void addActive(BKPreparationType type, int Id)
    {
        if (type == BKPreparationTypeNil) return;
        
        Array<int> theseActive = active.getUnchecked(type);
        
        theseActive.addIfNotAlreadyThere(Id);
        
        active.set(type, theseActive);
        
        DBG("active: " + arrayIntArrayToString(active));

    }
    
    inline bool isActive(BKPreparationType type, int Id)
    {
        if (type == BKPreparationTypeNil) return false;
        
        bool isThere = false;
        
        Array<int> theseActive = active.getUnchecked(type);
        
        isThere = theseActive.contains(Id);
        
        DBG("active: " + arrayIntArrayToString(active));

        
        return isThere;
    }
    
    inline void clearActive(void)
    {
        for (int i = 0; i < BKPreparationTypeNil; i++)
        {
            active.set(i, Array<int>());
        }
    }
    
    Array< Array<int>> active = Array <Array<int>>();
    
    int  currentDirectId = 1;
    int  currentSynchronicId = 1;
    int  currentNostalgicId = 1;
    int  currentTempoId = 1;
    int  currentTuningId = 1;
    
    int  currentModDirectId = 1;
    int  currentModSynchronicId = 1;
    int  currentModNostalgicId = 1;
    int  currentModTempoId = 1;
    int  currentModTuningId = 1;
    
    int  currentKeymapId = 1;
    
    bool idDidChange = false;
    bool galleryDidChange = false;
    bool directDidChange = false;
    bool pianoDidChange = false;
    bool pianoDidChangeForGraph = false;
    bool directPreparationDidChange = false;
    bool nostalgicPreparationDidChange = false;
    bool synchronicPreparationDidChange = false;
    bool tuningPreparationDidChange = false;
    bool tempoPreparationDidChange = false;
    bool generalSettingsDidChange = false;
    bool keymapDidChange = false;
    bool modificationDidChange = false;
    
    BKPreparationDisplay currentDisplay = DisplayNil;
    bool displayDidChange;
    
    void setCurrentDisplay(BKPreparationDisplay type)
    {
        currentDisplay = type;
        displayDidChange = true;
    }
    
    void setAllCurrentIdsTo(int val)
    {
        currentDirectId = val;
        currentSynchronicId = val;
        currentNostalgicId = val;
        currentTempoId = val;
        currentTuningId = val;
        
        currentModDirectId = val;
        currentModSynchronicId = val;
        currentModNostalgicId = val;
        currentModTempoId = val;
        currentModTuningId = val;
        
        currentKeymapId = val;
    }
    
private:

};


#endif  // BKUPDATESTATE_H_INCLUDED
