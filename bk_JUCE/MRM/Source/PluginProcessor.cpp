
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "BKPianoSampler.h"

#include "AudioConstants.h"

String notes[4] = {"A","C","D#","F#"};

//==============================================================================
MrmAudioProcessor::MrmAudioProcessor() {
    
    // For testing and developing, let's keep directory of samples in home folder on disk.
    String path = "~/samples/";
    
    // 88 voices seems to go over just fine...
    for (int i = 0; i < 44; i++) {
        mainPianoSynth.addVoice(new BKPianoSamplerVoice());
#if USE_SECOND_SYNTH
        secondaryPianoSynth.addVoice(new BKPianoSamplerVoice());
#endif
    
    }
    
    WavAudioFormat wavFormat;

    mainPianoSynth.clearSounds();
#if USE_SECOND_SYNTH
    secondaryPianoSynth.clearSounds();
#endif
    int numSamples = 0;
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j > 0)) continue;
            
            for (int k = 0; k < 7; k++)
            {
        
                String temp = path;
                temp.append(notes[j],3);
                temp.append(String(i),3);
                temp.append("v",2);
                temp.append(String(((k*2)+1)),3);
                temp.append(".wav",5);
                File file(temp);
                
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk()) {
                    
                    DBG("file opened OK: " + file.getFileName());
                    
                    String soundName = file.getFileName();
                
                    sampleReader = wavFormat.createReaderFor(new FileInputStream(file), true);
                    
                    BigInteger noteRange;
                    BigInteger noteRange2;
                    int root = 0;
                    if (j == 0) {
                        root = 9+12*i;
                        if (i == 7) {
                            // High C.
                            noteRange.setRange(root,4,true);
                            noteRange2.setRange(root+12,4,true);
                        }else {
                            noteRange.setRange(root,3,true);
                            noteRange2.setRange(root+12,3,true);
                        }
                    } else if (j == 1) {
                        root = 0+12*i;
                        noteRange.setRange(root,3,true);
                        noteRange2.setRange(root+12,3,true);
                    } else if (j == 2) {
                        root = 3+12*i;
                        noteRange.setRange(root,3,true);
                        noteRange2.setRange(root+12,3,true);
                    } else if (j == 3) {
                        root = 6+12*i;
                        noteRange.setRange(root,3,true);
                        noteRange2.setRange(root+12,3,true);
                    } else {
                        
                    }
                    
                    BigInteger velocityRange;
                    velocityRange.setRange(aVelocityThresh[k], (aVelocityThresh[k+1] - aVelocityThresh[k]), true);

                    
                    double sourceSampleRate = sampleReader->sampleRate;
                    const int numChannels = sampleReader->numChannels;
                    uint64 maxLength;
                    
                    if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                        maxLength = 0;
                        
                    } else {
                        maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        
                        ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                        sampleBuffers.insert(numSamples, newBuffer);
                        
                        mainPianoSynth.addSound(new BKPianoSamplerSound(soundName,
                                                                                      newBuffer,
                                                                                      maxLength,
                                                                                      sourceSampleRate,
                                                                                      noteRange,
                                                                                      root,
                                                                                      velocityRange));
                        
#if USE_SECOND_SYNTH
                        secondaryPianoSynth.addSound(new BKPianoSamplerSound(soundName,
                                                                                 newBuffer,
                                                                                 sourceSampleRate,
                                                                                 noteRange,
                                                                                 root+12,
                                                                                 velocityRange));
#endif
                        
                    }
                    
                    
                    
                    
                    
                } else {
                    DBG("file not opened OK: " + temp);
                }
                          
            }
            
        }
    }
    
    /*
    if (sampleReader1 != 0)
    {
        sampleBuffer1.setSize(sampleReader1->numChannels, sampleReader1->lengthInSamples);
        sampleReader1->read(&sampleBuffer1, 0, sampleReader1->lengthInSamples, 0, true, true);
        
        
        //setAudioChannels(0, sampleReader1->numChannels);
    }
    
    */
}

MrmAudioProcessor::~MrmAudioProcessor() {
}

//==============================================================================
const String MrmAudioProcessor::getName() const {
    
    return JucePlugin_Name;
}

bool MrmAudioProcessor::acceptsMidi() const {
    
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MrmAudioProcessor::producesMidi() const {
    
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double MrmAudioProcessor::getTailLengthSeconds() const {
    
    return 0.0;
}

int MrmAudioProcessor::getNumPrograms() {
    
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MrmAudioProcessor::getCurrentProgram() {
    
    return 0;
}

void MrmAudioProcessor::setCurrentProgram (int index) {
    
}

const String MrmAudioProcessor::getProgramName (int index) {
    
    return String();
}

void MrmAudioProcessor::changeProgramName (int index, const String& newName) {
    
}

//==============================================================================
void MrmAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    
    // Use this method as the place to do any pre-playback
    // initialisation that you need.
    
    mainPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
#if USE_SECOND_SYNTH
    secondaryPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
#endif

    
    
}

/*
void MrmAudioProcessor::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
 
}
 */

void MrmAudioProcessor::releaseResources() {
    
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    //fileBuffer.setSize (0, 0);
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MrmAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) {
    
    // Reject any bus arrangements that are not compatible with your plugin

    const int numChannels = preferredSet.size();

   #if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
   #elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
   #else
    if (numChannels != 1 && numChannels != 2)
        return false;

    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
   #endif

    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif

#define LAYERS 0
void MrmAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) {
    
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    buffer.clear();
    
    MidiBuffer processedMidi;
    int time;
    MidiMessage m;
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isNoteOn())
        {
            
            Array<float> tuningOffsets = Array<float>(aPartialTuning,aNumScaleDegrees);
            
            int tuningBasePitch = 4;
            
            mainPianoSynth.keyOn(
                                 m.getChannel(),
                                 m.getNoteNumber(),
                                 m.getFloatVelocity(),
                                 tuningOffsets,
                                 tuningBasePitch,
                                 Forward,
                                 Normal,
                                 3000, // start
                                 1500 // length
                                 );
            
        }
        else if (m.isNoteOff())
        {
            mainPianoSynth.keyOff(
                                  m.getChannel(),
                                  m.getNoteNumber(),
                                  m.getFloatVelocity(),
                                  true
                                  );
        }
        else if (m.isAftertouch())
        {
        }
        else if (m.isPitchWheel())
        {
        }
        
        processedMidi.addEvent (m, time);
    }
    
    midiMessages.swapWith (processedMidi);
    
    
    mainPianoSynth.renderNextBlock(buffer,midiMessages,0,buffer.getNumSamples());
#if USE_SECOND_SYNTH
    secondaryPianoSynth.renderNextBlock(buffer,midiMessages,0,buffer.getNumSamples());
#endif
    

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    /*
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);//.getWritePointer (channel);
        
        // ..do something to the data...
    }
     */
}




//==============================================================================
void MrmAudioProcessor::changeListenerCallback(ChangeBroadcaster *source) {
    
}

//==============================================================================
bool MrmAudioProcessor::hasEditor() const {
    
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MrmAudioProcessor::createEditor() {
    
    return new MrmAudioProcessorEditor (*this);
}

//==============================================================================
void MrmAudioProcessor::getStateInformation (MemoryBlock& destData) {
    
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MrmAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    
    return new MrmAudioProcessor();
}
