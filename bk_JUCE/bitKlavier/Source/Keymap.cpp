/*
  ==============================================================================

    Keymap.cpp
    Created: 19 Sep 2019 2:08:25pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Keymap.h"

#include "PluginProcessor.h"

Keymap::Keymap(BKAudioProcessor& processor, int Id):
processor(processor),
Id(Id),
name("Keymap "+String(Id)),
keymap(Array<bool>()),
targetStates(Array<bool>()),
midiEdit(false),
harMidiEdit(false),
harArrayMidiEdit(false),
inverted(false),
triggered(false),
midiInputNames(Array<String>()),
midiInputIdentifiers(Array<String>()),
defaultSelected(true),
onscreenSelected(true),
//harmonizerEnabled(false),
harKey(60),
harPreTranspose(0),
harPostTranspose(0),
//rangeExtend(4),
asym_k(1),
sym_k(1),
scale(1),
offset(0),
isToggle(false),
velocityInvert(false),
ignoreSustain(false),
allNotesOff(false),
sustainPedalKeys(false)
{
    keymap.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        keymap.add(false);
    }
    
    triggered.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        triggered.add(false);
    }
    
    targetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(false);
    }
    
    targetStates.setUnchecked(TargetTypeDirect, true);
    targetStates.setUnchecked(TargetTypeSynchronic, true);
    targetStates.setUnchecked(TargetTypeNostalgic, true);
    targetStates.setUnchecked(TargetTypeResonanceAdd, true);
    targetStates.setUnchecked(TargetTypeResonanceRing, true);
    targetStates.setUnchecked(TargetTypeTempo, true);
    targetStates.setUnchecked(TargetTypeTuning, true);
    
    harmonizerKeys.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        harmonizerKeys.add(Array<int>(i)); //"default" harmonizer for each key is to just play itself
    }
}

Keymap::Keymap(BKAudioProcessor& processor, Keymap::Ptr k):
processor(processor),
Id(k->getId()),
name("Keymap "+String(Id)),
midiEdit(false),
harMidiEdit(false),
harArrayMidiEdit(false),
inverted(false),
midiInputNames(k->getMidiInputNames()),
midiInputIdentifiers(k->getMidiInputIdentifiers()),
defaultSelected(k->isDefaultSelected()),
onscreenSelected(k->isOnscreenSelected()),
//harmonizerEnabled(k->getHarmonizerEnabled()),
harKey(k->getHarKey()),
harPreTranspose(k->getHarPreTranspose()),
harPostTranspose(k->getHarPostTranspose()),
//rangeExtend(k->getRangeExtend()),
asym_k(k->getAsym_k()),
sym_k(k->getSym_k()),
scale(k->getScale()),
offset(k->getOffset()),
velocityInvert(k->getVelocityInvert()),
ignoreSustain(k->getIgnoreSustain()),
allNotesOff(k->getAllNotesOff()),
sustainPedalKeys(k->getSustainPedalKeys()),
isToggle(k->getIsToggle())
{
    keymap.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        keymap.add(false);
    }
    setKeymap(k->keys());
    
    targetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(false);
    }
    
    targetStates.setUnchecked(TargetTypeDirect, true);
    targetStates.setUnchecked(TargetTypeSynchronic, true);
    targetStates.setUnchecked(TargetTypeNostalgic, true);
    targetStates.setUnchecked(TargetTypeResonanceAdd, true);
    targetStates.setUnchecked(TargetTypeResonanceRing, true);
    targetStates.setUnchecked(TargetTypeTempo, true);
    targetStates.setUnchecked(TargetTypeTuning, true);
    
    inverted = k->isInverted();

    harmonizerKeys.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        Array<int> otherArr = k->getHarmonizationForKey(i, false, false);
        if (otherArr.size() == 0) harmonizerKeys.add(Array<int>(i));
        else
        {
            harmonizerKeys.add(Array<int>({})); //"default" harmonizer for each key is to just play itself
            for (int j = 0; j < otherArr.size(); j++)
            {
                harmonizerKeys[i].add(otherArr.getUnchecked(j));
            }
        }
    }
}

Keymap::Keymap(BKAudioProcessor& processor, int Id, Keymap::Ptr k):
processor(processor),
Id(Id),
name("Keymap "+String(Id)),
midiEdit(false),
harMidiEdit(false),
harArrayMidiEdit(false),
inverted(false),
midiInputNames(k->getMidiInputNames()),
midiInputIdentifiers(k->getMidiInputIdentifiers()),
defaultSelected(k->isDefaultSelected()),
onscreenSelected(k->isOnscreenSelected()),
//harmonizerEnabled(false),
harKey(k->getHarKey()),
harPreTranspose(k->getHarPreTranspose()),
harPostTranspose(k->getHarPostTranspose()),
//rangeExtend(k->getRangeExtend()),
asym_k(k->getAsym_k()),
sym_k(k->getSym_k()),
scale(k->getScale()),
offset(k->getOffset()),
velocityInvert(k->getVelocityInvert()),
ignoreSustain(k->getIgnoreSustain()),
allNotesOff(k->getAllNotesOff()),
sustainPedalKeys(k->getSustainPedalKeys()),
isToggle(k->getIsToggle())
{
    keymap.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        keymap.add(false);
    }
    setKeymap(k->keys());
    
    targetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(false);
    }
    targetStates.setUnchecked(TargetTypeDirect, true);
    targetStates.setUnchecked(TargetTypeSynchronic, true);
    targetStates.setUnchecked(TargetTypeNostalgic, true);
    targetStates.setUnchecked(TargetTypeResonanceAdd, true);
    targetStates.setUnchecked(TargetTypeResonanceRing, true);
    targetStates.setUnchecked(TargetTypeTempo, true);
    targetStates.setUnchecked(TargetTypeTuning, true);
    
    inverted = k->isInverted();

    harmonizerKeys.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        Array<int> otherArr = k->getHarmonizationForKey(i, false, false);
        if (otherArr.size() == 0) harmonizerKeys.add(Array<int>(i));
        else
        {
            harmonizerKeys.add(Array<int>({})); //"default" harmonizer for each key is to just play itself
            for (int j = 0; j < otherArr.size(); j++)
            {
                harmonizerKeys[i].add(otherArr.getUnchecked(j));
            }
        }
    }
    //setHarmonizerKeys(k->getHarmonizerKeys());
}

Keymap::Keymap(BKAudioProcessor& processor):
processor(processor),
Id(-1),
name("Keymap "+String(Id)),
keymap(Array<bool>()),
targetStates(Array<bool>()),
midiEdit(false),
harMidiEdit(false),
harArrayMidiEdit(false),
inverted(false),
midiInputNames(Array<String>()),
midiInputIdentifiers(Array<String>()),
defaultSelected(true),
onscreenSelected(true),
//harmonizerEnabled(false),
harKey(60),
harPreTranspose(0),
harPostTranspose(0),
//rangeExtend(4),
asym_k(1),
sym_k(1),
scale(1),
offset(0),
velocityInvert(false),
ignoreSustain(false),
allNotesOff(false),
sustainPedalKeys(false),
isToggle(false)
{
    keymap.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        keymap.add(false);
    }
    
    targetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(false);
    }
    targetStates.setUnchecked(TargetTypeDirect, true);
    targetStates.setUnchecked(TargetTypeSynchronic, true);
    targetStates.setUnchecked(TargetTypeNostalgic, true);
    targetStates.setUnchecked(TargetTypeResonanceAdd, true);
    targetStates.setUnchecked(TargetTypeResonanceRing, true);
    targetStates.setUnchecked(TargetTypeTempo, true);
    targetStates.setUnchecked(TargetTypeTuning, true);

    harmonizerKeys.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        harmonizerKeys.add(Array<int>(i)); //"default" harmonizer for each key is to just play itself
    }
}

Keymap::~Keymap()
{
    
}

// Returns true if added, false if removed.
bool Keymap::toggleNote(int noteNumber)
{
    bool ret = !keymap[noteNumber];
    
    keymap.set(noteNumber, ret);
    
    return ret;
}

void Keymap::setKeymap(Array<int> km)
{
    clear();
    
    for (auto note : km)
    {
        if (note > 0) keymap.set(note, true);
    }
}

// Returns true if added. Returns false if not added (because it's already there).
bool Keymap::addNote(int noteNumber)
{
    bool ret = !keymap[noteNumber];
    
    keymap.set(noteNumber, true);
    
    return ret;
}

// Returns true if removed. Returns false if not removed (because it's not there to begin with).
bool Keymap::removeNote(int noteNumber)
{
    bool ret = keymap[noteNumber];
    
    keymap.set(noteNumber, false);
    
    return ret;
}

void Keymap::addKeymap(Keymap::Ptr otherKeymap)
{
    for (auto key : otherKeymap->keys())
    {
        keymap.set(key, true);
    }
}

void Keymap::removeKeymap(Keymap::Ptr otherKeymap)
{
    for (auto key : otherKeymap->keys())
    {
        keymap.set(key, false);
    }
}

bool Keymap::containsNote(int noteNumber)
{
    return keymap[noteNumber];
}

bool Keymap::containsNoteMapping(int noteNumber, int mappedFrom)
{
    if (!keymap.getUnchecked(mappedFrom)) return false;
    return harmonizerKeys.getUnchecked(mappedFrom + harPreTranspose).contains(noteNumber - harPostTranspose);
}

// Clears keymap.
void Keymap::clear(void)
{
    for (int note = 0; note < 128; note++)
    {
        keymap.set(note, false);
    }
}

Array<int> Keymap::keys(void)
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

void Keymap::setAll(bool action)
{
    for (int note = 0; note < 128; note++)
    {
        keymap.set(note, action);
    }
}

void Keymap::setWhite(bool action)
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

void Keymap::setBlack(bool action)
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

void Keymap::setOctatonic(OctType type, bool action)
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

void Keymap::setChord(KeySet set, PitchClass root, bool action)
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

void Keymap::setKeys(KeySet set, bool action, PitchClass pc)
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

void Keymap::setTarget(KeymapTargetType target, bool state)
{
    targetStates.setUnchecked(target, state);
    DBG("Keymap::setTarget = " + String(target) + " " + String((int) getTargetStates()[target]));
}

const Array<String> Keymap::getAllMidiInputNames()
{
    Array<String> sources;
    sources.addArray(midiInputNames);
    sources.add(cMidiInputDAW);
    if (onscreenSelected) sources.add(cMidiInputUI);
    if (defaultSelected) sources.addArray(processor.getDefaultMidiInputNames());
    return sources;
}

const Array<String> Keymap::getAllMidiInputIdentifiers()
{
    Array<String> sources;
    sources.addArray(midiInputIdentifiers);
    sources.add(cMidiInputDAW);
    if (onscreenSelected) sources.add(cMidiInputUI);
    if (defaultSelected) sources.addArray(processor.getDefaultMidiInputIdentifiers());
    return sources;
}

void Keymap::print(void)
{
    DBG("Id: " + String(Id));
    DBG("Keymap: "+ intArrayToString(keys()));
}

void Keymap::copyKeyPatternToAll(int keyToCopy)
{
    keyToCopy += harPreTranspose;
    if (keyToCopy < 0 || harmonizerKeys.size() <= keyToCopy) return;
    for (int i = 0; i < 128; i++)
    {
        Array<int> tempArray = harmonizerKeys[i];
        for (auto key : harmonizerKeys[keyToCopy])
            tempArray.addIfNotAlreadyThere(key - keyToCopy + i);
        harmonizerKeys.set(i, tempArray);
    }
}

void Keymap::copyKeyMappingToAll(int keyToTrap)
{
    keyToTrap += harPreTranspose;
    if (keyToTrap < 0 || harmonizerKeys.size() <= keyToTrap) return;
    for (int i = 0; i < 128; i++)
    {
        Array<int> tempArray = harmonizerKeys[i];
        for (auto key : harmonizerKeys[keyToTrap])
            tempArray.addIfNotAlreadyThere(key);
        harmonizerKeys.set(i, tempArray);
    }
}

void Keymap::copyKeyPatternToOctaves(int keyToCopy)
{
    keyToCopy += harPreTranspose;
    if (keyToCopy < 0 || harmonizerKeys.size() <= keyToCopy) return;
    for (int i = keyToCopy % 12; i < 128; i += 12)
    {
        Array<int> tempArray = harmonizerKeys[i];
        for (auto key : harmonizerKeys[keyToCopy])
            tempArray.addIfNotAlreadyThere(key - keyToCopy + i);
        harmonizerKeys.set(i, tempArray);
    }
}

void Keymap::copyKeyMappingToOctaves(int keyToTrap)
{
    keyToTrap += harPreTranspose;
    if (keyToTrap < 0 || harmonizerKeys.size() <= keyToTrap) return;
    for (int i = keyToTrap % 12; i < 128; i += 12)
    {
        Array<int> tempArray = harmonizerKeys[i];
        for (auto key : harmonizerKeys[keyToTrap])
            tempArray.addIfNotAlreadyThere(key);
        harmonizerKeys.set(i, tempArray);
    }
}

void Keymap::mirrorKey(int keyCenter)
{
    keyCenter += harPreTranspose;
    if (keyCenter < 0 || harmonizerKeys.size() <= keyCenter) return;
    harmonizerKeys.set(keyCenter, Array<int>(keyCenter));
    if (keyCenter < 64)
    {
        int j = keyCenter + 1;
        for (int i = keyCenter - 1; i >= 0; i--)
        {
            Array<int> tempArray = harmonizerKeys[i];
            tempArray.addIfNotAlreadyThere(j);
            harmonizerKeys.set(i, tempArray);

            tempArray = harmonizerKeys[j];
            tempArray.addIfNotAlreadyThere(i);
            harmonizerKeys.set(j, tempArray);
            j++;
        }
    }
    else
    {
        int i = keyCenter - 1;
        for (int j = keyCenter + 1; j < 128; j++)
        {
            Array<int> tempArray = harmonizerKeys[i];
            tempArray.addIfNotAlreadyThere(j);
            harmonizerKeys.set(i, tempArray);

            tempArray = harmonizerKeys[j];
            tempArray.addIfNotAlreadyThere(i);
            harmonizerKeys.set(j, tempArray);
            i--;
        }
    }
}

void Keymap::defaultHarmonizations()
{
    for (int i = 0; i < 128; i++)
    {
        Array<int> tempArray = harmonizerKeys[i];
        tempArray.addIfNotAlreadyThere(i);
        harmonizerKeys.set(i, tempArray);
    }
}

void Keymap::clearHarmonizations()
{
    for (int i = 0; i < 128; i++)
    {
        harmonizerKeys.set(i, Array<int>());
    }
}

void Keymap::setDefaultSelected(bool selected)
{
    if (processor.wrapperType == juce::AudioPluginInstance::wrapperType_AudioUnit ||
        processor.wrapperType == juce::AudioPluginInstance::wrapperType_VST ||
        processor.wrapperType == juce::AudioPluginInstance::wrapperType_VST3)
    {
        defaultSelected = true;
    }
    else defaultSelected = selected;
}

void Keymap::setOnscreenSelected(bool selected)
{
    if (processor.wrapperType == juce::AudioPluginInstance::wrapperType_AudioUnit ||
        processor.wrapperType == juce::AudioPluginInstance::wrapperType_VST ||
        processor.wrapperType == juce::AudioPluginInstance::wrapperType_VST3)
    {
        onscreenSelected = true;
    }
    else onscreenSelected = selected;
}

float Keymap::applyVelocityCurve(float velocity)
{
    /*
     **** Velocity Curving
     user settable parameters:
     --asymmetric warping coefficient (0, 10): default 1 (no warping)
     --symmetric warping coefficent (0, 5): default 1 (no warping)
     --scaling multipler (0, 10): default 1.
     --offset (-1, 1): default 0.
     --invert velocities, toggle: default off
     
     also, the user should be able to set extendRange (in dB), which is in BKPianoSampler::startNote()
     and will presumably need to pass through here.
     
     velocity curving doesn't actually extend the dynamic range (well, it could if scaling results
     in velocities > 1.), but rather just distributes the incoming velocities across the dynamic
     range with the sample layers. extendRange will extend the total dynamic of the sample set, and
     is set to 4dB by default at the moment (that's probably a reasonable default, and feels good for
     the Heavy set and other new bK sample libraries).
     */
    
    float velocityCurved = dt_warpscale(velocity, asym_k, sym_k, scale, offset);
    if (velocityInvert) velocityCurved = 1. - velocityCurved;
    
    if (velocityCurved < 0.) velocityCurved = 0.;
    else if (velocityCurved > 1.) velocityCurved = 1.; // not sure we need to cap this
    // something will break down the line if not capped - note from jeff
    
    //DBG("asym_k = " + String(asym_k));
    //DBG("sym_k = " + String(sym_k));
    //DBG("scale = " + String(scale));
    //DBG("offset = " + String(offset));
    //DBG("velocity, velocityCurved = " + String(velocity) + ", " + String(velocityCurved));
    
    return velocityCurved;
}
