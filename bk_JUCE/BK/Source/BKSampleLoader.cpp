/*
  ==============================================================================

    BKSampleLoader.cpp
    Created: 2 Dec 2016 1:25:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKSampleLoader.h"

#include "BKPianoSampler.h"


String notes[4] = {"A","C","D#","F#"};

void BKSampleLoader::loadMainPianoSamples(BKSynthesiser *synth, int numLayers)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    // 88 or more seems to work well
    for (int i = 0; i < 150; i++)
    {
        synth->addVoice(new BKPianoSamplerVoice());
    }
    
    synth->clearSounds();
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j > 0)) continue;
            
            for (int k = 0; k < numLayers; k++)
            {
                
                String temp = path;
                temp.append(notes[j],3);
                temp.append(String(i),3);
                temp.append("v",2);
                
                if (numLayers == 8)
                {
                    temp.append(String(((k*2)+1)),3);
                }
                else if (numLayers == 4)
                {
                    temp.append(String(((k*4)+2)),3);
                }
                else if (numLayers == 2)
                {
                    temp.append(String(k*7+8),3);
                }
                else
                {
                    
                }
                
                temp.append(".wav",5);
                File file(temp);
                
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk()) {
                    
                    DBG("file opened OK: " + file.getFileName());
                    
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
                    else
                    {
                        
                    }
                    
                    
                    double sourceSampleRate = sampleReader->sampleRate;
                    const int numChannels = sampleReader->numChannels;
                    uint64 maxLength;
                    
                    if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                        maxLength = 0;
                        
                    } else {
                        maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        
                        BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                        
                        synth->addSound(new BKPianoSamplerSound(soundName,
                                                                        newBuffer,
                                                                        maxLength,
                                                                        sourceSampleRate,
                                                                        noteRange,
                                                                        root,
                                                                        velocityRange));
                    }
                    
                    
                    
                    
                    
                } else {
                    DBG("file not opened OK: " + temp);
                }
                
            }
            
        }
    }
}

void BKSampleLoader::loadResonanceReleaseSamples(BKSynthesiser *synth)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    for (int i = 0; i < 88; i++)
    {
        synth->addVoice(new BKPianoSamplerVoice() );
    }
    
    synth->clearSounds();
    
    //load release resonance samples
    for (int i = 0; i < 7; i++) {       //i => octave
        for (int j = 0; j < 4; j++) {   //j => note name
            
            if ((i == 0) && (j > 0)) continue;
            if ((i == 6) && (j != 1) && (j != 2) ) continue;
            
            for (int k = 0; k < 3; k++) //k => velocity layer
            {
                String temp = path;
                temp += "harm";
                if(k==0) temp += "V3";
                else if(k==1) temp += "S";
                else if(k==2) temp += "L";
                temp += notes[j];
                temp += std::to_string(i);
                temp += ".wav";
                
                File file(temp);
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk()) {
                    
                    DBG("file opened OK: " + file.getFileName());
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
                        
                        BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                        
                        //DBG("added resonance: " + std::to_string(noteRange.toInteger()) + " " + std::to_string(root) + " " + std::to_string(velocityRange.toInteger()) );
                        synth->addSound(new BKPianoSamplerSound(soundName,
                                                                               newBuffer,
                                                                               maxLength,
                                                                               sourceSampleRate,
                                                                               noteRange,
                                                                               root,
                                                                               velocityRange));
                    }
                }
                else
                {
                    DBG("file not opened OK: " + temp);
                }
            }
        }
    }
}

void BKSampleLoader::loadHammerReleaseSamples(BKSynthesiser *synth)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    for (int i = 0; i < 88; i++)
    {
        synth->addVoice(new BKPianoSamplerVoice() );
    }
    
    synth->clearSounds();
    
    //load hammer release samples
    for (int i = 1; i <= 88; i++) {
        
        String temp = path;
        temp += "rel";
        temp += std::to_string(i);
        temp += ".wav";
        
        File file(temp);
        FileInputStream inputStream(file);
        
        if (inputStream.openedOk()) {
            
            DBG("file opened OK: " + file.getFileName());
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
                
                BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                
                synth->addSound(new BKPianoSamplerSound(soundName,
                                                                    newBuffer,
                                                                    maxLength,
                                                                    sourceSampleRate,
                                                                    noteRange,
                                                                    root,
                                                                    velocityRange));
            }
        } else {
            DBG("file not opened OK: " + temp);
        }
    }
    
}
