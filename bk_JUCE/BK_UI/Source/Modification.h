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
    
    inline void setId(int I){ Id = I; }
    inline void setNote(int n){note=n;}
    inline void setPrepId(int Id) { prepId = Id; }
    inline void setModBool(bool val) { modBool = val; }
    inline void setModInt(int val){modInt = val;}
    inline void setModFloat(float val){modFloat = val;}
    inline void setModIntArr(Array<int> arr){modIntArr = arr;}
    inline void setModFloatArr(Array<float> arr){modFloatArr = arr;}
    
    inline void setModArrFloatArr(Array<Array<float>> afarr)
    {
        modArrFloatArr.clear();
        for (auto arr : afarr)
        {
            modArrFloatArr.add(arr);
        }
    }

    inline const int getId(void){return Id;}
    inline const int getNote(void){return note;}
    inline const int getPrepId(void) { return prepId; }
    inline const bool getModBool(void){return modBool;}
    inline const int getModInt(void){return modInt;}
    inline const float getModFloat(void){return modFloat;}
    inline const Array<float> getModFloatArr(void){return modFloatArr;}
    
    inline const Array<Array<float>> getModArrFloatArr(void){return modArrFloatArr;}
    inline const Array<int> getModIntArr(void){return modIntArr;}
    
    
protected:
    int             Id;
    int             note;
    int             prepId;
    bool            modBool;
    int             modInt;
    float           modFloat;
    Array<int>      modIntArr;
    Array<float>    modFloatArr;
    Array<Array<float>> modArrFloatArr;
    
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
    
    
    DirectModification(int key, int whichPrep, DirectParameterType type, String val, int ident):
    type(type)
    {
        modFloatArr = Array<float>();
        modIntArr   = Array<int>();
        modBool = var::null;
        modInt = var::null;
        modFloat = var::null;
        
        Id = ident;
        note = key;
        prepId = whichPrep;
        
        if (cDirectDataTypes[type] == BKInt)
        {
            modInt = val.getIntValue();
        }
        else if (cDirectDataTypes[type] == BKIntArr)
        {
            modIntArr = stringToIntArray(val);
        }
        else if (cDirectDataTypes[type] == BKFloatArr)
        {
            modFloatArr = stringToFloatArray(val);
        }
        else // BKFloat
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
    
    SynchronicModification(int key, int whichPrep, SynchronicParameterType type, String val, int ident):
    type(type)
    {
        modArrFloatArr = Array<Array<float>>();
        modFloatArr = Array<float>();
        modIntArr   = Array<int>();
        modBool = var::null;
        modInt = var::null;
        modFloat = var::null;
        
        Id = ident;
        note = key;
        prepId = whichPrep;
        
        if (cSynchronicDataTypes[type] == BKInt)
        {
            modInt = val.getIntValue();
        }
        else if (cSynchronicDataTypes[type] == BKFloat) // BKFloat
        {
            modFloat = val.getFloatValue();
        }
        else if (cSynchronicDataTypes[type] == BKFloatArr)
        {
            modFloatArr = stringToFloatArray(val);
        }
        else if (cSynchronicDataTypes[type] == BKIntArr)
        {
            modIntArr = stringToIntArray(val);
        }
        else if (cSynchronicDataTypes[type] == BKArrFloatArr)
        {
            modArrFloatArr = stringToArrayFloatArray(val);
        }
            
        
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
    
    NostalgicModification(int key, int whichPrep, NostalgicParameterType type, String val, int ident):
    type(type)
    {
        modFloatArr = Array<float>();
        modIntArr   = Array<int>();
        modBool = var::null;
        modInt = var::null;
        modFloat = var::null;
        
        Id = ident;
        note = key;
        prepId = whichPrep;
        
        if (cNostalgicDataTypes[type] == BKInt)
        {
            modInt = val.getIntValue();
        }
        else if (cNostalgicDataTypes[type] == BKFloat) // BKFloat
        {
            modFloat = val.getFloatValue();
        }
        else if (cNostalgicDataTypes[type] == BKFloatArr)
        {
            modFloatArr = stringToFloatArray(val);
        }
        else if (cNostalgicDataTypes[type] == BKIntArr)
        {
            modIntArr = stringToIntArray(val);
        }
       
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
    
    TuningModification(int key, int whichPrep, TuningParameterType type, String val, int ident):
    type(type)
    {
        modFloatArr = Array<float>();
        modIntArr = Array<int>();
        modBool = var::null;
        modInt = var::null;
        modFloat = var::null;
        
        Id = ident;
        note = key;
        prepId = whichPrep;
        
        if (cTuningDataTypes[type] == BKInt)
        {
            modInt = val.getIntValue();
        }
        else if (cTuningDataTypes[type] == BKFloat)
        {
            modFloat = val.getFloatValue();
        }
        else if (cTuningDataTypes[type] == BKFloatArr)
        {
            modFloatArr = stringToFloatArray(val);
        }
        else if (cTuningDataTypes[type] == BKBool)
        {
            modBool = (bool)val.getIntValue();
        }
        else if (cTuningDataTypes[type] == BKIntArr)
        {
            modIntArr = stringToIntArray(val);
        }
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

class TempoModification : public Modification
{
public:
    typedef ReferenceCountedObjectPtr<TempoModification>   Ptr;
    typedef Array<TempoModification::Ptr>                  PtrArr;
    typedef Array<TempoModification::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TempoModification>                  Arr;
    typedef OwnedArray<TempoModification, CriticalSection> CSArr;
    
    TempoModification(int key, int whichPrep, TempoParameterType type, String val, int ident):
    type(type)
    {
        modFloatArr = Array<float>();
        modIntArr = Array<int>();
        modBool = var::null;
        modInt = var::null;
        modFloat = var::null;
        
        Id = ident;
        note = key;
        prepId = whichPrep;
        
        if (cTempoDataTypes[type] == BKInt)
        {
            modInt = val.getIntValue();
        }
        else if (cTempoDataTypes[type] == BKFloat)
        {
            modFloat = val.getFloatValue();
        }
        else if (cTempoDataTypes[type] == BKFloatArr)
        {
            modFloatArr = stringToFloatArray(val);
        }
        else if (cTempoDataTypes[type] == BKBool)
        {
            modBool = (bool)val.getIntValue();
        }
        else if (cTempoDataTypes[type] == BKIntArr)
        {
            modIntArr = stringToIntArray(val);
        }
    }
    
    
    ~TempoModification(void)
    {
        
    }
    
    inline void setParameterType(TempoParameterType t) { type = t; }
    
    inline TempoParameterType getParameterType(void) {return type; }
    
private:
    
    TempoParameterType type;
    
    JUCE_LEAK_DETECTOR(TempoModification)
};


#endif  // MODIFICATION_H_INCLUDED
