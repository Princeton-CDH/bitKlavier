
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "BKPianoSampler.h"

String notes[4] = {"A","C","D#","F#"};

//==============================================================================
MrmAudioProcessor::MrmAudioProcessor() {
    
    clusterSize = 0;
    
    // Synchronic beat multiplier initialization
    synchronicBeatMultipliers = Array<float>(aSynchronicBeatMultipliers,4);
    synchronicCurrentBeats = Array<uint32>();
    synchronicCurrentBeats.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        synchronicCurrentBeats.insert(i,0);
    }
    
    // Synchronic length multiplier initialization
    synchronicLengthMultipliers = Array<float>(aSynchronicLengthMultipliers,4);
    synchronicCurrentLengths = Array<uint32>();
    synchronicCurrentLengths.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        synchronicCurrentLengths.insert(i,0);
    }
    
    // Synchronic accent multiplier initialization
    synchronicAccentMultipliers = Array<float>(aSynchronicAccentMultipliers,4);
    synchronicCurrentAccents = Array<uint32>();
    synchronicCurrentAccents.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        synchronicCurrentAccents.insert(i,0);
    }
    
    // For testing and developing, let's keep directory of samples in home folder on disk.
    String path = "~/samples/";

    // 88 voices seems to go over just fine...
    for (int i = 0; i < 44; i++) {
        mainPianoSynth.addVoice(new BKPianoSamplerVoice());
    }
    
    synchronicOn = Array<int>();
    inSynchronicOn= Array<int>();
    inSynchronicOn.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        inSynchronicOn.insert(i,0);
    }
    
    synchronicCluster = Array<int>();
    inSynchronicCluster = Array<int>();
    inSynchronicCluster.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        inSynchronicCluster.insert(i,0);
    }

    
    synchronicTimers = Array<uint64>();
    synchronicTimers.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        synchronicTimers.insert(i,0);
    }
    
    synchronicClusterTimers = Array<uint64>();
    synchronicClusterTimers.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        synchronicClusterTimers.insert(i,0);
    }
    
    synchronicPhasors = Array<uint64>();
    synchronicPhasors.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        synchronicPhasors.insert(i,0);
    }
    
    synchronicNumPulses = Array<uint32>();
    synchronicNumPulses.ensureStorageAllocated(88);
    for (int i = 0; i < 88; i++) {
        synchronicNumPulses.insert(i,0);
    }
    
    for (int i = 0; i < 8; i++) {
        hammerReleaseSynth.addVoice(new BKPianoSamplerVoice() );
        resonanceReleaseSynth.addVoice(new BKPianoSamplerVoice() );
    }
    
    WavAudioFormat wavFormat;
    
    mainPianoSynth.clearSounds();
    hammerReleaseSynth.clearSounds();
    resonanceReleaseSynth.clearSounds();
    
    int numSamples = 0;
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j > 0)) continue;
            
            for (int k = 0; k < 8; k++)
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
                    
                    int root = 0;
                    if (j == 0) {
                        root = (9+12*i);
                        if (i == 7) {
                            // High C.
                            noteRange.setRange(root-1,4,true);
                        }else {
                            noteRange.setRange(root-1,3,true);
                        }
                    } else if (j == 1) {
                        root = (0+12*i);
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 2) {
                        root = (3+12*i);
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 3) {
                        root = (6+12*i);
                        noteRange.setRange(root-1,3,true);
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
                        
                    }
                    
                    
                    
                    
                    
                } else {
                    DBG("file not opened OK: " + temp);
                }
                          
            }
            
        }
    }
    
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
                
                ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                sampleBuffers.insert(numSamples, newBuffer);
                
                hammerReleaseSynth.addSound(new BKPianoSamplerSound(soundName,
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
                    if (j == 0) {
                        root = (9+12*i);
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 1) {
                        root = (0+12*i);
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 2) {
                        root = (3+12*i);
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 3) {
                        root = (6+12*i);
                        noteRange.setRange(root-1,3,true);
                    }
                    
                    //velocity switching
                    BigInteger velocityRange;
                    velocityRange.setRange(aResonanceVelocityThresh[k], (aResonanceVelocityThresh[k+1] - aResonanceVelocityThresh[k]), true);
                    
                    //load the sample, add to synth
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
                        
                        //DBG("added resonance: " + std::to_string(noteRange.toInteger()) + " " + std::to_string(root) + " " + std::to_string(velocityRange.toInteger()) );
                        resonanceReleaseSynth.addSound(new BKPianoSamplerSound(soundName,
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
    hammerReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    resonanceReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
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
uint64 time_ms = 0;
uint64 time_samp = 0;

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
    
    Array<float> tuningOffsets = Array<float>(aPartialTuning,aNumScaleDegrees);
    int tuningBasePitch = 0;
    int numSamples = buffer.getNumSamples();
    
    int synchronicOnSize = synchronicOn.size();
    
    for (int i = (synchronicOnSize-1); i >= 0; i--){
        int noteIndex = synchronicOn[i];
        
        uint32 totalPulses = synchronicNumPulses[noteIndex];
        uint8 beat = synchronicCurrentBeats[noteIndex];
        uint8 length = synchronicCurrentBeats[noteIndex];
        uint8 accent = synchronicCurrentAccents[noteIndex];
        
        if (totalPulses < aSynchronicNumPulses)
        {
            if (synchronicPhasors[noteIndex] >= (synchronicBeatMultipliers[beat] * getSampleRate() * (60.0/aSynchronicTempo)))
            {
                if (synchronicLengthMultipliers[length] < 0)
                {
                    mainPianoSynth.keyOn(
                                         m.getChannel(),
                                         noteIndex+9,
                                         synchronicAccentMultipliers[accent],
                                         tuningOffsets,
                                         tuningBasePitch,
                                         Reverse,
                                         FixedLengthFixedStart,
                                         BKNoteTypeNil,
                                         (fabs(synchronicLengthMultipliers[length]) * (60.0/aSynchronicTempo) * 1000.0), // start
                                         (fabs(synchronicLengthMultipliers[length]) * (60.0/aSynchronicTempo) * 1000.0)
                                         );
                }
                else
                {
                    mainPianoSynth.keyOn(
                                         m.getChannel(),
                                         noteIndex+9,
                                         synchronicAccentMultipliers[accent],
                                         tuningOffsets,
                                         tuningBasePitch,
                                         Forward,
                                         FixedLength,
                                         BKNoteTypeNil,
                                         0, // start
                                         (fabs(synchronicLengthMultipliers[length]) * (60.0/aSynchronicTempo) * 1000.0)
                                         );
                }
                
                
                
                
                if (++beat >= synchronicBeatMultipliers.size())
                    beat = 0;
                synchronicCurrentBeats.set(noteIndex,beat);
                
                if (++length >= synchronicLengthMultipliers.size())
                    length = 0;
                synchronicCurrentLengths.set(noteIndex,length);
                
                if (++accent >= synchronicAccentMultipliers.size())
                    accent = 0;
                synchronicCurrentAccents.set(noteIndex,accent);
                
                synchronicNumPulses.set(noteIndex,totalPulses+1);
                
                synchronicPhasors.set(noteIndex,0);
            }
            
        }
        else
        {
            inSynchronicOn.set(noteIndex,false);
            synchronicOn.remove(i);
        }
        
        synchronicTimers.set(noteIndex,synchronicTimers[noteIndex]+numSamples);
        synchronicPhasors.set(noteIndex,synchronicPhasors[noteIndex]+numSamples);
    }
    
    int clusterSize = synchronicCluster.size();
    for (int n = (clusterSize-1); n >= 0; n--)
    {
        int noteIndex = synchronicCluster[n];
        if (inSynchronicCluster[noteIndex] &&
            (synchronicClusterTimers[noteIndex] >= (aSynchronicClusterThreshold * getSampleRate())))
        {
            synchronicCluster.remove(n);
            inSynchronicCluster.set(noteIndex,inSynchronicCluster[noteIndex] - 1);
        }
        synchronicClusterTimers.set(noteIndex,synchronicClusterTimers[noteIndex]+numSamples);
        
    }
    
    
    
    // NOTE ON NOTE OFF
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isNoteOn())
        {
            int noteIndex = m.getNoteNumber()-9;
            
            synchronicTimers.set(noteIndex,0);
            
            inSynchronicCluster.set(noteIndex,inSynchronicCluster[noteIndex] + 1);
            synchronicCluster.add(noteIndex);
            synchronicClusterTimers.set(noteIndex,0);
            
            int clusterSize = synchronicCluster.size();
            int firstNote = synchronicCluster.getFirst();
            
            for (int n = (clusterSize-1); n >= 0; n--)
            {
                int note = synchronicCluster[n];
                
                if ((clusterSize >= aSynchronicClusterMin) &&
                    (clusterSize <= aSynchronicClusterMax))
                {
                    if (!inSynchronicOn[note])
                    {
                        inSynchronicOn.set(note, true);
                        synchronicOn.add(note);
                    }
                    if (aSynchronicSyncMode == FirstNoteSync)
                    {   // NOT WORKING
                        synchronicPhasors.set(note, synchronicPhasors[firstNote]);
                        synchronicCurrentBeats.set(note, synchronicCurrentBeats[firstNote]);
                        synchronicCurrentAccents.set(note, synchronicCurrentAccents[firstNote]);
                        synchronicCurrentLengths.set(note, synchronicCurrentLengths[firstNote]);
                        synchronicNumPulses.set(note, synchronicNumPulses[firstNote]);
                         
                    }
                    else if (aSynchronicSyncMode == LastNoteSync)
                    {
                        synchronicPhasors.set(note,0);
                        synchronicCurrentBeats.set(note,0);
                        synchronicCurrentAccents.set(note,0);
                        synchronicCurrentLengths.set(note,0);
                        synchronicNumPulses.set(note,0);
                    }
                   else
                    {
                        
                    }
                }
            }
            
            mainPianoSynth.keyOn(
                                 m.getChannel(),
                                 m.getNoteNumber(),
                                 m.getFloatVelocity(),
                                 tuningOffsets,
                                 tuningBasePitch,
                                 Forward,
                                 Normal,
                                 BKNoteTypeNil,
                                 1000, // start
                                 100 // length
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
            
            //DBG("off velocity " + std::to_string(m.getFloatVelocity()) );
            hammerReleaseSynth.keyOn(
                                     m.getChannel(),
                                     m.getNoteNumber(),
                                     m.getFloatVelocity() * 0.0025, //will want hammerGain multipler that user can set
                                     tuningOffsets,
                                     tuningBasePitch,
                                     Forward,
                                     FixedLength,
                                     BKNoteTypeNil,
                                     0,
                                     2000
                                     );
            
            resonanceReleaseSynth.keyOn(
                                        m.getChannel(),
                                        m.getNoteNumber(),
                                        m.getFloatVelocity() * 0.5, //will also want multiplier for resonance gain, though not here...
                                        tuningOffsets,
                                        tuningBasePitch,
                                        Forward,
                                        FixedLength,
                                        BKNoteTypeNil,
                                        0,
                                        2000
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
    hammerReleaseSynth.renderNextBlock(buffer,midiMessages,0,buffer.getNumSamples());
    resonanceReleaseSynth.renderNextBlock(buffer,midiMessages,0,buffer.getNumSamples());

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
