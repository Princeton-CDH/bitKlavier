/*
  ==============================================================================

	Blendronic.cpp
	Created: 11 Jun 2019 2:00:53pm
	Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "Blendronic.h"
#include "BKSynthesiser.h"
#include "Modification.h"

void BlendronicPreparation::performModification(BlendronicModification* b, Array<bool> dirty)
{
    bool reverse = b->altMod && modded;
    
    if (dirty[BlendronicOutGain]) outGain.modify(b->outGain, reverse);
    if (dirty[BlendronicBeats]) {
        bBeats.modify(b->bBeats, reverse);
        bBeatsStates.modify(b->bBeatsStates, reverse);
    }
    if (dirty[BlendronicDelayLengths]) {
        bDelayLengths.modify(b->bDelayLengths, reverse);
        bDelayLengthsStates.modify(b->bDelayLengthsStates, reverse);
    }
    if (dirty[BlendronicSmoothLengths]) {
        bSmoothLengths.modify(b->bSmoothLengths, reverse);
        bSmoothLengthsStates.modify(b->bSmoothLengthsStates, reverse);
    }
    if (dirty[BlendronicFeedbackCoeffs]) {
        bFeedbackCoefficients.modify(b->bFeedbackCoefficients, reverse);
        bFeedbackCoefficientsStates.modify(b->bFeedbackCoefficientsStates, reverse);
    }
    if (dirty[BlendronicDelayBufferSize]) delayBufferSizeInSeconds.modify(b->delayBufferSizeInSeconds, reverse);
    
    modded = !reverse;
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////// BLENDRONIC PROCESSOR /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

BlendronicProcessor::BlendronicProcessor(Blendronic::Ptr bBlendronic,
	TempoProcessor::Ptr bTempo, GeneralSettings::Ptr bGeneral, BKSynthesiser* bMain):
	blendronic(bBlendronic),
    synth(bMain),
	tempo(bTempo),
	general(bGeneral),
    keymaps(Keymap::PtrArr()),
    blendronicActive(true),
	sampleTimer(0),
	beatIndex(0),
    delayIndex(0),
	smoothIndex(0),
	feedbackIndex(0)
{
    for (int j = 0; j < 128; j++)
    {
        velocities.add(new Array<float>());
        invertVelocities.add(new Array<float>());
        for (int i = 0; i < TargetTypeTempo-TargetTypeBlendronicPatternSync; ++i)
        {
            velocities.getLast()->add(0.f);
            invertVelocities.getLast()->add(0.f);
        }
    }
    
    holdTimers.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        holdTimers.insert(i, 0);
    }
    
    inSyncCluster = false;
    inClearCluster = false;
    inOpenCluster = false;
    inCloseCluster = false;
    
    keysDepressed = Array<int>();
    
    resetPhase = false;
    
    for (int i = 0; i < 1/*numChannels*/; ++i)
        audio.add (new BlendronicDisplay::ChannelInfo (44100, 512));
    
    smoothing = std::make_unique<BlendronicDisplay::ChannelInfo>(44100, 512);
    
    DBG("Create bproc");
}

BlendronicProcessor::~BlendronicProcessor()
{
    DBG("Destroy bproc");
}

// maybe not so important, but i see a lot of multiplies and divides in here and this
// is called every sample, so it may be worth some effort to see if it can be streamlined
void BlendronicProcessor::tick(float* outputs)
{
    BlendronicPreparation::Ptr prep = blendronic->prep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->prep;
    
    if (!blendronicActive) return;
    if (tempoPrep->getSubdivisions() * tempoPrep->getTempo() == 0) return;

    // Update the pulse length in case tempo or subdiv changed
    // possible to put this behind conditional, so we aren't doing these operations ever tick?
    pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
    if (pulseLength != prevPulseLength) numSamplesBeat = prep->bBeats.value[beatIndex] * pulseLength * synth->getSampleRate();
    
    // Check for beat change
    if (sampleTimer >= numSamplesBeat)
    {
        // Clear if we need to
        if (clearDelayOnNextBeat)
        {
            delay->clear();
            for (auto channel : audio)
                channel->clear();
            clearDelayOnNextBeat = false;
        }

        float beatPatternLength = 0.0;
        for (auto b : prep->bBeats.value) beatPatternLength += b * pulseLength * synth->getSampleRate();

        if (numBeatPositions != (int)((delay->getDelayBuffer()->getNumSamples() / beatPatternLength) * prep->bBeats.value.size()) - 1)
        {
            beatPositionsInBuffer.clear();
            beatPositionsIndex = -1;
            pulseOffset = delay->getInPoint();
            numBeatPositions = ((delay->getDelayBuffer()->getNumSamples() / beatPatternLength) * prep->bBeats.value.size()) - 1;
        }

        // Set the next beat position, cycle if we've reached the max number of positions for the buffer
        beatPositionsInBuffer.set(++beatPositionsIndex, delay->getInPoint());
        if (beatPositionsIndex >= numBeatPositions) beatPositionsIndex = -1;
   
        // Step sequenced params
        beatIndex++;
        if (beatIndex >= prep->bBeats.value.size()) beatIndex = 0;
        delayIndex++;
        if (delayIndex >= prep->bDelayLengths.value.size()) delayIndex = 0;
        smoothIndex++;
        if (smoothIndex >= prep->bSmoothLengths.value.size()) smoothIndex = 0;
        feedbackIndex++;
        if (feedbackIndex >= prep->bFeedbackCoefficients.value.size()) feedbackIndex = 0;
             
        // Update numSamplesBeat for the new beat and reset sampleTimer
        numSamplesBeat = prep->bBeats.value[beatIndex] * pulseLength * synth->getSampleRate();
        sampleTimer = 0;
        
        updateDelayParameters();
    }
    sampleTimer++;

    if (pulseLength != prevPulseLength)
    {
        // Set parameters of the delay object
        updateDelayParameters();
        resetPhase = true;
    }
    prevPulseLength = pulseLength;
    
    // Tick the delay
    delay->tick(outputs, Decibels::decibelsToGain(prep->outGain.value));
    
    float dlr = 0.0f;
    if (pulseLength != INFINITY) dlr = delay->getDelayLength() / (pulseLength * synth->getSampleRate());

    
    int i = getInPoint() - 1;
    if (i < 0) i = getDelayBuffer()->getNumSamples() - 1;
    
    for (auto channel : audio)
        channel->pushSample (delay->getSample(0, i));
    
    smoothing->pushSample(dlr);
}

void BlendronicProcessor::processBlock(int numSamples, int midiChannel)
{
    
}

void BlendronicProcessor::keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    BlendronicPreparation::Ptr prep = blendronic->prep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->prep;
    
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an actual key press (not an inverted release) aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = velocities.getUnchecked(noteNumber);
        for (int i = 0; i < velocities.getUnchecked(noteNumber)->size(); ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i+TargetTypeBlendronicPatternSync));
        }
    }
    // If this an inverted release, aVels will be the incoming velocities,
    // but bVels will use the values from the last inverted press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = invertVelocities.getUnchecked(noteNumber);
    }
    
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    holdTimers.set(noteNumber, 0);
    
    // Get target flags
    int s = TargetTypeBlendronicPatternSync;
    bool doPatternSync = bVels->getUnchecked(TargetTypeBlendronicPatternSync-s) >= 0.f;
    bool doBeatSync = bVels->getUnchecked(TargetTypeBlendronicBeatSync-s) >= 0.f;
    bool doClear = bVels->getUnchecked(TargetTypeBlendronicClear-s) >= 0.f;
    bool doPausePlay = bVels->getUnchecked(TargetTypeBlendronicPausePlay-s) >= 0.f;
    bool doOpenCloseInput = bVels->getUnchecked(TargetTypeBlendronicOpenCloseInput-s) >= 0.f;
    bool doOpenCloseOutput = bVels->getUnchecked(TargetTypeBlendronicOpenCloseOutput-s) >= 0.f;
    
    // DBG("doPatternSync = " + String((int)doPatternSync) + " doBeatSync = " + String((int)doBeatSync));
    
    if (doPatternSync &&
        (prep->getTargetTypeBlendronicPatternSync() == NoteOn || prep->getTargetTypeBlendronicPatternSync() == Both))
    {
        // Reset the phase of all params and update values
        // setSampleTimer(0);
        setBeatIndex(prep->bBeats.value.size());
        setDelayIndex(prep->bDelayLengths.value.size());
        setSmoothIndex(prep->bSmoothLengths.value.size());
        setFeedbackIndex(prep->bFeedbackCoefficients.value.size());
        // beatPositionsInBuffer.clear();
        // beatPositionsInBuffer.add(delay->getCurrentSample());
        // pulseOffset = delay->getCurrentSample();
        // beatPositionsIndex = 0;
        // updateDelayParameters();
    }
    //else if (doBeatSync &&
    if (doBeatSync &&
             (prep->getTargetTypeBlendronicBeatSync() == NoteOn || prep->getTargetTypeBlendronicBeatSync() == Both))
    {
        // Sync to the next beat
        setSampleTimer(numSamplesBeat);
        beatPositionsInBuffer.clear();
        beatPositionsInBuffer.add(delay->getInPoint());
        pulseOffset = delay->getInPoint();
        beatPositionsIndex = 0;
        resetPhase = true;
    }
    
    if (doClear &&
        (prep->getTargetTypeBlendronicClear() == NoteOn || prep->getTargetTypeBlendronicClear() == Both))
    {
        delay->clear();
        for (auto channel : audio)
            channel->clear();
    }
    
    if (doPausePlay &&
        (prep->getTargetTypeBlendronicPausePlay() == NoteOn || prep->getTargetTypeBlendronicPausePlay() == Both))
    {
        toggleActive();
    }
    
    if (doOpenCloseInput &&
        (prep->getTargetTypeBlendronicOpenCloseInput() == NoteOn || prep->getTargetTypeBlendronicOpenCloseInput() == Both))
    {
        toggleInput();
    }
    
    if (doOpenCloseOutput &&
        (prep->getTargetTypeBlendronicOpenCloseOutput() == NoteOn || prep->getTargetTypeBlendronicOpenCloseOutput() == Both))
    {
        toggleOutput();
    }
}

void BlendronicProcessor::keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    BlendronicPreparation::Ptr prep = blendronic->prep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->prep;

    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = invertVelocities.getUnchecked(noteNumber);
        for (int i = 0; i < invertVelocities.getUnchecked(noteNumber)->size(); ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i+TargetTypeBlendronicPatternSync));
        }
    }
    // If this an actual release, aVels will be the incoming velocities,
    // but bVels will use the values from the last press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = velocities.getUnchecked(noteNumber);
    }
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    // Get target flags
    int s = TargetTypeBlendronicPatternSync;
    bool doPatternSync = bVels->getUnchecked(TargetTypeBlendronicPatternSync-s) >= 0.f;
    bool doBeatSync = bVels->getUnchecked(TargetTypeBlendronicBeatSync-s) >= 0.f;
    bool doClear = bVels->getUnchecked(TargetTypeBlendronicClear-s) >= 0.f;
    bool doPausePlay = bVels->getUnchecked(TargetTypeBlendronicPausePlay-s) >= 0.f;
    bool doOpenCloseInput = bVels->getUnchecked(TargetTypeBlendronicOpenCloseInput-s) >= 0.f;
    bool doOpenCloseOutput = bVels->getUnchecked(TargetTypeBlendronicOpenCloseOutput-s) >= 0.f;

    if (doPatternSync &&
        (prep->getTargetTypeBlendronicPatternSync() == NoteOff || prep->getTargetTypeBlendronicPatternSync() == Both))
    {
        // Reset the phase of all params and update values
        // setSampleTimer(0);
        setBeatIndex(0);
        setDelayIndex(0);
        setSmoothIndex(0);
        setFeedbackIndex(0);
        // beatPositionsInBuffer.clear();
        // beatPositionsInBuffer.add(delay->getCurrentSample());
        // pulseOffset = delay->getCurrentSample();
        // beatPositionsIndex = 0;
        // updateDelayParameters();
    }
    if (doBeatSync &&
        (prep->getTargetTypeBlendronicBeatSync() == NoteOff || prep->getTargetTypeBlendronicBeatSync() == Both))
    {
        // Sync to the next beat
        setSampleTimer(numSamplesBeat);
        beatPositionsInBuffer.clear();
        beatPositionsInBuffer.add(delay->getInPoint());
        pulseOffset = delay->getInPoint();
        beatPositionsIndex = 0;
    }
    
    if (doClear &&
        (prep->getTargetTypeBlendronicClear() == NoteOff || prep->getTargetTypeBlendronicClear() == Both))
    {
        delay->clear();
        for (auto channel : audio)
            channel->clear();
    }
    
    if (doPausePlay &&
        (prep->getTargetTypeBlendronicPausePlay() == NoteOff || prep->getTargetTypeBlendronicPausePlay() == Both))
    {
        toggleActive();
    }
    
    if (doOpenCloseInput &&
        (prep->getTargetTypeBlendronicOpenCloseInput() == NoteOff || prep->getTargetTypeBlendronicOpenCloseInput() == Both))
    {
        toggleInput();
    }
    
    if (doOpenCloseOutput &&
        (prep->getTargetTypeBlendronicOpenCloseOutput() == NoteOff || prep->getTargetTypeBlendronicOpenCloseOutput() == Both))
    {
        toggleOutput();
    }
}

void BlendronicProcessor::postRelease(int noteNumber, int midiChannel)
{
	//may not need anything except communicating with other preparations?
}

void BlendronicProcessor::prepareToPlay(double sr)
{
    BlendronicPreparation::Ptr prep = blendronic->prep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->prep;
    
    if (delay == nullptr)
    {
        delay = synth->createBlendronicDelay(prep->bDelayLengths.value[0], prep->delayBufferSizeInSeconds.value * synth->getSampleRate(), synth->getSampleRate(), true);
    }
    
    for (int i = 0; i < 1/*numChannels*/; ++i)
    {
        audio.getUnchecked(i)->setBufferSize(delay->getDelayBuffer()->getNumSamples());
    }
    
    smoothing->setBufferSize(delay->getDelayBuffer()->getNumSamples());
    
    beatPositionsInBuffer.ensureStorageAllocated(128);
    beatPositionsInBuffer.clear();
    beatPositionsInBuffer.add(0);
    pulseOffset = 0;
    beatPositionsIndex = 0;
    
    prevBeat = prep->bBeats.value[0];
    prevDelay = prep->bDelayLengths.value[0];
    
    delay->setSampleRate(sr);
    
    beatIndex = 0;
    delayIndex = 0;
    smoothIndex = 0;
    feedbackIndex = 0;
    
    pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
    numSamplesBeat = prep->bBeats.value[beatIndex] * synth->getSampleRate() * pulseLength;
    numSamplesDelay = prep->bDelayLengths.value[delayIndex] * synth->getSampleRate() * pulseLength;
    if (pulseLength == INFINITY)
    {
        numSamplesBeat = INFINITY;
        numSamplesBeat = 0;
    }
    
    delay->setDelayLength(numSamplesDelay);
    delay->setDelayTargetLength(numSamplesDelay);
    
    updateDelayParameters();
}

void BlendronicProcessor::playNote(int channel, int note, float velocity)
{
	//may not need anything except communicating with other preparations?
}

void BlendronicProcessor::updateDelayParameters()
{
    BlendronicPreparation::Ptr prep = blendronic->prep;
    
    numSamplesDelay = prep->bDelayLengths.value[delayIndex] * pulseLength * synth->getSampleRate();
    if (pulseLength == INFINITY) numSamplesDelay = 0;
    
    float delayDelta = fabsf(prevDelay - prep->bDelayLengths.value[delayIndex]);
    prevDelay = prep->bDelayLengths.value[delayIndex];
    
    // To complicated for now, may add back in later
//    float smoothRate = 0.0f;
//    float smoothBase = 0.0f;
//    if (prep->getSmoothBase()       == BlendronicSmoothPulse)
//    {
//        smoothBase = pulseLength;
//    }
//    else if (prep->getSmoothBase()  == BlendronicSmoothBeat)
//    {
//        smoothBase = pulseLength / prep->bBeats.value[beatIndex];
//    }
//
//    if (prep->getSmoothScale()  == BlendronicSmoothConstant)
//    {
//        smoothRate = smoothBase / prep->getSmoothValues()[smoothIndex];
//    }
//    else if (prep->getSmoothScale() == BlendronicSmoothFull)
//    {
//        smoothRate = delayDelta * (smoothBase / prep->getSmoothValues()[smoothIndex]);
//        if (delayDelta == 0) smoothRate = INFINITY;
//    }
    
    float smoothRate = (pulseLength * delayDelta) / (prep->bSmoothLengths.value[smoothIndex] * 0.001f);
    if (delayDelta == 0 || pulseLength == INFINITY) smoothRate = INFINITY;
    
    // DBG(String(getId()) + " new envelope target = " + String(numSamplesDelay));
    // DBG(String(1000. / smoothRate) + "ms"); // smooth rate is change / second
    delay->setDelayTargetLength(numSamplesDelay);
    delay->setSmoothRate(smoothRate); // this is really a rate, not a duration
    delay->setFeedback(prep->bFeedbackCoefficients.value[feedbackIndex]);
}

float BlendronicProcessor::getPulseLengthInSamples()
{
    return pulseLength * synth->getSampleRate();
}

void BlendronicProcessor::setDelayBufferSizeInSeconds(float size)
{
    const ScopedLock sl (lock);
    blendronic->prep->delayBufferSizeInSeconds.set(size);
    delay->setBufferSize(size * synth->getSampleRate());
    beatPositionsInBuffer.clear();
    beatPositionsIndex = -1;
    
    for (int i = 0; i < 1/*numChannels*/; ++i)
    {
        audio.getUnchecked(i)->setBufferSize(delay->getDelayBuffer()->getNumSamples());
    }
    
    smoothing->setBufferSize(delay->getDelayBuffer()->getNumSamples());
}
