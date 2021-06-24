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

    if (dirty[NostalgicGain]) nGain.modify(n->nGain, reverse);
    if (dirty[NostalgicBlendronicGain]) nBlendronicGain.modify(n->nBlendronicGain, reverse);
    if (dirty[NostalgicWaveDistance]) nWaveDistance.modify(n->nWaveDistance, reverse);
    if (dirty[NostalgicUndertow]) nUndertow.modify(n->nUndertow, reverse);
    if (dirty[NostalgicTransposition]) nTransposition.modify(n->nTransposition, reverse);
    if (dirty[NostalgicTranspUsesTuning]) nTranspUsesTuning.modify(n->nTranspUsesTuning, reverse);
    if (dirty[NostalgicLengthMultiplier]) nLengthMultiplier.modify(n->nLengthMultiplier, reverse);
    if (dirty[NostalgicBeatsToSkip]) nBeatsToSkip.modify(n->nBeatsToSkip, reverse);
    if (dirty[NostalgicMode]) nMode.modify(n->nMode, reverse);
    if (dirty[NostalgicReverseADSR])
    {
        nReverseAttack.modify(n->nReverseAttack, reverse);
        nReverseDecay.modify(n->nReverseDecay, reverse);
        nReverseSustain.modify(n->nReverseSustain, reverse);
        nReverseRelease.modify(n->nReverseRelease, reverse);
    }
    
    if (dirty[NostalgicUndertowADSR])
    {
        nUndertowAttack.modify(n->nUndertowAttack, reverse);
        nUndertowDecay.modify(n->nUndertowDecay, reverse);
        nUndertowSustain.modify(n->nUndertowSustain, reverse);
        nUndertowRelease.modify(n->nUndertowRelease, reverse);
    }
    
    if (dirty[NostalgicHoldMin]) holdMin.modify(n->holdMin, reverse);
    if (dirty[NostalgicHoldMax]) holdMax.modify(n->holdMax, reverse);
    if (dirty[NostalgicClusterMin]) clusterMin.modify(n->clusterMin, reverse);
    if (dirty[NostalgicClusterThreshold]) clusterThreshold.modify(n->clusterThreshold, reverse);
    if (dirty[NostalgicKeyOnReset]) keyOnReset.modify(n->keyOnReset, reverse);
    if (dirty[NostalgicVelocityMin]) velocityMin.modify(n->velocityMin, reverse);
    if (dirty[NostalgicVelocityMax]) velocityMax.modify(n->velocityMax, reverse);
    
    if (dirty[NostalgicUseGlobalSoundSet]) nUseGlobalSoundSet.modify(n->nUseGlobalSoundSet, reverse);
    
    if (dirty[NostalgicSoundSet])
    {
        nSoundSet.modify(n->nSoundSet, reverse);
        nSoundSetName.modify(n->nSoundSetName, reverse);
    }
    
    // If the mod didn't reverse, then it is modded
    modded = !reverse;
}

NostalgicProcessor::NostalgicProcessor(Nostalgic::Ptr nostalgic,
                                       TuningProcessor::Ptr tuning,
                                       SynchronicProcessor::Ptr synchronic,
										BlendronicProcessor::PtrArr blend,
                                       BKSynthesiser *s):
synth(s),
nostalgic(nostalgic),
tuner(tuning),
synchronic(synchronic),
blendronic(blend),
keymaps(Keymap::PtrArr())
{
    if (!nostalgic->prep->nUseGlobalSoundSet.value)
    {
        // comes in as "soundfont.sf2.subsound1"
        String name = nostalgic->prep->nSoundSetName.value;
        BKSampleLoadType type;
        String path;
        int subsound = 0;
        
        for (int i = 0; i < cBKSampleLoadTypes.size(); i++)
        {
            if (name == String(cBKSampleLoadTypes[i]))
            {
                type = (BKSampleLoadType) i;
            }
        }
        
        String sfName = name.upToLastOccurrenceOf(".subsound", false, false);
        for (auto sf : synth->processor.soundfontNames)
        {
            if (sf.contains(sfName))
            {
                type = BKLoadSoundfont;
                path = sf;
                subsound = name.fromLastOccurrenceOf(".subsound", false, false).getIntValue();
                break;
            }
        }
        
        
        for (auto cs : synth->processor.customSampleSetNames)
        {
            if (cs.fromLastOccurrenceOf(File::getSeparatorString(), false, false) == name)
            {
                type = BKLoadCustom;
                path = cs;
                break;
            }
        }
        
        int Id = synth->loadSamples(type, path, subsound, false);
        nostalgic->prep->setSoundSet(Id);
    }
    
    for (int j = 0; j < 128; j++)
    {
        pressVelocities.add(new Array<float>());
        releaseVelocities.add(new Array<float>());
        for (int i = 0; i < TargetTypeBlendronicPatternSync-TargetTypeNostalgic; ++i)
        {
            pressVelocities.getLast()->add(0.f);
            releaseVelocities.getLast()->add(0.f);
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
void NostalgicProcessor::keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress, bool post)
{
    NostalgicPreparation::Ptr prep = nostalgic->prep;
    
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    OwnedArray<Array<float>>* cVels;
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = releaseVelocities.getUnchecked(noteNumber);
        for (int i = 0; i < releaseVelocities.getUnchecked(noteNumber)->size(); ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i+TargetTypeNostalgic));
        }
        cVels = &releaseVelocities;
    }
    // If this an actual release, aVels will be the incoming velocities,
    // but bVels will use the values from the last press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = pressVelocities.getUnchecked(noteNumber);
        cVels = &pressVelocities;
    }
    
    int s = TargetTypeNostalgic;
    bool doNostalgic = bVels->getUnchecked(TargetTypeNostalgic-s) >= 0.f; // primary Nostalgic mode
    bool doClear = bVels->getUnchecked(TargetTypeNostalgicClear-s) >= 0.f; // clear all sounding notes
    
    if (doClear &&
        (prep->targetTypeNostalgicClear.value == NoteOff || prep->targetTypeNostalgicClear.value == Both))
    {
        clearAll(1);
    }
    
    if (!doNostalgic) return;
    
    float duration = 0.0;
    
    if (post || noteOn[noteNumber])
    {
        
        int offRamp;
        if (prep->nUndertow.value > 0) offRamp = aRampUndertowCrossMS;
        else offRamp = aRampNostalgicOffMS;

        SynchronicSyncMode syncTargetMode = synchronic->getSynchronic()->prep->sMode.value;
        
        if (prep->nMode.value == SynchronicSync2)
        {
            float duration = 0.0;
            
            int offRamp;
            if (prep->nUndertow.value > 0) offRamp = aRampUndertowCrossMS;
            else offRamp = aRampNostalgicOffMS;
            
            duration = synchronic->getTimeToBeatMS(prep->nBeatsToSkip.value) + offRamp + 30; // sum
            
            for (auto t : prep->nTransposition.value)
            {
                
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                 */
                
                int synthNoteNumber = noteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                
                // tune the transposition
                if (prep->nTranspUsesTuning.value) // use the Tuning setting
                    offset = t + tuner->getOffset(round(t) + noteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                    offset = t + tuner->getOffset(noteNumber, false);
               
                synthOffset = offset;

                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note

                DBG("reverse note on noteNum/offset " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset));
                if (!blendronic.isEmpty())
                {
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(0),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 prep->nReverseAttack.value,
                                 prep->nReverseDecay.value,
                                 prep->nReverseSustain.value,
                                 prep->nReverseRelease.value,
                                 tuner,
                                 prep->getGainPtr(),
                                 prep->getBlendronicGainPtr(),
                                 blendronic);
				}
				else
				{
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(0),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 prep->nReverseAttack.value,
                                 prep->nReverseDecay.value,
                                 prep->nReverseSustain.value,
                                 prep->nReverseRelease.value,
                                 tuner,
                                 prep->getGainPtr());
                }
            }
            
            activeNotes.removeFirstMatchingValue(noteNumber);
            noteOn.set(noteNumber, false);
            noteLengthTimers.set(noteNumber, 0);
            
            reverseNotes.insert(0, new NostalgicNoteStuff(noteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(prep);
            currentNote->setTuningAtKeyOn(tuner->getOffset(noteNumber, false));
            currentNote->setVelocityAtKeyOn(aVels->getUnchecked(0));
            currentNote->setReverseStartPosition((duration + prep->nWaveDistance.value) * synth->getSampleRate()/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(prep->nUndertow.value * synth->getSampleRate()/1000.);
            
        }
        else if (prep->nMode.value == NoteLengthSync)
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
                if (currentClusterSize >= prep->clusterMin.value)
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
                    
                    if (prep->holdMin.value <= prep->holdMax.value)
                    {
                        if (held >= prep->holdMin.value && held <= prep->holdMax.value)
                        {
                            passHoldTest = true;
                        }
                    }
                    else
                    {
                        if (held >= prep->holdMin.value || held <= prep->holdMax.value)
                        {
                            passHoldTest = true;
                        }
                    }
                    
                    bool playNote = passHoldTest;
            
                    if (!playNote) continue;

                    //clusterNotesPlayed.addIfNotAlreadyThere(note);
                    
                    //get length of played notes, subtract wave distance to set nostalgic reverse note length
                    duration =  (noteLengthTimers.getUnchecked(note) *
                                 prep->nLengthMultiplier.value +
                                 (offRamp + 30)) * (1000.0 / synth->getSampleRate());
                    
                    for (auto t : prep->nTransposition.value)
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
                        if (prep->nTranspUsesTuning.value) // use the Tuning setting
                             // offset = t + tuner->getOffset(round(t)  + midiNoteNumber, false);
                            offset = t + tuner->getOffset(round(t)  + note, false);
                        else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                             // offset = t + tuner->getOffset(midiNoteNumber, false);
                            offset = t + tuner->getOffset(note, false);
                        
                        synthOffset = offset;
                        
                        synthNoteNumber += (int)offset;
                        synthOffset     -= (int)offset;
                        
                        DBG("Nostalgic::keyReleased duration, waveDistance : " + String(duration) + " " + String(prep->nWaveDistance.value));
                        
                        //play nostalgic note
                        if (!blendronic.isEmpty())
                        {
                            synth->keyOn(1,
                                         note,
                                         synthNoteNumber,
                                         synthOffset,
                                         cVels->getUnchecked(note)->getUnchecked(0),
                                         aGlobalGain,
                                         Reverse,
                                         FixedLengthFixedStart,
                                         NostalgicNote,
                                         prep->getSoundSet(), //set
                                         nostalgic->getId(),
                                         duration + prep->nWaveDistance.value,
                                         duration,  // length
                                         // duration + prep->nReverseRelease.value,
                                         prep->nReverseAttack.value,
                                         prep->nReverseDecay.value,
                                         prep->nReverseSustain.value,
                                         prep->nReverseRelease.value,
                                         tuner,
                                         prep->getGainPtr());
						}
						else
						{
                            synth->keyOn(1,
                                         note,
                                         synthNoteNumber,
                                         synthOffset,
                                         cVels->getUnchecked(note)->getUnchecked(0),
                                         aGlobalGain,
                                         Reverse,
                                         FixedLengthFixedStart,
                                         NostalgicNote,
                                         prep->getSoundSet(), //set
                                         nostalgic->getId(),
                                         duration + prep->nWaveDistance.value,
                                         duration,  // length
                                         // duration + prep->nReverseRelease.value,
                                         prep->nReverseAttack.value,
                                         prep->nReverseDecay.value,
                                         prep->nReverseSustain.value,
                                         prep->nReverseRelease.value,
                                         tuner,
                                         prep->getGainPtr());
						}
                        
                        reverseNotes.insert(0, new NostalgicNoteStuff(note));
                        NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
                        currentNote->setPrepAtKeyOn(prep);
                        currentNote->setTuningAtKeyOn(tuner->getOffset(note, false));
                        currentNote->setVelocityAtKeyOn(cVels->getUnchecked(note)->getUnchecked(0));
                        currentNote->setReverseStartPosition((duration + prep->nWaveDistance.value) * synth->getSampleRate()/1000.);
                        currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
                        currentNote->setUndertowTargetLength(prep->nUndertow.value * synth->getSampleRate()/1000.);
                        
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
            duration = synchronic->getTimeToBeatMS(prep->nBeatsToSkip.value) + offRamp + 30; // sum
            
            for (auto t : prep->nTransposition.value)
            {
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                int synthNoteNumber = midiNoteNumber;
                 */
                
                int synthNoteNumber = noteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                 
                 // tune the transposition
                if (prep->nTranspUsesTuning.value) // use the Tuning setting
                     offset = t + tuner->getOffset(round(t) + noteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                     offset = t + tuner->getOffset(noteNumber, false);
                
                synthOffset = offset;
                
                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note
                DBG("reverse note on noteNum/offset " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset));
                
                if (!blendronic.isEmpty())
                {
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(0),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 prep->nReverseAttack.value,
                                 prep->nReverseDecay.value,
                                 prep->nReverseSustain.value,
                                 prep->nReverseRelease.value,
                                 tuner,
                                 prep->getGainPtr());
				}
				else
				{
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(0),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 prep->nReverseAttack.value,
                                 prep->nReverseDecay.value,
                                 prep->nReverseSustain.value,
                                 prep->nReverseRelease.value,
                                 tuner,
                                 prep->getGainPtr());
                    
				}
            }
            
            reverseNotes.insert(0, new NostalgicNoteStuff(noteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(prep);
            currentNote->setTuningAtKeyOn(tuner->getOffset(noteNumber, false));
            currentNote->setVelocityAtKeyOn(aVels->getUnchecked(0));// * prep->getGain());
            currentNote->setReverseStartPosition((duration + prep->nWaveDistance.value) * synth->getSampleRate()/1000.);
            //currentNote->setReverseTargetLength((duration - aRampUndertowCrossMS) * sampleRate/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(prep->nUndertow.value * synth->getSampleRate()/1000.);
        }
    }

}

//start timer for length of a particular note; called when key is pressed
void NostalgicProcessor::keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    NostalgicPreparation::Ptr prep = nostalgic->prep;
    
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an actual key press (not an inverted release) aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = pressVelocities.getUnchecked(noteNumber);
        for (int i = 0; i < pressVelocities.getUnchecked(noteNumber)->size(); ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i+TargetTypeNostalgic));
        }
    }
    // If this an inverted release, aVels will be the incoming velocities,
    // but bVels will use the values from the last inverted press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = releaseVelocities.getUnchecked(noteNumber);
    }
    
    int s = TargetTypeNostalgic;
    bool doNostalgic = bVels->getUnchecked(TargetTypeNostalgic-s) >= 0.f; // primary Nostalgic mode
    bool doClear = bVels->getUnchecked(TargetTypeNostalgicClear-s) >= 0.f; // clear all sounding notes
    
    noteOn.set(noteNumber, true);
    
    DBG("note = " + String(noteNumber) + " channel = " + " doClear = " + String((int)doClear));
    
    if (doClear &&
        (prep->targetTypeNostalgicClear.value == NoteOn || prep->targetTypeNostalgicClear.value == Both))
    {
        clearAll(1);
    }
    
    if (!doNostalgic) return;
    
    if (prep->nMode.value == SynchronicSync)
    {
        float duration = 0.0;
        
        int offRamp;
        if (prep->nUndertow.value > 0) offRamp = aRampUndertowCrossMS;
        else offRamp = aRampNostalgicOffMS;
        
        //get time in ms to target beat, summing over skipped beat lengths
        SynchronicSyncMode syncTargetMode = synchronic->getMode();
        
        if(syncTargetMode == FirstNoteOnSync || syncTargetMode == AnyNoteOnSync)
        {
            duration = synchronic->getTimeToBeatMS(prep->nBeatsToSkip.value) + offRamp + 30; // sum
            
            for (auto t : prep->nTransposition.value)
            {
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                int synthNoteNumber = midiNoteNumber;
                 */
                
                int synthNoteNumber = noteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                 
                // tune the transposition
                if (prep->nTranspUsesTuning.value) // use the Tuning setting
                     offset = t + tuner->getOffset(round(t) + noteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                     offset = t + tuner->getOffset(noteNumber, false);
                
                synthOffset = offset;
                
                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note
                DBG("reverse note on noteNum/offset " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset));
                
				if (!blendronic.isEmpty())
				{
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(0),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 prep->nReverseAttack.value,
                                 prep->nReverseDecay.value,
                                 prep->nReverseSustain.value,
                                 prep->nReverseRelease.value,
                                 tuner,
                                 prep->getGainPtr(),
                                 prep->getBlendronicGainPtr(),
                                 blendronic);
                }
				else
				{
                    synth->keyOn(1,
                                 noteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 aVels->getUnchecked(0),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->nWaveDistance.value,
                                 duration,  // length
                                 // duration + prep->nReverseRelease.value,
                                 prep->nReverseAttack.value,
                                 prep->nReverseDecay.value,
                                 prep->nReverseSustain.value,
                                 prep->nReverseRelease.value,
                                 tuner,
                                 prep->getGainPtr());
				}
            }
            
            reverseNotes.insert(0, new NostalgicNoteStuff(noteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(prep);
            currentNote->setTuningAtKeyOn(tuner->getOffset(noteNumber, false));
            currentNote->setVelocityAtKeyOn(aVels->getUnchecked(0));
            currentNote->setReverseStartPosition((duration + prep->nWaveDistance.value) * synth->getSampleRate()/1000.);
            //currentNote->setReverseTargetLength((duration - aRampUndertowCrossMS) * sampleRate/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(prep->nUndertow.value * synth->getSampleRate()/1000.);
        }
    }
    
    //activeNotes is for measuring lengths of held notes, so only relevant in NoteLengthSync mode
    if (prep->nMode.value == NoteLengthSync)
    {
        activeNotes.addIfNotAlreadyThere(noteNumber);
        noteLengthTimers.set(noteNumber, 0);
        lastKeyPlayed = noteNumber;
        
        if(!inCluster)
        {
            currentClusterSize = 0;
        }
        
        // KEY ON RESET STUFF
        if (prep->keyOnReset.value)
        {
            // REMOVE REVERSE NOTES
            for (int i = reverseNotes.size(); --i >= 0;)
            {
                NostalgicNoteStuff* note = reverseNotes[i];
                
                if ((note->getNoteNumber() == noteNumber) && note->isActive())
                {
                    if (prep != nullptr)
                    {
                        for (auto transp : prep->nTransposition.value)
                        {
                            DBG("reverse remove: " + String(noteNumber + transp));
                            synth->keyOff (1,
                                           NostalgicNote,
                                           prep->getSoundSet(),
                                           nostalgic->getId(),
                                           noteNumber,
                                           noteNumber+transp,
                                           64,
                                           aGlobalGain,
                                           prep->getGainPtr(),
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
                        for (auto transp : prep->nTransposition.value)
                        {
                            DBG("undertow remove: " + String(noteNumber + transp));
                            synth->keyOff (1,
                                           NostalgicNote,
                                           prep->getSoundSet(),
                                           nostalgic->getId(),
                                           noteNumber,
                                           noteNumber+transp,
                                           64,
                                           aGlobalGain,
                                           prep->getGainPtr(),
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
    NostalgicPreparation::Ptr prep = nostalgic->prep;
    
    // REMOVE REVERSE NOTES
    for (int i = reverseNotes.size(); --i >= 0;)
    {
        NostalgicNoteStuff* note = reverseNotes[i];
        
        if (note->isActive())
        {
            if (prep != nullptr)
            {
                for (auto transp : prep->nTransposition.value)
                {
                    DBG("reverse remove: " + String(note->getNoteNumber() + transp));
                    synth->keyOff (midiChannel,
                                   NostalgicNote,
                                   prep->getSoundSet(),
                                   nostalgic->getId(),
                                   note->getNoteNumber(),
                                   note->getNoteNumber()+transp,
                                   64,
                                   aGlobalGain,
                                   prep->getGainPtr(),
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
            for (auto transp : prep->nTransposition.value)
            {
                DBG("undertow remove: " + String(note->getNoteNumber() + transp));
                synth->keyOff (midiChannel,
                               NostalgicNote,
                               prep->getSoundSet(),
                               nostalgic->getId(),
                               note->getNoteNumber(),
                               note->getNoteNumber() + transp,
                               64,
                               aGlobalGain,
                               prep->getGainPtr(),
                               false, // THIS NEEDS TO BE FALSE ATM TO GET RID OF UNDERTOW NOTES : because our processsPiano doesn't distinguish between true keyOn/Offs from keyboard and these under the hood keyOn/Offs (which don't correspond with Off/On from keyboard)
                               true); // true for nostalgicOff
            }
        }
        undertowNotes.remove(i);
    }
}

float NostalgicProcessor::filterVelocity(float vel)
{
    NostalgicPreparation::Ptr prep = nostalgic->prep;
    
    if (!lastVelocityInRange) lastVelocity = vel;
    
    int velocity = vel*127.f;
    
    if(prep->velocityMin.value <= prep->velocityMax.value)
    {
        if (velocity >= prep->velocityMin.value && velocity <= prep->velocityMax.value)
        {
            lastVelocityInRange = true;
            lastVelocity = vel;
            return vel;
        }
    }
    else
    {
        if (velocity >= prep->velocityMin.value || velocity <= prep->velocityMax.value)
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
void NostalgicProcessor::processBlock(int numSamples, int midiChannel, BKSampleLoadType type)
{
    //cluster management
    if (inCluster)
    {
        NostalgicPreparation::Ptr prep = nostalgic->prep;
        //moved beyond clusterThreshold time, done with cluster
        //if (clusterThresholdTimer >= (sampleRate * 0.15)) //eventually expose this threshold
        if (clusterThresholdTimer >= (synth->getSampleRate() * .001 * prep->clusterThreshold.value))
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
            NostalgicPreparation::Ptr noteOnPrep = thisNote->getPrepAtKeyOn();
            
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
                          offset = t + tuner->getOffset(round(t) + midiNoteNumber, false);
                    else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                          offset = t + tuner->getOffset(midiNoteNumber, false);
                     
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
                    NostalgicPreparation::Ptr prep = nostalgic->prep;
                    
					if (!blendronic.isEmpty())
					{
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
                                     nostalgic->getId(),
                                     noteOnPrep->nWaveDistance.value,                        //start position
                                     noteOnPrep->nUndertow.value,                            //play length
                                     prep->nUndertowAttack.value,
                                     prep->nUndertowDecay.value,
                                     prep->nUndertowSustain.value,
                                     prep->nUndertowRelease.value,
                                     tuner,
                                     prep->getGainPtr(),
                                     prep->getBlendronicGainPtr(),
                                     blendronic);
					}
					else
					{
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
                                     nostalgic->getId(),
                                     noteOnPrep->nWaveDistance.value,                        //start position
                                     noteOnPrep->nUndertow.value,                            //play length
                                     prep->nUndertowAttack.value,
                                     prep->nUndertowDecay.value,
                                     prep->nUndertowSustain.value,
                                     prep->nUndertowRelease.value,
                                     tuner,
                                     prep->getGainPtr(),
                                     prep->getBlendronicGainPtr(),
                                     blendronic);
					}
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
