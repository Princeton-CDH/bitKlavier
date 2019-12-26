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
keymap(Array<bool>()),
targetStates(Array<KeymapTargetState>()),
midiEdit(false),
inverted(false),
midiInputSources(Array<String>()),
defaultSelected(false),
onscreenSelected(true)
{
    keymap.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        keymap.add(false);
    }
    
    targetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    if (processor.isMidiReady()) {
        defaultSelected = true;
    }
}

Keymap::Keymap(BKAudioProcessor& processor, Keymap::Ptr k):
processor(processor),
Id(k->getId()),
midiEdit(false),
inverted(false),
midiInputSources(k->getMidiInputSources()),
defaultSelected(k->isDefaultSelected()),
onscreenSelected(k->isOnscreenSelected())
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
        targetStates.add(TargetStateNil);
    }
    
    inverted = k->isInverted();
}

Keymap::Keymap(BKAudioProcessor& processor, int Id, Keymap::Ptr k):
processor(processor),
Id(Id),
midiEdit(false),
inverted(false),
midiInputSources(k->getMidiInputSources()),
defaultSelected(k->isDefaultSelected()),
onscreenSelected(k->isOnscreenSelected())
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
        targetStates.add(TargetStateNil);
    }
    
    inverted = k->isInverted();
}

Keymap::Keymap(BKAudioProcessor& processor):
processor(processor),
Id(-1),
keymap(Array<bool>()),
targetStates(Array<KeymapTargetState>()),
midiEdit(false),
inverted(false),
midiInputSources(Array<String>()),
defaultSelected(false),
onscreenSelected(true)
{
    keymap.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        keymap.add(false);
    }
    
    targetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    if (processor.isMidiReady()) {
        defaultSelected = true;
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
        keymap.set(note, true);
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

void Keymap::setTarget(KeymapTargetType target, KeymapTargetState state)
{
    if (targetStates[target] == TargetStateNil) return;
    
    //targetStates.set(target, state ? TargetStateDisabled : TargetStateEnabled);
    targetStates.set(target, state);
    DBG("Keymap::setTarget = " + String(target) + " " + String((int) getTargetState(target)));
}

void Keymap::toggleTarget(KeymapTargetType target)
{
    if (targetStates[target] == TargetStateNil) return;
    
    targetStates.set(target, (targetStates[target] == TargetStateDisabled) ? TargetStateEnabled : TargetStateDisabled);
    DBG("Keymap::toggleTarget = " + String(target) + " " + String((int) getTargetState(target)));
}

void Keymap::enableTarget(KeymapTargetType target)
{
    targetStates.set(target, TargetStateEnabled);
}

void Keymap::disableTarget(KeymapTargetType target)
{
    targetStates.set(target, TargetStateDisabled);
}

void Keymap::addTarget(KeymapTargetType target)
{
    if (targetStates[target] == TargetStateNil)
    {
        targetStates.set(target, TargetStateEnabled);
    }
}

void Keymap::addTarget(KeymapTargetType target, KeymapTargetState state)
{
    if (targetStates[target] == TargetStateNil)
    {
        targetStates.set(target, state);
    }
}

void Keymap::removeTarget(KeymapTargetType target)
{
    targetStates.set(target, TargetStateNil);
}


void Keymap::removeTargetsOfType(BKPreparationType type)
{
    if (type == PreparationTypeDirect)
    {
        removeTarget(TargetTypeDirect);
    }
    if (type == PreparationTypeSynchronic)
    {
        removeTarget(TargetTypeSynchronicSync);
        removeTarget(TargetTypeSynchronic);
        removeTarget(TargetTypeSynchronicPatternSync);
        removeTarget(TargetTypeSynchronicAddNotes);
        removeTarget(TargetTypeSynchronicPausePlay);
    }
    else if (type == PreparationTypeNostalgic)
    {
        removeTarget(TargetTypeNostalgic);
    }
    else if (type == PreparationTypeBlendronic)
    {
        removeTarget(TargetTypeBlendronicSync);
        removeTarget(TargetTypeBlendronicClear);
        removeTarget(TargetTypeBlendronicOpen);
        removeTarget(TargetTypeBlendronicClose);
    }
    else if (type == PreparationTypeTempo)
    {
        removeTarget(TargetTypeTempo);
    }
    else if (type == PreparationTypeTuning)
    {
        removeTarget(TargetTypeTuning);
    }
}

void Keymap::clearTargets()
{
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.set(i, TargetStateNil);
    }
}

const Array<String> Keymap::getAllMidiInputSources()
{
    Array<String> sources;
    sources.addArray(midiInputSources);
    if (onscreenSelected) sources.add(cNoteSourceUI);
    if (defaultSelected) sources.addArray(processor.getDefaultMidiInputSources());
        return sources;
}

void Keymap::print(void)
{
    DBG("Id: " + String(Id));
    DBG("Keymap: "+ intArrayToString(keys()));
}
