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

void BKSampleLoader::loadSoundfontFromFile(File sfzFile)
{
    BKSynthesiser* synth = &processor.mainPianoSynth;
    
    processor.progress = 0.0;
    processor.currentSoundfont = sfzFile.getFullPathName();
    
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    String ext = sfzFile.getFileExtension();
    
    synth->clearVoices();
    synth->clearSounds();
    
    for (int i = 0; i < 300; ++i)
    {
        synth->addVoice(new BKPianoSamplerVoice(processor.gallery->getGeneralSettings()));
    }
    
    bool isSF2 = false;
    
    ScopedPointer<sfzero::SF2Sound>     sf2sound;
    ScopedPointer<sfzero::SF2Reader>    sf2reader;
    ScopedPointer<sfzero::Sound>     sfzsound;
    ScopedPointer<sfzero::Reader>    sfzreader;
    
    if      (ext == ".sf2")
    {
        isSF2 = true;
        sf2sound   = new sfzero::SF2Sound(sfzFile);
        
        sf2sound->loadRegions(processor.currentInstrument);
        sf2sound->loadSamples(&formatManager);
        
        processor.currentInstrumentName = sf2sound->subsoundName(processor.currentInstrument);
        
        processor.instrumentNames.clear();
        for (int i = 0; i < sf2sound->numSubsounds(); i++)
        {
            processor.instrumentNames.add(sf2sound->subsoundName(i));
        }
        
        processor.regions.clear();
        processor.regions = sf2sound->getRegions();
        processor.progressInc = 1.0 / processor.regions.size();
    }
    else if (ext == ".sfz")
    {
        sfzsound   = new sfzero::Sound(sfzFile);
        
        processor.currentInstrument = 0;

        // POSSIBLY STILL DOUBLE LOADING SAMPLES? 
        sfzsound->loadRegions(processor.currentInstrument);
        sfzsound->loadSamples(&formatManager, &processor.progress);
        
        processor.currentInstrumentName = sfzsound->subsoundName(processor.currentInstrument);
        
        processor.instrumentNames.clear();
        for (int i = 0; i < sfzsound->numSubsounds(); i++)
        {
            processor.instrumentNames.add(sfzsound->subsoundName(i));
        }
        
        
        processor.regions.clear();
        processor.regions = sfzsound->getRegions();
        processor.progress = 0.0;
        processor.progressInc = 1.0 / processor.regions.size();

    }
    else    return;

    int count = 0;
    for (auto region : processor.regions)
    {
        processor.progress += processor.progressInc;
        
        int64 sampleStart, sampleLength;
        
        if (isSF2)
        {
            sampleStart = region->offset;
            sampleLength = region->end - sampleStart;
        }
        else
        {
            sampleStart = 0;
            sampleLength = region->sample->getSampleLength();
        }
   
        double sourceSampleRate = region->sample->getSampleRate();

        AudioSampleBuffer* sourceBuffer = region->sample->getBuffer();
        
        BKReferenceCountedBuffer::Ptr buffer = new BKReferenceCountedBuffer(region->sample->getShortName(), 1, (int)sampleLength);
        
        AudioSampleBuffer* destBuffer = buffer->getAudioSampleBuffer();
        
        destBuffer->copyFrom(0, 0, sourceBuffer->getReadPointer(0, sampleStart), (int)sampleLength);
        
        // WEIRD thing where sample metadata defines loop point instead of the sfz format
        if (!isSF2 && (region->loop_mode == 0))
        {
            if ((region->sample->getLoopStart() > 0) &&
                (region->sample->getLoopEnd() > 0))
            {
                region->loop_mode = sfzero::Region::loop_continuous;
                region->loop_start = region->sample->getLoopStart();
                region->loop_end = region->sample->getLoopEnd();
            }
        }
        
        DBG("~ ~ ~ ~ region " + String(count++) + " ~ ~ ~ ~ ~ ~");
        DBG("sample: " + region->sample->getShortName());
        DBG("offset: " + String(region->offset));
        region->end             -= region->offset;
        region->loop_start      -= region->offset;
        region->loop_end        -= region->offset;
        region->offset           = 0;
        
        if (region->pitch_keycenter < 0) region->pitch_keycenter = region->lokey;
        
        if (region->lokey == region->hikey)
        {
            if (region->lokey != region->pitch_keycenter)
            {
                region->transpose = region->lokey - region->pitch_keycenter;
            }
        }
    
        
        DBG("transp: " + String(region->transpose) + "   keycenter: " + String(region->pitch_keycenter) + " keytrack: " + String(region->pitch_keytrack));
        
        DBG("end: " + String(region->end) + "   ls: " + String(region->loop_start) + "   le: " + String(region->loop_end) + "   keyrange: " + String(region->lokey) + "-" + String(region->hikey) + "   velrange: " + String(region->lovel) + "-" + String(region->hivel));
        
        DBG("trigger: " +String(region->trigger));
        DBG("pedal needed: " + String((int)region->pedal));
        
        
        
        int nbits = region->hikey - region->lokey;
        int vbits = region->hivel - region->lovel;
        BigInteger nrange; nrange.setRange(region->lokey, nbits+1, true);
        BigInteger vrange; vrange.setRange(region->lovel, vbits+1, true);
        
        synth->addSound(new BKPianoSamplerSound(region->sample->getShortName(),
                                                        buffer,
                                                        sampleLength,
                                                        sourceSampleRate,
                                                        nrange,
                                                        region->pitch_keycenter,
                                                        region->transpose,
                                                        vrange,
                                                        region,isSF2));
        
    }
    
   
    

    processor.didLoadMainPianoSamples = true;
}



void BKSampleLoader::run(void)
{
    BKSampleLoadType type = processor.currentSampleType;
    
    processor.updateState->pianoSamplesAreLoading = true;
    
    String soundfont = processor.currentSoundfont;
    
    EXIT_CHECK;
    
    if (type == BKLoadSoundfont)
    {
        if (soundfont.startsWith("default.sf"))
        {
            int which = soundfont.fromLastOccurrenceOf("default.sf", false, false).getIntValue();
            
            File file;
            
            file = file.getSpecialLocation(File::invokedExecutableFile).getParentDirectory().getChildFile("soundfonts");

            switch (which)
            {
                case 0:
                    file = file.getChildFile("rhodes.sf2"); break;
                case 1:
                    file = file.getChildFile("harpsichord.sf2"); break;
                case 2:
                    file = file.getChildFile("kikazdrums.sf2"); break;
                case 3:
                    file = file.getChildFile("saw.sf2"); break;
                case 4:
                    file = file.getChildFile("ebass").getChildFile("Electric Bass.sfz"); break;
                default:
                    file = file.getChildFile("saw.sf2"); break;
            }
            
            loadSoundfontFromFile(file);
            
            file.deleteFile();
        }
        else
        {
            File file (soundfont);
            
            loadSoundfontFromFile(file);
        }
    }
    else
    {
    
        if (processor.currentSampleType == BKLoadSoundfont)
        {
            processor.currentSampleType = BKLoadLite;
            type = BKLoadLite;
        }
        
        loadMainPianoSamples(type);
        
        EXIT_CHECK;
        
        processor.didLoadMainPianoSamples = true;
        
        if (!processor.didLoadHammersAndRes && type == BKLoadHeavy)
        {
            processor.didLoadHammersAndRes = true;
            loadHammerReleaseSamples();
            
            EXIT_CHECK;
            
            loadResonanceReleaseSamples();
            
            loadPedalSamples();
        }
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
#endif
#if JUCE_MAC
    bkSamples = bkSamples.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("samples");
#endif
#if JUCE_LINUX || JUCE_WINDOWS
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
                                                                        0,
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
#endif
#if JUCE_MAC
    bkSamples = bkSamples.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("samples");
#endif
#if JUCE_LINUX || JUCE_WINDOWS
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

                        synth->addSound(new BKPianoSamplerSound(soundName,
                                                                newBuffer,
                                                                maxLength,
                                                                sourceSampleRate,
                                                                noteRange,
                                                                root, 0,
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
#endif
#if JUCE_MAC
    bkSamples = bkSamples.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("samples");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
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
                                                                    root, 0,
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

void BKSampleLoader::loadPedalSamples(void)
{
    WavAudioFormat wavFormat;
    BKSynthesiser* synth = &processor.pedalSynth;
    File bkSamples;
    
#if JUCE_IOS
    bkSamples = bkSamples.getSpecialLocation(File::invokedExecutableFile).getParentDirectory().getChildFile("samples");
#endif
#if JUCE_MAC
    bkSamples = bkSamples.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("samples");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    bkSamples = bkSamples.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("samples");
#endif
    
    synth->clearVoices();
    synth->clearSounds();
    
    for (int i = 0; i < 88; i++)    synth->addVoice(new BKPianoSamplerVoice(synth->generalSettings));
    
    //load hammer release samples
    for (int i = 0; i < 4; i++) {
        
        String temp;
        
        if(i==0) temp = "pedalD1.wav";
        else if(i==1) temp = "pedalD2.wav";
        else if(i==2) temp = "pedalU1.wav";
        else if(i==3) temp = "pedalU2.wav";

        
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
                                                        0,
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
