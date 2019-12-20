/*
  ==============================================================================

    Synchronic.cpp
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Synchronic.h"

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
keymaps(Keymap::PtrArr())
{
    velocities.ensureStorageAllocated(128);
    holdTimers.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        velocities.insert(i, 0.);
        holdTimers.insert(i, 0);
    }

    inCluster = false;
    inSyncCluster = false;
    
    keysDepressed = Array<int>();
    syncKeysDepressed = Array<int>();
    clusterKeysDepressed = Array<int>();
    
    activeSynchronicVoices = Array<BKSynthesiserVoice*>();
    activeSynchronicVoices.resize(128);
    activeSynchronicVoices.fill(nullptr);
}



SynchronicProcessor::~SynchronicProcessor()
{
}

void SynchronicProcessor::setCurrentPlaybackSampleRate(double sr)
{
    sampleRate = sr;
    
}


void SynchronicProcessor::playNote(int channel, int note, float velocity, SynchronicCluster::Ptr cluster)
{
    SynchronicPreparation::Ptr prep = synchronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    PianoSamplerNoteDirection noteDirection = Forward;
    float noteStartPos = 0.0;
    
    float noteLength = 0.0;
    
 
    if (tempoPrep->getTempoSystem() == AdaptiveTempo1)
    {
        noteLength = (fabs(prep->getLengthMultipliers()[cluster->getLengthMultiplierCounter()]) * tempoPrep->getBeatThreshMS());
    }
    else
    {
        noteLength = (fabs(prep->getLengthMultipliers()[cluster->getLengthMultiplierCounter()]) * tempoPrep->getBeatThreshMS() / tempoPrep->getSubdivisions());
    }
    
    
    if (prep->getLengthMultipliers()[cluster->getLengthMultiplierCounter()] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength + 3; //adjust for rampOn time == 3ms
    }
    
    for (auto t : prep->getTransposition()[cluster->getTranspCounter()])
    {

        float offset = t + tuner->getOffset(note, false), synthOffset = offset;
        int synthNoteNumber = (float)note;
        
        //if (sampleType < BKLoadSoundfont)
        {
            synthNoteNumber += (int)offset;
            synthOffset     -= (int)offset;
        }

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
                    prep->getGain() * aGlobalGain * prep->getAccentMultipliers()[cluster->getAccentMultiplierCounter()],
                    noteDirection,
                    FixedLengthFixedStart,
                    SynchronicNote,
                    synchronic->getId(),
                    noteStartPos,  // start
                    noteLength,
                    prep->getAttack(whichEnv),
                    prep->getDecay(whichEnv),
                    prep->getSustain(whichEnv),
                    prep->getRelease(whichEnv),
                    tuner,
                    1.,//b->getBlendronic()->aPrep->getInputGain(),
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
					prep->getGain() * aGlobalGain * prep->getAccentMultipliers()[cluster->getAccentMultiplierCounter()],
					noteDirection,
					FixedLengthFixedStart,
					SynchronicNote,
					synchronic->getId(),
					noteStartPos,  // start
					noteLength,
					prep->getAttack(whichEnv),
					prep->getDecay(whichEnv),
					prep->getSustain(whichEnv),
					prep->getRelease(whichEnv),
					tuner);
		}
        
        if (prep->getMidiOutput() != nullptr)
        {
            activeSynchronicVoices.set(currentVoice->getCurrentlyPlayingNote(), currentVoice);
            const MidiMessage message = MidiMessage::noteOn(1, synthNoteNumber, velocity);
            prep->getMidiOutput()->sendMessageNow(message);
            DBG("MIDI Message sent: Note on " + String(synthNoteNumber));
        }

    }
    
}

bool SynchronicProcessor::velocityCheck(int noteNumber)
{
    SynchronicPreparation::Ptr prep = synchronic->aPrep;
    
    int velocity = (int)(velocities.getUnchecked(noteNumber) * 127.0);
    
    if (velocity > 127) velocity = 127;
    if (velocity < 0)   velocity = 0;

    if(prep->getVelocityMin() <= prep->getVelocityMax())
    {
        if (velocity >= prep->getVelocityMin() && velocity <= prep->getVelocityMax())
        {
            return true;
        }
    }
    else
    {
        if (velocity >= prep->getVelocityMin() || velocity <= prep->getVelocityMax())
        {
            return true;
        }
    }
    
    DBG("failed velocity check");
    return false;
}

bool SynchronicProcessor::holdCheck(int noteNumber)
{
    SynchronicPreparation::Ptr prep = synchronic->aPrep;
    
    uint64 hold = holdTimers.getUnchecked(noteNumber) * (1000.0 / sampleRate);
    
    if(prep->getHoldMin() <= prep->getHoldMax())
    {
        if (hold >= prep->getHoldMin() && hold <= prep->getHoldMax())
        {
            return true;
        }
    }
    else
    {
        if (hold >= prep->getHoldMin() || hold <= prep->getHoldMax())
        {
            return true;
        }
    }
    
    DBG("failed hold check");
    return false;
}

void SynchronicProcessor::keyPressed(int noteNumber, float velocity, Array<KeymapTargetState> targetStates)
{
    SynchronicPreparation::Ptr prep = synchronic->aPrep;
    
    lastKeyPressed = noteNumber;
    lastKeyVelocity = velocity;
    
    //add note to array of depressed notes
    // should maybe keep track of sync and cluster keys depressed separately for first/last note on/off stuff
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    velocities.set(noteNumber, velocity);
    holdTimers.set(noteNumber, 0);
    
    bool doSync = targetStates[TargetTypeSynchronicSync] == TargetStateEnabled;
    bool doCluster = targetStates[TargetTypeSynchronicCluster] == TargetStateEnabled;
    
    if (doSync) syncKeysDepressed.addIfNotAlreadyThere(noteNumber);
    if (doCluster) clusterKeysDepressed.addIfNotAlreadyThere(noteNumber);
    
    if (!velocityCheck(noteNumber)) return;
    
    // Remove old or untriggered clusters
    if (doCluster)
    {
        for (int i = clusters.size(); --i >= 0; )
        {
            if (clusters[i]->getOver())
            {
                clusters.remove(i);
                continue;
            }
        }
    }
    
    SynchronicCluster::Ptr cluster = clusters.getLast();
    
    // If clusters are based on note ons
    if (prep->getOnOffMode() == KeyOn)
    {
        if (doCluster)
        {
            if (!inCluster || cluster == nullptr)
            {
                if (clusters.size() >= synchronic->aPrep->getNumClusters())
                {
                    clusters.remove(0); // remove first (oldest) cluster
                }
                
                cluster = new SynchronicCluster(prep);
                clusters.add(cluster);
                nextSyncOffIsFirst = true;
            }
            
            cluster->addNote(noteNumber);
            
            inCluster = true;
            
            //reset the timer for time between notes
            clusterThresholdTimer = 0;
        }
    }
    
    if (cluster == nullptr) return;
    
    if (doSync)
    {
        if (prep->getMode() == AnyNoteOnSync)
        {
            cluster->setPhasor(0);
            cluster->resetPhase();
            cluster->setShouldPlay(true);
        }
        // First note for sync can be either a sync-targeting note pressed when no other sync notes are pressed
        // or the first sync note after a new cluster has been created (does this make sense?)
        else if ((prep->getMode() == FirstNoteOnSync) && !inSyncCluster)
        {
            cluster->setPhasor(0);
            cluster->resetPhase();
            cluster->setShouldPlay(true);
        }
        
        nextSyncOffIsFirst = true;
        inSyncCluster = true;
        
        syncThresholdTimer = 0;
    }
}


void SynchronicProcessor::keyReleased(int noteNumber, float velocity, int channel, Array<KeymapTargetState> targetStates)
{
    SynchronicPreparation::Ptr prep = synchronic->aPrep;
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    bool doSync = targetStates[TargetTypeSynchronicSync] == TargetStateEnabled;
    bool doCluster = targetStates[TargetTypeSynchronicCluster] == TargetStateEnabled;
    
    if (doSync) syncKeysDepressed.removeAllInstancesOf(noteNumber);
    if (doCluster) clusterKeysDepressed.removeAllInstancesOf(noteNumber);
    
    
    if (!velocityCheck(noteNumber)) return;
    if (!holdCheck(noteNumber)) return;
    
    // Remove old or untriggered clusters
    if (doCluster)
    {
        for (int i = clusters.size(); --i >= 0; )
        {
            if (clusters[i]->getOver())
            {
                clusters.remove(i);
                continue;
            }
        }
    }
    
    SynchronicCluster::Ptr cluster = clusters.getLast();
    
    //cluster management
    if (synchronic->aPrep->getOnOffMode() == KeyOff)
    {
        if (doCluster)
        {
            if (!inCluster || cluster == nullptr)
            {
                if (clusters.size() >= synchronic->aPrep->getNumClusters())
                {
                    clusters.remove(0); // remove first (oldest) cluster
                }
                
                cluster = new SynchronicCluster(prep);
                clusters.add(cluster);
            }
            cluster->addNote(noteNumber);
            
            inCluster = true;
            
            //reset the timer for time between notes
            clusterThresholdTimer = 0;
        }
        else if (cluster == nullptr) return;
    }
    
    // If AnyNoteOffSync mode, reset phasor and multiplier indices.
    //only initiate pulses if ALL keys are released
    if (doSync)
    {
        if ((synchronic->aPrep->getMode() == FirstNoteOffSync && nextSyncOffIsFirst) ||
            (synchronic->aPrep->getMode() == AnyNoteOffSync) ||
            (synchronic->aPrep->getMode() == LastNoteOffSync && syncKeysDepressed.size() == 0))
        {
            if (doCluster)
            {
                for (int i = clusters.size(); --i >= 0; )
                {
                    if(clusters[i]->containsNote(noteNumber))
                    {
                        clusters[i]->resetPhase();
                        clusters[i]->setShouldPlay(true);
                        
                        //start right away
                        uint64 phasor = beatThresholdSamples *
                                        synchronic->aPrep->getBeatMultipliers()[cluster->getBeatMultiplierCounter()] *
                                        general->getPeriodMultiplier() *
                                        tempo->getPeriodMultiplier();
                        
                        clusters[i]->setPhasor(phasor);
                    }
                }
            }
            else
            {
                cluster->resetPhase();
                cluster->setShouldPlay(true);
                
                //start right away
                uint64 phasor = beatThresholdSamples *
                synchronic->aPrep->getBeatMultipliers()[cluster->getBeatMultiplierCounter()] *
                general->getPeriodMultiplier() *
                tempo->getPeriodMultiplier();
                
                cluster->setPhasor(phasor);
            }
        }
        nextSyncOffIsFirst = false;
    }
}


void SynchronicProcessor::processBlock(int numSamples, int channel, BKSampleLoadType type)
{
    SynchronicPreparation::Ptr prep = synchronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    while (clusters.size() > prep->getNumClusters())
    {
        clusters.remove(0);
    }
    
    //do this every block, for adaptive tempo updates
    sampleType = type;
    thresholdSamples = (prep->getClusterThreshSEC() * sampleRate);

    if (tempoPrep->getTempoSystem() == AdaptiveTempo1)
    {
        beatThresholdSamples = (tempoPrep->getBeatThresh() * sampleRate);
    }
    else
    {
        beatThresholdSamples = (tempoPrep->getBeatThresh() / tempoPrep->getSubdivisions() * sampleRate);
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
    
    if (inSyncCluster)
    {
        if (syncThresholdTimer >= thresholdSamples)
        {
            inSyncCluster = false;
        }
        else
        {
            syncThresholdTimer += numSamples;
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
            for(int i = 0; i< clusterNotes.size(); i++) tempCluster.set(i, clusterNotes.getUnchecked(i));
            if(tempCluster.size() > prep->getClusterCap()) tempCluster.resize(prep->getClusterCap());
            
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
            for(int i = 0; i< tempCluster.size(); i++)
            {
                slimCluster.addIfNotAlreadyThere(tempCluster.getUnchecked(i));
            }
            
            //DBG("slimCluster " + String(i) + ": " + intArrayToString(slimCluster));
            
            //get time until next beat => beat length scaled by beatMultiplier parameter
            
            numSamplesBeat =    beatThresholdSamples *
            prep->getBeatMultipliers()[cluster->getBeatMultiplierCounter()] *
            general->getPeriodMultiplier() *
            tempo->getPeriodMultiplier();
            
            //check to see if enough time has passed for next beat
            if (cluster->getPhasor() >= numSamplesBeat)
            {
                //update display of counters in UI
                /*
                 DBG(" samplerate: " + String(sampleRate) +
                 " length: "         + String(prep->getLengthMultipliers()[lengthMultiplierCounter]) +
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
                if(prep->getClusterMin() <= prep->getClusterMax())
                {
                    if (clusterNotes.size() >= prep->getClusterMin() && clusterNotes.size() <= prep->getClusterMax())
                        passCluster = true;
                }
                //the inverse case, where we only play cluster that are *outside* the range set by clusterMin and Max
                else
                {
                    if (clusterNotes.size() >= prep->getClusterMin() || clusterNotes.size() <= prep->getClusterMax())
                        passCluster = true;
                }
                
                playCluster = passCluster;
                
                if(playCluster)
                {
                    for (int n=0; n < slimCluster.size(); n++)
                    {
                        playNote(channel,
                                 slimCluster[n],
                                 velocities.getUnchecked(slimCluster[n]),
                                 cluster);
                    }
                }
                
                // step cluster data
                //cluster->step(numSamplesBeat);
                cluster->postStep();
                
            }
            
            //pass time until next beat
            cluster->incrementPhasor(numSamples);
        }
    }
    playCluster = play;  
    
    MidiBuffer midiBuffer = MidiBuffer();
    int i = 0;
    for (BKSynthesiserVoice* const voice : activeSynchronicVoices) {
        if (voice == nullptr) continue;
        if (!voice->isVoiceActive()) {
            int noteNumber = activeSynchronicVoices.indexOf(voice);
            if (noteNumber < 0) continue;
            activeSynchronicVoices.set(noteNumber, nullptr);
            const MidiMessage message = MidiMessage::noteOff(1, noteNumber);
            midiBuffer.addEvent(message, i);
            DBG("MIDI Message sent: Note off " + String(noteNumber));
        }
    }
    if (!midiBuffer.isEmpty()) prep->getMidiOutput()->sendBlockOfMessagesNow(midiBuffer);
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
        if (timeToReturn < 0.100f * sampleRate) beatsToSkip++;
        
        while(beatsToSkip-- > 0)
        {
            if (++myBeat >= synchronic->aPrep->getBeatMultipliers().size()) myBeat = 0;
            timeToReturn += synchronic->aPrep->getBeatMultipliers()[myBeat] *
            beatThresholdSamples *
            general->getPeriodMultiplier() *
            tempo->getPeriodMultiplier();
        }
        
        DBG("time in ms to next beat = " + String(timeToReturn * 1000./sampleRate));
        return timeToReturn * 1000./sampleRate; //optimize later....
    }
    
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
