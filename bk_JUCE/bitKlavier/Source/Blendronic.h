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
#include "BlendronicDisplay.h"


// Forward declaration to allow include of Blendronic in BKSynthesiser
class BKSynthesiser;

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONIC PREPARATION///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class BlendronicModification;

class BlendronicPreparation : public ReferenceCountedObject
{

public:
	typedef ReferenceCountedObjectPtr<BlendronicPreparation>   Ptr;
	typedef Array<BlendronicPreparation::Ptr>                  PtrArr;
	typedef Array<BlendronicPreparation::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BlendronicPreparation>                  Arr;
	typedef OwnedArray<BlendronicPreparation, CriticalSection> CSArr;


    //copy constructor
    BlendronicPreparation(BlendronicPreparation::Ptr b) :
    outGain(b->outGain),
    bBeats(b->bBeats),
    bDelayLengths(b->bDelayLengths),
    bSmoothLengths(b->bSmoothLengths),
    bFeedbackCoefficients(b->bFeedbackCoefficients),
    bBeatsStates(b->bBeatsStates),
    bDelayLengthsStates(b->bDelayLengthsStates),
    bSmoothLengthsStates(b->bSmoothLengthsStates),
    bFeedbackCoefficientsStates(b->bFeedbackCoefficientsStates),
    delayBufferSizeInSeconds( b->delayBufferSizeInSeconds),
    targetTypeBlendronicPatternSync(b->getTargetTypeBlendronicPatternSync()),
    targetTypeBlendronicBeatSync(b->getTargetTypeBlendronicBeatSync()),
    targetTypeBlendronicClear(b->getTargetTypeBlendronicClear()),
    targetTypeBlendronicPausePlay(b->getTargetTypeBlendronicPausePlay()),
    targetTypeBlendronicOpenCloseInput(b->getTargetTypeBlendronicOpenCloseInput()),
    targetTypeBlendronicOpenCloseOutput(b->getTargetTypeBlendronicOpenCloseOutput())
    {
    }
    
    //constructor with input
    BlendronicPreparation(String newName,
                          Array<float> beats,
                          Array<float> delayLengths,
                          Array<float> smoothLengths,
                          Array<float> feedbackCoefficients,
                          float clusterThresh,
                          float delayBufferSizeInSeconds) :
    outGain(1.0f),
    bBeats(beats),
    bDelayLengths(delayLengths),
    bSmoothLengths(smoothLengths),
    bFeedbackCoefficients(feedbackCoefficients),
    bBeatsStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    bDelayLengthsStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    bSmoothLengthsStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    bFeedbackCoefficientsStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    delayBufferSizeInSeconds(delayBufferSizeInSeconds),
    name(newName),
    targetTypeBlendronicPatternSync(NoteOn),
    targetTypeBlendronicBeatSync(NoteOn),
    targetTypeBlendronicClear(NoteOn),
    targetTypeBlendronicPausePlay(NoteOn),
    targetTypeBlendronicOpenCloseInput(NoteOn),
    targetTypeBlendronicOpenCloseOutput(NoteOn)
    {
        
    }
    
    //empty constructor
    BlendronicPreparation(void) :
    outGain(0.0, true),
    bBeats(Array<float>({ 4., 3., 2., 3.})),
    bDelayLengths(Array<float>({ 4., 3., 2., 3.})),
    bSmoothLengths(Array<float>({ 50.0f })),
    bFeedbackCoefficients(Array<float>({ 0.95 })),
    bBeatsStates(Array<bool>({true, true, true, true, false, false, false, false, false, false, false, false, })),
    bDelayLengthsStates(Array<bool>({true, true, true, true, false, false, false, false, false, false, false, false, })),
    bSmoothLengthsStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    bFeedbackCoefficientsStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    delayBufferSizeInSeconds(5.f),
    targetTypeBlendronicPatternSync(NoteOn),
    targetTypeBlendronicBeatSync(NoteOn),
    targetTypeBlendronicClear(NoteOn),
    targetTypeBlendronicPausePlay(NoteOn),
    targetTypeBlendronicOpenCloseInput(NoteOn),
    targetTypeBlendronicOpenCloseOutput(NoteOn)
    {
        
    }

	// copy, modify, compare, randomize
	inline void copy(BlendronicPreparation::Ptr b)
    {
        outGain = b->outGain;
        bBeats = b->bBeats;
        bDelayLengths = b->bDelayLengths;
        bSmoothLengths = b->bSmoothLengths;
        bFeedbackCoefficients = b->bFeedbackCoefficients;
        bBeatsStates = b->bBeatsStates;
        bDelayLengthsStates = b->bDelayLengthsStates;
        bSmoothLengthsStates = b->bSmoothLengthsStates;
        bFeedbackCoefficientsStates = b->bFeedbackCoefficientsStates;
        delayBufferSizeInSeconds =  b->delayBufferSizeInSeconds;
        targetTypeBlendronicPatternSync = b->getTargetTypeBlendronicPatternSync();
        targetTypeBlendronicBeatSync = b->getTargetTypeBlendronicBeatSync();
        targetTypeBlendronicClear = b->getTargetTypeBlendronicClear();
        targetTypeBlendronicPausePlay = b->getTargetTypeBlendronicPausePlay();
        targetTypeBlendronicOpenCloseInput = b->getTargetTypeBlendronicOpenCloseInput();
        targetTypeBlendronicOpenCloseOutput = b->getTargetTypeBlendronicOpenCloseOutput();
    }
    
    void performModification(BlendronicModification* b, Array<bool> dirty);
    
    void stepModdables()
    {
        outGain.step();
        
        bBeats.step();
        bDelayLengths.step();
        bSmoothLengths.step();
        bFeedbackCoefficients.step();
        
        bBeatsStates.step();
        bDelayLengthsStates.step();
        bSmoothLengthsStates.step();
        bFeedbackCoefficientsStates.step();
        
        delayBufferSizeInSeconds.step();
    }
    
    void resetModdables()
    {
        outGain.reset();
        
        bBeats.reset();
        bDelayLengths.reset();
        bSmoothLengths.reset();
        bFeedbackCoefficients.reset();
        
        bBeatsStates.reset();
        bDelayLengthsStates.reset();
        bSmoothLengthsStates.reset();
        bFeedbackCoefficientsStates.reset();
        
        delayBufferSizeInSeconds.reset();
    }
    
	inline bool compare(BlendronicPreparation::Ptr b)
    {
        bool beats = true;
        bool delays = true;
        bool smooths = true;
        bool feedbacks = true;
        bool beatsStates = true;
        bool delaysStates = true;
        bool smoothsStates = true;
        bool feedbacksStates = true;
        
        for (int i = b->bBeats.value.size(); --i>=0;)
        {
            if (b->bBeats.value[i] != bBeats.value[i])
            {
                beats = false;
                break;
            }
        }
        
        for (int i = b->bBeatsStates.value.size(); --i>=0;)
        {
            if (b->bBeatsStates.value[i] != bBeatsStates.value[i])
            {
                beatsStates = false;
                break;
            }
        }
        
        for (int i = b->bDelayLengths.value.size(); --i>=0;)
        {
            if (b->bDelayLengths.value[i] != bDelayLengths.value[i])
            {
                delays = false;
                break;
            }
        }
        
        for (int i = b->bDelayLengthsStates.value.size(); --i>=0;)
        {
            if (b->bDelayLengthsStates.value[i] != bDelayLengthsStates.value[i])
            {
                delaysStates = false;
                break;
            }
        }
        
        for (int i = b->bSmoothLengths.value.size(); --i>=0;)
        {
            if (b->bSmoothLengths.value[i] != bSmoothLengths.value[i])
            {
                smooths = false;
                break;
            }
        }
        
        for (int i = b->bSmoothLengthsStates.value.size(); --i>=0;)
        {
            if (b->bSmoothLengthsStates.value[i] != bSmoothLengthsStates.value[i])
            {
                smoothsStates = false;
                break;
            }
        }
        
        for (int i  = b->bFeedbackCoefficients.value.size(); --i >= 0;)
        {
            if (b->bFeedbackCoefficients.value[i] != bFeedbackCoefficients.value[i])
            {
                feedbacks = false;
                break;
            }
        }
        
        for (int i  = b->bFeedbackCoefficientsStates.value.size(); --i >= 0;)
        {
            if (b->bFeedbackCoefficientsStates.value[i] != bFeedbackCoefficientsStates.value[i])
            {
                feedbacksStates = false;
                break;
            }
        }
        
        return  beats && delays && smooths && feedbacks &&
                beatsStates && delaysStates && smoothsStates && feedbacksStates &&
                outGain == b->outGain &&
                delayBufferSizeInSeconds == b->delayBufferSizeInSeconds &&
                targetTypeBlendronicPatternSync == b->getTargetTypeBlendronicPatternSync() &&
                targetTypeBlendronicBeatSync == b->getTargetTypeBlendronicBeatSync() &&
                targetTypeBlendronicClear == b->getTargetTypeBlendronicClear() &&
                targetTypeBlendronicPausePlay == b->getTargetTypeBlendronicPausePlay() &&
                targetTypeBlendronicOpenCloseInput == b->getTargetTypeBlendronicOpenCloseInput() &&
                targetTypeBlendronicOpenCloseOutput == b->getTargetTypeBlendronicOpenCloseOutput() ;
    }
    
    inline void randomize()
    {
        Random::getSystemRandom().setSeedRandomly();
        
        float r[100];
        
        for (int i = 0; i < 100; i++)  r[i] = (Random::getSystemRandom().nextFloat());
        int idx = 0;
        
        Array<float> fa;
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            fa.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
        }
        bBeats.set(fa);
        
        
        fa.clear();
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            fa.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
        }
        bDelayLengths.set(fa);
        
        fa.clear();
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            fa.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
        }
        bSmoothLengths.set(fa);
        
        fa.clear();
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            fa.add(i, (Random::getSystemRandom().nextFloat()));
        }
        bFeedbackCoefficients.set(fa);
        
        outGain.set(r[idx++]);
        delayBufferSizeInSeconds = r[idx++] * 4;
    }

	//accessors
	inline const String getName() const noexcept { return name; }
    inline void setName(String n) { name = n; }
    
    inline const TargetNoteMode getTargetTypeBlendronicBeatSync() const noexcept { return targetTypeBlendronicBeatSync; }
    inline const TargetNoteMode getTargetTypeBlendronicPatternSync() const noexcept { return targetTypeBlendronicPatternSync; }
    inline const TargetNoteMode getTargetTypeBlendronicClear() const noexcept { return targetTypeBlendronicClear; }
    inline const TargetNoteMode getTargetTypeBlendronicPausePlay() const noexcept { return targetTypeBlendronicPausePlay; }
    inline const TargetNoteMode getTargetTypeBlendronicOpenCloseInput() const noexcept { return targetTypeBlendronicOpenCloseInput; }
    inline const TargetNoteMode getTargetTypeBlendronicOpenCloseOutput() const noexcept { return targetTypeBlendronicOpenCloseOutput; }
    inline const TargetNoteMode getTargetTypeBlendronic(KeymapTargetType which)
    {
        if (which == TargetTypeBlendronicBeatSync)          return targetTypeBlendronicBeatSync;
        if (which == TargetTypeBlendronicPatternSync)   return targetTypeBlendronicPatternSync;
        if (which == TargetTypeBlendronicClear)         return targetTypeBlendronicClear;
        if (which == TargetTypeBlendronicPausePlay)     return targetTypeBlendronicPausePlay;
        if (which == TargetTypeBlendronicOpenCloseInput)  return targetTypeBlendronicOpenCloseInput;
        if (which == TargetTypeBlendronicOpenCloseOutput)  return targetTypeBlendronicOpenCloseOutput;
        return TargetNoteModeNil;
    }
    
    inline void setTargetTypeBlendronicPatternSync(TargetNoteMode nm)   { targetTypeBlendronicPatternSync = nm; }
    inline void setTargetTypeBlendronicBeatSync(TargetNoteMode nm)      { targetTypeBlendronicBeatSync = nm; }
    inline void setTargetTypeBlendronicClear(TargetNoteMode nm)         { targetTypeBlendronicClear = nm; }
    inline void setTargetTypeBlendronicPausePlay(TargetNoteMode nm)     { targetTypeBlendronicPausePlay = nm; }
    inline void setTargetTypeBlendronicOpenCloseInput(TargetNoteMode nm)  { targetTypeBlendronicOpenCloseInput = nm; }
    inline void setTargetTypeBlendronicOpenCloseOutput(TargetNoteMode nm)  { targetTypeBlendronicOpenCloseOutput = nm; }
    inline void setTargetTypeBlendronic(KeymapTargetType which, TargetNoteMode nm)
    {
        if (which == TargetTypeBlendronicBeatSync)          { targetTypeBlendronicBeatSync = nm; }
        if (which == TargetTypeBlendronicPatternSync)   { targetTypeBlendronicPatternSync = nm; }
        if (which == TargetTypeBlendronicClear)         { targetTypeBlendronicClear = nm; }
        if (which == TargetTypeBlendronicPausePlay)     { targetTypeBlendronicPausePlay = nm; }
        if (which == TargetTypeBlendronicOpenCloseInput)  { targetTypeBlendronicOpenCloseInput = nm; }
        if (which == TargetTypeBlendronicOpenCloseOutput)  { targetTypeBlendronicOpenCloseOutput = nm; }
    }

    // TODO
	void print(void)
    {
        DBG("| - - - Blendronic Preparation - - - |");
        DBG("bBeats: " + floatArrayToString(bBeats.value));
        DBG("bDelayLengths: " + floatArrayToString(bDelayLengths.value));
        DBG("bSmoothLengths: " + floatArrayToString(bSmoothLengths.value));
        DBG("bFeedbackCoefficients: " + floatArrayToString(bFeedbackCoefficients.value));
        DBG("outGain: " + String(outGain.value));
        DBG("delayBufferSizeInSeconds: " + String(delayBufferSizeInSeconds.value));
    }
    
    // TODO
	ValueTree getState(void)
    {
        ValueTree prep("params");
        
        outGain.getState(prep, ptagBlendronic_outGain);
        delayBufferSizeInSeconds.getState(prep, ptagBlendronic_delayBufferSize);
        
        bBeats.getState(prep, StringArray(vtagBlendronic_beats, ptagFloat));
        bBeatsStates.getState(prep, StringArray(vtagBlendronic_beatsStates, ptagBool));
        
        bDelayLengths.getState(prep, StringArray(vtagBlendronic_delayLengths, ptagFloat));
        bDelayLengthsStates.getState(prep, StringArray(vtagBlendronic_delayLengthsStates, ptagBool));
        
        bSmoothLengths.getState(prep, StringArray(vtagBlendronic_smoothLengths, ptagFloat));
        bSmoothLengthsStates.getState(prep, StringArray(vtagBlendronic_smoothLengthsStates, ptagBool));

        bFeedbackCoefficients.getState(prep, StringArray(vtagBlendronic_feedbackCoefficients, ptagFloat));
        bFeedbackCoefficientsStates.getState(prep, StringArray(vtagBlendronic_feedbackCoefficientsStates, ptagBool));
        
        prep.setProperty( ptagBlendronic_targetPatternSync, getTargetTypeBlendronicPatternSync(), 0);
        prep.setProperty( ptagBlendronic_targetBeatSync, getTargetTypeBlendronicBeatSync(), 0);
        prep.setProperty( ptagBlendronic_targetClear, getTargetTypeBlendronicClear(), 0);
        prep.setProperty( ptagBlendronic_targetPausePlay, getTargetTypeBlendronicPausePlay(), 0);
        prep.setProperty( ptagBlendronic_targetOpenCloseInput, getTargetTypeBlendronicOpenCloseInput(), 0);
        prep.setProperty( ptagBlendronic_targetOpenCloseOutput, getTargetTypeBlendronicOpenCloseOutput(), 0);

        return prep;
    }
    
    // TODO
	void setState(XmlElement* e)
    {
        outGain.setState(e, ptagBlendronic_outGain, 1.0f);
        
        delayBufferSizeInSeconds.setState(e, ptagBlendronic_delayBufferSize, 4.f);
        
        bBeats.setState(e, StringArray(vtagBlendronic_beats, ptagFloat), 1.0);
        Array<bool> states;
        for (int i = 0; i < bBeats.base.size(); ++i) states.add(true);
        bBeatsStates.setState(e, StringArray(vtagBlendronic_beatsStates, ptagBool), states);
        
        bDelayLengths.setState(e, StringArray(vtagBlendronic_delayLengths, ptagFloat), 1.0);
        states.clear();
        for (int i = 0; i < bDelayLengths.base.size(); ++i) states.add(true);
        bDelayLengthsStates.setState(e, StringArray(vtagBlendronic_delayLengthsStates, ptagBool), states);
        
        bSmoothLengths.setState(e, StringArray(vtagBlendronic_smoothLengths, ptagFloat), 50.0);
        states.clear();
        for (int i = 0; i < bSmoothLengths.base.size(); ++i) states.add(true);
        bSmoothLengthsStates.setState(e, StringArray(vtagBlendronic_smoothLengthsStates, ptagBool), states);
        
        bFeedbackCoefficients.setState(e, StringArray(vtagBlendronic_feedbackCoefficients, ptagFloat), 0.95);
        states.clear();
        for (int i = 0; i < bFeedbackCoefficients.base.size(); ++i) states.add(true);
        bFeedbackCoefficientsStates.setState(e, StringArray(vtagBlendronic_feedbackCoefficientsStates, ptagBool), states);
        
        int i = e->getStringAttribute(ptagBlendronic_targetPatternSync).getIntValue();
        setTargetTypeBlendronicPatternSync((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagBlendronic_targetBeatSync).getIntValue();
        setTargetTypeBlendronicBeatSync((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagBlendronic_targetClear).getIntValue();
        setTargetTypeBlendronicClear((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagBlendronic_targetPausePlay).getIntValue();
        setTargetTypeBlendronicPausePlay((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagBlendronic_targetOpenCloseInput).getIntValue();
        setTargetTypeBlendronicOpenCloseInput((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagBlendronic_targetOpenCloseOutput).getIntValue();
        setTargetTypeBlendronicOpenCloseOutput((TargetNoteMode)i);
    }
    
    bool modded = false;
    
    // output gain
    Moddable<float> outGain;
    
    // sequenced parameters
    Moddable<Array<float>> bBeats;
    Moddable<Array<float>> bDelayLengths;
    Moddable<Array<float>> bSmoothLengths;
    Moddable<Array<float>> bFeedbackCoefficients;
    
    // slider states for sequenced params
    Moddable<Array<bool>> bBeatsStates;
    Moddable<Array<bool>> bDelayLengthsStates;
    Moddable<Array<bool>> bSmoothLengthsStates;
    Moddable<Array<bool>> bFeedbackCoefficientsStates;
    
    Moddable<float> delayBufferSizeInSeconds;

private:
    
    String name;
    
    TargetNoteMode targetTypeBlendronicPatternSync;
    TargetNoteMode targetTypeBlendronicBeatSync;
    TargetNoteMode targetTypeBlendronicClear;
    TargetNoteMode targetTypeBlendronicPausePlay;
    TargetNoteMode targetTypeBlendronicOpenCloseInput;
    TargetNoteMode targetTypeBlendronicOpenCloseOutput;
    
    JUCE_LEAK_DETECTOR(BlendronicPreparation);
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
        prep(new BlendronicPreparation(prep)),
		Id(Id),
		name("Blendronic "+String(Id))
	{

	}

	Blendronic(int Id, bool random = false) :
		Id(Id),
		name("Blendronic "+String(Id))
	{
		prep = new BlendronicPreparation();
		if (random) randomize();
	}

	inline Blendronic::Ptr duplicate()
	{
		BlendronicPreparation::Ptr copyPrep = new BlendronicPreparation(prep);

		Blendronic::Ptr copy = new Blendronic(copyPrep, -1);

		copy->setName(name);

		return copy;
	}

	inline void clear(void)
	{
		prep = new BlendronicPreparation();	}

	inline void copy(Blendronic::Ptr from)
	{
		prep->copy(from->prep);
	}

    // for unit-testing
	inline void randomize()
	{
		clear();
		prep->randomize();
		Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
		name = "random";
	}
	
	inline ValueTree getState(bool active = false)
	{
		ValueTree vt(vtagBlendronic);

		vt.setProperty("Id", Id, 0);
		vt.setProperty("name", name, 0);

		vt.addChild(prep->getState(), -1, 0);
        // prep.addChild(active ? aPrep->getState() : prep->getState(), -1, 0);

		return vt;
	}

	inline void setState(XmlElement* e)
	{
		Id = e->getStringAttribute("Id").getIntValue();

		String n = e->getStringAttribute("name");

		if (n != String())     name = n;
		else                   name = String(Id);


		XmlElement* params = e->getChildByName("params");

		if (params != nullptr)
		{
			prep->setState(params);
		}
		else
		{
			prep->setState(e);
		}
	}

	~Blendronic() {};

	inline int getId() { return Id; }
	inline void setId(int newId) { Id = newId; }
	inline void setName(String newName) { name = newName; }
	inline String getName() const noexcept { return name; }

	BlendronicPreparation::Ptr prep;
    
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

	//begin timing played note length, called with noteOn
	void keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress);

	//begin playing reverse note, called with noteOff
	void keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress);

	void postRelease(int noteNumber, int midiChannel);

	void prepareToPlay(double sr);

	//accessors
	inline Blendronic::Ptr getBlendronic(void) const noexcept { return blendronic; }
	inline TempoProcessor::Ptr getTempo(void) const noexcept { return tempo; }
	inline BlendronicDelay* getDelay(void) const noexcept { return delay; }
	inline int getId(void) const noexcept { return blendronic->getId(); }
    inline int getTempoId(void) const noexcept { return tempo->getId(); }
	inline const float getCurrentNumSamplesBeat(void) const noexcept { return numSamplesBeat; }
    inline uint64 getSampleTime(void) const noexcept { return sampleTimer; }
    inline const int getInPoint() const noexcept { return delay->getInPoint(); }
    inline const int getOutPoint() const noexcept { return delay->getOutPoint(); }
    inline int getBeatIndex(void) const noexcept { return beatIndex; }
    inline int getDelayIndex(void) const noexcept { return delayIndex; }
    inline int getSmoothIndex(void) const noexcept { return smoothIndex; }
    inline int getFeedbackIndex(void) const noexcept { return feedbackIndex; }
    inline BKSynthesiser* getSynth(void) const noexcept { return synth; }
    inline Array<int> getKeysDepressed(void) const noexcept { return keysDepressed; }
    inline const AudioBuffer<float>* getDelayBuffer(void) const noexcept { return delay->getDelayBuffer(); }
    inline const bool getActive() const noexcept { return blendronicActive; }
    inline const bool getInputState() const noexcept { return delay->getInputState(); }
    inline const bool getOutputState() const noexcept { return delay->getOutputState(); }
    inline const Array<uint64> getBeatPositionsInBuffer() const noexcept { return beatPositionsInBuffer; }
    inline const float getPulseOffset() const noexcept { return pulseOffset; }
    
    float getPulseLengthInSamples(); 
    
	//mutators
	inline void setBlendronic(Blendronic::Ptr blend) { blendronic = blend; }
	inline void setTempo(TempoProcessor::Ptr temp) { tempo = temp; }
    inline void setSampleTimer(uint64 sampleTime) { sampleTimer = sampleTime; }
    inline void setBeatIndex(int index) { beatIndex = index; }
    inline void setDelayIndex(int index) { delayIndex = index; }
    inline void setSmoothIndex(int index) { smoothIndex = index; }
    inline void setFeedbackIndex(int index) { feedbackIndex = index; }
    inline void setClearDelayOnNextBeat(bool clear) { clearDelayOnNextBeat = clear; }
    inline void setActive(bool newActive) { blendronicActive = newActive; }
    inline void toggleActive() { blendronicActive = !blendronicActive; }
    inline void setInputState(bool inputState) { delay->setInputState(inputState); }
    inline void toggleInput() { delay->toggleInput(); }
    inline void setOutputState(bool inputState) { delay->setOutputState(inputState); }
    inline void toggleOutput() { delay->toggleOutput(); }
    
    inline void reset(void)
    {
        blendronic->prep->resetModdables();
        DBG("blendronic reset called");
    }
    
    inline const bool getResetPhase() const noexcept { return resetPhase; }
    inline const void setResetPhase(bool reset) { resetPhase = reset; }
    
    void setDelayBufferSizeInSeconds(float size);
    
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
    
    inline OwnedArray<BlendronicDisplay::ChannelInfo>* getAudioDisplayData(void) { return &audio; }
    
    inline BlendronicDisplay::ChannelInfo* getSmoothingDisplayData(void) { return smoothing.get(); }
    
    Array<Array<float>>& getVelocities() { return velocities; }
    Array<Array<float>>& getInvertVelocities() { return invertVelocities; }
    
    void setVelocities(Array<Array<float>>& newVel) { velocities = newVel; }
    void setInvertVelocities(Array<Array<float>>& newVel) { invertVelocities = newVel; }

private:
    CriticalSection lock;
    /* Private Functions */

	void playNote(int channel, int note, float velocity);
    
    /* Private Variables */
    
    Blendronic::Ptr         blendronic;
    BKSynthesiser*          synth;
    TempoProcessor::Ptr     tempo;
    GeneralSettings::Ptr    general;
    BlendronicDelay::Ptr    delay;
    Keymap::PtrArr          keymaps;
    
    bool blendronicActive;
    
    Array<Array<float>> velocities;
    Array<Array<float>> invertVelocities;
    Array<uint64> holdTimers;
	Array<int> keysDepressed;   //current keys that are depressed
    
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
    float prevBeat, prevDelay, prevPulseLength;
    
    // Flag to clear the delay line on the next beat
    bool clearDelayOnNextBeat;
    
    // For access in BlendronicDisplay
    Array<uint64> beatPositionsInBuffer; // Record of the sample position of beat changes in the delay buffer (used in display)
    int numBeatPositions; // Number of beat positions in the buffer and to be displayed
    int beatPositionsIndex; // Index of beat sample positions for adding/removing positions
    float pulseOffset; // Sample offset of the pulse grid from grid aligned with buffer start (used in display)
    bool resetPhase;
    
    OwnedArray<BlendronicDisplay::ChannelInfo> audio;
    std::unique_ptr<BlendronicDisplay::ChannelInfo> smoothing;

	JUCE_LEAK_DETECTOR(BlendronicProcessor);
};
#endif
