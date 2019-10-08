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
	BlendronicPreparation(String newName, Array<float> beats, Array<float> smoothTimes,
		Array<float> feedbackCoefficients, float smoothValue, float smoothDuration, BlendronicSmoothMode smoothMode,
        BlendronicSyncMode syncMode, BlendronicClearMode clearMode, BlendronicOpenMode openMode, BlendronicCloseMode closeMode, 
        float delayMax, float delayLength, float feedbackCoefficient);
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
	inline const Array<float> getSmoothDurations() const noexcept { return bSmoothDurations; }
	inline const Array<float> getFeedbackCoefficients() const noexcept { return bFeedbackCoefficients; }
	inline const float getDelayMax() const noexcept { return bDelayMax; }
	inline const float getDelayLength() const noexcept { return bDelayLength; }
	inline const float getSmoothValue() const noexcept { return bSmoothValue; }
	inline const float getSmoothDuration() const noexcept { return bSmoothDuration; }
    inline const BlendronicSmoothMode getSmoothMode() const noexcept { return bSmoothMode; }
	inline const float getInputThreshSEC() const noexcept { return bInputThreshSec; }
	inline const float getInputThreshMS() const noexcept { return bInputThresh; }
	inline const int getHoldMin() const noexcept { return holdMin; }
	inline const int getHoldMax() const noexcept { return holdMax; }
	inline const int getVelocityMin() const noexcept { return velocityMin; }
	inline const int getVelocityMax() const noexcept { return velocityMax; }
	inline const bool getActive() const noexcept { return isActive; }
	inline const bool getInputGain() const noexcept { return inputGain; }
    inline const BlendronicSyncMode getSyncMode() const noexcept { return bSyncMode; }
    inline const BlendronicClearMode getClearMode() const noexcept { return bClearMode; }
    inline const BlendronicOpenMode getOpenMode() const noexcept { return bOpenMode; }
    inline const BlendronicCloseMode getCloseMode() const noexcept { return bCloseMode; }

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
    inline const void setSmoothMode(BlendronicSmoothMode mode) { bSmoothMode = mode; }
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
    inline const void setSyncMode(BlendronicSyncMode mode) { bSyncMode = mode; }
    inline const void setClearMode(BlendronicClearMode mode) { bClearMode = mode; }
    inline const void setOpenMode(BlendronicOpenMode mode) { bOpenMode = mode; }
    inline const void setCloseMode(BlendronicCloseMode mode) { bCloseMode = mode; }

    // TODO
	void print(void)
    {
        ;
    }
    
    // TODO
	ValueTree getState(void)
    {
//        ValueTree prep("params");
//
//        prep.setProperty( "gain", getGain(), 0);
//        prep.setProperty( ptagSynchronic_numBeats,            getNumBeats(), 0);
//        prep.setProperty( ptagSynchronic_clusterMin,          getClusterMin(), 0);
//        prep.setProperty( ptagSynchronic_clusterMax,          getClusterMax(), 0);
//        prep.setProperty( ptagSynchronic_clusterCap,          getClusterCap(), 0);
//        prep.setProperty( ptagSynchronic_clusterThresh,       getClusterThreshMS(), 0);
//        prep.setProperty( ptagSynchronic_mode,                getMode(), 0);
//        prep.setProperty( ptagSynchronic_beatsToSkip,         getBeatsToSkip(), 0);
//
//        prep.setProperty( "numClusters", getNumClusters(), 0);
//        prep.setProperty( "onOffMode", getOnOffMode(), 0);
//
//        prep.setProperty( "holdMin", getHoldMin(), 0);
//        prep.setProperty( "holdMax", getHoldMax(), 0);
//
//        prep.setProperty( "velocityMin", getVelocityMin(), 0);
//        prep.setProperty( "velocityMax", getVelocityMax(), 0);
//
//        ValueTree beatMults( vtagSynchronic_beatMults);
//        int count = 0;
//        for (auto f : getBeatMultipliers())
//        {
//            beatMults.      setProperty( ptagFloat + String(count++), f, 0);
//        }
//        prep.addChild(beatMults, -1, 0);
//
//
//        ValueTree lenMults( vtagSynchronic_lengthMults);
//        count = 0;
//        for (auto f : getLengthMultipliers())
//        {
//            lenMults.       setProperty( ptagFloat + String(count++), f, 0);
//        }
//        prep.addChild(lenMults, -1, 0);
//
//
//        ValueTree accentMults( vtagSynchronic_accentMults);
//        count = 0;
//        for (auto f : getAccentMultipliers())
//        {
//            accentMults.    setProperty( ptagFloat + String(count++), f, 0);
//        }
//        prep.addChild(accentMults, -1, 0);
//
//
//        ValueTree transposition( vtagSynchronic_transpOffsets);
//
//        int tcount = 0;
//        for (auto arr : getTransposition())
//        {
//            ValueTree t("t"+String(tcount++));
//            count = 0;
//            for (auto f : arr)  t.setProperty( ptagFloat + String(count++), f, 0);
//            transposition.addChild(t,-1,0);
//        }
//        prep.addChild(transposition, -1, 0);
//
//        ValueTree ADSRs( vtagSynchronic_ADSRs);
//
//        tcount = 0;
//        for (auto arr : getADSRs())
//        {
//            ValueTree e("e"+String(tcount++));
//            count = 0;
//            for (auto f : arr)  e.setProperty( ptagFloat + String(count++), f, 0);
//            ADSRs.addChild(e,-1,0);
//        }
//        prep.addChild(ADSRs, -1, 0);
//
//        return prep;
    }
    
    // TODO
	void setState(XmlElement* e)
    {
//        String n; int i; float f; bool b;
//
//        n = e->getStringAttribute("gain");
//        if (n != "") setGain(n.getFloatValue());
//        else         setGain(1.0);
//
//        i = e->getStringAttribute(ptagSynchronic_numBeats).getIntValue();
//        setNumBeats(i);
//
//        i = e->getStringAttribute(ptagSynchronic_clusterMin).getIntValue();
//        setClusterMin(i);
//
//        i = e->getStringAttribute(ptagSynchronic_clusterMax).getIntValue();
//        setClusterMax(i);
//
//        //i = e->getStringAttribute(ptagSynchronic_clusterCap).getIntValue();
//        n = e->getStringAttribute(ptagSynchronic_clusterCap);
//        if (n != "")    setClusterCap(n.getIntValue());
//        else            setClusterCap(8);
//
//        n = e->getStringAttribute("holdMin");
//
//        if (n != "")    setHoldMin(n.getIntValue());
//        else            setHoldMin(0);
//
//        n = e->getStringAttribute("holdMax");
//
//        if (n != "")    setHoldMax(n.getIntValue());
//        else            setHoldMax(12000);
//
//        n = e->getStringAttribute("velocityMin");
//
//        if (n != "")    setVelocityMin(n.getIntValue());
//        else            setVelocityMin(0);
//
//        n = e->getStringAttribute("velocityMax");
//
//        if (n != "")    setVelocityMax(n.getIntValue());
//        else            setVelocityMax(127);
//
//        f = e->getStringAttribute(ptagSynchronic_clusterThresh).getFloatValue();
//        setClusterThresh(f);
//
//        i = e->getStringAttribute(ptagSynchronic_mode).getIntValue();
//        setMode((SynchronicSyncMode) i);
//
//        i = e->getStringAttribute(ptagSynchronic_beatsToSkip).getIntValue();
//        setBeatsToSkip(i);
//
//        n = e->getStringAttribute("numClusters");
//
//        if (n != String())     setNumClusters(n.getIntValue());
//        else                        setNumClusters(1);
//
//        n = e->getStringAttribute("onOffMode");
//
//        if (n != String())     setOnOffMode((SynchronicOnOffMode) n.getIntValue());
//        else                        setOnOffMode(KeyOn);
//
//        forEachXmlChildElement (*e, sub)
//        {
//            if (sub->hasTagName(vtagSynchronic_beatMults))
//            {
//                Array<float> beats;
//                for (int k = 0; k < 128; k++)
//                {
//                    String attr = sub->getStringAttribute(ptagFloat + String(k));
//
//                    if (attr == String()) break;
//                    else
//                    {
//                        f = attr.getFloatValue();
//                        beats.add(f);
//                    }
//                }
//
//                setBeatMultipliers(beats);
//
//            }
//            else  if (sub->hasTagName(vtagSynchronic_accentMults))
//            {
//                Array<float> accents;
//                for (int k = 0; k < 128; k++)
//                {
//                    String attr = sub->getStringAttribute(ptagFloat + String(k));
//
//                    if (attr == String()) break;
//                    else
//                    {
//                        f = attr.getFloatValue();
//                        accents.add(f);
//                    }
//                }
//
//                setAccentMultipliers(accents);
//
//            }
//            else  if (sub->hasTagName(vtagSynchronic_lengthMults))
//            {
//                Array<float> lens;
//                for (int k = 0; k < 128; k++)
//                {
//                    String attr = sub->getStringAttribute(ptagFloat + String(k));
//
//                    if (attr == String()) break;
//                    else
//                    {
//                        f = attr.getFloatValue();
//                        lens.add(f);
//                    }
//                }
//
//                setLengthMultipliers(lens);
//
//            }
//            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
//            {
//                Array<Array<float>> atransp;
//                int tcount = 0;
//                forEachXmlChildElement (*sub, asub)
//                {
//                    if (asub->hasTagName("t"+String(tcount++)))
//                    {
//                        Array<float> transp;
//                        for (int k = 0; k < 128; k++)
//                        {
//                            String attr = asub->getStringAttribute(ptagFloat + String(k));
//
//                            if (attr == String()) break;
//                            else
//                            {
//                                f = attr.getFloatValue();
//                                transp.add(f);
//                            }
//                        }
//                        atransp.set(tcount-1, transp);
//                    }
//                }
//
//                setTransposition(atransp);
//            }
//            else  if (sub->hasTagName(vtagSynchronic_ADSRs))
//            {
//                Array<Array<float>> aADSRs;
//                int tcount = 0;
//                forEachXmlChildElement (*sub, asub)
//                {
//                    if (asub->hasTagName("e"+String(tcount++)))
//                    {
//                        Array<float> singleADSR;
//                        for (int k = 0; k < 5; k++)
//                        {
//                            String attr = asub->getStringAttribute(ptagFloat + String(k));
//
//                            if (attr == String()) break;
//                            else
//                            {
//                                f = attr.getFloatValue();
//                                singleADSR.add(f);
//                            }
//                        }
//                        aADSRs.set(tcount-1, singleADSR);
//                    }
//                }
//
//                setADSRs(aADSRs);
//            }
//        }
    }

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
    BlendronicSmoothMode bSmoothMode;

	//signal chain stk classes
	float bFeedbackCoefficient;

	//bK stuff
	float bInputThresh;
	float bInputThreshSec;
	int holdMin, holdMax, velocityMin, velocityMax;
    
    BlendronicSyncMode bSyncMode;
    BlendronicClearMode bClearMode;
    BlendronicOpenMode bOpenMode;
    BlendronicCloseMode bCloseMode;

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
		BlendronicDelay::Ptr delayL,
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
    inline int getBeatIndex(void) { return beatIndex; }
    inline int getSmoothIndex(void) { return smoothIndex; }
    inline int getFeedbackIndex(void) { return feedbackIndex; }
    inline BKSynthesiser* getSynth(void) { return synth; }
    inline Array<int> getKeysDepressed(void) { return keysDepressed; }


	//mutators
	inline void setBlendronic(Blendronic::Ptr blend) { blendronic = blend; }
	inline void setTempo(TempoProcessor::Ptr temp) { tempo = temp; }
    inline void setSampleTimer(uint64 sampleTime) { sampleTimer = sampleTime; }
    inline void setBeatIndex(int index) { beatIndex = index; }
    inline void setSmoothIndex(int index) { smoothIndex = index; }
    inline void setFeedbackIndex(int index) { feedbackIndex = index; }
	void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr; }
    inline void setClearDelayOnNextBeat(bool clear) { clearDelayOnNextBeat = clear; }
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
	BKSynthesiser* synth;
	GeneralSettings::Ptr general;

	Blendronic::Ptr blendronic;
	TempoProcessor::Ptr tempo;

	BlendronicDelay::Ptr delay;
    
    Keymap::PtrArr      keymaps;

	double sampleRate;

	void playNote(int channel, int note, float velocity);
	Array<float> velocities;    //record of velocities
    Array<uint64> holdTimers;
	Array<int> keysDepressed;   //current keys that are depressed

	float numSamplesBeat;          // = beatThresholdSamples * beatMultiplier
	uint64 sampleTimer;
    int beatIndex, smoothIndex, feedbackIndex;
    float prevBeat;
    bool clearDelayOnNextBeat;

	//JUCE_LEAK_DETECTOR(BlendronicProcessor);
};
#endif