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

typedef enum KeySet
{
    KeySetAll = 1,
    KeySetAllPC,
    KeySetBlack,
    KeySetWhite,
    KeySetOctatonicOne,
    KeySetOctatonicTwo,
    KeySetOctatonicThree,
    KeySetMajorTriad,
    KeySetMinorTriad,
    KeySetMajorSeven,
    KeySetDomSeven,
    KeySetMinorSeven,
    KeySetMajor,
    KeySetNaturalMinor,
    KeySetHarmonicMinor,
    KeySetNil
} KeySet;

typedef enum OctType
{
    Oct1 = 0,
    Oct2,
    Oct3,
    OctNil
} OctType;

typedef enum ChordType
{
    MajorTriad = 0,
    MinorTriad,
    MajorSeven,
    DomSeven,
    MinorSeven,
    ChordTypeNil
} ChordType;


class Keymap : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Keymap>   Ptr;
    typedef Array<Keymap::Ptr>                  PtrArr;
    typedef Array<Keymap::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Keymap>                  Arr;
    typedef OwnedArray<Keymap, CriticalSection> CSArr;
    
    Array<int> white = {0,2,4,5,7,9,11};
    Array<int> black = {1,3,6,8,10};
    Array<int> octatonic1 = {0,1,3,4,6,7,9,10};
    Array<int> octatonic2 = {1,2,4,5,7,8,10,11};
    Array<int> octatonic3 = {0,2,3,5,6,8,9,11};
    
    Array<int> majortriad = {0,4,7};
    Array<int> minortriad = {0,3,7};
    Array<int> majorseven = {0,4,7,11};
    Array<int> domseven = {0,4,7,10};
    Array<int> minorseven = {0,3,7,10};
    
    Array<int> major = {0,2,4,5,7,9,11};
    Array<int> naturalminor = {0,2,3,5,7,8,10};
    Array<int> harmonicminor = {0,2,3,5,7,8,11};
    
    Array<int> allpc = {0};
    
    Keymap(int Id):
    Id(Id),
    keymap(Array<bool>())
    {
        keymap.ensureStorageAllocated(128);
        
        for (int i = 0; i < 128; i++)
        {
            keymap.add(false);
        }
    }
    
    Keymap(Keymap::Ptr k):
    Id(k->getId())
    {
        keymap.ensureStorageAllocated(128);
        
        for (int i = 0; i < 128; i++)
        {
            keymap.add(false);
        }
        
        setKeymap(k->keys());
    }
    
    inline Keymap::Ptr duplicate(void)
    {
        Keymap::Ptr copyPrep = new Keymap(-1);
        
        copyPrep->setKeymap(keys());
        
        copyPrep->setName(name);
        
        return copyPrep;
    }
    
    Keymap(int Id, Keymap::Ptr k):
    Id(Id)
    {
        keymap.ensureStorageAllocated(128);
        
        for (int i = 0; i < 128; i++)
        {
            keymap.add(false);
        }
        
        setKeymap(k->keys());
    }
    
    Keymap(void):
    Id(-1)
    {
        keymap.ensureStorageAllocated(128);
        
        for (int i = 0; i < 128; i++)
        {
            keymap.add(false);
        }
    }
    
    
    ~Keymap()
    {
        
    }
    

    inline bool compare(Keymap::Ptr k)
    {
        Array<bool> otherKeymap = k->getKeymap();
        for (int i = 0; i < 128; i++)
        {
            if (keymap[i] != otherKeymap[i])
            {
                return false;
            }
        }
    
        return true;
    }

	//totally randomizes keymap
	inline void randomize()
	{
		Random::getSystemRandom().setSeedRandomly();
		keymap.clear();
		for (int i = 0; i < 128; i++)
		{
			keymap.add(i, Random::getSystemRandom().nextBool());
		}
		setName("random");
	}

	//sets keymap to a random keyset
	inline void randomizeKeyset()
	{
		Random::getSystemRandom().setSeedRandomly();
		KeySet set = (KeySet)(Random::getSystemRandom().nextInt(KeySetNil));
		PitchClass pitch = (PitchClass)(Random::getSystemRandom().nextInt(PitchClassNil));
		keymap.clear();
		setKeys(set, true, pitch);
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
    
    void addKeymap(Keymap::Ptr otherKeymap)
    {
        for (auto key : otherKeymap->keys())
        {
            keymap.set(key, true);
        }
    }
    
    void removeKeymap(Keymap::Ptr otherKeymap)
    {
        for (auto key : otherKeymap->keys())
        {
            keymap.set(key, false);
        }
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
    
    void setAll(bool action)
    {
        for (int note = 0; note < 128; note++)
        {
            keymap.set(note, action);
        }
    }
    
    void setWhite(bool action)
    {
        int pc;
        for (int note = 0; note < 128; note++)
        {
            pc = note % 12;
            
            if (white.contains(pc))
            {
                keymap.set(note, action);
            }
        }
    }
    
    void setBlack(bool action)
    {
        int pc;
        for (int note = 0; note < 128; note++)
        {
            pc = note % 12;
            
            if (black.contains(pc))
            {
                keymap.set(note, action);
            }
        }
    }
    
    void setOctatonic(OctType type, bool action)
    {
        int pc;
        Array<int> octatonic;
        if (type == Oct1)       octatonic = octatonic1;
        else if (type == Oct2)  octatonic = octatonic2;
        else if (type == Oct3)  octatonic = octatonic3;
        else                            return;
        
        for (int note = 0; note < 128; note++)
        {
            pc = note % 12;
            
            if (octatonic.contains(pc))
            {
                keymap.set(note, action);
            }
        }
    }
    
    void setChord(KeySet set, PitchClass root, bool action)
    {
        int pc;
        Array<int> chord;
        if      (set == KeySetMajorTriad)          chord = majortriad;
        else if (set == KeySetMinorTriad)          chord = minortriad;
        else if (set == KeySetMajorSeven)          chord = majorseven;
        else if (set == KeySetDomSeven)            chord = domseven;
        else if (set == KeySetMinorSeven)          chord = minorseven;
        else if (set == KeySetAllPC)               chord = allpc;
        else if (set == KeySetMajor)               chord = major;
        else if (set == KeySetNaturalMinor)        chord = naturalminor;
        else if (set == KeySetHarmonicMinor)       chord = harmonicminor;
        else                                       return;
        
        for (int note = 0; note < 128; note++)
        {
            pc = ((note - root) % 12);
            
            if (chord.contains(pc))
            {
                keymap.set(note, action);
            }
        }
    }
    
    void setKeys(KeySet set, bool action, PitchClass pc = PitchClassNil)
    {
        if (set == KeySetAll)
        {
            setAll(action);
        }
        else if (set == KeySetAllPC)
        {
            setChord(set, pc, action);
        }
        else if (set == KeySetBlack)
        {
            setBlack(action);
        }
        else if (set == KeySetWhite)
        {
            setWhite(action);
        }
        else if (set == KeySetOctatonicOne)
        {
            setOctatonic(Oct1,action);
        }
        else if (set == KeySetOctatonicTwo)
        {
            setOctatonic(Oct2,action);
        }
        else if (set == KeySetOctatonicThree)
        {
            setOctatonic(Oct3,action);
        }
        else
        {
            setChord(set, pc, action);
        }
    }
    
    void print(void)
    {
        DBG("Id: " + String(Id));
        DBG("Keymap: "+ intArrayToString(keys()));
    }
    
    inline ValueTree getState(void)
    {
        ValueTree keysave( vtagKeymap );
        
        keysave.setProperty( "Id",Id, 0);
        
        keysave.setProperty("name",name,0);
        
        int count = 0;
        for (auto key : keys())
        {
            keysave.setProperty(ptagKeymap_key + String(count++), key, 0);
        }
        
        return keysave;
    }
    
    inline void setState(XmlElement* e)
    {
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);

        clear();
        for (int k = 0; k < 128; k++)
        {
            String attr = e->getStringAttribute("k" + String(k));
            
            if (attr != String::empty)
            {
                keymap.setUnchecked(attr.getIntValue(), true);
            }
        }
    }
    
    inline Array<bool> getKeymap(void) const noexcept { return keymap; }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}
    
private:
    int Id;
    String name;
    Array<bool> keymap;
    
    JUCE_LEAK_DETECTOR (Keymap)
};

#endif  // KEYMAP_H_INCLUDED
