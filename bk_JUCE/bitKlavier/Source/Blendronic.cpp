/*
  ==============================================================================

	Blendronic.cpp
	Created: 11 Jun 2019 2:00:53pm
	Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "Blendronic.h"

#include "BKSynthesiser.h"

//copy constructor
BlendronicPreparation::BlendronicPreparation(BlendronicPreparation::Ptr p) :
	name(p->getName()),
	bBeats(p->getBeats()),
    bDelayLengths(p->getDelayLengths()),
    bSmoothLengths(p->getSmoothLengths()),
	bSmoothValues(p->getSmoothValues()),
	bFeedbackCoefficients(p->getFeedbackCoefficients()),
	bDelayMax(p->getDelayMax()),
    bSmoothBase(BlendronicSmoothPulse),
    bSmoothScale(BlendronicSmoothFull),
    targetTypeBlendronicPatternSync(p->getTargetTypeBlendronicPatternSync()),
    targetTypeBlendronicBeatSync(p->getTargetTypeBlendronicBeatSync()),
    targetTypeBlendronicClear(p->getTargetTypeBlendronicClear()),
    targetTypeBlendronicPausePlay(p->getTargetTypeBlendronicPausePlay()),
    targetTypeBlendronicOpenCloseInput(p->getTargetTypeBlendronicOpenCloseInput()),
    targetTypeBlendronicOpenCloseOutput(p->getTargetTypeBlendronicOpenCloseOutput())
{
}

//constructor with input
BlendronicPreparation::BlendronicPreparation(String newName,
                                             Array<float> beats,
                                             Array<float> delayLengths,
                                             Array<float> smoothLengths,
                                             Array<float> smoothValues,
                                             Array<float> feedbackCoefficients,
                                             float clusterThresh,
                                             float delayMax) :
	name(newName),
	bBeats(beats),
    bDelayLengths(delayLengths),
    bSmoothLengths(smoothLengths),
	bSmoothValues(smoothValues),
	bFeedbackCoefficients(feedbackCoefficients),
	bDelayMax(delayMax),
    bSmoothBase(BlendronicSmoothPulse),
    bSmoothScale(BlendronicSmoothFull),
    targetTypeBlendronicPatternSync(NoteOn),
    targetTypeBlendronicBeatSync(NoteOn),
    targetTypeBlendronicClear(NoteOn),
    targetTypeBlendronicPausePlay(NoteOn),
    targetTypeBlendronicOpenCloseInput(NoteOn),
    targetTypeBlendronicOpenCloseOutput(NoteOn)
{
}

//empty constructor
BlendronicPreparation::BlendronicPreparation(void) :
	name("blank blendronic"),
	bBeats(Array<float>({ 4., 3., 2., 3.})),
    bDelayLengths(Array<float>({ 4., 3., 2., 3.})),
    bSmoothLengths(Array<float>({ 50.0f })),
	bSmoothValues(Array<float>({ 0.1f })),
	bFeedbackCoefficients(Array<float>({ 0.95 })),
	bDelayMax(5.f),
    bSmoothBase(BlendronicSmoothPulse),
    bSmoothScale(BlendronicSmoothFull),
    targetTypeBlendronicPatternSync(NoteOn),
    targetTypeBlendronicBeatSync(NoteOn),
    targetTypeBlendronicClear(NoteOn),
    targetTypeBlendronicPausePlay(NoteOn),
    targetTypeBlendronicOpenCloseInput(NoteOn),
    targetTypeBlendronicOpenCloseOutput(NoteOn)
{
}

//copy
void BlendronicPreparation::copy(BlendronicPreparation::Ptr b)
{
    bBeats = b->getBeats();
    bDelayLengths = b->getDelayLengths();
    bSmoothLengths = b->getSmoothLengths();
    bSmoothValues = b->getSmoothValues();
    bFeedbackCoefficients = b->getFeedbackCoefficients();
    bDelayMax = b->getDelayMax();
    bSmoothBase = b->getSmoothBase();
    bSmoothScale = b->getSmoothScale();
    targetTypeBlendronicPatternSync = b->getTargetTypeBlendronicPatternSync();
    targetTypeBlendronicBeatSync = b->getTargetTypeBlendronicBeatSync();
    targetTypeBlendronicClear = b->getTargetTypeBlendronicClear();
    targetTypeBlendronicPausePlay = b->getTargetTypeBlendronicPausePlay();
    targetTypeBlendronicOpenCloseInput = b->getTargetTypeBlendronicOpenCloseInput();
    targetTypeBlendronicOpenCloseOutput = b->getTargetTypeBlendronicOpenCloseOutput();
}

//compares two blendronics
bool BlendronicPreparation::compare(BlendronicPreparation::Ptr b)
{
	//will do later
	return false;
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
    velocities.ensureStorageAllocated(128);
    holdTimers.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        velocities.insert(i, 0.);
        holdTimers.insert(i, 0);
    }
    
    inSyncCluster = false;
    inClearCluster = false;
    inOpenCluster = false;
    inCloseCluster = false;
    
    keysDepressed = Array<int>();
    
    DBG("Create bproc");
}

BlendronicProcessor::~BlendronicProcessor()
{
    DBG("Destroy bproc");
}

void BlendronicProcessor::tick(float* outputs)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    if (!blendronicActive) return;
    if (tempoPrep->getSubdivisions() * tempoPrep->getTempo() == 0) return;

    // Update the pulse length in case tempo or subdiv changed
    pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
    if (pulseLength != prevPulseLength) numSamplesBeat = prep->getBeats()[beatIndex] * pulseLength * sampleRate;
    
    // Check for beat change
    if (sampleTimer >= numSamplesBeat)
    {
        // Clear if we need to
        if (clearDelayOnNextBeat)
        {
            delay->clear();
            clearDelayOnNextBeat = false;
        }

        float beatPatternLength = 0.0;
        for (auto b : prep->getBeats()) beatPatternLength += b * pulseLength * sampleRate;

        if (numBeatPositions != (int)((delay->getDelayBuffer().getNumSamples() / beatPatternLength) * prep->getBeats().size()) - 1)
        {
            beatPositionsInBuffer.clear();
            beatPositionsIndex = -1;
            pulseOffset = delay->getCurrentSample();
            numBeatPositions = ((delay->getDelayBuffer().getNumSamples() / beatPatternLength) * prep->getBeats().size()) - 1;
        }

        // Set the next beat position, cycle if we've reached the max number of positions for the buffer
        beatPositionsInBuffer.set(++beatPositionsIndex, delay->getCurrentSample());
        if (beatPositionsIndex >= numBeatPositions) beatPositionsIndex = -1;
   
        // Step sequenced params
        beatIndex++;
        if (beatIndex >= prep->getBeats().size()) beatIndex = 0;
        delayIndex++;
        if (delayIndex >= prep->getDelayLengths().size()) delayIndex = 0;
        smoothIndex++;
        if (smoothIndex >= prep->getSmoothLengths().size()) smoothIndex = 0;
        feedbackIndex++;
        if (feedbackIndex >= prep->getFeedbackCoefficients().size()) feedbackIndex = 0;
             
        // Update numSamplesBeat for the new beat and reset sampleTimer
        numSamplesBeat = prep->getBeats()[beatIndex] * pulseLength * sampleRate;
        sampleTimer = 0;
        
        updateDelayParameters();
    }
    sampleTimer++;

    if (pulseLength != prevPulseLength)
    {
        // Set parameters of the delay object
        updateDelayParameters();
    }
    prevPulseLength = pulseLength;
    
    // Tick the delay
    delay->tick(outputs);
    
    float dlr = 0.0f;
    if (pulseLength != INFINITY) dlr = delay->getDelayLength() / (pulseLength * sampleRate);
    delayLengthRecord.set(delayLengthRecordInPoint++, dlr);
    if (delayLengthRecordInPoint >= delayLengthRecord.size()) delayLengthRecordInPoint = 0;
}

void BlendronicProcessor::processBlock(int numSamples, int midiChannel)
{
    
}

void BlendronicProcessor::keyPressed(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    velocities.set(noteNumber, velocity);
    holdTimers.set(noteNumber, 0);
    
    // Get target flags
    bool doPatternSync = targetStates[TargetTypeBlendronicPatternSync] == TargetStateEnabled;
    bool doBeatSync = targetStates[TargetTypeBlendronicBeatSync] == TargetStateEnabled;
    bool doClear = targetStates[TargetTypeBlendronicClear] == TargetStateEnabled;
    bool doPausePlay = targetStates[TargetTypeBlendronicPausePlay] == TargetStateEnabled;
    bool doOpenCloseInput = targetStates[TargetTypeBlendronicOpenCloseInput] == TargetStateEnabled;
    bool doOpenCloseOutput = targetStates[TargetTypeBlendronicOpenCloseOutput] == TargetStateEnabled;
    
    // DBG("doPatternSync = " + String((int)doPatternSync) + " doBeatSync = " + String((int)doBeatSync));
    
    if (doPatternSync &&
        (prep->getTargetTypeBlendronicPatternSync() == NoteOn || prep->getTargetTypeBlendronicPatternSync() == Both))
    {
        // Reset the phase of all params and update values
        // setSampleTimer(0);
        setBeatIndex(prep->getBeats().size());
        setDelayIndex(prep->getDelayLengths().size());
        setSmoothIndex(prep->getSmoothLengths().size());
        setFeedbackIndex(prep->getFeedbackCoefficients().size());
        // beatPositionsInBuffer.clear();
        // beatPositionsInBuffer.add(delay->getCurrentSample());
        // pulseOffset = delay->getCurrentSample();
        // beatPositionsIndex = 0;
        // updateDelayParameters();
    }
    else if (doBeatSync &&
             (prep->getTargetTypeBlendronicBeatSync() == NoteOn || prep->getTargetTypeBlendronicBeatSync() == Both))
    {
        // Sync to the next beat
        setSampleTimer(numSamplesBeat);
        beatPositionsInBuffer.clear();
        beatPositionsInBuffer.add(delay->getCurrentSample());
        pulseOffset = delay->getCurrentSample();
        beatPositionsIndex = 0;
    }
    
    if (doClear &&
        (prep->getTargetTypeBlendronicClear() == NoteOn || prep->getTargetTypeBlendronicClear() == Both))
    {
        delay->clear();
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

void BlendronicProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    // Get target flags
    bool doPatternSync = targetStates[TargetTypeBlendronicPatternSync] == TargetStateEnabled;
    bool doBeatSync = targetStates[TargetTypeBlendronicBeatSync] == TargetStateEnabled;
    bool doClear = targetStates[TargetTypeBlendronicClear] == TargetStateEnabled;
    bool doPausePlay = targetStates[TargetTypeBlendronicPausePlay] == TargetStateEnabled;
    bool doOpenCloseInput = targetStates[TargetTypeBlendronicOpenCloseInput] == TargetStateEnabled;
    bool doOpenCloseOutput = targetStates[TargetTypeBlendronicOpenCloseOutput] == TargetStateEnabled;

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
    else if (doBeatSync && (prep->getTargetTypeBlendronicBeatSync() == NoteOff || prep->getTargetTypeBlendronicBeatSync() == Both))
    {
        // Sync to the next beat
        setSampleTimer(numSamplesBeat);
        beatPositionsInBuffer.clear();
        beatPositionsInBuffer.add(delay->getCurrentSample());
        pulseOffset = delay->getCurrentSample();
        beatPositionsIndex = 0;
    }
    
    if (doClear &&
        (prep->getTargetTypeBlendronicClear() == NoteOff || prep->getTargetTypeBlendronicClear() == Both))
    {
        delay->clear();
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
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;

    sampleRate = sr;
    
    delay = synth->createBlendronicDelay(prep->getDelayLengths()[0], prep->getDelayMax(), sampleRate, true);

    delayLengthRecord.ensureStorageAllocated(prep->getDelayMax() * sampleRate);
    for (int i = 0; i < prep->getDelayMax() * sampleRate; i++)
    {
        delayLengthRecord.add(0.0f);
    }
    delayLengthRecordInPoint = 0;
    
    beatPositionsInBuffer.ensureStorageAllocated(128);
    beatPositionsInBuffer.clear();
    beatPositionsInBuffer.add(0);
    pulseOffset = 0;
    beatPositionsIndex = 0;
    
    prevBeat = prep->getBeats()[0];
    prevDelay = prep->getDelayLengths()[0];
    
	sampleRate = sr;
    delay->setSampleRate(sr);
    
    beatIndex = 0;
    delayIndex = 0;
    smoothIndex = 0;
    feedbackIndex = 0;
    
    pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
    numSamplesBeat = prep->getBeats()[beatIndex] * sampleRate * pulseLength;
    numSamplesDelay = prep->getDelayLengths()[delayIndex] * sampleRate * pulseLength;
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
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    
    numSamplesDelay = prep->getDelayLengths()[delayIndex] * pulseLength * sampleRate;
    if (pulseLength == INFINITY) numSamplesDelay = 0;
    
    float delayDelta = fabsf(prevDelay - prep->getDelayLengths()[delayIndex]);
    prevDelay = prep->getDelayLengths()[delayIndex];
    
    // To complicated for now, may add back in later
//    float smoothRate = 0.0f;
//    float smoothBase = 0.0f;
//    if (prep->getSmoothBase()       == BlendronicSmoothPulse)
//    {
//        smoothBase = pulseLength;
//    }
//    else if (prep->getSmoothBase()  == BlendronicSmoothBeat)
//    {
//        smoothBase = pulseLength / prep->getBeats()[beatIndex];
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
    
    float smoothRate = (pulseLength * delayDelta) / (prep->getSmoothLengths()[smoothIndex] * 0.001f);
    if (delayDelta == 0 || pulseLength == INFINITY) smoothRate = INFINITY;
    
    // DBG(String(getId()) + " new envelope target = " + String(numSamplesDelay));
    // DBG(String(1000. / smoothRate) + "ms"); // smooth rate is change / second
    delay->setDelayTargetLength(numSamplesDelay);
    delay->setSmoothRate(smoothRate); // this is really a rate, not a duration
    delay->setFeedback(prep->getFeedbackCoefficients()[feedbackIndex]);
}
