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

class BKAudioProcessor;

class BKSampleLoader
{
public:
    void loadMainPianoSamples(BKAudioProcessor& processor, BKSampleLoadType type);
    void loadResonanceReleaseSamples(BKAudioProcessor& processor);
    void loadHammerReleaseSamples(BKAudioProcessor& processor);
    
private:
    
    // Sample loading.
    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReader> sampleReader;
    ScopedPointer<AudioSampleBuffer> sampleBuffer;
  
    JUCE_LEAK_DETECTOR(BKSampleLoader)
};


#endif  // BKSAMPLELOADER_H_INCLUDED
