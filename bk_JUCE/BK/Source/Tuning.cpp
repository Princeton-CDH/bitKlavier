/*
 ==============================================================================
 
 Tuning.cpp
 Created: 4 Dec 2016 3:40:27pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "Tuning.h"


TuningProcessor::TuningProcessor(TuningPreparation::Ptr prep):
preparation(prep)
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
    if(preparation->getTuning() == AdaptiveTuning || preparation->getTuning() == AdaptiveAnchoredTuning)
        return adaptiveCalculate(midiNoteNumber);
    
    //else do regular tunings
    Array<float> currentTuning;
    if(preparation->getTuning() == CustomTuning) currentTuning = preparation->getCustomScale();
    else currentTuning = tuningLibrary.getUnchecked(preparation->getTuning());
    
    return (currentTuning[(midiNoteNumber - preparation->getFundamental()) % 12] + preparation->getFundamentalOffset());
    
}


//for keeping track of current cluster size
void TuningProcessor::incrementAdaptiveClusterTime(int numSamples)
{
    if(preparation->getTuning() == AdaptiveTuning || preparation->getTuning() == AdaptiveAnchoredTuning) {
        
        if(clusterTime <= preparation->getAdaptiveClusterThresh() * sampleRate * 0.001) clusterTime += numSamples;
        
    }
}


//add note to the adaptive tuning history, update adaptive fundamental
void TuningProcessor::keyOn(int midiNoteNumber)
{

    if(preparation->getTuning() == AdaptiveTuning)
    {
        if(clusterTime * (1000.0 / sampleRate) > preparation->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= preparation->getAdaptiveHistory() - 1)
        {
            adaptiveHistoryCounter = 0;
            adaptiveFundamentalFreq = adaptiveFundamentalFreq * adaptiveCalculateRatio(midiNoteNumber);
            adaptiveFundamentalNote = midiNoteNumber;
        }
        else adaptiveHistoryCounter++;
        
    }
    
    else if(preparation->getTuning() == AdaptiveAnchoredTuning)
    {
        if(clusterTime * (1000.0 / sampleRate) > preparation->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= preparation->getAdaptiveHistory() - 1)
        {
            adaptiveHistoryCounter = 0;
            
            const Array<float> anchorTuning = tuningLibrary.getUnchecked(preparation->getAdaptiveAnchorScale());
            adaptiveFundamentalFreq = mtof(midiNoteNumber +
                                           anchorTuning[(midiNoteNumber + preparation->getAdaptiveAnchorFundamental()) % 12]
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
    
    const Array<float> intervalScale = tuningLibrary.getUnchecked(preparation->getAdaptiveIntervalScale());
    
    if(!preparation->getAdaptiveInversional() || tempnote >= adaptiveFundamentalNote)
    {
        
        while((tempnote - adaptiveFundamentalNote) < 0) tempnote += 12;
    
        newnote = midiNoteNumber + intervalScale[(tempnote - adaptiveFundamentalNote) % 12];
        newratio = intervalToRatio(newnote - adaptiveFundamentalNote);
        
        return newratio;
        
    }
    //else
    newnote = midiNoteNumber - intervalScale[(adaptiveFundamentalNote - tempnote) % 12];
    newratio = intervalToRatio(newnote - adaptiveFundamentalNote);
    
    return newratio;
}

float TuningProcessor::adaptiveCalculate(int midiNoteNumber) const
{
    float newnote = adaptiveFundamentalFreq * adaptiveCalculateRatio(midiNoteNumber);
    return ftom(newnote) - midiNoteNumber;
}


