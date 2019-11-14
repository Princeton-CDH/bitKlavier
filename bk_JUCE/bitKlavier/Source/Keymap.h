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

class BKAudioProcessor;

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


class Keymap : public ReferenceCountedObject,
               public MidiInputCallback
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
    
    Keymap(BKAudioProcessor& processor);
    Keymap(BKAudioProcessor& processor, int Id);
    Keymap(BKAudioProcessor& processor, Keymap::Ptr k);
    Keymap(BKAudioProcessor& processor, int Id, Keymap::Ptr k);
    ~Keymap();
    
    inline Keymap::Ptr duplicate(void)
    {
        Keymap::Ptr copyPrep = new Keymap(processor, -1);
        
        copyPrep->setKeymap(keys());
        
        copyPrep->setName(name);
        
        return copyPrep;
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
    
    bool toggleNote(int noteNumber);
    bool addNote(int noteNumber);
    bool removeNote(int noteNumber);
    void addKeymap(Keymap::Ptr otherKeymap);
    void removeKeymap(Keymap::Ptr otherKeymap);
    bool containsNote(int noteNumber);
    void clear(void);
    Array<int> keys(void);
    
    void setKeymap(Array<int> km);
    void setAll(bool action);
    void setWhite(bool action);
    void setBlack(bool action);
    void setOctatonic(OctType type, bool action);
    void setChord(KeySet set, PitchClass root, bool action);
    void setKeys(KeySet set, bool action, PitchClass pc = PitchClassNil);
    
    void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override;
    
    void print(void);
    
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
        
        count = 0;
        for (auto state : getTargetStates())
        {
            keysave.setProperty(ptagKeymap_targetStates + String(count++), state, 0);
        }
        
        keysave.setProperty(ptagKeymap_inverted, isInverted(), 0);
        
        return keysave;
    }
    
    inline void setState(XmlElement* e)
    {
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String())     name = n;
        else                        name = String(Id);

        clear();
        for (int k = 0; k < 128; k++)
        {
            String attr = e->getStringAttribute(ptagKeymap_key + String(k));
            
            if (attr != String())
            {
                keymap.setUnchecked(attr.getIntValue(), true);
            }
        }
        for (int i = 0; i < TargetTypeNil; ++i)
        {
            String attr = e->getStringAttribute(ptagKeymap_targetStates + String(i));
            
            if (attr != String())
            {
                targetStates.setUnchecked(i, (KeymapTargetState) attr.getIntValue());
            }
        }
        
        inverted = e->getStringAttribute(ptagKeymap_inverted).getIntValue();
        
    }
    
    inline Array<bool> getKeymap(void) const noexcept { return keymap; }
    
    inline KeymapTargetState getTargetState(KeymapTargetType type) const noexcept { return targetStates[type]; }
    inline Array<KeymapTargetState> getTargetStates(void) const noexcept { return targetStates; }
    inline void setTargetStates(Array<KeymapTargetState> ts) { targetStates = ts; }
    
    inline bool isInverted(void) const noexcept { return inverted; }
    inline void setInverted(bool inv) { inverted = inv; }
    
    inline const std::shared_ptr<MidiInput> getMidiInput() const noexcept { return midiInput; }
    inline const String getMidiInputName() const noexcept { return midiInputName; }
    inline const void setMidiInput(std::shared_ptr<MidiInput> input)
    {
        midiInput = input;
        if (midiInput == nullptr) midiInputName = keymapDefaultMidiInputIdentifier;
        else midiInputName = midiInput->getName();
    }
    
    void setTarget(KeymapTargetType target, KeymapTargetState state);
    void toggleTarget(KeymapTargetType target);
    void enableTarget(KeymapTargetType target);
    void disableTarget(KeymapTargetType target);
    void addTarget(KeymapTargetType target);
    void removeTarget(KeymapTargetType target);
    void removeTargetsOfType(BKPreparationType type);
    void clearTargets(void);
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}
    
private:
    BKAudioProcessor& processor;
    int Id;
    String name;
    Array<bool> keymap;
    
    Array<KeymapTargetState> targetStates;
    
    bool inverted;
    
    std::shared_ptr<MidiInput> midiInput;
    String midiInputName;
    
    JUCE_LEAK_DETECTOR (Keymap)
};

#endif  // KEYMAP_H_INCLUDED
