/*
  ==============================================================================

    Nostalgic.cpp
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Nostalgic.h"
#include "PluginProcessor.h"

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
    if (!nostalgic->prep->getUseGlobalSoundSet())
    {
        // comes in as "soundfont.sf2.subsound1"
        String name = nostalgic->prep->getSoundSetName();
        BKSampleLoadType type = BKLoadSoundfont;
        
        for (int i = 0; i < cBKSampleLoadTypes.size(); i++)
        {
            if (name == String(cBKSampleLoadTypes[i]))
            {
                type = (BKSampleLoadType) i;
            }
        }
        
        String path = name.upToLastOccurrenceOf(".subsound", false, false);
        int subsound = 0;
        
        if (type == BKLoadSoundfont)
        {
            for (auto sf : synth->processor.soundfontNames)
            {
                if (sf.contains(path))
                {
                    path = sf;
                    break;
                }
            }

            subsound = name.fromLastOccurrenceOf(".subsound", false, false).getIntValue();
        }
        int Id = synth->loadSamples(type, path, subsound, false);
        nostalgic->prep->setSoundSet(Id);
    }
    
    noteLengthTimers.ensureStorageAllocated(128);
    velocities.ensureStorageAllocated(128);
    noteOn.ensureStorageAllocated(128);
    
    for (int i = 0; i < 128; i++)
    {
        noteLengthTimers.insert(i, 0); //initialize timers for all notes
        velocities.insert(i, 0); //store noteOn velocities to set Nostalgic velocities
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
void NostalgicProcessor::keyReleased(int midiNoteNumber, float midiVelocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    NostalgicPreparation::Ptr prep = nostalgic->prep;
    
    bool doNostalgic = targetStates[TargetTypeNostalgic] == TargetStateEnabled; // primary Nostalgic mode
    bool doClear = targetStates[TargetTypeNostalgicClear] == TargetStateEnabled; // only if clearing all sounding notes
    
    if (doClear &&
        (prep->getTargetTypeNostalgicClear() == NoteOff || prep->getTargetTypeNostalgicClear() == Both))
    {
        clearAll(midiChannel);
    }
    
    if (!doNostalgic) return;
    
    float duration = 0.0;
    
    if (post || noteOn[midiNoteNumber])
    {
        
        int offRamp;
        if (prep->getUndertow() > 0) offRamp = aRampUndertowCrossMS;
        else offRamp = aRampNostalgicOffMS;

        SynchronicSyncMode syncTargetMode = synchronic->getSynchronic()->prep->getMode();
        
        if (prep->getMode() == SynchronicSync2)
        {
            float duration = 0.0;
            
            int offRamp;
            if (prep->getUndertow() > 0) offRamp = aRampUndertowCrossMS;
            else offRamp = aRampNostalgicOffMS;
            
            duration = synchronic->getTimeToBeatMS(prep->getBeatsToSkip()) + offRamp + 30; // sum
            
            for (auto t : prep->getTransposition())
            {
                
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                 */
                
                int synthNoteNumber = midiNoteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                
                // tune the transposition
                if (prep->getTranspUsesTuning()) // use the Tuning setting
                    offset = t + tuner->getOffset(round(t) + midiNoteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                    offset = t + tuner->getOffset(midiNoteNumber, false);
               
                synthOffset = offset;

                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note

                DBG("reverse note on noteNum/offset " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset));
                if (!blendronic.isEmpty())
                {
                    synth->keyOn(
                                 midiChannel,
                                 midiNoteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 velocities.getUnchecked(midiNoteNumber),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->getWavedistance(),
                                 duration,  // length
                                 // duration + prep->getReverseRelease(),
                                 prep->getReverseAttack(),
                                 prep->getReverseDecay(),
                                 prep->getReverseSustain(),
                                 prep->getReverseRelease(),
                                 tuner,
                                 prep->getGainPtr(),
                                 prep->getBlendronicGainPtr(),
                                 blendronic);
				}
				else
				{
					synth->keyOn(midiChannel,
						midiNoteNumber,
						synthNoteNumber,
						synthOffset,
						velocities.getUnchecked(midiNoteNumber),
						aGlobalGain,
						Reverse,
						FixedLengthFixedStart,
						NostalgicNote,
                        prep->getSoundSet(), //set
						nostalgic->getId(),
						duration + prep->getWavedistance(),
						duration,  // length
                        // duration + prep->getReverseRelease(),
						prep->getReverseAttack(),
						prep->getReverseDecay(),
						prep->getReverseSustain(),
						prep->getReverseRelease(),
                        
						tuner,
                                 prep->getGainPtr());
				}
            }
            
            activeNotes.removeFirstMatchingValue(midiNoteNumber);
            noteOn.set(midiNoteNumber, false);
            noteLengthTimers.set(midiNoteNumber, 0);
            
            reverseNotes.insert(0, new NostalgicNoteStuff(midiNoteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(prep);
            currentNote->setTuningAtKeyOn(tuner->getOffset(midiNoteNumber, false));
            currentNote->setVelocityAtKeyOn(velocities.getUnchecked(midiNoteNumber));
            currentNote->setReverseStartPosition((duration + prep->getWavedistance()) * synth->getSampleRate()/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(prep->getUndertow() * synth->getSampleRate()/1000.);
            
        }
        else if (prep->getMode() == NoteLengthSync)
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
            cluster.add(midiNoteNumber);
            
            currentClusterSize++;
            
            //reset the timer for time between notes
            clusterThresholdTimer = 0;
            
            playCluster = false;
            
            //if (inCluster) //inCluster will ALWAYS be true here
            {
                //if (cluster.size() >= prep->getClusterMin())
                if (currentClusterSize >= prep->getClusterMin())
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
                    bool passHoldTest = false, passVelocityTest = false;
                    
                    float held = noteLengthTimers.getUnchecked(note) * (1000.0 / synth->getSampleRate());
                    int velocity = (int) (velocities.getUnchecked(note) * 127);
                    
                    if (prep->getHoldMin() <= prep->getHoldMax())
                    {
                        if (held >= prep->getHoldMin() && held <= prep->getHoldMax())
                        {
                            passHoldTest = true;
                        }
                    }
                    else
                    {
                        if (held >= prep->getHoldMin() || held <= prep->getHoldMax())
                        {
                            passHoldTest = true;
                        }
                    }
                    
                    if (prep->getVelocityMin() <= prep->getVelocityMax())
                    {
                        if (velocity >= prep->getVelocityMin() && velocity <= prep->getVelocityMax())
                        {
                            passVelocityTest = true;
                        }
                    }
                    else
                    {
                        if (velocity >= prep->getVelocityMin() || velocity <= prep->getVelocityMax())
                        {
                            passVelocityTest = true;
                        }
                    }
                    
                    bool playNote = (passHoldTest && passVelocityTest);
                    

                    if (!playNote) continue;

                    //clusterNotesPlayed.addIfNotAlreadyThere(note);
                    
                    //get length of played notes, subtract wave distance to set nostalgic reverse note length
                    duration =  (noteLengthTimers.getUnchecked(note) *
                                 prep->getLengthMultiplier() +
                                 (offRamp + 30)) * (1000.0 / synth->getSampleRate());
                    
                    for (auto t : prep->getTransposition())
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
                        if (prep->getTranspUsesTuning()) // use the Tuning setting
                             // offset = t + tuner->getOffset(round(t)  + midiNoteNumber, false);
                            offset = t + tuner->getOffset(round(t)  + note, false);
                        else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                             // offset = t + tuner->getOffset(midiNoteNumber, false);
                            offset = t + tuner->getOffset(note, false);
                        
                        synthOffset = offset;
                        
                        synthNoteNumber += (int)offset;
                        synthOffset     -= (int)offset;
                        
                        DBG("Nostalgic::keyReleased duration, waveDistance : " + String(duration) + " " + String(prep->getWavedistance()));
                        
                        //play nostalgic note
                        if (!blendronic.isEmpty())
                        {
                            synth->keyOn(midiChannel,
                                         note,
                                         synthNoteNumber,
                                         synthOffset,
                                         velocities.getUnchecked(note),
                                         aGlobalGain,
                                         Reverse,
                                         FixedLengthFixedStart,
                                         NostalgicNote,
                                         prep->getSoundSet(), //set
                                         nostalgic->getId(),
                                         duration + prep->getWavedistance(),
                                         duration,  // length
                                         // duration + prep->getReverseRelease(),
                                         prep->getReverseAttack(),
                                         prep->getReverseDecay(),
                                         prep->getReverseSustain(),
                                         prep->getReverseRelease(),
                                         tuner,
                                         prep->getGainPtr());
						}
						else
						{
                            synth->keyOn(midiChannel,
                                         note,
                                         synthNoteNumber,
                                         synthOffset,
                                         velocities.getUnchecked(note),
                                         aGlobalGain,
                                         Reverse,
                                         FixedLengthFixedStart,
                                         NostalgicNote,
                                         prep->getSoundSet(), //set
                                         nostalgic->getId(),
                                         duration + prep->getWavedistance(),
                                         duration,  // length
                                         // duration + prep->getReverseRelease(),
                                         prep->getReverseAttack(),
                                         prep->getReverseDecay(),
                                         prep->getReverseSustain(),
                                         prep->getReverseRelease(),
                                         tuner,
                                         prep->getGainPtr());
						}
                        
                        reverseNotes.insert(0, new NostalgicNoteStuff(note));
                        NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
                        currentNote->setPrepAtKeyOn(prep);
                        currentNote->setTuningAtKeyOn(tuner->getOffset(note, false));
                        currentNote->setVelocityAtKeyOn(velocities.getUnchecked(note));
                        currentNote->setReverseStartPosition((duration + prep->getWavedistance()) * synth->getSampleRate()/1000.);
                        currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
                        currentNote->setUndertowTargetLength(prep->getUndertow() * synth->getSampleRate()/1000.);
                        
                        noteLengthTimers.set(note, 0);
                    }
                }
                cluster.clearQuick();
            }
            
            
            // turn note length timers off
            activeNotes.removeFirstMatchingValue(midiNoteNumber);
            noteOn.set(midiNoteNumber, false);
            //DBG("nostalgic removed active note " + String(midiNoteNumber));
            
        }
        else if(syncTargetMode == LastNoteOffSync || syncTargetMode == AnyNoteOffSync)
        {
            duration = synchronic->getTimeToBeatMS(prep->getBeatsToSkip()) + offRamp + 30; // sum
            
            for (auto t : prep->getTransposition())
            {
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                int synthNoteNumber = midiNoteNumber;
                 */
                
                int synthNoteNumber = midiNoteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                 
                 // tune the transposition
                if (prep->getTranspUsesTuning()) // use the Tuning setting
                     offset = t + tuner->getOffset(round(t) + midiNoteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                     offset = t + tuner->getOffset(midiNoteNumber, false);
                
                synthOffset = offset;
                
                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note
                DBG("reverse note on noteNum/offset " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset));
                
                if (!blendronic.isEmpty())
                {
                    synth->keyOn(midiChannel,
                                 midiNoteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 velocities.getUnchecked(midiNoteNumber),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->getWavedistance(),
                                 duration,  // length
                                 // duration + prep->getReverseRelease(),
                                 prep->getReverseAttack(),
                                 prep->getReverseDecay(),
                                 prep->getReverseSustain(),
                                 prep->getReverseRelease(),
                                 tuner,
                                 prep->getGainPtr());
				}
				else
				{
                    synth->keyOn(midiChannel,
                                 midiNoteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 velocities.getUnchecked(midiNoteNumber),
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->getWavedistance(),
                                 duration,  // length
                                 // duration + prep->getReverseRelease(),
                                 prep->getReverseAttack(),
                                 prep->getReverseDecay(),
                                 prep->getReverseSustain(),
                                 prep->getReverseRelease(),
                                 tuner,
                                 prep->getGainPtr());
                    
				}
            }
            
            reverseNotes.insert(0, new NostalgicNoteStuff(midiNoteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(prep);
            currentNote->setTuningAtKeyOn(tuner->getOffset(midiNoteNumber, false));
            currentNote->setVelocityAtKeyOn(velocities.getUnchecked(midiNoteNumber));// * prep->getGain());
            currentNote->setReverseStartPosition((duration + prep->getWavedistance()) * synth->getSampleRate()/1000.);
            //currentNote->setReverseTargetLength((duration - aRampUndertowCrossMS) * sampleRate/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(prep->getUndertow() * synth->getSampleRate()/1000.);
        }
    }

}

//start timer for length of a particular note; called when key is pressed
void NostalgicProcessor::keyPressed(int midiNoteNumber, float midiNoteVelocity, int midiChannel, Array<KeymapTargetState> targetStates)
{
    NostalgicPreparation::Ptr prep = nostalgic->prep;
    
    bool doNostalgic = targetStates[TargetTypeNostalgic] == TargetStateEnabled; // primary Nostalgic mode
    bool doClear = targetStates[TargetTypeNostalgicClear] == TargetStateEnabled; // only if clearing all sounding notes
    
    DBG("note = " + String(midiNoteNumber) + " channel = " + String(midiChannel) + " doClear = " + String((int)doClear));
    
    if (doClear &&
        (prep->getTargetTypeNostalgicClear() == NoteOn || prep->getTargetTypeNostalgicClear() == Both))
    {
        clearAll(midiChannel);
    }
    
    if (!doNostalgic) return;
    
    lastVelocity = midiNoteVelocity;
    
    if (prep->getMode() == SynchronicSync)
    {
        float duration = 0.0;
        
        int offRamp;
        if (prep->getUndertow() > 0) offRamp = aRampUndertowCrossMS;
        else offRamp = aRampNostalgicOffMS;
        
        //get time in ms to target beat, summing over skipped beat lengths
        SynchronicSyncMode syncTargetMode = synchronic->getMode();
        
        if(syncTargetMode == FirstNoteOnSync || syncTargetMode == AnyNoteOnSync)
        {
            duration = synchronic->getTimeToBeatMS(prep->getBeatsToSkip()) + offRamp + 30; // sum
            
            for (auto t : prep->getTransposition())
            {
                /*
                float offset = t + tuner->getOffset(midiNoteNumber, false), synthOffset = offset;
                int synthNoteNumber = midiNoteNumber;
                 */
                
                int synthNoteNumber = midiNoteNumber;
                float offset; // offset from integer, but may be greater than 1
                float synthOffset; // offset from actual sample played, always less than 1.
                 
                // tune the transposition
                if (prep->getTranspUsesTuning()) // use the Tuning setting
                     offset = t + tuner->getOffset(round(t) + midiNoteNumber, false);
                else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
                     offset = t + tuner->getOffset(midiNoteNumber, false);
                
                synthOffset = offset;
                
                synthNoteNumber += (int)offset;
                synthOffset     -= (int)offset;
                
                //play nostalgic note
                DBG("reverse note on noteNum/offset " +
                    String(synthNoteNumber) + " " +
                    String(synthOffset));
                
				if (!blendronic.isEmpty())
				{
                    synth->keyOn(midiChannel,
                                 midiNoteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 midiNoteVelocity,
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->getWavedistance(),
                                 duration,  // length
                                 // duration + prep->getReverseRelease(),
                                 prep->getReverseAttack(),
                                 prep->getReverseDecay(),
                                 prep->getReverseSustain(),
                                 prep->getReverseRelease(),
                                 tuner,
                                 prep->getGainPtr(),
                                 prep->getBlendronicGainPtr(),
                                 blendronic);
                }
				else
				{
                    synth->keyOn(midiChannel,
                                 midiNoteNumber,
                                 synthNoteNumber,
                                 synthOffset,
                                 midiNoteVelocity,
                                 aGlobalGain,
                                 Reverse,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 prep->getSoundSet(), //set
                                 nostalgic->getId(),
                                 duration + prep->getWavedistance(),
                                 duration,  // length
                                 // duration + prep->getReverseRelease(),
                                 prep->getReverseAttack(),
                                 prep->getReverseDecay(),
                                 prep->getReverseSustain(),
                                 prep->getReverseRelease(),
                                 tuner,
                                 prep->getGainPtr());
				}
            }
            
            reverseNotes.insert(0, new NostalgicNoteStuff(midiNoteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(prep);
            currentNote->setTuningAtKeyOn(tuner->getOffset(midiNoteNumber, false));
            currentNote->setVelocityAtKeyOn(midiNoteVelocity);
            currentNote->setReverseStartPosition((duration + prep->getWavedistance()) * synth->getSampleRate()/1000.);
            //currentNote->setReverseTargetLength((duration - aRampUndertowCrossMS) * sampleRate/1000.);
            currentNote->setReverseTargetLength((duration) * synth->getSampleRate()/1000.);
            currentNote->setUndertowTargetLength(prep->getUndertow() * synth->getSampleRate()/1000.);
        }
    }
    
    //activeNotes is for measuring lengths of held notes, so only relevant in NoteLengthSync mode
    if (prep->getMode() == NoteLengthSync)
    {
        activeNotes.addIfNotAlreadyThere(midiNoteNumber);
        noteLengthTimers.set(midiNoteNumber, 0);
        lastKeyPlayed = midiNoteNumber;
        
        if(!inCluster)
        {
            currentClusterSize = 0;
        }
        
        // KEY ON RESET STUFF
        if (prep->getKeyOnReset())
        {
            // REMOVE REVERSE NOTES
            for (int i = reverseNotes.size(); --i >= 0;)
            {
                NostalgicNoteStuff* note = reverseNotes[i];
                
                if ((note->getNoteNumber() == midiNoteNumber) && note->isActive())
                {
                    if (prep != nullptr)
                    {
                        for (auto transp : prep->getTransposition())
                        {
                            DBG("reverse remove: " + String(midiNoteNumber + transp));
                            synth->keyOff (midiChannel,
                                           NostalgicNote,
                                           prep->getSoundSet(),
                                           nostalgic->getId(),
                                           midiNoteNumber,
                                           midiNoteNumber+transp,
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
                
                if ((note->getNoteNumber() == midiNoteNumber))
                {
                    if (prep != nullptr)
                    {
                        for (auto transp : prep->getTransposition())
                        {
                            DBG("undertow remove: " + String(midiNoteNumber + transp));
                            synth->keyOff (midiChannel,
                                           NostalgicNote,
                                           prep->getSoundSet(),
                                           nostalgic->getId(),
                                           midiNoteNumber,
                                           midiNoteNumber+transp,
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
    
    noteOn.set(midiNoteNumber, true);
    velocities.set(midiNoteNumber, midiNoteVelocity);
    
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
                for (auto transp : prep->getTransposition())
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
            for (auto transp : prep->getTransposition())
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

//main scheduling function
void NostalgicProcessor::processBlock(int numSamples, int midiChannel, BKSampleLoadType type)
{
    //cluster management
    if (inCluster)
    {
        NostalgicPreparation::Ptr prep = nostalgic->prep;
        //moved beyond clusterThreshold time, done with cluster
        //if (clusterThresholdTimer >= (sampleRate * 0.15)) //eventually expose this threshold
        if (clusterThresholdTimer >= (synth->getSampleRate() * .001 * prep->getClusterThreshold()))
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
            
            if(noteOnPrep->getUndertow() > 0)
            {
                for (auto t : noteOnPrep->getTransposition())
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
                    if (noteOnPrep->getTranspUsesTuning()) // use the Tuning setting
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
                        String(noteOnPrep->getUndertow()));
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
                                     noteOnPrep->getWavedistance(),                        //start position
                                     noteOnPrep->getUndertow(),                            //play length
                                     prep->getUndertowAttack(),
                                     prep->getUndertowDecay(),
                                     prep->getUndertowSustain(),
                                     prep->getUndertowRelease(),
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
                                     noteOnPrep->getWavedistance(),                        //start position
                                     noteOnPrep->getUndertow(),                            //play length
                                     prep->getUndertowAttack(),
                                     prep->getUndertowDecay(),
                                     prep->getUndertowSustain(),
                                     prep->getUndertowRelease(),
                                     tuner,
                                     prep->getGainPtr(),
                                     prep->getBlendronicGainPtr(),
                                     blendronic);
					}
                }

                undertowNotes.insert(0, new NostalgicNoteStuff(thisNote->getNoteNumber()));
                NostalgicNoteStuff* newNote = undertowNotes.getUnchecked(0);
                newNote->setUndertowTargetLength(thisNote->getUndertowTargetLength());
                newNote->setUndertowStartPosition(noteOnPrep->getWavedistance() * synth->getSampleRate()/1000.);
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
