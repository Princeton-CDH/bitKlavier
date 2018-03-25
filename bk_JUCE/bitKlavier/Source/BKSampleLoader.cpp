/*
  ==============================================================================

    BKSampleLoader.cpp
    Created: 2 Dec 2016 1:25:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKSampleLoader.h"

#include "BKPianoSampler.h"

#include "PluginProcessor.h"


String notes[4] = {"A","C","D#","F#"};

#define EXIT_CHECK if (threadShouldExit()) { processor.updateState->pianoSamplesAreLoading = false; return; }

void BKSampleLoader::run(void)
{
    BKSampleLoadType type = processor.currentSampleType;
    
    processor.updateState->pianoSamplesAreLoading = true;
    
    EXIT_CHECK;
    
    loadMainPianoSamples(type);
    
    EXIT_CHECK;
    
    processor.didLoadMainPianoSamples = true;
    
    if (!processor.didLoadHammersAndRes && type == BKLoadHeavy)
    {
        processor.didLoadHammersAndRes = true;
        loadHammerReleaseSamples();
        
        EXIT_CHECK;
        
        loadResonanceReleaseSamples();
    }
    
    processor.updateState->pianoSamplesAreLoading = false;
}

void BKSampleLoader::loadMainPianoSamples(BKSampleLoadType type)
{
    WavAudioFormat wavFormat;
    BKSynthesiser* synth = &processor.mainPianoSynth;
    
    File bkSamples;
    
#if JUCE_IOS
    bkSamples = bkSamples.getSpecialLocation(File::invokedExecutableFile).getParentDirectory().getChildFile("samples");
#else
    bkSamples = bkSamples.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("samples");
#endif
    
    int numLayers = 0;
    
    if      (type == BKLoadLitest)      numLayers = 1;
    else if (type == BKLoadLite)        numLayers = 2;
    else if (type == BKLoadMedium)      numLayers = 4;
    else if (type == BKLoadHeavy)       numLayers = 8;
    
    synth->clearVoices();
    synth->clearSounds();
    
    // 88 or more seems to work well
    for (int i = 0; i < 300; i++)   synth->addVoice(new BKPianoSamplerVoice(processor.gallery->getGeneralSettings()));
    
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j > 0)) continue;
            
            for (int k = 0; k < numLayers; k++)
            {
                
                //String temp = path;
                String temp;
                temp.append(notes[j],3);
                temp.append(String(i),3);
                temp.append("v",2);
                
                if (numLayers == 8)
                {
                    temp.append(String(((k*2)+1)),3);
                }
                else if (numLayers == 4)
                {
                    temp.append(String(((k*4)+3)),3);
                }
                else if (numLayers == 2)
                {
                    temp.append(String(k*8+7),3);
                }
                else if (numLayers == 1)
                {
                    temp += "13";
                }
                
                temp.append(".wav",5);
                //File file(temp);
                
                File file(bkSamples.getChildFile(temp));
                
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk())
                {
                    String soundName = file.getFileName();
                    
                    sampleReader = wavFormat.createReaderFor(new FileInputStream(file), true);
                    
                    BigInteger noteRange;
                    
                    int root = 0;
                    if (j == 0) {
                        root = (9+12*i) + 12;
                        if (i == 7) {
                            // High C.
                            noteRange.setRange(root-1,5,true);
                        }else {
                            noteRange.setRange(root-1,3,true);
                        }
                    } else if (j == 1) {
                        root = (0+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 2) {
                        root = (3+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 3) {
                        root = (6+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    } else {
                        
                    }
                    
                    BigInteger velocityRange;
                    if (numLayers == 8)
                    {
                        velocityRange.setRange(aVelocityThresh_Eight[k], (aVelocityThresh_Eight[k+1] - aVelocityThresh_Eight[k]), true);
                    }
                    else if (numLayers == 4)
                    {
                        velocityRange.setRange(aVelocityThresh_Four[k], (aVelocityThresh_Four[k+1] - aVelocityThresh_Four[k]), true);
                    }
                    else if (numLayers == 2)
                    {
                        velocityRange.setRange(aVelocityThresh_Two[k], (aVelocityThresh_Two[k+1] - aVelocityThresh_Two[k]), true);
                    }
                    else if (numLayers == 1)
                    {
                        velocityRange.setRange(aVelocityThresh_One[k], (aVelocityThresh_One[k+1] - aVelocityThresh_One[k]), true);
                    }
                    
                    double sourceSampleRate = sampleReader->sampleRate;
                    const int numChannels = sampleReader->numChannels;
                    uint64 maxLength;
                    
                    if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                        maxLength = 0;
                        
                    } else {
                        maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        
                        BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),(int)maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);

                        synth->addSound(new BKPianoSamplerSound(soundName,
                                                                        newBuffer,
                                                                        maxLength,
                                                                        sourceSampleRate,
                                                                        noteRange,
                                                                        root,
                                                                        velocityRange));
                    }
                    
                    
                    
                    processor.progress += processor.progressInc;
                    DBG(soundName+": " + String(processor.progress));
                    
                }
                else
                {
                    DBG("file not opened OK: " + temp);
                }
                
            }
            
        }
    }
}

void BKSampleLoader::loadResonanceReleaseSamples(void)
{
    WavAudioFormat wavFormat;
    BKSynthesiser* synth = &processor.resonanceReleaseSynth;
    
    File bkSamples;
    
#if JUCE_IOS
    bkSamples = bkSamples.getSpecialLocation(File::invokedExecutableFile).getParentDirectory().getChildFile("samples");
#else
    bkSamples = bkSamples.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("samples");
#endif
    
    synth->clearVoices();
    synth->clearSounds();
    
    for (int i = 0; i < 88; i++)    synth->addVoice(new BKPianoSamplerVoice(processor.gallery->getGeneralSettings()));

    //load release resonance samples
    for (int i = 0; i < 7; i++) {       //i => octave
        for (int j = 0; j < 4; j++) {   //j => note name
            
            if ((i == 0) && (j > 0)) continue;
            if ((i == 6) && (j != 1) && (j != 2) ) continue;
            
            for (int k = 0; k < 3; k++) //k => velocity layer
            {
                String temp;
                temp += "harm";
                if(k==0) temp += "V3";
                else if(k==1) temp += "S";
                else if(k==2) temp += "L";
                temp += notes[j];
                temp += String(i);
                temp += ".wav";
                
                //File file(temp);
                File file(bkSamples.getChildFile(temp));
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk()) {
                    String soundName = file.getFileName();
                    sampleReader = wavFormat.createReaderFor(new FileInputStream(file), true);
                    
                    //keymap assignment
                    BigInteger noteRange;
                    int root = 0;
                    if (j == 0)
                    {
                        root = (9+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 1)
                    {
                        root = (0+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 2)
                    {
                        root = (3+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 3)
                    {
                        root = (6+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    }
                    
                    //velocity switching
                    BigInteger velocityRange;
                    velocityRange.setRange(aResonanceVelocityThresh[k], (aResonanceVelocityThresh[k+1] - aResonanceVelocityThresh[k]), true);
                    
                    //load the sample, add to synth
                    double sourceSampleRate = sampleReader->sampleRate;
                    const int numChannels = sampleReader->numChannels;
                    uint64 maxLength;
                    
                    if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0)
                    {
                        maxLength = 0;
                    }
                    else
                    {
                        maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        
                        BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels), (int)maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);
                        
                        //DBG("added resonance: " + String(noteRange.toInteger()) + " " + String(root) + " " + String(velocityRange.toInteger()) );
                        synth->addSound(new BKPianoSamplerSound(soundName,
                                                                               newBuffer,
                                                                               maxLength,
                                                                               sourceSampleRate,
                                                                               noteRange,
                                                                               root,
                                                                               velocityRange));
                    }
                    
                    processor.progress += processor.progressInc;
                    DBG(soundName+": " + String(processor.progress));
                }
                else
                {
                    DBG("file not opened OK: " + temp);
                }
            }
        }
    }
}

void BKSampleLoader::loadHammerReleaseSamples(void)
{
    WavAudioFormat wavFormat;
    BKSynthesiser* synth = &processor.hammerReleaseSynth;
    File bkSamples;
    
#if JUCE_IOS
    bkSamples = bkSamples.getSpecialLocation(File::invokedExecutableFile).getParentDirectory().getChildFile("samples");
#else
    bkSamples = bkSamples.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("samples");
#endif
    
    synth->clearVoices();
    synth->clearSounds();
    
    for (int i = 0; i < 88; i++)    synth->addVoice(new BKPianoSamplerVoice(processor.gallery->getGeneralSettings()));
    
    //load hammer release samples
    for (int i = 1; i <= 88; i++) {
        
        String temp;
        temp += "rel";
        temp += String(i);
        temp += ".wav";
        
        //File file(temp);
        File file(bkSamples.getChildFile(temp));
        FileInputStream inputStream(file);
        
        if (inputStream.openedOk()) {
            String soundName = file.getFileName();
            sampleReader = wavFormat.createReaderFor(new FileInputStream(file), true);
            
            BigInteger noteRange;
            noteRange.setRange(20 + i, 1, true);
            
            BigInteger velocityRange;
            velocityRange.setRange(0, 128, true);
            
            int root = 20 + i;
            
            double sourceSampleRate = sampleReader->sampleRate;
            const int numChannels = sampleReader->numChannels;
            uint64 maxLength;
            
            if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                maxLength = 0;
                
            } else {
                maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                
                BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels), (int)maxLength);
                sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);
                
                synth->addSound(new BKPianoSamplerSound(soundName,
                                                                    newBuffer,
                                                                    maxLength,
                                                                    sourceSampleRate,
                                                                    noteRange,
                                                                    root,
                                                                    velocityRange));
            }
            processor.progress += processor.progressInc;
            DBG(soundName+": " + String(processor.progress));
        }
        else
        {
            DBG("file not opened OK: " + temp);
        }
    }
    
}
