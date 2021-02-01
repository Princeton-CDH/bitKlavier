/*
  ==============================================================================

    Synchronic.cpp
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Synchronic.h"
#include "PluginProcessor.h"
#include "Modification.h"

void SynchronicPreparation::performModification(SynchronicModification* s, Array<bool> dirty)
{
    bool reverse = s->altMod && modded;
    
    if (dirty[SynchronicGain]) sGain.modify(s->sGain, reverse);
    if (dirty[SynchronicBlendronicGain]) sBlendronicGain.modify(s->sBlendronicGain, reverse);
    if (dirty[SynchronicNumPulses]) sNumBeats.modify(s->sNumBeats, reverse);
    if (dirty[SynchronicClusterMin]) sClusterMin.modify(s->sClusterMin, reverse);
    if (dirty[SynchronicClusterMax]) sClusterMax.modify(s->sClusterMax, reverse);
    if (dirty[SynchronicClusterCap]) sClusterCap.modify(s->sClusterCap, reverse);
    if (dirty[SynchronicMode]) sMode.modify(s->sMode, reverse);
    
    if (dirty[SynchronicBeatsToSkip]) sBeatsToSkip.modify(s->sBeatsToSkip, reverse);
    
    if (dirty[SynchronicBeatMultipliers]) {
        
        sBeatMultipliers.modify(s->sBeatMultipliers, reverse);
        sBeatMultipliersStates.modify(s->sBeatMultipliersStates, reverse);
    }
    if (dirty[SynchronicAccentMultipliers]) {
        sAccentMultipliers.modify(s->sAccentMultipliers, reverse);
        sAccentMultipliersStates.modify(s->sAccentMultipliersStates, reverse);
    }
    if (dirty[SynchronicLengthMultipliers]) {
        sLengthMultipliers.modify(s->sLengthMultipliers, reverse);
        sLengthMultipliersStates.modify(s->sLengthMultipliersStates, reverse);
    }
    if (dirty[SynchronicTranspOffsets]) {
        sTransposition.modify(s->sTransposition, reverse);
        sTranspositionStates.modify(s->sTranspositionStates, reverse);
    }
    
    if (dirty[SynchronicTranspUsesTuning]) sTranspUsesTuning.modify(s->sTranspUsesTuning, reverse);
    
    if (dirty[SynchronicClusterThresh])
    {
        sClusterThresh.modify(s->sClusterThresh, reverse);
        sClusterThreshSec.modify(s->sClusterThreshSec, reverse);
    }
    
    if (dirty[SynchronicADSRs]) sADSRs.modify(s->sADSRs, reverse);
    
    if (dirty[SynchronicNumClusters]) numClusters.modify(s->numClusters, reverse);
    if (dirty[SynchronicOnOff]) onOffMode.modify(s->onOffMode, reverse);
    
    if (dirty[SynchronicHoldMin]) holdMin.modify(s->holdMin, reverse);
    if (dirty[SynchronicHoldMax]) holdMax.modify(s->holdMax, reverse);
    
    if (dirty[SynchronicVelocityMin]) velocityMin.modify(s->velocityMin, reverse);
    if (dirty[SynchronicVelocityMax]) velocityMax.modify(s->velocityMax, reverse);
    
    if (dirty[SynchronicMidiOutput]) midiOutput.modify(s->midiOutput, reverse);
    
    if (dirty[SynchronicUseGlobalSoundSet]) sUseGlobalSoundSet.modify(s->sUseGlobalSoundSet, reverse);
    
    if (dirty[SynchronicSoundSet])
    {
        sSoundSet.modify(s->sSoundSet, reverse);
        sSoundSetName.modify(s->sSoundSetName, reverse);
    }
    
    modded = !reverse;
}

SynchronicProcessor::SynchronicProcessor(Synchronic::Ptr synchronic,
                                         TuningProcessor::Ptr tuning,
                                         TempoProcessor::Ptr tempo,
										 BlendronicProcessor::PtrArr blend,
                                         BKSynthesiser* main,
                                         GeneralSettings::Ptr general):
synth(main),
general(general),
synchronic(synchronic),
tuner(tuning),
tempo(tempo),
keymaps(Keymap::PtrArr()),
notePlayed(false)
{
    if (!synchronic->prep->sUseGlobalSoundSet.value)
    {
        // comes in as "soundfont.sf2.subsound1"
        String name = synchronic->prep->sSoundSetName.value;
        BKSampleLoadType type = BKLoadSoundfont;
        
        for (int i = 0; i < cBKSampleLoadTypes.size(); i++)
        {
            if (name == String(cBKSampleLoadTypes[i]))
            {
                type = (BKSampleLoadType) i;
            }
        }
        
        String path = name.upToLastOccurrenceOf(".subsound", false, false);
        int subsound = 0;
        
        if (type == BKLoadSoundfont)
        {
            for (auto sf : synth->processor.soundfontNames)
            {
                if (sf.contains(path))
                {
                    path = sf;
                    break;
                }
            }

            subsound = name.fromLastOccurrenceOf(".subsound", false, false).getIntValue();
        }
        int Id = synth->loadSamples(type, path, subsound, false);
        synchronic->prep->setSoundSet(Id);
    }
    
    velocities.ensureStorageAllocated(128);
    velocitiesActive.ensureStorageAllocated(128);
    holdTimers.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        velocities.insert(i, 0.);
        velocitiesActive.insert(i, 0);
        holdTimers.insert(i, 0);
    }

    inCluster = false;
    pausePlay = false;
    
    keysDepressed = Array<int>();
    clusterKeysDepressed = Array<int>();
    
    activeSynchronicVoices = Array<BKSynthesiserVoice*>();
    voiceMidiValues = Array<int>();
}

SynchronicProcessor::~SynchronicProcessor()
{
}

void SynchronicProcessor::playNote(int channel, int note, float velocity, SynchronicCluster::Ptr cluster)
{
    SynchronicPreparation::Ptr prep = synchronic->prep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->prep;
    
    PianoSamplerNoteDirection noteDirection = Forward;
    float noteStartPos = 0.0;
    float noteLength = 0.0;
    
 
    if (tempoPrep->getTempoSystem() == AdaptiveTempo1)
    {
        noteLength = (fabs(prep->sLengthMultipliers.value[cluster->getLengthMultiplierCounter()]) * tempoPrep->getBeatThreshMS());
    }
    else
    {
        noteLength = (fabs(prep->sLengthMultipliers.value[cluster->getLengthMultiplierCounter()]) * tempoPrep->getBeatThreshMS() / tempoPrep->getSubdivisions());
    }
        
    if (prep->sLengthMultipliers.value[cluster->getLengthMultiplierCounter()] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength + 3; //adjust for rampOn time == 3ms
    }
    
    for (auto t : prep->sTransposition.value[cluster->getTranspCounter()])
    {

        /*
        float offset = t + tuner->getOffset(note, false), synthOffset = offset;
        */
        
        int synthNoteNumber = note;
        float offset; // offset from integer, but may be greater than 1
        float synthOffset; // offset from actual sample played, always less than 1.
       
        // tune the transposition
        if (prep->sTranspUsesTuning.value) // use the Tuning setting
           offset = t + tuner->getOffset(round(t)+ note, false);
        else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
           offset = t + tuner->getOffset(note, false);
       
        synthOffset = offset;

        synthNoteNumber += (int)offset;
        synthOffset     -= (int)offset;

        int whichEnv = cluster->getEnvelopeCounter();
		BKSynthesiserVoice* currentVoice;
		if (!blendronic.isEmpty())
		{
            currentVoice =
                synth->keyOn(channel,
                             note,
                             synthNoteNumber,
                             synthOffset,
                             velocity,
                             aGlobalGain * prep->sAccentMultipliers.value[cluster->getAccentMultiplierCounter()],
                             noteDirection,
                             FixedLengthFixedStart,
                             SynchronicNote,
                             prep->getSoundSet(), //set
                             synchronic->getId(),
                             noteStartPos,  // start
                             noteLength,
                             prep->getAttack(whichEnv),
                             prep->getDecay(whichEnv),
                             prep->getSustain(whichEnv),
                             prep->getRelease(whichEnv),
                             tuner,
                             prep->getGainPtr(),
                             prep->getBlendronicGainPtr(),
                             blendronic);
		}
		else
		{
			currentVoice =
				synth->keyOn(channel,
                             note,
                             synthNoteNumber,
                             synthOffset,
                             velocity,
                             aGlobalGain * prep->sAccentMultipliers.value[cluster->getAccentMultiplierCounter()],
                             noteDirection,
                             FixedLengthFixedStart,
                             SynchronicNote,
                             prep->getSoundSet(), //set
                             synchronic->getId(),
                             noteStartPos,  // start
                             noteLength,
                             prep->getAttack(whichEnv),
                             prep->getDecay(whichEnv),
                             prep->getSustain(whichEnv),
                             prep->getRelease(whichEnv),
                             tuner,
                             prep->getGainPtr());
		}
        
        notePlayed = true;
        if (prep->midiOutput.value != nullptr && currentVoice != nullptr)
        {
            activeSynchronicVoices.add(currentVoice);
            int midiNote = currentVoice->getCurrentlyPlayingNote();
            voiceMidiValues.add(midiNote);
            const MidiMessage message = MidiMessage::noteOn(1, synthNoteNumber, velocity);
            prep->midiOutput.value->sendMessageNow(message);
            DBG("MIDI Message sent: Note on " + String(synthNoteNumber));
        }

    }
    
}

bool SynchronicProcessor::velocityCheck(int noteNumber)
{
    SynchronicPreparation::Ptr prep = synchronic->prep;
    
    int velocity = (int)(velocities.getUnchecked(noteNumber) * 127.0);
    
    if (velocity > 127) velocity = 127;
    if (velocity < 0)   velocity = 0;

    if(prep->velocityMin.value <= prep->velocityMax.value)
    {
        if (velocity >= prep->velocityMin.value && velocity <= prep->velocityMax.value)
        {
            return true;
        }
    }
    else
    {
        if (velocity >= prep->velocityMin.value || velocity <= prep->velocityMax.value)
        {
            return true;
        }
    }
    
    DBG("failed velocity check");
    return false;
}

bool SynchronicProcessor::holdCheck(int noteNumber)
{
    SynchronicPreparation::Ptr prep = synchronic->prep;
    
    uint64 hold = holdTimers.getUnchecked(noteNumber) * (1000.0 / synth->getSampleRate());
    
    if(prep->holdMin.value <= prep->holdMax.value)
    {
        if (hold >= prep->holdMin.value && hold <= prep->holdMax.value)
        {
            return true;
        }
    }
    else
    {
        if (hold >= prep->holdMin.value || hold <= prep->holdMax.value)
        {
            return true;
        }
    }
    
    DBG("failed hold check");
    return false;
}


void SynchronicProcessor::keyPressed(int noteNumber, float velocity, Array<KeymapTargetState> targetStates)
{
    
    SynchronicPreparation::Ptr prep = synchronic->prep;
    
    lastKeyPressed = noteNumber;
    lastKeyVelocity = velocity;
    
    // reset the timer for hold time checks
    holdTimers.set(noteNumber, 0);
    
    // check velocity filtering
    //  need to save old velocity, in case this new velocity failes the velocity test
    float velocitySave = velocitiesActive.getUnchecked(noteNumber);
    
    // save this velocity, for velocity checks, here and in keyRelease()
    velocities.set(noteNumber, velocity); // used for velocity checks
    
    // save this as the active velocity, for playback as well
    velocitiesActive.set(noteNumber, velocity); // used for actual note playback velocity
    
    // check the velocity
    if (!velocityCheck(noteNumber))
    {
        // need to set the active velocity back to what it was, since we're going to ignore this one
        velocitiesActive.set(noteNumber, velocitySave);
        return;
    }
    
    // add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    
    // track the note's target, as set in Keymap
    /*
    Keymap Target modes:
       01. Synchronic: current functionality; launches clusters/layers, syncs, etc.... (doCluster)
       02. Pattern Sync: calls cluster->resetPhase(), regardless of aPrep->getMode(), last cluster/layer (doPatternSync)
     eventually, we could allow targeting of individual patterns
       03. Beat Sync: calls cluster->setPhasor(0), regardless of aPrep->getMode(), last cluster/layer (doSync)
       04. Pause/Play: stop/start incrementing phasor, all clusters (could also have Pause/Play Last, for last cluster only)
       05. Add Notes: calls cluster->addNote(noteNumber), last cluster/layer
       06. Remove Oldest Note?: call cluster->removeOldestNote(); a way of thinning a cluster
            could combine with Add Notes to transform a cluster
       07. Remove Newest Note?: call cluster->removeNewestNote()
       08. Delete Oldest Layer:
       09. Delete Newest Layer:
       10. Rotate Layers: newest becomes oldest, next newest becomes newest
       11. Remove All Layers: essentially a Stop function
     
     too many? i can imagine these being useful though
    */
    
    bool doCluster = targetStates[TargetTypeSynchronic] == TargetStateEnabled; // primary Synchronic mode
    bool doPatternSync = targetStates[TargetTypeSynchronicPatternSync] == TargetStateEnabled; // only if resetting pattern phases
    bool doBeatSync = targetStates[TargetTypeSynchronicBeatSync] == TargetStateEnabled; // only if resetting beat phase
    bool doAddNotes = targetStates[TargetTypeSynchronicAddNotes] == TargetStateEnabled; // if only adding notes to cluster
    bool doPausePlay = targetStates[TargetTypeSynchronicPausePlay] == TargetStateEnabled; // if targeting pause/play
    bool doClear = targetStates[TargetTypeSynchronicClear] == TargetStateEnabled;
    bool doDeleteOldest = targetStates[TargetTypeSynchronicDeleteOldest] == TargetStateEnabled;
    bool doDeleteNewest = targetStates[TargetTypeSynchronicDeleteNewest] == TargetStateEnabled;
    bool doRotate = targetStates[TargetTypeSynchronicRotate] == TargetStateEnabled;
    
    // is this a new cluster?
    bool isNewCluster = false;
    
    // add note to clusterKeysDepressed (keys targeting the main synchronic functionality)
    if (doCluster) clusterKeysDepressed.addIfNotAlreadyThere(noteNumber);

    // always work on the most recent cluster/layer
    SynchronicCluster::Ptr cluster = clusters.getLast();
    
    // do only if this note is targeted as a primary Synchronic note (TargetTypeSynchronic)
    if (doCluster && !pausePlay)
    {
        
        // Remove old clusters, deal with layers and NoteOffSync modes
        for (int i = clusters.size(); --i >= 0; )
        {
            if (!clusters[i]->getShouldPlay() && !inCluster)
            {
                clusters.remove(i);
                continue;
            }
            
            if(   (synchronic->prep->sMode.value == LastNoteOffSync)
               || (synchronic->prep->sMode.value == AnyNoteOffSync))
            {
                if(clusters.size() == 1) clusters[0]->setShouldPlay(false);
                else
                {
                    if(clusters[i]->containsNote(noteNumber))
                    {
                        clusters[i]->removeNote(noteNumber);
                    }
                    
                }
            }
        }
         
        
        // OnOffMode determines whether the keyOffs or keyOns determine whether notes are within the cluster threshold
        // here, we only look at keyOns
        if (prep->onOffMode.value == KeyOn) // onOffMode.value is set by the "determines cluster"
        {
            // if we have a new cluster
            if (!inCluster || cluster == nullptr)
            {
                // check to see if we have as many clusters as we are allowed, remove oldest if we are
                if (clusters.size() >= synchronic->prep->numClusters.value)
                {
                    clusters.remove(0); // remove first (oldest) cluster
                }
                
                // make the new one and add it to the array of clusters
                cluster = new SynchronicCluster(prep);
                clusters.add(cluster); // add to the array of clusters (what are called "layers" in the UI
                
                // this is a new cluster!
                isNewCluster = true;
            }
            
            // add this played note to the cluster
            cluster->addNote(noteNumber);
            
            // yep, we are in a cluster!
            inCluster = true;
            
            // reset the timer for time between notes; we do this for every note added to a cluster
            clusterThresholdTimer = 0;
            
            // reset the beat phase and pattern phase, and start playing, depending on the mode
            if (prep->sMode.value == AnyNoteOnSync)
            {
                cluster->setShouldPlay(true);
                cluster->setBeatPhasor(0);
                cluster->resetPatternPhase();
            }
            else if (prep->sMode.value == FirstNoteOnSync)
            {
                cluster->setShouldPlay(true);
                
                if (isNewCluster)
                {
                    cluster->setBeatPhasor(0);
                    cluster->resetPatternPhase();
                }
            }
            
            /*
            // reset the beat phase and pattern phase, and start playing, depending on the mode
            if ((prep->getMode() == AnyNoteOnSync) || (prep->getMode() == FirstNoteOnSync))
            {
                cluster->setShouldPlay(true);
                
                if (isNewCluster)
                {
                    cluster->setBeatPhasor(0);
                    cluster->resetPatternPhase();
                }
            }
             */
        }
    }
    
    // we should have a cluster now, but if not...
    if (cluster == nullptr) return;
    
    // since it's a new cluster, the next noteOff will be a first noteOff
    // this will be needed for keyReleased(), when in FirstNoteOffSync mode
    if (isNewCluster) nextOffIsFirst = true;
    
    
    // ** now trigger behaviors set by Keymap targeting **
    //
    // synchronize beat, if targeting beat sync on noteOn or on both noteOn/Off
    if (doBeatSync && (prep->getTargetTypeSynchronicBeatSync() == NoteOn || prep->getTargetTypeSynchronicBeatSync() == Both))
    {
        //start right away
        uint64 phasor = beatThresholdSamples *
                        synchronic->prep->sBeatMultipliers.value[cluster->getBeatMultiplierCounter()] *
                        general->getPeriodMultiplier() *
                        tempo->getPeriodMultiplier();
        
        cluster->setBeatPhasor(phasor);      // resetBeatPhasor resets beat timing
    }
    
    // resetPatternPhase() starts patterns over, if targeting beat sync on noteOn or on both noteOn/Off
    if (doPatternSync && (prep->getTargetTypeSynchronicPatternSync() == NoteOn || prep->getTargetTypeSynchronicPatternSync() == Both))   cluster->resetPatternPhase();
    
    // add notes to the cluster, if targeting beat sync on noteOn or on both noteOn/Off
    if (doAddNotes && (prep->getTargetTypeSynchronicAddNotes() == NoteOn || prep->getTargetTypeSynchronicAddNotes() == Both))      cluster->addNote(noteNumber);
    
    // toggle pause/play, if targeting beat sync on noteOn or on both noteOn/Off
    if (doPausePlay && (prep->getTargetTypeSynchronicPausePlay() == NoteOn || prep->getTargetTypeSynchronicPausePlay() == Both))
    {
        if (pausePlay) pausePlay = false;
        else pausePlay = true;
    }
    
    if (doClear && (prep->getTargetTypeSynchronicClear() == NoteOn || prep->getTargetTypeSynchronicClear() == Both))
    {
        clusters.clear();
    }
    
    if (doDeleteOldest && (prep->getTargetTypeSynchronicDeleteOldest() == NoteOn || prep->getTargetTypeSynchronicDeleteOldest() == Both))
    {
        if (!clusters.isEmpty()) clusters.remove(0);
    }
    
    if (doDeleteNewest && (prep->getTargetTypeSynchronicDeleteNewest() == NoteOn || prep->getTargetTypeSynchronicDeleteNewest() == Both))
    {
        if (!clusters.isEmpty()) clusters.remove(clusters.size() - 1);
    }
    
    if (doRotate && (prep->getTargetTypeSynchronicRotate() == NoteOn || prep->getTargetTypeSynchronicRotate() == Both))
    {
        if (!clusters.isEmpty())
        {
            clusters.move(clusters.size() - 1, 0);
        }
    }
}

void SynchronicProcessor::keyReleased(int noteNumber, float velocity, int channel, Array<KeymapTargetState> targetStates)
{
    SynchronicPreparation::Ptr prep = synchronic->prep;
    
    // remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    // track the note's target, as set in Keymap; save as needed
    bool doCluster = targetStates[TargetTypeSynchronic] == TargetStateEnabled;
    bool doPatternSync = targetStates[TargetTypeSynchronicPatternSync] == TargetStateEnabled;
    bool doBeatSync = targetStates[TargetTypeSynchronicBeatSync] == TargetStateEnabled;
    bool doAddNotes = targetStates[TargetTypeSynchronicAddNotes] == TargetStateEnabled;
    bool doPausePlay = targetStates[TargetTypeSynchronicPausePlay] == TargetStateEnabled;
    bool doClear = targetStates[TargetTypeSynchronicClear] == TargetStateEnabled;
    bool doDeleteOldest = targetStates[TargetTypeSynchronicDeleteOldest] == TargetStateEnabled;
    bool doDeleteNewest = targetStates[TargetTypeSynchronicDeleteNewest] == TargetStateEnabled;
    bool doRotate = targetStates[TargetTypeSynchronicRotate] == TargetStateEnabled;
    
    DBG("SynchronicProcessor::doCluster " + String((int)doCluster));
    
    // is this a new cluster?
    bool isNewCluster = false;

    // remove key from cluster-targeted keys
    if (doCluster) clusterKeysDepressed.removeAllInstancesOf(noteNumber);
    
    // do velocity and hold-time filtering (how long the key was held down)
    if (!velocityCheck(noteNumber)) return;
    if (!holdCheck(noteNumber)) return;
    
    // always work on the most recent cluster/layer
    SynchronicCluster::Ptr cluster = clusters.getLast();
    
    // do only if this note is targeted as a primary Synchronic note (TargetTypeSynchronic)
    if (doCluster && !pausePlay)
    {
        // cluster management
        // OnOffMode determines whether the timing of keyOffs or keyOns determine whether notes are within the cluster threshold
        // in this case, we only want to do these things when keyOffs set the clusters
        if (synchronic->prep->onOffMode.value == KeyOff) // set in the "determines cluster" menu
        {
            if (!inCluster || cluster == nullptr)
            {
                if (clusters.size() >= synchronic->prep->numClusters.value)
                {
                    // remove first (oldest) cluster
                    clusters.remove(0);
                }
                
                // make the new cluster, add it to the array of clusters
                cluster = new SynchronicCluster(prep);
                clusters.add(cluster);
                
                // this is a new cluster!
                isNewCluster = true;
            }
            
            // add the note to the cluster
            cluster->addNote(noteNumber);
            
            // yes, we are in a cluster
            inCluster = true;
                        
            // if it's a new cluster, the next noteOff will be a first noteOff
            // this will be needed for FirstNoteOffSync mode
            if (isNewCluster) nextOffIsFirst = true;
            
            // reset the timer for time between notes
            clusterThresholdTimer = 0;
        }
            
        // depending on the mode, and whether this is a first or last note, reset the beat and pattern phase and start playing
        if ((synchronic->prep->sMode.value == FirstNoteOffSync && nextOffIsFirst) ||
            (synchronic->prep->sMode.value == AnyNoteOffSync) ||
            (synchronic->prep->sMode.value == LastNoteOffSync && clusterKeysDepressed.size() == 0))
        {
            for (int i = clusters.size(); --i >= 0; )
            {
                if(clusters[i]->containsNote(noteNumber))
                {
                    clusters[i]->resetPatternPhase();
                    clusters[i]->setShouldPlay(true);
                    
                    //start right away
                    uint64 phasor = beatThresholdSamples *
                                    synchronic->prep->sBeatMultipliers.value[cluster->getBeatMultiplierCounter()] *
                                    general->getPeriodMultiplier() *
                                    tempo->getPeriodMultiplier();
                    
                    clusters[i]->setBeatPhasor(phasor);
                }
            }
            
            // have done at least one noteOff, so next one will not be first one.
            nextOffIsFirst = false;
        }
    }
    
    // we should have a cluster now, but if not...
    if (cluster == nullptr) return;
    
    // ** now trigger behaviors set by Keymap targeting **
    //
    // synchronize beat, if targeting beat sync on noteOff or on both noteOn/Off
    if (doBeatSync && (prep->getTargetTypeSynchronicBeatSync() == NoteOff || prep->getTargetTypeSynchronicBeatSync() == Both))
    {
        //start right away
        uint64 phasor = beatThresholdSamples *
                        synchronic->prep->sBeatMultipliers.value[cluster->getBeatMultiplierCounter()] *
                        general->getPeriodMultiplier() *
                        tempo->getPeriodMultiplier();
        
        cluster->setBeatPhasor(phasor);      // resetBeatPhasor resets beat timing
        cluster->setShouldPlay(true);
    }
    
    // resetPatternPhase() starts patterns over, if targeting beat sync on noteOff or on both noteOn/Off
    if (doPatternSync && (prep->getTargetTypeSynchronicPatternSync() == NoteOff || prep->getTargetTypeSynchronicPatternSync() == Both))
    {
        cluster->resetPatternPhase();
        cluster->setShouldPlay(true);
    }
    
    // add notes to the cluster, if targeting beat sync on noteOff or on both noteOn/Off
    if (doAddNotes && (prep->getTargetTypeSynchronicAddNotes() == NoteOff || prep->getTargetTypeSynchronicAddNotes() == Both))      cluster->addNote(noteNumber);
    
    // toggle pause/play, if targeting beat sync on noteOff or on both noteOn/Off
    if (doPausePlay && (prep->getTargetTypeSynchronicPausePlay() == NoteOff || prep->getTargetTypeSynchronicPausePlay() == Both))
    {
        if (pausePlay) pausePlay = false;
        else pausePlay = true;
    }
    
    if (doClear && (prep->getTargetTypeSynchronicClear() == NoteOff || prep->getTargetTypeSynchronicClear() == Both))
    {
        clusters.clear();
    }
    
    if (doDeleteOldest && (prep->getTargetTypeSynchronicDeleteOldest() == NoteOff || prep->getTargetTypeSynchronicDeleteOldest() == Both))
    {
        if (!clusters.isEmpty()) clusters.remove(0);
    }
    
    if (doDeleteNewest && (prep->getTargetTypeSynchronicDeleteNewest() == NoteOff || prep->getTargetTypeSynchronicDeleteNewest() == Both))
    {
        if (!clusters.isEmpty()) clusters.remove(clusters.size() - 1);
    }
    
    if (doRotate && (prep->getTargetTypeSynchronicRotate() == NoteOff || prep->getTargetTypeSynchronicRotate() == Both))
    {
        if (!clusters.isEmpty())
        {
            clusters.move(clusters.size() - 1, 0);
        }
    }
}

    
void SynchronicProcessor::processBlock(int numSamples, int channel, BKSampleLoadType type)
{
    // don't do anything if we are paused!
    if (pausePlay) return;
    
    SynchronicPreparation::Ptr prep = synchronic->prep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->prep;
    
    while (clusters.size() > prep->numClusters.value)
    {
        clusters.remove(0);
    }
    
    //do this every block, for adaptive tempo updates
    sampleType = type;
    thresholdSamples = (prep->sClusterThreshSec.value * synth->getSampleRate());

    if (tempoPrep->getTempoSystem() == AdaptiveTempo1)
    {
        beatThresholdSamples = (tempoPrep->getBeatThresh() * synth->getSampleRate());
    }
    else
    {
        beatThresholdSamples = (tempoPrep->getBeatThresh() / tempoPrep->getSubdivisions() * synth->getSampleRate());
    }
    
    for (auto key : keysDepressed)
    {
        uint64 time = holdTimers.getUnchecked(key) + numSamples;
        holdTimers.setUnchecked(key, time);
    }
    
    //cluster management
    if (inCluster)
    {
        //moved beyond clusterThreshold time, done with cluster
        if (clusterThresholdTimer >= thresholdSamples)
        {
            inCluster = false;
        }
        
        //otherwise incrument cluster timer
        else
        {
            clusterThresholdTimer += numSamples;
        }
    }
    
    bool play = false;
    
    for (int i = clusters.size(); --i >= 0;)
    {
        SynchronicCluster::Ptr cluster = clusters.getUnchecked(i);
    
        if (cluster->getShouldPlay()) // check if pulses are done and remove cluster if so
        {
            play = true;
            
            Array<int> clusterNotes = cluster->getCluster();
            
            //DBG("cluster " + String(i) + ": " + intArrayToString(clusterNotes));
            
            //cap size of slimCluster, removing oldest notes
            Array<int> tempCluster;
            for(int i = 0; i < clusterNotes.size(); i++) tempCluster.set(i, clusterNotes.getUnchecked(i));
            if(tempCluster.size() > prep->sClusterCap.value) tempCluster.resize(prep->sClusterCap.value);
            
            //why not use clusterMax for this? the intent is different:
            //clusterMax: max number of keys pressed within clusterThresh, otherwise shut off pulses
            //clusterCap: the most number of notes allowed in a cluster when playing pulses
            //an example: clusterMax=9, clusterCap=8; playing 9 notes simultaneously will result in cluster with 8 notes, but playing 10 notes will shut off pulse
            //another example: clusterMax=20, clusterCap=8; play a rapid ascending scale more than 8 and less than 20 notes, then stop; only last 8 notes will be in the cluster. If your scale exceeds 20 notes then it won't play.
            
            //for now, we'll leave clusterCap unexposed, just to avoid confusion for the user. after all,
            //I used it this way for all of the etudes to date! but might want to expose eventually...
            //perhaps call beatVoices? since it's essentially the number of "voices" in the pulse chord?
            
            //remove duplicates from cluster, so we don't play the same note twice in a single pulse
            slimCluster.clearQuick();
            for(int i = 0; i < tempCluster.size(); i++)
            {
                slimCluster.addIfNotAlreadyThere(tempCluster.getUnchecked(i));
            }
            
            //DBG("slimCluster " + String(i) + ": " + intArrayToString(slimCluster));
            
            //get time until next beat => beat length scaled by beatMultiplier parameter
            
            numSamplesBeat =    beatThresholdSamples *
            prep->sBeatMultipliers.value[cluster->getBeatMultiplierCounter()] *
            general->getPeriodMultiplier() *
            tempo->getPeriodMultiplier();
            
            //check to see if enough time has passed for next beat
            if (cluster->getPhasor() >= numSamplesBeat)
            {
                //update display of counters in UI
                /*
                 DBG(" samplerate: " + String(sampleRate) +
                 " length: "         + String(prep->sLengthMultipliers.value[lengthMultiplierCounter]) +
                 " length counter: "  + String(lengthMultiplierCounter) +
                 " accent: "         + String(prep->getAccentMultipliers()[accentMultiplierCounter]) +
                 " accent counter: " + String(accentMultiplierCounter) +
                 " transp: "         + "{ "+floatArrayToString(prep->getTransposition()[transpCounter]) + " }" +
                 " transp counter: " + String(transpCounter) +
                 " envelope on: "       + String((int)prep->getEnvelopesOn()[envelopeCounter]) +
                 " envelope counter: " + String(envelopeCounter) +
                 " ADSR :" + String(prep->getAttack(envelopeCounter)) + " " + String(prep->getDecay(envelopeCounter)) + " " + String(prep->getSustain(envelopeCounter)) + " " + String(prep->getRelease(envelopeCounter))
                 );
                 */
                cluster->step(numSamplesBeat);
                //cluster->postStep();
                
                //figure out whether to play the cluster
                bool passCluster = false;
                
                //in the normal case, where cluster is within a range defined by clusterMin and Max
                if(prep->sClusterMin.value <= prep->sClusterMax.value)
                {
                    if (clusterNotes.size() >= prep->sClusterMin.value && clusterNotes.size() <= prep->sClusterMax.value)
                        passCluster = true;
                }
                //the inverse case, where we only play cluster that are *outside* the range set by clusterMin and Max
                else
                {
                    if (clusterNotes.size() >= prep->sClusterMin.value || clusterNotes.size() <= prep->sClusterMax.value)
                        passCluster = true;
                }
                
                playCluster = passCluster;
                
                if(playCluster)
                {
                    for (int n=0; n < slimCluster.size(); n++)
                    {
                        playNote(channel,
                                 slimCluster[n],
                                 velocitiesActive.getUnchecked(slimCluster[n]),
                                 cluster);
                    }
                }
                
                // step cluster data
                cluster->postStep();
                
            }
            
            //pass time until next beat
            cluster->incrementPhasor(numSamples);
        }
    }
    playCluster = play;  
    
//    MidiBuffer midiBuffer = MidiBuffer();
   
    for (int i = 0; i < activeSynchronicVoices.size(); ++i)
    {
        BKSynthesiserVoice* const voice = activeSynchronicVoices.getUnchecked(i);
        if (voice->getCurrentlyPlayingNote() < 0) {
            int noteNumber = voiceMidiValues.getUnchecked(i);
            if (noteNumber < 0) continue;
            const MidiMessage message = MidiMessage::noteOff(1, noteNumber);
            prep->midiOutput.value->sendMessageNow(message);

            // Collecting all the off in a buffer might have better performance at the cost of using more memory
            // than sendMessageNow()
            // Either seems 100% fine for normal cases, just not sure about extremes
//            midiBuffer.addEvent(message, 1);
            DBG("MIDI Message sent: Note off " + String(noteNumber));
            activeSynchronicVoices.remove(i);
            voiceMidiValues.remove(i--);
        }
    }
//    if (!midiBuffer.isEmpty()) prep->midiOutput.value->sendBlockOfMessagesNow(midiBuffer);
}

//return time in ms to future beat, given beatsToSkip
float SynchronicProcessor::getTimeToBeatMS(float beatsToSkip)
{
    // Using first cluster as default because this affects Nostalgic behavior
    SynchronicCluster::Ptr cluster = clusters.getFirst();
    
    if (cluster != nullptr)
    {
        uint64 timeToReturn = numSamplesBeat - cluster->getPhasor(); //next beat
        int myBeat = cluster->getBeatMultiplierCounter();
        
        //tolerance: if key release happens just before beat (<100ms) then add a beatToSkip
        if (timeToReturn < 0.100f * synth->getSampleRate()) beatsToSkip++;
        
        while(beatsToSkip-- > 0)
        {
            if (++myBeat >= synchronic->prep->sBeatMultipliers.value.size()) myBeat = 0;
            timeToReturn += synchronic->prep->sBeatMultipliers.value[myBeat] *
            beatThresholdSamples *
            general->getPeriodMultiplier() *
            tempo->getPeriodMultiplier();
        }
        
        DBG("time in ms to next beat = " + String(timeToReturn * 1000./synth->getSampleRate()));
        return timeToReturn * 1000./synth->getSampleRate(); //optimize later....
    }
    return 0.0f;
}

#if BK_UNIT_TESTS


class SynchronicTests : public UnitTest
{
public:
	SynchronicTests() : UnitTest("Synchronics", "Synchronic") {}

	void runTest() override
	{
		beginTest("SynchronicPreparation");

		for (int i = 0; i < 10; i++)
		{
			// create synchronic preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random SynchronicPreparation " + String(i);
			DBG("test consistency: " + name);

			SynchronicPreparation::Ptr sp1 = new SynchronicPreparation();
			sp1->randomize();

			Synchronic s1(sp1, 1);
			s1.setName(name);

			ValueTree vt1 = s1.getState();

			XmlElement* xml = vt1.createXml();

			SynchronicPreparation::Ptr sp2 = new SynchronicPreparation();

			Synchronic s2(sp2, 1);

			//dummy parameters for setState
			Tuning::PtrArr t;
			Tempo::PtrArr m;

			s2.setState(xml, t, m);
			s2.setName(name);

			ValueTree vt2 = s2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"synchronic preparation: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());

			//expect(sp2->compare(sp1), sp1->getName() + " and " + sp2->getName() + " did not match.");
		}

		//test synchronic rather than synchronicPreparation
		for (int i = 0; i < 10; i++)
		{
			// create synchronic preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random synchronic " + String(i);
			DBG("test consistency: " + name);

			Synchronic s1(-1, true);
			s1.setName(name);

			ValueTree vt1 = s1.getState();

			XmlElement* xml = vt1.createXml();

			Synchronic s2(-1, true);

			//dummy parameters for setState
			Tuning::PtrArr t;
			Tempo::PtrArr m;

			s2.setState(xml, t, m);
			s2.setName(name);

			ValueTree vt2 = s2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"synchronic: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());
		}
	}
};

static SynchronicTests synchronicTests;


class SynchronicModTests : public UnitTest
{
public:
	SynchronicModTests() : UnitTest("SynchronicMods", "SynchronicMod") {}

	void runTest() override
	{
		beginTest("SynchronicMod");

		for (int i = 0; i < 10; i++)
		{
			// create synchronic mod preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random synchronic mod " + String(i);
			DBG("test consistency: " + name);

			SynchronicPreparation::Ptr sp1 = new SynchronicPreparation();
			SynchronicModification::Ptr sm1 = new SynchronicModification(sp1, 1);


			sm1->randomize();
			sm1->setName(name);

			ValueTree vt1 = sm1->getState();

			XmlElement* xml = vt1.createXml();

			SynchronicPreparation::Ptr sp2 = new SynchronicPreparation();
			SynchronicModification::Ptr sm2 = new SynchronicModification(sp2, 1);

			sm2->setState(xml);

			ValueTree vt2 = sm2->getState();

			expect(vt1.isEquivalentTo(vt2),
				"synchronic mod: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());

		}

	}
};

static SynchronicModTests synchronicModTests;


#endif
