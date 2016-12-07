/*
  ==============================================================================

    Tuning.h
    Created: 4 Dec 2016 3:40:02pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef TUNING_H_INCLUDED
#define TUNING_H_INCLUDED

#include "BKUtilities.h"

class TuningProcessor : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TuningProcessor>   Ptr;
    typedef Array<TuningProcessor::Ptr>                  Arr;
    typedef Array<TuningProcessor::Ptr, CriticalSection> CSArr;
    
    TuningProcessor();
    ~TuningProcessor();
    
    void setGlobalOffset(float offset) { globalOffset = offset;}
    const float getGlobalOffset(void) const noexcept {return globalOffset;}
    
    //returns offsets
    float getOffset(int midiNoteNumber, TuningSystem tuning, PitchClass basepitch) const;
    
    //for calculating adaptive tuning
    void keyOn(int midiNoteNumber);
    
private:
    
    /* Array of all the default tunings
        Just:       1/1,    16/15,  9/8,    6/5,   5/4,    4/3,    7/5,    3/2,    8/5,    5/3,    7/4,    15/8
        Partial:    1/1,    16/15,  9/8,    7/6,   5/4,    4/3,    11/8,   3/2,    13/8,   5/3,    7/4,    11/6
        Duodene:    1/1,    16/15,  9/8,    6/5,   5/4,    4/3,    45/32,  3/2,    8/5,    5/3,    16/9,   15/8
        Otonal:     1/1,    17/16,  9/8,    19/16, 5/4,    21/16,  11/8,   3/2,    13/8,   27/16,  7/4,    15/8
        Utonal:     1/1,    16/15,  8/7,    32/27, 16/13,  4/3,    16/11,  32/21,  8/5,    32/19,  16/9,   32/17
    */
    Array<Array<float>> tuningLibrary;
    Array<float> tEqualTuning       = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
    Array<float> tJustTuning        = {0., .117313, .039101,  .156414, -.13686, -.019547, -.174873, .019547, .136864, -.15641, -.311745, -.11731};
    Array<float> tPartialTuning     = {0., .117313, .039101, -.331291, -.13686, -.019547, -.486824, .019547, .405273, -.15641, -.311745, -.506371};
    Array<float> tDuodeneTuning     = {0., .117313, .039101, .156414, -.13686, -.019547, -.097763, .019547, .136864, -.15641, -.039101, -.11731};
    Array<float> tOtonalTuning      = {0., .049553, .039101, -.02872, -.13686, -.292191, -.486824, .019547, .405273, .058647, -.311745, -.11731};
    Array<float> tUtonalTuning      = {0., .117313, .311745, .156414, -.405273, -.019547, .486824, .292191, .136864, .024847, -.039101,  -.049553};

    
    //adaptive tuning functions
    float   intervalToRatio(float interval) const { return mtof(interval + 60.) / mtof(60.); }
    float   adaptiveTuning_calculateRatio(int midiNoteNumber);
    void    adaptiveTuning_newNote(int midiNoteNumber, TuningSystem tuningType);
    float   adaptiveTuning_calculate(int midiNoteNumber, int anchored);
    void    adaptiveTuning_clusterTimer();
    
    /*
     60				 	=>  int   	adaptiveTuning_fundamentalNote; //moves with adaptive tuning
     Std.mtof(60.) 		=>  float 	adaptiveTuning_fundamentalFreq;
     60				 	=>  int 	adaptiveTuning_anchorFundamental; //fixed; when in anchored mode, this determines where new fundamentals are initially tuned
     partialTuning 		@=> float 	adaptiveTuning_anchorScale[];
     partialTuning 		@=> float 	adaptiveTuning_intervalScale[];
     0 					=>  int 	adaptiveTuning_reset;
     [60.,Std.mtof(60.)] @=> float 	adaptive_lastNote[]; //remembers last key played and frequency sounded
     
     500::ms 			=> dur adaptiveTuning_clusterThresh;
     0::ms   			=> dur adaptiveTuning_clusterTime;
     spork 				~ adaptiveTuning_clusterTimer();
     
     4 					=> int adaptiveTuning_history;
     0 					=> int adaptiveTuning_historyCounter;
     0 					=> int adaptiveTuning_inversional; //set intervals so, say, down a seventh is the same as up a seventh (instead of being same as up a step and then transposed down an octave).
     
     float 				lastNote[128];
     for(0=>int i;i<128;i++) 60.0 => lastNote[i];
     */
    
    JUCE_LEAK_DETECTOR(TuningProcessor);
};


#endif  // TUNING_H_INCLUDED
