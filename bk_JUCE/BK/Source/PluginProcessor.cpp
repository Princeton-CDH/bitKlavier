
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "BKPianoSampler.h"

//#include "BKState.h"

String notes[4] = {"A","C","D#","F#"};

//==============================================================================
MrmAudioProcessor::MrmAudioProcessor() {
    
    numSynchronicLayers = 2;
    currentSynchronicLayer = 0;
    // For testing and developing, let's keep directory of samples in home folder on disk.
    
    synchronic = OwnedArray<SynchronicProcessor,CriticalSection>();
    synchronic.ensureStorageAllocated(numSynchronicLayers);
    
    loadMainPianoSamples(&mainPianoSynth, aNumLayers);
    loadHammerReleaseSamples(&hammerReleaseSynth);
    loadResonanceRelaseSamples(&resonanceReleaseSynth);
}

MrmAudioProcessor::~MrmAudioProcessor()
{
    
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
    
    return String("bitKlavier");
}

void MrmAudioProcessor::changeProgramName (int index, const String& newName) {
    
}

//==============================================================================
void MrmAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    
    // Use this method as the place to do any pre-playback
    // initialisation that you need.
    
    mainPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
    
#if USE_SECOND_SYNTH
    secondPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
#endif
    
    hammerReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    
    resonanceReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    
    sPrep = new SynchronicPreparation(120.0,
                                      8,
                                      2,
                                      5,
                                      1.0,
                                      FirstNoteSync,
                                      0,
                                      Array<float>({1.0}),
                                      Array<float>({1.0}),
                                      Array<float>({1.0}),
                                      Array<float>(aJustTuning,12),
                                      0);
    
    sProcess = new SynchronicProcessor(&mainPianoSynth, sPrep);
    
    nostalgic.attachToSynth(&mainPianoSynth);

}

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
    
    sProcess->renderNextBlock(channel, numSamples);
    //nostalgic.processBlock(buffer.getNumSamples(), m.getChannel());
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        int noteIndex = m.getNoteNumber()-9;
        channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            sProcess->notePlayed(noteIndex, m.getVelocity());
            
            //nostalgic.noteLengthTimerOn(m.getNoteNumber(), m.getFloatVelocity()); //start measuring note length
            
            mainPianoSynth.keyOn(
                                 m.getChannel(),
                                 m.getNoteNumber(),
                                 m.getFloatVelocity() * .5,
                                 tuningOffsets,
                                 tuningBasePitch,
                                 Forward,
                                 Normal,
                                 BKNoteTypeNil,
                                 1000, // start
                                 100, // length
                                 0,
                                 30 );
            
            
        }
        else if (m.isNoteOff())
        {
            
            nostalgic.playNote(m.getNoteNumber(), m.getChannel());
            
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
                                     2000,
                                     4,
                                     4 );
            
            resonanceReleaseSynth.keyOn(
                                        m.getChannel(),
                                        m.getNoteNumber(),
                                        m.getFloatVelocity(), //will also want multiplier for resonance gain, though not here...
                                        tuningOffsets,
                                        tuningBasePitch,
                                        Forward,
                                        FixedLength,
                                        BKNoteTypeNil,
                                        0,
                                        2000,
                                        4,
                                        4 );
            
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
    secondPianoSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
#endif
    
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
void MrmAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
    
}

//==============================================================================
bool MrmAudioProcessor::hasEditor() const
{
    
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MrmAudioProcessor::createEditor()
{
    
    return new MrmAudioProcessorEditor (*this);
}

//==============================================================================
void MrmAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MrmAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    
    return new MrmAudioProcessor();
}

void MrmAudioProcessor::loadMainPianoSamples(BKSynthesiser *synth, int numLayers)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    // 88 voices seems to go over just fine...
    for (int i = 0; i < 88; i++)
    {
        mainPianoSynth.addVoice(new BKPianoSamplerVoice());
    }
    
    mainPianoSynth.clearSounds();
    
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
                        
                        ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                        
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
}

void MrmAudioProcessor::loadResonanceRelaseSamples(BKSynthesiser *synth)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    for (int i = 0; i < 88; i++)
    {
        resonanceReleaseSynth.addVoice(new BKPianoSamplerVoice() );
    }
    
    resonanceReleaseSynth.clearSounds();
    
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
                        root = (9+12*i);
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 1)
                    {
                        root = (0+12*i);
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 2)
                    {
                        root = (3+12*i);
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 3)
                    {
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
                    
                    if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0)
                    {
                        maxLength = 0;
                    }
                    else
                    {
                        maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        
                        ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                        
                        //DBG("added resonance: " + std::to_string(noteRange.toInteger()) + " " + std::to_string(root) + " " + std::to_string(velocityRange.toInteger()) );
                        resonanceReleaseSynth.addSound(new BKPianoSamplerSound(soundName,
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

void MrmAudioProcessor::loadHammerReleaseSamples(BKSynthesiser *synth)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    for (int i = 0; i < 88; i++)
    {
        hammerReleaseSynth.addVoice(new BKPianoSamplerVoice() );
    }
    
    hammerReleaseSynth.clearSounds();
    
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
    
}

