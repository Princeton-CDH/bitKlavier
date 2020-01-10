/*
  ==============================================================================

	Blendronic.h
	Created: 11 Jun 2019 2:00:53pm
	Author:  Theodore R Trevisan
 
    The original algorithm for Bledrónic was developed by Dan for the Feedback
    movement from "neither Anvil nor Pulley," and was subsequently used in
    Clapping Machine Music Variations, Olagón, and others. A paper describing
    the original algorithm was presented at the International Computer Music
    Conference in 2010 (http://www.manyarrowsmusic.com/papers/cmmv.pdf).
 
    "Clapping Machine Music Variations: A Composition for Acoustic/Laptop Ensemble"
    Dan Trueman
    Proceedings for the International Computer Music Conference
    SUNY Stony Brook, 2010
 
    The basic idea is that the length of a delay line changes periodically, as
    set by a sequence of beat lengths; the changes can happen instantaneously,
    or can take place over a period of time, a "smoothing" time that creates
    a variety of artifacts, tied to the beat pattern. The smoothing parameters
    themselves can be sequenced in a pattern, as can a feedback coefficient,
    which determines how much of the out of the delay line is fed back into it.

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

/*
BlendronicPreparation holds all the state variable values for the
Blendrónic preparation. As with other preparation types, bK will use
two instantiations of BlendronicPreparation for every active
Blendrónic in the gallery, one to store the static state of the
preparation, and the other to store the active state. These will
be the same, unless a Modification is triggered, in which case the
active state will be changed (and a Reset will revert the active state
to the static state).
*/

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
	BlendronicPreparation(String newName,
                          Array<float> beats,
                          Array<float> delayLengths,
                          Array<float> smoothTimes,
                          Array<float> feedbackCoefficients,
                          float clusterThresh,
//                          BlendronicSmoothMode smoothMode,
//                          BlendronicSyncMode syncMode,
//                          BlendronicClearMode clearMode,
//                          BlendronicOpenMode openMode,
//                          BlendronicCloseMode closeMode,
                          float delayMax);
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
	inline const Array<float> getSmoothValues() const noexcept { return bSmoothValues; }
	inline const Array<float> getFeedbackCoefficients() const noexcept { return bFeedbackCoefficients; }
	inline const float getDelayMax() const noexcept { return bDelayMax; }
	
//    inline const BlendronicSmoothMode getSmoothMode() const noexcept { return bSmoothMode; }
    inline const BlendronicSmoothBase getSmoothBase() const noexcept { return bSmoothBase; }
    inline const BlendronicSmoothScale getSmoothScale() const noexcept { return bSmoothScale; }
	inline const float getInputThreshSEC() const noexcept { return bInputThreshSec; }
	inline const float getInputThreshMS() const noexcept { return bInputThresh; }
	inline const int getHoldMin() const noexcept { return holdMin; }
	inline const int getHoldMax() const noexcept { return holdMax; }
	inline const int getVelocityMin() const noexcept { return velocityMin; }
	inline const int getVelocityMax() const noexcept { return velocityMax; }
	inline const bool getInputGain() const noexcept { return inputGain; }
    inline const float getClusterThreshSEC() const noexcept {return bClusterThreshSec; }
    inline const float getClusterThreshMS() const noexcept {return bClusterThresh; }
//    inline const BlendronicSyncMode getSyncMode() const noexcept { return bSyncMode; }
//    inline const BlendronicClearMode getClearMode() const noexcept { return bClearMode; }
//    inline const BlendronicOpenMode getOpenMode() const noexcept { return bOpenMode; }
//    inline const BlendronicCloseMode getCloseMode() const noexcept { return bCloseMode; }

	//mutators
	inline void setName(String n) { name = n; }
	inline void setBeats(Array<float> beats) { bBeats.swapWith(beats); }
    inline void setDelayLengths(Array<float> delayLengths) { bDelayLengths.swapWith(delayLengths); }
	inline void setSmoothValues(Array<float> smoothValues) { bSmoothValues.swapWith(smoothValues); }
	inline void setFeedbackCoefficients(Array<float> feedbackCoefficients) { bFeedbackCoefficients.swapWith(feedbackCoefficients); }
    
    inline void setDelayMax(float delayMax) { bDelayMax = delayMax; }
    
    inline void setBeat(int whichSlider, float value) { bBeats.set(whichSlider, value); }
    inline void setDelayLength(int whichSlider, float value) { bDelayLengths.set(whichSlider, value); }
    inline void setSmoothValue(int whichSlider, float value) { bSmoothValues.set(whichSlider, value); }
    inline void setFeedbackCoefficient(int whichSlider, float value) { bFeedbackCoefficients.set(whichSlider, value); }
    
//    inline const void setSmoothMode(BlendronicSmoothMode mode) { bSmoothMode = mode; }
    inline const void setSmoothBase(BlendronicSmoothBase base) { bSmoothBase = base; }
    inline const void setSmoothScale(BlendronicSmoothScale scale) { bSmoothScale = scale; }
    inline const void toggleSmoothBase()
    {
        if (bSmoothBase == BlendronicSmoothPulse)
        {
            bSmoothBase = BlendronicSmoothBeat;
        }
        else bSmoothBase = BlendronicSmoothPulse;
    }
    inline const void toggleSmoothScale()
    {
        if (bSmoothScale == BlendronicSmoothConstant)
        {
            bSmoothScale = BlendronicSmoothFull;
        }
        else bSmoothScale = BlendronicSmoothConstant;
    }
//    inline const void setSyncMode(BlendronicSyncMode mode) { bSyncMode = mode; }
//    inline const void setClearMode(BlendronicClearMode mode) { bClearMode = mode; }
//    inline const void setOpenMode(BlendronicOpenMode mode) { bOpenMode = mode; }
//    inline const void setCloseMode(BlendronicCloseMode mode) { bCloseMode = mode; }
    
    inline const TargetNoteMode getTargetTypeBlendronicSync() const noexcept { return targetTypeBlendronicSync; }
    inline const TargetNoteMode getTargetTypeBlendronicPatternSync() const noexcept { return targetTypeBlendronicPatternSync; }
    inline const TargetNoteMode getTargetTypeBlendronicClear() const noexcept { return targetTypeBlendronicClear; }
    inline const TargetNoteMode getTargetTypeBlendronicPausePlay() const noexcept { return targetTypeBlendronicPausePlay; }
    inline const TargetNoteMode getTargetTypeBlendronicOpenCloseInput() const noexcept { return targetTypeBlendronicOpenCloseInput; }
    inline const TargetNoteMode getTargetTypeBlendronicOpenCloseOutput() const noexcept { return targetTypeBlendronicOpenCloseOutput; }
    inline const TargetNoteMode getTargetTypeBlendronic(KeymapTargetType which)
    {
        if (which == TargetTypeBlendronicSync)          return targetTypeBlendronicSync;
        if (which == TargetTypeBlendronicPatternSync)   return targetTypeBlendronicPatternSync;
        if (which == TargetTypeBlendronicClear)         return targetTypeBlendronicClear;
        if (which == TargetTypeBlendronicPausePlay)     return targetTypeBlendronicPausePlay;
        if (which == TargetTypeBlendronicOpenCloseInput)  return targetTypeBlendronicOpenCloseInput;
        if (which == TargetTypeBlendronicOpenCloseOutput)  return targetTypeBlendronicOpenCloseOutput;
        return TargetNoteModeNil;
    }
    
    inline void setTargetTypeBlendronicSync(TargetNoteMode nm)          { targetTypeBlendronicSync = nm; }
    inline void setTargetTypeBlendronicPatternSync(TargetNoteMode nm)   { targetTypeBlendronicPatternSync = nm; }
    inline void setTargetTypeBlendronicClear(TargetNoteMode nm)         { targetTypeBlendronicClear = nm; }
    inline void setTargetTypeBlendronicPausePlay(TargetNoteMode nm)     { targetTypeBlendronicPausePlay = nm; }
    inline void setTargetTypeBlendronicOpenCloseInput(TargetNoteMode nm)  { targetTypeBlendronicOpenCloseInput = nm; }
    inline void setTargetTypeBlendronicOpenCloseOutput(TargetNoteMode nm)  { targetTypeBlendronicOpenCloseOutput = nm; }
    inline void setTargetTypeBlendronic(KeymapTargetType which, TargetNoteMode nm)
    {
        if (which == TargetTypeBlendronicSync)          { targetTypeBlendronicSync = nm; }
        if (which == TargetTypeBlendronicPatternSync)   { targetTypeBlendronicPatternSync = nm; }
        if (which == TargetTypeBlendronicClear)         { targetTypeBlendronicClear = nm; }
        if (which == TargetTypeBlendronicPausePlay)     { targetTypeBlendronicPausePlay = nm; }
        if (which == TargetTypeBlendronicOpenCloseInput)  { targetTypeBlendronicOpenCloseInput = nm; }
        if (which == TargetTypeBlendronicOpenCloseOutput)  { targetTypeBlendronicOpenCloseOutput = nm; }
    }
    
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
    
    inline void setClusterThresh(float clusterThresh)
    {
        bClusterThresh = clusterThresh;
        bClusterThreshSec = bClusterThresh * .001;
    }

    // TODO
	void print(void)
    {
        ;
    }
    
    // TODO
	ValueTree getState(void)
    {
        ValueTree prep("params");
        
//        prep.setProperty(ptagBlendronic_smoothMode, getSmoothMode(), 0);
//        prep.setProperty(ptagBlendronic_syncMode, getSyncMode(), 0);
//        prep.setProperty(ptagBlendronic_clearMode, getClearMode(), 0);
//        prep.setProperty(ptagBlendronic_openMode, getOpenMode(), 0);
//        prep.setProperty(ptagBlendronic_closeMode, getCloseMode(), 0);

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
        
        ValueTree smoothValues(vtagBlendronic_smoothValues);
        count = 0;
        for (auto f : getSmoothValues())
        {
            smoothValues.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(smoothValues, -1, 0);

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
        
//        n = e->getStringAttribute(ptagBlendronic_smoothMode);
//        if (n != String())     setSmoothMode((BlendronicSmoothMode) n.getIntValue());
//        else                   setSmoothMode(ConstantTimeSmooth);

//        n = e->getStringAttribute(ptagBlendronic_syncMode);
//        if (n != String())     setSyncMode((BlendronicSyncMode) n.getIntValue());
//        else                   setSyncMode(BlendronicFirstNoteOnSync);
//
//        n = e->getStringAttribute(ptagBlendronic_clearMode);
//        if (n != String())     setClearMode((BlendronicClearMode) n.getIntValue());
//        else                   setClearMode(BlendronicFirstNoteOnClear);
//
//        n = e->getStringAttribute(ptagBlendronic_openMode);
//        if (n != String())     setOpenMode((BlendronicOpenMode) n.getIntValue());
//        else                   setOpenMode(BlendronicOpenModeNil);
//
//        n = e->getStringAttribute(ptagBlendronic_closeMode);
//        if (n != String())     setCloseMode((BlendronicCloseMode) n.getIntValue());
//        else                   setCloseMode(BlendronicCloseModeNil);

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
            else if (sub->hasTagName(vtagBlendronic_smoothValues))
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
                setSmoothValues(durs);
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
	Array<float> bSmoothValues;
	Array<float> bFeedbackCoefficients;

	//d0 stuff
	float bDelayMax;

	//dsmooth stuff
    BlendronicSmoothBase bSmoothBase;
    BlendronicSmoothScale bSmoothScale;
    
//    BlendronicSyncMode bSyncMode;
//    BlendronicClearMode bClearMode;
//    BlendronicOpenMode bOpenMode;
//    BlendronicCloseMode bCloseMode;
    
    TargetNoteMode targetTypeBlendronicSync;
    TargetNoteMode targetTypeBlendronicPatternSync;
    TargetNoteMode targetTypeBlendronicClear;
    TargetNoteMode targetTypeBlendronicPausePlay;
    TargetNoteMode targetTypeBlendronicOpenCloseInput;
    TargetNoteMode targetTypeBlendronicOpenCloseOutput;

	//signal chain stk classes
	float bFeedbackCoefficient;

	//bK stuff
	float bInputThresh;
	float bInputThreshSec;
	int holdMin, holdMax, velocityMin, velocityMax;
    
    float bClusterThresh;      //max time between played notes before new cluster is started, in MS
    float bClusterThreshSec;

	//needed for sampling
	float inputGain;
    
    JUCE_LEAK_DETECTOR(BlendronicPreparation);
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONIC///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/*
This class owns two BlendronicPreparations: sPrep and aPrep
As with other preparation, sPrep is the static preparation, while
aPrep is the active preparation currently in use. sPrep and aPrep
remain the same unless a Modification is triggered, which will change
aPrep but not sPrep. aPrep will be restored to sPrep when a Reset
is triggered.
*/

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

    // for unit-testing
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

	JUCE_LEAK_DETECTOR(Blendronic)
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONIC PROCESSOR/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/*
 BlendronicProcessor has the primary function -- tick() -- that handles the delay line.
 Unlike the other preparations, it doesn't process a block directly, because it needs
 audio samples from whatever other preparations it is connected to. So, instead, it is
 forward declared so BKSynthesiser::renderDelays can call tick() and send Blendronic
 the samples it needs. The actual delay class is BlendronicDelay, a delay line with
 linear interpolation and feedback.

 It connects Keymap, and Tempo preparations together as needed, and gets the Blendrónic
 values it needs to behave as expected.
*/

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
    inline const Array<float> getDelayLengthRecord() const noexcept { return delay->getDelayLengthRecord(); }
    inline const bool getActive() const noexcept { return delay->getActive(); }
    inline const bool getInputState() const noexcept { return delay->getInputState(); }
    inline const bool getOutputState() const noexcept { return delay->getOutputState(); }
    inline const float getPulseLengthInSamples() const noexcept { return pulseLength * sampleRate; }
    inline const Array<uint64> getBeatPositionsInBuffer() const noexcept { return beatPositionsInBuffer; }
    inline const float getPulseOffset() const noexcept { return pulseOffset; }
    
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
    inline const void setInputState(bool inputState) { delay->setInputState(inputState); }
    inline const void toggleInput() { delay->toggleInput(); }
    inline const void setOutputState(bool inputState) { delay->setOutputState(inputState); }
    inline const void toggleOutput() { delay->toggleOutput(); }
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
    
    /* Private Functions */

	void playNote(int channel, int note, float velocity);
    
    /* Private Variables */
    
    Blendronic::Ptr         blendronic;
    BKSynthesiser*          synth;
    TempoProcessor::Ptr     tempo;
    GeneralSettings::Ptr    general;
    BlendronicDelay::Ptr    delay;
    Keymap::PtrArr          keymaps;
    
    double sampleRate;
    
	Array<float> velocities;    //record of velocities
    Array<uint64> holdTimers;
	Array<int> keysDepressed;   //current keys that are depressed
//
//    Array<int> syncKeysDepressed;
//    Array<int> clearKeysDepressed;
//    Array<int> openKeysDepressed;
//    Array<int> closeKeysDepressed;
    
    bool inSyncCluster, inClearCluster, inOpenCluster, inCloseCluster;
    bool nextSyncOffIsFirst, nextClearOffIsFirst, nextOpenOffIsFirst, nextCloseOffIsFirst;
    
    uint64 thresholdSamples;
    uint64 syncThresholdTimer;
    uint64 clearThresholdTimer;
    uint64 openThresholdTimer;
    uint64 closeThresholdTimer;

    float pulseLength; // Length in seconds of a pulse (1.0 length beat)
	float numSamplesBeat; // Length in samples of the current step in the beat pattern
    float numSamplesDelay; // Length in sample of the current step in the delay pattern
    
	uint64 sampleTimer; // Sample count for timing param sequence steps
    
    // Index of sequenced param patterns
    int beatIndex, delayIndex, smoothIndex, feedbackIndex;
    
    // Values of previous step values for smoothing. Saved separately from param arrays to account for changes to the sequences
    float prevBeat, prevDelay;
    
    // Flag to clear the delay line on the next beat
    bool clearDelayOnNextBeat;
    
    // For access in BlendronicDisplay
    Array<uint64> beatPositionsInBuffer; // Record of the sample position of beat changes in the delay buffer (used in display)
    int numBeatPositions; // Number of beat positions in the buffer and to be displayed
    int beatPositionsIndex; // Index of beat sample positions for adding/removing positions
    float pulseOffset; // Sample offset of the pulse grid from grid aligned with buffer start (used in display)

	JUCE_LEAK_DETECTOR(BlendronicProcessor);
};
#endif
