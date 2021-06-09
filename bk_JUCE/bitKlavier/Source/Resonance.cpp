/*
  ==============================================================================

    Resonance.cpp
    Created: 12 May 2021 12:41:26pm
    Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "Resonance.h"
#include "PluginProcessor.h"
#include "Modification.h"

void ResonancePreparation::performModification(ResonancePreparation* r, Array<bool> dirty)
{
    //TBD
}

SympPartial::SympPartial(int newHeldKey, int newPartialKey, float newGain, float newOffset, BKSynthesiserVoice* newVoice)
{
    heldKey     = newHeldKey;
    partialKey  = newPartialKey;
    gain        = newGain;
    offset      = newOffset;
    voice       = newVoice;

    playPosition = maxPlayPosition; // default to past end of sample
}

SympPartial::SympPartial(int newHeldKey, int newPartialKey, float newGain, float newOffset) 
{
    heldKey     = newHeldKey;
    partialKey  = newPartialKey;
    gain        = newGain;
    offset      = newOffset;

    playPosition = maxPlayPosition;
}


// copy constructor
SympPartial& SympPartial::operator= (const SympPartial& other)
{
    heldKey     = other.heldKey;
    partialKey  = other.partialKey;
    gain        = other.gain;
    offset      = other.offset;
    voice       = other.voice;
    
    return *this;
}

// assignment operator
SympPartial& SympPartial::operator= (SympPartial&& other) noexcept
{
    heldKey     = other.heldKey;
    partialKey  = other.partialKey;
    gain        = other.gain;
    offset      = other.offset;
    voice       = std::move (other.voice);
    
    return *this;
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

    //**********
    // NEW DAN IMPLEMENTATION BELOW
    
    // default partials
    // integer interval from fundamental (corresponding to key distance), gain, offset from ET (cents)
    partialStructure.add({0,  1.0, 0});
    partialStructure.add({12, 0.8, 0});
    partialStructure.add({19, 0.7, 2});
    partialStructure.add({24, 0.8, 0});
    partialStructure.add({28, 0.6, -13.7});
    partialStructure.add({31, 0.7, 2});
    partialStructure.add({34, 0.5, -31.175});
    partialStructure.add({36, 0.8, 0});

    DBG("Create rProc");
}

ResonanceProcessor::~ResonanceProcessor()
{
    DBG("Destroy rProc");
}

// this will cause all the resonating strings that have overlapping partials with this new struck string to ring sympathetically
void ResonanceProcessor::ringSympStrings(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates)
{
    DBG("Resonance::ringSympStrings");
    // resonate existing sympStrings
    // see if there is overlap with the newly pressed key's partials and any sympPartials
    //for (auto heldNotePartials : sympStrings)
    //for (HashMap<int, SympPartial::PtrMap>::Iterator heldNotePartials (sympStrings2); heldNotePartials.next();)
    //for (HashMap<int, HashMap<int, SympPartial>>::Iterator heldNotePartials (sympStrings); heldNotePartials.next();)
    //for (HashMap<int, HashMap<int, SympPartial::Ptr>>::Iterator heldNotePartials (sympStrings); heldNotePartials.next();)
    for (HashMap<int, Array<SympPartial::Ptr>>::Iterator heldNotePartials (sympStrings); heldNotePartials.next();)
    {
        DBG("Resonance::ringSympStrings: iterating through sympStrings");
        // indexed by heldNote (midiNoteNumber)
        // iterate through partials of incoming note (noteNumber) and see if they are contained in this set of heldNotePartials
        for (int j = 0; j < partialStructure.size(); j++)
        {
            DBG("Resonance::ringSympStrings: iterating through partialStructure");
            // strucknote (noteNumber) + partialKey offset (key closets to this partial)
            int currentStruckPartial = noteNumber + partialStructure.getReference(j)[0];
            
            //if (heldNotePartials.getValue().contains(currentStruckPartial))
            for (auto currentSympPartial : *heldNotePartials)
            {
                DBG("Resonance::ringSympStrings: iterating through heldNotePartials");
                DBG("Resonance: currentStruckPartial = " + String(currentStruckPartial) + " heldKey = " + String(currentSympPartial->heldKey));
                if (currentSympPartial->partialKey == currentStruckPartial)
                {
                    // found an overlapping partial!
                    DBG("Resonance: found an overlapping partial");
                    
                    // calculate play position for this new resonance (ms)
                    // based on velocity; so higher velocity, the further into the sample it will play
                    int newPlayPosition = resonance->prep->getMinStartTime()
                                        + (resonance->prep->getMaxStartTime() - resonance->prep->getMinStartTime())
                                        * (1. - velocity);
                    
                    newPlayPosition = 400; //testing
                    
                    // only create a new resonance if it would be louder/brighter than what is currently there
                    //if (newPlayPosition < heldNotePartials.getValue()[currentStruckPartial]->playPosition / synth->getSampleRate())
                    if (newPlayPosition < currentSympPartial->playPosition / synth->getSampleRate())
                    {
                        // set the playPosition for this new resonance
                        currentSympPartial->playPosition = newPlayPosition;
                        
                        
                        // turn off the current resonance here, if it's playing
                            //if (currentSympPartial->voice->isVoiceActive())
                        /*
                            {
                                synth->keyOffByVoice(
                                                midiChannel,
                                                ResonanceNote,
                                                resonance->prep->getSoundSet(),
                                                resonance->getId(),
                                                noteNumber,
                                                noteNumber,
                                                64,
                                                aGlobalGain,
                                                resonance->prep->getDefaultGainPtr(),
                                                true, // need to test more here
                                                currentSympPartial->voice,
                                                false);
                            }
                         */

                        // calculate the tuning gap between attached tuning and the tuning of this partial
                        // taking into account attached Tuning system, and defined partial structure (which may or may not be the same!)
                        float tuningGap = fabs( tuning->getOffset(noteNumber, false)
                                               - (tuning->getOffset(currentSympPartial->heldKey, false) + currentSympPartial->offset)
                                              ) / 50.; // in cents?

                        if (tuningGap > 2.) tuningGap = 2.;

                        // play it, and store the voice so it can be shut off as needed
                        // use max velocity for all resonance; loudness/brightness is set by newPlayPosition
                        // adjust gain according to gap in tuning; 50 cent gap will result in 6dB cut in gain
                        //      might be better to adjust playback position due to gap, instead of adjusting the gain?
                        
                        //currentSympPartial->voice = keyOn(currentSympPartial->gain * (1. - 0.5 * tuningGap), currentSympPartial->offset, velocity = 1., newPlayPosition);

                        DBG("Resonance keyOn: " + String(noteNumber));
                        Array<float> ADSRvals = resonance->prep->getADSRvals();
                        if (!blendronic.isEmpty())
                        {
                            currentSympPartial->voice = synth->keyOn(
                                midiChannel,
                                                                     noteNumber,
                                                                     currentStruckPartial,
                                // tuning->getOffset(noteNumber, false) + currentSympPartial->offset,
                                tuning->getOffset(noteNumber, false),
                                1., // use max velocity sample for all resonance samples; intensity is set by newPlayPosition
                                aGlobalGain * currentSympPartial->gain * (1. - 0.5 * tuningGap),
                                Forward,
                                NormalFixedStart,
                                ResonanceNote,
                                resonance->prep->getSoundSet(), //set
                                resonance->getId(),
                                newPlayPosition,                //start position
                                0,                              //play length
                                ADSRvals[0],
                                ADSRvals[1],
                                ADSRvals[2],
                                ADSRvals[3],
                                tuning,
                                resonance->prep->getDefaultGainPtr(),
                                resonance->prep->getBlendGainPtr(),
                                blendronic);
                        }
                        else
                        {
                            currentSympPartial->voice = synth->keyOn(
                                midiChannel,
                                noteNumber,
                                currentStruckPartial,
                                //tuning->getOffset(noteNumber, false) + currentSympPartial->offset,
                                tuning->getOffset(noteNumber, false),
                                1.,
                                aGlobalGain * currentSympPartial->gain * (1. - 0.5 * tuningGap),
                                Forward,
                                NormalFixedStart,
                                ResonanceNote,
                                resonance->prep->getSoundSet(), //set
                                resonance->getId(),
                                newPlayPosition,                //start position
                                0,                              //play length
                                ADSRvals[0],
                                ADSRvals[1],
                                ADSRvals[2],
                                ADSRvals[3],
                                tuning,
                                resonance->prep->getDefaultGainPtr(),
                                resonance->prep->getBlendGainPtr());
                        }
                    }
                }
            }
        }
    }
}

// this will add this string and all its partials to the currently available sympathetic strings (sympStrings)
void ResonanceProcessor::addSympStrings(int noteNumber)
{
    DBG("Resonance: addingSympatheticString " + String(noteNumber));
    //individualStrings.add(new HashMap<int, SympPartial::Ptr>);
    //HashMap<int, SympPartial::Ptr> newPartials = individualStrings.getLast();
    
    // HashMap<int, SympPartial>* newPartials = new HashMap<int, SympPartial>(0);
    //OwnedArray<SympPartial> newPartials;
    for (int i = 0; i < partialStructure.size(); i++)
    {
        // heldKey = noteNumber
        // partialKey = key that this partial is nearest, as assigned by partialStructure
        int partialKey = noteNumber + partialStructure.getUnchecked(i)[0];
        if (partialKey > 108 || partialKey < 21) continue;

        // make a newPartial object, with gain and offset vals, and active
        // SympPartial::Ptr newPartial = new SympPartial(noteNumber, partialKey, partialStructure[i][1], partialStructure[i][2]);
        //allSymps.add(new SympPartial(noteNumber, partialKey, partialStructure[i][1], partialStructure[i][2]));
        //SympPartial newPartial(noteNumber, partialKey, partialStructure[i][1], partialStructure[i][2]);
        //individualStrings.getLast()->set(partialKey, newPartial);
        //individualStrings.getLast()->set(partialKey, allStrings.getLast());
        //sympsByNoteNumber.add(new HashMap<int, SympPartial::Ptr>);
        //sympsByNoteNumber.getLast()->set(partialKey, allSymps.getLast());
        //newPartials.add(&newPartial);
        
        //sympStrings[noteNumber][partialKey] = new SympPartial(noteNumber, partialKey, partialStructure[i][1], partialStructure[i][2]);
        DBG("Resonance: adding partial " + String(partialKey) + " to " + String(noteNumber));
        if(sympStrings[noteNumber].size() == 0) {
            // make new Array
            //sympStrings.set(noteNumber, Array<SympPartial::Ptr>());
        }
        //sympStrings[noteNumber].add(new SympPartial(noteNumber, partialKey, partialStructure[i][1], partialStructure[i][2]));
        sympStrings.getReference(noteNumber).add(new SympPartial(noteNumber, partialKey, partialStructure[i][1], partialStructure[i][2]));
        
        
        
        //sympStrings[noteNumber][partialKey]->heldKey    = noteNumber;
        //sympStrings[noteNumber][partialKey]->partialKey = partialKey;
        //sympStrings[noteNumber][partialKey]->gain       = partialStructure[i][1];
        //sympStrings[noteNumber][partialKey]->offset     = partialStructure[i][2];
    }
    //sympStrings.set(noteNumber, sympsByNoteNumber.getLast());
    //sympStrings.set(noteNumber, newPartials);
    //sympStrings.add(&newPartials);
    int numParts = sympStrings[noteNumber].size();
    DBG("Resonance: number of partials = " + String(numParts));
     
     
}

// this will turn off all the resonace associated with this string/key, and then remove those from the currently available sympathetic strings
void ResonanceProcessor::removeSympStrings(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    
    
    //HashMap<int, SympPartial> removePartials = sympStrings[noteNumber];

    // turn off each partial associated with this string
    //for (HashMap<int, SympPartial::Ptr>::Iterator sympString (sympStrings[noteNumber]); sympString.next();)
    DBG("Resonance: removing partials of " + String(noteNumber));
    for (auto sympString : sympStrings[noteNumber])
    //for (auto sympString : sympStrings)
    {
        // keyOff this partial, by voice
        // keyOff(i.getValue()->voice);
        
        //if(sympString->heldKey == noteNumber) { // sympPartials will have the same heldKey in this sympString
            //for (auto sympPartial : *sympString) {
        DBG("Resonance: removing partial " + String(sympString->partialKey));
        
        //if(sympString-)
        /*
        {
            synth->keyOffByVoice(
                        midiChannel,
                        ResonanceNote,
                        resonance->prep->getSoundSet(),
                        resonance->getId(),
                        noteNumber,
                        noteNumber,
                        64,
                        aGlobalGain,
                        resonance->prep->getDefaultGainPtr(),
                        true, // need to test more here
                        //sympString.getValue()->voice,
                        sympString->voice,
                        false);
        }
         */
            //break; // should only have one of these sympStrings, since it corresponds to a heldKey, so we can break here
        //}
        
        /*
        synth->keyOff(
                    midiChannel,
                    ResonanceNote,
                    resonance->prep->getSoundSet(),
                    resonance->getId(),
                    sympString->heldKey,
                    sympString->partialKey,
                    64,
                    aGlobalGain,
                    resonance->prep->getDefaultGainPtr(),
                    true, // need to test more here
                    false);
         */
    }

    // clear this held string's partials
    sympStrings.remove(noteNumber);
     
}

void ResonanceProcessor::keyPressed(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates)
{

    // resonate the currently available strings and their overlapping partials
    ringSympStrings(noteNumber, velocity, midiChannel, targetStates);

    // then, add this new string and its partials to the currently available sympathetic strings
    addSympStrings(noteNumber);

    /*
    // theo implmentation
     
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
     */
}

void ResonanceProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    // this will turn off all the resonace associated with this string/key, and then remove those from the currently available sympathetic strings
    removeSympStrings(noteNumber, velocity, midiChannel, targetStates, post);
    
    
    /*
     // theo implementation
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
     */
}

void ResonanceProcessor::prepareToPlay(double sr)
{
    //TBD, might not be necessary?
}

void ResonanceProcessor::processBlock(int numSamples, int midiChannel)
{

    for (HashMap<int, Array<SympPartial::Ptr>>::Iterator heldNotePartials (sympStrings); heldNotePartials.next();)
    {
        for (auto sympString : *heldNotePartials)
        {
            sympString->playPosition += numSamples;
        }
    }

    /*
     // theo implementation
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
     */
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
