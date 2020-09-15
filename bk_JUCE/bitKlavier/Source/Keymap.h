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
    bool containsNoteMapping(int noteNumber, int mappedFrom);
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
            for (auto f : getHarmonizationForKey(a, false, false)) t.setProperty(ptagInt + String(count++), f, 0);
            harmonizer.addChild(t, -1, 0);
        }
        keysave.addChild(harmonizer, -1, 0);

        keysave.setProperty(ptagKeymap_harmonizerPreTranspose, harPreTranspose, 0);
        keysave.setProperty(ptagKeymap_harmonizerPostTranspose, harPostTranspose, 0);

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
        
        harPreTranspose = e->getIntAttribute(ptagKeymap_harmonizerPreTranspose, 0);
        harPostTranspose = e->getIntAttribute(ptagKeymap_harmonizerPostTranspose, 0);

        setAllNotesOff((bool) e->getIntAttribute(ptagKeymap_endKeystrokes, 0));
        setIgnoreSustain((bool) e->getIntAttribute(ptagKeymap_ignoreSustain, 0));

        setDefaultSelected((bool) e->getIntAttribute(ptagKeymap_defaultSelected, 1));
        setOnscreenSelected((bool) e->getIntAttribute(ptagKeymap_onscreenSelected, 1));
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
    void setDefaultSelected(bool selected);
    
    inline bool isOnscreenSelected() { return onscreenSelected; }
    void setOnscreenSelected(bool selected);
    
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

    String getHarmonizerTextForDisplay()
    {
        String compositeString = "";
        for (int i = 0; i < harmonizerKeys.size(); i++)
        {
            if (harmonizerKeys[i].size() > 0)
            {
                if (harmonizerKeys[i].size() > 1 ||
                    (harmonizerKeys[i].size() == 1 && harmonizerKeys[i][0] != i))
                {
                    compositeString += (String(i) + "[");
                    for (int j = 0; j < harmonizerKeys[i].size(); j++)
                    {
                        compositeString += (String(harmonizerKeys[i][j] - i)); //using the half step offset rather than the absoute midi value
                        if (j < harmonizerKeys[i].size() - 1) compositeString += " ";
                    }
                    compositeString += "] ";
                }
            }
        }

        return compositeString;
    }

    void setHarmonizerFromDisplayText(String s)
    {
        String temp = "";
        
        bool inBracket = false;
        
        String::CharPointerType c = s.getCharPointer();
        
        juce_wchar dash = '-';
        juce_wchar open = '[';
        juce_wchar close = ']';
        
        int harIndex;
        
        for (int i = 0; i < (s.length()+1); i++)
        {
            juce_wchar c1 = c.getAndAdvance();
            
            bool isOpen = !CharacterFunctions::compare(c1, open);
            bool isClose = !CharacterFunctions::compare(c1, close);
            bool isNumChar = CharacterFunctions::isDigit(c1) || !CharacterFunctions::compare(c1, dash);
            
            if (inBracket)
            {
                if (isClose)
                {
                    Array<int> arr = keymapStringToIntArray(temp);
                    for (int i = 0; i < arr.size(); ++i)
                        arr.setUnchecked(i, arr.getUnchecked(i) + harIndex);
                    harmonizerKeys.set(harIndex, arr);
                    temp = "";
                    inBracket = false;
                }
                else temp += c1;
            }
            else if (isNumChar)
            {
                temp += c1;
            }
            else if (isOpen)
            {
                harIndex = temp.getIntValue();
                temp = "";
                inBracket = true;
            }
        }
    }

    Array<int> getHarmonizationForKey(int key, bool withPreTranspose, bool withPostTranspose)
    { 
        Array<int> h = Array<int>();
        h.ensureStorageAllocated(128);
        
        if (withPreTranspose) key += harPreTranspose;
        
        if (key > harmonizerKeys.size()) return h;

        for (int i = 0; i < harmonizerKeys[key].size(); i++)
        {
            if (withPostTranspose) h.add(harmonizerKeys[key][i] + harPostTranspose);
            else h.add(harmonizerKeys[key][i]);
        }

        return h;
    }

    //overloaded to default to harKey, this saves function calls in KeymapViewController
    Array<int> getHarmonizationForKey(bool withPreTranspose, bool withPostTranspose)
    {
        Array<int> h = Array<int>();
        h.ensureStorageAllocated(128);
        
        int tempHarKey = harKey;
        
        if (withPreTranspose) tempHarKey += harPreTranspose;
        
        if (tempHarKey > harmonizerKeys.size()) return h;

        for (int i = 0; i < harmonizerKeys[tempHarKey].size(); i++)
        {
            if (withPostTranspose) h.add(harmonizerKeys[tempHarKey][i] + harPostTranspose);
            else h.add(harmonizerKeys[tempHarKey][i]);
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
        int tempHarKey = harKey + harPreTranspose;
        if (tempHarKey < 0) return;
            
        Array<int> singleHar = harmonizerKeys[tempHarKey];

        if (harmonizerKeys[tempHarKey].contains(keyHarmonized))
        {
            singleHar.removeAllInstancesOf(keyHarmonized);
        }
        else
        {
            singleHar.add(keyHarmonized);
        }

        harmonizerKeys.set(tempHarKey, singleHar);
    }
    inline void setHarmonizerList(int index, Array<int> harmonization) { harmonizerKeys.set(index, harmonization); }

    void copyKeyPatternToAll(int keyToCopy);
    void copyKeyMappingToAll(int keyToTrap);
    void copyKeyPatternToOctaves(int keyToCopy);
    void copyKeyMappingToOctaves(int keyToTrap);
    void mirrorKey(int keyCenter);

    void defaultHarmonizations();
    void clearHarmonizations();

    /*
    inline bool getHarmonizerEnabled() { return harmonizerEnabled; }
    inline void setHarmonizerEnabled(bool toSet) { harmonizerEnabled = toSet; }
    inline void toggleHarmonizerEnabled() { harmonizerEnabled = !harmonizerEnabled; }
    */

    inline int getHarKey() { return harKey; }
    inline void setHarKey(int toSet) {harKey = toSet;}
    
    inline int getHarPreTranspose() { return harPreTranspose; }
    inline void setHarPreTranspose(int toSet) { harPreTranspose = toSet; }

    inline int getHarPostTranspose() { return harPostTranspose; }
    inline void setHarPostTranspose(int toSet) { harPostTranspose = toSet; }

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

    Array<String> midiInputNames;
    Array<String> midiInputIdentifiers;
    
    bool defaultSelected;
    bool onscreenSelected;
    
    int harKey;
    int harPreTranspose;
    int harPostTranspose;

    bool ignoreSustain;
    
    bool allNotesOff;

    JUCE_LEAK_DETECTOR (Keymap)
};

#endif  // KEYMAP_H_INCLUDED
