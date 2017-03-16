/*
  ==============================================================================

    Tempo.cpp
    Created: 26 Feb 2017 11:38:35pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "Tempo.h"

TempoProcessor::TempoProcessor(TempoPreparation::Ptr active):
active(active)
{
    atTimer = 0;
    atLastTime = 0;
    atDeltaHistory.ensureStorageAllocated(10);
    for (int i = 0; i < 10; i++)
    {
        atDeltaHistory.insert(0, (60000.0/active->getTempo()));
    }
    adaptiveTempoPeriodMultiplier = 1.;
}

TempoProcessor::~TempoProcessor()
{
}

void TempoProcessor::processBlock(int numSamples, int channel)
{
    atTimer += numSamples;
}

void TempoProcessor::keyPressed(int noteNumber, float velocity)
{
    DBG("adding adaptive tempo note" + String(noteNumber));
    atNewNote();
}

void TempoProcessor::keyReleased(int noteNumber, int channel)
{
    atNewNoteOff();
}

//adaptive tempo functions
void TempoProcessor::atNewNote()
{
    if(active->getAdaptiveTempo1Mode() == TimeBetweenNotes) atCalculatePeriodMultiplier();
    atLastTime = atTimer;
}

void TempoProcessor::atNewNoteOff()
{
    if(active->getAdaptiveTempo1Mode() == NoteLength) atCalculatePeriodMultiplier();
}

//really basic, using constrained moving average of time-between-notes (or note-length)
void TempoProcessor::atCalculatePeriodMultiplier()
{
    //only do if history val is > 0
    if(active->getAdaptiveTempo1History()) {
        
        atDelta = (atTimer - atLastTime) / (0.001 * sampleRate); //fix this? make sampleRateMS
        //DBG("atTimer = " + String(atTimer) + " atLastTime = " + String(atLastTime));
        //DBG("atDelta = " + String(atDelta));
        //DBG("sampleRate = " + String(sampleRate));
        
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
                                            active->getBeatThreshMS() /
                                            active->getAdaptiveTempo1Subdivisions();
            
            DBG("adaptiveTempoPeriodMultiplier = " + String(adaptiveTempoPeriodMultiplier));
        }
    }
}

void TempoProcessor::reset()
{
    for (int i = 0; i < active->getAdaptiveTempo1History(); i++)
    {
        atDeltaHistory.insert(0, (60000.0/active->getTempo()));
    }
    adaptiveTempoPeriodMultiplier = 1.;
}
