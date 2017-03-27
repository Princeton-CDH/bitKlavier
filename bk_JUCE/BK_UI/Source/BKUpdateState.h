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
    
    bool galleryDidChange = false;
    bool pianoDidChange = false;
    bool directPreparationDidChange = false;
    bool nostalgicPreparationDidChange = false;
    bool synchronicPreparationDidChange = false;
    bool tuningPreparationDidChange = false;
    bool tempoPreparationDidChange = false;
    bool generalSettingsDidChange = false;
    
private:

};


#endif  // BKUPDATESTATE_H_INCLUDED
