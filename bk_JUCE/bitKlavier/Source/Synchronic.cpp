/*
  ==============================================================================

    Synchronic.cpp
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Synchronic.h"

SynchronicProcessor::SynchronicProcessor(Synchronic::Ptr synchronic,
                                         TuningProcessor::Ptr tuning,
                                         TempoProcessor::Ptr tempo,
                                         BKSynthesiser* main,
                                         GeneralSettings::Ptr general):
synth(main),
general(general),
synchronic(synchronic),
tuner(tuning),
tempo(tempo)
{
    velocities.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        velocities.insert(i, 0.);
    }
    
    clusterTimer = 0;
    phasor = 0;
    envelopeCounter = 0;
     
    inCluster = false;
    
    cluster = Array<int>();
    keysDepressed = Array<int>();
    
    shouldPlay = false;
}



SynchronicProcessor::~SynchronicProcessor()
{
}

void SynchronicProcessor::setCurrentPlaybackSampleRate(double sr)
{
    sampleRate = sr;
    
}


void SynchronicProcessor::playNote(int channel, int note, float velocity)
{
	
    PianoSamplerNoteDirection noteDirection = Forward;
    float noteStartPos = 0.0;
    
    float noteLength = (fabs(synchronic->aPrep->getLengthMultipliers()[lengthMultiplierCounter]) * tempo->getTempo()->aPrep->getBeatThreshMS());
    //float noteLength = (fabs(synchronic->aPrep->getLengthMultipliers()[lengthMultiplierCounter]) * 50.0); //original way,  multiples of 50ms
    
    if (synchronic->aPrep->getLengthMultipliers()[lengthMultiplierCounter] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength + 3; //adjust for rampOn time == 3ms
    }
    
    for (auto t : synchronic->aPrep->getTransposition()[transpCounter])
    {
        float offset = tuner->getOffset(note) + t;
        int synthNoteNumber = ((float)note + (int)offset);
        float synthOffset = offset - (int)offset;

        synth->keyOn(channel,
                     note,
                     synthNoteNumber,
                     synthOffset,
                     velocity,
                     synchronic->aPrep->getGain() * aGlobalGain * synchronic->aPrep->getAccentMultipliers()[accentMultiplierCounter],
                     noteDirection,
                     FixedLengthFixedStart,
                     SynchronicNote,
                     synchronic->getId(),
                     noteStartPos,  // start
                     noteLength,
                     synchronic->aPrep->getAttack(envelopeCounter),
                     synchronic->aPrep->getDecay(envelopeCounter),
                     synchronic->aPrep->getSustain(envelopeCounter),
                     synchronic->aPrep->getRelease(envelopeCounter)
                     );
    }
    
}

void SynchronicProcessor::resetPhase(int skipBeats)
{
    //DBG("resetting phase");
    beatMultiplierCounter   = skipBeats % synchronic->aPrep->getBeatMultipliers().size();
    lengthMultiplierCounter = skipBeats % synchronic->aPrep->getLengthMultipliers().size();
    accentMultiplierCounter = skipBeats % synchronic->aPrep->getAccentMultipliers().size();
    transpCounter           = skipBeats % synchronic->aPrep->getTransposition().size();
    envelopeCounter         = skipBeats % synchronic->aPrep->getEnvelopesOn().size();
    
    beatCounter = 0;

}

void SynchronicProcessor::keyPressed(int noteNumber, float velocity)
{
	velocities.set(noteNumber, velocity);
 
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    
    //add to adaptive tempo history, update adaptive tempo
    //atNewNote();
    
    //silence pulses if in NoteOffSync
    if(    (synchronic->aPrep->getMode() == LastNoteOffSync)
        || (synchronic->aPrep->getMode() == AnyNoteOffSync))
            shouldPlay = false;
    
    else shouldPlay = true;
    
    //cluster management
    if(!inCluster) //we have a new cluster
    {
        
        //reset phasor
        phasor = 0;
        
        //clear cluster
        cluster.clearQuick();
        
        //reset parameter counters; need to account for skipBeats
        resetPhase(synchronic->aPrep->getBeatsToSkip());

        //now we are in a cluster!
        inCluster = true;
        
    }
    else if (synchronic->aPrep->getMode() == AnyNoteOnSync)
    {
        
        //reset phasor if in AnyNoteOnSync
        phasor = 0;
        resetPhase(synchronic->aPrep->getBeatsToSkip());

    }
    
    //at this point, we are in cluster one way or another!
    
    //save note in the cluster, even if it's already there. then cap the cluster to a hard cap (8 for now)
    //this is different than avoiding dupes at this stage (with "addIfNotAlreadyThere") because it allows
    //repeated notes to push older notes out the back.
    //later, we remove dupes so we don't inadvertently play the same note twice in a pulse
    
    cluster.insert(0, noteNumber);
    if(cluster.size() > synchronic->aPrep->getClusterCap()) cluster.resize(synchronic->aPrep->getClusterCap());
    DBG("cluster size: " + String(cluster.size()) + " " + String(clusterThresholdSamples/sampleRate));
    
    //why not use clusterMax for this? the intent is different:
    //clusterMax: max number of keys pressed within clusterThresh, otherwise shut off pulses
    //clusterCap: the most number of notes allowed in a cluster when playing pulses
    //so, let's say we are playing a rapid passage where successive notes are within the clusterThresh
    //and we want the pulse to emerge when we stop; clusterMax wouldn't allow this to happen
    //if we had exceeded clusterMax in that passage, which is bad, but we still want clusterCap
    //to limit the number of notes included in the cluster.
    //for now, we'll leave clusterCap unexposed, just to avoid confusion for the user. after all,
    //I used it this way for all of the etudes to date! but might want to expose eventually...
    //perhaps call beatVoices? since it's essentially the number of "voices" in the pulse chord?
    
    //reset the timer for time between notes
    clusterThresholdTimer = 0;

}


void SynchronicProcessor::keyReleased(int noteNumber, float velocity, int channel)
{
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    //adaptive tempo
    //atNewNoteOff();
    
    // If AnyNoteOffSync mode, reset phasor and multiplier indices.
    //only initiate pulses if ALL keys are released
    if ((synchronic->aPrep->getMode() == LastNoteOffSync && keysDepressed.size() == 0) || 
		(synchronic->aPrep->getMode() == AnyNoteOffSync))
    {
        
        resetPhase(synchronic->aPrep->getBeatsToSkip() - 1);
        
        //start right away
        phasor =    beatThresholdSamples *
                    synchronic->aPrep->getBeatMultipliers()[beatMultiplierCounter] *
                    general->getPeriodMultiplier() *
                    tempo->getPeriodMultiplier();
        
        
        inCluster = true;
        shouldPlay = true;
    }
}


void SynchronicProcessor::processBlock(int numSamples, int channel)
{
    //need to do this every block?
    clusterThresholdSamples = (synchronic->aPrep->getClusterThreshSEC() * sampleRate);
    //beatThresholdSamples = (synchronic->aPrep->getBeatThresh() * sampleRate);
    beatThresholdSamples = (tempo->getTempo()->aPrep->getBeatThresh() * sampleRate);
    
    //cluster management
    if (inCluster)
    {
        //moved beyond clusterThreshold time, done with cluster
        if (clusterThresholdTimer >= clusterThresholdSamples)
        {
            inCluster = false;
        }
        //otherwise incrument cluster timer
        else
        {
            clusterThresholdTimer += numSamples;
        }
    }
    
    if(shouldPlay)
    {
        
        //remove duplicates from cluster, so we don't play the same note twice in a single pulse
        slimCluster.clearQuick();
        for(int i = 0; i< cluster.size(); i++) slimCluster.addIfNotAlreadyThere(cluster.getUnchecked(i));
    
        //get time until next beat => beat length scaled by beatMultiplier parameter
        numSamplesBeat =    beatThresholdSamples *
                            synchronic->aPrep->getBeatMultipliers()[beatMultiplierCounter] *
                            general->getPeriodMultiplier() *
                            tempo->getPeriodMultiplier();
        
        //check to see if enough time has passed for next beat
        if (phasor >= numSamplesBeat)
        {
            
            //reset phasor for next beat
            phasor -= numSamplesBeat;
            
            //increment parameter counters
            if (++lengthMultiplierCounter   >= synchronic->aPrep->getLengthMultipliers().size())     lengthMultiplierCounter = 0;
            if (++accentMultiplierCounter   >= synchronic->aPrep->getAccentMultipliers().size())     accentMultiplierCounter = 0;
            if (++transpCounter             >= synchronic->aPrep->getTransposition().size())         transpCounter = 0;
            if (++envelopeCounter           >= synchronic->aPrep->getEnvelopesOn().size())           envelopeCounter = 0;
  
            while(!synchronic->aPrep->getEnvelopesOn()[envelopeCounter]) //skip untoggled envelopes
            {
                envelopeCounter++;
                if (envelopeCounter >= synchronic->aPrep->getEnvelopesOn().size()) envelopeCounter = 0;
            }
            
            //update display of counters in UI
            /*
            DBG(" samplerate: " + String(sampleRate) +
                " length: "         + String(synchronic->aPrep->getLengthMultipliers()[lengthMultiplierCounter]) +
                " length counter: "  + String(lengthMultiplierCounter) +
                " accent: "         + String(synchronic->aPrep->getAccentMultipliers()[accentMultiplierCounter]) +
                " accent counter: " + String(accentMultiplierCounter) +
                " transp: "         + "{ "+floatArrayToString(synchronic->aPrep->getTransposition()[transpCounter]) + " }" +
                " transp counter: " + String(transpCounter) +
                " envelope on: "       + String((int)synchronic->aPrep->getEnvelopesOn()[envelopeCounter]) +
                " envelope counter: " + String(envelopeCounter) +
                " ADSR :" + String(synchronic->aPrep->getAttack(envelopeCounter)) + " " + String(synchronic->aPrep->getDecay(envelopeCounter)) + " " + String(synchronic->aPrep->getSustain(envelopeCounter)) + " " + String(synchronic->aPrep->getRelease(envelopeCounter))
                );
            */
            
            //figure out whether to play the cluster
            playCluster = false;
            
            //in the normal case, where cluster is within a range defined by clusterMin and Max
            if(synchronic->aPrep->getClusterMin() <= synchronic->aPrep->getClusterMax())
            {
                if (cluster.size() >= synchronic->aPrep->getClusterMin() && cluster.size() <= synchronic->aPrep->getClusterMax())
                    playCluster = true;
            }
            //the inverse case, where we only play cluster that are *outside* the range set by clusterMin and Max
            else
            {
                if (cluster.size() >= synchronic->aPrep->getClusterMin() || cluster.size() <= synchronic->aPrep->getClusterMax())
                    playCluster = true;
            }
     
            //if (cluster.size() >= synchronic->aPrep->getClusterMin() && cluster.size() <= synchronic->aPrep->getClusterMax())
            if(playCluster)
            {
                //for (int n = slimCluster.size(); --n >= 0;)
                for (int n=0; n < cluster.size(); n++)
                {
                    
                    /*playNote(channel,
                             cluster[n],
                             velocities.getUnchecked(cluster[n]));*/
                     
                   
					playNote(channel,
                             slimCluster[n],
                             velocities.getUnchecked(slimCluster[n]));
					
                }
                
            }
            
            //increment beat and beatMultiplier counters, for next beat; check maxes and adjust
            if (++beatMultiplierCounter >= synchronic->aPrep->getBeatMultipliers().size()) beatMultiplierCounter = 0;
            if (++beatCounter >= synchronic->aPrep->getNumBeats()) shouldPlay = false; //done with pulses

        }
        
        //pass time until next beat
        phasor += numSamples;
    }
    
}

//return time in ms to future beat, given beatsToSkip
float SynchronicProcessor::getTimeToBeatMS(float beatsToSkip)
{

    uint64 timeToReturn = numSamplesBeat - phasor; //next beat
    int myBeat = beatMultiplierCounter;
    
    //tolerance: if key release happens just before beat (<100ms) then add a beatToSkip
    if (timeToReturn < .1 * sampleRate) beatsToSkip++;
    
    while(beatsToSkip-- > 0)
    {
        if (++myBeat >= synchronic->aPrep->getBeatMultipliers().size()) myBeat = 0;
        timeToReturn += synchronic->aPrep->getBeatMultipliers()[myBeat] *
                        beatThresholdSamples *
                        general->getPeriodMultiplier() *
                        tempo->getPeriodMultiplier();
                        //adaptiveTempoPeriodMultiplier;
    }
    
    //DBG("time in ms to next beat = " + String(timeToReturn * 1000./sampleRate));
    return timeToReturn * 1000./sampleRate; //optimize later....
}



