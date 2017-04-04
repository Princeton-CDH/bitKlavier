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
   
    BKUpdateState(){};
    ~BKUpdateState(){};
    
    
    int  currentDirectId = 0;
    int  currentSynchronicId = 0;
    int  currentNostalgicId = 0;
    int  currentTempoId = 0;
    int  currentTuningId = 0;
    
    int  currentModDirectId = 0;
    int  currentModSynchronicId = 0;
    int  currentModNostalgicId = 0;
    int  currentModTempoId = 0;
    int  currentModTuningId = 0;
    
    int  currentKeymapId = 0;
    
    bool galleryDidChange = false;
    bool directDidChange = false;
    bool pianoDidChange = false;
    bool directPreparationDidChange = false;
    bool nostalgicPreparationDidChange = false;
    bool synchronicPreparationDidChange = false;
    bool tuningPreparationDidChange = false;
    bool tempoPreparationDidChange = false;
    bool generalSettingsDidChange = false;
    bool keymapDidChange = false;
    
    BKPreparationDisplay currentPreparationDisplay = DisplayDirect;
    bool displayDidChange;
    
private:

};


#endif  // BKUPDATESTATE_H_INCLUDED
