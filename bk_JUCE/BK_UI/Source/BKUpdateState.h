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
        
    }
    
    ~BKUpdateState()
    {
        
    }

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
    
    int getCurrentId(BKPreparationDisplay type)
    {
        if (type == DisplayDirect)
        {
            return currentDirectId;
        }
        else if (type == DisplaySynchronic)
        {
            return currentSynchronicId;
        }
        else if (type == DisplayNostalgic)
        {
            return currentNostalgicId;
        }
        else if (type == DisplayTuning)
        {
            return currentTuningId;
        }
        else if (type == DisplayTempo)
        {
            return currentTempoId;
        }
        else if (type == DisplayKeymap)
        {
            return currentKeymapId;
        }
        else if (type == DisplayDirectMod)
        {
            return currentModDirectId;
        }
        else if (type == DisplaySynchronicMod)
        {
            return currentModSynchronicId;
        }
        else if (type == DisplayNostalgicMod)
        {
            return currentModNostalgicId;
        }
        else if (type == DisplayTuningMod)
        {
            return currentModTuningId;
        }
        else if (type == DisplayTempoMod)
        {
            return currentModTempoId;
        }
        return -1;
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
