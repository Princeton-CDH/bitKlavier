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
    SynchronicPreparation(SynchronicPreparation::Ptr p)
    {
        copy(p);
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
    numClusters(1),
    holdMin(0),
    holdMax(12000),
    velocityMin(0),
    velocityMax(127),
    sMode(mode),
    sBeatsToSkip(beatsToSkip),
    onOffMode(KeyOn),
    sTranspUsesTuning(false),
    sClusterThresh(clusterThresh),
    sClusterThreshSec(.001 * sClusterThresh.base),
    sReleaseVelocitySetsSynchronic(velocityMode),
    midiOutput(nullptr),
    sUseGlobalSoundSet(true),
    sSoundSet(-1),
    targetTypeSynchronicPatternSync(NoteOn),
    targetTypeSynchronicBeatSync(NoteOn),
    targetTypeSynchronicAddNotes(NoteOn),
    targetTypeSynchronicPausePlay(NoteOn),
    targetTypeSynchronicClear(NoteOn),
    targetTypeSynchronicDeleteOldest(NoteOn),
    targetTypeSynchronicDeleteNewest(NoteOn),
    targetTypeSynchronicRotate(NoteOn)
    {
        sBeatMultipliers = Array<Moddable<float>>();
        for (auto f : beatMultipliers) sBeatMultipliers.add(Moddable<float>(f));
        
        sAccentMultipliers = Array<Moddable<float>>();
        for (auto f : accentMultipliers) sAccentMultipliers.add(Moddable<float>(f));
        
        sLengthMultipliers = Array<Moddable<float>>();
        for (auto f : lengthMultipliers) sLengthMultipliers.add(Moddable<float>(f));
        
        sTransposition = Array<Array<Moddable<float>>>();
        for (auto a : transp)
        {
            Array<Moddable<float>> arr;
            for (auto f : a) arr.add(Moddable<float>(f));
            sTransposition.add(arr);
        }
        
        sBeatMultipliersStates = Array<Moddable<bool>>();
        sAccentMultipliersStates = Array<Moddable<bool>>();
        sLengthMultipliersStates = Array<Moddable<bool>>();
        sTranspositionStates = Array<Moddable<bool>>();
        for (int i = 0; i < 12; ++i)
        {
            sBeatMultipliersStates.add(Moddable<bool>(i == 0));
            sAccentMultipliersStates.add(Moddable<bool>(i == 0));
            sLengthMultipliersStates.add(Moddable<bool>(i == 0));
            sTranspositionStates.add(Moddable<bool>(i == 0));
        }
    }

    
    SynchronicPreparation(void):
    sGain(1.0),
    sBlendronicGain(1.0f),
    sNumBeats(20),
    sClusterMin(1),
    sClusterMax(12),
    sClusterCap(8), //8 in original bK, but behavior is different here?
    numClusters(1),
    holdMin(0),
    holdMax(12000),
    velocityMin(0),
    velocityMax(127),
    sMode(FirstNoteOnSync),
    sBeatsToSkip(0),
    onOffMode(KeyOn),
    sTranspUsesTuning(false),
    sClusterThresh(500),
    sClusterThreshSec(.001 * sClusterThresh.base),
    midiOutput(nullptr),
    sUseGlobalSoundSet(true),
    sSoundSet(-1),
    targetTypeSynchronicPatternSync(NoteOn),
    targetTypeSynchronicBeatSync(NoteOn),
    targetTypeSynchronicAddNotes(NoteOn),
    targetTypeSynchronicPausePlay(NoteOn),
    targetTypeSynchronicClear(NoteOn),
    targetTypeSynchronicDeleteOldest(NoteOn),
    targetTypeSynchronicDeleteNewest(NoteOn),
    targetTypeSynchronicRotate(NoteOn)
    {
        sBeatMultipliers = Array<Moddable<float>>();
        sBeatMultipliers.add(Moddable<float>(1.0f));
        
        sAccentMultipliers = Array<Moddable<float>>();
        sAccentMultipliers.add(Moddable<float>(1.0f));
        
        sLengthMultipliers = Array<Moddable<float>>();
        sLengthMultipliers.add(Moddable<float>(1.0f));
        
        sTransposition = Array<Array<Moddable<float>>>();
        Array<Moddable<float>> arr;
        arr.add(Moddable<float>(0.0f));
        sTransposition.add(arr);
        
        sBeatMultipliersStates = Array<Moddable<bool>>();
        sAccentMultipliersStates = Array<Moddable<bool>>();
        sLengthMultipliersStates = Array<Moddable<bool>>();
        sAttacks = Array<Moddable<float>>();
        sDecays = Array<Moddable<float>>();
        sSustains = Array<Moddable<float>>();
        sReleases = Array<Moddable<float>>();
        envelopeOn = Array<Moddable<bool>>();
        for (int i = 0; i < 12; ++i)
        {
            sBeatMultipliersStates.add(Moddable<bool>(i == 0));
            sAccentMultipliersStates.add(Moddable<bool>(i == 0));
            sLengthMultipliersStates.add(Moddable<bool>(i == 0));
            sTranspositionStates.add(Moddable<bool>(i == 0));
            sAttacks.add(Moddable<float>(3));
            sDecays.add(Moddable<float>(3));
            sSustains.add(Moddable<float>(1.0f));
            sReleases.add(Moddable<float>(30));
            envelopeOn.add(Moddable<bool>(i == 0));
        }
    }
    
    inline void copy(SynchronicPreparation::Ptr s)
    {
        sNumBeats = s->sNumBeats;
        sClusterMin = s->sClusterMin;
        sClusterMax = s->sClusterMax;
        sClusterCap = s->sClusterCap;
        sMode = s->sMode;
        sBeatsToSkip = s->sBeatsToSkip;
        sBeatMultipliers = s->sBeatMultipliers;
        sAccentMultipliers = s->sAccentMultipliers;
        sLengthMultipliers = s->sLengthMultipliers;
        sGain = s->sGain;
        sBlendronicGain = s->sBlendronicGain;
        
        sTransposition = s->sTransposition;
        sClusterThresh = s->sClusterThresh;
        sClusterThreshSec = s->sClusterThreshSec;
        sReleaseVelocitySetsSynchronic = s->sReleaseVelocitySetsSynchronic;
        
        sBeatMultipliersStates = s->sBeatMultipliersStates;
        sAccentMultipliersStates = s->sAccentMultipliersStates;
        sLengthMultipliersStates = s->sLengthMultipliersStates;
        sTranspositionStates = s->sTranspositionStates;
        
        sTranspUsesTuning = s->sTranspUsesTuning;
        
        sAttacks = s->sAttacks;
        sDecays = s->sDecays;
        sSustains = s->sSustains;
        sReleases = s->sReleases;
        
        envelopeOn = s->envelopeOn;
        
        numClusters = s->numClusters;
        onOffMode = s->onOffMode;
        
        holdMin = s->holdMin;
        holdMax = s->holdMax;
        
        velocityMin = s->velocityMin;
        velocityMax = s->velocityMax;
        
        targetTypeSynchronicPatternSync = s->getTargetTypeSynchronicPatternSync();
        targetTypeSynchronicBeatSync = s->getTargetTypeSynchronicBeatSync();
        targetTypeSynchronicAddNotes = s->getTargetTypeSynchronicAddNotes();
        targetTypeSynchronicPausePlay = s->getTargetTypeSynchronicPausePlay();
        targetTypeSynchronicClear = s->getTargetTypeSynchronicClear();
        targetTypeSynchronicDeleteOldest = s->getTargetTypeSynchronicDeleteOldest();
        targetTypeSynchronicDeleteNewest = s->getTargetTypeSynchronicDeleteNewest();
        targetTypeSynchronicRotate = s->getTargetTypeSynchronicRotate();
        
        midiOutput = s->midiOutput;
        
        sUseGlobalSoundSet = s->sUseGlobalSoundSet;
        sSoundSet = s->sSoundSet;
    }
    
    void performModification(SynchronicPreparation::Ptr s, Array<bool> dirty)
    {
        if (dirty[SynchronicGain]) sGain.modify(s->sGain, false);
        if (dirty[SynchronicBlendronicGain]) sBlendronicGain.modify(s->sBlendronicGain, false);
        if (dirty[SynchronicNumPulses]) sNumBeats = s->sNumBeats;
        if (dirty[SynchronicClusterMin]) sClusterMin = s->sClusterMin;
        if (dirty[SynchronicClusterMax]) sClusterMax = s->sClusterMax;
        if (dirty[SynchronicClusterCap]) sClusterCap = s->sClusterCap;
        if (dirty[SynchronicMode]) sMode = s->sMode;
        
        if (dirty[SynchronicBeatsToSkip]) sBeatsToSkip = s->sBeatsToSkip;
        
        if (dirty[SynchronicBeatMultipliers]) {
            
            sBeatMultipliers = s->sBeatMultipliers;
            sBeatMultipliersStates = s->sBeatMultipliersStates;
        }
        if (dirty[SynchronicAccentMultipliers]) {
            sAccentMultipliers = s->sAccentMultipliers;
            sAccentMultipliersStates = s->sAccentMultipliersStates;
        }
        if (dirty[SynchronicLengthMultipliers]) {
            sLengthMultipliers = s->sLengthMultipliers;
            sLengthMultipliersStates = s->sLengthMultipliersStates;
        }
        if (dirty[SynchronicTranspOffsets]) {
            sTransposition = s->sTransposition;
            sTranspositionStates = s->sTranspositionStates;
        }
        
        if (dirty[SynchronicTranspUsesTuning]) sTranspUsesTuning = s->sTranspUsesTuning;
        
        if (dirty[SynchronicClusterThresh])
        {
            sClusterThresh = s->sClusterThresh;
            sClusterThreshSec = s->sClusterThreshSec;
        }

        if (dirty[SynchronicADSRs])
        {
            sAttacks = s->sAttacks;
            sDecays = s->sDecays;
            sSustains = s->sSustains;
            sReleases = s->sReleases;
            
            envelopeOn = s->envelopeOn;
        }
        
        if (dirty[SynchronicNumClusters]) numClusters = s->numClusters;
        if (dirty[SynchronicOnOff]) onOffMode = s->onOffMode;
        
        if (dirty[SynchronicHoldMin]) holdMin = s->holdMin;
        if (dirty[SynchronicHoldMax]) holdMax = s->holdMax;
        
        if (dirty[SynchronicVelocityMin]) velocityMin = s->velocityMin;
        if (dirty[SynchronicVelocityMax]) velocityMax = s->velocityMax;
        
        if (dirty[SynchronicMidiOutput]) midiOutput = s->midiOutput;
        
        if (dirty[SynchronicUseGlobalSoundSet]) sUseGlobalSoundSet = s->sUseGlobalSoundSet;
        
        if (dirty[SynchronicSoundSet])
        {
            sSoundSet = s->sSoundSet;
            sSoundSetName = s->sSoundSetName;
        }
    }
        
    void stepModdables()
    {
        sGain.step();
        sBlendronicGain.step();
    }
    
    void resetModdables()
    {
        sGain.reset();
        sBlendronicGain.reset();
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
        bool lensStates = true;
        bool accentsStates = true;
        bool beatsStates = true;
        bool transpStates = true;
        
        for (int i = s->sLengthMultipliers.size(); --i>=0;)
        {
            if (s->sLengthMultipliers[i] != sLengthMultipliers[i])
            {
                lens = false;
                break;
            }
        }
        
        for (int i = s->sAccentMultipliers.size(); --i>=0;)
        {
            if (s->sAccentMultipliers[i] != sAccentMultipliers[i])
            {
                accents = false;
                break;
            }
        }
        
        for (int i = s->sBeatMultipliers.size(); --i>=0;)
        {
            if (s->sBeatMultipliers[i] != sBeatMultipliers[i])
            {
                beats = false;
                break;
            }
        }
        
        for (int i  = s->sTransposition.size(); --i >= 0;)
        {
            Array<Moddable<float>> transposition = s->sTransposition[i];
            for (int j = transposition.size(); --j >= 0;)
            {
                if (transposition[j] != sTransposition[i][j])
                {
                    transp = false;
                    break;
                }
            }
        }
        
        for (int i = s->sLengthMultipliersStates.size(); --i>=0;)
        {
            if (s->sLengthMultipliersStates[i] != sLengthMultipliersStates[i])
            {
                lensStates = false;
                break;
            }
        }
        
        for (int i = s->sAccentMultipliersStates.size(); --i>=0;)
        {
            if (s->sAccentMultipliersStates[i] != sAccentMultipliersStates[i])
            {
                accentsStates = false;
                break;
            }
        }
        
        for (int i = s->sBeatMultipliersStates.size(); --i>=0;)
        {
            if (s->sBeatMultipliersStates[i] != sBeatMultipliersStates[i])
            {
                beatsStates = false;
                break;
            }
        }
        
        for (int i = s->sTranspositionStates.size(); --i>=0;)
        {
            if (s->sTranspositionStates[i] != sTranspositionStates[i])
            {
                transpStates = false;
                break;
            }
        }
        
        for (int i = s->sAttacks.size(); --i>=0;)
        {
            if (s->sAttacks[i] != sAttacks[i])
            {
                attack = false;
                break;
            }
        }
        
        for (int i = s->sDecays.size(); --i>=0;)
        {
            if (s->sDecays[i] != sDecays[i])
            {
                decay = false;
                break;
            }
        }
        
        for (int i = s->sSustains.size(); --i>=0;)
        {
            if (s->sSustains[i] != sSustains[i])
            {
                sustain = false;
                break;
            }
        }
        
        for (int i = s->sReleases.size(); --i>=0;)
        {
            if (s->sReleases[i] != sReleases[i])
            {
                release = false;
                break;
            }
        }
        
        for (int i = s->envelopeOn.size(); --i>=0;)
        {
            if (s->envelopeOn[i] != envelopeOn[i])
            {
                envelope = false;
                break;
            }
        }
        
        return (sNumBeats == s->sNumBeats &&
                sClusterMin == s->sClusterMin &&
                sClusterMax == s->sClusterMax &&
                sClusterCap == s->sClusterCap &&
                sMode == s->sMode &&
                transp && lens && accents && beats && attack && decay && sustain && release &&
                transpStates && lensStates && accentsStates && beatsStates &&
                sGain == s->sGain &&
                sBlendronicGain == s->sBlendronicGain &&
                sTranspUsesTuning == s->sTranspUsesTuning &&
                sClusterThresh == s->sClusterThresh &&
                sClusterThreshSec == s->sClusterThreshSec &&
                sReleaseVelocitySetsSynchronic == s->sReleaseVelocitySetsSynchronic &&
                numClusters == s->numClusters &&
                onOffMode == s->onOffMode &&
                holdMin == s->holdMin &&
                holdMax == s->holdMax &&
                velocityMin == s->velocityMin &&
                velocityMax == s->velocityMax &&
                targetTypeSynchronicPatternSync == s->getTargetTypeSynchronicPatternSync() &&
                targetTypeSynchronicBeatSync == s->getTargetTypeSynchronicBeatSync() &&
                targetTypeSynchronicAddNotes == s->getTargetTypeSynchronicAddNotes() &&
                targetTypeSynchronicPausePlay == s->getTargetTypeSynchronicPausePlay()) &&
                targetTypeSynchronicClear == s->getTargetTypeSynchronicClear() &&
                targetTypeSynchronicDeleteOldest == s->getTargetTypeSynchronicDeleteOldest() &&
                targetTypeSynchronicDeleteNewest == s->getTargetTypeSynchronicDeleteNewest() &&
                targetTypeSynchronicRotate == s->getTargetTypeSynchronicRotate() ;
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
        sBlendronicGain = r[idx++] * 2;
		sTransposition.clear();
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			Array<Moddable<float>> transposition;
			for (int j = 0; j < Random::getSystemRandom().nextInt(10); j++)
			{
				transposition.add(i, Moddable<float>((Random::getSystemRandom().nextFloat()) * 48.0f - 24.0f));
			}
			sTransposition.add(transposition);
		}
		sClusterThresh = (r[idx++] * 2000) + 1;
		sClusterThreshSec = sClusterThresh.value * 0.001f;
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
    
    inline const int getOffsetParamToggle() const noexcept
    {
        if(sMode == FirstNoteOnSync || sMode == AnyNoteOnSync) return sBeatsToSkip.value + 1;
        else return sBeatsToSkip.value;
    }
    
    inline float* getGainPtr() { return &sGain.value; }
    inline float* getBlendronicGainPtr() { return &sBlendronicGain.value; }
    
    inline const int getAttack(int which) const noexcept    {return sAttacks[which].value;}
    inline const int getDecay(int which) const noexcept     {return sDecays[which].value;}
    inline const float getSustain(int which) const noexcept {return sSustains[which].value;}
    inline const int getRelease(int which) const noexcept   {return sReleases[which].value;}
    
    inline const Array<Array<Moddable<float>>> getADSRs() const noexcept
    {
        Array<Array<Moddable<float>>> allADSRs;
        for(int i=0; i<sAttacks.size(); i++)
        {
            Array<Moddable<float>> singleADSR;
            singleADSR.insert(0, sAttacks[i]);
            singleADSR.insert(1, sDecays[i]);
            singleADSR.insert(2, sSustains[i]);
            singleADSR.insert(3, sReleases[i]);
            if(envelopeOn[i].value)singleADSR.insert(4, 1);
            else singleADSR.insert(4, 0);
            
            allADSRs.insert(i, singleADSR);
        }
        
        return allADSRs;
    }
    
    inline const bool getEnvelopeOn(int which) const noexcept   { return envelopeOn[which].value; }
    
    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = sClusterThresh.base * .001;
    }
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}

    inline void setBeatMultipliers(Array<Moddable<float>> beatMultipliers)
    {
        sBeatMultipliers.clear();
        for (auto f : beatMultipliers) sBeatMultipliers.add(f);
    }
    inline void setAccentMultipliers(Array<Moddable<float>> accentMultipliers)
    {
        sAccentMultipliers.clear();
        for (auto f : accentMultipliers) sAccentMultipliers.add(f);
    }
    inline void setTransposition(Array<Array<Moddable<float>>> transp)
    {
        sTransposition.clear();
        for (auto a : transp)
        {
            Array<Moddable<float>> arr;
            for (auto f : a) arr.add(f);
            sTransposition.add(arr);
        }
    }
    inline void setLengthMultipliers(Array<Moddable<float>> lengthMultipliers)
    {
        sLengthMultipliers.clear();
        for (auto f : lengthMultipliers) sLengthMultipliers.add(f);
    }
    inline void setBeatMultipliersStates(Array<Moddable<bool>> beatMultipliers)
    {
        sBeatMultipliersStates.clear();
        for (auto b : beatMultipliers) sBeatMultipliersStates.add(b);
    }
    inline void setAccentMultipliersStates(Array<Moddable<bool>> accentMultipliers)
    {
        sAccentMultipliersStates.clear();
        for (auto b : accentMultipliers) sAccentMultipliersStates.add(b);
    }
    inline void setTranspositionStates(Array<Moddable<bool>> transp)
    {
        sTranspositionStates.clear();
        for (auto b : transp) sTranspositionStates.add(b);
    }
    inline void setLengthMultipliersStates(Array<Moddable<bool>> lengthMultipliers)
    {
        sLengthMultipliersStates.clear();
        for (auto b : lengthMultipliers) sLengthMultipliersStates.add(b);
    }
    
    inline void setBeatMultipliers(Array<float> beatMultipliers)
    {
        sBeatMultipliers.clear();
        for (auto f : beatMultipliers) sBeatMultipliers.add(f);
    }
    inline void setAccentMultipliers(Array<float> accentMultipliers)
    {
        sAccentMultipliers.clear();
        for (auto f : accentMultipliers) sAccentMultipliers.add(f);
    }
    inline void setTransposition(Array<Array<float>> transp)
    {
        sTransposition.clear();
        for (auto a : transp)
        {
            Array<Moddable<float>> arr;
            for (auto f : a) arr.add(f);
            sTransposition.add(arr);
        }
    }
    inline void setLengthMultipliers(Array<float> lengthMultipliers)
    {
        sLengthMultipliers.clear();
        for (auto f : lengthMultipliers) sLengthMultipliers.add(f);
    }
    inline void setBeatMultipliersStates(Array<bool> beatMultipliers)
    {
        sBeatMultipliersStates.clear();
        for (auto b : beatMultipliers) sBeatMultipliersStates.add(b);
    }
    inline void setAccentMultipliersStates(Array<bool> accentMultipliers)
    {
        sAccentMultipliersStates.clear();
        for (auto b : accentMultipliers) sAccentMultipliersStates.add(b);
    }
    inline void setTranspositionStates(Array<bool> transp)
    {
        sTranspositionStates.clear();
        for (auto b : transp) sTranspositionStates.add(b);
    }
    inline void setLengthMultipliersStates(Array<bool> lengthMultipliers)
    {
        sLengthMultipliersStates.clear();
        for (auto b : lengthMultipliers) sLengthMultipliersStates.add(b);
    }
    
    inline void setBeatMultiplier(int whichSlider, float value)
    {
        sBeatMultipliers.set(whichSlider, value);
    }
    inline void setAccentMultiplier(int whichSlider, float value)
    {
        sAccentMultipliers.set(whichSlider, value);
    }
    inline void setLengthMultiplier(int whichSlider, float value)
    {
        sLengthMultipliers.set(whichSlider, value);
    }
    inline void setSingleTransposition(int whichSlider, Array<float> values)
    {
        Array<Moddable<float>> arr;
        for (auto v : values) arr.add(v);
        sTransposition.set(whichSlider, arr);
    }
    
    inline void setAttacks(Array<int> attacks)
    {
        sAttacks.clear();
        for (auto f : attacks) sAttacks.add(Moddable<float>(f));
    }
    inline void setDecays(Array<int> decays)
    {
        sDecays.clear();
        for (auto f : decays) sDecays.add(Moddable<float>(f));
    }
    inline void setSustains(Array<float> sustains)
    {
        sSustains.clear();
        for (auto f : sustains) sSustains.add(Moddable<float>(f));
    }
    inline void setReleases(Array<int> releases)
    {
        sReleases.clear();
        for (auto f : releases) sReleases.add(Moddable<float>(f));
    }
    
    inline void setAttack(int which, int val)       {sAttacks.set(which, val);}
    inline void setDecay(int which, int val)        {sDecays.set(which, val);}
    inline void setSustain(int which, float val)    {sSustains.set(which, val);}
    inline void setRelease(int which, int val)      {sReleases.set(which, val);}
    
    inline const TargetNoteMode getTargetTypeSynchronicPatternSync() const noexcept { return targetTypeSynchronicPatternSync; }
    inline const TargetNoteMode getTargetTypeSynchronicBeatSync() const noexcept { return targetTypeSynchronicBeatSync; }
    inline const TargetNoteMode getTargetTypeSynchronicAddNotes() const noexcept { return targetTypeSynchronicAddNotes; }
    inline const TargetNoteMode getTargetTypeSynchronicPausePlay() const noexcept { return targetTypeSynchronicPausePlay; }
    inline const TargetNoteMode getTargetTypeSynchronicClear() const noexcept { return targetTypeSynchronicClear; }
    inline const TargetNoteMode getTargetTypeSynchronicDeleteOldest() const noexcept { return targetTypeSynchronicDeleteOldest; }
    inline const TargetNoteMode getTargetTypeSynchronicDeleteNewest() const noexcept { return targetTypeSynchronicDeleteNewest; }
    inline const TargetNoteMode getTargetTypeSynchronicRotate() const noexcept { return targetTypeSynchronicRotate; }
    inline const TargetNoteMode getTargetTypeSynchronic(KeymapTargetType which)
    {
        if (which == TargetTypeSynchronicPatternSync)   return targetTypeSynchronicPatternSync;
        if (which == TargetTypeSynchronicBeatSync)      return targetTypeSynchronicBeatSync;
        if (which == TargetTypeSynchronicAddNotes)      return targetTypeSynchronicAddNotes;
        if (which == TargetTypeSynchronicPausePlay)     return targetTypeSynchronicPausePlay;
        if (which == TargetTypeSynchronicClear)         return targetTypeSynchronicClear;
        if (which == TargetTypeSynchronicDeleteOldest)  return targetTypeSynchronicDeleteOldest;
        if (which == TargetTypeSynchronicDeleteNewest)  return targetTypeSynchronicDeleteNewest;
        if (which == TargetTypeSynchronicRotate)        return targetTypeSynchronicRotate;
        return TargetNoteModeNil;
    }
    
    inline void setTargetTypeSynchronicPatternSync(TargetNoteMode nm)   { targetTypeSynchronicPatternSync = nm; }
    inline void setTargetTypeSynchronicBeatSync(TargetNoteMode nm)      { targetTypeSynchronicBeatSync = nm; }
    inline void setTargetTypeSynchronicAddNotes(TargetNoteMode nm)      { targetTypeSynchronicAddNotes = nm; }
    inline void setTargetTypeSynchronicPausePlay(TargetNoteMode nm)     { targetTypeSynchronicPausePlay = nm; }
    inline void setTargetTypeSynchronicClear(TargetNoteMode nm)         { targetTypeSynchronicClear = nm; }
    inline void setTargetTypeSynchronicDeleteOldest(TargetNoteMode nm)  { targetTypeSynchronicDeleteOldest = nm; }
    inline void setTargetTypeSynchronicDeleteNewest(TargetNoteMode nm)  { targetTypeSynchronicDeleteNewest = nm; }
    inline void setTargetTypeSynchronicRotate(TargetNoteMode nm)        { targetTypeSynchronicRotate = nm; }
    inline void setTargetTypeSynchronic(KeymapTargetType which, TargetNoteMode nm)
    {
        if (which == TargetTypeSynchronicPatternSync)   { targetTypeSynchronicPatternSync = nm; }
        if (which == TargetTypeSynchronicBeatSync)      { targetTypeSynchronicBeatSync = nm; }
        if (which == TargetTypeSynchronicAddNotes)      { targetTypeSynchronicAddNotes = nm; }
        if (which == TargetTypeSynchronicPausePlay)     { targetTypeSynchronicPausePlay = nm; }
        if (which == TargetTypeSynchronicClear)         { targetTypeSynchronicClear = nm; }
        if (which == TargetTypeSynchronicDeleteOldest)  { targetTypeSynchronicDeleteOldest = nm; }
        if (which == TargetTypeSynchronicDeleteNewest)  { targetTypeSynchronicDeleteNewest = nm; }
        if (which == TargetTypeSynchronicRotate)        { targetTypeSynchronicRotate = nm; }
    }
    
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
        // DBG("ADSR envelopeOn = " + String(which) + " " + String((int)getEnvelopeOn(which)));
        
    }
    
    inline void setEnvelopeOn(int which, bool val)  {envelopeOn.set(which, val);}
    
    inline const void setMidiOutput(MidiDeviceInfo device)
    {
        midiOutput = MidiOutput::openDevice(device.identifier);
        if (!midiOutput.value) midiOutput = nullptr;
    }
    inline const void setMidiOutput(std::shared_ptr<MidiOutput> output)
    {
        midiOutput = output;
    }
    
    void print(void)
    {
        DBG("| - - - Synchronic Preparation - - - |");
        DBG("sTempo: " + String(sTempo.value));
        DBG("sNumBeats: " + String(sNumBeats.value));
        DBG("sClusterMin: " + String(sClusterMin.value));
        DBG("sClusterMax: " + String(sClusterMax.value));
        DBG("sClusterCap: " + String(sClusterCap.value));
        DBG("sClusterThresh: " + String(sClusterThresh.value));
        DBG("sReleaseVelocitySetsSynchronic: " + String((int)sReleaseVelocitySetsSynchronic.value));
        DBG("sMode: " + String(sMode.value));
        DBG("sBeatsToSkip: " + String(sBeatsToSkip.value));
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers));
        
        String s = "";
        for (auto arr : sTransposition) s += "{ " + floatArrayToString(arr) + " },\n";
        DBG("sTransposition: " + s);
        
        DBG("sClusterThreshSec: " + String(sClusterThreshSec.value));
        //DBG("resetKeymap: " + intArrayToString(getResetMap()->keys()));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
    ValueTree getState(void)
    {
        ValueTree prep("params");
        
        sGain.getState(prep, ptagSynchronic_gain);
        sBlendronicGain.getState(prep, ptagSynchronic_blendronicGain);
        
        sNumBeats.getState(prep, ptagSynchronic_numBeats);
        sClusterMin.getState(prep, ptagSynchronic_clusterMin);
        sClusterMax.getState(prep, ptagSynchronic_clusterMax);
        sClusterCap.getState(prep, ptagSynchronic_clusterCap);
        sClusterThresh.getState(prep, ptagSynchronic_clusterThresh);
        sMode.getState(prep, ptagSynchronic_mode);
        sBeatsToSkip.getState(prep, ptagSynchronic_beatsToSkip);
        sTranspUsesTuning.getState(prep, ptagSynchronic_transpUsesTuning);

        numClusters.getState(prep, "numClusters");
        onOffMode.getState(prep, "onOffMode");
        
        holdMin.getState(prep, "holdMin");
        holdMax.getState(prep, "holdMax");
        
        velocityMin.getState(prep, "velocityMin");
        velocityMax.getState(prep, "velocityMax");

        prep.setProperty( ptagSynchronic_targetPatternSync, getTargetTypeSynchronicPatternSync(), 0);
        prep.setProperty( ptagSynchronic_targetBeatSync, getTargetTypeSynchronicBeatSync(), 0);
        prep.setProperty( ptagSynchronic_targetAddNotes, getTargetTypeSynchronicAddNotes(), 0);
        prep.setProperty( ptagSynchronic_targetClear, getTargetTypeSynchronicClear(), 0);
        prep.setProperty( ptagSynchronic_targetPausePlay, getTargetTypeSynchronicPausePlay(), 0);
        prep.setProperty( ptagSynchronic_targetDeleteOldest, getTargetTypeSynchronicDeleteOldest(), 0);
        prep.setProperty( ptagSynchronic_targetDeleteNewest, getTargetTypeSynchronicDeleteNewest(), 0);
        prep.setProperty( ptagSynchronic_targetRotate, getTargetTypeSynchronicRotate(), 0);
                 
        ValueTree beatMults( vtagSynchronic_beatMults);
        int count = 0;
        for (auto f : sBeatMultipliers)
        {
            f.getState(beatMults, ptagFloat + String(count++));
        }
        prep.addChild(beatMults, -1, 0);
        
        ValueTree beatMultsStates( vtagSynchronic_beatMultsStates);
        count = 0;
        for (auto f : sBeatMultipliersStates)
        {
            f.getState(beatMultsStates, ptagBool + String(count++));
        }
        prep.addChild(beatMultsStates, -1, 0);
        
        
        ValueTree lenMults( vtagSynchronic_lengthMults);
        count = 0;
        for (auto f : sLengthMultipliers)
        {
            f.getState(lenMults, ptagFloat + String(count++));
        }
        prep.addChild(lenMults, -1, 0);
        
        ValueTree lenMultsStates( vtagSynchronic_lengthMultsStates);
        count = 0;
        for (auto f : sLengthMultipliersStates)
        {
            f.getState(lenMultsStates, ptagBool + String(count++));
        }
        prep.addChild(lenMultsStates, -1, 0);
        
        
        ValueTree accentMults( vtagSynchronic_accentMults);
        count = 0;
        for (auto f : sAccentMultipliers)
        {
            f.getState(accentMults, ptagFloat + String(count++));
        }
        prep.addChild(accentMults, -1, 0);
        
        ValueTree accentMultsStates( vtagSynchronic_accentMultsStates);
        count = 0;
        for (auto f : sAccentMultipliersStates)
        {
            f.getState(accentMultsStates, ptagBool + String(count++));
        }
        prep.addChild(accentMultsStates, -1, 0);
        
        
        ValueTree transposition( vtagSynchronic_transpOffsets);
        
        int tcount = 0;
        for (auto arr : sTransposition)
        {
            ValueTree t("t"+String(tcount++));
            count = 0;
            for (auto f : arr)  f.getState(t, ptagFloat + String(count++));
            transposition.addChild(t,-1,0);
        }
        prep.addChild(transposition, -1, 0);
        
        ValueTree transpositionStates( vtagSynchronic_transpOffsetsStates);
        count = 0;
        for (auto f : sTranspositionStates)
        {
            f.getState(transpositionStates, ptagBool + String(count++));
        }
        prep.addChild(transpositionStates, -1, 0);

        
        ValueTree ADSRs( vtagSynchronic_ADSRs);
        tcount = 0;
        for (auto arr : getADSRs())
        {
            ValueTree e("e"+String(tcount++));
            count = 0;
            for (auto f : arr)  f.getState(e, ptagFloat + String(count++));
            ADSRs.addChild(e,-1,0);
        }
        prep.addChild(ADSRs, -1, 0);
        
        sUseGlobalSoundSet.getState(prep, ptagSynchronic_useGlobalSoundSet);
        sSoundSetName.getState(prep, ptagSynchronic_soundSet);
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        String n; int i; float f;
        
        sGain.setState(e, ptagSynchronic_gain, 1.0f);
        sBlendronicGain.setState(e, ptagSynchronic_blendronicGain, 1.0f);
 

//        holdMin(0),
//        holdMax(12000),
//        velocityMin(0),
//        velocityMax(127),
//        sMode(FirstNoteOnSync),
//        sBeatsToSkip(0),
//        onOffMode(KeyOn),
//        sTranspUsesTuning(false),
        
//        sClusterThreshSec(.001 * sClusterThresh.base),
//        midiOutput(nullptr),
//        sUseGlobalSoundSet(true),
//        sSoundSet(-1),
        
        sNumBeats.setState(e, ptagSynchronic_numBeats, 20);
        sClusterMin.setState(e, ptagSynchronic_clusterMin, 1);
        sClusterMax.setState(e, ptagSynchronic_clusterMax, 12);
        sClusterCap.setState(e, ptagSynchronic_clusterCap, 8);
        sClusterThresh.setState(e, ptagSynchronic_clusterThresh, 500);
        setClusterThresh(sClusterThresh.base);
        numClusters.setState(e, "numClusters", 1);
        
        holdMin.setState(e, "holdMin", 0);
        holdMax.setState(e, "holdMax", 12000);

        velocityMin.setState(e, "velocityMin", 0);
        velocityMax.setState(e, "velocityMax", 127);
        
        sMode.setState(e, ptagSynchronic_mode, FirstNoteOnSync);
        sBeatsToSkip.setState(e, ptagSynchronic_beatsToSkip, 0);
        onOffMode.setState(e, "onOffMode", KeyOn);
        
        sTranspUsesTuning.setState(e, ptagSynchronic_transpUsesTuning, false);
        sUseGlobalSoundSet.setState(e, ptagSynchronic_useGlobalSoundSet, true);
        sSoundSetName.setState(e, ptagSynchronic_soundSet, String());

        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagSynchronic_beatMults))
            {
                sBeatMultipliers.clear();
                // for pre-v.2.5.2, when we didn't save beatstates
                sBeatMultipliersStates.clear();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    Moddable<float> f;
                    f.setState(sub, ptagFloat + String(k), 1.0);
                    sBeatMultipliers.add(f);
                    // for pre-v.2.5.2, when we didn't save beatstates
                    sBeatMultipliersStates.add(Moddable<bool>(true));
                }
            }
            else if (sub->hasTagName(vtagSynchronic_beatMultsStates))
            {
                sBeatMultipliersStates.clear();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    Moddable<bool> b;
                    b.setState(sub, ptagBool + String(k), false);
                    sBeatMultipliersStates.add(b);
                }
            }
            else  if (sub->hasTagName(vtagSynchronic_accentMults))
            {
                sAccentMultipliers.clear();
                sAccentMultipliersStates.clear();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    Moddable<float> f;
                    f.setState(sub, ptagFloat + String(k), 1.0);
                    sAccentMultipliers.add(f);
                    // for pre-v.2.5.2, when we didn't save beatstates
                    sAccentMultipliersStates.add(Moddable<bool>(true));
                }
            }
            else if (sub->hasTagName(vtagSynchronic_accentMultsStates))
            {
                sAccentMultipliersStates.clear();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    Moddable<bool> b;
                    b.setState(sub, ptagBool + String(k), false);
                    sAccentMultipliersStates.add(b);
                }
            }
            else  if (sub->hasTagName(vtagSynchronic_lengthMults))
            {
                sLengthMultipliers.clear();
                sLengthMultipliersStates.clear();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    Moddable<float> f;
                    f.setState(sub, ptagFloat + String(k), 1.0);
                    sLengthMultipliers.add(f);
                    // for pre-v.2.5.2, when we didn't save beatstates
                    sLengthMultipliersStates.add(Moddable<bool>(true));
                }
            }
            else if (sub->hasTagName(vtagSynchronic_lengthMultsStates))
            {
                sLengthMultipliersStates.clear();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    Moddable<bool> b;
                    b.setState(sub, ptagBool + String(k), false);
                    sLengthMultipliersStates.add(b);
                }
            }
            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
            {
                sTransposition.clear();
                sTranspositionStates.clear();
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("t" + String(tcount++)))
                    {
                        Array<Moddable<float>> arr;
                        for (int k = 0; k < asub->getNumAttributes(); k++)
                        {
                            Moddable<float> f;
                            f.setState(asub, ptagFloat + String(k), 0.0);
                            arr.add(f);
                        }
                        sTransposition.add(arr);
                        sTranspositionStates.add(true);
                    }
                }
            }
            else if (sub->hasTagName(vtagSynchronic_transpOffsetsStates))
            {
                sTranspositionStates.clear();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    Moddable<bool> b;
                    b.setState(sub, ptagBool + String(k), false);
                    sTranspositionStates.add(b);
                }
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
                        for (int k = 0; k < asub->getNumAttributes(); k++)
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
  
        i = e->getStringAttribute(ptagSynchronic_targetPatternSync).getIntValue();
        setTargetTypeSynchronicPatternSync((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagSynchronic_targetBeatSync).getIntValue();
        setTargetTypeSynchronicBeatSync((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagSynchronic_targetAddNotes).getIntValue();
        setTargetTypeSynchronicAddNotes((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagSynchronic_targetClear).getIntValue();
        setTargetTypeSynchronicClear((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagSynchronic_targetPausePlay).getIntValue();
        setTargetTypeSynchronicPausePlay((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagSynchronic_targetDeleteOldest).getIntValue();
        setTargetTypeSynchronicDeleteOldest((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagSynchronic_targetDeleteNewest).getIntValue();
        setTargetTypeSynchronicDeleteNewest((TargetNoteMode)i);
        
        i = e->getStringAttribute(ptagSynchronic_targetRotate).getIntValue();
        setTargetTypeSynchronicRotate((TargetNoteMode)i);
    }
    
    inline void setSoundSet(int Id) { sSoundSet = Id; }
    inline int getSoundSet(void) { return sUseGlobalSoundSet.value ? -1 : sSoundSet.value; }
    
    bool modded;
    
    Moddable<float> sGain;
    Moddable<float> sBlendronicGain;
    
    Moddable<float> sTempo;
    Moddable<int> sNumBeats,sClusterMin,sClusterMax;
    Moddable<int> sClusterCap = 8; //max in cluster; 8 in original bK. called Cluster Thickness in v2.4 UI
    
    Moddable<int> numClusters;
    
    Moddable<int> holdMin, holdMax;
    Moddable<int> velocityMin, velocityMax;
    
    Moddable<SynchronicSyncMode> sMode;
    Moddable<int> sBeatsToSkip;
    Moddable<SynchronicOnOffMode> onOffMode;
    
    // arrays of step sequencer values
    Array<Moddable<float>> sBeatMultipliers;      // multiply pulse lengths by these
    Array<Moddable<float>> sAccentMultipliers;    // multiply velocities by these
    Array<Moddable<float>> sLengthMultipliers;    // multiply note duration by these
    Array<Array<Moddable<float>>> sTransposition; // transpose by these
    
    // to remember which sliders are active in the UI
    // true => slider is active, false => slider is inactive
    Array<Moddable<bool>> sBeatMultipliersStates;
    Array<Moddable<bool>> sAccentMultipliersStates;
    Array<Moddable<bool>> sLengthMultipliersStates;
    Array<Moddable<bool>> sTranspositionStates;
    
    // do the transposition values use the Tuning system, or are they relative to the main played note?
    Moddable<bool> sTranspUsesTuning;
    
    // ADSR vals
    Array<Moddable<float>> sAttacks;
    Array<Moddable<float>> sDecays;
    Array<Moddable<float>> sSustains;
    Array<Moddable<float>> sReleases;
    Array<Moddable<bool>> envelopeOn;
    
    Moddable<float> sClusterThresh;      //max time between played notes before new cluster is started, in MS
    Moddable<float> sClusterThreshSec;
    
    Moddable<bool> sReleaseVelocitySetsSynchronic;
    
    Moddable<std::shared_ptr<MidiOutput>> midiOutput;
    
    Moddable<bool> sUseGlobalSoundSet;
    Moddable<int> sSoundSet;
    Moddable<String> sSoundSetName;
    
private:
    String name;
    
    // stores what kind of note event triggers targeted events (as set in Keymap)
    TargetNoteMode targetTypeSynchronicPatternSync;
    TargetNoteMode targetTypeSynchronicBeatSync;
    TargetNoteMode targetTypeSynchronicAddNotes;
    TargetNoteMode targetTypeSynchronicPausePlay;
    TargetNoteMode targetTypeSynchronicClear;
    TargetNoteMode targetTypeSynchronicDeleteOldest;
    TargetNoteMode targetTypeSynchronicDeleteNewest;
    TargetNoteMode targetTypeSynchronicRotate;

    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};


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
    prep(new SynchronicPreparation(prep)),
    Id(Id),
    name("Synchronic "+String(Id))
    {
        
    }
    
	Synchronic(int Id, bool random = false) :
    Id(Id),
    name("Synchronic "+String(Id))
    {
		prep = new SynchronicPreparation();
		if (random) randomize();
    }
    
    inline Synchronic::Ptr duplicate()
    {
        SynchronicPreparation::Ptr copyPrep = new SynchronicPreparation(prep);
        
        Synchronic::Ptr copy = new Synchronic(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    inline void clear(void)
    {
        prep       = new SynchronicPreparation();
    }

    inline void copy(Synchronic::Ptr from)
    {
        prep->copy(from->prep);
    }

	inline void randomize()
	{
		clear();
		prep->randomize();
		Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
		name = "random";
	}

    inline ValueTree getState(bool active = false)
    {
        ValueTree vt(vtagSynchronic);
        
        vt.setProperty( "Id",Id, 0);
        vt.setProperty( "name", name, 0);
        
        vt.addChild(prep->getState(), -1, 0);
        
        return vt;
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
            prep->setState(params);
        }
        else
        {
            prep->setState(e);
        }
    }
    
    ~Synchronic() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    SynchronicPreparation::Ptr      prep;

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
        int tempsize = prep->sLengthMultipliers.size();
        //int counter = getLengthMultiplierCounter() - 1;
        int counter = getLengthMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getBeatMultiplierCounterForDisplay()
    {
        int tempsize = prep->sBeatMultipliers.size();
        int counter = getBeatMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getAccentMultiplierCounterForDisplay()
    {
        int tempsize = prep->sAccentMultipliers.size();
        int counter = getAccentMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getTranspCounterForDisplay()
    {
        int tempsize = prep->sTransposition.size();
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
        if (++lengthMultiplierCounter   >= prep->sLengthMultipliers.size())     lengthMultiplierCounter = 0;
        if (++accentMultiplierCounter   >= prep->sAccentMultipliers.size())     accentMultiplierCounter = 0;
        if (++transpCounter             >= prep->sTransposition.size())         transpCounter = 0;
        if (++envelopeCounter           >= prep->envelopeOn.size())             envelopeCounter = 0;
        
        while(!prep->envelopeOn[envelopeCounter].value) //skip untoggled envelopes
        {
            envelopeCounter++;
            if (envelopeCounter >= prep->envelopeOn.size()) envelopeCounter = 0;
        }
        
    }
    
    inline void postStep ()
    {
    
        if (++beatMultiplierCounter >= prep->sBeatMultipliers.size())
        {
            //increment beat and beatMultiplier counters, for next beat; check maxes and adjust
            beatMultiplierCounter = 0;
        }
        
        if (++beatCounter >= (prep->sNumBeats.value + prep->sBeatsToSkip.value))
        {
            shouldPlay = false;
        }
    }
    
    inline void resetPatternPhase()
    {
        int skipBeats = prep->sBeatsToSkip.value - 1;
        int idx = (skipBeats < -1) ? -1 : skipBeats;
        
        if(prep->sBeatMultipliers.size() > 0)
            beatMultiplierCounter = mod(idx, prep->sBeatMultipliers.size());
        if(prep->sLengthMultipliers.size() > 0)
            lengthMultiplierCounter = mod(idx, prep->sLengthMultipliers.size());
        if(prep->sAccentMultipliers.size() > 0)
            accentMultiplierCounter = mod(idx, prep->sAccentMultipliers.size());
        if(prep->sTransposition.size() > 0)
            transpCounter = mod(idx, prep->sTransposition.size());
        if(prep->envelopeOn.size() > 0)
            envelopeCounter = mod(idx, prep->envelopeOn.size());

        // DBG("beatMultiplierCounter = " + String(beatMultiplierCounter));

        beatCounter             = 0;
    }
    
    inline Array<int> getCluster() {return cluster;}
    
    inline void setCluster(Array<int> c) { cluster = c; }
    
    inline void addNote(int note)
    {
        // DBG("adding note: " + String(note));
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
    
    inline const uint64 getCurrentNumSamplesBeat(void) const noexcept   { return numSamplesBeat;    }
    
    
    BKSampleLoadType sampleType;
    void processBlock(int numSamples, int midiChannel, BKSampleLoadType type);
    void keyPressed(int noteNumber, float velocity, Array<KeymapTargetState> targetStates);
    void keyReleased(int noteNumber, float velocity, int channel, Array<KeymapTargetState> targetStates);
    float getTimeToBeatMS(float beatsToSkip);
    
    
    inline const float getClusterThresholdTimer() const noexcept { return 1000. * clusterThresholdTimer / synth->getSampleRate() ;}
    inline const float getClusterThreshold() const noexcept { return 1000. * thresholdSamples / synth->getSampleRate() ;}
    inline const int getNumKeysDepressed() const noexcept {return keysDepressed.size(); }
    inline const bool getPlayCluster() const noexcept { return playCluster; }
    
    inline const float getHoldTimer() const noexcept
    {
        // if(keysDepressed.size() == 0 ) return 0;
        return 1000. * holdTimers[lastKeyPressed] / synth->getSampleRate() ;
    }
    
    inline const float getLastVelocity() const noexcept
    {
        // if(keysDepressed.size() == 0 ) return 0;
        return lastKeyVelocity;
    }
    
    inline const SynchronicSyncMode getMode() const noexcept {return synchronic->prep->sMode.value; }

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
    }
    //void  atReset();
    
    inline void reset(void)
    {
        synchronic->prep->resetModdables();
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
    
    void setClusters(SynchronicCluster::PtrArr nclusters)
    {
        clusters = nclusters;
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
    
    // Trying out graph indication for synchronic, bit crude for now
    inline bool noteDidPlay(void)
    {
        if (notePlayed)
        {
            notePlayed = false;
            return true;
        }
        return notePlayed;
    }
    
private:
    BKSynthesiser* synth;
    GeneralSettings::Ptr general;
    
    Synchronic::Ptr synchronic;
    TuningProcessor::Ptr tuner;
    TempoProcessor::Ptr tempo;
	BlendronicProcessor::PtrArr blendronic;
    
    Keymap::PtrArr      keymaps;
        
    Array<float> tuningOffsets;
    PitchClass tuningBasePitch;
    
    void playNote(int channel, int note, float velocity, SynchronicCluster::Ptr cluster);
    Array<float> velocities;    //record of velocities
    Array<float> velocitiesActive;
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
    
    bool notePlayed;
    
    BKSynthesiserVoice** asv;
    Array<BKSynthesiserVoice*> activeSynchronicVoices;

    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};


#endif  // SYNCHRONIC_H_INCLUDED
