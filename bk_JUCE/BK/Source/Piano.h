/*
  ==============================================================================

    Piano.h
    Created: 8 Dec 2016 12:59:46am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PIANO_H_INCLUDED
#define PIANO_H_INCLUDED

#include "BKUtilities.h"

class Piano : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Piano>    Ptr;
    typedef Array<Piano::Ptr>                   Arr;
    typedef Array<Piano::Ptr, CriticalSection>  CSArr;
    
    Piano(BKPreparationType type, int pianoNum, int keymapId, int preparationId, int tuningId):
    type(type),
    pianoNumber(pianoNum),
    keymapId(keymapId),
    preparationId(preparationId),
    tuningId(tuningId)
    {
        
    }
    
    ~Piano()
    {
    }
    
    inline void setType(BKPreparationType val)  {   type = val; print();            }
    inline void setPianoNumber(int val)         {   pianoNumber = val; print();     }
    inline void setKeymap(int val)              {   keymapId = val;  print();       }
    inline void setPreparation(int val)         {   preparationId = val; print();   }
    inline void setTuning(int val)              {   tuningId = val; print();        }
    
    inline BKPreparationType getType(void)      {   return type;            }
    inline int getPianoNumber(void)             {   return pianoNumber;     }
    inline int getKeymap(void)                  {   return keymapId;        }
    inline int getPreparation(void)             {   return preparationId;   }
    inline int getTuning(void)                  {   return tuningId;        }
    
    
    void print(void)
    {
        DBG("type: " + String(type));
        DBG("pianoNum: " + String(pianoNumber));
        DBG("keymapId: " + String(keymapId));
        DBG("preparationId: " + String(preparationId));
        DBG("tuningId:" + String(tuningId));
    }
    
private:
    BKPreparationType type;
    int pianoNumber;
    int keymapId;
    int preparationId;
    int tuningId;
    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // PIANO_H_INCLUDED
