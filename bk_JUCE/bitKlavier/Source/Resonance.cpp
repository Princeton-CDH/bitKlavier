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

void ResonancePreparation::performModification(ResonanceModification* r, Array<bool> dirty)
{
    DBG("ResonancePreparation::performModification");
    // Should the mod be reversed?
    bool reverse = r->altMod && modded;
    
    if (dirty[ResonanceGain]) rDefaultGain.modify(r->rDefaultGain, reverse);
    if (dirty[ResonanceBlendronicGain]) rBlendronicGain.modify(r->rBlendronicGain, reverse);
    if (dirty[ResonanceMinStartTime]) rMinStartTimeMS.modify(r->rMinStartTimeMS, reverse);
    if (dirty[ResonanceMaxStartTime]) rMaxStartTimeMS.modify(r->rMaxStartTimeMS, reverse);
    
    if (dirty[ResonanceFundamental]) rFundamentalKey.modify(r->rFundamentalKey, reverse);
    if (dirty[ResonanceClosestKeys]) rResonanceKeys.modify(r->rResonanceKeys, reverse);
    if (dirty[ResonanceOffsets]) rOffsetsKeys.modify(r->rOffsetsKeys, reverse);
    if (dirty[ResonanceGains]) rGainsKeys.modify(r->rGainsKeys, reverse);
    
    if (dirty[ResonanceADSR])
    {
        rAttack.modify(r->rAttack, reverse);
        rDecay.modify(r->rDecay, reverse);
        rSustain.modify(r->rSustain, reverse);
        rRelease.modify(r->rRelease, reverse);
    }
    
    if (dirty[ResonanceUseGlobalSoundSet]) rUseGlobalSoundSet.modify(r->rUseGlobalSoundSet, reverse);
    if (dirty[ResonanceSoundSet])
    {
        rSoundSet.modify(r->rSoundSet, reverse);
        rSoundSetName.modify(r->rSoundSetName, reverse);
    }
    
    // If the mod didn't reverse, then it is modded
    modded = !reverse;

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
    for (int j = 0; j < 128; j++)
    {
        velocities.add(Array<float>());
        invertVelocities.add(Array<float>());
        for (int i = 0; i < TargetTypeNil; ++i)
        {
            velocities.getReference(j).add(-1.f);
            invertVelocities.getReference(j).add(-1.f);
        }
    }
    
    if (!resonance->prep->rUseGlobalSoundSet.value)
    {
        String name = resonance->prep->rSoundSetName.value;
        int Id = synth->processor.findPathAndLoadSamples(name);
        resonance->prep->setSoundSet(Id);
    }
    
    /*
    partialStructure.add({0,  1.0, 0});
    partialStructure.add({12, 0.8, 0});
    partialStructure.add({19, 0.7, 2});
    partialStructure.add({24, 0.8, 0});
    partialStructure.add({28, 0.6, -13.7});
    partialStructure.add({31, 0.7, 2});
    partialStructure.add({34, 0.5, -31.175});
    partialStructure.add({36, 0.8, 0});
    */

    DBG("Create rProc");
}

ResonanceProcessor::~ResonanceProcessor()
{
    DBG("Destroy rProc");
}

// this will cause all the resonating strings that have overlapping partials with this new struck string to ring sympathetically
// N*M^2, where N is number of notes held, M is number of partials in partial structure
void ResonanceProcessor::ringSympStrings(int noteNumber, float velocity)
{
    // DBG("Resonance::ringSympStrings");
    
    // resonate existing sympStrings
    // see if there is overlap with the newly pressed key's partials and any sympPartials

    for (HashMap<int, Array<SympPartial::Ptr>>::Iterator heldNotePartials (sympStrings); heldNotePartials.next();)
    {
        DBG("Resonance::ringSympStrings: iterating through sympStrings");
        // indexed by heldNote (midiNoteNumber)
        // iterate through partials of incoming note (noteNumber) and see if they are contained in this set of heldNotePartials
        //for (int j = 0; j < partialStructure.size(); j++)
        for (int j = 0; j < resonance->prep->getPartialStructure().size(); j++)
        {
            DBG("Resonance::ringSympStrings: iterating through partialStructure");
            // strucknote (noteNumber) + partialKey offset (key closest to this partial)
            //int currentStruckPartial = noteNumber + partialStructure.getReference(j)[0];
            int currentStruckPartial = noteNumber + resonance->prep->getPartialStructure().getReference(j)[0];
            
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
                        synth->keyOff(1,
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
                                1,
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
                                1,
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
void ResonanceProcessor::addSympStrings(int noteNumber, float velocity)
{
    if(sympStrings.size() > resonance->prep->getMaxSympStrings())
    {
        DBG("Resonance: removing oldest sympathetic string");
        int oldestString = activeSympStrings.getLast();
        removeSympStrings(oldestString, velocity);
        activeSympStrings.removeLast();
    }
    
    DBG("Resonance: addingSympatheticString " + String(noteNumber));
    //for (int i = 0; i < partialStructure.size(); i++)
    //resonance->prep->getPartialStructure()
    for (int i = 0; i < resonance->prep->getPartialStructure().size(); i++)
    {
        // heldKey      = noteNumber
        // partialKey   = key that this partial is nearest, as assigned by partialStructure
        int partialKey = noteNumber + resonance->prep->getPartialStructure().getUnchecked(i)[0];
        if (partialKey > 108 || partialKey < 21) continue;

        // make a newPartial object, with gain and offset vals
        DBG("Resonance: adding partial " + String(partialKey) + " to " + String(noteNumber));
        sympStrings.getReference(noteNumber).add(new SympPartial(noteNumber, partialKey, resonance->prep->getPartialStructure()[i][1], resonance->prep->getPartialStructure()[i][2]));
        
        // add to list of currently active strings, for voice management
        activeSympStrings.insert(0, noteNumber);
    }

    DBG("Resonance: number of partials = " + String(sympStrings[noteNumber].size()));
}

// this will turn off all the resonances associated with this string/key, and then remove those from the currently available sympathetic strings
void ResonanceProcessor::removeSympStrings(int noteNumber, float velocity)
{
    // turn off each partial associated with this string
    DBG("Resonance: removing partials of " + String(noteNumber));
    for (auto sympString : sympStrings.getReference(noteNumber))
    {
        DBG("Resonance: removing partial " + String(sympString->partialKey) + " of held key " + String(sympString->heldKey));
        
        synth->keyOff(1,
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

void ResonanceProcessor::keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an actual key press (not an inverted release) aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &velocities.getReference(noteNumber);
        for (int i = TargetTypeResonance; i < TargetTypeResonance+1; ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i));
        }
    }
    // If this an inverted release, aVels will be the incoming velocities,
    // but bVels will use the values from the last inverted press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = &invertVelocities.getReference(noteNumber);
    }
    
    if (bVels->getUnchecked(TargetTypeResonance) < 0.f) return;
    
    // resonate the currently available strings and their overlapping partials
    ringSympStrings(noteNumber, aVels->getUnchecked(TargetTypeResonance));

    // then, add this new string and its partials to the currently available sympathetic strings
    addSympStrings(noteNumber, aVels->getUnchecked(TargetTypeResonance));
}

void ResonanceProcessor::keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &invertVelocities.getReference(noteNumber);
        for (int i = TargetTypeResonance; i < TargetTypeResonance+1; ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i));
        }
    }
    // If this an actual release, aVels will be the incoming velocities,
    // but bVels will use the values from the last press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = &velocities.getReference(noteNumber);
    }
    
    if (bVels->getUnchecked(TargetTypeResonance) < 0.f) return;
    
    // this will turn off all the resonace associated with this string/key, and then remove those from the currently available sympathetic strings
    removeSympStrings(noteNumber, aVels->getUnchecked(TargetTypeResonance));
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

