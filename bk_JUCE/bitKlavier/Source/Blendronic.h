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
                          Array<float> smoothLengths,
                          Array<float> smoothValues,
                          Array<float> feedbackCoefficients,
                          float clusterThresh,
                          float delayBufferSizeInSeconds);
	BlendronicPreparation(void);

	// copy, modify, compare, randomize
	inline void copy(BlendronicPreparation::Ptr b)
    {
        outGain = b->outGain;
        bBeats = b->getBeats();
        bDelayLengths = b->getDelayLengths();
        bSmoothLengths = b->getSmoothLengths();
        bSmoothValues = b->getSmoothValues();
        bFeedbackCoefficients = b->getFeedbackCoefficients();
        bBeatsStates = b->getBeatsStates();
        bDelayLengthsStates = b->getDelayLengthsStates();
        bSmoothLengthsStates = b->getSmoothLengthsStates();
        bSmoothValuesStates = b->getSmoothValuesStates();
        bFeedbackCoefficientsStates = b->getFeedbackCoefficientsStates();
        bSmoothBase = b->getSmoothBase();
        bSmoothScale = b->getSmoothScale();
        targetTypeBlendronicPatternSync = b->getTargetTypeBlendronicPatternSync();
        targetTypeBlendronicBeatSync = b->getTargetTypeBlendronicBeatSync();
        targetTypeBlendronicClear = b->getTargetTypeBlendronicClear();
        targetTypeBlendronicPausePlay = b->getTargetTypeBlendronicPausePlay();
        targetTypeBlendronicOpenCloseInput = b->getTargetTypeBlendronicOpenCloseInput();
        targetTypeBlendronicOpenCloseOutput = b->getTargetTypeBlendronicOpenCloseOutput();
        delayBufferSizeInSeconds =  b->getDelayBufferSizeInSeconds();
    }
    
    void performModification(BlendronicPreparation::Ptr b, Array<bool> dirty)
    {
        if (dirty[BlendronicOutGain]) outGain.modTo(b->outGain);
        if (dirty[BlendronicBeats]) {
            bBeats = b->getBeats();
            bBeatsStates = b->getBeatsStates();
        }
        if (dirty[BlendronicDelayLengths]) {
            bDelayLengths = b->getDelayLengths();
            bDelayLengthsStates = b->getDelayLengthsStates();
        }
        if (dirty[BlendronicSmoothLengths]) {
            bSmoothLengths = b->getSmoothLengths();
            bSmoothLengthsStates = b->getSmoothLengthsStates();
        }
        if (dirty[BlendronicSmoothValues]) {
            bSmoothValues = b->getSmoothValues();
            bSmoothValuesStates = b->getSmoothValuesStates();
        }
        if (dirty[BlendronicFeedbackCoeffs]) {
            bFeedbackCoefficients = b->getFeedbackCoefficients();
            bFeedbackCoefficientsStates = b->getFeedbackCoefficientsStates();
        }
        if (dirty[BlendronicDelayBufferSize]) delayBufferSizeInSeconds = b->getDelayBufferSizeInSeconds();
    }
    
    void stepModdables()
    {
        outGain.step();
    }
    
    void resetModdables()
    {
        outGain.reset();
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
        
        for (int i = b->getBeats().size(); --i>=0;)
        {
            if (b->getBeats()[i] != bBeats[i])
            {
                beats = false;
                break;
            }
        }
        
        for (int i = b->getBeatsStates().size(); --i>=0;)
        {
            if (b->getBeatsStates()[i] != bBeatsStates[i])
            {
                beatsStates = false;
                break;
            }
        }
        
        for (int i = b->getDelayLengths().size(); --i>=0;)
        {
            if (b->getDelayLengths()[i] != bDelayLengths[i])
            {
                delays = false;
                break;
            }
        }
        
        for (int i = b->getDelayLengthsStates().size(); --i>=0;)
        {
            if (b->getDelayLengthsStates()[i] != bDelayLengthsStates[i])
            {
                delaysStates = false;
                break;
            }
        }
        
        for (int i = b->getSmoothLengths().size(); --i>=0;)
        {
            if (b->getSmoothLengths()[i] != bSmoothLengths[i])
            {
                smooths = false;
                break;
            }
        }
        
        for (int i = b->getSmoothLengthsStates().size(); --i>=0;)
        {
            if (b->getSmoothLengthsStates()[i] != bSmoothLengthsStates[i])
            {
                smoothsStates = false;
                break;
            }
        }
        
        for (int i  = b->getFeedbackCoefficients().size(); --i >= 0;)
        {
            if (b->getFeedbackCoefficients()[i] != bFeedbackCoefficients[i])
            {
                feedbacks = false;
                break;
            }
        }
        
        for (int i  = b->getFeedbackCoefficientsStates().size(); --i >= 0;)
        {
            if (b->getFeedbackCoefficientsStates()[i] != bFeedbackCoefficientsStates[i])
            {
                feedbacksStates = false;
                break;
            }
        }
        
        return  beats && delays && smooths && feedbacks &&
                beatsStates && delaysStates && smoothsStates && feedbacksStates &&
                outGain == b->outGain &&
                bSmoothBase == b->getSmoothBase() &&
                bSmoothScale == b->getSmoothScale() &&
                targetTypeBlendronicPatternSync == b->getTargetTypeBlendronicPatternSync() &&
                targetTypeBlendronicBeatSync == b->getTargetTypeBlendronicBeatSync() &&
                targetTypeBlendronicClear == b->getTargetTypeBlendronicClear() &&
                targetTypeBlendronicPausePlay == b->getTargetTypeBlendronicPausePlay() &&
                targetTypeBlendronicOpenCloseInput == b->getTargetTypeBlendronicOpenCloseInput() &&
                targetTypeBlendronicOpenCloseOutput == b->getTargetTypeBlendronicOpenCloseOutput() &&
                delayBufferSizeInSeconds == b->getDelayBufferSizeInSeconds() ;
    }
    
    inline void randomize()
    {
        Random::getSystemRandom().setSeedRandomly();
        
        float r[100];
        
        for (int i = 0; i < 100; i++)  r[i] = (Random::getSystemRandom().nextFloat());
        int idx = 0;
        
        bSmoothBase = (BlendronicSmoothBase)(int)(r[idx++] * BlendronicSmoothBaseNil);
        bSmoothScale = (BlendronicSmoothScale)(int)(r[idx++] * BlendronicSmoothScaleNil);
        
        bBeats.clear();
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            bBeats.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
        }
        bDelayLengths.clear();
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            bDelayLengths.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
        }
        bSmoothLengths.clear();
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            bSmoothLengths.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
        }
        bFeedbackCoefficients.clear();
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            bFeedbackCoefficients.add(i, (Random::getSystemRandom().nextFloat()));
        }
        
        outGain.set(r[idx++]);
        delayBufferSizeInSeconds = r[idx++] * 4;
    }

	//accessors
	inline const String getName() const noexcept { return name; }
    
	inline const Array<float> getBeats() const noexcept { return bBeats; }
    inline const Array<float> getDelayLengths() const noexcept { return bDelayLengths; }
    inline const Array<float> getSmoothLengths() const noexcept { return bSmoothLengths; }
	inline const Array<float> getSmoothValues() const noexcept { return bSmoothValues; }
	inline const Array<float> getFeedbackCoefficients() const noexcept { return bFeedbackCoefficients; }
    
    inline const Array<bool> getBeatsStates() const noexcept { return bBeatsStates; }
    inline const Array<bool> getDelayLengthsStates() const noexcept { return bDelayLengthsStates; }
    inline const Array<bool> getSmoothLengthsStates() const noexcept { return bSmoothLengthsStates; }
    inline const Array<bool> getSmoothValuesStates() const noexcept { return bSmoothValuesStates; }
    inline const Array<bool> getFeedbackCoefficientsStates() const noexcept { return bFeedbackCoefficientsStates; }
    
    inline const float getDelayBufferSizeInSeconds() const noexcept { return delayBufferSizeInSeconds; }
	
    inline const BlendronicSmoothBase getSmoothBase() const noexcept { return bSmoothBase; }
    inline const BlendronicSmoothScale getSmoothScale() const noexcept { return bSmoothScale; }
	
	//mutators
	inline void setName(String n) { name = n; }
	inline void setBeats(Array<float> beats) { bBeats.swapWith(beats); }
    inline void setDelayLengths(Array<float> delayLengths) { bDelayLengths.swapWith(delayLengths); }
    inline void setSmoothLengths(Array<float> smoothLengths) { bSmoothLengths.swapWith(smoothLengths); }
	inline void setSmoothValues(Array<float> smoothValues) { bSmoothValues.swapWith(smoothValues); }
	inline void setFeedbackCoefficients(Array<float> feedbackCoefficients) { bFeedbackCoefficients.swapWith(feedbackCoefficients); }
    
    inline void setBeatsStates(Array<bool> beats) { bBeatsStates.swapWith(beats); }
    inline void setDelayLengthsStates(Array<bool> delayLengths) { bDelayLengthsStates.swapWith(delayLengths); }
    inline void setSmoothLengthsStates(Array<bool> smoothLengths) { bSmoothLengthsStates.swapWith(smoothLengths); }
    inline void setSmoothValuesStates(Array<bool> smoothValues) { bSmoothValuesStates.swapWith(smoothValues); }
    inline void setFeedbackCoefficientsStates(Array<bool> feedbackCoefficients) { bFeedbackCoefficientsStates.swapWith(feedbackCoefficients); }

    inline void setDelayBufferSizeInSeconds(float size) { delayBufferSizeInSeconds = size; }
    
    inline void setBeat(int whichSlider, float value) { bBeats.set(whichSlider, value); }
    inline void setDelayLength(int whichSlider, float value) { bDelayLengths.set(whichSlider, value); }
    inline void setSmoothLengths(int whichSlider, float value) { bSmoothLengths.set(whichSlider, value); }
    inline void setSmoothValue(int whichSlider, float value) { bSmoothValues.set(whichSlider, value); }
    inline void setFeedbackCoefficient(int whichSlider, float value) { bFeedbackCoefficients.set(whichSlider, value); }
    
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
        DBG("bBeats: " + floatArrayToString(bBeats));
        DBG("bDelayLengths: " + floatArrayToString(bDelayLengths));
        DBG("bSmoothLengths: " + floatArrayToString(bSmoothLengths));
        DBG("bFeedbackCoefficients: " + floatArrayToString(bFeedbackCoefficients));
        DBG("outGain: " + String(outGain.value));
        DBG("delayBufferSizeInSeconds: " + String(delayBufferSizeInSeconds));
    }
    
    // TODO
	ValueTree getState(void)
    {
        ValueTree prep("params");
        
        outGain.getState(prep, ptagBlendronic_outGain);
        prep.setProperty( ptagBlendronic_targetPatternSync, getTargetTypeBlendronicPatternSync(), 0);
        prep.setProperty( ptagBlendronic_targetBeatSync, getTargetTypeBlendronicBeatSync(), 0);
        prep.setProperty( ptagBlendronic_targetClear, getTargetTypeBlendronicClear(), 0);
        prep.setProperty( ptagBlendronic_targetPausePlay, getTargetTypeBlendronicPausePlay(), 0);
        prep.setProperty( ptagBlendronic_targetOpenCloseInput, getTargetTypeBlendronicOpenCloseInput(), 0);
        prep.setProperty( ptagBlendronic_targetOpenCloseOutput, getTargetTypeBlendronicOpenCloseOutput(), 0);
        prep.setProperty( ptagBlendronic_delayBufferSize, getDelayBufferSizeInSeconds(), 0);

        ValueTree beats(vtagBlendronic_beats);
        int count = 0;
        for (auto f : getBeats())
        {
            beats.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(beats, -1, 0);
        
        ValueTree beatsStates(vtagBlendronic_beatsStates);
        count = 0;
        for (auto f : getBeatsStates())
        {
            beatsStates.setProperty(ptagBool + String(count++), f ? 1 : 0, 0);
        }
        prep.addChild(beatsStates, -1, 0);
     
        
        ValueTree delayLengths(vtagBlendronic_delayLengths);
        count = 0;
        for (auto f : getDelayLengths())
        {
            delayLengths.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(delayLengths, -1, 0);
        
        ValueTree delayLengthsStates(vtagBlendronic_delayLengthsStates);
        count = 0;
        for (auto f : getDelayLengthsStates())
        {
            delayLengthsStates.setProperty(ptagBool + String(count++), f ? 1 : 0, 0);
        }
        prep.addChild(delayLengthsStates, -1, 0);
        
        /*
         ValueTree beatMultsStates( vtagSynchronic_beatMultsStates);
         count = 0;
         for (auto f : getBeatMultipliersStates())
         {
             beatMultsStates.setProperty( ptagBool + String(count++), f ? 1 : 0, 0);
         }
         prep.addChild(beatMultsStates, -1, 0);
         */
        
        
        ValueTree smoothLengths(vtagBlendronic_smoothLengths);
        count = 0;
        for (auto f : getSmoothLengths())
        {
            smoothLengths.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(smoothLengths, -1, 0);
        
        ValueTree smoothLengthsStates(vtagBlendronic_smoothLengthsStates);
        count = 0;
        for (auto f : getSmoothLengthsStates())
        {
            smoothLengthsStates.setProperty(ptagBool + String(count++), f ? 1 : 0, 0);
        }
        prep.addChild(smoothLengthsStates, -1, 0);
        
//        ValueTree smoothValues(vtagBlendronic_smoothValues);
//        count = 0;
//        for (auto f : getSmoothValues())
//        {
//            smoothValues.setProperty(ptagFloat + String(count++), f, 0);
//        }
//        prep.addChild(smoothValues, -1, 0);

        ValueTree feedbackCoefficients(vtagBlendronic_feedbackCoefficients);
        count = 0;
        for (auto f : getFeedbackCoefficients())
        {
            feedbackCoefficients.setProperty(ptagFloat + String(count++), f, 0);
        }
        prep.addChild(feedbackCoefficients, -1, 0);
        
        ValueTree feedbackCoefficientsStates(vtagBlendronic_feedbackCoefficientsStates);
        count = 0;
        for (auto f : getFeedbackCoefficientsStates())
        {
            feedbackCoefficientsStates.setProperty(ptagBool + String(count++), f ? 1 : 0, 0);
        }
        prep.addChild(feedbackCoefficientsStates, -1, 0);

        return prep;
    }
    
    // TODO
	void setState(XmlElement* e)
    {
        String n; float f; int i;
        
        outGain.setState(e, ptagBlendronic_outGain, 1.0f);
        
        i = e->getStringAttribute(ptagBlendronic_targetPatternSync).getIntValue();
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
        
        f = e->getStringAttribute(ptagBlendronic_delayBufferSize).getFloatValue();
        if (f == 0) f = 4.f;
        setDelayBufferSizeInSeconds(f);

        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagBlendronic_beats))
            {
                Array<float> beats;
                Array<bool> beatsstates;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        beats.add(f);
                        beatsstates.add(true);
                    }
                }
                setBeats(beats);
                setBeatsStates(beatsstates);
            }
            else if (sub->hasTagName(vtagBlendronic_beatsStates))
            {
                Array<bool> beatsstates;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagBool + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = (bool)attr.getIntValue();
                        beatsstates.add(f);
                    }
                }
                setBeatsStates(beatsstates);
            }
            else if (sub->hasTagName(vtagBlendronic_delayLengths))
            {
                Array<float> lengths;
                Array<bool> lengthsstates;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        lengths.add(f);
                        lengthsstates.add(true);
                    }
                }
                setDelayLengths(lengths);
                setDelayLengthsStates(lengthsstates);
            }
            else if (sub->hasTagName(vtagBlendronic_delayLengthsStates))
            {
                Array<bool> nstates;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagBool + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = (bool)attr.getIntValue();
                        nstates.add(f);
                    }
                }
                setDelayLengthsStates(nstates);
            }
            else if (sub->hasTagName(vtagBlendronic_smoothLengths))
            {
                Array<float> durs;
                Array<bool> dursstates;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        durs.add(f);
                        dursstates.add(true);
                    }
                }
                setSmoothLengths(durs);
                setSmoothLengthsStates(dursstates);
            }
            else if (sub->hasTagName(vtagBlendronic_smoothLengthsStates))
            {
                Array<bool> nstates;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagBool + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = (bool)attr.getIntValue();
                        nstates.add(f);
                    }
                }
                setSmoothLengthsStates(nstates);
            }
//            else if (sub->hasTagName(vtagBlendronic_smoothValues))
//            {
//                Array<float> durs;
//                for (int k = 0; k < 128; k++)
//                {
//                    String attr = sub->getStringAttribute(ptagFloat + String(k));
//
//                    if (attr == String()) break;
//                    else
//                    {
//                        f = attr.getFloatValue();
//                        durs.add(f);
//                    }
//                }
//                setSmoothValues(durs);
//            }
            else if (sub->hasTagName(vtagBlendronic_feedbackCoefficients))
            {
                Array<float> coeffs;
                Array<bool> coeffsstates;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        coeffs.add(f);
                        coeffsstates.add(true);
                    }
                }
                setFeedbackCoefficients(coeffs);
                setFeedbackCoefficientsStates(coeffsstates);
            }
            else if (sub->hasTagName(vtagBlendronic_feedbackCoefficientsStates))
            {
                Array<bool> nstates;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagBool + String(k));

                    if (attr == String()) break;
                    else
                    {
                        f = (bool)attr.getIntValue();
                        nstates.add(f);
                    }
                }
                setFeedbackCoefficientsStates(nstates);
            }
        }
    }
    
    // output gain
    Moddable<float> outGain;

private:
    
	String name;
	bool isActive;

	// sequenced parameters
	Array<float> bBeats;
    Array<float> bDelayLengths;
    Array<float> bSmoothLengths;
	Array<float> bSmoothValues;
	Array<float> bFeedbackCoefficients;
    
    // slider states for sequenced params
    Array<bool> bBeatsStates;
    Array<bool> bDelayLengthsStates;
    Array<bool> bSmoothLengthsStates;
    Array<bool> bSmoothValuesStates;
    Array<bool> bFeedbackCoefficientsStates;

	// dsmooth stuff
    BlendronicSmoothBase bSmoothBase;
    BlendronicSmoothScale bSmoothScale;
    
    TargetNoteMode targetTypeBlendronicPatternSync;
    TargetNoteMode targetTypeBlendronicBeatSync;
    TargetNoteMode targetTypeBlendronicClear;
    TargetNoteMode targetTypeBlendronicPausePlay;
    TargetNoteMode targetTypeBlendronicOpenCloseInput;
    TargetNoteMode targetTypeBlendronicOpenCloseOutput;

	// signal chain stk classes
	float bFeedbackCoefficient;
    
    float delayBufferSizeInSeconds;

	//needed for sampling
//    float inputGain;
    
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
	void keyPressed(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates);

	//begin playing reverse note, called with noteOff
	void keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post = false);

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
    
	Array<float> velocities;    //record of velocities
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
