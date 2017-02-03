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
    Modification()
    {
        
    }
    
    ~Modification()
    {
        
    }
    
    inline void setPrepId(int Id) { prepId = Id; }
    inline void setModBool(bool val) { modBool = val; }
    inline void setModInt(int val){modInt = val;}
    inline void setModFloat(float val){modFloat = val;}
    inline void setModIntArr(Array<int> arr){modIntArr = arr;}
    inline void setModFloatArr(Array<float> arr){modFloatArr = arr;}
    
    inline const int getPrepId(void) { return prepId; }
    inline const bool getModBool(void){return modBool;}
    inline const int getModInt(void){return modInt;}
    inline const float getModFloat(void){return modFloat;}
    inline const Array<float> getModFloatArr(void){return modFloatArr;}
    inline const Array<int> getModIntArr(void){return modIntArr;}
    
    
protected:
    int             prepId;
    bool            modBool;
    int             modInt;
    float           modFloat;
    Array<int>      modIntArr;
    Array<float>    modFloatArr;
    
private:
    
    
    JUCE_LEAK_DETECTOR(Modification)
};

class DirectModification : public Modification
{
public:
    typedef ReferenceCountedObjectPtr<DirectModification>   Ptr;
    typedef Array<DirectModification::Ptr>                  PtrArr;
    typedef Array<DirectModification::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<DirectModification>                  Arr;
    typedef OwnedArray<DirectModification, CriticalSection> CSArr;
    
    
    DirectModification(int whichPrep, DirectParameterType type, String val):
    type(type)
    {
        prepId = whichPrep;
        
        if (type == DirectTuning)
        {
            modInt = val.getIntValue();
        }
        else // Gains
        {
            modFloat = val.getFloatValue();
        }
    }
    
    ~DirectModification(void)
    {
        
    }
    
    inline void setParameterType(DirectParameterType t) { type = t; }
    
    inline DirectParameterType getParameterType(void) {return type; }
    
private:
    int Id;
    DirectParameterType type;
    
    JUCE_LEAK_DETECTOR(DirectModification)
};

class SynchronicModification : public Modification
{
public:
    typedef ReferenceCountedObjectPtr<SynchronicModification>   Ptr;
    typedef Array<SynchronicModification::Ptr>                  PtrArr;
    typedef Array<SynchronicModification::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicModification>                  Arr;
    typedef OwnedArray<SynchronicModification, CriticalSection> CSArr;
    
    SynchronicModification(SynchronicParameterType type, int whichPrep, int            val):
    type(type)
    {
        prepId = whichPrep;
        modInt = val;
    }
    
    SynchronicModification(SynchronicParameterType type, int whichPrep, float          val):
    type(type)
    {
        prepId = whichPrep;
        modFloat = val;
    }
    
    SynchronicModification(SynchronicParameterType type, int whichPrep, Array<float> val):
    type(type)
    {
        prepId = whichPrep;
        modFloatArr = val;
    }
    
    ~SynchronicModification(void)
    {
        
    }
    
    inline void setParameterType(SynchronicParameterType t) { type = t; }
    
    inline SynchronicParameterType getParameterType(void) {return type; }
    
private:
    
    SynchronicParameterType type;
    
    JUCE_LEAK_DETECTOR(SynchronicModification)
};


class NostalgicModification : public Modification
{
public:
    typedef ReferenceCountedObjectPtr<NostalgicModification>   Ptr;
    typedef Array<NostalgicModification::Ptr>                  PtrArr;
    typedef Array<NostalgicModification::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicModification>                  Arr;
    typedef OwnedArray<NostalgicModification, CriticalSection> CSArr;
    
    NostalgicModification(NostalgicParameterType type, int whichPrep, int            val):
    type(type)
    {
        prepId = whichPrep;
        modInt = val;
    }
    
    NostalgicModification(NostalgicParameterType type, int whichPrep, float          val):
    type(type)
    {
        prepId = whichPrep;
        modFloat = val;
    }
    
    ~NostalgicModification(void)
    {
        
    }
    
    inline void setParameterType(NostalgicParameterType t) { type = t; }
    
    inline NostalgicParameterType getParameterType(void) {return type; }
    
private:
    
    NostalgicParameterType type;
    
    JUCE_LEAK_DETECTOR(NostalgicModification)
};

class TuningModification : public Modification
{
public:
    typedef ReferenceCountedObjectPtr<TuningModification>   Ptr;
    typedef Array<TuningModification::Ptr>                  PtrArr;
    typedef Array<TuningModification::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TuningModification>                  Arr;
    typedef OwnedArray<TuningModification, CriticalSection> CSArr;
    
    TuningModification(TuningParameterType type, int whichPrep, int            val):
    type(type)
    {
        prepId = whichPrep;
        modInt = val;
    }
    
    TuningModification(TuningParameterType type, int whichPrep, float          val):
    type(type)
    {
        prepId = whichPrep;
        modFloat = val;
    }
    
    TuningModification(TuningParameterType type, int whichPrep, Array<float> val):
    type(type)
    {
        prepId = whichPrep;
        modFloatArr = val;
    }
    
    TuningModification(TuningParameterType type, int whichPrep, bool val):
    type(type)
    {
        prepId = whichPrep;
        modBool = val;
    }
    
    
    ~TuningModification(void)
    {
        
    }
    
    inline void setParameterType(TuningParameterType t) { type = t; }
    
    inline TuningParameterType getParameterType(void) {return type; }
    
private:
    
    TuningParameterType type;
    
    JUCE_LEAK_DETECTOR(TuningModification)
};


#endif  // MODIFICATION_H_INCLUDED
