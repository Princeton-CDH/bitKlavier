/*
  ==============================================================================

    Tuning.cpp
    Created: 4 Dec 2016 3:40:27pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "Tuning.h"


TuningProcessor::TuningProcessor()
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

//returns offsets
float TuningProcessor::getOffset(int midiNoteNumber, TuningSystem tuning, int basepitch) const
{
    
    if(tuning == AdaptiveTuning) return 0.; //need to implement
    if(tuning == AdaptiveAnchoredTuning) return 0.;
    
    Array<float> currentTuning = tuningLibrary.getUnchecked(tuning);
    return(currentTuning[(midiNoteNumber - basepitch) % 12]);
    
}

//for calculating adaptive tuning
void TuningProcessor::keyOn(int midiNoteNumber)
{
    mtof(60.);
}
