/*
  ==============================================================================

    Nostalgic.cpp
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Nostalgic.h"
#include "PluginProcessor.h"
#include "Modification.h"

void NostalgicPreparation::performModification(NostalgicModification* n, Array<bool> dirty)
{
    // Determin if the mod should be reversed
    bool reverse = n->altMod && modded;
    
    if (dirty[NostalgicGain]) defaultGain.modify(n->getPrep()->defaultGain, reverse);
//    if (dirty[NostalgicBlendronicGain]) nBlendronicGain.modify(n->nBlendronicGain, reverse);
    if (dirty[NostalgicWaveDistance]) nWaveDistance.modify(n->getPrepPtr()->nWaveDistance, reverse);
    if (dirty[NostalgicUndertow]) nUndertow.modify(n->getPrepPtr()->nUndertow, reverse);
    if (dirty[NostalgicTransposition]) nTransposition.modify(n->getPrepPtr()->nTransposition, reverse);
    if (dirty[NostalgicTranspUsesTuning]) nTranspUsesTuning.modify(n->getPrepPtr()->nTranspUsesTuning, reverse);
    if (dirty[NostalgicLengthMultiplier]) nLengthMultiplier.modify(n->getPrepPtr()->nLengthMultiplier, reverse);
    if (dirty[NostalgicBeatsToSkip]) nBeatsToSkip.modify(n->getPrepPtr()->nBeatsToSkip, reverse);
    if (dirty[NostalgicMode]) nMode.modify(n->getPrepPtr()->nMode, reverse);
    if (dirty[NostalgicReverseADSR])
    {
        nReverseAttack.modify(n->getPrepPtr()->nReverseAttack, reverse);
        nReverseDecay.modify(n->getPrepPtr()->nReverseDecay, reverse);
        nReverseSustain.modify(n->getPrepPtr()->nReverseSustain, reverse);
        nReverseRelease.modify(n->getPrepPtr()->nReverseRelease, reverse);
    }
    
    if (dirty[NostalgicUndertowADSR])
    {
        nUndertowAttack.modify(n->getPrepPtr()->nUndertowAttack, reverse);
        nUndertowDecay.modify(n->getPrepPtr()->nUndertowDecay, reverse);
        nUndertowSustain.modify(n->getPrepPtr()->nUndertowSustain, reverse);
        nUndertowRelease.modify(n->getPrepPtr()->nUndertowRelease, reverse);
    }
    
    if (dirty[NostalgicHoldMin]) holdMin.modify(n->getPrepPtr()->holdMin, reverse);
    if (dirty[NostalgicHoldMax]) holdMax.modify(n->getPrepPtr()->holdMax, reverse);
    if (dirty[NostalgicClusterMin]) clusterMin.modify(n->getPrepPtr()->clusterMin, reverse);
    if (dirty[NostalgicClusterThreshold]) clusterThreshold.modify(n->getPrepPtr()->clusterThreshold, reverse);
    if (dirty[NostalgicKeyOnReset]) keyOnReset.modify(n->getPrepPtr()->keyOnReset, reverse);
    if (dirty[NostalgicVelocityMin]) velocityMin.modify(n->getPrepPtr()->velocityMin, reverse);
    if (dirty[NostalgicVelocityMax]) velocityMax.modify(n->getPrepPtr()->velocityMax, reverse);
    
    if (dirty[NostalgicUseGlobalSoundSet]) useGlobalSoundSet.modify(n->getPrep()->useGlobalSoundSet, reverse);
    
    if (dirty[NostalgicSoundSet])
    {
        soundSet.modify(n->getPrep()->soundSet, reverse);
        soundSetName.modify(n->getPrep()->soundSetName, reverse);
    }
    
    // If the mod didn't reverse, then it is modded
    modded = !reverse;
}

NostalgicProcessor::NostalgicProcessor(NostalgicPreparation::Ptr nostalgic,
                                       TuningProcessor::Ptr tuning,
                                       SynchronicProcessor::Ptr synchronic,
                                       BKAudioProcessor& processor
                                       /*BKSynthesiser *s */):
//synth(s),
GenericProcessor(processor, PreparationTypeNostalgic, nostalgic, TargetTypeNostalgic, TargetTypeNostalgicClear),
post(false),
tuner(tuning),
synchronic(synchronic),
keymaps(Keymap::PtrArr()),
synth(new BKSynthesiser(processor, processor.gallery->getGeneralSettings(),processor.mainPianoSoundSet ))
{
    if (!prep->useGlobalSoundSet.value)
    {
        // comes in as "soundfont.sf2.subsound1"
        String name = prep->soundSetName.value;
        int Id = processor.findPathAndLoadSamples(name);
        prep->setSoundSet(Id);
    }
    for (int i = 0; i < 300; i++)
    {
        synth->addVoice(new BKPianoSamplerVoice(processor.gallery->getGeneralSettings()));
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
    
    noteLengthTimers.ensureStorageAllocated(128);
    noteOn.ensureStorageAllocated(128);
    
    for (int i = 0; i < 128; i++)
    {
        noteLengthTimers.insert(i, 0); //initialize timers for all notes
        noteOn.set(i, false);
    }

}

NostalgicProcessor::~NostalgicProcessor()
{
    
}

//begin reverse note; called when key is released
void NostalgicProcessor::postRelease(int midiNoteNumber, int midiChannel)
{
    // turn note length timers off
    activeNotes.removeFirstMatchingValue(midiNoteNumber);
    noteOn.set(midiNoteNumber, false);
    noteLengthTimers.set(midiNoteNumber, 0);
}

//begin reverse note; called when key is released
void NostalgicProcessor::keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    SynchronicProcessor* _synchronic = dynamic_cast<SynchronicProcessor*>(synchronic.get());
    TuningProcessor* _tuner = dynamic_cast<TuningProcessor*>(tuner.get());
    NostalgicPreparation* _prep = dynamic_cast<NostalgicPreparation*>(prep.get());
    
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &invertVelocities.getReference(noteNumber);
        for (int i = TargetTypeNostalgic; i <= TargetTypeNostalgicClear; ++i)
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
    
    bool doNostalgic = bVels->getUnchecked(TargetTypeNostalgic) >= 0.f; // primary Nostalgic mode
    bool doClear = bVels->getUnchecked(TargetTypeNostalgicClear) >= 0.f; // clear all sounding notes
    
    if (doClear &&
        (_prep->targetTypeNostalgicClear.value == NoteOff || _prep->targetTypeNostalgicClear.value == Both))
    {
        clearAll(1);
    }
    
    if (!doNostalgic) return;
    
    float duration = 0.0;
    
    if (post || noteOn[noteNumber])
    {
        
        int offRamp;
        if (_prep->nUndertow.value > 0) offRamp = aRampUndertowCrossMS;
        else offRamp = aRampNostalgicOffMS;

        SynchronicSyncMode syncTargetMode = dynamic_cast<SynchronicPreparation*>(_synchronic->getPrep().get())->sMode.value;
        
        if (_prep->nMode.value == SynchronicSync2)
        {
            float duration = 0.0;
            
            int offRamp;
            if (_prep->nUndertow.value > 0) offRamp = aRampUndertowCrossMS;
            else offRamp = aRampNostalgicOffMS;
            
            duration = _synchronic->getTimeToBeatMS(_prep->nBeatsToSkip.value) + offRamp + 30; // sum
            
            for (auto t : _prep->nTransposition.value)
            {
                
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                 */
                
                int synthNoteNumber = noteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                
                // tune the transposition
                if (_prep->nTranspUsesTuning.value) // use the Tuning setting
                    offset = t + _tuner->getOffset(round(t) + noteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                    offset = t + _tuner->getOffset(noteNumber, false);
               
                synthOffset = offset;

                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note

                DBG("reverse note on noteNum/offset/velocity " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset) + " " +
                    String(aVels->getUnchecked(0)));
               
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(TargetTypeNostalgic),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 prep->getId(),
                                 duration + _prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 _prep->nReverseAttack.value,
                                 _prep->nReverseDecay.value,
                                 _prep->nReverseSustain.value,
                                 _prep->nReverseRelease.value,
                                 tuner,
                                 prep->getDefaultGainPtr());
            }
            
            activeNotes.removeFirstMatchingValue(noteNumber);
            noteOn.set(noteNumber, false);
            noteLengthTimers.set(noteNumber, 0);
            
            reverseNotes.insert(0, new NostalgicNoteStuff(noteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(_prep);
            currentNote->setTuningAtKeyOn(_tuner->getOffset(noteNumber, false));
            currentNote->setVelocityAtKeyOn(aVels->getUnchecked(TargetTypeNostalgic));
            currentNote->setReverseStartPosition((duration + _prep->nWaveDistance.value) * synth->getSampleRate()/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(_prep->nUndertow.value * synth->getSampleRate()/1000.);
            
        }
        else if (_prep->nMode.value == NoteLengthSync)
        {
            //===========CLUSTER MANAGEMENT===========
            if(!inCluster) //we have a new cluster
            {
                // clear cluster
                cluster.clearQuick();

                currentClusterSize = 0;
                //clusterNotesPlayed.clearQuick();
                
                // now we are in a cluster!
                inCluster = true;
            }
            
            //cluster.addIfNotAlreadyThere(midiNoteNumber);
            cluster.add(noteNumber);
            
            currentClusterSize++;
            
            //reset the timer for time between notes
            clusterThresholdTimer = 0;
            
            playCluster = false;
            
            //if (inCluster) //inCluster will ALWAYS be true here
            {
                //if (cluster.size() >= prep->getClusterMin())
                if (currentClusterSize >= _prep->clusterMin.value)
                {
                    playCluster = true;
                }
            }
            //=========================================
            
            if (playCluster)
            {
                for (auto note : cluster)
                {
                    //int note = midiNoteNumber; //fixes repetition issue in NS_1, but breaks clusters
                    bool passHoldTest = false;
                    
                    float held = noteLengthTimers.getUnchecked(note) * (1000.0 / synth->getSampleRate());
                    
                    if (_prep->holdMin.value <= _prep->holdMax.value)
                    {
                        if (held >= _prep->holdMin.value && held <= _prep->holdMax.value)
                        {
                            passHoldTest = true;
                        }
                    }
                    else
                    {
                        if (held >= _prep->holdMin.value || held <= _prep->holdMax.value)
                        {
                            passHoldTest = true;
                        }
                    }
                    
                    bool playNote = passHoldTest;
            
                    if (!playNote) continue;

                    //clusterNotesPlayed.addIfNotAlreadyThere(note);
                    
                    //get length of played notes, subtract wave distance to set nostalgic reverse note length
                    duration =  (noteLengthTimers.getUnchecked(note) *
                                 _prep->nLengthMultiplier.value +
                                 (offRamp + 30)) * (1000.0 / synth->getSampleRate());
                    
                    for (auto t : _prep->nTransposition.value)
                    {
                        /*
                        float offset = t + tuner->getOffset(note, false), synthOffset = offset;
                        int synthNoteNumber = note;
                         */
                        
                        // int synthNoteNumber = midiNoteNumber;
                        int synthNoteNumber = note;
                        float offset; // offset from integer, but may be greater than 1
                        float synthOffset; // offset from actual sample played, always less than 1.
                         
                        // tune the transposition
                        if (_prep->nTranspUsesTuning.value) // use the Tuning setting
                             // offset = t + tuner->getOffset(round(t)  + midiNoteNumber, false);
                            offset = t + _tuner->getOffset(round(t)  + note, false);
                        else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                             // offset = t + tuner->getOffset(midiNoteNumber, false);
                            offset = t + _tuner->getOffset(note, false);
                        
                        synthOffset = offset;
                        
                        synthNoteNumber += (int)offset;
                        synthOffset     -= (int)offset;
                        
                        DBG("Nostalgic::keyReleased duration, waveDistance : " + String(duration) + " " + String(_prep->nWaveDistance.value));
                        
                   
                            synth->keyOn(1,
                                         note,
                                         synthNoteNumber,
                                         synthOffset,
                                         aVels->getUnchecked(TargetTypeNostalgic),
                                         aGlobalGain,
                                         Reverse,
                                         FixedLengthFixedStart,
                                         NostalgicNote,
                                         prep->getSoundSet(), //set
                                         prep->getId(),
                                         duration + _prep->nWaveDistance.value,
                                         duration,  // length
                                         // duration + prep->nReverseRelease.value,
                                         _prep->nReverseAttack.value,
                                         _prep->nReverseDecay.value,
                                         _prep->nReverseSustain.value,
                                         _prep->nReverseRelease.value,
                                         tuner,
                                         prep->getDefaultGainPtr());
                        
                        reverseNotes.insert(0, new NostalgicNoteStuff(note));
                        NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
                        currentNote->setPrepAtKeyOn(_prep);
                        currentNote->setTuningAtKeyOn(_tuner->getOffset(note, false));
                        currentNote->setVelocityAtKeyOn(aVels->getUnchecked(TargetTypeNostalgic));
                        currentNote->setReverseStartPosition((duration + _prep->nWaveDistance.value) * synth->getSampleRate()/1000.);
                        currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
                        currentNote->setUndertowTargetLength(_prep->nUndertow.value * synth->getSampleRate()/1000.);
                        
                        noteLengthTimers.set(note, 0);
                    }
                }
                cluster.clearQuick();
            }
            
            
            // turn note length timers off
            activeNotes.removeFirstMatchingValue(noteNumber);
            noteOn.set(noteNumber, false);
            //DBG("nostalgic removed active note " + String(midiNoteNumber));
            
        }
        else if(syncTargetMode == LastNoteOffSync || syncTargetMode == AnyNoteOffSync)
        {
            duration = _synchronic->getTimeToBeatMS(_prep->nBeatsToSkip.value) + offRamp + 30; // sum
            
            for (auto t : _prep->nTransposition.value)
            {
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                int synthNoteNumber = midiNoteNumber;
                 */
                
                int synthNoteNumber = noteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                 
                 // tune the transposition
                if (_prep->nTranspUsesTuning.value) // use the Tuning setting
                     offset = t + _tuner->getOffset(round(t) + noteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                     offset = t + _tuner->getOffset(noteNumber, false);
                
                synthOffset = offset;
                
                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note
                DBG("reverse note on noteNum/offset " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset));
                
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(TargetTypeNostalgic),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 prep->getId(),
                                 duration + _prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 _prep->nReverseAttack.value,
                                 _prep->nReverseDecay.value,
                                 _prep->nReverseSustain.value,
                                 _prep->nReverseRelease.value,
                                 tuner,
                                 _prep->getDefaultGainPtr());
                    
            }
            
            reverseNotes.insert(0, new NostalgicNoteStuff(noteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(_prep);
            currentNote->setTuningAtKeyOn(_tuner->getOffset(noteNumber, false));
            currentNote->setVelocityAtKeyOn(aVels->getUnchecked(TargetTypeNostalgic));// * _prep->getDefaultGain());
            currentNote->setReverseStartPosition((duration + _prep->nWaveDistance.value) * synth->getSampleRate()/1000.);
            //currentNote->setReverseTargetLength((duration - aRampUndertowCrossMS) * sampleRate/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(_prep->nUndertow.value * synth->getSampleRate()/1000.);
        }
    }

}

//start timer for length of a particular note; called when key is pressed
void NostalgicProcessor::keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    SynchronicProcessor* _synchronic = dynamic_cast<SynchronicProcessor*>(synchronic.get());
    TuningProcessor* _tuner = dynamic_cast<TuningProcessor*>(tuner.get());
    NostalgicPreparation* _prep = dynamic_cast<NostalgicPreparation*>(prep.get());
    
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an actual key press (not an inverted release) aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &velocities.getReference(noteNumber);
        for (int i = TargetTypeNostalgic; i <= TargetTypeNostalgicClear; ++i)
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
    
    bool doNostalgic = bVels->getUnchecked(TargetTypeNostalgic) >= 0.f; // primary Nostalgic mode
    bool doClear = bVels->getUnchecked(TargetTypeNostalgicClear) >= 0.f; // clear all sounding notes
    
    noteOn.set(noteNumber, true);
    
    DBG("note = " + String(noteNumber) + " channel = " + " doClear = " + String((int)doClear));
    
    if (doClear &&
        (_prep->targetTypeNostalgicClear.value == NoteOn || _prep->targetTypeNostalgicClear.value == Both))
    {
        clearAll(1);
    }
    
    if (!doNostalgic) return;
    
    if (_prep->nMode.value == SynchronicSync)
    {
        float duration = 0.0;
        
        int offRamp;
        if (_prep->nUndertow.value > 0) offRamp = aRampUndertowCrossMS;
        else offRamp = aRampNostalgicOffMS;
        
        //get time in ms to target beat, summing over skipped beat lengths
        SynchronicSyncMode syncTargetMode = _synchronic->getMode();
        
        if(syncTargetMode == FirstNoteOnSync || syncTargetMode == AnyNoteOnSync)
        {
            duration = _synchronic->getTimeToBeatMS(_prep->nBeatsToSkip.value) + offRamp + 30; // sum
            
            for (auto t :_prep->nTransposition.value)
            {
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                int synthNoteNumber = midiNoteNumber;
                 */
                
                int synthNoteNumber = noteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                 
                // tune the transposition
                if (_prep->nTranspUsesTuning.value) // use the Tuning setting
                     offset = t + _tuner->getOffset(round(t) + noteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                     offset = t + _tuner->getOffset(noteNumber, false);
                
                synthOffset = offset;
                
                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note
                DBG("reverse note on noteNum/offset " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset));
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(TargetTypeNostalgic),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 prep->getId(),
                                 duration + _prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 _prep->nReverseAttack.value,
                                 _prep->nReverseDecay.value,
                                 _prep->nReverseSustain.value,
                                 _prep->nReverseRelease.value,
                                 tuner,
                                 prep->getDefaultGainPtr());
            }
            
            reverseNotes.insert(0, new NostalgicNoteStuff(noteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(_prep);
            currentNote->setTuningAtKeyOn(_tuner->getOffset(noteNumber, false));
            currentNote->setVelocityAtKeyOn(aVels->getUnchecked(TargetTypeNostalgic));
            currentNote->setReverseStartPosition((duration + _prep->nWaveDistance.value) * synth->getSampleRate()/1000.);
            //currentNote->setReverseTargetLength((duration - aRampUndertowCrossMS) * sampleRate/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(_prep->nUndertow.value * synth->getSampleRate()/1000.);
        }
    }
    
    //activeNotes is for measuring lengths of held notes, so only relevant in NoteLengthSync mode
    if (_prep->nMode.value == NoteLengthSync)
    {
        activeNotes.addIfNotAlreadyThere(noteNumber);
        noteLengthTimers.set(noteNumber, 0);
        lastKeyPlayed = noteNumber;
        
        if(!inCluster)
        {
            currentClusterSize = 0;
        }
        
        // KEY ON RESET STUFF
        if (_prep->keyOnReset.value)
        {
            // REMOVE REVERSE NOTES
            for (int i = reverseNotes.size(); --i >= 0;)
            {
                NostalgicNoteStuff* note = reverseNotes[i];
                
                if ((note->getNoteNumber() == noteNumber) && note->isActive())
                {
                    if (_prep != nullptr)
                    {
                        for (auto transp :_prep->nTransposition.value)
                        {
                            DBG("reverse remove: " + String(noteNumber + transp));
                            synth->keyOff (1,
                                           NostalgicNote,
                                           _prep->getSoundSet(),
                                           prep->getId(),
                                           noteNumber,
                                           noteNumber+transp,
                                           64,
                                           aGlobalGain,
                                           prep->getDefaultGainPtr(),
                                           true,
                                           true); // true for nostalgicOff
                        }
                    }
                    reverseNotes.remove(i);
                }
            }
            
            // REMOVE UNDERTOW NOTES
            for (int i = undertowNotes.size(); --i >= 0;)
            {
                NostalgicNoteStuff* note = undertowNotes[i];
                
                if ((note->getNoteNumber() == noteNumber))
                {
                    if (prep != nullptr)
                    {
                        for (auto transp :_prep->nTransposition.value)
                        {
                            DBG("undertow remove: " + String(noteNumber + transp));
                            synth->keyOff (1,
                                           NostalgicNote,
                                           prep->getSoundSet(),
                                           prep->getId(),
                                           noteNumber,
                                           noteNumber+transp,
                                           64,
                                           aGlobalGain,
                                           _prep->getDefaultGainPtr(),
                                           false, // THIS NEEDS TO BE FALSE ATM TO GET RID OF UNDERTOW NOTES : because our processsPiano doesn't distinguish between true keyOn/Offs from keyboard and these under the hood keyOn/Offs (which don't correspond with Off/On from keyboard)
                                           true); // true for nostalgicOff
                        }
                    }
                    undertowNotes.remove(i);
                }
            }
        }
        
    }
}

// clear all sounding notes
void NostalgicProcessor::clearAll(int midiChannel)
{
    NostalgicPreparation* _prep = dynamic_cast<NostalgicPreparation*>(prep.get());
    
    // REMOVE REVERSE NOTES
    for (int i = reverseNotes.size(); --i >= 0;)
    {
        NostalgicNoteStuff* note = reverseNotes[i];
        
        if (note->isActive())
        {
            if (prep != nullptr)
            {
                for (auto transp :_prep->nTransposition.value)
                {
                    DBG("reverse remove: " + String(note->getNoteNumber() + transp));
                    synth->keyOff (midiChannel,
                                   NostalgicNote,
                                   prep->getSoundSet(),
                                   prep->getId(),
                                   note->getNoteNumber(),
                                   note->getNoteNumber()+transp,
                                   64,
                                   aGlobalGain,
                                   _prep->getDefaultGainPtr(),
                                   true,
                                   true); // true for nostalgicOff
                }
            }
            reverseNotes.remove(i);
        }
    }
    
    // REMOVE UNDERTOW NOTES
    for (int i = undertowNotes.size(); --i >= 0;)
    {
        NostalgicNoteStuff* note = undertowNotes[i];
        
        if (prep != nullptr)
        {
            for (auto transp :_prep->nTransposition.value)
            {
                DBG("undertow remove: " + String(note->getNoteNumber() + transp));
                synth->keyOff (midiChannel,
                               NostalgicNote,
                               prep->getSoundSet(),
                               prep->getId(),
                               note->getNoteNumber(),
                               note->getNoteNumber() + transp,
                               64,
                               aGlobalGain,
                               _prep->getDefaultGainPtr(),
                               false, // THIS NEEDS TO BE FALSE ATM TO GET RID OF UNDERTOW NOTES : because our processsPiano doesn't distinguish between true keyOn/Offs from keyboard and these under the hood keyOn/Offs (which don't correspond with Off/On from keyboard)
                               true); // true for nostalgicOff
            }
        }
        undertowNotes.remove(i);
    }
}

float NostalgicProcessor::filterVelocity(float vel)
{
    NostalgicPreparation* _prep = dynamic_cast<NostalgicPreparation*>(prep.get());
    
    if (!lastVelocityInRange) lastVelocity = vel;
    
    int velocity = vel*127.f;
    
    if(_prep->velocityMin.value <=_prep->velocityMax.value)
    {
        if (velocity >=_prep->velocityMin.value && velocity <=_prep->velocityMax.value)
        {
            lastVelocityInRange = true;
            lastVelocity = vel;
            return vel;
        }
    }
    else
    {
        if (velocity >=_prep->velocityMin.value || velocity <=_prep->velocityMax.value)
        {
            lastVelocityInRange = true;
            lastVelocity = vel;
            return vel;
        }
    }
    
    DBG("failed velocity check");
    return -1.f;
}

//main scheduling function
void NostalgicProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages, int numSamples, int midiChannel, BKSampleLoadType type)
{
    SynchronicProcessor* _synchronic = dynamic_cast<SynchronicProcessor*>(synchronic.get());
    TuningProcessor* _tuner = dynamic_cast<TuningProcessor*>(tuner.get());
    //cluster management
    NostalgicPreparation* _prep = dynamic_cast<NostalgicPreparation*>(prep.get());
    if (inCluster)
    {
        
        //moved beyond clusterThreshold time, done with cluster
        //if (clusterThresholdTimer >= (sampleRate * 0.15)) //eventually expose this threshold
        if (clusterThresholdTimer >= (synth->getSampleRate() * .001 *_prep->clusterThreshold.value))
        {
            inCluster = false;
            currentClusterSize = 0;
            //clusterNotesPlayed.clearQuick();
        }
        //otherwise increment cluster timer
        else
        {
            clusterThresholdTimer += numSamples;
        }
    }
    
    sampleType = type;
    incrementTimers(numSamples);

    for(int i = undertowNotes.size() - 1; i >= 0; --i)
    {
        NostalgicNoteStuff* thisNote = undertowNotes.getUnchecked(i);
        
        if(thisNote->undertowTimerExceedsTarget() || !thisNote->isActive())
            undertowNotes.remove(i);
    }
    
    for(int i = reverseNotes.size() - 1; i >= 0; --i)
    //for(int i=0; i<reverseNotes.size(); i++)
    {
        NostalgicNoteStuff* thisNote = reverseNotes.getUnchecked(i);

        if (thisNote->reverseTimerExceedsTarget())
        {
            NostalgicPreparation* noteOnPrep = dynamic_cast<NostalgicPreparation*>(thisNote->getPrepAtKeyOn().get());
            
            if(noteOnPrep->nUndertow.value > 0)
            {
                for (auto t : noteOnPrep->nTransposition.value)
                {
                    /*
                    float offset = t + thisNote->getTuningAtKeyOn();
                    float synthOffset = offset;
                     */
                    
                    int synthNoteNumber = thisNote->getNoteNumber();
                    int midiNoteNumber = synthNoteNumber;
                    
                    float offset; // offset from integer, but may be greater than 1
                    float synthOffset; // offset from actual sample played, always less than 1.
                      
                    // tune the transposition
                    if (noteOnPrep->nTranspUsesTuning.value) // use the Tuning setting
                          offset = t + _tuner->getOffset(round(t) + midiNoteNumber, false);
                    else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                          offset = t + _tuner->getOffset(midiNoteNumber, false);
                     
                    synthOffset = offset;
                     
                    
                    synthNoteNumber += (int)offset;
                    synthOffset     -= (int)offset;
                    
                    /*
                     DBG("undertow note on note/noteNum/offset/duration " +
                        String(thisNote->getNoteNumber()) + " " +
                        String(synthNoteNumber) + " " +
                        String(synthOffset) + " " +
                        String(noteOnPrep->nUndertow.value));
                     */
              
                    
						synth->keyOn(midiChannel,
                                     thisNote->getNoteNumber(),
                                     synthNoteNumber,
                                     synthOffset,
                                     thisNote->getVelocityAtKeyOn(),
                                     aGlobalGain,
                                     Forward,
                                     FixedLengthFixedStart,
                                     NostalgicNote,
                                     prep->getSoundSet(), //set
                                     prep->getId(),
                                     noteOnPrep->nWaveDistance.value,                        //start position
                                     noteOnPrep->nUndertow.value,                            //play length
                                    _prep->nUndertowAttack.value,
                                    _prep->nUndertowDecay.value,
                                    _prep->nUndertowSustain.value,
                                    _prep->nUndertowRelease.value,
                                     tuner,
                                     _prep->getDefaultGainPtr()
                                     );
                }

                undertowNotes.insert(0, new NostalgicNoteStuff(thisNote->getNoteNumber()));
                NostalgicNoteStuff* newNote = undertowNotes.getUnchecked(0);
                newNote->setUndertowTargetLength(thisNote->getUndertowTargetLength());
                newNote->setUndertowStartPosition(noteOnPrep->nWaveDistance.value * synth->getSampleRate()/1000.);
            }
            
            //remove from active notes list
            reverseNotes.remove(i);
        }
        
        if(!thisNote->isActive())
        {
            reverseNotes.remove(i);
        }
    }
    synth->renderNextBlock(buffer, midiMessages, 0, numSamples);
}

//increment timers for all active notes, and all currently reversing notes
void NostalgicProcessor::incrementTimers(int numSamples)
{
    
    for(int i=(activeNotes.size()-1); i >= 0; --i)
    {
        noteLengthTimers.set(activeNotes.getUnchecked(i),
                             noteLengthTimers.getUnchecked(activeNotes.getUnchecked(i)) + numSamples);
    }
    
    for(int i = 0; i<reverseNotes.size(); i++)
    {
        reverseNotes.getUnchecked(i)->incrementReverseTimer(numSamples);
    }
    for(int i = 0; i<undertowNotes.size(); i++)
    {
        undertowNotes.getUnchecked(i)->incrementUndertowTimer(numSamples);
    }
}

Array<int> NostalgicProcessor::getPlayPositions() //return playback positions in ms, not samples
{
    Array<int> newpositions;
    
    for(auto note : reverseNotes)
    {
        if (note->isActive())
            newpositions.add(note->getReversePlayPosition() * 1000./synth->getSampleRate());
    }
    
    return newpositions;
}

Array<int> NostalgicProcessor::getUndertowPositions() //return playback positions in ms, not samples
{
    Array<int> newpositions;

    for(auto note : undertowNotes)
    {
        if (note->isActive())
            newpositions.add(note->getUndertowPlayPosition() * 1000./synth->getSampleRate());
    }
    
    return newpositions;
}

#if BK_UNIT_TESTS


class NostalgicTests : public UnitTest
{
public:
	NostalgicTests() : UnitTest("Nostalgics", "Nostalgic") {}

	void runTest() override
	{
		beginTest("Nostalgic");

		for (int i = 0; i < 10; i++)
		{
			// create nostalgic preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random nostalgic preparation " + String(i);
			DBG("test consistency: " + name);

			NostalgicPreparation::Ptr np1 = new NostalgicPreparation();
			np1->randomize();

			Nostalgic n1(np1, 1);
			n1.setName(name);

			ValueTree vt1 = n1.getState();

			XmlElement* xml = vt1.createXml();

			NostalgicPreparation::Ptr np2 = new NostalgicPreparation();

			Nostalgic n2(np2, 1);

			//dummy parameters for setState
			Tuning::PtrArr t;
			Synchronic::PtrArr s;

			n2.setState(xml, t, s);
			n2.setName(name);

			ValueTree vt2 = n2.getState();

			expect(vt1.isEquivalentTo(vt2), "synchronic preparation value trees don't match");

			//expect(np2->compare(np1), np1->getName() + " and " + np2->getName() + " did not match.");
		}

		//test nostalgic wrapper class
		for (int i = 0; i < 10; i++)
		{
			// create nostalgic and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random nostalgic " + String(i);
			DBG("test consistency: " + name);

			Nostalgic n1(-1, true);
			n1.setName(name);

			ValueTree vt1 = n1.getState();

			XmlElement* xml = vt1.createXml();

			Nostalgic n2(-1, true);

			//dummy parameters for setState
			Tuning::PtrArr t;
			Synchronic::PtrArr s;

			n2.setState(xml, t, s);
			n2.setName(name);

			ValueTree vt2 = n2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"nostalgic: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());
		}

	}
};

static NostalgicTests nostalgicTests;

class NostalgicModTests : public UnitTest
{
public:
	NostalgicModTests() : UnitTest("NostalgicMods", "NostalgicMod") {}

	void runTest() override
	{
		beginTest("NostalgicMod");

		for (int i = 0; i < 10; i++)
		{
			// create nostalgic mod preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random nostalgic mod " + String(i);
			DBG("test consistency: " + name);

			NostalgicPreparation::Ptr np1 = new NostalgicPreparation();
			NostalgicModification::Ptr nm1 = new NostalgicModification(np1, 1);


			nm1->randomize();
			nm1->setName(name);

			ValueTree vt1 = nm1->getState();

			XmlElement* xml = vt1.createXml();

			NostalgicPreparation::Ptr np2 = new NostalgicPreparation();
			NostalgicModification::Ptr nm2 = new NostalgicModification(np2, 1);

			nm2->setState(xml);

			ValueTree vt2 = nm2->getState();

			expect(vt1.isEquivalentTo(vt2),
				"nostalgic mod: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());


			//expect(nm1->compare(nm2), "nostalgic mod: preparations do not match");
		}

	}
};

static NostalgicModTests nostalgicModTests;

#endif
