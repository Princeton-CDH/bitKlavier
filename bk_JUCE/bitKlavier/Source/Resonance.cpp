/*
  ==============================================================================

    Resonance.cpp
    Created: 12 May 2021 12:41:26pm
    Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "Resonance.h"
#include "Modification.h"

void ResonancePreparation::performModification(ResonancePreparation* r, Array<bool> dirty)
{
    //TBD
}

ResonanceProcessor::ResonanceProcessor(Resonance::Ptr rResonance, TuningProcessor::Ptr rTuning, GeneralSettings::Ptr rGeneral, BKSynthesiser* rMain):
    resonance(rResonance),
    tuning(rTuning),
    general(rGeneral),
    synth(rMain),
    keymaps(Keymap::PtrArr())
{
    keysDepressed = Array<int>();
    keysExcited = Array<int>();
    keysExcitedDupes = Array<int>();

    DBG("Create rProc");
}

ResonanceProcessor::~ResonanceProcessor()
{
    DBG("Destroy rProc");
}

void ResonanceProcessor::keyPressed(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates)
{
    //TBD, pseudocode outline here (add the key to depressed keys, check if its overtones are already depressed, if so add them to excited lists)

    keysDepressed.add(noteNumber);
    DBG("key " + String(noteNumber) + " pressed");
    DBG("KeysDepressed after key added: " + intArrayToString(keysDepressed));

    // check for overtones of the played note
    for (int i : resonance->prep->getDistances())
    {
        DBG("current overtone being checked: " + String(noteNumber + i));
        if (keysDepressed.contains(noteNumber + i))
        {
            keysExcitedDupes.add(noteNumber + i);
            DBG("key " + String(noteNumber + i) + " added to dupes");
            DBG("Dupes after key added: " + intArrayToString(keysExcitedDupes));
            if (!(keysExcited.contains(noteNumber + i)))
            {
                DBG("key " + String(noteNumber + i) + " added to excited list");
                DBG("KeysExcited after key added: " + intArrayToString(keysExcited));
                //start timer for newly excited key - need to figure out how to implement
                //need to figure out check for gain threshold
                if (velocity < resonance->prep->getAttackThresh())
                {
                    DBG("velocity didn't exceed attack threshold");
                }
                else
                {
                    DBG("going to play note");
                    Array<float> ADSRvals = resonance->prep->getADSRvals();
                    if (!blendronic.isEmpty())
                    {
                        synth->keyOn(midiChannel,
                            noteNumber + i,
                            noteNumber + i,
                            tuning->getOffset(noteNumber + i, false),
                            velocity, // maybe need to set a different velocity than the played note's velocity for resonant notes
                            aGlobalGain,
                            Forward,
                            NormalFixedStart,
                            ResonanceNote,
                            resonance->prep->getSoundSet(), //set
                            resonance->getId(),
                            resonance->prep->getStartTime(),                        //start position
                            resonance->prep->getLength(),                            //play length
                            ADSRvals[0],
                            ADSRvals[1],
                            ADSRvals[2],
                            ADSRvals[3],
                            tuning,
                            resonance->prep->getDefaultGainPtr(),
                            resonance->prep->getBlendGainPtr(),
                            blendronic = blendronic);
                    }
                    else
                    {
                        synth->keyOn(midiChannel,
                            noteNumber + i,
                            noteNumber + i,
                            tuning->getOffset(noteNumber + i, false),
                            velocity, // maybe need to set a different velocity than the played note's velocity for resonant notes
                            aGlobalGain,
                            Forward,
                            NormalFixedStart,
                            ResonanceNote,
                            resonance->prep->getSoundSet(), //set
                            resonance->getId(),
                            resonance->prep->getStartTime(),                        //start position
                            resonance->prep->getLength(),                            //play length
                            ADSRvals[0],
                            ADSRvals[1],
                            ADSRvals[2],
                            ADSRvals[3],
                            tuning,
                            resonance->prep->getDefaultGainPtr(),
                            resonance->prep->getBlendGainPtr(),
                            blendronic = blendronic);
                    }
                    keysExcited.insert(0, noteNumber + i);
                    resonantNotes.insert(0, new ResonanceNoteStuff(noteNumber + i));
                    ResonanceNoteStuff* currentNote = resonantNotes.getUnchecked(0);
                    currentNote->setPrepAtKeyOn(resonance->prep);
                    currentNote->setTuningAtKeyOn(tuning->getOffset(noteNumber + i, false));
                    currentNote->setVelocityAtKeyOn(velocity);
                    currentNote->setStartPosition((resonance->prep->getStartTime()) * synth->getSampleRate() / 1000.);
                    currentNote->setTargetLength((resonance->prep->getLength())*synth->getSampleRate() / 1000.);
                }
            }
            else
            {
                DBG("Key " + String(noteNumber + i) + " not added to excited list because already present");
            }
        }
    }

    
}

void ResonanceProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    //TBD, will put pseudocode here (remove key from depressed keys, remove any resonating overtones, remove it from overtones if excited)

    DBG("key " + String(noteNumber) + " removed");
    keysDepressed.removeAllInstancesOf(noteNumber);
    DBG("KeysDepressed after key removed: " + intArrayToString(keysDepressed));

    bool alreadyInReleaseList = false;

    if (keysReleasedExcited.contains(noteNumber))
    {
        alreadyInReleaseList = true;
    }

    
    //currently leaving this out, with this commented out keys will only release when the resonating key is released/exceeds its target
    for (int i : resonance->prep->getDistances())
    {
        if (keysExcitedDupes.contains(noteNumber + i) && !keysDepressed.contains(noteNumber + i))
        {
            keysExcitedDupes.removeFirstMatchingValue(noteNumber + i);
            DBG("key " + String(noteNumber + i) + " removed from dupes");
            if (!(keysExcitedDupes.contains(noteNumber + i)))
            {
                DBG("key " + String(noteNumber + i) + " removed from excited list");
                synth->keyOff(midiChannel,
                    ResonanceNote,
                    resonance->prep->getSoundSet(),
                    resonance->getId(),
                    noteNumber + i,
                    noteNumber + i,
                    64,
                    aGlobalGain,
                    resonance->prep->getDefaultGainPtr(),
                    true, // need to test more here
                    true); // need to test more here
                int index = keysExcited.indexOf(noteNumber + i);
                keysExcited.remove(index);
                resonantNotes.remove(index);
                
                int releasedIndex = keysReleasedExcited.indexOf(noteNumber + i);
                if (releasedIndex >= 0) keysReleasedExcited.remove(releasedIndex);
            }
        }
    }
    

    bool hasFundamental = false;

    // check if key is a resonant overtone of an already depressed key, play it on release
    for (int i : resonance->prep->getDistances())
    {
        DBG("current possible fundamental being checked: " + String(noteNumber - i));
        if (keysDepressed.contains(noteNumber - i))
        {
            hasFundamental = true;
            keysExcitedDupes.add(noteNumber);
            DBG("key " + String(noteNumber) + " added to dupes");
            DBG("Dupes after key added: " + intArrayToString(keysExcitedDupes));
            if (!(keysExcited.contains(noteNumber)))
            {
                DBG("key " + String(noteNumber) + " added to excited list");
                DBG("KeysExcited after key added: " + intArrayToString(keysExcited));
                //start timer for newly excited key - need to figure out how to implement
                //need to figure out check for gain threshold
                if ((velocity < resonance->prep->getAttackThresh()) || (alreadyInReleaseList))
                {
                    DBG("velocity didn't exceed attack threshold or already in release list");
                }
                else
                {
                    DBG("going to play note");
                    Array<float> ADSRvals = resonance->prep->getADSRvals();
                    if (!blendronic.isEmpty())
                    {
                        synth->keyOn(midiChannel,
                            noteNumber,
                            noteNumber,
                            tuning->getOffset(noteNumber, false),
                            velocity, // maybe need to set a different velocity than the played note's velocity for resonant notes
                            aGlobalGain,
                            Forward,
                            NormalFixedStart,
                            ResonanceNote,
                            resonance->prep->getSoundSet(), //set
                            resonance->getId(),
                            resonance->prep->getStartTime(),                        //start position
                            resonance->prep->getLength(),                            //play length
                            ADSRvals[0],
                            ADSRvals[1],
                            ADSRvals[2],
                            ADSRvals[3],
                            tuning,
                            resonance->prep->getDefaultGainPtr(),
                            resonance->prep->getBlendGainPtr(),
                            blendronic = blendronic);
                    }
                    else
                    {
                        synth->keyOn(midiChannel,
                            noteNumber,
                            noteNumber,
                            tuning->getOffset(noteNumber, false),
                            velocity, // maybe need to set a different velocity than the played note's velocity for resonant notes
                            aGlobalGain,
                            Forward,
                            NormalFixedStart,
                            ResonanceNote,
                            resonance->prep->getSoundSet(), //set
                            resonance->getId(),
                            resonance->prep->getStartTime(),                        //start position
                            resonance->prep->getLength(),                            //play length
                            ADSRvals[0],
                            ADSRvals[1],
                            ADSRvals[2],
                            ADSRvals[3],
                            tuning,
                            resonance->prep->getDefaultGainPtr(),
                            resonance->prep->getBlendGainPtr(),
                            blendronic = blendronic);
                    }
                    
                    keysReleasedExcited.insert(0, noteNumber);
                    resonantNotes.insert(0, new ResonanceNoteStuff(noteNumber));
                    ResonanceNoteStuff* currentNote = resonantNotes.getUnchecked(0);
                    currentNote->setPrepAtKeyOn(resonance->prep);
                    currentNote->setTuningAtKeyOn(tuning->getOffset(noteNumber, false)); // bool value might need to change here?
                    currentNote->setVelocityAtKeyOn(velocity);
                    currentNote->setStartPosition((resonance->prep->getStartTime()) * synth->getSampleRate() / 1000.);
                    currentNote->setTargetLength((resonance->prep->getLength()) * synth->getSampleRate() / 1000.);
                }
            }
            else
            {
                DBG("Key " + String(noteNumber + i) + " not added to excited list because already present");
            }
        }
    }
    if (!hasFundamental)
    {
        if (keysExcitedDupes.contains(noteNumber))
        {
            keysExcitedDupes.removeAllInstancesOf(noteNumber);
            DBG("key " + String(noteNumber) + " removed from excited/dupes because released");
            synth->keyOff(midiChannel,
                ResonanceNote,
                resonance->prep->getSoundSet(),
                resonance->getId(),
                noteNumber,
                noteNumber,
                64,
                aGlobalGain,
                resonance->prep->getDefaultGainPtr(),
                true, // need to test more here
                true); // need to test more here
            int index = keysExcited.indexOf(noteNumber);
            keysExcited.remove(index);
            resonantNotes.remove(index);

            int releasedIndex = keysReleasedExcited.indexOf(noteNumber);
            if (releasedIndex >= 0) keysReleasedExcited.remove(releasedIndex);
        }
    }
    DBG("released excited list at end of keyReleased: " + intArrayToString(keysReleasedExcited));
}

void ResonanceProcessor::prepareToPlay(double sr)
{
    //TBD, might not be necessary?
}

void ResonanceProcessor::processBlock(int numSamples, int midiChannel)
{
    incrementTimers(numSamples);
    
    DBG("released excited list at start of process block: " + intArrayToString(keysReleasedExcited));

    for (int i = resonantNotes.size() - 1; i >= 0; --i)
    {
        ResonanceNoteStuff* thisNote = resonantNotes.getUnchecked(i);

        if (thisNote->timerExceedsTarget() || !thisNote->isActive())
        {
            int noteNumber = thisNote->getNoteNumber();
            keysExcitedDupes.removeAllInstancesOf(noteNumber);
            DBG("key " + String(noteNumber) + " removed from excited/dupes because timer elapsed");
            synth->keyOff(midiChannel,
                ResonanceNote,
                resonance->prep->getSoundSet(),
                resonance->getId(),
                noteNumber,
                noteNumber,
                64,
                aGlobalGain,
                resonance->prep->getDefaultGainPtr(),
                true, // need to test more here
                true); // need to test more here
            keysExcited.remove(i);
            resonantNotes.remove(i);

            int releasedIndex = keysReleasedExcited.indexOf(i);
            if (releasedIndex >= 0) keysReleasedExcited.remove(releasedIndex);
        }
    }
}

void ResonanceProcessor::playNote(int channel, int note, float velocity)
{
    //TBD, might not be necessary?
}

void ResonanceProcessor::incrementTimers(int numsamples)
{
    for (int i = (resonantNotes.size() - 1); i >= 0; --i)
    {
        noteLengthTimers.set(keysExcited.getUnchecked(i),
            noteLengthTimers.getUnchecked(keysExcited.getUnchecked(i)) + numsamples);
    }
}
