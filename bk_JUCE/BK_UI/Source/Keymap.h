/*
  ==============================================================================

    Keymap.h
    Created: 2 Dec 2016 12:23:33pm
    Author:  Michael R Mulshine
 
    A "Keymap" is simply an array of booleans corresponding to the notes on a keyboard
        true => that key/note is prepared in some way

  ==============================================================================
*/

#ifndef KEYMAP_H_INCLUDED
#define KEYMAP_H_INCLUDED

#include "BKUtilities.h"

typedef enum OctatonicType
{
    OctatonicOne,
    OctatonicTwo,
    OctatonicThree,
    OctatonicNil
} OctatonicType;

class Keymap : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Keymap>   Ptr;
    typedef Array<Keymap::Ptr>                  PtrArr;
    typedef Array<Keymap::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Keymap>                  Arr;
    typedef OwnedArray<Keymap, CriticalSection> CSArr;
    
    Keymap(int Id):
    Id(Id),
    keymap(Array<bool>())
    {
        keymap.ensureStorageAllocated(128);
        
        for (int i = 0; i < 128; i++)
        {
            keymap.set(i,false);
        }
    }
    
    Keymap(Keymap::Ptr k):
    Id(k->getId())
    {
        setKeymap(k->keys());
    }
    
    
    ~Keymap()
    {
        
    }
    

    inline bool compare(Keymap::Ptr k)
    {
        for (int i = 0; i < 128; i++)
            if (keymap[i] != k->keymap[i]) return false;
                                  
        return true;
    }
    
    inline void setId(int ident) {   Id = ident; }

    inline void copy(Keymap::Ptr k)
    {
        setKeymap(k->keys());
    }

    inline int getId(void) {   return Id; }
    
    // Returns true if added, false if removed.
    bool toggleNote(int noteNumber)
    {
        bool ret = !keymap[noteNumber];
        
        keymap.set(noteNumber, ret);
        
        return ret;
    }
    
    void setKeymap(Array<int> km)
    {
        clear();
        
        for (auto note : km)
        {
            keymap.set(note, true);
        }
    }
    
    // Returns true if added. Returns false if not added (because it's already there).
    bool addNote(int noteNumber)
    {
        bool ret = !keymap[noteNumber];
        
        keymap.set(noteNumber, true);
        
        return ret;
    }
    
    // Returns true if removed. Returns false if not removed (because it's not there to begin with).
    bool removeNote(int noteNumber)
    {
        bool ret = keymap[noteNumber];
        
        keymap.set(noteNumber, false);
        
        return ret;
    }
    
    
    bool containsNote(int noteNumber)
    {
        return keymap[noteNumber];
    }
    
    // Clears keymap.
    void clear(void)
    {
        for (int note = 0; note < 128; note++)
        {
            keymap.set(note, false);
        }
    }
    
    Array<int> keys(void)
    {
        Array<int> k = Array<int>();
        k.ensureStorageAllocated(128);
        
        for (int note = 0; note < 128; note++)
        {
            if (keymap[note])
            {
                k.add(note);
            }
        }
        
        return k;
    }
    
    void setAllWhiteKeys(void);
    void setAllBlackKeys(void);
    void setAllOctatonicKeys(OctatonicType type);
    
    void print(void)
    {
        DBG("Id: " + String(Id));
        DBG("Keymap: "+ intArrayToString(keys()));
    }
    
    inline ValueTree getState(int ki)
    {
 
        ValueTree keysave( vtagKeymap + String(ki));
        int count = 0;
        for (auto key : keys())
        {
            keysave.setProperty(ptagKeymap_key + String(count++), key, 0);
        }
        
        return keysave;
    }
    
    
private:
    int Id;
    Array<bool> keymap;
    
    JUCE_LEAK_DETECTOR (Keymap)
};



#endif  // KEYMAP_H_INCLUDED
