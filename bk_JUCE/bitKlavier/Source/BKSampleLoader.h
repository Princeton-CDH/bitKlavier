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

class BKSampleLoader : public ThreadPoolJob
{
public:
    BKSampleLoader(BKAudioProcessor& p, BKSampleLoadType sampleType, String soundfont, int instrument, int soundSetId):
    ThreadPoolJob("sample_loader"),
    processor(p),
    loadingSampleType(sampleType),
    loadingSoundfont(soundfont),
    loadingInstrument(instrument),
    loadingSoundSetId(soundSetId)
    {
        int numSamplesPerLayer = 29;
        int numHarmSamples = 69;
        int numResSamples = 88;
        
        progressInc =
        1.0f / ((loadingSampleType == BKLoadHeavy)  ? (numSamplesPerLayer * 8 + (numResSamples + numHarmSamples)) :
                              (loadingSampleType == BKLoadMedium) ? (numSamplesPerLayer * 4) :
                              (loadingSampleType == BKLoadLite)   ? (numSamplesPerLayer * 2) :
                              (loadingSampleType == BKLoadLitest) ? (numSamplesPerLayer * 1) : 0.0);
    }
    
    ~BKSampleLoader(void)
    {
        
    }
private:
    
    BKAudioProcessor& processor;
    // Sample loading.
    AudioFormatManager formatManager;
    WavAudioFormat wavFormat;
    std::unique_ptr<AudioFormatReader> sampleReader;
    std::unique_ptr<AudioSampleBuffer> sampleBuffer;
    
    BKSampleLoadType loadingSampleType;
    String loadingSoundfont;
    int loadingInstrument;
    int loadingSoundSetId;
    
    double progressInc;
    
    JobStatus runJob(void) override;
    
    void loadSoundfontFromFile(File sfzFile);
    void loadMainPianoSamples(BKSampleLoadType type);
    void loadResonanceReleaseSamples(void);
    void loadHammerReleaseSamples(void);
    void loadPedalSamples(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKSampleLoader)
};


#endif  // BKSAMPLELOADER_H_INCLUDED
