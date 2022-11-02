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
#include "BKPianoSampler.h"
#include "GenericProcessor.h"
class SynchronicModification;

class SynchronicPreparation : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicPreparation>   Ptr;
    typedef Array<SynchronicPreparation::Ptr>                  PtrArr;
    typedef Array<SynchronicPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicPreparation>                  Arr;
    typedef OwnedArray<SynchronicPreparation, CriticalSection> CSArr;
    
    // Copy Constructor
    SynchronicPreparation(SynchronicPreparation::Ptr s) :
    sGain(s->sGain),
    sBlendronicGain(s->sBlendronicGain),
    sTempo(s->sTempo),
    sNumBeats(s->sNumBeats),
    sClusterMin(s->sClusterMin),
    sClusterMax(s->sClusterMax),
    sClusterCap(s->sClusterCap),
    numClusters(s->numClusters),
    holdMin(s->holdMin),
    holdMax(s->holdMax),
    velocityMin(s->velocityMin),
    velocityMax(s->velocityMax),
    sMode(s->sMode),
    sBeatsToSkip(s->sBeatsToSkip),
    onOffMode(s->onOffMode),
    sBeatMultipliers(s->sBeatMultipliers),
    sAccentMultipliers(s->sAccentMultipliers),
    sLengthMultipliers(s->sLengthMultipliers),
    sTransposition(s->sTransposition),
    sBeatMultipliersStates(s->sBeatMultipliersStates),
    sAccentMultipliersStates(s->sAccentMultipliersStates),
    sLengthMultipliersStates(s->sLengthMultipliersStates),
    sTranspositionStates(s->sTranspositionStates),
    sTranspUsesTuning(s->sTranspUsesTuning),
    sADSRs(s->sADSRs),
    sClusterThresh(s->sClusterThresh),
    sClusterThreshSec(s->sClusterThreshSec),
    sReleaseVelocitySetsSynchronic(s->sReleaseVelocitySetsSynchronic),
    midiOutput(s->midiOutput),
    sUseGlobalSoundSet(s->sUseGlobalSoundSet),
    sSoundSet(s->sSoundSet),
    sSoundSetName(s->sSoundSetName),
    targetTypeSynchronicPatternSync(s->getTargetTypeSynchronicPatternSync()),
    targetTypeSynchronicBeatSync(s->getTargetTypeSynchronicBeatSync()),
    targetTypeSynchronicAddNotes(s->getTargetTypeSynchronicAddNotes()),
    targetTypeSynchronicPausePlay(s->getTargetTypeSynchronicPausePlay()),
    targetTypeSynchronicClear(s->getTargetTypeSynchronicClear()),
    targetTypeSynchronicDeleteOldest(s->getTargetTypeSynchronicDeleteOldest()),
    targetTypeSynchronicDeleteNewest(s->getTargetTypeSynchronicDeleteNewest()),
    targetTypeSynchronicRotate(s->getTargetTypeSynchronicRotate())
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
    sGain(1.0f),
    sBlendronicGain(1.0f),
    sTempo(120),
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
    sBeatMultipliers(beatMultipliers),
    sAccentMultipliers(accentMultipliers),
    sLengthMultipliers(lengthMultipliers),
    sTransposition(transp),
    sBeatMultipliersStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    sAccentMultipliersStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    sLengthMultipliersStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    sTranspositionStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    sTranspUsesTuning(false),
    sADSRs(Array<Array<float>>(0.0f)),
    sClusterThresh(clusterThresh),
    sClusterThreshSec(.001 * sClusterThresh.base),
    sReleaseVelocitySetsSynchronic(velocityMode),
    midiOutput(nullptr),
    sUseGlobalSoundSet(true),
    sSoundSet(-1),
    sSoundSetName(String()),
    targetTypeSynchronicPatternSync(NoteOn),
    targetTypeSynchronicBeatSync(NoteOn),
    targetTypeSynchronicAddNotes(NoteOn),
    targetTypeSynchronicPausePlay(NoteOn),
    targetTypeSynchronicClear(NoteOn),
    targetTypeSynchronicDeleteOldest(NoteOn),
    targetTypeSynchronicDeleteNewest(NoteOn),
    targetTypeSynchronicRotate(NoteOn)
    {
        Array<Array<float>> aa;
        for (int i = 0; i < 12; ++i)
        {
            // A D S R active
            Array<float> a(3, 3, 1, 30, float(i == 0));
            aa.add(a);
        }
        sADSRs.set(aa);
    }

    
    SynchronicPreparation(void):
    sGain(0.0, true),
    sBlendronicGain(0.0, true),
    sTempo(120.f),
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
    sBeatMultipliers(Array<float>({1.0})),
    sAccentMultipliers(Array<float>({1.0})),
    sLengthMultipliers(Array<float>({1.0})),
    sTransposition(Array<Array<float>>(0.0f)),
    sBeatMultipliersStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    sAccentMultipliersStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    sLengthMultipliersStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    sTranspositionStates(Array<bool>({true, false, false, false, false, false, false, false, false, false, false, false, })),
    sTranspUsesTuning(false),
    sADSRs(Array<Array<float>>(0.0f)),
    sClusterThresh(500),
    sClusterThreshSec(.001 * sClusterThresh.base),
    sReleaseVelocitySetsSynchronic(false),
    midiOutput(nullptr),
    sUseGlobalSoundSet(true),
    sSoundSet(-1),
    sSoundSetName(String()),
    targetTypeSynchronicPatternSync(NoteOn),
    targetTypeSynchronicBeatSync(NoteOn),
    targetTypeSynchronicAddNotes(NoteOn),
    targetTypeSynchronicPausePlay(NoteOn),
    targetTypeSynchronicClear(NoteOn),
    targetTypeSynchronicDeleteOldest(NoteOn),
    targetTypeSynchronicDeleteNewest(NoteOn),
    targetTypeSynchronicRotate(NoteOn)
    {
        Array<Array<float>> aa;
        for (int i = 0; i < 12; ++i)
        {
            // A D S R active
            Array<float> a(3, 3, 1, 30, float(i == 0));
            aa.add(a);
        }
        sADSRs.set(aa);
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
        
        sADSRs = s->sADSRs;
        
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
    
    void performModification(SynchronicModification* s, Array<bool> dirty);
   
    void stepModdables()
    {
        sGain.step();
        sBlendronicGain.step();
        
        sTempo.step();
        sNumBeats.step();
        sClusterMin.step();
        sClusterMax.step();
        sClusterCap.step();
        
        numClusters.step();
        
        holdMin.step();
        holdMax.step();
        velocityMin.step();
        velocityMax.step();
        
        sMode.step();
        sBeatsToSkip.step();
        onOffMode.step();
        
        sBeatMultipliers.step();
        sAccentMultipliers.step();
        sLengthMultipliers.step();
        sTransposition.step();
        
        sBeatMultipliersStates.step();
        sAccentMultipliersStates.step();
        sLengthMultipliersStates.step();
        sTranspositionStates.step();
        
        sTranspUsesTuning.step();
        
        sADSRs.step();
        
        sClusterThresh.step();
        sClusterThreshSec.step();
        
        sReleaseVelocitySetsSynchronic.step();
        
        midiOutput.step();
        
        sUseGlobalSoundSet.step();
        sSoundSet.step();
        sSoundSetName.step();
    }
    
    void resetModdables()
    {
        sGain.reset();
        sBlendronicGain.reset();
        
        sTempo.reset();
        sNumBeats.reset();
        sClusterMin.reset();
        sClusterMax.reset();
        sClusterCap.reset();
        
        numClusters.reset();
        
        holdMin.reset();
        holdMax.reset();
        velocityMin.reset();
        velocityMax.reset();
        
        sMode.reset();
        sBeatsToSkip.reset();
        onOffMode.reset();
        
        sBeatMultipliers.reset();
        sAccentMultipliers.reset();
        sLengthMultipliers.reset();
        sTransposition.reset();
        
        sBeatMultipliersStates.reset();
        sAccentMultipliersStates.reset();
        sLengthMultipliersStates.reset();
        sTranspositionStates.reset();
        
        sTranspUsesTuning.reset();
        
        sADSRs.reset();
        
        sClusterThresh.reset();
        sClusterThreshSec.reset();
        
        sReleaseVelocitySetsSynchronic.reset();
        
        midiOutput.reset();
        
        sUseGlobalSoundSet.reset();
        sSoundSet.reset();
        sSoundSetName.reset();
    }
    
    bool compare(SynchronicPreparation::Ptr s)
    {
        bool lens = true;
        bool accents = true;
        bool beats = true;
        bool transp = true;
        bool adsr = true;
        bool lensStates = true;
        bool accentsStates = true;
        bool beatsStates = true;
        bool transpStates = true;
        
        for (int i = s->sLengthMultipliers.value.size(); --i>=0;)
        {
            if (s->sLengthMultipliers.value[i] != sLengthMultipliers.value[i])
            {
                lens = false;
                break;
            }
        }
        
        for (int i = s->sAccentMultipliers.value.size(); --i>=0;)
        {
            if (s->sAccentMultipliers.value[i] != sAccentMultipliers.value[i])
            {
                accents = false;
                break;
            }
        }
        
        for (int i = s->sBeatMultipliers.value.size(); --i>=0;)
        {
            if (s->sBeatMultipliers.value[i] != sBeatMultipliers.value[i])
            {
                beats = false;
                break;
            }
        }
        
        for (int i  = s->sTransposition.value.size(); --i >= 0;)
        {
            Array<float> transposition = s->sTransposition.value[i];
            for (int j = transposition.size(); --j >= 0;)
            {
                if (transposition[j] != sTransposition.value[i][j])
                {
                    transp = false;
                    break;
                }
            }
        }
        
        for (int i = s->sLengthMultipliersStates.value.size(); --i>=0;)
        {
            if (s->sLengthMultipliersStates.value[i] != sLengthMultipliersStates.value[i])
            {
                lensStates = false;
                break;
            }
        }
        
        for (int i = s->sAccentMultipliersStates.value.size(); --i>=0;)
        {
            if (s->sAccentMultipliersStates.value[i] != sAccentMultipliersStates.value[i])
            {
                accentsStates = false;
                break;
            }
        }
        
        for (int i = s->sBeatMultipliersStates.value.size(); --i>=0;)
        {
            if (s->sBeatMultipliersStates.value[i] != sBeatMultipliersStates.value[i])
            {
                beatsStates = false;
                break;
            }
        }
        
        for (int i = s->sTranspositionStates.value.size(); --i>=0;)
        {
            if (s->sTranspositionStates.value[i] != sTranspositionStates.value[i])
            {
                transpStates = false;
                break;
            }
        }
        
        for (int i  = s->sADSRs.value.size(); --i >= 0;)
        {
            Array<float> adsr = s->sADSRs.value[i];
            for (int j = adsr.size(); --j >= 0;)
            {
                if (adsr[j] != sADSRs.value[i][j])
                {
                    adsr = false;
                    break;
                }
            }
        }
        
        return (sNumBeats == s->sNumBeats &&
                sClusterMin == s->sClusterMin &&
                sClusterMax == s->sClusterMax &&
                sClusterCap == s->sClusterCap &&
                sMode == s->sMode &&
                transp && lens && accents && beats && adsr &&
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
        
        Array<float> fa;
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			fa.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
		}
        sBeatMultipliers.set(fa);
        
        fa.clear();
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			fa.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
		}
        sAccentMultipliers.set(fa);
        
        fa.clear();
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			fa.add(i, (Random::getSystemRandom().nextFloat() * 4.0f - 2.0f));
		}
        sLengthMultipliers.set(fa);
        
		sGain = r[idx++] * 10;
        sBlendronicGain = r[idx++] * 2;
        
        Array<Array<float>> faa;
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			Array<float> transposition;
			for (int j = 0; j < Random::getSystemRandom().nextInt(10); j++)
			{
				transposition.add(i, (Random::getSystemRandom().nextFloat()) * 48.0f - 24.0f);
			}
			faa.add(transposition);
		}
        sTransposition.set(faa);
        
        sClusterThresh = (r[idx++] * 2000) + 1;
		sClusterThreshSec = sClusterThresh.value * 0.001f;
		sReleaseVelocitySetsSynchronic = (bool)((int)(r[idx++] * 2));

		int numEnvelopes = 12;

        faa.clear();
        for (int i = 0; i < numEnvelopes; ++i)
        {
            Array<float> adsr;
            adsr.add(Random::getSystemRandom().nextInt(Range<int>(1, 1000)));
            adsr.add(Random::getSystemRandom().nextInt(Range<int>(1, 1000)));
            adsr.add(Random::getSystemRandom().nextFloat());
            adsr.add(Random::getSystemRandom().nextInt(Range<int>(1, 1000)));
            if (i == 0) adsr.add(1);
            else adsr.add(Random::getSystemRandom().nextInt(Range<int>(0, 1)));
            faa.add(adsr);
        }
        sADSRs.set(faa);
	}
    
    inline const int getOffsetParamToggle() const noexcept
    {
        if(sMode == FirstNoteOnSync || sMode == AnyNoteOnSync) return sBeatsToSkip.value + 1;
        else return sBeatsToSkip.value;
    }
    
    inline float* getGainPtr() { return &sGain.value; }
    inline float* getBlendronicGainPtr() { return &sBlendronicGain.value; }
    
    inline const int getAttack(int which) const noexcept    {return sADSRs.value[which][0];}
    inline const int getDecay(int which) const noexcept     {return sADSRs.value[which][1];}
    inline const float getSustain(int which) const noexcept {return sADSRs.value[which][2];}
    inline const int getRelease(int which) const noexcept   {return sADSRs.value[which][3];}
    inline const bool getEnvelopeOn(int which) const noexcept   {return sADSRs.value[which][4] > 0;}
    
    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = sClusterThresh.base * .001;
    }
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}

    inline void clearADSRs()
    {
        sADSRs.set(Array<Array<float>>());
    }
    
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
    
    inline const void setMidiOutput(String identifier)
    {
        midiOutput = MidiOutput::openDevice(identifier);
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
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers.value));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers.value));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers.value));
        
        String s = "";
        for (auto arr : sTransposition.value) s += "{ " + floatArrayToString(arr) + " },\n";
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
                 
        sBeatMultipliers.getState(prep, StringArray(vtagSynchronic_beatMults, ptagFloat));
        sBeatMultipliersStates.getState(prep, StringArray(vtagSynchronic_beatMultsStates, ptagBool));
        
        sLengthMultipliers.getState(prep, StringArray(vtagSynchronic_lengthMults, ptagFloat));
        sLengthMultipliersStates.getState(prep, StringArray(vtagSynchronic_lengthMultsStates, ptagBool));
        
        sAccentMultipliers.getState(prep, StringArray(vtagSynchronic_accentMults, ptagFloat));
        sAccentMultipliersStates.getState(prep, StringArray(vtagSynchronic_accentMultsStates, ptagBool));
        
        sTransposition.getState(prep, StringArray(vtagSynchronic_transpOffsets, "t", ptagFloat));
        sTranspositionStates.getState(prep, StringArray(vtagSynchronic_transpOffsetsStates, ptagBool));

        sADSRs.getState(prep, StringArray(vtagSynchronic_ADSRs, "e", ptagFloat));
        
        sUseGlobalSoundSet.getState(prep, ptagSynchronic_useGlobalSoundSet);
        sSoundSetName.getState(prep, ptagSynchronic_soundSet);
        
        if (midiOutput.value != nullptr)
            prep.setProperty( ptagSynchronic_midiOutput, midiOutput.value->getIdentifier(), 0);
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        
        sGain.setState(e, ptagSynchronic_gain, 1.0f);
        sBlendronicGain.setState(e, ptagSynchronic_blendronicGain, 1.0f);
        
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

        sBeatMultipliers.setState(e, StringArray(vtagSynchronic_beatMults, ptagFloat), 1.0);
        // for pre-v.2.5.2, when we didn't save beatstates
        Array<bool> states;
        for (int i = 0; i < sBeatMultipliers.base.size(); ++i) states.add(true);
        sBeatMultipliersStates.setState(e, StringArray(vtagSynchronic_beatMultsStates, ptagBool), states);
        
        sAccentMultipliers.setState(e, StringArray(vtagSynchronic_accentMults, ptagFloat), 1.0);
        states.clear();
        for (int i = 0; i < sAccentMultipliers.base.size(); ++i) states.add(true);
        sAccentMultipliersStates.setState(e, StringArray(vtagSynchronic_accentMultsStates, ptagBool),states);
        
        sLengthMultipliers.setState(e, StringArray(vtagSynchronic_lengthMults, ptagFloat), 1.0);
        states.clear();
        for (int i = 0; i < sLengthMultipliers.base.size(); ++i) states.add(true);
        sLengthMultipliersStates.setState(e, StringArray(vtagSynchronic_lengthMultsStates, ptagBool), states);
        
        sTransposition.setState(e, StringArray(vtagSynchronic_transpOffsets, "t", ptagFloat), Array<Array<float>>(0.0f));
        states.clear();
        for (int i = 0; i < sTransposition.base.size(); ++i) states.add(true);
        sTranspositionStates.setState(e, StringArray(vtagSynchronic_transpOffsetsStates, ptagBool), states);
        
        Array<Array<float>> aa;
        for (int i = 0; i < 12; ++i)
        {
            // A D S R active
            Array<float> a(3, 3, 1, 30, i == 0);
            aa.add(a);
        }
        sADSRs.setState(e, StringArray(vtagSynchronic_ADSRs, "e", ptagFloat), aa);
        
        String s = e->getStringAttribute(ptagSynchronic_midiOutput);
        if (s != String()) setMidiOutput(s);
  
        int i;
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
    
    bool modded = false;
    
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
    Moddable<Array<float>> sBeatMultipliers;      // multiply pulse lengths by these
    Moddable<Array<float>> sAccentMultipliers;    // multiply velocities by these
    Moddable<Array<float>> sLengthMultipliers;    // multiply note duration by these
    Moddable<Array<Array<float>>> sTransposition; // transpose by these
    
    // to remember which sliders are active in the UI
    // true => slider is active, false => slider is inactive
    Moddable<Array<bool>> sBeatMultipliersStates;
    Moddable<Array<bool>> sAccentMultipliersStates;
    Moddable<Array<bool>> sLengthMultipliersStates;
    Moddable<Array<bool>> sTranspositionStates;
    
    // do the transposition values use the Tuning system, or are they relative to the main played note?
    Moddable<bool> sTranspUsesTuning;
    
    Moddable<Array<Array<float>>> sADSRs;
    
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
        int tempsize = prep->sLengthMultipliers.value.size();
        //int counter = getLengthMultiplierCounter() - 1;
        int counter = getLengthMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getBeatMultiplierCounterForDisplay()
    {
        int tempsize = prep->sBeatMultipliers.value.size();
        int counter = getBeatMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getAccentMultiplierCounterForDisplay()
    {
        int tempsize = prep->sAccentMultipliers.value.size();
        int counter = getAccentMultiplierCounter() ;
        
        if(counter < 0) counter = tempsize - 1;
        if(counter >= tempsize) counter = 0;
        
        return counter;
    }
    
    int getTranspCounterForDisplay()
    {
        int tempsize = prep->sTransposition.value.size();
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
        if (++lengthMultiplierCounter   >= prep->sLengthMultipliers.value.size())     lengthMultiplierCounter = 0;
        if (++accentMultiplierCounter   >= prep->sAccentMultipliers.value.size())     accentMultiplierCounter = 0;
        if (++transpCounter             >= prep->sTransposition.value.size())         transpCounter = 0;
        if (++envelopeCounter           >= prep->sADSRs.value.size())             envelopeCounter = 0;
        
        while(prep->sADSRs.value[envelopeCounter][4] == 0) //skip untoggled envelopes
        {
            envelopeCounter++;
            if (envelopeCounter >= prep->sADSRs.value.size()) envelopeCounter = 0;
        }
    }
    
    inline void postStep ()
    {
        if (++beatMultiplierCounter >= prep->sBeatMultipliers.value.size())
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
        
        if(prep->sBeatMultipliers.value.size() > 0)
            beatMultiplierCounter = mod(idx, prep->sBeatMultipliers.value.size());
        if(prep->sLengthMultipliers.value.size() > 0)
            lengthMultiplierCounter = mod(idx, prep->sLengthMultipliers.value.size());
        if(prep->sAccentMultipliers.value.size() > 0)
            accentMultiplierCounter = mod(idx, prep->sAccentMultipliers.value.size());
        if(prep->sTransposition.value.size() > 0)
            transpCounter = mod(idx, prep->sTransposition.value.size());
        if(prep->sADSRs.value.size() > 0)
            envelopeCounter = mod(idx, prep->sADSRs.value.size());

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

class SynchronicProcessor  : public GenericProcessor
{
    
public:
//    typedef ReferenceCountedObjectPtr<SynchronicProcessor>   Ptr;
//    typedef Array<SynchronicProcessor::Ptr>                  PtrArr;
//    typedef Array<SynchronicProcessor::Ptr, CriticalSection> CSPtrArr;
//    typedef OwnedArray<SynchronicProcessor>                  Arr;
//    typedef OwnedArray<SynchronicProcessor,CriticalSection>  CSArr;
    
    
    SynchronicProcessor(Synchronic::Ptr synchronic,
                        TuningProcessor::Ptr tuning,
                        TempoProcessor::Ptr tempo,
                        BKAudioProcessor &processor,
                        //BKSynthesiser* main,
                        GeneralSettings::Ptr general);
    
    ~SynchronicProcessor();
    
    inline const uint64 getNumSamplesBeat(void) const noexcept { return numSamplesBeat;    }
    
    BKSampleLoadType sampleType;
    void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages, int numSamples, int midiChannel, BKSampleLoadType type);
    void keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress);
    void keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress);
    float getTimeToBeatMS(float beatsToSkip);
    
    
    inline const float getClusterThresholdTimer() const noexcept { return 1000. * clusterThresholdTimer / synth->getSampleRate() ;}
    inline const float getClusterThreshold() const noexcept { return 1000. * thresholdSamples / synth->getSampleRate() ;}
    inline const int getNumKeysDepressed() const noexcept {return keysDepressed.size(); }
    inline const bool getPlayCluster() const noexcept { return playCluster; }
    
    inline const float getHoldTimer() const noexcept
    {
        return 1000. * holdTimers[lastKeyPressed] / synth->getSampleRate() ;
    }
    
    inline const float getLastVelocity() const noexcept
    {
        return lastVelocity;
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

    inline int getTuningId(void) const noexcept
    {
        return tuner->getId();
    }
    
    inline int getTempoId(void) const noexcept
    {
        return tempo->getId();
    }

    
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
    
    float filterVelocity(float vel);
    void resetLastVelocity() { lastVelocityInRange = false; }
    
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
    
    Array<Array<float>>& getVelocities() { return velocities; }
    Array<Array<float>>& getInvertVelocities() { return invertVelocities; }
    Array<float>& getClusterVelocities() { return clusterVelocities; }
    
    void setVelocities(Array<Array<float>>& newVel) { velocities = newVel; }
    void setInvertVelocities(Array<Array<float>>& newVel) { invertVelocities = newVel; }
    void swapClusterVelocities(Array<float>& swap) { clusterVelocities.swapWith(swap); }
    void handleMidiEvent (const MidiMessage& m)
    {
        synth->handleMidiEvent(m);
    }
    inline void prepareToPlay(GeneralSettings::Ptr gen)
    {
        synth->playbackSampleRateChanged();
        synth->setGeneralSettings(gen);
        
        synth->clearVoices();
        
        for (int i = 0; i < 300; i++)
        {
            synth->addVoice(new BKPianoSamplerVoice(gen));
        }
        DBG("syncrhonic preparetoplay");
       
    }
    
    inline void allNotesOff()
    {
        for(int i = 0; i < 15; i++)
        {
           synth->allNotesOff(i,true);
        }
    }
    void copyProcessorState(GenericProcessor::Ptr copy)
    {
        GenericProcessor::copyProcessorState(copy);
        setClusters(dynamic_cast<SynchronicProcessor*>(copy.get())->getClusters());
        //setVelocities(copy->getVelocities());
        //setInvertVelocities(copy->getInvertVelocities());
        swapClusterVelocities(dynamic_cast<SynchronicProcessor*>(copy.get())->getClusterVelocities());
    }
private:
    BKSynthesiser::Ptr synth;
    GeneralSettings::Ptr general;
    
    Synchronic::Ptr synchronic;
    TuningProcessor::Ptr tuner;
    TempoProcessor::Ptr tempo;
    
    Keymap::PtrArr      keymaps;
        
    Array<float> tuningOffsets;
    PitchClass tuningBasePitch;
    
    void playNote(int channel, int note, float velocity, SynchronicCluster::Ptr cluster);
    
    Array<Array<float>> velocities;
    Array<Array<float>> invertVelocities;
    Array<float> clusterVelocities;
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

    uint64 numSamplesBeat = 0;          // = beatThresholdSamples * beatMultiplier
    uint64 beatThresholdSamples;    // # samples in a beat, as set by tempo
    
    Array<uint64> holdTimers;
    int lastKeyPressed;
    float lastVelocity = 0.f;
    bool lastVelocityInRange = false;
    
    bool notePlayed;
    
    BKSynthesiserVoice** asv;
    Array<BKSynthesiserVoice*> activeSynchronicVoices;
    Array<int> voiceMidiValues;

    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};


#endif  // SYNCHRONIC_H_INCLUDED
