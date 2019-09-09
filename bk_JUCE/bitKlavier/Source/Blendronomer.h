/*
  ==============================================================================

	Blendronomer.h
	Created: 11 Jun 2019 2:00:53pm
	Author:  Theodore R Trevisan

  ==============================================================================
*/

#ifndef BLENDRONOMER_H_INCLUDED
#define BLENDRONOMER_H_INCLUDED

#include "BKUtilities.h"
#include "BKSynthesiser.h"
#include "Tuning.h"
#include "Tempo.h"
#include "General.h"
#include "Keymap.h"
#include "BKSTK.h"

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONOMER PREPARATION///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class BlendronomerPreparation : public ReferenceCountedObject
{

public:
	typedef ReferenceCountedObjectPtr<BlendronomerPreparation>   Ptr;
	typedef Array<BlendronomerPreparation::Ptr>                  PtrArr;
	typedef Array<BlendronomerPreparation::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BlendronomerPreparation>                  Arr;
	typedef OwnedArray<BlendronomerPreparation, CriticalSection> CSArr;

	//constructors
	BlendronomerPreparation(BlendronomerPreparation::Ptr p);
	BlendronomerPreparation(String newName, Array<int> beats, Array<float> smoothTimes,
		Array<float> feedbackCoefficients, float smoothValue,
		float smoothDuration, float delayMax, float delayLength, float feedbackGain);
	BlendronomerPreparation(void);

	// copy, modify, compare, randomize
	void copy(BlendronomerPreparation::Ptr b);
	void performModification(BlendronomerPreparation::Ptr b, Array<bool> dirty);
	bool compare(BlendronomerPreparation::Ptr b);
    
    inline void randomize()
    {
        return;
    }

	//accessors
	inline const String getName() const noexcept { return name; }
	inline const float getTempo() const noexcept { return bTempo; }
	inline const Array<int> getBeats() const noexcept { return bBeats; }
	inline const Array<float> getSmoothDurations() const noexcept { return bSmoothDurations; }
	inline const Array<float> getFeedbackCoefficients() const noexcept { return bFeedbackCoefficients; }
	inline const float getDelayMax() const noexcept { return bDelayMax; }
	inline const float getFeedbackGain() const noexcept { return bFeedbackGain; }
	inline const float getDelayLength() const noexcept { return bDelayLength; }
	inline const float getSmoothValue() const noexcept { return bSmoothValue; }
	inline const float getSmoothDuration() const noexcept { return bSmoothDuration; }
	inline const float getInputThreshSEC() const noexcept { return bInputThreshSec; }
	inline const float getInputThreshMS() const noexcept { return bInputThresh; }
	inline const int getHoldMin() const noexcept { return holdMin; }
	inline const int getHoldMax() const noexcept { return holdMax; }
	inline const int getVelocityMin() const noexcept { return velocityMin; }
	inline const int getVelocityMax() const noexcept { return velocityMax; }
	inline const int getNumVoices() const noexcept { return bNumVoices; }
	inline const bool getActive() const noexcept { return isActive; }
	inline const bool getInputGain() const noexcept { return inputGain; }

	//mutators
	inline void setName(String n) { name = n; }
	inline void setTempo(int tempo) { bTempo = tempo; }
	inline void setBeats(Array<int> beats) { bBeats.swapWith(beats); }
	inline void setSmoothDurations(Array<float> smoothTimes) { bSmoothDurations.swapWith(smoothTimes); }
	inline void setFeedbackCoefficients(Array<float> feedbackCoefficients) { bFeedbackCoefficients.swapWith(feedbackCoefficients); }
	inline void setDelayMax(float delayMax) { bDelayMax = delayMax; }
	inline void setFeedbackGain(float FeedbackGain) { bFeedbackGain = FeedbackGain; }
	inline void setDelayLength(float delayLength) { bDelayLength = delayLength; }
	inline void setSmoothValue(float smoothValue) { bSmoothValue = smoothValue; }
	inline void setSmoothDuration(float smoothDuration) { bSmoothDuration = smoothDuration; }
	inline void setInputThresh(float newThresh)
	{
		bInputThresh = newThresh;
		bInputThreshSec = bInputThresh * .001;
	}
	inline const void setHoldMin(int min) { holdMin = min; }
	inline const void setHoldMax(int max) { holdMax = max; }
	inline const void setVelocityMin(int min) { velocityMin = min; }
	inline const void setVelocityMax(int max) { velocityMax = max; }
	inline const void setNumVoices(int numVoices) { bNumVoices = numVoices; }
	inline const void setActive(bool newActive) { isActive = newActive; }
	inline const void toggleActive() { isActive = !isActive; }
	inline const void setInputGain(float gain) { inputGain = gain; }

	void print(void);
	ValueTree getState(void);
	void setState(XmlElement* e);

private:
	String name;
	bool isActive;

	//stuff from preset
	float bTempo;
	Array<int> bBeats;
	Array<float> bSmoothDurations;
	Array<float> bFeedbackCoefficients;

	//d0 stuff
	float bDelayMax;
	float bDelayLength;

	//dsmooth stuff
	float bSmoothValue;
	float bSmoothDuration;

	//signal chain stk classes
	float bFeedbackGain;

	//voices? will be implemented later
	int bNumVoices;

	//bK stuff
	float bInputThresh;
	float bInputThreshSec;
	int holdMin, holdMax, velocityMin, velocityMax;

	//needed for sampling
	float inputGain;
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONOMER///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class Blendronomer : public ReferenceCountedObject
{

public:
	typedef ReferenceCountedObjectPtr<Blendronomer>   Ptr;
	typedef Array<Blendronomer::Ptr>                  PtrArr;
	typedef Array<Blendronomer::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<Blendronomer>                  Arr;
	typedef OwnedArray<Blendronomer, CriticalSection> CSArr;

	Blendronomer(BlendronomerPreparation::Ptr prep, int Id) :
		sPrep(new BlendronomerPreparation(prep)),
		aPrep(new BlendronomerPreparation(sPrep)),
		Id(Id),
		name(String(Id))
	{

	}

	Blendronomer(int Id, bool random = false) :
		Id(Id),
		name(String(Id))
	{
		sPrep = new BlendronomerPreparation();
		aPrep = new BlendronomerPreparation(sPrep);
		if (random) randomize();
	}

	inline Blendronomer::Ptr duplicate()
	{
		BlendronomerPreparation::Ptr copyPrep = new BlendronomerPreparation(sPrep);

		Blendronomer::Ptr copy = new Blendronomer(copyPrep, -1);

		copy->setName(name);

		return copy;
	}

	inline void clear(void)
	{
		sPrep = new BlendronomerPreparation();
		aPrep = new BlendronomerPreparation(sPrep);
	}

	inline void copy(Blendronomer::Ptr from)
	{
		sPrep->copy(from->sPrep);
		aPrep->copy(sPrep);
	}

	inline void randomize()
	{
		clear();
		sPrep->randomize();
		aPrep->randomize();
		Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
		name = "random";
	}
	
	inline ValueTree getState(bool active = false)
	{
		ValueTree prep(vtagBlendronomer);

		prep.setProperty("Id", Id, 0);
		prep.setProperty("name", name, 0);

		prep.addChild(active ? aPrep->getState() : aPrep->getState(), -1, 0);

		return prep;
	}

	inline void setState(XmlElement* e)
	{
		Id = e->getStringAttribute("Id").getIntValue();

		String n = e->getStringAttribute("name");

		if (n != String())     name = n;
		else                        name = String(Id);


		XmlElement* params = e->getChildByName("params");

		if (params != nullptr)
		{
			aPrep->setState(params);
		}
		else
		{
			aPrep->setState(e);
		}

		aPrep->copy(aPrep);
	}

	~Blendronomer() {};

	inline int getId() { return Id; }
	inline void setId(int newId) { Id = newId; }
	inline void setName(String newName) { name = newName; }
	inline String getName() const noexcept { return name; }

	BlendronomerPreparation::Ptr sPrep;
	BlendronomerPreparation::Ptr aPrep;
    
private:
	int Id;
	String name;

	//JUCE_LEAK_DETECTOR(Blendronomer)
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONOMER PROCESSOR/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class BlendronomerProcessor : public ReferenceCountedObject
{

public:
	typedef ReferenceCountedObjectPtr<BlendronomerProcessor>   Ptr;
	typedef Array<BlendronomerProcessor::Ptr>                  PtrArr;
	typedef Array<BlendronomerProcessor::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BlendronomerProcessor>                  Arr;
	typedef OwnedArray<BlendronomerProcessor, CriticalSection> CSArr;

	BlendronomerProcessor(Blendronomer::Ptr bBlendronomer,
		TuningProcessor::Ptr bTuning,
		TempoProcessor::Ptr bTempo,
		BKDelay::Ptr delayL,
		GeneralSettings::Ptr bGeneral,
		BKSynthesiser* bMain
		);
	~BlendronomerProcessor();

	//called with every audio vector
	BKSampleLoadType sampleType;

	float getTimeToBeatMS(float beatsToSkip);

	//begin timing played note length, called with noteOn
	void keyPressed(int midiNoteNumber, float midiNoteVelocity, int midiChannel);

	//begin playing reverse note, called with noteOff
	void keyReleased(int midiNoteNumber, float midiVelocity, int midiChannel, bool post = false);

	void postRelease(int midiNoteNumber, int midiChannel);

	void prepareToPlay(double sr);

	inline void attachToSynthesiser(BKSynthesiser* main)
	{
		synth = main;
	}

	//accessors
	inline Blendronomer::Ptr getBlendronomer(void) const noexcept { return blendronomer; }
	inline TuningProcessor::Ptr getTuner(void) const noexcept { return tuner; }
	inline TempoProcessor::Ptr getTempo(void) const noexcept { return tempo; }
	inline BKDelay::Ptr getDelay(void) const noexcept { return delay; }
	inline int getId(void) const noexcept { return blendronomer->getId(); }
	inline int getTunerId(void) const noexcept { return tuner->getId(); }
	inline int getTempoId(void) const noexcept { return tempo->getId(); }
	inline const uint64 getCurrentNumSamplesBeat(void) const noexcept { return numSamplesBeat; }


	//mutators
	inline void setBlendronomer(Blendronomer::Ptr blend) { blendronomer = blend; }
	inline void setTuner(TuningProcessor::Ptr tune) { tuner = tune; }
	inline void setTempo(TempoProcessor::Ptr temp) { tempo = temp; }
	void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr; }
	inline void reset(void) { blendronomer->aPrep->copy(blendronomer->sPrep); }

	void processBlock(int numSamples, int midiChannel);


private:
	BKSynthesiser* synth;
	GeneralSettings::Ptr general;

	Blendronomer::Ptr blendronomer;
	TuningProcessor::Ptr tuner;
	TempoProcessor::Ptr tempo;

	BKDelay::Ptr delay;

	double sampleRate;

	Array<float> tuningOffsets;
	PitchClass tuningBasePitch;

	void playNote(int channel, int note, float velocity);
	Array<float> velocities;    //record of velocities
	Array<int> keysDepressed;   //current keys that are depressed

	bool runChain;
	bool inChain;

	uint64 numSamplesBeat;          // = beatThresholdSamples * beatMultiplier
	uint64 beatThresholdSamples;    // # samples in a beat, as set by tempo
	uint64 sampleTimer;
    uint64 beatIndex, smoothIndex, gainIndex;

	//JUCE_LEAK_DETECTOR(BlendronomerProcessor);
};
#endif
