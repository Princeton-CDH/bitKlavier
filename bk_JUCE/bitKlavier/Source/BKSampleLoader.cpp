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


// Order of these arrays matters
String bkNotes[4] = { "C", "D#", "F#", "A" };

String completeNotes[17] = {
    "C", "C#", "Db", "D", "D#", "Eb", "E", "F", "F#",
    "Gb", "G", "G#", "Ab", "A", "A#", "Bb", "B"
};

static inline int noteNameToRoot(String name)
{
    int root = 0;
    if (name[0] == 'C') root = 0;
    else if (name[0] == 'D') root = 2;
    else if (name[0] == 'E') root = 4;
    else if (name[0] == 'F') root = 5;
    else if (name[0] == 'G') root = 7;
    else if (name[0] == 'A') root = 9;
    else if (name[0] == 'B') root = 11;
    
    if (name[1] == '#') root++;
    else if (name[1] == 'b') root--;
    
    root += 12 * name.getTrailingIntValue() + 12;
    
    return root;
}

#define EXIT_CHECK if (shouldExit()) \
{ \
if (processor.loader.getNumJobs() == 1) processor.updateState->pianoSamplesAreLoading = false; \
/*if (processor.loader.getNumJobs() == 1) processor.touchThread.startThread();*/ \
return jobStatus; \
}

BKSampleLoader::JobStatus BKSampleLoader::runJob(void)
{
    BKSampleLoadType type = loadingSampleType;
    
    processor.updateState->pianoSamplesAreLoading = true;
    
    String soundfont = loadingSoundfont;
    
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
                    file = file.getChildFile("jRhodes3b-stereo.sf2"); break;
                case 1:
                    // This is not actually the Blanchet-1720 sf that is installed with the desktop version
                    // It is a much smaller harpsichord sf, but named the same for loading purposes
                    file = file.getChildFile("Blanchet-1720.sf2"); break;
                case 2:
                    file = file.getChildFile("1276-The KiKaZ DrUmZ.sf2"); break;
                case 3:
                    file = file.getChildFile("SCC Saw 1.sf2"); break;
                case 4:
                    file = file.getChildFile("ebass").getChildFile("Electric Bass.sfz"); break;
                case 5 :
                    file = file.getChildFile("acoust_kits_1-4.sf2"); break;
                case 6 :
                    file = file.getChildFile("Orgue de salon.sf2"); break;
                default:
                    file = file.getChildFile("SCC Saw 1.sf2"); break;
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
    else if (type == BKLoadCustom)
    {
        processor.instrumentNames.set(loadingSoundSetId, StringArray());
        
        loadCustomSamples();
        
        EXIT_CHECK;
        
        processor.didLoadMainPianoSamples = true;
    }
    else
    {
        processor.instrumentNames.set(loadingSoundSetId, StringArray());
        
        // device type
        DBG("device type = " + SystemStats::getDeviceDescription());
        String deviceType = SystemStats::getDeviceDescription();
        if (deviceType.contains("iPhone")) DBG("is iPhone");
        if (deviceType.contains("iPad")) DBG("is iPad");
        
        // older iPhones can't handle memory of lots of samples...
        if (deviceType.contains("iPhone")) loadMainPianoSamples(BKLoadLitest);
        else loadMainPianoSamples(type);
        
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
    
    processor.progress -= 1.0f;
    if (processor.loader.getNumJobs() == 1) processor.updateState->pianoSamplesAreLoading = false;
    jobStatus = jobHasFinished;
    
    //    if (processor.loader.getNumJobs() == 1) processor.touchThread.startThread();
    return jobHasFinished;
}

BKSampleLoader::JobStatus BKSampleLoader::loadMainPianoSamples(BKSampleLoadType type)
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
    bkSamples = bkSamples.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("samples");
#endif
    
    int numLayers = 0;
    
    if (type == BKLoadLitest)      numLayers = 1;
    else if (type == BKLoadLite)        numLayers = 2;
    else if (type == BKLoadMedium)      numLayers = 4;
    else if (type == BKLoadHeavy)       numLayers = 8;
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            float dBFSBelow = -100.f;
            for (int k = 0; k < numLayers; k++)
            {
                EXIT_CHECK;
                
                //String temp = path;
                String temp;
                temp.append(bkNotes[j],3);
                temp.append(String(i),3);
                temp.append("v",2);
                
                if (numLayers == 8)
                {
                    temp.append(String((k*2)+1),3);
                }
                else if (numLayers == 4)
                {
                    temp.append(String((k*4)+3),3);
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
                    
                    MemoryMappedAudioFormatReader* memoryMappedReader;
                    if (memoryMappingEnabled)
                    {
                        memoryMappedReader = wavFormat.createMemoryMappedReader(new FileInputStream(file));
                    }
                    else
                    {
                        sampleReader = std::unique_ptr<AudioFormatReader> (wavFormat.createReaderFor(new FileInputStream(file), true));
                    }
                    
                    BigInteger noteRange;
                    
                    int root = (12 * i) + noteNameToRoot(bkNotes[j]) + 12;
                    if (i == 7 && j == 3) noteRange.setRange(root-1, 5, true); //High A
                    else noteRange.setRange(root-1, 3, true);
                    
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
                    
                    if (memoryMappingEnabled)
                    {
                        double sourceSampleRate = memoryMappedReader->sampleRate;
                        uint64 maxLength;
                        if (sourceSampleRate <= 0 || memoryMappedReader->lengthInSamples <= 0) {
                            maxLength = 0;
                            
                        } else {
                            maxLength = jmin((uint64)memoryMappedReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                            if (memoryMappedReader->mapEntireFile())
                            {
                                BKPianoSamplerSound* newSound =
                                new BKPianoSamplerSound(soundName, memoryMappedReader,
                                                        maxLength, sourceSampleRate,
                                                        noteRange, root,
                                                        0, velocityRange,
                                                        k+1, numLayers,
                                                        dBFSBelow);
                                dBFSBelow = newSound->getDBFSLevel();
                                synth->addSound(loadingSoundSetId, newSound);
                            }
                            else DBG("File mapping failed");
                        }
                    }
                    else
                    {
                        double sourceSampleRate = sampleReader->sampleRate;
                        const int numChannels = sampleReader->numChannels;
                        uint64 maxLength;
                        if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                            maxLength = 0;
                            
                        } else {
                            maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                            
                            BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),(int)maxLength);
                            sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);
                            BKPianoSamplerSound* newSound =
                            new BKPianoSamplerSound(soundName, newBuffer,
                                                    maxLength, sourceSampleRate,
                                                    noteRange, root,
                                                    0, velocityRange,
                                                    k+1, numLayers,
                                                    dBFSBelow);
                            dBFSBelow = newSound->getDBFSLevel();
                            synth->addSound(loadingSoundSetId, newSound);
                        }
                    }
                    
                    processor.progress += progressInc;
                    //DBG(soundName+": " + String(processor.progress));
                    
                }
                else
                {
                    DBG("File not found: " + temp);
                }
                
            }
            
        }
    }
    
    processor.didLoadMainPianoSamples = true;
    
    return jobStatus;
}

BKSampleLoader::JobStatus BKSampleLoader::loadResonanceReleaseSamples(void)
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
    bkSamples = bkSamples.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("samples");
#endif
    
    //load release resonance samples
    for (int i = 0; i < 7; i++) {       //i => octave
        for (int j = 0; j < 4; j++) {   //j => note name
            
            float dBFSBelow = -100.f;
            for (int k = 0; k < 3; k++) //k => velocity layer
            {
                EXIT_CHECK;
                String temp;
                temp += "harm";
                if(k==0) temp += "V3";
                else if(k==1) temp += "S";
                else if(k==2) temp += "L";
                temp += bkNotes[j];
                temp += String(i);
                temp += ".wav";
                
                //File file(temp);
                File file(bkSamples.getChildFile(temp));
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk()) {
                    
                    String soundName = file.getFileName();
                    
                    MemoryMappedAudioFormatReader* memoryMappedReader;
                    if (memoryMappingEnabled)
                    {
                        memoryMappedReader = wavFormat.createMemoryMappedReader(new FileInputStream(file));
                    }
                    else
                    {
                        sampleReader = std::unique_ptr<AudioFormatReader> (wavFormat.createReaderFor(new FileInputStream(file), true));
                    }
                    
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
                    if (memoryMappingEnabled)
                    {
                        double sourceSampleRate = memoryMappedReader->sampleRate;
                        uint64 maxLength;
                        if (sourceSampleRate <= 0 || memoryMappedReader->lengthInSamples <= 0) {
                            maxLength = 0;
                            
                        } else {
                            maxLength = jmin((uint64)memoryMappedReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                            if (memoryMappedReader->mapEntireFile())
                            {
                                BKPianoSamplerSound* newSound =
                                new BKPianoSamplerSound(soundName, memoryMappedReader,
                                                        maxLength, sourceSampleRate,
                                                        noteRange, root,
                                                        0, velocityRange,
                                                        k+1, 3,
                                                        dBFSBelow);
                                dBFSBelow = newSound->getDBFSLevel();
                                synth->addSound(loadingSoundSetId, newSound);
                            }
                            else DBG("File mapping failed");
                        }
                    }
                    else
                    {
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
                            BKPianoSamplerSound* newSound =
                            new BKPianoSamplerSound(soundName, newBuffer,
                                                    maxLength, sourceSampleRate,
                                                    noteRange, root,
                                                    0, velocityRange,
                                                    k+1, 3,
                                                    dBFSBelow);
                            dBFSBelow = newSound->getDBFSLevel();
                            synth->addSound(loadingSoundSetId, newSound);
                        }
                    }
                    
                    processor.progress += progressInc;
                    //DBG(soundName+": " + String(processor.progress));
                }
                else
                {
                    DBG("File not found: " + temp);
                }
            }
        }
    }
    return jobStatus;
}

BKSampleLoader::JobStatus BKSampleLoader::loadHammerReleaseSamples(void)
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
    bkSamples = bkSamples.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("samples");
#endif
    
    //load hammer release samples
    for (int i = 1; i <= 88; i++) {
        EXIT_CHECK;
        String temp;
        temp += "rel";
        temp += String(i);
        temp += ".wav";
        
        //File file(temp);
        File file(bkSamples.getChildFile(temp));
        FileInputStream inputStream(file);
        
        if (inputStream.openedOk()) {
            
            String soundName = file.getFileName();
            
            MemoryMappedAudioFormatReader* memoryMappedReader;
            if (memoryMappingEnabled)
            {
                memoryMappedReader = wavFormat.createMemoryMappedReader(new FileInputStream(file));
            }
            else
            {
                sampleReader = std::unique_ptr<AudioFormatReader> (wavFormat.createReaderFor(new FileInputStream(file), true));
            }
            
            BigInteger noteRange;
            noteRange.setRange(20 + i, 1, true);
            
            BigInteger velocityRange;
            velocityRange.setRange(0, 128, true);
            
            int root = 20 + i;
            
            if (memoryMappingEnabled)
            {
                double sourceSampleRate = memoryMappedReader->sampleRate;
                uint64 maxLength;
                if (sourceSampleRate <= 0 || memoryMappedReader->lengthInSamples <= 0) {
                    maxLength = 0;
                    
                } else {
                    maxLength = jmin((uint64)memoryMappedReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                    if (memoryMappedReader->mapEntireFile())
                    {
                        BKPianoSamplerSound* newSound =
                        new BKPianoSamplerSound(soundName, memoryMappedReader,
                                                maxLength, sourceSampleRate,
                                                noteRange, root,
                                                0, velocityRange,
                                                1, 1,
                                                0.f);
                        synth->addSound(loadingSoundSetId, newSound);
                    }
                    else DBG("File mapping failed");
                }
            }
            else
            {
                double sourceSampleRate = sampleReader->sampleRate;
                const int numChannels = sampleReader->numChannels;
                uint64 maxLength;
                
                if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                    maxLength = 0;
                    
                } else {
                    maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                    
                    BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels), (int)maxLength);
                    sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);
                    BKPianoSamplerSound* newSound =
                    new BKPianoSamplerSound(soundName, newBuffer,
                                            maxLength, sourceSampleRate,
                                            noteRange, root,
                                            0, velocityRange,
                                            1, 1,
                                            0.f);
                    synth->addSound(loadingSoundSetId, newSound);
                }
            }
            processor.progress += progressInc;
            //DBG(soundName+": " + String(processor.progress));
        }
        else
        {
            DBG("File not found: " + temp);
        }
    }
    return jobStatus;
}

BKSampleLoader::JobStatus BKSampleLoader::loadPedalSamples(void)
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
    bkSamples = bkSamples.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("samples");
#endif
    
    //load pedal release samples
    for (int i = 0; i < 4; i++) {
        EXIT_CHECK;
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
            
            MemoryMappedAudioFormatReader* memoryMappedReader;
            if (memoryMappingEnabled)
            {
                memoryMappedReader = wavFormat.createMemoryMappedReader(new FileInputStream(file));
            }
            else
            {
                sampleReader = std::unique_ptr<AudioFormatReader> (wavFormat.createReaderFor(new FileInputStream(file), true));
            }
            
            BigInteger noteRange;
            noteRange.setRange(20 + i, 1, true);
            
            BigInteger velocityRange;
            velocityRange.setRange(0, 128, true);
            
            int root = 20 + i;
            
            if (memoryMappingEnabled)
            {
                double sourceSampleRate = memoryMappedReader->sampleRate;
                uint64 maxLength;
                if (sourceSampleRate <= 0 || memoryMappedReader->lengthInSamples <= 0) {
                    maxLength = 0;
                    
                } else {
                    maxLength = jmin((uint64)memoryMappedReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                    if (memoryMappedReader->mapEntireFile())
                    {
                        BKPianoSamplerSound* newSound =
                        new BKPianoSamplerSound(soundName, memoryMappedReader,
                                                maxLength, sourceSampleRate,
                                                noteRange, root,
                                                0, velocityRange,
                                                1, 1,
                                                0.f);
                        synth->addSound(loadingSoundSetId, newSound);
                    }
                    else DBG("File mapping failed");
                }
            }
            else
            {
                double sourceSampleRate = sampleReader->sampleRate;
                const int numChannels = sampleReader->numChannels;
                uint64 maxLength;
                
                if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                    maxLength = 0;
                    
                } else {
                    maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                    
                    BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels), (int)maxLength);
                    sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);
                    BKPianoSamplerSound* newSound =
                    new BKPianoSamplerSound(soundName, newBuffer,
                                            maxLength, sourceSampleRate,
                                            noteRange, root,
                                            0, velocityRange,
                                            1, 1,
                                            0.f);
                    synth->addSound(loadingSoundSetId, newSound);
                }
            }
            processor.progress += progressInc;
            //DBG(soundName+": " + String(processor.progress));
        }
        else
        {
            DBG("File not found: " + temp);
        }
    }
    return jobStatus;
}

BKSampleLoader::JobStatus BKSampleLoader::loadSoundfontFromFile(File sfzFile)
{
    BKSynthesiser* synth = &processor.mainPianoSynth;
    
    processor.progress = 0.0;
    
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    String ext = sfzFile.getFileExtension();
    
    EXIT_CHECK;
    
    bool isSF2 = false;
    
    sfzero::SF2Sound::Ptr     sf2sound;
    sfzero::Sound::Ptr        sfzsound;
    
    if      (ext == ".sf2")
    {
        isSF2 = true;
        sf2sound   = new sfzero::SF2Sound(sfzFile);
        
        sf2sound->loadRegions(loadingInstrument);
        sf2sound->loadSamples(&formatManager);
        
        processor.instrumentNames.set(loadingSoundSetId, StringArray());
        for (int i = 0; i < sf2sound->numSubsounds(); i++)
        {
            processor.instrumentNames.getReference(loadingSoundSetId).add(sf2sound->subsoundName(i));
        }
        
        //        processor.regions.clear();
        //        processor.regions.ensureStorageAllocated(processor.regions.size()+1);
        processor.regions.set(loadingSoundSetId, sf2sound->getRegions());
        progressInc = 1.0 / processor.regions.getReference(loadingSoundSetId).size();
    }
    else if (ext == ".sfz")
    {
        sfzsound   = new sfzero::Sound(sfzFile);
        
        loadingInstrument = 0;
        
        // POSSIBLY STILL DOUBLE LOADING SAMPLES?
        sfzsound->loadRegions(loadingInstrument);
        sfzsound->loadSamples(&formatManager);
        
        processor.instrumentNames.set(loadingSoundSetId, StringArray());
        for (int i = 0; i < sfzsound->numSubsounds(); i++)
        {
            processor.instrumentNames.getReference(loadingSoundSetId).add(sfzsound->subsoundName(i));
        }
        
        //        processor.regions.clear();
        //        processor.regions.ensureStorageAllocated(processor.regions.size()+1);
        processor.regions.set(loadingSoundSetId, sfzsound->getRegions());
        progressInc = 1.0 / processor.regions.getReference(loadingSoundSetId).size();
    }
    else    return jobStatus;
    
    int n = 1;
    float dBFSBelow = -100.f;
    for (auto region : processor.regions.getReference(loadingSoundSetId))
    {
        EXIT_CHECK;
        processor.progress += progressInc;
        
        int64 sampleStart, sampleLength;
        
        if (isSF2)
        {
            sampleStart = region->offset;
            sampleLength = region->end - sampleStart;
        }
        else
        {
            if (region->sample == nullptr) continue;
            sampleStart = 0;
            sampleLength = region->sample->getSampleLength();
        }
        
        double sourceSampleRate = region->sample->getSampleRate();
        
        AudioSampleBuffer* sourceBuffer = region->sample->getBuffer();
        
        if (sourceBuffer == NULL) continue;
        
        // Only get the first channel of the sample because individual sample of soundfonts should supposedly
        // always be mono, but I've found some soundfont formatted as a stereo sample with audio only in the first channel
        // and if we get both channels (as in the commented block below) the audio only be in the left channel in output.
        BKReferenceCountedBuffer::Ptr buffer = new BKReferenceCountedBuffer(region->sample->getShortName(), 1, (int)sampleLength);
        
        AudioSampleBuffer* destBuffer = buffer->getAudioSampleBuffer();
        
        destBuffer->copyFrom(0, 0, sourceBuffer->getReadPointer(0, (int)sampleStart), (int)sampleLength);
        
        //        BKReferenceCountedBuffer::Ptr buffer = new BKReferenceCountedBuffer(region->sample->getShortName(), sourceBuffer->getNumChannels(), (int)sampleLength);
        //
        //        AudioSampleBuffer* destBuffer = buffer->getAudioSampleBuffer();
        //
        //        for (int i = 0; i < destBuffer->getNumChannels(); i++)
        //            destBuffer->copyFrom(i, 0, sourceBuffer->getReadPointer(i, (int)sampleStart), (int)sampleLength);
        
        
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
        
        //DBG("~ ~ ~ ~ region " + String(count++) + " ~ ~ ~ ~ ~ ~");
        //DBG("sample: " + region->sample->getShortName());
        //DBG("offset: " + String(region->offset));
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
        
        //DBG("transp: " + String(region->transpose) + "   keycenter: " + String(region->pitch_keycenter) + " keytrack: " + String(region->pitch_keytrack));
        
        //DBG("end: " + String(region->end) + "   ls: " + String(region->loop_start) + "   le: " + String(region->loop_end) + "   keyrange: " + String(region->lokey) + "-" + String(region->hikey) + "   velrange: " + String(region->lovel) + "-" + String(region->hivel));
        
        //DBG("trigger: " +String(region->trigger));
        //DBG("pedal needed: " + String((int)region->pedal));
        
        int nbits = region->hikey - region->lokey;
        int vbits = region->hivel - region->lovel;
        BigInteger nrange; nrange.setRange(region->lokey, nbits+1, true);
        BigInteger vrange; vrange.setRange(region->lovel, vbits+1, true);
        BKPianoSamplerSound* newSound =
        new BKPianoSamplerSound(region->sample->getShortName(),
                                buffer,
                                sampleLength,
                                sourceSampleRate,
                                nrange,
                                region->pitch_keycenter,
                                region->transpose,
                                vrange, n, processor.regions.size(),
                                dBFSBelow, region);
       
        DBG("NEW LAYER NUM = " + String(n));
        DBG("NEW NUM LAYERS = " + String(processor.regions.size()));
        dBFSBelow = newSound->getDBFSLevel();
        //dBFSBelow = newSound->getDBFSLevel() > dBFSBelow ? newSound->getDBFSLevel() : 0.f;
        synth->addSound(loadingSoundSetId, newSound);
        n++;
    }
    
    processor.didLoadMainPianoSamples = true;
    
    return jobStatus;
}

BKSampleLoader::JobStatus BKSampleLoader::loadCustomSamples()
{
    WavAudioFormat wavFormat;
    BKSynthesiser* mainSynth = &processor.mainPianoSynth;
    BKSynthesiser* resSynth = &processor.resonanceReleaseSynth;
    BKSynthesiser* hamSynth = &processor.hammerReleaseSynth;
    BKSynthesiser* pedalSynth = &processor.pedalSynth;
    
    File samples (loadingSoundfont);
    
    Array<int> noteRoots, resRoots;
    
    int numLayers = 0;
    int maxOctave = 0;
    int numResLayers = 0;
    int maxResOctave = 0;
    int numHammers = 0;
    int numPedal = 0;
    int numTotalSample = 0;
    
    // Figure out how many layers there
    std::regex noteReg("\\b[ABCDEFG]#*b*\\dv\\d+\\b");
    std::regex harmReg("\\bharm[ABCDEFG]#*b*\\dv\\d+\\b");
    std::regex relReg("\\brel\\d+\\b");
    std::regex pedalReg("\\bpedal[DU]\\d+\\b");
    
    for (auto iter : RangedDirectoryIterator (File (samples), false, "*.wav"))
    {
        String fileName = iter.getFile().getFileNameWithoutExtension();
        
        if (std::regex_search(fileName.toStdString(), noteReg))
        {
            numLayers = jmax(numLayers, fileName.getTrailingIntValue());
            String noteName = fileName.upToFirstOccurrenceOf("v", false, false);
            maxOctave = jmax(maxOctave, noteName.getTrailingIntValue());
            noteRoots.addIfNotAlreadyThere(noteNameToRoot(noteName));
            noteRoots.sort();
            numTotalSample++;
        }
        else if (std::regex_search(fileName.toStdString(), harmReg))
        {
            numResLayers = jmax(numResLayers, fileName.getTrailingIntValue());
            String resName = fileName.fromFirstOccurrenceOf("harm", false, false)
            .upToFirstOccurrenceOf("v", false, false);
            maxResOctave = jmax(maxResOctave, resName.getTrailingIntValue());
            resRoots.addIfNotAlreadyThere(noteNameToRoot(resName));
            resRoots.sort();
            numTotalSample++;
        }
        else if (std::regex_search(fileName.toStdString(), relReg))
        {
            numHammers = jmax(numHammers, fileName.getTrailingIntValue());
            numTotalSample++;
        }
        else if (std::regex_search(fileName.toStdString(), pedalReg))
        {
            numPedal = jmax(numPedal, fileName.getTrailingIntValue());
            numTotalSample++;
        }
    }
    
    progressInc = 1./numTotalSample;
    int noteCount = 0;
    
    //==============================================================================
    //==============================================================================
    // Load the normal samples

    if (numLayers > 0)
    {
        for (int oct = 0; oct <= maxOctave; oct++) {
            for (int n = 0; n < 17; n++) {
                float dBFSBelow = -100.f;
                bool layerFoundForNote = false;
                for (int k = 0; k < numLayers; k++)
                {
                    EXIT_CHECK;
                    
                    String temp;
                    temp.append(completeNotes[n],3);
                    temp.append(String(oct),3);
                    temp.append("v",2);
                    temp.append(String(k+1),3);
                    temp.append(".wav",5);
                    
                    File file(samples.getChildFile(temp));
                    
                    FileInputStream inputStream(file);
                    
                    if (inputStream.openedOk())
                    {
                        String soundName = file.getFileName();
                        
                        MemoryMappedAudioFormatReader* memoryMappedReader = nullptr;
                        if (memoryMappingEnabled)
                        {
                            memoryMappedReader = wavFormat.createMemoryMappedReader(new FileInputStream(file));
                        }
                        else
                        {
                            sampleReader = std::unique_ptr<AudioFormatReader> (wavFormat.createReaderFor(new FileInputStream(file), true));
                        }
                        
                        int root = noteRoots[noteCount];
                        int rootBelow = 0;
                        int rootAbove = 127;
                        
                        if (noteCount > 0) rootBelow = noteRoots[noteCount-1];
                        if (noteCount < noteRoots.size()-1) rootAbove = noteRoots[noteCount+1];
                        
                        int lowKey = 1 + (root + rootBelow) / 2;
                        int highKey = 1 + (root + rootAbove) / 2;
                        
                        BigInteger noteRange;
                        
                        noteRange.setRange(lowKey, highKey-lowKey, true);
                        
                        BigInteger velocityRange;
                        
                        int lowerVelocityThresh = 128 * (float(k) / numLayers);
                        int upperVelocityThresh = 128 * (float(k+1) / numLayers);
                        velocityRange.setRange(lowerVelocityThresh, (upperVelocityThresh - lowerVelocityThresh), true);
                        
                        if (memoryMappingEnabled)
                        {
                            double sourceSampleRate = memoryMappedReader->sampleRate;
                            uint64 maxLength;
                            if (sourceSampleRate <= 0 || memoryMappedReader->lengthInSamples <= 0) {
                                maxLength = 0;
                                
                            } else {
                                maxLength = jmin((uint64)memoryMappedReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                                if (memoryMappedReader->mapEntireFile())
                                {
                                    BKPianoSamplerSound* newSound =
                                    new BKPianoSamplerSound(soundName, memoryMappedReader,
                                                            maxLength, sourceSampleRate,
                                                            noteRange, root,
                                                            0, velocityRange,
                                                            k+1, numLayers,
                                                            dBFSBelow);
                                    dBFSBelow = newSound->getDBFSLevel();
                                    mainSynth->addSound(loadingSoundSetId, newSound);
                                }
                                else DBG("File mapping failed");
                            }
                        }
                        else
                        {
                            double sourceSampleRate = sampleReader->sampleRate;
                            const int numChannels = sampleReader->numChannels;
                            uint64 maxLength;
                            if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                                maxLength = 0;
                                
                            } else {
                                maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                                
                                BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),(int)maxLength);
                                sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);
                                BKPianoSamplerSound* newSound =
                                new BKPianoSamplerSound(soundName, newBuffer,
                                                        maxLength, sourceSampleRate,
                                                        noteRange, root,
                                                        0, velocityRange,
                                                        k+1, numLayers,
                                                        dBFSBelow);
                                dBFSBelow = newSound->getDBFSLevel();
                                mainSynth->addSound(loadingSoundSetId, newSound);
                            }
                        }
                        
                        processor.progress += progressInc;
                        layerFoundForNote = true;
                        //DBG(soundName+": " + String(processor.progress))
                    }
                    else
                    {
                        //                    DBG("File not found: " + temp);
                    }
                }
                if (layerFoundForNote) noteCount++;
            }
        }
    }
    
    //==============================================================================
    //==============================================================================
    // Load any resonance samples
    
    if (numResLayers > 0)
    {
        noteCount = 0;
        for (int oct = 0; oct <= maxResOctave; oct++) {
            for (int n = 0; n < 17; n++) {
                float dBFSBelow = -100.f;
                bool layerFoundForNote = false;
                for (int k = 0; k < numResLayers; k++)
                {
                    EXIT_CHECK;
                    
                    String temp;
                    temp += "harm";
                    temp.append(completeNotes[n],3);
                    temp.append(String(oct),3);
                    temp.append("v",2);
                    temp.append(String(k+1),3);
                    temp.append(".wav",5);
                    
                    //File file(temp);
                    File file(samples.getChildFile(temp));
                    FileInputStream inputStream(file);
                    
                    if (inputStream.openedOk()) {
                        
                        String soundName = file.getFileName();
                        
                        MemoryMappedAudioFormatReader* memoryMappedReader;
                        if (memoryMappingEnabled)
                        {
                            memoryMappedReader = wavFormat.createMemoryMappedReader(new FileInputStream(file));
                        }
                        else
                        {
                            sampleReader = std::unique_ptr<AudioFormatReader> (wavFormat.createReaderFor(new FileInputStream(file), true));
                        }
                        
                        int root = noteRoots[noteCount];
                        int rootBelow = 0;
                        int rootAbove = 127;
                        
                        if (noteCount > 0) rootBelow = noteRoots[noteCount-1];
                        if (noteCount < noteRoots.size()-1) rootAbove = noteRoots[noteCount+1];
                        
                        int lowKey = 1 + (root + rootBelow) / 2;
                        int highKey = 1 + (root + rootAbove) / 2;
                        
                        BigInteger noteRange;
                        
                        noteRange.setRange(lowKey, highKey-lowKey, true);
                        
                        BigInteger velocityRange;
                        
                        int lowerVelocityThresh = 128 * (float(k) / numResLayers);
                        int upperVelocityThresh = 128 * (float(k+1) / numResLayers);
                        velocityRange.setRange(lowerVelocityThresh, (upperVelocityThresh - lowerVelocityThresh), true);
                        
                        //load the sample, add to synth
                        if (memoryMappingEnabled)
                        {
                            double sourceSampleRate = memoryMappedReader->sampleRate;
                            uint64 maxLength;
                            if (sourceSampleRate <= 0 || memoryMappedReader->lengthInSamples <= 0) {
                                maxLength = 0;
                                
                            } else {
                                maxLength = jmin((uint64)memoryMappedReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                                if (memoryMappedReader->mapEntireFile())
                                {
                                    BKPianoSamplerSound* newSound =
                                    new BKPianoSamplerSound(soundName, memoryMappedReader,
                                                            maxLength, sourceSampleRate,
                                                            noteRange, root,
                                                            0, velocityRange,
                                                            k+1, numResLayers,
                                                            dBFSBelow);
                                    dBFSBelow = newSound->getDBFSLevel();
                                    resSynth->addSound(loadingSoundSetId, newSound);
                                }
                                else DBG("File mapping failed");
                            }
                        }
                        else
                        {
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
                                BKPianoSamplerSound* newSound =
                                new BKPianoSamplerSound(soundName, newBuffer,
                                                        maxLength, sourceSampleRate,
                                                        noteRange, root,
                                                        0, velocityRange,
                                                        k+1, numResLayers,
                                                        dBFSBelow);
                                dBFSBelow = newSound->getDBFSLevel();
                                resSynth->addSound(loadingSoundSetId, newSound);
                            }
                        }
                        
                        processor.progress += progressInc;
                        layerFoundForNote = true;
                        //DBG(soundName+": " + String(processor.progress));
                    }
                    else
                    {
                        DBG("File not found: " + temp);
                    }
                }
                if (layerFoundForNote) noteCount++;
            }
        }
    }
    
    //==============================================================================
    //==============================================================================
    // Load any hammer release samples
    
    if (numHammers > 0)
    {
        for (int i = 1; i <= numHammers; i++) {
            EXIT_CHECK;
            String temp;
            temp += "rel";
            temp += String(i);
            temp += ".wav";
            
            //File file(temp);
            File file(samples.getChildFile(temp));
            FileInputStream inputStream(file);
            
            if (inputStream.openedOk()) {
                
                String soundName = file.getFileName();
                
                MemoryMappedAudioFormatReader* memoryMappedReader;
                if (memoryMappingEnabled)
                {
                    memoryMappedReader = wavFormat.createMemoryMappedReader(new FileInputStream(file));
                }
                else
                {
                    sampleReader = std::unique_ptr<AudioFormatReader> (wavFormat.createReaderFor(new FileInputStream(file), true));
                }
                
                BigInteger noteRange;
                // Distribute hammers across the keyboard
                int a = ((88. / numHammers) * i) - i;
                noteRange.setRange(20 - a + (88. / numHammers) * i, 1 + a, true);
                
                BigInteger velocityRange;
                velocityRange.setRange(0, 128, true);
                
                int root = 20 + i;
                
                if (memoryMappingEnabled)
                {
                    double sourceSampleRate = memoryMappedReader->sampleRate;
                    uint64 maxLength;
                    if (sourceSampleRate <= 0 || memoryMappedReader->lengthInSamples <= 0) {
                        maxLength = 0;
                        
                    } else {
                        maxLength = jmin((uint64)memoryMappedReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        if (memoryMappedReader->mapEntireFile())
                        {
                            BKPianoSamplerSound* newSound =
                            new BKPianoSamplerSound(soundName, memoryMappedReader,
                                                    maxLength, sourceSampleRate,
                                                    noteRange, root,
                                                    0, velocityRange,
                                                    1, 1,
                                                    0.f);
                            hamSynth->addSound(loadingSoundSetId, newSound);
                        }
                        else DBG("File mapping failed");
                    }
                }
                else
                {
                    double sourceSampleRate = sampleReader->sampleRate;
                    const int numChannels = sampleReader->numChannels;
                    uint64 maxLength;
                    
                    if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                        maxLength = 0;
                        
                    } else {
                        maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        
                        BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels), (int)maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);
                        BKPianoSamplerSound* newSound =
                        new BKPianoSamplerSound(soundName, newBuffer,
                                                maxLength, sourceSampleRate,
                                                noteRange, root,
                                                0, velocityRange,
                                                1, 1,
                                                0.f);
                        hamSynth->addSound(loadingSoundSetId, newSound);
                    }
                }
                processor.progress += progressInc;
                //DBG(soundName+": " + String(processor.progress));
            }
            else
            {
                DBG("File not found: " + temp);
            }
        }
    }
    
    //==============================================================================
    //==============================================================================
    // Load any pedal samples
    
    if (numPedal > 0)
    {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < numPedal; j++)
            {
                EXIT_CHECK;
                String temp;
                
                temp += "pedal";
                temp += i == 0 ? "D" : "U";
                temp += String(j+1);
                temp += ".wav";
                
                File file(samples.getChildFile(temp));
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk()) {
                    
                    String soundName = file.getFileName();
                    
                    MemoryMappedAudioFormatReader* memoryMappedReader;
                    if (memoryMappingEnabled)
                    {
                        memoryMappedReader = wavFormat.createMemoryMappedReader(new FileInputStream(file));
                    }
                    else
                    {
                        sampleReader = std::unique_ptr<AudioFormatReader> (wavFormat.createReaderFor(new FileInputStream(file), true));
                    }
                    
                    BigInteger noteRange;
                    noteRange.setRange(20 + i, 1, true);
                    
                    BigInteger velocityRange;
                    velocityRange.setRange(0, 128, true);
                    
                    int root = 20 + i;
                    
                    if (memoryMappingEnabled)
                    {
                        double sourceSampleRate = memoryMappedReader->sampleRate;
                        uint64 maxLength;
                        if (sourceSampleRate <= 0 || memoryMappedReader->lengthInSamples <= 0) {
                            maxLength = 0;
                            
                        } else {
                            maxLength = jmin((uint64)memoryMappedReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                            if (memoryMappedReader->mapEntireFile())
                            {
                                BKPianoSamplerSound* newSound =
                                new BKPianoSamplerSound(soundName, memoryMappedReader,
                                                        maxLength, sourceSampleRate,
                                                        noteRange, root,
                                                        0, velocityRange,
                                                        1, 1,
                                                        0.f);
                                pedalSynth->addSound(loadingSoundSetId, newSound);
                            }
                            else DBG("File mapping failed");
                        }
                    }
                    else
                    {
                        double sourceSampleRate = sampleReader->sampleRate;
                        const int numChannels = sampleReader->numChannels;
                        uint64 maxLength;
                        
                        if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                            maxLength = 0;
                            
                        } else {
                            maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                            
                            BKReferenceCountedBuffer::Ptr newBuffer = new BKReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels), (int)maxLength);
                            sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, (int)sampleReader->lengthInSamples, 0, true, true);
                            BKPianoSamplerSound* newSound =
                            new BKPianoSamplerSound(soundName, newBuffer,
                                                    maxLength, sourceSampleRate,
                                                    noteRange, root,
                                                    0, velocityRange,
                                                    1, 1,
                                                    0.f);
                            pedalSynth->addSound(loadingSoundSetId, newSound);
                        }
                    }
                    processor.progress += progressInc;
                    //DBG(soundName+": " + String(processor.progress));
                }
                else
                {
                    DBG("File not found: " + temp);
                }
            }
        }
    }
    
    processor.didLoadMainPianoSamples = true;
    
    return jobStatus;
}

//========================================================================
SampleTouchThread::SampleTouchThread(BKAudioProcessor& p) :
Thread("SampleTouchThread"),
processor(p)
{
}

SampleTouchThread::~SampleTouchThread() {}

void SampleTouchThread::run()
{
    while (!threadShouldExit())
    {
        for (auto voice : processor.mainPianoSynth.getVoices())
        {
            if (BKPianoSamplerVoice* samplerVoice = dynamic_cast<BKPianoSamplerVoice*>(voice))
            {
                if (BKPianoSamplerSound* sound = dynamic_cast<BKPianoSamplerSound*>(samplerVoice->getCurrentlyPlayingSound().get()))
                {
                    if (MemoryMappedAudioFormatReader* reader = sound->getReader())
                    {
                        int position = samplerVoice->getSourceSamplePosition();
                        while (position < reader->getMappedSection().getLength())
                        {
                            reader->touchSample(reader->getMappedSection().clipValue(position));
                            position += processor.getBlockSize();
                            if (threadShouldExit()) return;
                        }
                    }
                }
            }
            if (threadShouldExit()) return;
        }
        
        Thread::wait(1);
        /*       for (auto set : processor.mainPianoSynth.getSounds())
         {
         for (auto sound : *set)
         {
         if (BKPianoSamplerSound* samplerSound = dynamic_cast<BKPianoSamplerSound*>(sound))
         {
         if (MemoryMappedAudioFormatReader* reader = samplerSound->getReader())
         {
         reader->touchSample(0);
         }
         }
         if (threadShouldExit()) return;
         }
         }*/
    }
}



