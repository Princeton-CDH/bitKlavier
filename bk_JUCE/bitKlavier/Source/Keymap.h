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

struct Note
{
    int noteNumber;
    float velocity;
    int channel;
    int mappedFrom; // tracks what key was played that triggered this note, for harmonizer purposes
    String source;
};

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
    
    /*
    Sostenuto Implementation Notes
    two cases:
     
        1. an actual sostenuto pedal; most people don't have these!
        2. the sustain pedal behaves like a sostenuto pedal
     
    in case (1), the sustain pedal might also be used, and so should behave as expected
    in case (2), the user has selected "sostenuto mode" in Keymap
     
    Basic process:
        - when any note in the Keymap is played, it is a potential sostenuto note, so it is stored
        - when the sostenuto pedal is pressed (or the sustain pedal, when in "sostenuto mode"), the potential
            sostenuto notes are now active sostenuto notes and should sustain even when their keys are released
        - all non-active sostenuto notes should stop on keyRelease
                - unless the sustain pedal is down (which is not possible in "sostenuto mode")
        - when the sostenuto pedal (or sustain pedal in "sostenuto mode") is released, all active
            sostenuto notes should be stopped
                - unless their keys are still down!
                - or if the sustain pedal is down!
    
    And of course this behavior needs to interface correctly with all the preparations and the harmonizer!
     
    */

    // any key that is currently down is a potential sostenuto note
    void addToPotentialSostenutoNotes(Note newnote) {
        potentialSostenutoNotes.add(newnote);
    }
    
    // keys that are released are now longer potential sostenuto notes and should be removed
    //      note that these will NOT be removed from currently active sostenuto notes
    void removeFromPotentialSostenutoNotes(Note removenote) {
        
        for (int i = potentialSostenutoNotes.size() - 1; i >= 0; i--) {
            if(potentialSostenutoNotes.getUnchecked(i).noteNumber == removenote.noteNumber &&
               (potentialSostenutoNotes.getUnchecked(i).source == removenote.source) &&
               (potentialSostenutoNotes.getUnchecked(i).mappedFrom == removenote.mappedFrom))
                potentialSostenutoNotes.remove(i);
        }
    }
    
    // when the sostentuto pedal is pressed, potential sostenuto notes are now active sostenuto notes
    void copyPotentialToActiveSostenutoNotes() {
        activeSostenutoNotes.clearQuick();
        activeSostenutoNotes = potentialSostenutoNotes;
    }
    
    void print(void);
    
    inline ValueTree getState(void)
    {
        ValueTree keysave;
        
        if ( Id == 255)
            keysave = ValueTree( "upkeymap" );
        else if (Id == 256)
            keysave = ValueTree( "downkeymap");
        else
            keysave = ValueTree(vtagKeymap);
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
            keysave.setProperty(ptagKeymap_targetStates + String(count++), state ? 1 : 0, 0);
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
        keysave.setProperty(ptagKeymap_ignoreNoteOff, ignoreNoteOff ? 1 : 0, 0);
        keysave.setProperty(ptagKeymap_sustainPedalKeys, sustainPedalKeys ? 1 : 0, 0);
        keysave.setProperty(ptagKeymap_toggleKey, isToggle ? 1 : 0, 0);
        keysave.setProperty(ptagKeymap_sostenutoMode, isSostenuto ? 1 : 0, 0);
        //ptagKeymap_sostenutoMode
        //keysave.setProperty(ptagKeymap_extendRange, rangeExtend, 0);
        keysave.setProperty(ptagKeymap_asymmetricalWarp, asym_k, 0);
        keysave.setProperty(ptagKeymap_symmetricalWarp, sym_k, 0);
        keysave.setProperty(ptagKeymap_scale, scale, 0);
        keysave.setProperty(ptagKeymap_offset, offset, 0);
        keysave.setProperty(ptagKeymap_velocityInvert, velocityInvert ? 1 : 0, 0);
        
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

        // If the xml is old enough to not have targetStates saved these should be set to true
        targetStates.setUnchecked(TargetTypeSynchronic, true);
        targetStates.setUnchecked(TargetTypeNostalgic, true);
        for (int i = 0; i < TargetTypeNil; ++i)
        {
            String attr = e->getStringAttribute(ptagKeymap_targetStates + String(i));
            
            if (attr != String())
            {
                // Because this used to be an enum, both 0 and 2 are false and only 1 is true
                targetStates.setUnchecked(i, attr.getIntValue() == 1);
            }
        }
        // These should always be true regardless of what has been saved
        // (until we add other targets to these)
        targetStates.setUnchecked(TargetTypeDirect, true);
        targetStates.setUnchecked(TargetTypeTempo, true);
        targetStates.setUnchecked(TargetTypeTuning, true);

        
        inverted = e->getStringAttribute(ptagKeymap_inverted).getIntValue();
        
        for (auto sub : e->getChildIterator())
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
                for (auto asub : sub->getChildIterator())
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
        setIgnoreNoteOff((bool) e->getIntAttribute(ptagKeymap_ignoreNoteOff, 0));
        setSustainPedalKeys((bool) e->getIntAttribute(ptagKeymap_sustainPedalKeys, 0));
        setIsToggle((bool)e->getIntAttribute(ptagKeymap_toggleKey, 0));
        setIsSostenuto((bool)e->getIntAttribute(ptagKeymap_sostenutoMode, 0));
        // Not sure what value the second argument needs to be. Right now I'm using the default values, but these are the values that bK uses for velocity curving before the view controller is opened and the values update to what they were saved to be.
        //rangeExtend = (float) e->getDoubleAttribute(ptagKeymap_extendRange, 4);
        asym_k = (float) e->getDoubleAttribute(ptagKeymap_asymmetricalWarp, 1);
        sym_k = (float) e->getDoubleAttribute(ptagKeymap_symmetricalWarp, 1);
        scale = (float) e->getDoubleAttribute(ptagKeymap_scale, 1);
        offset = (float) e->getDoubleAttribute(ptagKeymap_offset, 0);
        velocityInvert = (bool) e->getIntAttribute(ptagKeymap_velocityInvert, 0);

        setDefaultSelected((bool) e->getIntAttribute(ptagKeymap_defaultSelected, 1));
        setOnscreenSelected((bool) e->getIntAttribute(ptagKeymap_onscreenSelected, 1));
    }
    
    inline Array<bool> getKeymap(void) const noexcept { return keymap; }
    
    inline const Array<bool>& getTargetStates(void) const noexcept { return targetStates; }
    inline void setTargetStates(Array<bool> ts) { targetStates = ts; }
    
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
    
    void setTarget(KeymapTargetType target, bool state);
    
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
                    int transposed = i - harPreTranspose;
                    if (transposed < 0 || harmonizerKeys.size() <= transposed) continue;
                    compositeString += (String(transposed) + "[");
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
                    if (0 <= harIndex && harIndex < harmonizerKeys.size())
                    {
                        Array<int> arr = keymapStringToIntArray(temp);
                        for (int i = 0; i < arr.size(); ++i)
                            arr.setUnchecked(i, arr.getUnchecked(i) + harIndex);
                        harmonizerKeys.set(harIndex, arr);
                    }
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
                harIndex = temp.getIntValue() + harPreTranspose;
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
    
    float applyVelocityCurve(float velocity);

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
    
    inline bool getIgnoreNoteOff() { return ignoreNoteOff; }
    inline void setIgnoreNoteOff(bool toSet) { ignoreNoteOff = toSet; }
    inline void toggleIgnoreNoteOff() { ignoreNoteOff = !ignoreNoteOff; }
    
    inline bool getSustainPedalKeys() { return sustainPedalKeys; }
    inline void setSustainPedalKeys(bool toSet) { sustainPedalKeys = toSet; }
    inline void toggleSustainPedalKeys() { sustainPedalKeys = !sustainPedalKeys; }
    
    inline bool getIsToggle() { return isToggle; }
    inline void setIsToggle(bool toSet) { isToggle = toSet; }
    inline void toggleIsToggle() { isToggle = !isToggle; }
    
    inline bool getIsSostenuto()            { return isSostenuto; }
    inline void setIsSostenuto(bool toSet)  { isSostenuto = toSet; }
    inline void toggleIsSostenuto()         { isSostenuto = !isSostenuto; }
    inline void activateSostenuto()         { copyPotentialToActiveSostenutoNotes(); }
    inline void deactivateSostenuto()       { activeSostenutoNotes.clearQuick(); }
    
    // this somewhat delicate function sorts out whether a note should be cut off or not
    //      depending on the status of the sostenuto and sustain pedals
    //      and also in case the sustain pedal is set to behave like a sostenuto pedal
    //      lots of contingencies, depending on when and what order pedals were pressed,
    //          what keys are held down at that time, etc...
    inline bool isUnsustainingNote(int noteNumber, bool sostenutoPedalIsDepressed, bool sustainPedalIsDepressed)
    {
        // isSostenuto is when the sustain pedal is set to behave like a sostenuto pedal
        // in these first two cases, that is not true, so we know that note is sustaining if the sustain pedal is pressed
        // and we also know that it is NOT sustaining if neither pedal is depressed
        //if(!isSostenuto && sustainPedalIsDepressed) return false;
        //if(!isSostenuto && !sustainPedalIsDepressed && !sostenutoPedalIsDepressed) return true;
        
        if(!isSostenuto) {
            if (sustainPedalIsDepressed) return false;
            if (!sustainPedalIsDepressed && !sostenutoPedalIsDepressed) return true;
        }
        
        // we are in sostenuto mode, either because of the sostenuto pedal being depressed or being in sostenuto mode
        // in this case, any notes that are not activeSostenutoNotes should NOT sustain
        if (isSostenuto || sostenutoPedalIsDepressed) {
            // here we have no active sostenuto notes, so this must be an unsustaining note
            if (activeSostenutoNotes.size() == 0) return true;
            else {
                for (auto testNote : activeSostenutoNotes)
                {
                    // here we have an active sostenuto note, so it should sustain
                    if (testNote.noteNumber == noteNumber) return false;
                }
                // if we get this far, then the note is not an active sustaining note and should be cut off
                return true;
            }
        }
        
        // we are not in sostenuto mode, so note should sustain
        return false;
    }
    
    inline bool getToggleState(int noteNumber) { return triggered.getUnchecked(noteNumber); }
    //inline bool setToggleState(bool toSet) { trigger.setUnchecked(); }
    inline void toggleToggleState(int noteNumber) { triggered.setUnchecked(noteNumber, !triggered.getUnchecked(noteNumber)); }
    
    // Velocity Curving getters & setters
    inline float getAsym_k() { return asym_k; }
    inline float getSym_k() { return sym_k; }
    inline float getScale() { return scale; }
    inline float getOffset() { return offset; }
    inline bool getVelocityInvert() { return velocityInvert; }
    inline bool didVelocitiesChange() { return velocitiesChanged; }
    
    //inline void setRangeExtend(float newRangeExtend) { rangeExtend = newRangeExtend; }
    inline void setAsym_k(float newAsym_k) { asym_k = newAsym_k; }
    inline void setSym_k(float newSym_k) { sym_k = newSym_k; }
    inline void setScale(float newScale) { scale = newScale; }
    inline void setOffset(float newOffset) { offset = newOffset; }
    inline void setVelocityInvert(bool newVelocityInvert) { velocityInvert = newVelocityInvert; }
    inline void setVelocitiesChanged(bool newVelocitiesChanged) { velocitiesChanged = newVelocitiesChanged; }
    
    // Velocity list handling - for velocity curve graph
    inline void addVelocity(int note, float toAdd)
    { velocities.insert(std::pair<int, float>(note, toAdd)); setVelocitiesChanged(true); }
    inline void removeVelocity(int note)
    { velocities.erase(note); setVelocitiesChanged(true);  }
    inline std::map<int, float>& getVelocities() { return velocities; }
    
private:
    BKAudioProcessor& processor;
    int Id;
    String name;
    Array<bool> keymap;
    
    Array<bool> targetStates;
    
    // Use midi input to edit active keys 
    bool midiEdit;

    bool harMidiEdit;
    bool harArrayMidiEdit;
    
    bool inverted;
    
    Array<bool> toggleState;
    
    Array<bool> triggered;
    
    Array<Array<int>> harmonizerKeys;
    //bool harmonizerEnabled; //functionality has largely been commented out

    Array<String> midiInputNames;
    Array<String> midiInputIdentifiers;
    
    // currently depressed keys in this keymap
    //   that will be included in sostenuto notes when sustain pedal is depressed
    Array<Note> potentialSostenutoNotes;
    
    // notes that were depressed when sostenuto pedal was depressed
    //   these are now active sostenuto notes and should be sustained
    Array<Note> activeSostenutoNotes;
    
    bool defaultSelected = 1;
    bool onscreenSelected = 1;
    
    int harKey;
    int harPreTranspose;
    int harPostTranspose;
    
    // Velocity Curving Params - initalized to default values in the constructors
    //float rangeExtend;
    float asym_k;
    float sym_k;
    float scale;
    float offset;
    bool velocityInvert;
    std::map<int, float> velocities;
    bool velocitiesChanged = false;

    bool ignoreSustain;
    
    bool ignoreNoteOff;
    
    bool allNotesOff;
    
    bool sustainPedalKeys;
    
    bool isToggle;
    
    bool isSostenuto;

    JUCE_LEAK_DETECTOR (Keymap)
};

#endif  // KEYMAP_H_INCLUDED
