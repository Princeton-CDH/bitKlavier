/*
 ==============================================================================
 
 Tuning.cpp
 Created: 4 Dec 2016 3:40:27pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "Tuning.h"


TuningProcessor::TuningProcessor(TuningPreparation::Ptr prep, int layer)
:
preparation(prep),
layer(layer)
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

void TuningProcessor::incrementAdaptiveClusterTime(int numSamples)
{
    if(clusterTime <= preparation->getAdaptiveClusterThresh()) clusterTime += numSamples;
}

//returns offsets
float TuningProcessor::getOffset(int midiNoteNumber) const
{
    
    if(preparation->getTuning() == AdaptiveTuning) return 0.; //need to implement
    if(preparation->getTuning() == AdaptiveAnchoredTuning) return 0.;
    
    Array<float> currentTuning = tuningLibrary.getUnchecked(preparation->getTuning());
    return (currentTuning[(midiNoteNumber - preparation->getFundamental()) % 12]);
    
}

//for calculating adaptive tuning
void TuningProcessor::keyOn(int midiNoteNumber)
{
    mtof(60.);
}
