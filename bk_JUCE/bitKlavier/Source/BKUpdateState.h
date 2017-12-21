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
    bool galleriesUpdated = false;
    bool galleryLoaded = false;
    bool directDidChange = false;
    bool pianoDidChangeForGraph = false;
    bool directPreparationDidChange = false;
    bool nostalgicPreparationDidChange = false;
    bool synchronicPreparationDidChange = false;
    bool tuningPreparationDidChange = false;
    bool tempoPreparationDidChange = false;
    bool generalSettingsDidChange = false;
    bool keymapDidChange = false;
    bool modificationDidChange = false;
    bool pianoSamplesAreLoading = false;
    
    
    BKPreparationDisplay currentDisplay = DisplayNil;
    bool displayDidChange;
    
    bool loadedJson = false;

    void setCurrentDisplay(BKPreparationDisplay type)
    {
        currentDisplay = type;
        displayDidChange = true;
    }
    void setCurrentDisplay(BKPreparationType type, int Id = -1)
    {
        if (type == PreparationTypeDirect)
        {
            currentDirectId = (Id == -1) ? currentDirectId : Id;
            directPreparationDidChange = true;
            setCurrentDisplay(DisplayDirect);
        }
        else if (type == PreparationTypeSynchronic)
        {
            currentSynchronicId = (Id == -1) ? currentSynchronicId : Id;
            synchronicPreparationDidChange = true;
            setCurrentDisplay(DisplaySynchronic);
        }
        else if (type == PreparationTypeNostalgic)
        {
            currentNostalgicId = (Id == -1) ? currentNostalgicId : Id;
            nostalgicPreparationDidChange = true;
            setCurrentDisplay(DisplayNostalgic);
        }
        else if (type == PreparationTypeTuning)
        {
            currentTuningId = (Id == -1) ? currentTuningId : Id;
            tuningPreparationDidChange = true;
            setCurrentDisplay(DisplayTuning);
        }
        else if (type == PreparationTypeTempo)
        {
            currentTempoId = (Id == -1) ? currentTempoId : Id;
            tempoPreparationDidChange = true;
            setCurrentDisplay(DisplayTempo);
        }
        else if (type == PreparationTypeKeymap)
        {
            currentKeymapId = (Id == -1) ? currentKeymapId : Id;
            keymapDidChange = true;
            setCurrentDisplay(DisplayKeymap);
        }
        else if (type == PreparationTypeDirectMod)
        {
            currentModDirectId = (Id == -1) ? currentModDirectId : Id;
            directDidChange = true;
            setCurrentDisplay(DisplayDirectMod);
        }
        else if (type == PreparationTypeNostalgicMod)
        {
            currentModNostalgicId = (Id == -1) ? currentModNostalgicId : Id;
            nostalgicPreparationDidChange = true;
            setCurrentDisplay(DisplayNostalgicMod);
        }
        else if (type == PreparationTypeSynchronicMod)
        {
            currentModSynchronicId = (Id == -1) ? currentModSynchronicId : Id;
            synchronicPreparationDidChange = true;
            setCurrentDisplay(DisplaySynchronicMod);
        }
        else if (type == PreparationTypeTuningMod)
        {
            currentModTuningId = (Id == -1) ? currentModTuningId : Id;
            tuningPreparationDidChange = true;
            setCurrentDisplay(DisplayTuningMod);
        }
        else if (type == PreparationTypeTempoMod)
        {
            currentModTempoId = (Id == -1) ? currentModTempoId : Id;
            tempoPreparationDidChange = true;
            setCurrentDisplay(DisplayTempoMod);
        }
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
