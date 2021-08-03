/*
  ==============================================================================

    Direct.cpp
    Created: 6 Dec 2016 12:46:37pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Direct.h"
#include "PluginProcessor.h"
#include "Modification.h"

void DirectPreparation::performModification(DirectModification* d, Array<bool> dirty)
{
    // Should the mod be reversed?
    bool reverse = d->altMod && modded;
    
    if (dirty[DirectGain]) dGain.modify(d->dGain, reverse);
    if (dirty[DirectResGain]) dResonanceGain.modify(d->dResonanceGain, reverse);
    if (dirty[DirectHammerGain]) dHammerGain.modify(d->dHammerGain, reverse);
    if (dirty[DirectBlendronicGain]) dBlendronicGain.modify(d->dBlendronicGain, reverse);
    if (dirty[DirectTransposition]) dTransposition.modify(d->dTransposition, reverse);
    if (dirty[DirectTranspUsesTuning]) dTranspUsesTuning.modify(d->dTranspUsesTuning, reverse);
    if (dirty[DirectADSR])
    {
        dAttack.modify(d->dAttack, reverse);
        dDecay.modify(d->dDecay, reverse);
        dSustain.modify(d->dSustain, reverse);
        dRelease.modify(d->dRelease, reverse);
    }
    if (dirty[DirectUseGlobalSoundSet]) dUseGlobalSoundSet.modify(d->dUseGlobalSoundSet, reverse);
    if (dirty[DirectSoundSet])
    {
        dSoundSet.modify(d->dSoundSet, reverse);
        dSoundSetName.modify(d->dSoundSetName, reverse);
    }
    if (dirty[DirectVelocityMin]) velocityMin.modify(d->velocityMin, reverse);
    if (dirty[DirectVelocityMax]) velocityMax.modify(d->velocityMax, reverse);
    
    // If the mod didn't reverse, then it is modded
    modded = !reverse;
}

DirectProcessor::DirectProcessor(Direct::Ptr direct,
                                 TuningProcessor::Ptr tuning,
                                 BlendronicProcessor::PtrArr blend,
                                 BKSynthesiser *s,
                                 BKSynthesiser *res,
                                 BKSynthesiser *ham):
synth(s),
resonanceSynth(res),
hammerSynth(ham),
direct(direct),
tuner(tuning),
blendronic(blend),
keymaps(Keymap::PtrArr())
{
    // Only one Direct target right now, but will use structure for multiple in case we add more
    for (int j = 0; j < 128; j++)
    {
        velocities.add(Array<float>());
        invertVelocities.add(Array<float>());
        for (int i = 0; i < TargetTypeNil; ++i)
        {
            velocities.getReference(j).add(-1.f);
            invertVelocities.getReference(j).add(-1.f);
        }
    }
         
    
    if (!direct->prep->dUseGlobalSoundSet.value)
    {
        String name = direct->prep->dSoundSetName.value;
        int Id = synth->processor.findPathAndLoadSamples(name);
        direct->prep->setSoundSet(Id);
    }
}

DirectProcessor::~DirectProcessor(void)
{
    
}

void DirectProcessor::keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an actual key press (not an inverted release) aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &velocities.getReference(noteNumber);
        for (int i = TargetTypeDirect; i < TargetTypeDirect+1; ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i));
        }
    }
    // If this an inverted release, aVels will be the incoming velocities,
    // but bVels will use the values from the last inverted press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = &invertVelocities.getReference(noteNumber);
    }
    
    if (bVels->getUnchecked(TargetTypeDirect) < 0.f) return;
    
    tuner->getOffset(noteNumber, true);
    
    DirectPreparation::Ptr prep = direct->prep;
    
    for (auto t : prep->dTransposition.value)
    {
        // synthNoteNumber is what determines what sample is chosen
        // without transposition or extreme tuning, this will be the same as noteNumber
        // we need to keep track of both, for noteOff messages, since with transpositions
        // we may have multiple synthNotes associated with one key/noteNumber
        int synthNoteNumber = noteNumber;
        
        float offset; // offset from integer, but may be greater than 1
        float synthOffset; // offset from actual sample played, always less than 1.
        
        // tune the transposition
        if (prep->dTranspUsesTuning.value) // use the Tuning setting
            offset = t + tuner->getOffset(round(t) + noteNumber, false);
        else  // or set it absolutely, tuning only the note that is played (default, and original behavior)
            offset = t + tuner->getOffset(noteNumber, false);
        
        synthOffset = offset;
        
        // if offset is greater than 1, we change the synthNoteNumber and reset the offSet accordingly
        synthNoteNumber += (int)offset;
        synthOffset     -= (int)offset;
        
        // blendronic stuff, or not
		if (!blendronic.isEmpty())
		{
            for (auto b : blendronic)
            {
                b->setClearDelayOnNextBeat(false);
            }
            synth->keyOn(1,
                         noteNumber,
                         synthNoteNumber,
                         synthOffset,
                         aVels->getUnchecked(TargetTypeDirect),
                         aGlobalGain,
                         Forward,
                         Normal,
                         MainNote,
                         direct->prep->getSoundSet(), //set
                         direct->getId(),
                         0,     // start
                         0,     // length
                         prep->dAttack.value,
                         prep->dDecay.value,
                         prep->dSustain.value,
                         prep->dRelease.value,
                         tuner,
                         prep->getGainPtr(),
                         prep->getBlendronicGainPtr(),
                         blendronic);
		}
		else
		{
			synth->keyOn(1,
                         noteNumber,
                         synthNoteNumber,
                         synthOffset,
                         aVels->getUnchecked(TargetTypeDirect),
                         aGlobalGain,
                         Forward,
                         Normal,
                         MainNote,
                         prep->getSoundSet(), //set
                         direct->getId(),
                         0,     // start
                         0,     // length
                         prep->dAttack.value,
                         prep->dDecay.value,
                         prep->dSustain.value,
                         prep->dRelease.value,
                         tuner,
                         prep->getGainPtr());
		}
        
        //store synthNoteNumbers by noteNumber
        keyPlayed[noteNumber].add(synthNoteNumber);
        keyPlayedOffset[noteNumber].add(synthOffset);
        
    }
}

#define HAMMER_GAIN_SCALE 0.02f
#define RES_GAIN_SCALE 0.2f
void DirectProcessor::keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &invertVelocities.getReference(noteNumber);
        for (int i = TargetTypeDirect; i < TargetTypeDirect+1; ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i));
        }
    }
    // If this an actual release, aVels will be the incoming velocities,
    // but bVels will use the values from the last press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = &velocities.getReference(noteNumber);
    }
    
    if (bVels->getUnchecked(TargetTypeDirect) < 0.f) return;
    
    DBG("DirectProcessor::keyReleased " + String(noteNumber));
    for (int i = 0; i < keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        //float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        DBG("DirectProcessor::keyReleased sending keyOff");
        synth->keyOff(1,
                      MainNote,
                      direct->prep->getSoundSet(), //set
                      direct->getId(),
                      noteNumber,
                      t,
                      aVels->getUnchecked(TargetTypeDirect),
                      aGlobalGain,
                      direct->prep->getGainPtr(),
                      true);
    }

    keyPlayed[noteNumber].removeFirstMatchingValue(noteNumber);
    keyPlayedOffset[noteNumber].removeFirstMatchingValue(noteNumber);
}

void DirectProcessor::playReleaseSample(int noteNumber, Array<float>& targetVelocities,
                                        bool fromPress, bool soundfont)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &invertVelocities.getReference(noteNumber);
        for (int i = TargetTypeDirect; i < TargetTypeDirect+1; ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i+TargetTypeDirect));
        }
    }
    // If this an actual release, aVels will be the incoming velocities,
    // but bVels will use the values from the last press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = &velocities.getReference(noteNumber);
    }
    
    if (bVels->getUnchecked(TargetTypeDirect) < 0.f) return;
    
    //only play hammers/resonance for first note in layers of transpositions
    if (keyPlayed[noteNumber].isEmpty()) return;
    int t = keyPlayed[noteNumber].getUnchecked(0);
    float t_offset = keyPlayedOffset[noteNumber].getUnchecked(0);
    
    /*
     if (soundfont)
     {
     synth->keyOff(channel, HammerNote, direct->getId(), noteNumber, noteNumber, velocity, true);
     }
     else
     */
    if (!soundfont)
    {
        hammerSynth->keyOn  (1,
                             noteNumber,
                             t,
                             0,
                             aVels->getUnchecked(TargetTypeDirect),
                             HAMMER_GAIN_SCALE,
                             Forward,
                             Normal,
                             HammerNote,
                             direct->prep->getSoundSet(), //set
                             direct->getId(),
                             0,
                             2000,
                             3,
                             3,
                             tuner,
                             direct->prep->getHammerGainPtr());
        
        resonanceSynth->keyOn(1,
                              noteNumber,
                              t,
                              t_offset,
                              aVels->getUnchecked(TargetTypeDirect),
                              RES_GAIN_SCALE,
                              Forward,
                              Normal,
                              ResonanceNote,
                              direct->prep->getSoundSet(), //set
                              direct->getId(),
                              0,
                              2000,
                              3,
                              3,
                              tuner,
                              direct->prep->getResonanceGainPtr());
    }
}

float DirectProcessor::filterVelocity(float vel)
{
    DirectPreparation::Ptr prep = direct->prep;
    
    if (!lastVelocityInRange) lastVelocity = vel;
    
    int velocity = vel*127.f;
    
    if(prep->velocityMin.value <= prep->velocityMax.value)
    {
        if (velocity >= prep->velocityMin.value && velocity <= prep->velocityMax.value)
        {
            lastVelocityInRange = true;
            lastVelocity = vel;
            return vel;
        }
    }
    else
    {
        if (velocity >= prep->velocityMin.value || velocity <= prep->velocityMax.value)
        {
            lastVelocityInRange = true;
            lastVelocity = vel;
            return vel;
        }
    }
    
    DBG("failed velocity check");
    return -1.f;
}

void DirectProcessor::processBlock(int numSamples, int midiChannel, BKSampleLoadType type)
{
    sampleType = type;
    //tuner->processor->incrementAdaptiveClusterTime(numSamples);
}

#if BK_UNIT_TESTS

class DirectTests : public UnitTest
{
public:
    DirectTests() : UnitTest("Direct", "Direct") {}
    
    void runTest() override
    {
        beginTest("Direct");

        for (int i = 0; i < 10; i++)
        {
            // create direct preparation and randomize it
            // call getState() to convert to ValueTree
            // call setState() to convert from ValueTree to preparation
            // compare begin and end states
            String name = "random direct preparation " + String(i);
            DBG("test consistency: " + name);
            
            DirectPreparation::Ptr dp1 = new DirectPreparation();
            
            Direct d1(dp1, 1);
            d1.setName(name);
            
            ValueTree vt1 = d1.getState();
            
            XmlElement* xml = vt1.createXml();
            
            DirectPreparation::Ptr dp2 = new DirectPreparation();
            
            Direct d2(dp2, 1);
            
            d2.setState(xml);
            d2.setName(name);
            
            ValueTree vt2 = d2.getState();
            
            expect(vt1.isEquivalentTo(vt2), "direct preparation value trees don't match");
            
            expect(dp2->compare(dp1), dp1->getName() + " and " + dp2->getName() + " did not match.");
        }

		//test direct wrapper class
		for (int i = 0; i < 10; i++)
		{
			// create direct and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random direct " + String(i);
			DBG("test consistency: " + name);

			Direct d1(-1, true);
			d1.setName(name);

			ValueTree vt1 = d1.getState();

			XmlElement* xml = vt1.createXml();

			Direct d2(-1, true);

			d2.setState(xml);
			d2.setName(name);

			ValueTree vt2 = d2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"direct: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());
		}
        
    }
};

static DirectTests directTests;

class DirectModTests : public UnitTest
{
public:
    DirectModTests() : UnitTest("DirectMod", "DirectMod") {}
    
    void runTest() override
    {
        beginTest("DirectMod");
        
        for (int i = 0; i < 10; i++)
        {
            // create direct mod preparation and randomize it
            // call getState() to convert to ValueTree
            // call setState() to convert from ValueTree to preparation
            // compare begin and end states
            String name = "random direct mod " + String(i);
            DBG("test consistency: " + name);
            
            DirectPreparation::Ptr dp1 = new DirectPreparation();
            DirectModification::Ptr dm1 = new DirectModification(dp1, 1);
            
            dm1->randomize();
            
            ValueTree vt1 = dm1->getState();
            
            XmlElement* xml = vt1.createXml();
            
            DirectPreparation::Ptr dp2 = new DirectPreparation();
            DirectModification::Ptr dm2 = new DirectModification(dp2, 1);
            
            dm2->setState(xml);
        
            ValueTree vt2 = dm2->getState();
            
            expect(vt1.isEquivalentTo(vt2),
                   "direct mod: value trees do not match\n" +
                   vt1.toXmlString() +
                   "\n=======================\n" +
                   vt2.toXmlString());
            
            //expect(dm1->compare(dm2), "direct mod: preparations do not match");
        }
        
    }
};

static DirectModTests directModTests;

#endif

