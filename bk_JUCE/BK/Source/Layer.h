/*
  ==============================================================================

    Layer.h
    Created: 8 Dec 2016 12:59:46am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LAYER_H_INCLUDED
#define LAYER_H_INCLUDED

#include "BKUtilities.h"

class Layer : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Layer>    Ptr;
    typedef Array<Layer::Ptr>                   Arr;
    typedef Array<Layer::Ptr, CriticalSection>  CSArr;
    
    Layer(BKPreparationType type, int layerNum, int keymapId, int preparationId, int tuningId):
    type(type),
    layerNumber(layerNum),
    keymapId(keymapId),
    preparationId(preparationId),
    tuningId(tuningId)
    {
        
    }
    
    ~Layer()
    {
    }
    
    inline void setType(BKPreparationType val)  {   type = val; print();           }
    inline void setLayerNumber(int val)         {   layerNumber = val; print();      }
    inline void setKeymap(int val)              {   keymapId = val;  print();       }
    inline void setPreparation(int val)         {   preparationId = val; print();   }
    inline void setTuning(int val)              {   tuningId = val; print();        }
    
    inline BKPreparationType getType(void)      {   return type;            }
    inline int getLayerNumber(void)             {   return layerNumber;     }
    inline int getKeymap(void)                  {   return keymapId;        }
    inline int getPreparation(void)             {   return preparationId;   }
    inline int getTuning(void)                  {   return tuningId;        }
    
    
    void print(void)
    {
        DBG("type: " + String(type));
        DBG("layerNum: " + String(layerNumber));
        DBG("keymapId: " + String(keymapId));
        DBG("preparationId: " + String(preparationId));
        DBG("tuningId:" + String(tuningId));
    }
    
private:
    BKPreparationType type;
    int layerNumber;
    int keymapId;
    int preparationId;
    int tuningId;
    
    JUCE_LEAK_DETECTOR(Layer)
};


#endif  // LAYER_H_INCLUDED
