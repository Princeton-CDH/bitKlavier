/*
  ==============================================================================

	Blendronic.h
	Created: 11 Jun 2019 2:00:53pm
	Author:  Theodore R Trevisan

  ==============================================================================
*/

#ifndef BLENDRONIC_H_INCLUDED
#define BLENDRONIC_H_INCLUDED

#include "BKUtilities.h"
#include "Tuning.h"
#include "Tempo.h"
#include "General.h"
#include "Keymap.h"
#include "BKSTK.h"

// Forward declaration to allow include of Blendronic in BKSynthesiser
class BKSynthesiser;

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONIC PREPARATION///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class BlendronicPreparation : public ReferenceCountedObject
{

public:
	typedef ReferenceCountedObjectPtr<BlendronicPreparation>   Ptr;
	typedef Array<BlendronicPreparation::Ptr>                  PtrArr;
	typedef Array<BlendronicPreparation::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BlendronicPreparation>                  Arr;
	typedef OwnedArray<BlendronicPreparation, CriticalSection> CSArr;

	//constructors
	BlendronicPreparation(BlendronicPreparation::Ptr p);
	BlendronicPreparation(String newName, Array<float> beats, Array<float> delayLengths, Array<float> smoothTimes,
		Array<float> feedbackCoefficients, BlendronicSmoothMode smoothMode, BlendronicSyncMode syncMode,
        BlendronicClearMode clearMode, BlendronicOpenMode openMode, BlendronicCloseMode closeMode, float delayMax);
	BlendronicPreparation(void);

	// copy, modify, compare, randomize
	void copy(BlendronicPreparation::Ptr b);
	void performModification(BlendronicPreparation::Ptr b, Array<bool> dirty);
	bool compare(BlendronicPreparation::Ptr b);
    
    inline void randomize()
    {
        return;
    }

	//accessors
	inline const String getName() const noexcept { return name; }
	inline const Array<float> getBeats() const noexcept { return bBeats; }
    inline const Array<float> getDelayLengths() const noexcept { return bDelayLengths; }
	inline const Array<float> getSmoothDurations() const noexcept { return bSmoothDurations; }
	inline const Array<float> getFeedbackCoefficients() const noexcept { return bFeedbackCoefficients; }
	inline const float getDelayMax() const noexcept { return bDelayMax; }
	
    inline const BlendronicSmoothMode getSmoothMode() const noexcept { return bSmoothMode; }
	inline const float getInputThreshSEC() const noexcept { return bInputThreshSec; }
	inline const float getInputThreshMS() const noexcept { return bInputThresh; }
	inline const int getHoldMin() const noexcept { return holdMin; }
	inline const int getHoldMax() const noexcept { return holdMax; }
	inline const int getVelocityMin() const noexcept { return velocityMin; }
	inline const int getVelocityMax() const noexcept { return velocityMax; }
	inline const bool getInputGain() const noexcept { return inputGain; }
    inline const BlendronicSyncMode getSyncMode() const noexcept { return bSyncMode; }
    inline const BlendronicClearMode getClearMode() const noexcept { return bClearMode; }
    inline const BlendronicOpenMode getOpenMode() const noexcept { return bOpenMode; }
    inline const BlendronicCloseMode getCloseMode() const noexcept { return bCloseMode; }
    inline const int getRotation() const noexcept { return bRotation; }

	//mutators
	inline void setName(String n) { name = n; }
	inline void setBeats(Array<float> beats) { bBeats.swapWith(beats); }
    inline void setDelayLengths(Array<float> delayLengths) { bDelayLengths.swapWith(delayLengths); }
	inline void setSmoothDurations(Array<float> smoothTimes) { bSmoothDurations.swapWith(smoothTimes); }
	inline void setFeedbackCoefficients(Array<float> feedbackCoefficients) { bFeedbackCoefficients.swapWith(feedbackCoefficients); }
    
    inline void setDelayMax(float delayMax) { bDelayMax = delayMax; }
    
    inline void setBeat(int whichSlider, float value) { bBeats.set(whichSlider, value); }
    inline void setDelayLength(int whichSlider, float value) { bDelayLengths.set(whichSlider, value); }
    inline void setSmoothDuration(int whichSlider, float value) { bSmoothDurations.set(whichSlider, value); }
    inline void setFeedbackCoefficient(int whichSlider, float value) { bFeedbackCoefficients.set(whichSlider, value); }
    
    inline const void setSmoothMode(BlendronicSmoothMode mode) { bSmoothMode = mode; }
    inline const void setSyncMode(BlendronicSyncMode mode) { bSyncMode = mode; }
    inline const void setClearMode(BlendronicClearMode mode) { bClearMode = mode; }
    inline const void setOpenMode(BlendronicOpenMode mode) { bOpenMode = mode; }
    inline const void setCloseMode(BlendronicCloseMode mode) { bCloseMode = mode; }
    inline const void setRotation(int rotation) { bRotation = rotation; }
    
	inline void setInputThresh(float newThresh)
	{
		bInputThresh = newThresh;
		bInputThreshSec = bInputThresh * .001;
	}
	inline const void setHoldMin(int min) { holdMin = min; }
	inline const void setHoldMax(int max) { holdMax = max; }
	inline const void setVelocityMin(int min) { velocityMin = min; }
	inline const void setVelocityMax(int max) { velocityMax = max; }
	inline const void setInputGain(float gain) { inputGain = gain; }

    // TODO
	void print(void)
    {
        ;
    }
    
    // TODO
	ValueTree getState(void)
    {
        ValueTree prep("params");
        
        prep.setProperty(ptagBlendronic_smoothMode, getSmoothMode(), 0);
        prep.setProperty(ptagBlendronic_syncMode, getSyncMode(), 0);
        prep.setProperty(ptagBlendronic_clearMode, getClearMode(), 0);
        prep.setProperty(ptagBlendronic_openMode, getOpenMode(), 0);
        prep.setProperty(ptagBlendronic_closeMode, getCloseMode(), 0);

        ValueTree beats(vtagBlendronic_beats);
        int count = 0;
        for (auto f : getBeats())
        {
            beats.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(beats, -1, 0);
        
        ValueTree delayLengths(vtagBlendronic_delayLengths);
        count = 0;
        for (auto f : getDelayLengths())
        {
            delayLengths.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(delayLengths, -1, 0);
        
        ValueTree smoothDurations(vtagBlendronic_smoothDurations);
        count = 0;
        for (auto f : getSmoothDurations())
        {
            smoothDurations.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(smoothDurations, -1, 0);

        ValueTree feedbackCoefficients(vtagBlendronic_feedbackCoefficients);
        count = 0;
        for (auto f : getFeedbackCoefficients())
        {
            feedbackCoefficients.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(feedbackCoefficients, -1, 0);

        return prep;
    }
    
    // TODO
	void setState(XmlElement* e)
    {
        String n; float f;
        
        n = e->getStringAttribute(ptagBlendronic_smoothMode);
        if (n != String())     setSmoothMode((BlendronicSmoothMode) n.getIntValue());
        else                   setSmoothMode(ConstantTimeSmooth);

        n = e->getStringAttribute(ptagBlendronic_syncMode);
        if (n != String())     setSyncMode((BlendronicSyncMode) n.getIntValue());
        else                   setSyncMode(BlendronicFirstNoteOnSync);
        
        n = e->getStringAttribute(ptagBlendronic_clearMode);
        if (n != String())     setClearMode((BlendronicClearMode) n.getIntValue());
        else                   setClearMode(BlendronicFirstNoteOnClear);
        
        n = e->getStringAttribute(ptagBlendronic_openMode);
        if (n != String())     setOpenMode((BlendronicOpenMode) n.getIntValue());
        else                   setOpenMode(BlendronicOpenModeNil);
        
        n = e->getStringAttribute(ptagBlendronic_closeMode);
        if (n != String())     setCloseMode((BlendronicCloseMode) n.getIntValue());
        else                   setCloseMode(BlendronicCloseModeNil);

        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagBlendronic_beats))
            {
                Array<float> beats;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        beats.add(f);
                    }
                }
                setBeats(beats);
            }
            else if (sub->hasTagName(vtagBlendronic_delayLengths))
            {
                Array<float> lengths;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        lengths.add(f);
                    }
                }
                setDelayLengths(lengths);
            }
            else if (sub->hasTagName(vtagBlendronic_smoothDurations))
            {
                Array<float> durs;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        durs.add(f);
                    }
                }
                setSmoothDurations(durs);
            }
            else if (sub->hasTagName(vtagBlendronic_feedbackCoefficients))
            {
                Array<float> coeffs;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        coeffs.add(f);
                    }
                }
                setFeedbackCoefficients(coeffs);
            }
        }
    }

private:
	String name;
	bool isActive;

	//stuff from preset
	Array<float> bBeats;
    Array<float> bDelayLengths;
	Array<float> bSmoothDurations;
	Array<float> bFeedbackCoefficients;

	//d0 stuff
	float bDelayMax;

	//dsmooth stuff
    BlendronicSmoothMode bSmoothMode;
    
    BlendronicSyncMode bSyncMode;
    BlendronicClearMode bClearMode;
    BlendronicOpenMode bOpenMode;
    BlendronicCloseMode bCloseMode;
    
    int bRotation;

	//signal chain stk classes
	float bFeedbackCoefficient;

	//bK stuff
	float bInputThresh;
	float bInputThreshSec;
	int holdMin, holdMax, velocityMin, velocityMax;

	//needed for sampling
	float inputGain;
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONIC///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class Blendronic : public ReferenceCountedObject
{

public:
	typedef ReferenceCountedObjectPtr<Blendronic>   Ptr;
	typedef Array<Blendronic::Ptr>                  PtrArr;
	typedef Array<Blendronic::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<Blendronic>                  Arr;
	typedef OwnedArray<Blendronic, CriticalSection> CSArr;

	Blendronic(BlendronicPreparation::Ptr prep, int Id) :
		sPrep(new BlendronicPreparation(prep)),
		aPrep(new BlendronicPreparation(sPrep)),
		Id(Id),
		name(String(Id))
	{

	}

	Blendronic(int Id, bool random = false) :
		Id(Id),
		name(String(Id))
	{
		sPrep = new BlendronicPreparation();
		aPrep = new BlendronicPreparation(sPrep);
		if (random) randomize();
	}

	inline Blendronic::Ptr duplicate()
	{
		BlendronicPreparation::Ptr copyPrep = new BlendronicPreparation(sPrep);

		Blendronic::Ptr copy = new Blendronic(copyPrep, -1);

		copy->setName(name);

		return copy;
	}

	inline void clear(void)
	{
		sPrep = new BlendronicPreparation();
		aPrep = new BlendronicPreparation(sPrep);
	}

	inline void copy(Blendronic::Ptr from)
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
		ValueTree prep(vtagBlendronic);

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

	~Blendronic() {};

	inline int getId() { return Id; }
	inline void setId(int newId) { Id = newId; }
	inline void setName(String newName) { name = newName; }
	inline String getName() const noexcept { return name; }

	BlendronicPreparation::Ptr sPrep;
	BlendronicPreparation::Ptr aPrep;
    
private:
	int Id;
	String name;

	//JUCE_LEAK_DETECTOR(Blendronic)
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONIC PROCESSOR/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class BlendronicProcessor : public ReferenceCountedObject
{

public:
	typedef ReferenceCountedObjectPtr<BlendronicProcessor>   Ptr;
	typedef Array<BlendronicProcessor::Ptr>                  PtrArr;
	typedef Array<BlendronicProcessor::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BlendronicProcessor>                  Arr;
	typedef OwnedArray<BlendronicProcessor, CriticalSection> CSArr;

	BlendronicProcessor(Blendronic::Ptr bBlendronic,
		TempoProcessor::Ptr bTempo,
		GeneralSettings::Ptr bGeneral,
		BKSynthesiser* bMain
		);
	~BlendronicProcessor();

	//called with every audio vector
	BKSampleLoadType sampleType;

	float getTimeToBeatMS(float beatsToSkip);
    
    bool velocityCheck(int noteNumber);
    bool holdCheck(int noteNumber);

	//begin timing played note length, called with noteOn
	void keyPressed(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates);

	//begin playing reverse note, called with noteOff
	void keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post = false);

	void postRelease(int noteNumber, int midiChannel);

	void prepareToPlay(double sr);

	//accessors
	inline Blendronic::Ptr getBlendronic(void) const noexcept { return blendronic; }
	inline TempoProcessor::Ptr getTempo(void) const noexcept { return tempo; }
	inline BlendronicDelay::Ptr getDelay(void) const noexcept { return delay; }
	inline int getId(void) const noexcept { return blendronic->getId(); }
    inline int getTempoId(void) const noexcept { return tempo->getId(); }
	inline const float getCurrentNumSamplesBeat(void) const noexcept { return numSamplesBeat; }
    inline uint64 getSampleTime(void) const noexcept { return sampleTimer; }
    inline const uint64 getCurrentSample() const noexcept { return delay->getCurrentSample(); }
    inline const uint64 getDelayedSample() const noexcept { return delay->getDelayedSample(); }
    inline int getBeatIndex(void) const noexcept { return beatIndex; }
    inline int getDelayIndex(void) const noexcept { return delayIndex; }
    inline int getSmoothIndex(void) const noexcept { return smoothIndex; }
    inline int getFeedbackIndex(void) const noexcept { return feedbackIndex; }
    inline BKSynthesiser* getSynth(void) const noexcept { return synth; }
    inline Array<int> getKeysDepressed(void) const noexcept { return keysDepressed; }
    inline const AudioBuffer<float> getDelayBuffer(void) const noexcept { return delay->getDelayBuffer(); }
    inline const bool getActive() const noexcept { return delay->getActive(); }
    inline const float getPulseLengthInSamples() const noexcept { return pulseLength * sampleRate; }
    inline const Array<uint64> getBeatPositionsInBuffer() const noexcept { return beatPositionsInBuffer; }

	//mutators
	inline void setBlendronic(Blendronic::Ptr blend) { blendronic = blend; }
	inline void setTempo(TempoProcessor::Ptr temp) { tempo = temp; }
    inline void setSampleTimer(uint64 sampleTime) { sampleTimer = sampleTime; }
    inline void setBeatIndex(int index) { beatIndex = index; }
    inline void setDelayIndex(int index) { delayIndex = index; }
    inline void setSmoothIndex(int index) { smoothIndex = index; }
    inline void setFeedbackIndex(int index) { feedbackIndex = index; }
	void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr; }
    inline void setClearDelayOnNextBeat(bool clear) { clearDelayOnNextBeat = clear; }
    inline const void setActive(bool newActive) { delay->setActive(newActive); }
    inline const void toggleActive() { delay->toggleActive(); }
	inline void reset(void) { blendronic->aPrep->copy(blendronic->sPrep); }
    
    void tick(float* outputs);
    void updateDelayParameters();
	void processBlock(int numSamples, int midiChannel);
    
    inline void addKeymap(Keymap::Ptr keymap)
    {
        keymaps.add(keymap);
    }
    
    inline Keymap::PtrArr getKeymaps(void)
    {
        return keymaps;
    }

private:

	Blendronic::Ptr blendronic;
    
    BKSynthesiser* synth;
    
	TempoProcessor::Ptr tempo;

    GeneralSettings::Ptr general;

	BlendronicDelay::Ptr delay;
    
    Keymap::PtrArr      keymaps;

	double sampleRate;

	void playNote(int channel, int note, float velocity);
	Array<float> velocities;    //record of velocities
    Array<uint64> holdTimers;
	Array<int> keysDepressed;   //current keys that are depressed

    float pulseLength;
	float numSamplesBeat;
    float numSamplesDelay; 
	uint64 sampleTimer;
    Array<uint64> beatPositionsInBuffer;
    int beatPositionsIndex;
    int beatIndex, delayIndex, smoothIndex, feedbackIndex;
    float prevBeat, prevDelay;
    bool clearDelayOnNextBeat;

	//JUCE_LEAK_DETECTOR(BlendronicProcessor);
};
#endif
