/*
  ==============================================================================

    BKSampleLoader.h
    Created: 2 Dec 2016 1:25:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKSAMPLELOADER_H_INCLUDED
#define BKSAMPLELOADER_H_INCLUDED

#include "BKUtilities.h"

#include "BKSynthesiser.h"


class BKSampleLoader
{
public:
    void loadMainPianoSamples(BKSynthesiser *synth, int numLayers);
    void loadResonanceReleaseSamples(BKSynthesiser *synth);
    void loadHammerReleaseSamples(BKSynthesiser *synth);
    
private:
    
    // Sample loading.
    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReader> sampleReader;
    ScopedPointer<AudioSampleBuffer> sampleBuffer;
  
    JUCE_LEAK_DETECTOR(BKSampleLoader)
};


#endif  // BKSAMPLELOADER_H_INCLUDED
