/*
  ==============================================================================

    Resonance.cpp
    Created: 12 May 2021 12:41:26pm
    Author:  Dan Trueman and Theodore R Trevisan
 
    Name candidate for this preparation: Understrings, Undersonic
        after hardanger understrings, but also the sense that these sounds are "under" the main sounds of the piano
        also, U is available as a keystroke ;--}

  ==============================================================================
*/

#include "Resonance.h"
#include "PluginProcessor.h"
#include "Modification.h"

void ResonancePreparation::performModification(ResonancePreparation* r, Array<bool> dirty)
{
    //TBD
}

SympPartial::SympPartial(int newHeldKey, int newPartialKey, float newGain, float newOffset) 
{
    heldKey     = newHeldKey;
    partialKey  = newPartialKey;
    gain        = newGain;
    offset      = newOffset;

    playPosition = maxPlayPosition;
}

ResonanceProcessor::ResonanceProcessor(Resonance::Ptr rResonance, TuningProcessor::Ptr rTuning, GeneralSettings::Ptr rGeneral, BKSynthesiser* rMain):
    resonance(rResonance),
    synth(rMain),
    tuning(rTuning),
    general(rGeneral),
    keymaps(Keymap::PtrArr())
{
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
// N*M^2, where N is number of notes held, M is number of partials in partial structure
void ResonanceProcessor::ringSympStrings(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates)
{
    // DBG("Resonance::ringSympStrings");
    
    // resonate existing sympStrings
    // see if there is overlap with the newly pressed key's partials and any sympPartials

    for (HashMap<int, Array<SympPartial::Ptr>>::Iterator heldNotePartials (sympStrings); heldNotePartials.next();)
    {
        DBG("Resonance::ringSympStrings: iterating through sympStrings");
        // indexed by heldNote (midiNoteNumber)
        // iterate through partials of incoming note (noteNumber) and see if they are contained in this set of heldNotePartials
        for (int j = 0; j < partialStructure.size(); j++)
        {
            DBG("Resonance::ringSympStrings: iterating through partialStructure");
            // strucknote (noteNumber) + partialKey offset (key closest to this partial)
            int currentStruckPartial = noteNumber + partialStructure.getReference(j)[0];
            
            for (auto currentSympPartial : *heldNotePartials)
            {
                // DBG("Resonance::ringSympStrings: iterating through heldNotePartials");
                DBG("Resonance: currentStruckPartial = " + String(currentStruckPartial) + " heldKey = " + String(currentSympPartial->heldKey));
                if (currentSympPartial->partialKey == currentStruckPartial)
                {
                    // found an overlapping partial!
                    
                    // calculate play position for this new resonance (ms)
                    // based on velocity; so higher velocity, the further into the sample it will play
                    int newPlayPosition = resonance->prep->getMinStartTime()
                                        + (resonance->prep->getMaxStartTime() - resonance->prep->getMinStartTime())
                                        * (1. - velocity);
                    
                    DBG("Resonance: found an overlapping partial, currentPlayPosition = "
                        + String(currentSympPartial->playPosition  / (.001 *  synth->getSampleRate()))
                        + " newPlayPosition = " + String(newPlayPosition));
                    
                    // only create a new resonance if it would be louder/brighter than what is currently there
                    if (newPlayPosition < currentSympPartial->playPosition  / (.001 *  synth->getSampleRate()))
                    {
                        // set the playPosition for this new resonance
                        currentSympPartial->playPosition = newPlayPosition * (.001 *  synth->getSampleRate());
                        
                        // turn off the current resonance here, if it's playing
                        synth->keyOff(
                                    midiChannel,
                                    ResonanceNote,
                                    resonance->prep->getSoundSet(),
                                    resonance->getId(),
                                    currentSympPartial->heldKey,
                                    currentSympPartial->partialKey,
                                    64,
                                    aGlobalGain,
                                    resonance->prep->getDefaultGainPtr(),
                                    true, // need to test more here
                                    false);

                        // calculate the tuning gap between attached tuning and the tuning of this partial
                        // taking into account attached Tuning system, and defined partial structure (which may or may not be the same!)
                        float tuningGap = fabs( tuning->getOffset(noteNumber, false)
                                               - (tuning->getOffset(currentSympPartial->heldKey, false) + .01 * currentSympPartial->offset)
                                              ) / .5; // in fractional note values

                        if (tuningGap > 2.) tuningGap = 2.;
                        DBG("Resonance: tuningGap = " + String(tuningGap));

                        // play it, use max velocity for all resonance; loudness/brightness is set by newPlayPosition
                        // adjust gain according to gap in tuning; 50 cent gap will result in 6dB cut in gain
                        //      might be better to adjust playback position due to gap, instead of adjusting the gain?
                        DBG("Resonance playNote: "   + String(noteNumber)
                                                    + " heldKey = " + String(currentSympPartial->heldKey)
                                                    + " partialKey = " + String(currentStruckPartial)
                                                    + " heldKey offset = " + String(tuning->getOffset(noteNumber, false))
                                                    + " partialKey offset = " + String(tuning->getOffset(currentSympPartial->heldKey, false) + .01 * currentSympPartial->offset));
                        
                        Array<float> ADSRvals = resonance->prep->getADSRvals();
                        DBG("Resonance: ADSR release time = " + String(ADSRvals[3]));
                        if (!blendronic.isEmpty())
                        {
                                synth->keyOn(
                                midiChannel,
                                //noteNumber,
                                currentSympPartial->heldKey,
                                currentStruckPartial,
                                tuning->getOffset(currentSympPartial->heldKey, false) + currentSympPartial->offset * .01,
                                // tuning->getOffset(noteNumber, false),
                                1., // use max velocity sample for all resonance samples; intensity is set by newPlayPosition
                                aGlobalGain * currentSympPartial->gain * (1. - 0.5 * tuningGap),
                                //aGlobalGain,
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
                                synth->keyOn(
                                midiChannel,
                                //noteNumber,
                                currentSympPartial->heldKey,
                                currentStruckPartial,
                                tuning->getOffset(currentSympPartial->heldKey, false) + currentSympPartial->offset * .01,
                                //tuning->getOffset(noteNumber, false),
                                1.,
                                aGlobalGain * currentSympPartial->gain * (1. - 0.5 * tuningGap),
                                //aGlobalGain,
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
                                resonance->prep->getDefaultGainPtr());
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
    for (int i = 0; i < partialStructure.size(); i++)
    {
        // heldKey      = noteNumber
        // partialKey   = key that this partial is nearest, as assigned by partialStructure
        int partialKey = noteNumber + partialStructure.getUnchecked(i)[0];
        if (partialKey > 108 || partialKey < 21) continue;

        // make a newPartial object, with gain and offset vals
        DBG("Resonance: adding partial " + String(partialKey) + " to " + String(noteNumber));
        sympStrings.getReference(noteNumber).add(new SympPartial(noteNumber, partialKey, partialStructure[i][1], partialStructure[i][2]));
    }

    int numParts = sympStrings[noteNumber].size();
    DBG("Resonance: number of partials = " + String(numParts));
}

// this will turn off all the resonances associated with this string/key, and then remove those from the currently available sympathetic strings
// **** note that the sustain pedal doesn't seem to work properly with this!
void ResonanceProcessor::removeSympStrings(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    // turn off each partial associated with this string
    DBG("Resonance: removing partials of " + String(noteNumber));
    for (auto sympString : sympStrings.getReference(noteNumber))
    {
        DBG("Resonance: removing partial " + String(sympString->partialKey) + " of held key " + String(sympString->heldKey));
        
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
}

void ResonanceProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    // this will turn off all the resonace associated with this string/key, and then remove those from the currently available sympathetic strings
    removeSympStrings(noteNumber, velocity, midiChannel, targetStates, post);
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
}

