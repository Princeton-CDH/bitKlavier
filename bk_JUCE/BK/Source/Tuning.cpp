/*
 ==============================================================================
 
 Tuning.cpp
 Created: 4 Dec 2016 3:40:27pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "Tuning.h"


TuningProcessor::TuningProcessor(TuningPreparation::Ptr active):
active(active)
{
    
    tuningLibrary.ensureStorageAllocated(6);
    
    tuningLibrary.set(PartialTuning, tPartialTuning);
    tuningLibrary.set(JustTuning, tJustTuning);
    tuningLibrary.set(EqualTemperament, tEqualTuning);
    tuningLibrary.set(DuodeneTuning, tDuodeneTuning);
    tuningLibrary.set(OtonalTuning, tOtonalTuning);
    tuningLibrary.set(UtonalTuning, tUtonalTuning);
    
}

TuningProcessor::~TuningProcessor()
{
}

//returns offsets; main callback
float TuningProcessor::getOffset(int midiNoteNumber) const
{
    
    //do adaptive tunings if using
    if(active->getTuning() == AdaptiveTuning || active->getTuning() == AdaptiveAnchoredTuning)
        return adaptiveCalculate(midiNoteNumber);
    
    //do absolute tuning if using
    if(active->getTuning() == AbsoluteTuning)
    {
        Array<float> offsets = active->getAbsoluteOffsets();
        return offsets.getUnchecked(midiNoteNumber);
    }

    //else do regular tunings
    Array<float> currentTuning;
    if(active->getTuning() == CustomTuning) currentTuning = active->getCustomScale();
    else currentTuning = tuningLibrary.getUnchecked(active->getTuning());
    
    //return (currentTuning[(midiNoteNumber - active->getFundamental()) % 12] + active->getFundamentalOffset());
    return (currentTuning[(midiNoteNumber - active->getFundamental()) % currentTuning.size()] + active->getFundamentalOffset());
    
}


//for keeping track of current cluster size
void TuningProcessor::incrementAdaptiveClusterTime(int numSamples)
{
    if(active->getTuning() == AdaptiveTuning || active->getTuning() == AdaptiveAnchoredTuning) {
        
        if(clusterTime <= active->getAdaptiveClusterThresh() * sampleRate * 0.001) clusterTime += numSamples;
        
    }
}


//add note to the adaptive tuning history, update adaptive fundamental
void TuningProcessor::keyOn(int midiNoteNumber)
{

    if(active->getTuning() == AdaptiveTuning)
    {
        if(clusterTime * (1000.0 / sampleRate) > active->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= active->getAdaptiveHistory() - 1)
        {
            adaptiveHistoryCounter = 0;
            adaptiveFundamentalFreq = adaptiveFundamentalFreq * adaptiveCalculateRatio(midiNoteNumber);
            adaptiveFundamentalNote = midiNoteNumber;
        }
        else adaptiveHistoryCounter++;
        
    }
    
    else if(active->getTuning() == AdaptiveAnchoredTuning)
    {
        if(clusterTime * (1000.0 / sampleRate) > active->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= active->getAdaptiveHistory() - 1)
        {
            adaptiveHistoryCounter = 0;
            
            const Array<float> anchorTuning = tuningLibrary.getUnchecked(active->getAdaptiveAnchorScale());
            adaptiveFundamentalFreq = mtof(midiNoteNumber +
                                           anchorTuning[(midiNoteNumber + active->getAdaptiveAnchorFundamental()) % anchorTuning.size()]
                                           );
            adaptiveFundamentalNote = midiNoteNumber;
        }
        else adaptiveHistoryCounter++;
    }
    
    clusterTime = 0;
    
}

float TuningProcessor::adaptiveCalculateRatio(const int midiNoteNumber) const
{
    int tempnote = midiNoteNumber;
    float newnote;
    float newratio;
    
    const Array<float> intervalScale = tuningLibrary.getUnchecked(active->getAdaptiveIntervalScale());
    
    if(!active->getAdaptiveInversional() || tempnote >= adaptiveFundamentalNote)
    {
        
        while((tempnote - adaptiveFundamentalNote) < 0) tempnote += 12;
    
        newnote = midiNoteNumber + intervalScale[(tempnote - adaptiveFundamentalNote) % intervalScale.size()];
        newratio = intervalToRatio(newnote - adaptiveFundamentalNote);
        
        return newratio;
        
    }
    //else
    newnote = midiNoteNumber - intervalScale[(adaptiveFundamentalNote - tempnote) % intervalScale.size()];
    newratio = intervalToRatio(newnote - adaptiveFundamentalNote);
    
    return newratio;
}

float TuningProcessor::adaptiveCalculate(int midiNoteNumber) const
{
    float newnote = adaptiveFundamentalFreq * adaptiveCalculateRatio(midiNoteNumber);
    return ftom(newnote) - midiNoteNumber;
}


