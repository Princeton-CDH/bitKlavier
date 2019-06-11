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
                                         BKSynthesiser* main,
                                         GeneralSettings::Ptr general):
synth(main),
general(general),
synchronic(synchronic),
tuner(tuning),
tempo(tempo)
{
    velocities.ensureStorageAllocated(128);
    holdTimers.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        velocities.insert(i, 0.);
        holdTimers.insert(i, 0);
    }

    inCluster = false;
    
    keysDepressed = Array<int>();
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

void SynchronicProcessor::keyPressed(int noteNumber, float velocity)
{
    SynchronicPreparation::Ptr prep = synchronic->aPrep;
    
    lastKeyPressed = noteNumber;
    lastKeyVelocity = velocity;
    
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    velocities.set(noteNumber, velocity);
    holdTimers.set(noteNumber, 0);
    
    if (!velocityCheck(noteNumber)) return;
    
    // Remove old clusters, deal with layers and NoteOffSync modes
    for (int i = clusters.size(); --i >= 0; )
    {
        if (!clusters[i]->getShouldPlay() && !inCluster)
        {
            clusters.remove(i);
            continue;
        }
        
        if(   (synchronic->aPrep->getMode() == LastNoteOffSync)
           || (synchronic->aPrep->getMode() == AnyNoteOffSync))
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
    
    //cluster management
    /*
    if (prep->getOnOffMode() == KeyOn)
    {
        SynchronicCluster::Ptr cluster = clusters.getLast();
        
        if(!inCluster) //we have a new cluster
        {
            // [OLD]
            //reset phasor
            // phasor = 0;
            //clear cluster
            // cluster.clearQuick();
            
            cluster = new SynchronicCluster(prep);
            clusters.add(cluster);
            
            //reset parameter counters; need to account for skipBeats
            cluster->resetPhase();
            
            //now we are in a cluster!
            inCluster = true;
        }
        else if (prep->getMode() == AnyNoteOnSync)
        {
            // might be able to move this whole else block ^ out of if else, since we are in cluster
            //reset phasor if in AnyNoteOnSync
            if (cluster != nullptr)
            {
                cluster->setPhasor(0);
                cluster->resetPhase();
                cluster->setShouldPlay(true);
            }
        }
        else if (prep->getMode() == FirstNoteOnSync)
        {
            if (cluster != nullptr)
            {
                cluster->setShouldPlay(true);
            }
        }
     */
    if (prep->getOnOffMode() == KeyOn)
    {
        SynchronicCluster::Ptr cluster = clusters.getLast();
        
        if (cluster == nullptr)
        {
            cluster = new SynchronicCluster(prep);
            clusters.add(cluster);
        }
        
        if(!inCluster) //we have a new cluster
        {
            // [OLD]
            //reset phasor
            // phasor = 0;
            //clear cluster
            //cluster.clearQuick();
            if (clusters.size() >= synchronic->aPrep->getNumClusters())
            {
                clusters.remove(0); // remove first (oldest) cluster
            }
            
            cluster = new SynchronicCluster(prep);
            //clusters.add(cluster);
            
            //clusters.clearQuick();
            clusters.add(cluster);
            
            //reset parameter counters; need to account for skipBeats
            cluster->resetPhase();
            
            //now we are in a cluster!
            inCluster = true;
        }
        else if (prep->getMode() == AnyNoteOnSync)
        {
            // might be able to move this whole else block ^ out of if else, since we are in cluster
            //reset phasor if in AnyNoteOnSync
            //if (cluster != nullptr)
            {
                cluster->setPhasor(0);
                cluster->resetPhase();
                cluster->setShouldPlay(true);
            }
        }
        else if (prep->getMode() == FirstNoteOnSync)
        {
            //if (cluster != nullptr)
            {
                cluster->setShouldPlay(true);
            }
        }
        
        //at this point, we are in cluster one way or another!
        
        //save note in the cluster, even if it's already there. then cap the cluster to a hard cap (8 for now)
        //this is different than avoiding dupes at this stage (with "addIfNotAlreadyThere") because it allows
        //repeated notes to push older notes out the back.
        //later, we remove dupes so we don't inadvertently play the same note twice in a pulse
        cluster->addNote(noteNumber);
        
        //reset the timer for time between notes
        clusterThresholdTimer = 0;
    }
}


void SynchronicProcessor::keyReleased(int noteNumber, float velocity, int channel)
{
    SynchronicPreparation::Ptr prep = synchronic->aPrep;
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    SynchronicCluster::Ptr cluster = clusters.getLast();
    
    if (!velocityCheck(noteNumber)) return;
    if (!holdCheck(noteNumber)) return;
    
    if (cluster == nullptr)
    {
        cluster = new SynchronicCluster(prep);
        clusters.add(cluster);
    }
    
    //cluster management
    if (synchronic->aPrep->getOnOffMode() == KeyOff)
    {
        if(!inCluster) //we have a new cluster
        {
            if (clusters.size() >= synchronic->aPrep->getNumClusters())
            {
                clusters.remove(0); // remove first (oldest) cluster
            }
            
            cluster = new SynchronicCluster(synchronic->aPrep);
            //clusters.clearQuick();
            clusters.add(cluster);
            
            //reset parameter counters; need to account for skipBeats
            cluster->resetPhase();
            cluster->setShouldPlay(true);
            
            //now we are in a cluster!
            inCluster = true;
        }
        else if (synchronic->aPrep->getMode() == AnyNoteOffSync)
        {
            // might be able to move this whole else block ^ out of if else, since we are in cluster
            //reset phasor if in AnyNoteOnSync
            //if (cluster != nullptr)
            {
                cluster->setPhasor(0);
                cluster->resetPhase();
                cluster->setShouldPlay(true);
            }
        }

        
        //at this point, we are in cluster one way or another!
        
        //save note in the cluster, even if it's already there. then cap the cluster to a hard cap (8 for now)
        //this is different than avoiding dupes at this stage (with "addIfNotAlreadyThere") because it allows
        //repeated notes to push older notes out the back.
        //later, we remove dupes so we don't inadvertently play the same note twice in a pulse
        cluster->addNote(noteNumber);
        DBG("cluster: " + intArrayToString(cluster->getCluster()));
        
        //reset the timer for time between notes
        clusterThresholdTimer = 0;
    }
    
    // If AnyNoteOffSync mode, reset phasor and multiplier indices.
    //only initiate pulses if ALL keys are released
    if ((synchronic->aPrep->getMode() == LastNoteOffSync && keysDepressed.size() == 0) ||
		(synchronic->aPrep->getMode() == AnyNoteOffSync))
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
                
                inCluster = true;
                
            }
        }
        
        /*
        if (cluster != nullptr)
        {
            cluster->resetPhase();
            
            //start right away
            uint64 phasor = beatThresholdSamples *
                            synchronic->aPrep->getBeatMultipliers()[cluster->getBeatMultiplierCounter()] *
                            general->getPeriodMultiplier() * tempo->getPeriodMultiplier();
            
            cluster->setPhasor(phasor);
            
            cluster->setShouldPlay(true);
            
            inCluster = true;
        }
         */
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
    clusterThresholdSamples = (prep->getClusterThreshSEC() * sampleRate);

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
        if (clusterThresholdTimer >= clusterThresholdSamples)
        {
            inCluster = false;
        }
        //otherwise incrument cluster timer
        else
        {
            clusterThresholdTimer += numSamples;
        }
    }
    
    
    for (int i = clusters.size(); --i >= 0;)
    {
        SynchronicCluster::Ptr cluster = clusters.getUnchecked(i);
    
        if (cluster->getShouldPlay()) // check if pulses are done and remove cluster if so
        {
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
                cluster->step(numSamplesBeat);
                
            }
            
            //pass time until next beat
            cluster->incrementPhasor(numSamples);
        }
    }
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

			ScopedPointer<XmlElement> xml = vt1.createXml();

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

			ScopedPointer<XmlElement> xml = vt1.createXml();

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

			ScopedPointer<XmlElement> xml = vt1.createXml();

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
