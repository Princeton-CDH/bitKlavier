/*
  ==============================================================================

    Layer.h
    Created: 7 Dec 2016 3:33:44pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LAYER_H_INCLUDED
#define LAYER_H_INCLUDED

#include "BKUtilities.h"

#include "Keymap.h"

#include "Synchronic.h"

#include "Tuning.h"

class SynchronicLayer : public ReferenceCountedObject
{
    
public:
    
    typedef ReferenceCountedObjectPtr<SynchronicLayer>      Ptr;
    typedef Array<SynchronicLayer::Ptr>                     Arr;
    typedef Array<SynchronicLayer::Ptr, CriticalSection>    CSArr;
    
    SynchronicLayer(Keymap::Ptr km, SynchronicPreparation::Ptr prep, int ID):
    ID(ID),
    keymap(km),
    preparation(prep),
    keymapID(keymap->getID()),
    preparationID(preparation->getID())
    {

    }
    
    ~SynchronicLayer()
    {
    }
    
    inline const int getID(void) const noexcept { return ID; }
    
    SynchronicPreparation::Ptr  getPreparation(void)    { return preparation;            }
    Keymap::Ptr                 getKeymap(void)         { return keymap;                 }
    //inline TuningPreparation::Ptr      getTuning(void)         { return tuning;         }
    
    void setPreparation(SynchronicPreparation::Ptr p)
    {
        preparation = p;
        
        preparationID = preparation->getID();
    }
    
    void setKeymap(Keymap::Ptr k)
    {
        keymap = k;
        
        keymapID = keymap->getID();
    }
   
    /*
    inline void setTuning(TuningPreparation::Ptr t)
    {
        tuning = t;
        
        tuningID = tuning->getID();
    }
    */
    
private:
    int ID;
    
    Keymap::Ptr                 keymap;
    SynchronicPreparation::Ptr  preparation;
    //TuningPreparation::Ptr      tuning;
    
    int keymapID;
    int preparationID;
    //int tuningID;
    
    JUCE_LEAK_DETECTOR(SynchronicLayer);
    
};


#endif  // LAYER_H_INCLUDED
