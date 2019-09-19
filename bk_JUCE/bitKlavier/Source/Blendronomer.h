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
#include "Tuning.h"
#include "Tempo.h"
#include "General.h"
#include "Keymap.h"
#include "BKSTK.h"

// Forward declaration to allow include of Blendronomer in BKSynthesiser
class BKSynthesiser;

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
	BlendronomerPreparation(String newName, Array<float> beats, Array<float> smoothTimes,
		Array<float> feedbackCoefficients, float smoothValue, float smoothDuration, BlendronomerSmoothMode smoothMode,
        BlendronomerSyncMode syncMode, BlendronomerClearMode clearMode,
        float delayMax, float delayLength, float feedbackCoefficient);
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
	inline const Array<float> getBeats() const noexcept { return bBeats; }
	inline const Array<float> getSmoothDurations() const noexcept { return bSmoothDurations; }
	inline const Array<float> getFeedbackCoefficients() const noexcept { return bFeedbackCoefficients; }
	inline const float getDelayMax() const noexcept { return bDelayMax; }
	inline const float getDelayLength() const noexcept { return bDelayLength; }
	inline const float getSmoothValue() const noexcept { return bSmoothValue; }
	inline const float getSmoothDuration() const noexcept { return bSmoothDuration; }
    inline const BlendronomerSmoothMode getSmoothMode() const noexcept { return bSmoothMode; }
	inline const float getInputThreshSEC() const noexcept { return bInputThreshSec; }
	inline const float getInputThreshMS() const noexcept { return bInputThresh; }
	inline const int getHoldMin() const noexcept { return holdMin; }
	inline const int getHoldMax() const noexcept { return holdMax; }
	inline const int getVelocityMin() const noexcept { return velocityMin; }
	inline const int getVelocityMax() const noexcept { return velocityMax; }
	inline const bool getActive() const noexcept { return isActive; }
	inline const bool getInputGain() const noexcept { return inputGain; }
    inline const BlendronomerSyncMode getSyncMode() const noexcept { return bSyncMode; }
    inline const BlendronomerClearMode getClearMode() const noexcept { return bClearMode; }

	//mutators
	inline void setName(String n) { name = n; }
	inline void setBeats(Array<float> beats) { bBeats.swapWith(beats); }
	inline void setSmoothDurations(Array<float> smoothTimes) { bSmoothDurations.swapWith(smoothTimes); }
	inline void setFeedbackCoefficients(Array<float> feedbackCoefficients) { bFeedbackCoefficients.swapWith(feedbackCoefficients); }
    
    inline void setDelayMax(float delayMax) { bDelayMax = delayMax; }
    inline void setBeat(int whichSlider, float value) { bBeats.set(whichSlider, value); }
    inline void setDelayLength(float delayLength) { bDelayLength = delayLength; }
    inline void setSmoothDuration(int whichSlider, float value) { bSmoothDurations.set(whichSlider, value); }
    inline void setSmoothDuration(float smoothDuration) { bSmoothDuration = smoothDuration; }
    inline void setFeedbackCoefficient(int whichSlider, float value) { bFeedbackCoefficients.set(whichSlider, value); }
	inline void setSmoothValue(float smoothValue) { bSmoothValue = smoothValue; }
    inline const void setSmoothMode(BlendronomerSmoothMode mode) { bSmoothMode = mode; }
	inline void setInputThresh(float newThresh)
	{
		bInputThresh = newThresh;
		bInputThreshSec = bInputThresh * .001;
	}
	inline const void setHoldMin(int min) { holdMin = min; }
	inline const void setHoldMax(int max) { holdMax = max; }
	inline const void setVelocityMin(int min) { velocityMin = min; }
	inline const void setVelocityMax(int max) { velocityMax = max; }
	inline const void setActive(bool newActive) { isActive = newActive; }
	inline const void toggleActive() { isActive = !isActive; }
	inline const void setInputGain(float gain) { inputGain = gain; }
    inline const void setSyncMode(BlendronomerSyncMode mode) { bSyncMode = mode; }
    inline const void setClearMode(BlendronomerClearMode mode) { bClearMode = mode; }

	void print(void);
	ValueTree getState(void);
	void setState(XmlElement* e);

private:
	String name;
	bool isActive;

	//stuff from preset
	Array<float> bBeats;
	Array<float> bSmoothDurations;
	Array<float> bFeedbackCoefficients;

	//d0 stuff
	float bDelayMax;
	float bDelayLength;

	//dsmooth stuff
	float bSmoothValue;
	float bSmoothDuration;
    BlendronomerSmoothMode bSmoothMode;

	//signal chain stk classes
	float bFeedbackCoefficient;

	//bK stuff
	float bInputThresh;
	float bInputThreshSec;
	int holdMin, holdMax, velocityMin, velocityMax;
    
    BlendronomerSyncMode bSyncMode;
    BlendronomerClearMode bClearMode;

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
		TempoProcessor::Ptr bTempo,
		BlendronicDelay::Ptr delayL,
		GeneralSettings::Ptr bGeneral,
		BKSynthesiser* bMain
		);
	~BlendronomerProcessor();

	//called with every audio vector
	BKSampleLoadType sampleType;

	float getTimeToBeatMS(float beatsToSkip);
    
    bool velocityCheck(int noteNumber);
    bool holdCheck(int noteNumber);

	//begin timing played note length, called with noteOn
	void keyPressed(int noteNumber, float velocity, int midiChannel);

	//begin playing reverse note, called with noteOff
	void keyReleased(int noteNumber, float velocity, int midiChannel, bool post = false);

	void postRelease(int noteNumber, int midiChannel);

	void prepareToPlay(double sr);

	//accessors
	inline Blendronomer::Ptr getBlendronomer(void) const noexcept { return blendronomer; }
	inline TempoProcessor::Ptr getTempo(void) const noexcept { return tempo; }
	inline BlendronicDelay::Ptr getDelay(void) const noexcept { return delay; }
	inline int getId(void) const noexcept { return blendronomer->getId(); }
    inline int getTempoId(void) const noexcept { return tempo->getId(); }
	inline const float getCurrentNumSamplesBeat(void) const noexcept { return numSamplesBeat; }
    inline uint64 getSampleTime(void) const noexcept { return sampleTimer; }
    inline int getBeatIndex(void) { return beatIndex; }
    inline int getSmoothIndex(void) { return smoothIndex; }
    inline int getFeedbackIndex(void) { return feedbackIndex; }
    inline BKSynthesiser* getSynth(void) { return synth; }


	//mutators
	inline void setBlendronomer(Blendronomer::Ptr blend) { blendronomer = blend; }
	inline void setTempo(TempoProcessor::Ptr temp) { tempo = temp; }
    inline void setSampleTimer(uint64 sampleTime) { sampleTimer = sampleTime; }
    inline void setBeatIndex(int index) { beatIndex = index; }
    inline void setSmoothIndex(int index) { smoothIndex = index; }
    inline void setFeedbackIndex(int index) { feedbackIndex = index; }
	void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr; }
	inline void reset(void) { blendronomer->aPrep->copy(blendronomer->sPrep); }
    
    float* tick();
	void processBlock(int numSamples, int midiChannel);


private:
	BKSynthesiser* synth;
	GeneralSettings::Ptr general;

	Blendronomer::Ptr blendronomer;
	TempoProcessor::Ptr tempo;

	BlendronicDelay::Ptr delay;

	double sampleRate;

	void playNote(int channel, int note, float velocity);
	Array<float> velocities;    //record of velocities
    Array<uint64> holdTimers;
	Array<int> keysDepressed;   //current keys that are depressed

	float numSamplesBeat;          // = beatThresholdSamples * beatMultiplier
	uint64 sampleTimer;
    int beatIndex, smoothIndex, feedbackIndex;

	//JUCE_LEAK_DETECTOR(BlendronomerProcessor);
};
#endif
