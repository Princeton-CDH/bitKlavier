/*
  ==============================================================================

    Synchronic.cpp
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Synchronic.h"

SynchronicProcessor::SynchronicProcessor(BKSynthesiser *s,
                                         SynchronicPreparation::Ptr ap,
                                         GeneralSettings::Ptr general,
                                         int Id):
Id(Id),
synth(s),
general(general),
active(ap),
tuner(active->getTuning()->processor)
{
    velocities.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        velocities.insert(i, 0.);
    }
    
    clusterTimer = 0;
    phasor = 0;
    
    atTimer = 0;
    atLastTime = 0;
    atDeltaHistory.ensureStorageAllocated(10);
    for (int i = 0; i < 10; i++)
    {
        atDeltaHistory.insert(0, 0.5 * (active->getAdaptiveTempo1Min() + active->getAdaptiveTempo1Max()));
    }
    adaptiveTempoPeriodMultiplier = 1.;
    
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
    tuner->setCurrentPlaybackSampleRate(sr);
}



void SynchronicProcessor::playNote(int channel, int note, float velocity)
{
    PianoSamplerNoteDirection noteDirection = Forward;
    float noteStartPos = 0.0;
    
    //float noteLength = (fabs(active->getLengthMultipliers()[length]) * tempoPeriod); //option for later
    float noteLength = (fabs(active->getLengthMultipliers()[lengthMultiplierCounter]) * 50.0);
    
    if (active->getLengthMultipliers()[lengthMultiplierCounter] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength;
    }
    
    float offset = tuner->getOffset(note) + active->getTranspOffsets()[transpOffsetCounter];
    int synthNoteNumber = ((float)note + (int)offset);
    offset -= (int)offset;
    
    synth->keyOn(channel,
                 synthNoteNumber,
                 offset,
                 velocity,
                 aGlobalGain,
                 noteDirection,
                 FixedLengthFixedStart,
                 SynchronicNote,
                 Id,
                 noteStartPos, // start
                 noteLength,
                 3,
                 30);
}

void SynchronicProcessor::resetPhase(int skipBeats)
{
    
    beatMultiplierCounter   = skipBeats % active->getBeatMultipliers().size();
    lengthMultiplierCounter = skipBeats % active->getLengthMultipliers().size();
    accentMultiplierCounter = skipBeats % active->getAccentMultipliers().size();
    transpOffsetCounter     = skipBeats % active->getTranspOffsets().size();
    
    beatCounter   = 0;

}

void SynchronicProcessor::keyPressed(int noteNumber, float velocity)
{
    
    //set tuning
    tuner = active->getTuning()->processor;
    
    //store velocity
    velocities.set(noteNumber, velocity);
 
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    
    //add to adaptive tempo history, update adaptive tempo
    atNewNote();
    
    //silence pulses if in NoteOffSync
    if(    (active->getMode() == LastNoteOffSync)
        || (active->getMode() == AnyNoteOffSync))
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
        resetPhase(active->getBeatsToSkip());

        //now we are in a cluster!
        inCluster = true;
        
    }
    else if (active->getMode() == AnyNoteOnSync)
    {
        
        //reset phasor if in AnyNoteOnSync
        phasor = 0;
        resetPhase(active->getBeatsToSkip());

    }
    
    //at this point, we are in cluster one way or another!
    
    //save note in the cluster, even if it's already there. then cap the cluster to a hard cap (8 for now)
    //this is different than avoiding dupes at this stage (with "addIfNotAlreadyThere") because it allows
    //repeated notes to push older notes out the back.
    //later, we remove dupes so we don't inadvertently play the same note twice in a pulse
    
    cluster.insert(0, noteNumber);
    if(cluster.size() > active->getClusterCap()) cluster.resize(active->getClusterCap());
    //DBG("cluster size: " + String(cluster.size()) + " " + String(clusterThresholdSamples/sampleRate));
    
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


void SynchronicProcessor::keyReleased(int noteNumber, int channel)
{
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    //adaptive tempo
    atNewNoteOff();
    
    // If AnyNoteOffSync mode, reset phasor and multiplier indices.
    //only initiate pulses if ALL keys are released
    if (    (active->getMode() == LastNoteOffSync && keysDepressed.size() == 0)
         || (active->getMode() == AnyNoteOffSync))
    {
        phasor = beatThresholdSamples; //start right away
        resetPhase(active->getBeatsToSkip() - 1);
        
        inCluster = true;
        shouldPlay = true;
    }
}


void SynchronicProcessor::processBlock(int numSamples, int channel)
{
    
    //adaptive tuning timer update
    tuner->incrementAdaptiveClusterTime(numSamples);
    
    //adaptive tempo timer update
    atTimer += numSamples;
    
    //need to do this every block?
    clusterThresholdSamples = (active->getClusterThreshSEC() * sampleRate);
    beatThresholdSamples = (active->getBeatThresh() * sampleRate);
    
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
                            active->getBeatMultipliers()[beatMultiplierCounter] *
                            general->getTempoDivider() *
                            adaptiveTempoPeriodMultiplier;
        
        //check to see if enough time has passed for next beat
        if (phasor >= numSamplesBeat)
        {
            
            //reset phasor for next beat
            phasor -= numSamplesBeat;
            
            //increment parameter counters
            if (++lengthMultiplierCounter   >= active->getLengthMultipliers().size())     lengthMultiplierCounter = 0;
            if (++accentMultiplierCounter   >= active->getAccentMultipliers().size())     accentMultiplierCounter = 0;
            if (++transpOffsetCounter       >= active->getTranspOffsets().size())         transpOffsetCounter = 0;
            
            /*
            //update display of counters in UI
            DBG(" length: "         + String(active->getLengthMultipliers()[lengthMultiplierCounter]) +
                " length counter:"  + String(lengthMultiplierCounter) +
                " accent: "         + String(active->getAccentMultipliers()[accentMultiplierCounter]) +
                " accent counter: " + String(accentMultiplierCounter) +
                " transp: "         + String(active->getTranspOffsets()[transpOffsetCounter]) +
                " transp counter: " + String(transpOffsetCounter)
                );
            */
            
            //play all the notes in the cluster, with current parameter vals
            if (cluster.size() >= active->getClusterMin() && cluster.size() <= active->getClusterMax())
            {
                for (int n = 0; n < slimCluster.size(); n++)
                {
                    playNote(channel,
                             cluster[n],
                             velocities.getUnchecked(cluster[n]) * active->getAccentMultipliers()[accentMultiplierCounter]);
                }
                
            }
            
            //increment beat and beatMultiplier counters, for next beat; check maxes and adjust
            if (++beatMultiplierCounter >= active->getBeatMultipliers().size()) beatMultiplierCounter = 0;
            if (++beatCounter >= active->getNumBeats()) shouldPlay = false; //done with pulses
            
            //update display of beat counter in UI
            /*
            DBG(" beat length: "    + String(active->getBeatMultipliers()[beatMultiplierCounter]) +
                " beatMultiplier counter: "   + String(beatMultiplierCounter)
                );
            DBG(" ");
              */

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
    
    //tolerance: if key release happens just before beat (<30ms) then add a beatToSkip
    if (timeToReturn < .03 * sampleRate) beatsToSkip++;
    
    while(beatsToSkip-- > 0)
    {
        if (++myBeat >= active->getBeatMultipliers().size()) myBeat = 0;
        timeToReturn += active->getBeatMultipliers()[myBeat] *
                        beatThresholdSamples *
                        general->getTempoDivider() *
                        adaptiveTempoPeriodMultiplier;
    }
    
    DBG("time in ms to next beat = " + std::to_string(timeToReturn * 1000./sampleRate));
    return timeToReturn * 1000./sampleRate; //optimize later....
}


/*
 //Adaptive Tempo 1
 inline AdaptiveTempo1Mode getAdaptiveTempo1Mode(void)   {return at1Mode;   }
 inline int getAdaptiveTempo1History(void)               {return at1History;}
 inline float getAdaptiveTempo1Subdivisions(void)        {return at1Subdivisions;}
 inline float getAdaptiveTempo1Min(void)                 {return at1Min;}
 inline float getAdaptiveTempo1Max(void)                 {return at1Max;}
 
 */

void SynchronicProcessor::atNewNote()
{
    if(active->getAdaptiveTempo1Mode() == TimeBetweenNotes) atCalculatePeriodMultiplier();
    atLastTime = atTimer;
}

void SynchronicProcessor::atNewNoteOff()
{
    if(active->getAdaptiveTempo1Mode() == NoteLength) atCalculatePeriodMultiplier();
}

void SynchronicProcessor::atCalculatePeriodMultiplier()
{
    //only do if history val is > 0
    if(active->getAdaptiveTempo1History()) {
        
        atDelta = (atTimer - atLastTime) / (0.001 * sampleRate);
        //DBG("atDelta = " + String(atDelta));
        
        //constrain be min and max times between notes
        if(atDelta > active->getAdaptiveTempo1Min() && atDelta < active->getAdaptiveTempo1Max()) {
            
            //insert delta at beginning of history
            atDeltaHistory.insert(0, atDelta);
            
            //eliminate oldest time difference
            atDeltaHistory.resize(active->getAdaptiveTempo1History());
            
            //calculate moving average and then tempo period multiplier
            int totalDeltas = 0;
            for(int i = 0; i < atDeltaHistory.size(); i++) totalDeltas += atDeltaHistory.getUnchecked(i);
            float movingAverage = totalDeltas / active->getAdaptiveTempo1History();
            
            adaptiveTempoPeriodMultiplier = movingAverage /
                                            (beatThresholdSamples / (0.001 * sampleRate)) /
                                            active->getAdaptiveTempo1Subdivisions();
            
            DBG("adaptiveTempoPeriodMultiplier = " + String(adaptiveTempoPeriodMultiplier));
        }
    }
}


