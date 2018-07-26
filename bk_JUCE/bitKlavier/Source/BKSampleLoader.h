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

class BKSampleLoader : public Thread
{
public:
    BKSampleLoader(BKAudioProcessor& p):
    processor(p),
    Thread("sample_loader")
    {
        
    }
    
    ~BKSampleLoader(void)
    {
    }
private:
    
    // Sample loading.
    AudioFormatManager formatManager;
    WavAudioFormat wavFormat;
    ScopedPointer<AudioFormatReader> sampleReader;
    ScopedPointer<AudioSampleBuffer> sampleBuffer;
    
    void run(void) override;
    
    void loadSoundfontFromFile(File sfzFile);
    void loadMainPianoSamples(BKSampleLoadType type);
    void loadResonanceReleaseSamples(void);
    void loadHammerReleaseSamples(void);
    void loadPedalSamples(void);
    
    BKAudioProcessor& processor;
  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKSampleLoader)
};


#endif  // BKSAMPLELOADER_H_INCLUDED
