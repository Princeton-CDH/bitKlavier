/*
  ==============================================================================

    Synchronic.h
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine and Dan Trueman

  ==============================================================================
*/

#ifndef SYNCHRONIC_H_INCLUDED
#define SYNCHRONIC_H_INCLUDED

#include "BKUtilities.h"
#include "BKSynthesiser.h"
#include "Tuning.h"
#include "Tempo.h"
#include "General.h"
#include "Keymap.h"
#include "Blendronic.h"


/*
SynchronicPreparation holds all the state variable values for the
Synchronic preparation. As with other preparation types, bK will use
two instantiations of SynchronicPreparation for every active
Synchronic in the gallery, one to store the static state of the
preparation, and the other to store the active state. These will
be the same, unless a Modification is triggered, in which case the
active state will be changed (and a Reset will revert the active state
to the static state).
*/

class SynchronicPreparation : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicPreparation>   Ptr;
    typedef Array<SynchronicPreparation::Ptr>                  PtrArr;
    typedef Array<SynchronicPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicPreparation>                  Arr;
    typedef OwnedArray<SynchronicPreparation, CriticalSection> CSArr;
    
    // Copy Constructor
    SynchronicPreparation(SynchronicPreparation::Ptr p):
    sNumBeats(p->getNumBeats()),
    sClusterMin(p->getClusterMin()),
    sClusterMax(p->getClusterMax()),
    holdMin(p->getHoldMin()),
    holdMax(p->getHoldMax()),
    velocityMin(p->getVelocityMin()),
    velocityMax(p->getVelocityMax()),
    sClusterCap(p->getClusterCap()),
    sMode(p->getMode()),
    sBeatsToSkip(p->getBeatsToSkip()),
    sBeatMultipliers(p->getBeatMultipliers()),
    sAccentMultipliers(p->getAccentMultipliers()),
    sLengthMultipliers(p->getLengthMultipliers()),
    sTransposition(p->getTransposition()),
    sAttacks(p->getAttacks()),
    sDecays(p->getDecays()),
    sSustains(p->getSustains()),
    sReleases(p->getReleases()),
    envelopeOn(p->getEnvelopesOn()),
    sGain(p->getGain()),
    sClusterThresh(p->getClusterThreshMS()),
    sClusterThreshSec(p->getClusterThreshSEC()),
    sReleaseVelocitySetsSynchronic(p->getReleaseVelocitySetsSynchronic()),
    numClusters(1),
    onOffMode(KeyOn),
    targetTypeSynchronicSync(p->getTargetTypeSynchronicSync()),
    targetTypeSynchronicPatternSync(p->getTargetTypeSynchronicPatternSync()),
    targetTypeSynchronicAddNotes(p->getTargetTypeSynchronicAddNotes()),
    targetTypeSynchronicPausePlay(p->getTargetTypeSynchronicPausePlay()),
    midiOutput(p->getMidiOutput())
    {
        
    }
    
                        
    SynchronicPreparation(int numBeats,
                          int clusterMin,
                          int clusterMax,
                          float clusterThresh,
                          SynchronicSyncMode mode,
                          bool velocityMode,
                          int beatsToSkip,
                          Array<float> beatMultipliers,
                          Array<float> accentMultipliers,
                          Array<float> lengthMultipliers,
                          Array<Array<float>> transp):
    sNumBeats(numBeats),
    sClusterMin(clusterMin),
    sClusterMax(clusterMax),
    sClusterCap(8),
    sMode(mode),
    sBeatsToSkip(beatsToSkip),
    sBeatMultipliers(beatMultipliers),
    sAccentMultipliers(accentMultipliers),
    sLengthMultipliers(lengthMultipliers),
    sTransposition(transp),
    sClusterThresh(clusterThresh),
    sClusterThreshSec(.001 * sClusterThresh),
    sReleaseVelocitySetsSynchronic(velocityMode),
    holdMin(0),
    holdMax(12000),
    velocityMin(0),
    velocityMax(127),
    numClusters(1),
    onOffMode(KeyOn),
    targetTypeSynchronicSync(NoteOn),
    targetTypeSynchronicPatternSync(NoteOn),
    targetTypeSynchronicAddNotes(NoteOn),
    targetTypeSynchronicPausePlay(NoteOn),
    midiOutput(nullptr)
    {
    }

    
    SynchronicPreparation(void):
    sNumBeats(20),
    sClusterMin(1),
    sClusterMax(12),
    sClusterCap(8), //8 in original bK, but behavior is different here?
    sMode(FirstNoteOnSync),
    sBeatsToSkip(0),
    sBeatMultipliers(Array<float>({1.0})),
    sAccentMultipliers(Array<float>({1.0})),
    sLengthMultipliers(Array<float>({1.0})),
    sAttacks(Array<int>({3,3,3,3,3,3,3,3,3,3,3,3})),
    sDecays(Array<int>({3,3,3,3,3,3,3,3,3,3,3,3})),
    sSustains(Array<float>({1.,1,1,1,1,1,1,1,1,1,1,1})),
    sReleases(Array<int>({30,30,30,30,30,30,30,30,30,30,30,30})),
    envelopeOn(Array<bool>({true,false,false,false,false,false,false,false,false,false,false,false})),
    sGain(1.0),
    sClusterThresh(500),
    sClusterThreshSec(.001 * sClusterThresh),
    holdMin(0),
    holdMax(12000),
    velocityMin(0),
    velocityMax(127),
    numClusters(1),
    onOffMode(KeyOn),
    targetTypeSynchronicSync(NoteOn),
    targetTypeSynchronicPatternSync(NoteOn),
    targetTypeSynchronicAddNotes(NoteOn),
    targetTypeSynchronicPausePlay(NoteOn),
    midiOutput(nullptr)
    {
        sTransposition.ensureStorageAllocated(1);
        sTransposition.add(Array<float>({0.0}));
    }
    
    inline void copy(SynchronicPreparation::Ptr s)
    {
        sNumBeats = s->getNumBeats();
        sClusterMin = s->getClusterMin();
        sClusterMax = s->getClusterMax();
        sClusterCap = s->getClusterCap();
        sMode = s->getMode();
        sBeatsToSkip = s->getBeatsToSkip();
        sBeatMultipliers = s->getBeatMultipliers();
        sAccentMultipliers = s->getAccentMultipliers();
        sLengthMultipliers = s->getLengthMultipliers();
        sGain = s->getGain();
        sTransposition = s->getTransposition();
        sClusterThresh = s->getClusterThreshMS();
        sClusterThreshSec = s->getClusterThreshSEC();
        sReleaseVelocitySetsSynchronic = s->getReleaseVelocitySetsSynchronic();
        
        sAttacks = s->getAttacks();
        sDecays = s->getDecays();
        sSustains = s->getSustains();
        sReleases = s->getReleases();
        
        envelopeOn = s->getEnvelopesOn();
        
        numClusters = s->getNumClusters();
        onOffMode = s->getOnOffMode();
        
        holdMin = s->getHoldMin();
        holdMax = s->getHoldMax();
        
        velocityMin = s->getVelocityMin();
        velocityMax = s->getVelocityMax();
        
        targetTypeSynchronicSync = s->getTargetTypeSynchronicSync();
        targetTypeSynchronicPatternSync = s->getTargetTypeSynchronicPatternSync();
        targetTypeSynchronicAddNotes = s->getTargetTypeSynchronicAddNotes();
        targetTypeSynchronicPausePlay = s->getTargetTypeSynchronicPausePlay();
        
        midiOutput = s->getMidiOutput();
    }
    
    inline void performModification(SynchronicPreparation::Ptr s, Array<bool> dirty)
    {
        if (dirty[SynchronicNumPulses]) sNumBeats = s->getNumBeats();
        if (dirty[SynchronicClusterMin]) sClusterMin = s->getClusterMin();
        if (dirty[SynchronicClusterMax]) sClusterMax = s->getClusterMax();
        if (dirty[SynchronicClusterCap]) sClusterCap = s->getClusterCap();
        if (dirty[SynchronicMode]) sMode = s->getMode();
        if (dirty[SynchronicBeatsToSkip]) sBeatsToSkip = s->getBeatsToSkip();
        if (dirty[SynchronicBeatMultipliers]) sBeatMultipliers = s->getBeatMultipliers();
        if (dirty[SynchronicAccentMultipliers]) sAccentMultipliers = s->getAccentMultipliers();
        if (dirty[SynchronicLengthMultipliers]) sLengthMultipliers = s->getLengthMultipliers();
        if (dirty[SynchronicGain]) sGain = s->getGain();
        if (dirty[SynchronicTranspOffsets]) sTransposition = s->getTransposition();
        if (dirty[SynchronicClusterThresh])
        {
            sClusterThresh = s->getClusterThreshMS();
            sClusterThreshSec = s->getClusterThreshSEC();
        }

        if (dirty[SynchronicADSRs])
        {
            sAttacks = s->getAttacks();
            sDecays = s->getDecays();
            sSustains = s->getSustains();
            sReleases = s->getReleases();
            
            envelopeOn = s->getEnvelopesOn();
        }
        
        if (dirty[SynchronicNumClusters]) numClusters = s->getNumClusters();
        if (dirty[SynchronicOnOff]) onOffMode = s->getOnOffMode();
        
        if (dirty[SynchronicHoldMin]) holdMin = s->getHoldMin();
        if (dirty[SynchronicHoldMax]) holdMax = s->getHoldMax();
        
        if (dirty[SynchronicVelocityMin]) velocityMin = s->getVelocityMin();
        if (dirty[SynchronicVelocityMax]) velocityMax = s->getVelocityMax();
        
        if (dirty[SynchronicMidiOutput]) midiOutput = s->getMidiOutput();
    }
    
    bool compare(SynchronicPreparation::Ptr s)
    {
        bool lens = true;
        bool accents = true;
        bool beats = true;
        bool transp = true;
        bool attack = true;
        bool decay = true;
        bool sustain = true;
        bool release = true;
        bool envelope = true;
        
        for (int i = s->getLengthMultipliers().size(); --i>=0;)
        {
            if (s->getLengthMultipliers()[i] != sLengthMultipliers[i])
            {
                lens = false;
                break;
                
            }
        }
        
        for (int i = s->getAccentMultipliers().size(); --i>=0;)
        {
            if (s->getAccentMultipliers()[i] != sAccentMultipliers[i])
            {
                accents = false;
                break;
                
            }
        }
        
        for (int i = s->getBeatMultipliers().size(); --i>=0;)
        {
            if (s->getBeatMultipliers()[i] != sBeatMultipliers[i])
            {
                beats = false;
                break;
                
            }
        }
        
        for (int i  = s->getTransposition().size(); --i >= 0;)
        {
            Array<float> transposition = s->getTransposition()[i];
            for (int j = transposition.size(); --j >= 0;)
            {
                if (transposition[j] != sTransposition[i][j])
                {
                    transp = false;
                    break;
                    
                }
            }
        }
        
        for (int i = s->getAttacks().size(); --i>=0;)
        {
            if (s->getAttacks()[i] != sAttacks[i])
            {
                attack = false;
                break;
            }
        }
        
        for (int i = s->getDecays().size(); --i>=0;)
        {
            if (s->getDecays()[i] != sDecays[i])
            {
                decay = false;
                break;
            }
        }
        
        for (int i = s->getSustains().size(); --i>=0;)
        {
            if (s->getSustains()[i] != sSustains[i])
            {
                sustain = false;
                break;
            }
        }
        
        for (int i = s->getReleases().size(); --i>=0;)
        {
            if (s->getReleases()[i] != sReleases[i])
            {
                release = false;
                break;
            }
        }
        
        for (int i = s->getEnvelopesOn().size(); --i>=0;)
        {
            if (s->getEnvelopesOn()[i] != envelopeOn[i])
            {
                envelope = false;
                break;
            }
        }
        
        return (sNumBeats == s->getNumBeats() &&
                sClusterMin == s->getClusterMin() &&
                sClusterMax == s->getClusterMax() &&
                sClusterCap == s->getClusterCap() &&
                (sMode == s->getMode()) &&
                transp && lens && accents && beats && attack && decay && sustain && release &&
                sGain == s->getGain() &&
                sClusterThresh == s->getClusterThreshMS() &&
                sClusterThreshSec == s->getClusterThreshSEC() &&
                sReleaseVelocitySetsSynchronic == s->getReleaseVelocitySetsSynchronic() &&
                numClusters == s->getNumClusters() &&
                onOffMode == s->getOnOffMode() &&
                holdMin == s->getHoldMin() &&
                holdMax == s->getHoldMax() &&
                velocityMin == s->getVelocityMin() &&
                velocityMax == s->getVelocityMax() &&
                targetTypeSynchronicSync == s->getTargetTypeSynchronicSync() &&
                targetTypeSynchronicPatternSync == s->getTargetTypeSynchronicPatternSync() &&
                targetTypeSynchronicAddNotes == s->getTargetTypeSynchronicAddNotes() &&
                targetTypeSynchronicPausePlay == s->getTargetTypeSynchronicPausePlay());
                
    }

    // for unit-testing
	inline void randomize()
	{
		Random::getSystemRandom().setSeedRandomly();

		float r[100];

		for (int i = 0; i < 100; i++)  r[i] = (Random::getSystemRandom().nextFloat());
		int idx = 0;

		sNumBeats = (int)(r[idx++] * 100);
		sClusterMin = (int)(r[idx++] * 20);
		sClusterMax = (int)(r[idx++] * 20);
		sClusterCap = (int)(r[idx++] * 20);
		sMode = (SynchronicSyncMode)(int)(r[idx++] * SynchronicSyncModeNil);
		sBeatsToSkip = (int)(r[idx++] * 2);
        
        
        numClusters = (int)(r[idx++] * 20) + 1;
        onOffMode = (r[idx++] < 0.5) ? KeyOn : KeyOff;
        holdMin = (int)(r[idx++] * 12000);
        holdMax = (int)(r[idx++] * 12000);
        velocityMin = (int)(r[idx++] * 127);
        velocityMax = (int)(r[idx++] * 127);
        
		sBeatMultipliers.clear();
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			sBeatMultipliers.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
		}
		sAccentMultipliers.clear();
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			sAccentMultipliers.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
		}
		sLengthMultipliers.clear();
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			sLengthMultipliers.add(i, (Random::getSystemRandom().nextFloat() * 4.0f - 2.0f));
		}
		sGain = r[idx++] * 10;
		sTransposition.clear();
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			Array<float> transposition;
			for (int j = 0; j < Random::getSystemRandom().nextInt(10); j++)
			{
				transposition.add(i, (Random::getSystemRandom().nextFloat()) * 48.0f - 24.0f);
			}
			sTransposition.add(transposition);
		}
		sClusterThresh = (r[idx++] * 2000) + 1;
		sClusterThreshSec = sClusterThresh * 0.001f;
		sReleaseVelocitySetsSynchronic = (bool)((int)(r[idx++] * 2));

		int numEnvelopes = 12;

		sAttacks.clear();
		for (int i = 0; i < numEnvelopes; ++i)
		{
			sAttacks.add(i, Random::getSystemRandom().nextInt(Range<int>(1, 1000)));
		}
		sDecays.clear();
		for (int i = 0; i < numEnvelopes; ++i)
		{
			sDecays.add(i, Random::getSystemRandom().nextInt(Range<int>(1, 1000)));
		}
		sSustains.clear();
		for (int i = 0; i < numEnvelopes; ++i)
		{
			sSustains.add(i, (Random::getSystemRandom().nextFloat()));
		}
		sReleases.clear();
		for (int i = 0; i < numEnvelopes; ++i)
		{
			sReleases.add(i, Random::getSystemRandom().nextInt(Range<int>(1, 1000)));
		}

		envelopeOn.clear();
		envelopeOn.add(true); // needed for accurate unit testing - set state always defaults first to be true so there is 1 ADSR
		for (int i = 1; i < numEnvelopes; ++i)
		{
			envelopeOn.add(i, Random::getSystemRandom().nextBool());
		}
		
	}
    
    //inline const float getTempo() const noexcept                       {return sTempo;               }
    inline const int getNumBeats() const noexcept                      {return sNumBeats;              }
    inline const int getClusterMin() const noexcept                    {return sClusterMin;            }
    inline const int getClusterMax() const noexcept                    {return sClusterMax;            }
    inline const int getClusterCap() const noexcept                    {return sClusterCap;            }
    inline const float getClusterThreshSEC() const noexcept            {return sClusterThreshSec;      }
    inline const float getClusterThreshMS() const noexcept             {return sClusterThresh;         }
    inline const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    inline const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    inline const int getBeatsToSkip() const noexcept                   {return sBeatsToSkip;           }
    inline const int getOffsetParamToggle() const noexcept
    {
        if(getMode() == FirstNoteOnSync || getMode() == AnyNoteOnSync) return getBeatsToSkip() + 1;
        else return getBeatsToSkip();
    }
    
    inline const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    inline const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    inline const Array<Array<float>> getTransposition() const noexcept {return sTransposition;         }
    inline const bool getReleaseVelocitySetsSynchronic() const noexcept{return sReleaseVelocitySetsSynchronic; }
    inline const float getGain() const noexcept                        {return sGain;                   }
    
    inline const Array<int> getAttacks() const noexcept         {return sAttacks;   }
    inline const Array<int> getDecays() const noexcept          {return sDecays;    }
    inline const Array<float> getSustains() const noexcept      {return sSustains;  }
    inline const Array<int> getReleases() const noexcept        {return sReleases;  }
    inline const Array<bool> getEnvelopesOn() const noexcept    {return envelopeOn; }
    
    inline const int getAttack(int which) const noexcept    {return sAttacks[which];}
    inline const int getDecay(int which) const noexcept     {return sDecays[which];}
    inline const float getSustain(int which) const noexcept {return sSustains[which];}
    inline const int getRelease(int which) const noexcept   {return sReleases[which];}
    
    inline const Array<Array<float>> getADSRs() const noexcept
    {
        Array<Array<float>> allADSRs;
        for(int i=0; i<sAttacks.size(); i++)
        {
            Array<float> singleADSR;
            singleADSR.insert(0, sAttacks[i]);
            singleADSR.insert(1, sDecays[i]);
            singleADSR.insert(2, sSustains[i]);
            singleADSR.insert(3, sReleases[i]);
            if(envelopeOn[i])singleADSR.insert(4, 1);
            else singleADSR.insert(4, 0);
            
            allADSRs.insert(i, singleADSR);
        }
        
        return allADSRs;
    }
    
    inline const bool getEnvelopeOn(int which) const noexcept   {return envelopeOn[which];}
    
    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = sClusterThresh * .001;
    }
    
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline void setNumBeats(int numBeats)                              {sNumBeats = numBeats;                              }
    inline void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    inline void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    inline void setClusterCap(int clusterCap)                          {sClusterCap = clusterCap;                          }
    inline void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    inline void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    inline void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    inline void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    inline void setTransposition(Array<Array<float>> transp)           {sTransposition.swapWith(transp);                   }
    inline void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    
    inline void setBeatMultiplier(int whichSlider, float value)        {sBeatMultipliers.set(whichSlider, value);           }
    inline void setAccentMultiplier(int whichSlider, float value)      {sAccentMultipliers.set(whichSlider, value);         }
    inline void setLengthMultiplier(int whichSlider, float value)      {sLengthMultipliers.set(whichSlider, value);         }
    inline void setSingleTransposition(int whichSlider, Array<float> values) {sTransposition.set(whichSlider, values); }
    inline void setReleaseVelocitySetsSynchronic(bool rvss)            {sReleaseVelocitySetsSynchronic = rvss;          }
    inline void setGain(float gain)                                    {sGain = gain;                          }
    
    inline void setAttacks(Array<int> attacks)      {sAttacks.swapWith(attacks);    }
    inline void setDecays(Array<int> decays)        {sDecays.swapWith(decays);      }
    inline void setSustains(Array<float> sustains)  {sSustains.swapWith(sustains);  }
    inline void setReleases(Array<int> releases)    {sReleases.swapWith(releases);  }
    
    inline void setAttack(int which, int val)       {sAttacks.set(which, val);}
    inline void setDecay(int which, int val)        {sDecays.set(which, val);}
    inline void setSustain(int which, float val)    {sSustains.set(which, val);}
    inline void setRelease(int which, int val)      {sReleases.set(which, val);}
    
    inline const int getHoldMin() const noexcept { return holdMin; }
    inline const int getHoldMax() const noexcept { return holdMax; }
    
    inline const void setHoldMin(int min)  { holdMin = min; }
    inline const void setHoldMax(int max)  { holdMax = max; }
    
    inline const int getVelocityMin() const noexcept { return velocityMin; }
    inline const int getVelocityMax() const noexcept { return velocityMax; }
    
    inline const TargetNoteMode getTargetTypeSynchronicSync() const noexcept { return targetTypeSynchronicSync; }
    inline const TargetNoteMode getTargetTypeSynchronicPatternSync() const noexcept { return targetTypeSynchronicPatternSync; }
    inline const TargetNoteMode getTargetTypeSynchronicAddNotes() const noexcept { return targetTypeSynchronicAddNotes; }
    inline const TargetNoteMode getTargetTypeSynchronicPausePlay() const noexcept { return targetTypeSynchronicPausePlay; }
    inline const TargetNoteMode getTargetTypeSynchronic(KeymapTargetType which)
    {
        if (which == TargetTypeSynchronicSync)          return targetTypeSynchronicSync;
        if (which == TargetTypeSynchronicPatternSync)   return targetTypeSynchronicPatternSync;
        if (which == TargetTypeSynchronicAddNotes)      return targetTypeSynchronicAddNotes;
        if (which == TargetTypeSynchronicPausePlay)     return targetTypeSynchronicPausePlay;
        
    }
    
    inline void setTargetTypeSynchronicSync(TargetNoteMode nm)          { targetTypeSynchronicSync = nm; }
    inline void setTargetTypeSynchronicPatternSync(TargetNoteMode nm)   { targetTypeSynchronicPatternSync = nm; }
    inline void setTargetTypeSynchronicAddNotes(TargetNoteMode nm)      { targetTypeSynchronicAddNotes = nm; }
    inline void setTargetTypeSynchronicPausePlay(TargetNoteMode nm)     { targetTypeSynchronicPausePlay = nm; }
    
    inline const void setVelocityMin(int min)
    {
        velocityMin = min;
    }
    
    inline const void setVelocityMax(int max)
    {
        velocityMax = max;
    }
    
	inline void clearADSRs()
	{
		sAttacks.clear();
		sDecays.clear();
		sSustains.clear();
		sReleases.clear();
		envelopeOn.clear();
	}
    
    inline void setOnOffMode(SynchronicOnOffMode oo)
    {
        onOffMode = oo;
    }
    
    inline SynchronicOnOffMode getOnOffMode(void)
    {
        return onOffMode;
    }
    
    inline void setNumClusters(int c)
    {
        numClusters = c;
        DBG("setNumClusters = " + String(c));
    }
    
    inline int getNumClusters(void)
    {
        return numClusters;
    }

    inline void setADSRs(Array<Array<float>> allADSRs)
    {
		clearADSRs();
        for(int i=0; i<allADSRs.size(); i++)
        {
            setAttack(i, allADSRs[i][0]);
            setDecay(i, allADSRs[i][1]);
            setSustain(i, allADSRs[i][2]);
            setRelease(i, allADSRs[i][3]);
            if(allADSRs[i][4] > 0 || i==0) setEnvelopeOn(i, true);
            else setEnvelopeOn(i, false);
            //DBG("ADSR envelopeOn = " + String(i) + " " + String((int)getEnvelopeOn(i)));
        }
    }
    
    inline void setADSR(int which, Array<float> oneADSR)
    {
        setAttack(which, oneADSR[0]);
        setDecay(which, oneADSR[1]);
        setSustain(which, oneADSR[2]);
        setRelease(which, oneADSR[3]);
        if(oneADSR[4] > 0 || which==0) setEnvelopeOn(which, true);
        else setEnvelopeOn(which, false);
        DBG("ADSR envelopeOn = " + String(which) + " " + String((int)getEnvelopeOn(which)));
        
    }
    
    inline void setEnvelopeOn(int which, bool val)  {envelopeOn.set(which, val);}
    
    inline const std::shared_ptr<MidiOutput> getMidiOutput() const noexcept { return midiOutput; }
    inline const void setMidiOutput(MidiDeviceInfo device)
    {
        midiOutput = MidiOutput::openDevice(device.identifier);
        if (!midiOutput) midiOutput = nullptr;
    }
    inline const void setMidiOutput(std::shared_ptr<MidiOutput> output)
    {
        midiOutput = output;
    }
    
    void print(void)
    {
        DBG("| - - - Synchronic Preparation - - - |");
        DBG("sTempo: " + String(sTempo));
        DBG("sNumBeats: " + String(sNumBeats));
        DBG("sClusterMin: " + String(sClusterMin));
        DBG("sClusterMax: " + String(sClusterMax));
        DBG("sClusterCap: " + String(sClusterCap));
        DBG("sClusterThresh: " + String(sClusterThresh));
        DBG("sReleaseVelocitySetsSynchronic: " + String((int)sReleaseVelocitySetsSynchronic));
        DBG("sMode: " + String(sMode));
        DBG("sBeatsToSkip: " + String(sBeatsToSkip));
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers));
        
        String s = "";
        for (auto arr : sTransposition) s += "{ " + floatArrayToString(arr) + " },\n";
        DBG("sTransposition: " + s);
        
        DBG("sClusterThreshSec: " + String(sClusterThreshSec));
        //DBG("resetKeymap: " + intArrayToString(getResetMap()->keys()));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
    ValueTree getState(void)
    {
        ValueTree prep("params");
        
        prep.setProperty( "gain", getGain(), 0);
        prep.setProperty( ptagSynchronic_numBeats,            getNumBeats(), 0);
        prep.setProperty( ptagSynchronic_clusterMin,          getClusterMin(), 0);
        prep.setProperty( ptagSynchronic_clusterMax,          getClusterMax(), 0);
        prep.setProperty( ptagSynchronic_clusterCap,          getClusterCap(), 0);
        prep.setProperty( ptagSynchronic_clusterThresh,       getClusterThreshMS(), 0);
        prep.setProperty( ptagSynchronic_mode,                getMode(), 0);
        prep.setProperty( ptagSynchronic_beatsToSkip,         getBeatsToSkip(), 0);
        
        prep.setProperty( "numClusters", getNumClusters(), 0);
        prep.setProperty( "onOffMode", getOnOffMode(), 0);
        
        prep.setProperty( "holdMin", getHoldMin(), 0);
        prep.setProperty( "holdMax", getHoldMax(), 0);
        
        prep.setProperty( "velocityMin", getVelocityMin(), 0);
        prep.setProperty( "velocityMax", getVelocityMax(), 0);


        prep.setProperty( "targetTypeSynchronicSync", getTargetTypeSynchronicSync(), 0);
        prep.setProperty( "targetTypeSynchronicPatternSync", getTargetTypeSynchronicPatternSync(), 0);
        prep.setProperty( "targetTypeSynchronicAddNotes", getTargetTypeSynchronicAddNotes(), 0);
        prep.setProperty( "targetTypeSynchronicPausePlay", getTargetTypeSynchronicPausePlay(), 0);

                 
        ValueTree beatMults( vtagSynchronic_beatMults);
        int count = 0;
        for (auto f : getBeatMultipliers())
        {
            beatMults.      setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(beatMults, -1, 0);
        
        
        ValueTree lenMults( vtagSynchronic_lengthMults);
        count = 0;
        for (auto f : getLengthMultipliers())
        {
            lenMults.       setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(lenMults, -1, 0);
        
        
        ValueTree accentMults( vtagSynchronic_accentMults);
        count = 0;
        for (auto f : getAccentMultipliers())
        {
            accentMults.    setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(accentMults, -1, 0);
        
        
        ValueTree transposition( vtagSynchronic_transpOffsets);
        
        int tcount = 0;
        for (auto arr : getTransposition())
        {
            ValueTree t("t"+String(tcount++));
            count = 0;
            for (auto f : arr)  t.setProperty( ptagFloat + String(count++), f, 0);
            transposition.addChild(t,-1,0);
        }
        prep.addChild(transposition, -1, 0);
        
        ValueTree ADSRs( vtagSynchronic_ADSRs);
        
        tcount = 0;
        for (auto arr : getADSRs())
        {
            ValueTree e("e"+String(tcount++));
            count = 0;
            for (auto f : arr)  e.setProperty( ptagFloat + String(count++), f, 0);
            ADSRs.addChild(e,-1,0);
        }
        prep.addChild(ADSRs, -1, 0);
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        String n; int i; float f; bool b;
        
        n = e->getStringAttribute("gain");
        if (n != "") setGain(n.getFloatValue());
        else         setGain(1.0);
        
        i = e->getStringAttribute(ptagSynchronic_numBeats).getIntValue();
        setNumBeats(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterMin).getIntValue();
        setClusterMin(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterMax).getIntValue();
        setClusterMax(i);
        
        //i = e->getStringAttribute(ptagSynchronic_clusterCap).getIntValue();
        n = e->getStringAttribute(ptagSynchronic_clusterCap);
        if (n != "")    setClusterCap(n.getIntValue());
        else            setClusterCap(8);
        
        n = e->getStringAttribute("holdMin");
        
        if (n != "")    setHoldMin(n.getIntValue());
        else            setHoldMin(0);
        
        n = e->getStringAttribute("holdMax");
        
        if (n != "")    setHoldMax(n.getIntValue());
        else            setHoldMax(12000);
        
        n = e->getStringAttribute("velocityMin");
        
        if (n != "")    setVelocityMin(n.getIntValue());
        else            setVelocityMin(0);
        
        n = e->getStringAttribute("velocityMax");
        
        if (n != "")    setVelocityMax(n.getIntValue());
        else            setVelocityMax(127);
        
        f = e->getStringAttribute(ptagSynchronic_clusterThresh).getFloatValue();
        setClusterThresh(f);
        
        i = e->getStringAttribute(ptagSynchronic_mode).getIntValue();
        setMode((SynchronicSyncMode) i);
        
        i = e->getStringAttribute(ptagSynchronic_beatsToSkip).getIntValue();
        setBeatsToSkip(i);
        
        n = e->getStringAttribute("numClusters");
        if (n != String())     setNumClusters(n.getIntValue());
        else                   setNumClusters(1);
        
        n = e->getStringAttribute("onOffMode");
        if (n != String())     setOnOffMode((SynchronicOnOffMode) n.getIntValue());
        else                   setOnOffMode(KeyOn);

        i = e->getStringAttribute("targetTypeSynchronicSync").getIntValue();
        setTargetTypeSynchronicSync((TargetNoteMode)i);
        
        i = e->getStringAttribute("targetTypeSynchronicPatternSync").getIntValue();
        setTargetTypeSynchronicPatternSync((TargetNoteMode)i);
        
        i = e->getStringAttribute("targetTypeSynchronicAddNotes").getIntValue();
        setTargetTypeSynchronicAddNotes((TargetNoteMode)i);
        
        i = e->getStringAttribute("targetTypeSynchronicPausePlay").getIntValue();
        setTargetTypeSynchronicPausePlay((TargetNoteMode)i);
 
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagSynchronic_beatMults))
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
                
                setBeatMultipliers(beats);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_accentMults))
            {
                Array<float> accents;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        accents.add(f);
                    }
                }
                
                setAccentMultipliers(accents);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_lengthMults))
            {
                Array<float> lens;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        lens.add(f);
                    }
                }
                
                setLengthMultipliers(lens);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
            {
                Array<Array<float>> atransp;
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("t"+String(tcount++)))
                    {
                        Array<float> transp;
                        for (int k = 0; k < 128; k++)
                        {
                            String attr = asub->getStringAttribute(ptagFloat + String(k));
                            
                            if (attr == String()) break;
                            else
                            {
                                f = attr.getFloatValue();
                                transp.add(f);
                            }
                        }
                        atransp.set(tcount-1, transp);
                    }
                }
                
                setTransposition(atransp);
            }
            else  if (sub->hasTagName(vtagSynchronic_ADSRs))
            {
                Array<Array<float>> aADSRs;
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("e"+String(tcount++)))
                    {
                        Array<float> singleADSR;
                        for (int k = 0; k < 5; k++)
                        {
                            String attr = asub->getStringAttribute(ptagFloat + String(k));
                            
                            if (attr == String()) break;
                            else
                            {
                                f = attr.getFloatValue();
                                singleADSR.add(f);
                            }
                        }
                        aADSRs.set(tcount-1, singleADSR);
                    }
                }
                
                setADSRs(aADSRs);
            }
        }
    }
    
private:
    String name;
    float sTempo;
    int sNumBeats,sClusterMin,sClusterMax;
    int sClusterCap = 8; //max in cluster; 8 in original bK. called Cluster Thickness in v2.4 UI
    
    int numClusters;
    
    int holdMin, holdMax;
    int velocityMin, velocityMax;
    
    SynchronicSyncMode sMode;
    int sBeatsToSkip;
    SynchronicOnOffMode onOffMode;
    
    Array<float> sBeatMultipliers;      //multiply pulse lengths by these
    Array<float> sAccentMultipliers;    //multiply velocities by these
    Array<float> sLengthMultipliers;    //multiply note duration by these
    Array<Array<float>> sTransposition; //transpose by these
    
    Array<int> sAttacks;
    Array<int> sDecays;
    Array<float> sSustains;
    Array<int> sReleases;
    Array<bool> envelopeOn;
    
    float sGain;               //gain multiplier
    float sClusterThresh;      //max time between played notes before new cluster is started, in MS
    float sClusterThreshSec;
    
    bool sReleaseVelocitySetsSynchronic;
    
    // stores what kind of note event triggers targeted events (as set in Keymap)
    TargetNoteMode targetTypeSynchronicSync;
    TargetNoteMode targetTypeSynchronicPatternSync;
    TargetNoteMode targetTypeSynchronicAddNotes;
    TargetNoteMode targetTypeSynchronicPausePlay;

    std::shared_ptr<MidiOutput> midiOutput;

    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};

/*
This class owns two SynchronicPreparations: sPrep and aPrep
As with other preparation, sPrep is the static preparation, while
aPrep is the active preparation currently in use. sPrep and aPrep
remain the same unless a Modification is triggered, which will change
aPrep but not sPrep. aPrep will be restored to sPrep when a Reset
is triggered.
*/

class Synchronic : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Synchronic>   Ptr;
    typedef Array<Synchronic::Ptr>                  PtrArr;
    typedef Array<Synchronic::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Synchronic>                  Arr;
    typedef OwnedArray<Synchronic, CriticalSection> CSArr;
    
    Synchronic(SynchronicPreparation::Ptr prep,
               int Id):
    sPrep(new SynchronicPreparation(prep)),
    aPrep(new SynchronicPreparation(sPrep)),
    Id(Id),
    name(String(Id))
    {
        
    }
    
	Synchronic(int Id, bool random = false) :
    Id(Id),
    name(String(Id))
    {
		sPrep = new SynchronicPreparation();
		aPrep = new SynchronicPreparation(sPrep);
		if (random) randomize();
    }
    
    inline Synchronic::Ptr duplicate()
    {
        SynchronicPreparation::Ptr copyPrep = new SynchronicPreparation(sPrep);
        
        Synchronic::Ptr copy = new Synchronic(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    inline void clear(void)
    {
        sPrep       = new SynchronicPreparation();
        aPrep       = new SynchronicPreparation(sPrep);
    }

    inline void copy(Synchronic::Ptr from)
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
        ValueTree prep(vtagSynchronic);
        
        prep.setProperty( "Id",Id, 0);
        prep.setProperty( "name",                          name, 0);
        
        prep.addChild(active ? aPrep->getState() : sPrep->getState(), -1, 0);
        
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
            sPrep->setState(params);
        }
        else
        {
            sPrep->setState(e);
        }
        
        aPrep->copy(sPrep);
    }
    
    ~Synchronic() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    SynchronicPreparation::Ptr      sPrep;
    SynchronicPreparation::Ptr      aPrep;

    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
    }
    
private:
    int Id;
    String name;
    
    
    JUCE_LEAK_DETECTOR(Synchronic)
};


/*
 This class enables layers of Synchronic pulses by
 maintaining a set of counters moving through all the
 primary multi-parameters (accents, transpositions, etc...)
 and by updating timers for each Synchronic independently
 */

class SynchronicCluster : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<SynchronicCluster>   Ptr;
    typedef Array<SynchronicCluster::Ptr>                  PtrArr;
    
    SynchronicCluster(SynchronicPreparation::Ptr prep) :
    prep(prep)
    {
        phasor = 0;
        envelopeCounter = 0;
        shouldPlay = false;
        over = false;
        
//        SynchronicSyncMode mode = prep->getMode();
//        
//        if (mode == AnyNoteOnSync || mode == FirstNoteOnSync)
//        {
//            shouldPlay = true;
//        }
    }
    
    ~SynchronicCluster() {}
    
    inline const uint64 getPhasor(void) const noexcept   { return phasor;            }
    inline const int getBeatMultiplierCounter() const noexcept { return beatMultiplierCounter; }
    inline const int getAccentMultiplierCounter() const noexcept { return accentMultiplierCounter; }
    inline const int getLengthMultiplierCounter() const noexcept { return lengthMultiplierCounter; }
    inline const int getTranspCounter() const noexcept { return transpCounter; }
    inline const int getEnvelopeCounter() const noexcept { return envelopeCounter; }
    inline const int getBeatCounter() const noexcept { return beatCounter; }
    inline const int getClusterSize() const noexcept { return cluster.size(); }
    
    int getLengthMultiplierCounterForDisplay()
    {
        int tempsize = prep->getLengthMultipliers().size();
        //int counter = getLengthMultiplierCounter() - 1;
        int counter = getLengthMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getBeatMultiplierCounterForDisplay()
    {
        int tempsize = prep->getBeatMultipliers().size();
        int counter = getBeatMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getAccentMultiplierCounterForDisplay()
    {
        int tempsize = prep->getAccentMultipliers().size();
        int counter = getAccentMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getTranspCounterForDisplay()
    {
        int tempsize = prep->getTransposition().size();
        int counter = getTranspCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    inline void setBeatPhasor(uint64 c)  { phasor = c; }
    inline void setBeatMultiplierCounter(int c) {  beatMultiplierCounter = c; }
    inline void setAccentMultiplierCounter(int c)  { accentMultiplierCounter = c; }
    inline void setLengthMultiplierCounter(int c)  { lengthMultiplierCounter = c; }
    inline void setTranspCounter(int c)  { transpCounter = c; }
    inline void setEnvelopeCounter(int c)  { envelopeCounter = c; }
    inline void setBeatCounter(int c)  { beatCounter = c; }
    
    inline bool getOver(void) { return over; }
    inline void setOver(bool o) { over = o; }
    
    inline void incrementPhasor (int numSamples)
    {
        phasor += numSamples;
    }
    
    inline void step (uint64 numSamplesBeat)
    {
        phasor -= numSamplesBeat;
        
        //increment parameter counters
        if (++lengthMultiplierCounter   >= prep->getLengthMultipliers().size())     lengthMultiplierCounter = 0;
        if (++accentMultiplierCounter   >= prep->getAccentMultipliers().size())     accentMultiplierCounter = 0;
        if (++transpCounter             >= prep->getTransposition().size())         transpCounter = 0;
        if (++envelopeCounter           >= prep->getEnvelopesOn().size())           envelopeCounter = 0;
        
        while(!prep->getEnvelopesOn()[envelopeCounter]) //skip untoggled envelopes
        {
            envelopeCounter++;
            if (envelopeCounter >= prep->getEnvelopesOn().size()) envelopeCounter = 0;
        }
        
    }
    
    inline void postStep ()
    {
    
        if (++beatMultiplierCounter >= prep->getBeatMultipliers().size())
        {
            //increment beat and beatMultiplier counters, for next beat; check maxes and adjust
            beatMultiplierCounter = 0;
        }
        
        if (++beatCounter >= (prep->getNumBeats() + prep->getBeatsToSkip()))
        {
            shouldPlay = false;
        }
    }
    
    inline void resetPatternPhase()
    {
        int skipBeats = prep->getBeatsToSkip() - 1;
        int idx = (skipBeats < -1) ? -1 : skipBeats;
        
        if(prep->getBeatMultipliers().size() > 0)   beatMultiplierCounter   = mod(idx, prep->getBeatMultipliers().size());
        if(prep->getLengthMultipliers().size() > 0) lengthMultiplierCounter = mod(idx, prep->getLengthMultipliers().size());
        if(prep->getAccentMultipliers().size() > 0) accentMultiplierCounter = mod(idx, prep->getAccentMultipliers().size());
        if(prep->getTransposition().size() > 0)     transpCounter           = mod(idx, prep->getTransposition().size());
        if(prep->getEnvelopesOn().size() > 0)       envelopeCounter         = mod(idx, prep->getEnvelopesOn().size());

        DBG("beatMultiplierCounter = " + String(beatMultiplierCounter));

        beatCounter             = 0;
    }
    
    inline Array<int> getCluster() {return cluster;}
    
    inline void setCluster(Array<int> c) { cluster = c; }
    
    inline void addNote(int note)
    {
        DBG("adding note: " + String(note));
        cluster.insert(0, note);
    }
    
    inline void removeNote(int note)
    {
        int idx = 0;
        
        for (auto n : cluster)
        {
            if (n == note)
            {
                break;
            }
            idx++;
        }
        
        cluster.remove(idx);
    }
    
    inline bool containsNote(int note)
    {
        return cluster.contains(note);
    }
    
    inline void setShouldPlay(bool play)
    {
        shouldPlay = play;
    }
    
    inline bool getShouldPlay(void)
    {
        return shouldPlay;
        
    }
    
private:
    
    Array<int> cluster;
    
    int beatCounter;  //beat (or pulse) counter; max set by users -- sNumBeats
    
    SynchronicPreparation::Ptr prep;
    
    //parameter field counters
    int beatMultiplierCounter;   //beat length (time between beats) multipliers
    int accentMultiplierCounter; //accent multipliers
    int lengthMultiplierCounter; //note length (sounding length) multipliers (multiples of 50ms, at least for now)
    int transpCounter;     //transposition offsets
    int envelopeCounter;
    
    uint64 phasor;
    bool shouldPlay, over;
    
    JUCE_LEAK_DETECTOR(SynchronicCluster);
};


/*
 SynchronicProcessor does the main work, including processing a block
 of samples and sending it out. It connects Keymap, Tuning, and
 Blendronic preparations together as needed, and gets the Synchronic
 values it needs to behave as expected. It also uses a SynchronicCluster
 rather than an individual Synchronic, to enable layering of
 multiple synchronic pulses all sharing the same settings
 */

class SynchronicProcessor  : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicProcessor>   Ptr;
    typedef Array<SynchronicProcessor::Ptr>                  PtrArr;
    typedef Array<SynchronicProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicProcessor>                  Arr;
    typedef OwnedArray<SynchronicProcessor,CriticalSection>  CSArr;
    
    
    SynchronicProcessor(Synchronic::Ptr synchronic,
                        TuningProcessor::Ptr tuning,
                        TempoProcessor::Ptr tempo,
						BlendronicProcessor::PtrArr blend,
                        BKSynthesiser* main,
                        GeneralSettings::Ptr general);
    
    ~SynchronicProcessor();
    
    void         setCurrentPlaybackSampleRate(double sr);
    inline const uint64 getCurrentNumSamplesBeat(void) const noexcept   { return numSamplesBeat;    }
    
    
    BKSampleLoadType sampleType;
    void processBlock(int numSamples, int midiChannel, BKSampleLoadType type);
    void keyPressed(int noteNumber, float velocity, Array<KeymapTargetState> targetStates);
    void keyReleased(int noteNumber, float velocity, int channel, Array<KeymapTargetState> targetStates);
    float getTimeToBeatMS(float beatsToSkip);
    
    
    inline const float getClusterThresholdTimer() const noexcept { return 1000. * clusterThresholdTimer / sampleRate ;}
    inline const float getClusterThreshold() const noexcept { return 1000. * thresholdSamples / sampleRate ;}
    inline const int getNumKeysDepressed() const noexcept {return keysDepressed.size(); }
    inline const bool getPlayCluster() const noexcept { return playCluster; }
    
    inline const float getHoldTimer() const noexcept
    {
        if(keysDepressed.size() == 0 ) return 0;
        return 1000. * holdTimers[lastKeyPressed] / sampleRate ;
    }
    
    inline const float getLastVelocity() const noexcept
    {
        if(keysDepressed.size() == 0 ) return 0;
        return lastKeyVelocity;
    }
    
    inline const SynchronicSyncMode getMode() const noexcept {return synchronic->aPrep->getMode(); }

    inline int getId(void) const noexcept { return synchronic->getId(); }
    
    inline void setSynchronic(Synchronic::Ptr newSynchronic)
    {
        synchronic = newSynchronic;
    }
    
    inline void setTuning(TuningProcessor::Ptr tuning)
    {
        tuner = tuning;
    }
    
    inline void setTempo(TempoProcessor::Ptr newTempo)
    {
        tempo = newTempo;
    }

	inline void addBlendronic(BlendronicProcessor::Ptr blend)
	{
		blendronic.add(blend);
	}
    
    inline Synchronic::Ptr getSynchronic(void) const noexcept
    {
        return synchronic;
    }
    
    inline TuningProcessor::Ptr getTuning(void) const noexcept
    {
        return tuner;
    }
    
    inline TempoProcessor::Ptr getTempo(void) const noexcept
    {
        return tempo;
    }

	inline BlendronicProcessor::PtrArr getBlendronic(void) const noexcept
	{
		return blendronic;
	}
    
    inline int getTuningId(void) const noexcept
    {
        return tuner->getId();
    }
    
    inline int getTempoId(void) const noexcept
    {
        return tempo->getId();
    }

//    inline int getBlendronicId(void) const noexcept
//    {
//        return blendronic->getId();
//    }

    inline void prepareToPlay(float sr, BKSynthesiser* main)
    {
        synth = main;
        sampleRate = sr;
    }
    //void  atReset();
    
    inline void reset(void)
    {
        synchronic->aPrep->copy(synchronic->sPrep);
    }
    
    void clearOldNotes()
    {
        keysDepressed.clearQuick();
        clusters.clearQuick();
    }
    
    inline SynchronicCluster::PtrArr getClusters(void)
    {
        return clusters;
    }
    
    inline SynchronicCluster* getCluster(int which)
    {
        return clusters[which];
    }
    
    bool velocityCheck(int noteNumber);
    bool holdCheck(int noteNumber);
    
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
    
    Synchronic::Ptr synchronic;
    TuningProcessor::Ptr tuner;
    TempoProcessor::Ptr tempo;
	BlendronicProcessor::PtrArr blendronic;
    
    Keymap::PtrArr      keymaps;
    
    double sampleRate;

    
    Array<float> tuningOffsets;
    PitchClass tuningBasePitch;
    
    void playNote(int channel, int note, float velocity, SynchronicCluster::Ptr cluster);
    Array<float> velocities;    //record of velocities
    Array<int> keysDepressed;   //current keys that are depressed
    Array<int> syncKeysDepressed;
    Array<int> clusterKeysDepressed;
    Array<int> patternSyncKeysDepressed;
    bool playCluster;
    
    bool inCluster;
    // bool inSyncCluster;
    bool nextOffIsFirst;
    
    bool pausePlay; // pause phasor incrementing

    uint64 thresholdSamples;
    uint64 clusterThresholdTimer;
    uint64 syncThresholdTimer;
    Array<int> slimCluster;     //cluster without repetitions
    
    SynchronicCluster::PtrArr clusters;

    uint64 numSamplesBeat;          // = beatThresholdSamples * beatMultiplier
    uint64 beatThresholdSamples;    // # samples in a beat, as set by tempo
    
    Array<uint64> holdTimers;
    int lastKeyPressed;
    float lastKeyVelocity;
    
    BKSynthesiserVoice** asv;
    Array<BKSynthesiserVoice*> activeSynchronicVoices;

    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};


#endif  // SYNCHRONIC_H_INCLUDED
