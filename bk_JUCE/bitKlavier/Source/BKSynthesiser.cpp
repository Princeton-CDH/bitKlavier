/*
 ==============================================================================

 BKSynthesiser.cpp
 Created: 19 Oct 2016 9:59:49am
 Author:  Michael R Mulshine

 ==============================================================================
 */

#include "BKSynthesiser.h"

#include "PluginProcessor.h"

BKSynthesiserSound::BKSynthesiserSound(void) {}
BKSynthesiserSound::~BKSynthesiserSound() {}

//==============================================================================
BKSynthesiserVoice::BKSynthesiserVoice()
	: currentSampleRate(44100.0),
	currentlyPlayingNote(-1),
	currentPlayingMidiChannel(0),
	noteOnTime(0),
	keyIsDown(false),
	sustainPedalDown(false),
	sostenutoPedalDown(false)
{
}


BKSynthesiserVoice::~BKSynthesiserVoice()
{
}

bool BKSynthesiserVoice::isPlayingChannel(const int midiChannel) const
{
	return currentPlayingMidiChannel == midiChannel;
}

void BKSynthesiserVoice::setCurrentPlaybackSampleRate(const double newRate)
{
	currentSampleRate = newRate;
}

bool BKSynthesiserVoice::isVoiceActive() const
{
	return getCurrentlyPlayingNote() >= 0;
}

void BKSynthesiserVoice::clearCurrentNote()
{
	currentlyPlayingNote = -1;
	currentlyPlayingSound = nullptr;
	currentPlayingMidiChannel = 0;
}

void BKSynthesiserVoice::aftertouchChanged(int) {}
void BKSynthesiserVoice::channelPressureChanged(int) {}

bool BKSynthesiserVoice::wasStartedBefore(const BKSynthesiserVoice& other) const noexcept
{
return noteOnTime < other.noteOnTime;
}
    
void BKSynthesiserVoice::renderNextBlock (AudioBuffer<double>& outputBuffer,
                                          int startSample, int numSamples)
{
    AudioBuffer<double> subBuffer (outputBuffer.getArrayOfWritePointers(),
                                   outputBuffer.getNumChannels(),
                                   startSample, numSamples);
    
    tempBuffer.makeCopyOf (subBuffer);
    renderNextBlock (tempBuffer, 0, numSamples);
    subBuffer.makeCopyOf (tempBuffer);
}
    
//==============================================================================
BKSynthesiser::BKSynthesiser(BKAudioProcessor& processor, GeneralSettings::Ptr gen):
generalSettings(gen),
processor(processor),
pitchWheelValue(8192),
lastNoteOnCounter (0),
minimumSubBlockSize (32),
subBlockSubdivisionIsStrict (false),
shouldStealNotes (true)
{
    for (int i = 0; i < numElementsInArray (lastPitchWheelValues); ++i)
        lastPitchWheelValues[i] = 0x2000;
    
}
    
BKSynthesiser::BKSynthesiser(BKAudioProcessor& processor):
processor(processor),
pitchWheelValue(8192),
lastNoteOnCounter (0),
minimumSubBlockSize (32),
subBlockSubdivisionIsStrict (false),
shouldStealNotes (true)
{
    for (int i = 0; i < numElementsInArray (lastPitchWheelValues); ++i)
        lastPitchWheelValues[i] = 0x2000;
    
}

void BKSynthesiser::setGeneralSettings(GeneralSettings::Ptr gen)
{
    generalSettings = gen;
}

void BKSynthesiser::updateGeneralSettings(GeneralSettings::Ptr gen)
{
    for (int i = voices.size(); --i >= 0;)
    {
        voices.getUnchecked(i)->setGeneralSettings(gen);
    }
}

BKSynthesiser::~BKSynthesiser()
{
}

//==============================================================================
int BKSynthesiser::loadSamples(BKSampleLoadType type, String path, int subsound, bool updateGlobalSet)
{
    return processor.loadSamples(type, path, subsound, updateGlobalSet);
}


BKSynthesiserVoice* BKSynthesiser::getVoice (const int index) const
{
    //const ScopedLock sl (lock);
    return voices [index];
}

void BKSynthesiser::clearVoices()
{
    //const ScopedLock sl (lock);
    voices.clear();
}

BKSynthesiserVoice* BKSynthesiser::addVoice (const BKSynthesiserVoice::Ptr& newVoice)
{
    //const ScopedLock sl (lock);
    newVoice->setCurrentPlaybackSampleRate (processor.getCurrentSampleRate());
    return voices.add (newVoice);
}

void BKSynthesiser::removeVoice (const int index)
{
    //const ScopedLock sl (lock);
    voices.remove (index);
}

void BKSynthesiser::clearSounds(int set)
{
    //const ScopedLock sl (lock);
    if (soundSets.size() - 1 < set) return;
    soundSets.getUnchecked(set)->clear();
}

BKSynthesiserSound* BKSynthesiser::addSound (int set, const BKSynthesiserSound::Ptr& newSound)
{
    //const ScopedLock sl (lock);
    while (soundSets.size() - 1 < set)
    {
//        soundSets.insertMultiple(soundSets.size(), ReferenceCountedArray<BKSynthesiserSound>(), set - (soundSets.size() - 1));
        soundSets.ensureStorageAllocated(set + 1);
        soundSets.add(new ReferenceCountedArray<BKSynthesiserSound>());
    }
    return soundSets.getUnchecked(set)->add (newSound);
}

void BKSynthesiser::removeSound (int set, const int index)
{
    //const ScopedLock sl (lock);
    soundSets.getUnchecked(set)->remove (index);
}

void BKSynthesiser::setNoteStealingEnabled (const bool shouldSteal)
{
    shouldStealNotes = shouldSteal;
}

double BKSynthesiser::getSampleRate() const noexcept
{
    return processor.getCurrentSampleRate();
}

void BKSynthesiser::setMinimumRenderingSubdivisionSize (int numSamples, bool shouldBeStrict) noexcept
{
    jassert (numSamples > 0); // it wouldn't make much sense for this to be less than 1
    minimumSubBlockSize = numSamples;
    subBlockSubdivisionIsStrict = shouldBeStrict;
}

BlendronicDelay::Ptr BKSynthesiser::createBlendronicDelay(float delayLength, int delayBufferSize, double sr, bool active)
{
    const ScopedLock sl (lock);
    BlendronicDelay::Ptr delay = new BlendronicDelay(delayLength, delayLength, INFINITY, delayBufferSize, sr, active);
    return delay;
}

void BKSynthesiser::addBlendronicProcessor(BlendronicProcessor::Ptr bproc)
{
    const ScopedLock sl (lock);
    bprocessors.add(bproc);
}

void BKSynthesiser::removeBlendronicProcessor(int Id)
{
    const ScopedLock sl (lock);
    
    for (int i = 0; i < bprocessors.size(); ++i)
    {
        if (bprocessors[i]->getId() == Id) bprocessors.remove(i);
    }
}

void BKSynthesiser::clearNextDelayBlock(int numSamples)
{
    for (auto b : bprocessors)
    {
		for (int i = 0; i < numSamples; i++)
		{
			b->getDelay()->getDelay()->scalePrevious(0, i, 0);
			b->getDelay()->getDelay()->scalePrevious(0, i, 1);
		}
    }
}


void BKSynthesiser::renderDelays(AudioBuffer<double>& outputAudio, int startSample, int numSamples)
{

    double* outL = outputAudio.getWritePointer (0, startSample);
    double* outR = outputAudio.getNumChannels() > 1 ? outputAudio.getWritePointer (1, startSample) : nullptr;
    
	float totalOutputL = 0.0f;
	float totalOutputR = 0.0f;
    
    BlendronicProcessor::PtrArr activebprocessors;
    for (auto b : bprocessors)
    {
        if (b->getActive()) activebprocessors.add(b);
    }
    
	while (--numSamples >= 0)
	{
		totalOutputL = 0.0f;
		totalOutputR = 0.0f;

		for (auto b : activebprocessors)
		{
			if (b != nullptr)
			{
                float outputs[2];
                b->tick(outputs);
                totalOutputL += outputs[0];
                totalOutputR += outputs[1];
			}
		}

		if (outR != nullptr)
		{
			*outL++ += ((double) totalOutputL) * 1.0;
			*outR++ += ((double) totalOutputR) * 1.0;
		}
		else
		{
			*outL++ += (((double)totalOutputL) + ((double)totalOutputR)) * 0.5;
		}
	}
}


void BKSynthesiser::renderDelays(AudioBuffer<float>& outputAudio, int startSample, int numSamples)
{

    float* outL = outputAudio.getWritePointer (0, startSample);
    float* outR = outputAudio.getNumChannels() > 1 ? outputAudio.getWritePointer (1, startSample) : nullptr;
    
    float totalOutputL = 0.0f;
	float totalOutputR = 0.0f;
    
    BlendronicProcessor::PtrArr activebprocessors;
    for (auto b : bprocessors)
    {
        if (b->getActive()) activebprocessors.add(b);
    }
    
    while (--numSamples >= 0)
    {
        totalOutputL = 0.0f;
		totalOutputR = 0.0f;
        
        for (auto b : activebprocessors)
        {
			if (b != nullptr)
			{
                float outputs[2];
                b->tick(outputs);
                totalOutputL += outputs[0];
                totalOutputR += outputs[1];
			}
        }

        if (outR != nullptr)
        {
            *outL++ += (totalOutputL * 1.0f);
            *outR++ += (totalOutputR * 1.0f);
        }
        else
        {
            *outL++ += ((totalOutputL + totalOutputR) * 0.5f);
        }
    }
}

//==============================================================================
void BKSynthesiser::playbackSampleRateChanged ()
{
    const ScopedLock sl (lock);
    
    allNotesOff (0, false);
    
    for (int i = voices.size(); --i >= 0;)
        voices.getUnchecked (i)->setCurrentPlaybackSampleRate (processor.getCurrentSampleRate());
}

template <typename floatType>
void BKSynthesiser::processNextBlock (AudioBuffer<floatType>& outputAudio,
                                      const MidiBuffer& midiData,
                                      int startSample,
                                      int numSamples)
{
    // must set the sample rate before using this!
    jassert (processor.getCurrentSampleRate() != 0);
    
    const ScopedLock sl (lock);
    
    bool firstEvent = true;
    
    for (MidiMessageMetadata m : midiData)
    {
        if (numSamples >= 0)
        {
            const int samplesToNextMidiMessage = m.samplePosition - startSample;
        
            if (samplesToNextMidiMessage >= numSamples)
            {
                clearNextDelayBlock(numSamples);
                renderVoices (outputAudio, startSample, numSamples);
                renderDelays(outputAudio, startSample, numSamples);
                handleMidiEvent (m.getMessage());
                break;
            }
            
            if (samplesToNextMidiMessage < ((firstEvent && ! subBlockSubdivisionIsStrict) ? 1 : minimumSubBlockSize))
            {
                handleMidiEvent (m.getMessage());
                continue;
            }
            
            firstEvent = false;
            
            clearNextDelayBlock(samplesToNextMidiMessage);
            renderVoices (outputAudio, startSample, samplesToNextMidiMessage);
            renderDelays(outputAudio, startSample, samplesToNextMidiMessage);
            handleMidiEvent (m.getMessage());
            
            startSample += samplesToNextMidiMessage;
            numSamples  -= samplesToNextMidiMessage;
        }
        else handleMidiEvent (m.getMessage());
    }
    
    clearNextDelayBlock(numSamples);
    renderVoices (outputAudio, startSample, numSamples);
    renderDelays(outputAudio, startSample, numSamples);
    return;
}

// explicit template instantiation
template void BKSynthesiser::processNextBlock<float> (AudioBuffer<float>& outputAudio,
                                                      const MidiBuffer& midiData,
                                                      int startSample,
                                                      int numSamples);
template void BKSynthesiser::processNextBlock<double> (AudioBuffer<double>& outputAudio,
                                                       const MidiBuffer& midiData,
                                                       int startSample,
                                                       int numSamples);

void BKSynthesiser::renderVoices (AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    for (int i = voices.size(); --i >= 0;)
        voices.getUnchecked (i)->renderNextBlock (buffer, startSample, numSamples);
    
    
}

void BKSynthesiser::renderVoices (AudioBuffer<double>& buffer, int startSample, int numSamples)
{
    for (int i = voices.size(); --i >= 0;)
        voices.getUnchecked (i)->renderNextBlock (buffer, startSample, numSamples);
}

void BKSynthesiser::handleMidiEvent (const MidiMessage& m)
{
    const int channel = m.getChannel();
    
    if (m.isNoteOn())
    {
        
    }
    else if (m.isNoteOff())
    {
        
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {
        allNotesOff (channel, true);
    }
    else if (m.isPitchWheel())
    {
        const int wheelPos = m.getPitchWheelValue();
        lastPitchWheelValues [channel - 1] = wheelPos;
        handlePitchWheel (channel, wheelPos);
    }
    else if (m.isAftertouch())
    {
        handleAftertouch (channel, m.getNoteNumber(), m.getAfterTouchValue());
    }
    else if (m.isChannelPressure())
    {
        handleChannelPressure (channel, m.getChannelPressureValue());
    }
    else if (m.isController())
    {
        handleController (channel, m.getControllerNumber(), m.getControllerValue());
    }
    else if (m.isProgramChange())
    {
        handleProgramChange (channel, m.getProgramChangeNumber());
    }
}

//==============================================================================

BKSynthesiserVoice* BKSynthesiser::keyOn(const int midiChannel,
	const int keyNoteNumber,
	const int midiNoteNumber,
	const float transp,
	const float velocity,
	const float gain,
	PianoSamplerNoteDirection direction,
	PianoSamplerNoteType type,
	BKNoteType bktype,
    int set,
	int layer,
	const float startingPositionMS,
	const float lengthMS,
	const float rampOnMS, //included in lengthMS
	const float rampOffMS, //included in lengthMS
	TuningProcessor::Ptr tuner,
    float* dynamicGain,
	float* blendronicGain,
	BlendronicProcessor::PtrArr blendronic)
{
                return keyOn   ( midiChannel,
                                 keyNoteNumber,
                                 midiNoteNumber,
                                 transp,
                                 velocity,
                                 gain,
                                 direction,
                                 type,
                                 bktype,
                                 set,
                                 layer,
                                 startingPositionMS,
                                 lengthMS,
                                 rampOnMS,
                                 3.,
                                 1.,
                                 rampOffMS,
                                 tuner,
                                 dynamicGain,
                                 blendronicGain,
								 blendronic);
}
		
BKSynthesiserVoice* BKSynthesiser::keyOn (const int midiChannel,
                                          const int keyNoteNumber,
                                          const int midiNoteNumber,
                                          const float transp,
                                          const float velocity,
                                          const float gain,
                                          PianoSamplerNoteDirection direction,
                                          PianoSamplerNoteType type,
                                          BKNoteType bktype,
                                          int set,
                                          int layer,
                                          const float startingPositionMS,
                                          const float lengthMS,
                                          float adsrAttackMS,
                                          float adsrDecayMS,
                                          float adsrSustain,
                                          float adsrReleaseMS,
                                          TuningProcessor::Ptr tuner,
                                          float* dynamicGain,
                                          float* blendronicGain,
                                          BlendronicProcessor::PtrArr blendronic)
{
	//DBG("BKSynthesiser::keyOn " + String(keyNoteNumber) + " " + String(midiNoteNumber));

	const ScopedLock sl(lock);

	int noteNumber = midiNoteNumber;

	// ADDED THIS
	if (noteNumber > 108 || noteNumber < 21) return nullptr;

	float transposition = transp;
    
    // needed for MIDI Out; will just return the last found voice, if there are multiple voices
    BKSynthesiserVoice* voiceToReturn;
    
    float sampleRateMS = 0.001f * getSampleRate();
    
    int soundSetId = (set < 0) ? processor.globalSoundSetId : set;
    if (soundSets.size() - 1 < soundSetId) return nullptr;
    if (soundSets.getUnchecked(soundSetId) == nullptr) return nullptr;

	for (int i = soundSets.getUnchecked(soundSetId)->size(); --i >= 0;)
	{
		BKSynthesiserSound* const sound = soundSets.getUnchecked(soundSetId)->getUnchecked(i);

		// Check if sound applies to note, velocity, and channel.
		if (sound->appliesToNote(noteNumber) &&
			sound->appliesToVelocity((int)(velocity * 127.0)))
		{
			if (sound->region_ != nullptr)
			{
				if ((sound->trigger == sfzero::Region::release) ||
					(sound->trigger == sfzero::Region::release_key) /*||
					(sustainPedalsDown[midiChannel] != sound->pedal)*/)
				{
					continue;
				}
			}
            
            BKSynthesiserVoice* voice = findFreeVoice (sound, midiChannel, noteNumber, shouldStealNotes);
            startVoice(findFreeVoice(sound, midiChannel, noteNumber, shouldStealNotes),
                       sound,
                       midiChannel,
                       keyNoteNumber,
                       noteNumber,
                       transposition,
                       gain,
                       velocity,
                       direction,
                       type,
                       bktype,
                       layer,
                       (uint64)((startingPositionMS * sampleRateMS)),
                       (uint64)(lengthMS * sampleRateMS),
                       adsrAttackMS * sampleRateMS,
                       adsrDecayMS * sampleRateMS,
                       adsrSustain,
                       adsrReleaseMS * sampleRateMS,
                       tuner,
                       dynamicGain,
                       blendronicGain,
                       blendronic);
			
			// voice to return;
            voiceToReturn = voice;
		}
	}
    
    // use for MIDI Out
    return voiceToReturn;
}

// VELOCITY IN MASTER REGIONS NEEDS TO BE APPLIED APPROPRIATELY

void BKSynthesiser::startVoice(BKSynthesiserVoice* const voice,
                               BKSynthesiserSound* const sound,
                               const int midiChannel,
                               const int keyNoteNumber,
                               const int midiNoteNumber,
                               const float midiNoteNumberOffset,
                               const float gain,
                               const float velocity,
                               PianoSamplerNoteDirection direction,
                               PianoSamplerNoteType type,
                               BKNoteType bktype,
                               int layer,
                               const uint64 startingPosition,
                               const uint64 length,
                               uint64 voiceRampOn,
                               uint64 voiceRampOff,
                               TuningProcessor::Ptr tuner,
                               float* dynamicGain,
                               float* blendronicGain,
                               BlendronicProcessor::PtrArr blendronic
)
{
    startVoice(voice,
               sound,
               midiChannel,
               keyNoteNumber,
               midiNoteNumber,
               midiNoteNumberOffset,
               gain,
               velocity,
               direction,
               type,
               bktype,
               layer,
               startingPosition,
               length,
               voiceRampOn,
               3. * 0.001f * getSampleRate(),
               1.,
               30. * 0.001f * getSampleRate(),
               tuner,
               dynamicGain,
               blendronicGain,
               blendronic);
}


void BKSynthesiser::startVoice(BKSynthesiserVoice* const voice,
                               BKSynthesiserSound* const sound,
                               const int midiChannel,
                               const int keyNoteNumber,
                               const int midiNoteNumber,
                               const float midiNoteNumberOffset,
                               const float gain,
                               const float velocity,
                               PianoSamplerNoteDirection direction,
                               PianoSamplerNoteType type,
                               BKNoteType bktype,
                               int layer,
                               const uint64 startingPosition,
                               const uint64 length,
                               uint64 adsrAttack,
                               uint64 adsrDecay,
                               float adsrSustain,
                               uint64 adsrRelease,
                               TuningProcessor::Ptr tuner,
                               float* dynamicGain,
                               float* blendronicGain,
                               BlendronicProcessor::PtrArr blendronic
)
{
	//DBG("BKSynthesiser::startVoice " + String(keyNoteNumber) + " " + String(midiNoteNumber));

	if (voice != nullptr && sound != nullptr)
	{
		if (voice->currentlyPlayingSound != nullptr)
			voice->stopNote(0.0f, false);

		voice->currentlyPlayingKey = keyNoteNumber; //keep track of which physical key is associated with this voice
		voice->currentlyPlayingNote = midiNoteNumber; //midiNoteNumber + (int)midiNoteNumberOffset)  
		voice->layerId = layerToLayerId(bktype, layer);
		voice->length = (int)length;
		voice->type = type;
		voice->bktype = bktype;
		voice->currentPlayingMidiChannel = midiChannel;
		voice->noteOnTime = ++lastNoteOnCounter;
		voice->currentlyPlayingSound = sound;
		voice->keyIsDown = true;
		voice->sostenutoPedalDown = false;
		voice->sustainPedalDown = sustainPedalsDown[midiChannel];
		voice->tuning = tuner;
        voice->blendronicGain = blendronicGain;
		voice->blendronic = blendronic;


		float g = gain;

		if (sound->region_ != nullptr)
		{
			g *= Decibels::decibelsToGain(sound->region_->volume);
		}

        voice->startNote(midiNoteNumber,
                         midiNoteNumberOffset,
                         pitchWheelValue,
                         g,
                         velocity,
                         direction,
                         type,
                         bktype,
                         startingPosition,
                         length,
                         adsrAttack,
                         adsrDecay,
                         adsrSustain,
                         adsrRelease,
                         sound,
                         dynamicGain,
                         blendronicGain,
                         blendronic);
	}
}

void BKSynthesiser::stopVoice(BKSynthesiserVoice* voice, float velocity, const bool allowTailOff)
{
	jassert(voice != nullptr);

	voice->stopNote(velocity, allowTailOff);

	// the subclass MUST call clearCurrentNote() if it's not tailing off! RTFM for stopNote()!
	jassert(allowTailOff || (voice->getCurrentlyPlayingNote() < 0 && voice->getCurrentlyPlayingSound() == nullptr));
}

void BKSynthesiser::keyOff(const int midiChannel,
                           const BKNoteType type,
                           const int set,
                           const int layerId,
                           const int keyNoteNumber,
                           const int midiNoteNumber,
                           const float velocity,
                           const float gain,
                           float* dynamicGain,
                           bool allowTailOff,
                           bool nostalgicOff)
{

	//DBG("BKSynthesiser::keyOff " + String(keyNoteNumber) + " " + String(midiNoteNumber));
	const ScopedLock sl(lock);

	for (int i = voices.size(); --i >= 0;)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		/*
		if (nostalgicOff)
		{
			DBG(" ~ ~ ~ ~ ~ ~ ~ ~ ");
			DBG(String((int)(voice->getCurrentlyPlayingNote() == midiNoteNumber)));
			DBG(String((int)(voice->getCurrentlyPlayingKey() == keyNoteNumber)));
			DBG(String((int)(voice->isPlayingChannel (midiChannel))));
			DBG(String((int)(voice->layerId == layerToLayerId(type, layerId))));
		}
		 */


		if (voice->getCurrentlyPlayingNote() == midiNoteNumber
			&& voice->getCurrentlyPlayingKey() == keyNoteNumber
			&& voice->isPlayingChannel(midiChannel)
			&& (voice->layerId == layerToLayerId(type, layerId))) //need to add transposition level as well here
		{
			if (BKSynthesiserSound * const sound = voice->getCurrentlyPlayingSound())
			{
				if (sound->appliesToNote(midiNoteNumber)
					&& sound->appliesToChannel(midiChannel))
				{
					// Let synthesiser know that key is no longer down,
					voice->keyIsDown = false;

					if (nostalgicOff || (!((voice->type == FixedLengthFixedStart) ||
						(voice->type == FixedLength) ||
						voice->sostenutoPedalDown)))
					{
						stopVoice(voice, velocity, allowTailOff);
					}
				}
			}
		}
	}

	int noteNumber = midiNoteNumber;

	if (noteNumber > 108 || noteNumber < 21) return;
    
    int soundSetId = (set < 0) ? processor.globalSoundSetId : set;
    if (soundSets.size() - 1 < soundSetId) return;
    if (soundSets.getUnchecked(soundSetId) == nullptr) return;

	for (int i = soundSets.getUnchecked(soundSetId)->size(); --i >= 0;)
	{
		BKSynthesiserSound* const sound = soundSets.getUnchecked(soundSetId)->getUnchecked(i);

		// Check if sound applies to note, velocity, and channel.

		bool appliesToNote = sound->appliesToNote(noteNumber);
		bool appliesToVel = sound->appliesToVelocity((int)(velocity * 127.0));
		bool isRelease = false;
		//DT: TESTING TO SEE IF THIS IS WHAT IS CAUSING GHOST RELEASE NOTES; seems to be, so we need to find another fix....
		//bool isRelease = (sound->trigger == sfzero::Region::release) || (sound->trigger == sfzero::Region::release_key);
		bool pedalStatesMatch = (sustainPedalsDown[midiChannel] == sound->pedal);

		if (appliesToNote && appliesToVel && isRelease && pedalStatesMatch)
		{
            startVoice(findFreeVoice(sound, midiChannel, noteNumber, shouldStealNotes),
                       sound,
                       midiChannel,
                       keyNoteNumber,
                       noteNumber,
                       0, // might need to deal with this
                       gain,
                       velocity,
                       Forward,
                       FixedLengthFixedStart,
                       DirectNote,             //
                       0,                      //
                       0,                      //
                       sound->sampleLength,    //  len
                       0.001f,                 //  A
                       0.001f,                 //  D
                       1.0f,                   //  S
                       0.001f,                 // R
                       nullptr,
                       dynamicGain,
                       nullptr,
                       BlendronicProcessor::PtrArr());

			// break;

		}
	}
}


void BKSynthesiser::allNotesOff(const int midiChannel, const bool allowTailOff)
{
	const ScopedLock sl(lock);

	for (int i = voices.size(); --i >= 0;)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		if (midiChannel <= 0 || voice->isPlayingChannel(midiChannel))
			voice->stopNote(1.0f, allowTailOff);
	}

	sustainPedalsDown.clear();
}

void BKSynthesiser::handlePitchWheel(const int midiChannel, const int wheelValue)
{
	const ScopedLock sl(lock);

	pitchWheelValue = wheelValue;
	//DBG("BKSynthesiser::handlePitchWheel : " + String(pitchWheelValue));

	for (int i = voices.size(); --i >= 0;)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		//DBG("midichannel: " + String(midiChannel) + " voice active channel: " + String(voice->currentPlayingMidiChannel));
		if (midiChannel <= 0 || voice->isPlayingChannel(midiChannel))
			voice->pitchWheelMoved(pitchWheelValue);
	}
}

/*
void BKSynthesiser::updateTuning (int midiChannel, int midiNoteNumber, float offset) //or should it be a multiplier?
{
	const ScopedLock sl (lock);

	for (int i = voices.size(); --i >= 0;)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked (i);

		if (voice->getCurrentlyPlayingNote() == midiNoteNumber
			&& (midiChannel <= 0 || voice->isPlayingChannel (midiChannel)))
			voice->tuningChanged (offset);
	}
}
 */

void BKSynthesiser::handleController(const int midiChannel,
	const int controllerNumber,
	const int controllerValue)
{
	switch (controllerNumber)
	{
	case 0x40:  handleSustainPedal(midiChannel, controllerValue >= 64); break;
	case 0x42:  handleSostenutoPedal(midiChannel, controllerValue >= 64); break;
	case 0x43:  handleSoftPedal(midiChannel, controllerValue >= 64); break;
	default:    break;
	}

	const ScopedLock sl(lock);

	for (int i = voices.size(); --i >= 0;)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		if (midiChannel <= 0 || voice->isPlayingChannel(midiChannel))
			voice->controllerMoved(controllerNumber, controllerValue);
	}
}

void BKSynthesiser::handleAftertouch(int midiChannel, int midiNoteNumber, int aftertouchValue)
{
	const ScopedLock sl(lock);

	for (int i = voices.size(); --i >= 0;)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		if (voice->getCurrentlyPlayingNote() == midiNoteNumber
			&& (midiChannel <= 0 || voice->isPlayingChannel(midiChannel)))
			voice->aftertouchChanged(aftertouchValue);
	}
}

void BKSynthesiser::handleChannelPressure(int midiChannel, int channelPressureValue)
{
	const ScopedLock sl(lock);

	for (int i = voices.size(); --i >= 0;)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		if (midiChannel <= 0 || voice->isPlayingChannel(midiChannel))
			voice->channelPressureChanged(channelPressureValue);
	}
}

void BKSynthesiser::handleSustainPedal(int midiChannel, bool isDown)
{
	jassert(midiChannel > 0 && midiChannel <= 16);
	const ScopedLock sl(lock);

	// Invert sustain should be dealt with around here? 
	if (isDown)
	{
		sustainPedalsDown.setBit(midiChannel);

		for (int i = voices.size(); --i >= 0;)
		{
			BKSynthesiserVoice* const voice = voices.getUnchecked(i);

			if (voice->isPlayingChannel(midiChannel) && voice->isKeyDown())
				voice->sustainPedalDown = true;
		}
	}
	else
	{
		for (int i = voices.size(); --i >= 0;)
		{
			BKSynthesiserVoice* const voice = voices.getUnchecked(i);

			if (voice->isPlayingChannel(midiChannel))
			{
				voice->sustainPedalDown = false;

				if (!voice->isKeyDown())
					stopVoice(voice, 1.0f, true);
			}
		}

		sustainPedalsDown.clearBit(midiChannel);
	}
}

void BKSynthesiser::handleSostenutoPedal(int midiChannel, bool isDown)
{
	jassert(midiChannel > 0 && midiChannel <= 16);
	const ScopedLock sl(lock);

	for (int i = voices.size(); --i >= 0;)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		if (voice->isPlayingChannel(midiChannel))
		{
			if (isDown)
				voice->sostenutoPedalDown = true;
			else if (voice->sostenutoPedalDown)
				stopVoice(voice, 1.0f, true);
		}
	}
}

void BKSynthesiser::handleSoftPedal(int midiChannel, bool /*isDown*/)
{
	ignoreUnused(midiChannel);
	jassert(midiChannel > 0 && midiChannel <= 16);
}

void BKSynthesiser::handleProgramChange(int midiChannel, int programNumber)
{
	ignoreUnused(midiChannel, programNumber);
	jassert(midiChannel > 0 && midiChannel <= 16);
}

//==============================================================================
BKSynthesiserVoice* BKSynthesiser::findFreeVoice(BKSynthesiserSound* soundToPlay,
	int midiChannel, int midiNoteNumber,
	const bool stealIfNoneAvailable) const
{
	const ScopedLock sl(lock);

	for (int i = 0; i < voices.size(); ++i)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		if ((!voice->isVoiceActive()) && voice->canPlaySound(soundToPlay))
			return voice;
	}

	if (stealIfNoneAvailable)
		return findVoiceToSteal(soundToPlay, midiChannel, midiNoteNumber);

	return nullptr;
}

struct VoiceAgeSorter
{
	static int compareElements(BKSynthesiserVoice* v1, BKSynthesiserVoice* v2) noexcept
	{
		return v1->wasStartedBefore(*v2) ? -1 : (v2->wasStartedBefore(*v1) ? 1 : 0);
	}
};

BKSynthesiserVoice* BKSynthesiser::findVoiceToSteal(BKSynthesiserSound* soundToPlay,
	int /*midiChannel*/, int midiNoteNumber) const
{
	// This voice-stealing algorithm applies the following heuristics:
	// - Re-use the oldest notes first
	// - Protect the lowest & topmost notes, even if sustained, but not if they've been released.

	// apparently you are trying to render audio without having any voices...
	jassert(voices.size() > 0);

	// These are the voices we want to protect (ie: only steal if unavoidable)
	BKSynthesiserVoice* low = nullptr; // Lowest sounding note, might be sustained, but NOT in release phase
	BKSynthesiserVoice* top = nullptr; // Highest sounding note, might be sustained, but NOT in release phase

	// this is a list of voices we can steal, sorted by how long they've been running
	Array<BKSynthesiserVoice*> usableVoices;
	usableVoices.ensureStorageAllocated(voices.size());

	for (int i = 0; i < voices.size(); ++i)
	{
		BKSynthesiserVoice* const voice = voices.getUnchecked(i);

		if (voice->canPlaySound(soundToPlay))
		{
			jassert(voice->isVoiceActive()); // We wouldn't be here otherwise

			VoiceAgeSorter sorter;
			usableVoices.addSorted(sorter, voice);

			if (!voice->isPlayingButReleased()) // Don't protect released notes
			{
				const int note = voice->getCurrentlyPlayingNote();

				if (low == nullptr || note < low->getCurrentlyPlayingNote())
					low = voice;

				if (top == nullptr || note > top->getCurrentlyPlayingNote())
					top = voice;
			}
		}
	}

	// Eliminate pathological cases (ie: only 1 note playing): we always give precedence to the lowest note(s)
	if (top == low)
		top = nullptr;

	const int numUsableVoices = usableVoices.size();

	// The oldest note that's playing with the target pitch is ideal..
	for (int i = 0; i < numUsableVoices; ++i)
	{
		BKSynthesiserVoice* const voice = usableVoices.getUnchecked(i);

		if (voice->getCurrentlyPlayingNote() == midiNoteNumber)
			return voice;
	}

	// Oldest voice that has been released (no finger on it and not held by sustain pedal)
	for (int i = 0; i < numUsableVoices; ++i)
	{
		BKSynthesiserVoice* const voice = usableVoices.getUnchecked(i);

		if (voice != low && voice != top && voice->isPlayingButReleased())
			return voice;
	}

	// Oldest voice that doesn't have a finger on it:
	for (int i = 0; i < numUsableVoices; ++i)
	{
		BKSynthesiserVoice* const voice = usableVoices.getUnchecked(i);

		if (voice != low && voice != top && !voice->isKeyDown())
			return voice;
	}

	// Oldest voice that isn't protected
	for (int i = 0; i < numUsableVoices; ++i)
	{
		BKSynthesiserVoice* const voice = usableVoices.getUnchecked(i);

		if (voice != low && voice != top)
			return voice;
	}

	// We've only got "protected" voices now: lowest note takes priority
	jassert(low != nullptr);

	// Duophonic synth: give priority to the bass note:
	if (top != nullptr)
		return top;

	return low;
}
