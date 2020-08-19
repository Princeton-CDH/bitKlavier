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
    
    inline void setMidiEdit(bool edit) { midiEdit = edit; }
    inline void toggleMidiEdit() { midiEdit = !midiEdit; }
    inline bool getMidiEdit() { return midiEdit; }

    inline void setHarMidiEdit(bool edit) { harMidiEdit = edit; }
    inline void toggleHarMidiEdit() { harMidiEdit = !harMidiEdit; }
    inline bool getHarMidiEdit() { return harMidiEdit; }

    inline void setHarArrayMidiEdit(bool edit) { harArrayMidiEdit = edit; }
    inline void toggleHarArrayMidiEdit() { harArrayMidiEdit = !harArrayMidiEdit; }
    inline bool getHarArrayMidiEdit() { return harArrayMidiEdit; }
    
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
        
        ValueTree inputs( vtagKeymap_midiInputNames);
        count = 0;
        for (auto name : getMidiInputNames())
        {
            inputs.setProperty( ptagKeymap_midiInputName + String(count++), name, 0);
        }
        
        ValueTree inputIdentifiers( vtagKeymap_midiInputIdentifiers);
        count = 0;
        for (auto identifier : getMidiInputIdentifiers())
        {
            inputIdentifiers.setProperty( ptagKeymap_midiInputIdentifier + String(count++), identifier, 0);
        }
        
        keysave.addChild(inputs, -1, 0);
        keysave.addChild(inputIdentifiers, -1, 0);
        
        ValueTree harmonizer(vtagKeymap_harmonizer);
        int hCount = 0;
        for (int a = 0; a < 128; a++)
        {
            ValueTree t("t" + String(hCount++));
            count = 0;
            for (auto f : getHarmonizationForKey(a)) t.setProperty(ptagInt + String(count++), f, 0);
            harmonizer.addChild(t, -1, 0);
        }
        keysave.addChild(harmonizer, -1, 0);

        keysave.setProperty(ptagKeymap_harmonizerShift, harShift, 0);

        keysave.setProperty(ptagKeymap_endKeystrokes, allNotesOff ? 1 : 0, 0);
        keysave.setProperty(ptagKeymap_ignoreSustain, ignoreSustain ? 1 : 0, 0);
        
        keysave.setProperty(ptagKeymap_defaultSelected, defaultSelected, 0);
        
        keysave.setProperty(ptagKeymap_onscreenSelected, onscreenSelected, 0);
        
        return keysave;
    }
    
    inline void setState(XmlElement* e)
    {
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String())     name = n;
        else                   name = String(Id);

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
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagKeymap_midiInputNames))
            {
                Array<String> inputs;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagKeymap_midiInputName + String(k));
                    if (attr == String()) continue;
                    inputs.add(attr);
                }
                
                setMidiInputNames(inputs);
            }
            
            if (sub->hasTagName(vtagKeymap_midiInputIdentifiers))
            {
                Array<String> inputs;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagKeymap_midiInputIdentifier + String(k));
                    if (attr == String()) continue;
                    inputs.add(attr);
                }
                
                setMidiInputIdentifiers(inputs);
            }
            
            else if (sub->hasTagName(vtagKeymap_harmonizer))
            {
                int hCount = 0;
                forEachXmlChildElement(*sub, asub)
                {
                    if (asub->hasTagName("t" + String(hCount++)))
                    {
                        Array<int> harKeys;
                        int i;
                        for (int k = 0; k < asub->getNumAttributes(); k++)
                        {
                            String attr = asub->getStringAttribute(ptagInt + String(k));

                            if (attr == String()) break;
                            else
                            {
                                i = attr.getIntValue();
                                harKeys.add(i);
                            }
                        }
                        setHarmonizerList(hCount - 1, harKeys);
                    }
                }
            }
            
        }
        
        n = e->getStringAttribute(ptagKeymap_harmonizerShift);
        if (n != "")
        {
            harShift = n.getIntValue();
        }
        else
        {
            harShift = 0;
        }

        n = e->getStringAttribute(ptagKeymap_endKeystrokes);
        if (n != "") setAllNotesOff((bool)n.getIntValue());
        else setAllNotesOff(false);

        n = e->getStringAttribute(ptagKeymap_ignoreSustain);
        if (n != "") setIgnoreSustain((bool)n.getIntValue());
        else setIgnoreSustain(false);

        n = e->getStringAttribute(ptagKeymap_defaultSelected);
        if (n != "") defaultSelected = n.getIntValue();
        else defaultSelected = true;
        
        n = e->getStringAttribute(ptagKeymap_onscreenSelected);
        if (n != "") onscreenSelected = n.getIntValue();
        else onscreenSelected = true;
    }
    
    inline Array<bool> getKeymap(void) const noexcept { return keymap; }
    
    inline KeymapTargetState getTargetState(KeymapTargetType type) const noexcept { return targetStates[type]; }
    inline Array<KeymapTargetState> getTargetStates(void) const noexcept { return targetStates; }
    inline void setTargetStates(Array<KeymapTargetState> ts) { targetStates = ts; }
    
    inline bool isInverted(void) const noexcept { return inverted; }
    inline void setInverted(bool inv) { inverted = inv; }
    
    inline Array<bool> getTriggeredKeys() const noexcept { return triggered; }
    inline void setTriggered(int key, bool trig) { triggered.set(key, trig); }

    const Array<String> getAllMidiInputNames();
    const Array<String> getAllMidiInputIdentifiers();
    
    inline Array<String> getMidiInputNames(void) { return midiInputNames; }
    inline Array<String> getMidiInputIdentifiers(void) { return midiInputIdentifiers; }

    inline void setMidiInputNames(Array<String> names) { midiInputNames = names; }
    inline void setMidiInputIdentifiers(Array<String> identifiers) { midiInputIdentifiers = identifiers; }
    inline void addMidiInputSource(MidiDeviceInfo source)
    {
        midiInputNames.add(source.name);
        midiInputIdentifiers.add(source.identifier);
    }
    inline void removeMidiInputSource(MidiDeviceInfo source)
    {
        int i = midiInputIdentifiers.indexOf(source.identifier);
        if (i < 0) return;
        midiInputNames.remove(i);
        midiInputIdentifiers.remove(i);
    }
    
    inline bool isDefaultSelected() { return defaultSelected; }
    inline void setDefaultSelected(bool selected) { defaultSelected = selected; }
    
    inline bool isOnscreenSelected() { return onscreenSelected; }
    inline void setOnscreenSelected(bool selected) { onscreenSelected = selected; }
    
    void setTarget(KeymapTargetType target, KeymapTargetState state);
    void toggleTarget(KeymapTargetType target);
    void enableTarget(KeymapTargetType target);
    void disableTarget(KeymapTargetType target);
    void addTarget(KeymapTargetType target);
    void addTarget(KeymapTargetType target, KeymapTargetState state);
    void removeTarget(KeymapTargetType target);
    void removeTargetsOfType(BKPreparationType type);
    void clearTargets(void);
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}

    inline void setAllNotesOff(bool newValue) { allNotesOff = newValue; }
    inline void toggleAllNotesOff() { allNotesOff = !allNotesOff; }
    inline bool getAllNotesOff() { return allNotesOff; }

    inline Array<Array<int>> getHarmonizerKeys() { return harmonizerKeys; }
    inline void setHarmonizerKeys(Array<Array<int>> toSet) { harmonizerKeys = toSet; }

    Array<int> getHarmonizationForKey(int key, bool useShift = false)
    { 
        Array<int> h = Array<int>();
        h.ensureStorageAllocated(128);

        for (int i = 0; i < harmonizerKeys[key].size(); i++)
        {
            if (useShift) h.add(harmonizerKeys[key][i] + harShift);
            else h.add(harmonizerKeys[key][i]);
        }

        return h;
    }

    //overloaded to default to harKey, this saves function calls in KeymapViewController
    Array<int> getHarmonizationForKey(bool useShift = false)
    {
        Array<int> h = Array<int>();
        h.ensureStorageAllocated(128);

        for (int i = 0; i < harmonizerKeys[harKey].size(); i++)
        {
            if (useShift) h.add(harmonizerKeys[harKey][i] + harShift);
            else h.add(harmonizerKeys[harKey][i]);
        }

        return h;
    }
    inline void toggleHarmonizerList(int index, int keyHarmonized) 
    { 
        Array<int> singleHar = harmonizerKeys[index];

        if (harmonizerKeys[index].contains(keyHarmonized))
        {
            singleHar.removeAllInstancesOf(keyHarmonized);
        }
        else
        {
            singleHar.add(keyHarmonized);
        }

        harmonizerKeys.set(index, singleHar);
    }

    //overloaded to default to harKey, this saves function calls in KeymapViewController
    inline void toggleHarmonizerList(int keyHarmonized)
    {
        Array<int> singleHar = harmonizerKeys[harKey];

        if (harmonizerKeys[harKey].contains(keyHarmonized))
        {
            singleHar.removeAllInstancesOf(keyHarmonized);
        }
        else
        {
            singleHar.add(keyHarmonized);
        }

        harmonizerKeys.set(harKey, singleHar);
    }
    inline void setHarmonizerList(int index, Array<int> harmonization) { harmonizerKeys.set(index, harmonization); }
    // overloaded to default to harKey; this avoids pointless function calls
    inline void setHarmonizerList(Array<int> harmonization) { harmonizerKeys.set(harKey, harmonization); }

    void trapKey(int keyToTrap);
    //overloaded to default to harKey, this saves function calls in KeymapViewController
    void trapKey();

    void mirrorKey(int keyCenter);
    //overloaded to default to harKey, this saves function calls in KeymapViewController
    void mirrorKey();

    void defaultHarmonizations();
    void clearHarmonizations();

    /*
    inline bool getHarmonizerEnabled() { return harmonizerEnabled; }
    inline void setHarmonizerEnabled(bool toSet) { harmonizerEnabled = toSet; }
    inline void toggleHarmonizerEnabled() { harmonizerEnabled = !harmonizerEnabled; }
    */

    inline int getHarKey() { return harKey; }
    inline void setHarKey(int toSet) {harKey = toSet;}

    inline int getHarShift() { return harShift; }
    inline void setHarShift(int toSet) { harShift = toSet; }

    inline bool getIgnoreSustain() { return ignoreSustain; }
    inline void setIgnoreSustain(bool toSet) { ignoreSustain = toSet; }
    inline void toggleIgnoreSustain() { ignoreSustain = !ignoreSustain; }
    
private:
    BKAudioProcessor& processor;
    int Id;
    String name;
    Array<bool> keymap;
    
    Array<KeymapTargetState> targetStates;
    
    // Use midi input to edit active keys 
    bool midiEdit;

    bool harMidiEdit;
    bool harArrayMidiEdit;
    
    bool inverted;
    
    Array<bool> triggered;
    
    Array<Array<int>> harmonizerKeys;
    //bool harmonizerEnabled; //functionality has largely been commented out

    int harKey;
    int harShift;

    Array<String> midiInputNames;
    Array<String> midiInputIdentifiers;
    
    bool defaultSelected;
    bool onscreenSelected;
    
    bool allNotesOff;

    bool ignoreSustain;

    JUCE_LEAK_DETECTOR (Keymap)
};

#endif  // KEYMAP_H_INCLUDED
