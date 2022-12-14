/*
  ==============================================================================

    Resonance.cpp
    Created: 12 May 2021 12:41:26pm
    Author:  Dan Trueman and Theodore R Trevisan
 
    Name candidate for this _preparation: Understrings, Undersonic
        after hardanger understrings, but also the sense that these sounds are "under" the main sounds of the piano
        also, U is available as a keystroke ;--}

  ==============================================================================
*/

#include "Resonance.h"
#include "PluginProcessor.h"
#include "Modification.h"

void ResonancePreparation::performModification(ResonanceModification* _r, Array<bool> dirty)
{
    DBG("ResonancePreparation::performModification");
    // Should the mod be reversed?
    ResonancePreparation* r = dynamic_cast<ResonancePreparation*>(_r->getPrep().get());
    bool reverse = _r->altMod && modded;
    
    if (dirty[ResonanceGain]) defaultGain.modify(r->defaultGain, reverse);
    if (dirty[ResonanceBlendronicGain]) rBlendronicGain.modify(r->rBlendronicGain, reverse);
    if (dirty[ResonanceMinStartTime]) rMinStartTimeMS.modify(r->rMinStartTimeMS, reverse);
    if (dirty[ResonanceMaxStartTime]) rMaxStartTimeMS.modify(r->rMaxStartTimeMS, reverse);
    
    if (dirty[ResonanceFundamental]) rFundamentalKey.modify(r->rFundamentalKey, reverse);
    if (dirty[ResonanceClosestKeys]) rResonanceKeys.modify(r->rResonanceKeys, reverse);
    if (dirty[ResonanceOffsets]) rOffsetsKeys.modify(r->rOffsetsKeys, reverse);
    if (dirty[ResonanceGains]) rGainsKeys.modify(r->rGainsKeys, reverse);
    if (dirty[ResonanceHeld])
    {
        for (auto n : rActiveHeldKeys.value)
        {
            removeSympStrings(n, 0);
            clearSympString(n);
        }
        
        rActiveHeldKeys.modify(r->rActiveHeldKeys,reverse);
        for (auto n : rActiveHeldKeys.value)
        {
            addSympStrings(n, 0, false);
        }
    }
    
    if (dirty[ResonanceADSR])
    {
        rAttack.modify(r->rAttack, reverse);
        rDecay.modify(r->rDecay, reverse);
        rSustain.modify(r->rSustain, reverse);
        rRelease.modify(r->rRelease, reverse);
    }
    
    if (dirty[ResonanceUseGlobalSoundSet]) useGlobalSoundSet.modify(r->useGlobalSoundSet, reverse);
    if (dirty[ResonanceSoundSet])
    {
        soundSet.modify(r->soundSet, reverse);
        soundSetName.modify(r->soundSetName, reverse);
    }
    
    // If the mod didn't reverse, then it is modded
    modded = !reverse;

    updatePartialStructure();

}

Array<int> ResonancePreparation::getRingingStrings()
{
    Array<int> arr;
    ReferenceCountedArray<BKSynthesiserVoice>& voices = synth->getVoices();
    for (auto voice : voices)
    {
        arr.add(voice->getCurrentlyPlayingNote());
    }
    return arr;
}

SympPartial::SympPartial(int newHeldKey, int newPartialKey, float newGain, float newOffset) 
{
    heldKey     = newHeldKey;
    partialKey  = newPartialKey;
    gain        = newGain;
    offset      = newOffset;

    //playPosition = maxPlayPosition;
}

ResonanceProcessor::ResonanceProcessor(ResonancePreparation::Ptr rResonance,
                                       TuningProcessor::Ptr rTuning,
                                       GeneralSettings::Ptr rGeneral,
                                       BKAudioProcessor &processor):
                                       GenericProcessor( processor, PreparationTypeResonance, rResonance, TargetTypeResonanceAdd, TargetTypeResonanceRing),
                                       tuning(rTuning),
                                       general(rGeneral),
                                       keymaps(Keymap::PtrArr())
{
    ////ADDSYNTH
    ResonancePreparation* _prep = dynamic_cast<ResonancePreparation*>(prep.get());
    _prep->synth = new BKSynthesiser(processor, processor.gallery->getGeneralSettings(), processor.mainPianoSoundSet);
    for (int i = 0; i < 300; i++)
    {
        _prep->synth->addVoice(new BKPianoSamplerVoice(processor.gallery->getGeneralSettings()));
    }
    
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
    
    if (!_prep->useGlobalSoundSet.value)
    {
        String name = _prep->soundSetName.value;
        int Id = processor.findPathAndLoadSamples(name);
        _prep->soundSet = Id;
    }
    for (int i : _prep->rActiveHeldKeys.value)
    {
        //prep->addSympStrings(i, 127);
        _prep->addSympStrings(i, 127, false);
    }

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
    TuningProcessor* _tuner =  dynamic_cast<TuningProcessor*>(tuning.get());
    ResonancePreparation* _prep = dynamic_cast<ResonancePreparation*>(prep.get());
    // resonate existing sympStrings
    // see if there is overlap with the newly pressed key's partials and any sympPartials
    for (HashMap<int, Array<SympPartial::Ptr>>::Iterator heldNotePartials (_prep->sympStrings); heldNotePartials.next();)
    {
        // DBG("Resonance::ringSympStrings: iterating through sympStrings");
        // indexed by heldNote (midiNoteNumber)
        // iterate through partials of incoming note (noteNumber) and see if they are contained in this set of heldNotePartials
        for (int j = 0; j < _prep->getPartialStructure().size(); j++)
        {
            // DBG("Resonance::ringSympStrings: iterating through partialStructure");
            // strucknote (noteNumber) + partialKey offset (key closest to this partial)
            int currentStruckPartial = noteNumber + _prep->getPartialStructure().getReference(j)[0];
            
            for (auto currentSympPartial : *heldNotePartials)
            {
                if (currentSympPartial->partialKey == currentStruckPartial)
                {
                    // found an overlapping partial!
                    
                    // calculate play position for this new resonance (ms)
                    // based on velocity; so higher velocity, the further into the sample it will play
                    int newPlayPosition = _prep->getMinStartTime()
                                        + (_prep->getMaxStartTime() - _prep->getMinStartTime())
                                        * (1. - velocity);
                    
                    //DBG("Resonance: found an overlapping partial, currentPlayPosition = "
                        //+ String(currentSympPartial->playPosition  / (.001 *  _prep->synth->getSampleRate()))
                        //+ " newPlayPosition = " + String(newPlayPosition));
                    
                    // only create a new resonance if it would be louder/brighter than what is currently there
                    //      so, only if the newPlayPosition is less than the current play position
                    //if (newPlayPosition < currentSympPartial->playPosition  / (.001 *  _prep->synth->getSampleRate()))
                    // ** not convinced this is a good conditional; doesn't seem to help performance much, and is noticable sonically
                    //if (newPlayPosition < (currentSympPartial->getPlayPosition(_prep->getCurrentTime()) / (.001 *  _prep->synth->getSampleRate())))
                    {
                        // set the start time for this new resonance
                        //currentSympPartial->playPosition = newPlayPosition * (.001 *  _prep->synth->getSampleRate());
                        currentSympPartial->setStartTime((uint64)(newPlayPosition * (.001 *  _prep->synth->getSampleRate())),
                                                         _prep->getCurrentTime());
                        
                        // turn off the current resonance here, if it's playing
                        // ** don't do the followign if not doing the conditional above
                        /*
                        _prep->synth->keyOff(1,
                                      ResonanceNote,
                                      _prep->getSoundSet(),
                                      resonance->getId(),
                                      currentSympPartial->heldKey,
                                      currentSympPartial->partialKey,
                                      64,
                                      aGlobalGain,
                                      _prep->getDefaultGainPtr(),
                                      true, // need to test more here
                                      false);
                         */

                        // calculate the tuning gap between attached tuning and the tuning of this partial
                        // taking into account attached Tuning system, and defined partial structure (which may or may not be the same!)
                        float tuningGap = fabs( _tuner->getOffset(noteNumber, false)
                                               - (_tuner->getOffset(currentSympPartial->heldKey, false) + .01 * currentSympPartial->offset)
                                              ) / .5; // in fractional note values

                        if (tuningGap > 2.) tuningGap = 2.;
                        //DBG("Resonance: tuningGap = " + String(tuningGap));

                        // play it, use max velocity for all resonance; loudness/brightness is set by newPlayPosition
                        // adjust gain according to gap in tuning; 50 cent gap will result in 6dB cut in gain
                        //      might be better to adjust playback position due to gap, instead of adjusting the gain?
                        /*
                        DBG("Resonance playNote: "   + String(noteNumber)
                                                    + " heldKey = " + String(currentSympPartial->heldKey)
                                                    + " partialKey = " + String(currentStruckPartial)
                                                    + " heldKey offset = " + String(tuning->getOffset(noteNumber, false))
                                                    + " partialKey offset = " + String(tuning->getOffset(currentSympPartial->heldKey, false) + .01 * currentSympPartial->offset));
                         */
                        
                        Array<float> ADSRvals = _prep->getADSRvals();
                        _prep->synth->keyOn(
                                1,
                                //noteNumber,
                                currentSympPartial->heldKey,
                                currentStruckPartial,
                                _tuner->getOffset(currentSympPartial->heldKey, false) + currentSympPartial->offset * .01,
                                //tuning->getOffset(noteNumber, false),
                                1.,
                                aGlobalGain * currentSympPartial->gain * (1. - 0.5 * tuningGap),
                                //aGlobalGain,
                                Forward,
                                NormalFixedStart,
                                ResonanceNote,
                                _prep->getSoundSet(), //set
                                prep->getId(),
                                newPlayPosition,                //start position
                                0,                              //play length
                                ADSRvals[0],
                                ADSRvals[1],
                                ADSRvals[2],
                                ADSRvals[3],
                                tuning,
                                _prep->getDefaultGainPtr());
                    }
                }
            }
        }
    }
}

void ResonancePreparation::addSympStrings(int noteNumber, float velocity)
{
    //addSympStrings(noteNumber, velocity, false);
    addSympStrings(noteNumber, velocity, true);
}

// this will add this string and all its partials to the currently available sympathetic strings (sympStrings)
void ResonancePreparation::addSympStrings(int noteNumber, float velocity, bool ignoreRepeatedNotes)
{
    const ScopedLock sl (lock);
    
    // don't add a symp string that is already there
    //DBG("Held Key already contains this note = " + String((int)getHeldKeys().contains(noteNumber)));
    if(getHeldKeys().contains(noteNumber) && ignoreRepeatedNotes) {
        //DBG("ResonancePreparation::addSympStrings, not adding new string as it is already there");
        return;
    }
    
    if(sympStrings.size() > getMaxSympStrings())
    {
        //DBG("Resonance: removing oldest sympathetic string");
        // might want to offer some more options here:
            // for instance, remove highest note (since that will have the fewest resonances in most cases)
            // or remove quietest? probably harder...
        int oldestString = rActiveHeldKeys.value.getLast();
        removeSympStrings(oldestString, velocity);
        sympStrings.remove(oldestString); // just added
        rActiveHeldKeys.arrayRemoveAllInstancesOf(oldestString);
        //_prep->rActiveHeldKeys.value.removeLast();
    }
    
    //DBG("Resonance: addingSympatheticString " + String(noteNumber));
    //for (int i = 0; i < partialStructure.size(); i++)
    //_prep->getPartialStructure()
    for (int i = 0; i < getPartialStructure().size(); i++)
    {
        // heldKey      = noteNumber
        // partialKey   = key that this partial is nearest, as assigned by partialStructure
        int partialKey = noteNumber + getPartialStructure().getUnchecked(i)[0];
        if (partialKey > 127 || partialKey < 0) continue;

        // make a newPartial object, with gain and offset vals
        //DBG("Resonance: adding partial " + String(partialKey) + " to " + String(noteNumber));
        sympStrings.getReference(noteNumber).add(new SympPartial(noteNumber, partialKey, getPartialStructure()[i][1], getPartialStructure()[i][2]));
    }
    // add to list of currently active held keys
    if (!rActiveHeldKeys.value.contains(noteNumber))
        rActiveHeldKeys.value.insert(0, noteNumber);

    //DBG("num active held heys = " + String(rActiveHeldKeys.value.size()));
    //DBG("Resonance: number of partials = " + String(sympStrings[noteNumber].size()));
}

// this will turn off all the resonances associated with this string/key, and then remove those from the currently available sympathetic strings
void ResonancePreparation::removeSympStrings(int noteNumber, float velocity)
{
    const ScopedLock sl (lock);
    
    // turn off each partial associated with this string
    //DBG("Resonance: removing partials of " + String(noteNumber));
    for (auto sympString : sympStrings.getReference(noteNumber))
    {
      //  DBG("Resonance: removing partial " + String(sympString->partialKey) + " of held key " + String(sympString->heldKey));
        
        synth->keyOff(1,
                      ResonanceNote,
                      getSoundSet(),
                      getId(),
                      sympString->heldKey,
                      sympString->partialKey,
                      64,
                      aGlobalGain,
                      getDefaultGainPtr(),
                      true, // need to test more here
                      false);

    }
}



void ResonanceProcessor::keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    ResonancePreparation* _prep = dynamic_cast<ResonancePreparation*>(prep.get());
    // If this is an actual key press (not an inverted release) aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &velocities.getReference(noteNumber);
        for (int i = TargetTypeResonanceAdd; i <= TargetTypeResonanceRing; ++i)
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
    
    bool doRing = (bVels->getUnchecked(TargetTypeResonanceRing) >= 0.f);
    bool doAdd = (bVels->getUnchecked(TargetTypeResonanceAdd) >= 0.f);
    
    if (doRing)
    {
        // resonate the currently available strings and their overlapping partials
        ringSympStrings(noteNumber, aVels->getUnchecked(TargetTypeResonanceRing));
    }
    if (doAdd)
    {
        // then, add this new string and its partials to the currently available sympathetic strings
            // 3rd arg ignore repeated notes = true, so don't add this string if it's already there
        _prep->addSympStrings(noteNumber, aVels->getUnchecked(TargetTypeResonanceAdd), true);
    }
}

void ResonanceProcessor::keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    ResonancePreparation* _prep = dynamic_cast<ResonancePreparation*>(prep.get());
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &invertVelocities.getReference(noteNumber);
        for (int i = TargetTypeResonanceAdd; i < TargetTypeResonanceRing; ++i)
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
    
    bool doRing = (bVels->getUnchecked(TargetTypeResonanceRing) >= 0.f);
    bool doAdd = (bVels->getUnchecked(TargetTypeResonanceAdd) >= 0.f);
    
    /*
    if (doRing && doAdd) // don't want to remove resonating strings if we aren't in doAdd target mode, so both need to be true
    {
        // this will turn off all the resonance associated with this string/key, and then remove those from the currently available sympathetic strings
        _prep->removeSympStrings(noteNumber, aVels->getUnchecked(TargetTypeResonanceRing));
    }
     */
        
    // if (doAdd)
    if ( (doRing && doAdd) || doAdd) // don't want to remove resonating strings if we aren't in doAdd target mode, so doAdd always needs to be true
    {
        // clear this held string's partials
        // DBG("Resonance: clearing sympathetic string: " + String(noteNumber));
        _prep->removeSympStrings(noteNumber, aVels->getUnchecked(TargetTypeResonanceRing));
        _prep->clearSympString(noteNumber);
        
        _prep->rActiveHeldKeys.arrayRemoveAllInstancesOf(noteNumber);
    }
}


void ResonanceProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages, int numSamples, int midiChannel, BKSampleLoadType type)
{
    // should not be any thread safety issues with this...
    // NOTE: this might be necessary anymore, since it is only needed for determining whether to
    //          ring a note that might already be sounding, which is currently commented out
    ResonancePreparation* _prep = dynamic_cast<ResonancePreparation*>(prep.get());
    _prep->updateCurrentTime(numSamples);
    
    /*
    for (HashMap<int, Array<SympPartial::Ptr>>::Iterator heldNotePartials (_prep->sympStrings); heldNotePartials.next();)
    {
        for (auto sympString : *heldNotePartials)
        {
            // if (sympString == nullptr) return; 
            sympString->playPosition += numSamples;
            
            // i think this is still not thread-safe, and I don't want to put a ScopeLock here on the audio thread
            // instead: increment a _prep->samplesFromStart value, which simply counts how many samples have passed since the proc started
            // when a sympString starts, record samplesFromStart as stringStartSample
            // and then sympString->playPosition = samplesFromStart - stringStartSample
            // or some such. that way the processBlock doesn't need to access anything other than the _prep
        }
    }
     */
    _prep->synth->renderNextBlock(buffer, midiMessages, 0, numSamples);
}

