/*
  ==============================================================================

    Nostalgic.cpp
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Nostalgic.h"

NostalgicProcessor::NostalgicProcessor(BKSynthesiser *s,
                                       NostalgicPreparation::Ptr activePrep,
                                       int Id):
Id(Id),
synth(s),
active(activePrep),
tuner(active->getTuning()->processor),
syncProcessor(SynchronicProcessor::Ptr())
{
    noteLengthTimers.ensureStorageAllocated(128);
    velocities.ensureStorageAllocated(128);
    reverseLengthTimers.ensureStorageAllocated(128);
    reverseTargetLength.ensureStorageAllocated(128);
    tuningsAtKeyOn.ensureStorageAllocated(128);
    velocitiesAtKeyOn.ensureStorageAllocated(128);
    preparationAtKeyOn.ensureStorageAllocated(128);
    noteOn.ensureStorageAllocated(128);
    playPositions.ensureStorageAllocated(128);
    startPositions.ensureStorageAllocated(128);
    undertowNoteTimers.ensureStorageAllocated(128);
    undertowPositions.ensureStorageAllocated(128);
    
    for (int i = 0; i < 128; i++)
    {
        noteLengthTimers.insert(i, 0); //initialize timers for all notes
        velocities.insert(i, 0); //store noteOn velocities to set Nostalgic velocities
        reverseLengthTimers.insert(i, 0); //initialize timers for handling wavedistance/undertow
        reverseTargetLength.insert(i, 0);
        tuningsAtKeyOn.insert(i, 0);
        velocitiesAtKeyOn.insert(i, 0);
        preparationAtKeyOn.insert(i, active);
        noteOn.set(i, false);
        playPositions.insert(i, 0);
        startPositions.insert(i, 0);
        undertowNoteTimers.insert(i, 0);
        undertowPositions.insert(i, 0);
    }

}

NostalgicProcessor::~NostalgicProcessor()
{
}

void NostalgicProcessor::setCurrentPlaybackSampleRate(double sr)
{
    sampleRate = sr;
    tuner->setCurrentPlaybackSampleRate(sr);
}

void NostalgicProcessor::playNote(int channel, int note)
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
void NostalgicProcessor::keyReleased(int midiNoteNumber, int midiChannel)
{
    float duration = 0.0;
    
    if (noteOn[midiNoteNumber])
    {
        
        int offRamp;
        if (active->getUndertow() > 0) offRamp = aRampUndertowCrossMS;
        else offRamp = aRampNostalgicOffMS;
        
        //DBG("nostalgic OFFRAMP = " + String(offRamp));
        
        if (active->getMode() == NoteLengthSync)
        {
            //get length of played notes, subtract wave distance to set nostalgic reverse note length
            
            duration =  (noteLengthTimers.getUnchecked(midiNoteNumber) *
                        active->getLengthMultiplier() +
                        (offRamp + 30)) *          //offRamp + onRamp
                        (1000.0 / sampleRate);
             /*
            duration =  (noteLengthTimers.getUnchecked(midiNoteNumber) *
                         active->getLengthMultiplier() ) *
                        (1000.0 / sampleRate);
              */
        /*
        }
        
        else //SynchronicSync
        {
            //get time in ms to target beat, summing over skipped beat lengths
            //duration = syncProcessor->getTimeToBeatMS(active->getBeatsToSkip()); // sum
            SynchronicProcessor::Ptr syncTarget = active->getSyncTargetProcessor();
            duration = syncTarget->getTimeToBeatMS(active->getBeatsToSkip()) + offRamp + 3; // sum
        }
         */
        
            for (auto t : active->getTransposition())
            {
                float offset = t + tuner->getOffset(midiNoteNumber);
                int synthNoteNumber = midiNoteNumber + (int)offset;
                float synthOffset = offset - (int)offset;
                
                //play nostalgic note
                synth->keyOn(
                             midiChannel,
                             synthNoteNumber,
                             synthOffset,
                             velocities.getUnchecked(midiNoteNumber),
                             active->getGain() * aGlobalGain,
                             Reverse,
                             FixedLengthFixedStart,
                             NostalgicNote,
                             Id,
                             duration + active->getWavedistance(),
                             duration,  // length
                             30,
                             offRamp ); //ramp off
            }
            
            // turn note length timers off
            activeNotes.removeFirstMatchingValue(midiNoteNumber);
            noteOn.set(midiNoteNumber, false);
            noteLengthTimers.set(midiNoteNumber, 0);
            //DBG("nostalgic removed active note " + std::to_string(midiNoteNumber));
            
            /*
            //time how long the reverse note has played, to trigger undertow note
            activeReverseNotes.addIfNotAlreadyThere(midiNoteNumber);
            //activeReverseNotes.add(midiNoteNumber);
            reverseLengthTimers.set(midiNoteNumber, 0); //FIX: need 2d array of these, to allow for multiples of the same reverse note! or class...
            reverseTargetLength.set(midiNoteNumber, (duration - (aRampUndertowCrossMS + 30)) * sampleRate/1000.); //to schedule undertow note
            //reverseTargetLength.set(midiNoteNumber, duration * sampleRate/1000.); //to schedule undertow note
            startPositions.set(midiNoteNumber, (duration + active->getWavedistance()) * sampleRate/1000.);
            
            //store values for when undertow note is played (in the event the preparation changes in the meantime)
            tuningsAtKeyOn.set(midiNoteNumber, tuner->getOffset(midiNoteNumber));
            velocitiesAtKeyOn.set(midiNoteNumber, velocities.getUnchecked(midiNoteNumber) * active->getGain());
            preparationAtKeyOn.set(midiNoteNumber, active);
             */
            
            reverseNotes.insert(0, new NostalgicNoteStuff(midiNoteNumber));
            NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
            currentNote->setPrepAtKeyOn(active);
            currentNote->setTuningAtKeyOn(tuner->getOffset(midiNoteNumber));
            currentNote->setVelocityAtKeyOn(velocities.getUnchecked(midiNoteNumber) * active->getGain());
            currentNote->setReverseStartPosition((duration + active->getWavedistance()) * sampleRate/1000.);
            currentNote->setReverseTargetLength((duration - (aRampUndertowCrossMS + 30)) * sampleRate/1000.);
            currentNote->setUndertowTargetLength(active->getUndertow() * sampleRate/1000.);
        }
    }

}

//start timer for length of a particular note; called when key is pressed
void NostalgicProcessor::keyPressed(int midiNoteNumber, float midiNoteVelocity, int midiChannel)
{
    
    if (active->getMode() == SynchronicSync)
    {
        
        tuner = active->getTuning()->processor;
        float duration = 0.0;
        
        int offRamp;
        if (active->getUndertow() > 0) offRamp = aRampUndertowCrossMS;
        else offRamp = aRampNostalgicOffMS;
        
        //get time in ms to target beat, summing over skipped beat lengths
        //duration = syncProcessor->getTimeToBeatMS(active->getBeatsToSkip()); // sum
        SynchronicProcessor::Ptr syncTarget = active->getSyncTargetProcessor();
        duration = syncTarget->getTimeToBeatMS(active->getBeatsToSkip()) + offRamp + 30; // sum
        
        for (auto t : active->getTransposition())
        {
            float offset = t + tuner->getOffset(midiNoteNumber);
            int synthNoteNumber = midiNoteNumber + (int)offset;
            float synthOffset = offset - (int)offset;
            
            /*
            DBG("playing nostalgic note in sync mode "
                + String(synthNoteNumber) + " "
                + String(duration) + " "
                + String(midiNoteVelocity)
                );
             */
            
            //play nostalgic note
            synth->keyOn(
                         midiChannel,
                         synthNoteNumber,
                         synthOffset,
                         midiNoteVelocity,
                         active->getGain() * aGlobalGain,
                         Reverse,
                         FixedLengthFixedStart,
                         NostalgicNote,
                         Id,
                         duration + active->getWavedistance(), //tweak to make sync sound better?
                         duration,  // length
                         30,
                         offRamp ); //ramp off
        }
        
        /*
        //time how long the reverse note has played, to trigger undertow note
        activeReverseNotes.addIfNotAlreadyThere(midiNoteNumber);
        //activeReverseNotes.add(midiNoteNumber);
        reverseLengthTimers.set(midiNoteNumber, 0);
        reverseTargetLength.set(midiNoteNumber, (duration - aRampUndertowCrossMS) * sampleRate/1000.); //to schedule undertow note
        //reverseTargetLength.set(midiNoteNumber, duration * sampleRate/1000.); //to schedule undertow note
        
        //store values for when undertow note is played (in the event the preparation changes in the meantime)
        tuningsAtKeyOn.set(midiNoteNumber, tuner->getOffset(midiNoteNumber));
        velocitiesAtKeyOn.set(midiNoteNumber, velocities.getUnchecked(midiNoteNumber) * active->getGain());
        preparationAtKeyOn.set(midiNoteNumber, active);
         */
        
        reverseNotes.insert(0, new NostalgicNoteStuff(midiNoteNumber));
        NostalgicNoteStuff* currentNote = reverseNotes.getUnchecked(0);
        currentNote->setPrepAtKeyOn(active);
        currentNote->setTuningAtKeyOn(tuner->getOffset(midiNoteNumber));
        currentNote->setVelocityAtKeyOn(midiNoteVelocity);
        currentNote->setReverseStartPosition((duration + active->getWavedistance()) * sampleRate/1000.);
        currentNote->setReverseTargetLength((duration - aRampUndertowCrossMS) * sampleRate/1000.);
        currentNote->setUndertowTargetLength(active->getUndertow() * sampleRate/1000.);
    }
    
    //else?
    tuner = active->getTuning()->processor;
    
    activeNotes.addIfNotAlreadyThere(midiNoteNumber);
    //activeNotes.add(midiNoteNumber);
    noteOn.set(midiNoteNumber, true);
    noteLengthTimers.set(midiNoteNumber, 0);
    velocities.set(midiNoteNumber, midiNoteVelocity);
    
}

//main scheduling function
void NostalgicProcessor::processBlock(int numSamples, int midiChannel)
{
    
    incrementTimers(numSamples); //at end or beginning?
    
    /*
    for(int i = (activeUndertowNotes.size() - 1); i >= 0; --i)
    {
        int tempnote = activeUndertowNotes.getUnchecked(i);
        NostalgicPreparation::Ptr noteOnPrep = preparationAtKeyOn.getUnchecked(tempnote);
        
        if(undertowNoteTimers.getUnchecked(tempnote) > noteOnPrep->getUndertow() * sampleRate/1000.)
        {
            activeUndertowNotes.removeFirstMatchingValue(tempnote);
        }
    }
     */
    
    for(int i = undertowNotes.size() - 1; i >= 0; --i)
    {
        if(undertowNotes.getUnchecked(i)->undertowTimerExceedsTarget())
            undertowNotes.remove(i);
    }
    
    for(int i = reverseNotes.size() - 1; i >= 0; --i)
    {
        NostalgicNoteStuff* thisNote = reverseNotes.getUnchecked(i);
        
        if(thisNote->reverseTimerExceedsTarget())
        {
            NostalgicPreparation::Ptr noteOnPrep = thisNote->getPrepAtKeyOn();
            
            if(noteOnPrep->getUndertow() > 0)
            {
                for (auto t : noteOnPrep->getTransposition())
                {
                    float offset = t + thisNote->getTuningAtKeyOn();
                    int synthNoteNumber = thisNote->getNoteNumber() +  (int)offset;
                    float synthOffset = offset - (int)offset;
                    
                    synth->keyOn(midiChannel,
                                 synthNoteNumber,
                                 synthOffset,
                                 thisNote->getVelocityAtKeyOn(),
                                 aGlobalGain,
                                 Forward,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 Id,
                                 noteOnPrep->getWavedistance(),                        //start position
                                 noteOnPrep->getUndertow(),                            //play length
                                 aRampUndertowCrossMS,                                 //ramp up length
                                 noteOnPrep->getUndertow() - aRampUndertowCrossMS);    //ramp down length
                }

                undertowNotes.insert(0, new NostalgicNoteStuff(thisNote->getNoteNumber()));
                NostalgicNoteStuff* newNote = undertowNotes.getUnchecked(0);
                newNote->setUndertowTargetLength(thisNote->getUndertowTargetLength());
                newNote->setUndertowStartPosition(noteOnPrep->getWavedistance() * sampleRate/1000.);
                
            }
            
            //remove from active notes list
            reverseNotes.remove(i);
        }
    }

    /*
    //check timers to see if any are at an undertow turnaround point, then call keyOn(forward) and keyOff, with 50ms ramps
    for(int i = (activeReverseNotes.size() - 1); i >= 0; --i)
    {
        int tempnote = activeReverseNotes.getUnchecked(i);
        
        //need to use preparation values from when note was played, stored in undertowPreparations
        NostalgicPreparation::Ptr noteOnPrep = preparationAtKeyOn.getUnchecked(tempnote);
        
        if (reverseLengthTimers.getUnchecked(tempnote) > reverseTargetLength.getUnchecked(tempnote))
        //if (reverseLengthTimers.getUnchecked(tempnote) > (reverseTargetLength.getUnchecked(tempnote) - aRampUndertowCrossMS * sampleRate/1000))
        {
 
            if(noteOnPrep->getUndertow() > 0)
            {
                for (auto t : noteOnPrep->getTransposition())
                {
                    float offset = t + tuningsAtKeyOn.getUnchecked(tempnote);
                    int synthNoteNumber = tempnote +  (int)offset;
                    float synthOffset = offset - (int)offset;
                    
                    synth->keyOn(midiChannel,
                                 synthNoteNumber,
                                 synthOffset,
                                 velocitiesAtKeyOn.getUnchecked(tempnote),
                                 aGlobalGain,
                                 Forward,
                                 FixedLengthFixedStart,
                                 NostalgicNote,
                                 Id,
                                 noteOnPrep->getWavedistance(),                        //start position
                                 noteOnPrep->getUndertow(),                            //play length
                                 aRampUndertowCrossMS,                                 //ramp up length
                                 noteOnPrep->getUndertow() - aRampUndertowCrossMS);    //ramp down length
                }
                
                undertowNoteTimers.set(tempnote, 0);
                activeUndertowNotes.addIfNotAlreadyThere(tempnote);
                //activeUndertowNotes.add(tempnote);
            }
            
            //remove from active notes list
            activeReverseNotes.removeFirstMatchingValue(tempnote);

        }
    }
     */
    
    //incrementTimers(numSamples);
}

//increment timers for all active notes, and all currently reversing notes
void NostalgicProcessor::incrementTimers(int numSamples)
{
    
    for(int i = (activeNotes.size() - 1); i >= 0; --i)
    {
        noteLengthTimers.set(activeNotes.getUnchecked(i),
                             noteLengthTimers.getUnchecked(activeNotes.getUnchecked(i)) + numSamples);
    }
    
    /*
    for(int i = (activeReverseNotes.size() - 1); i >= 0; --i)
    {
        reverseLengthTimers.set(activeReverseNotes.getUnchecked(i),
                                reverseLengthTimers.getUnchecked(activeReverseNotes.getUnchecked(i)) + numSamples);
        
        playPositions.set(activeReverseNotes.getUnchecked(i),
                          startPositions.getUnchecked(activeReverseNotes.getUnchecked(i)) -
                          reverseLengthTimers.getUnchecked(activeReverseNotes.getUnchecked(i)));
    }
    
    for(int i = (activeUndertowNotes.size() - 1); i>=0; i--)
    {
        undertowNoteTimers.set(activeUndertowNotes.getUnchecked(i),
                               undertowNoteTimers.getUnchecked(activeUndertowNotes.getUnchecked(i)) + numSamples);
        
        undertowPositions.set(activeUndertowNotes.getUnchecked(i),
                             active->getWavedistance() * sampleRate/1000. +
                             undertowNoteTimers.getUnchecked(activeUndertowNotes.getUnchecked(i)));
        
    }
     */
    
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
    
    /*
    for(int i=0; i<activeReverseNotes.size(); i++)
    {
        newpositions.set(i, playPositions.getUnchecked(activeReverseNotes.getUnchecked(i)) * 1000./sampleRate);
    }
     */
    
    for(int i = 0; i<reverseNotes.size(); i++)
    {
        newpositions.set(i, reverseNotes.getUnchecked(i)->getReversePlayPosition() * 1000./sampleRate);
    }
    
    return newpositions;
}

Array<int> NostalgicProcessor::getUndertowPositions() //return playback positions in ms, not samples
{
    Array<int> newpositions;
    
    /*
    for(int i=0; i<activeUndertowNotes.size(); i++)
    {
        newpositions.set(i, undertowPositions.getUnchecked(activeUndertowNotes.getUnchecked(i)) * 1000./sampleRate);
    }
     */
    
    for(int i = 0; i<undertowNotes.size(); i++)
    {
        newpositions.set(i, undertowNotes.getUnchecked(i)->getUndertowPlayPosition() * 1000./sampleRate);
    }
    
    return newpositions;
}
