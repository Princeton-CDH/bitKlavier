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
    velocities.ensureStorageAllocated(128);
    velocitiesActive.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        velocities.insert(i, 0.);
        velocitiesActive.insert(i, 0);
    }
    
    if (!direct->prep->getUseGlobalSoundSet())
    {
        // comes in as "soundfont.sf2.subsound1"
        String name = direct->prep->getSoundSetName();
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
        direct->prep->setSoundSet(Id);
        direct->prep->setSoundSet(Id);
    }
}

DirectProcessor::~DirectProcessor(void)
{
    
}

void DirectProcessor::keyPressed(int noteNumber, float velocity, int channel)
{
    tuner->getOffset(noteNumber, true);
    
    lastVelocity = velocity;
    
    DirectPreparation::Ptr prep = direct->prep;
    
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
    
    for (auto t : prep->getTransposition())
    {
        // synthNoteNumber is what determines what sample is chosen
        // without transposition or extreme tuning, this will be the same as noteNumber
        // we need to keep track of both, for noteOff messages, since with transpositions
        // we may have multiple synthNotes associated with one key/noteNumber
        int synthNoteNumber = noteNumber;
        
        float offset; // offset from integer, but may be greater than 1
        float synthOffset; // offset from actual sample played, always less than 1.
        
        // tune the transposition
        if (prep->getTranspUsesTuning()) // use the Tuning setting
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
            synth->keyOn(channel,
                         noteNumber,
                         synthNoteNumber,
                         synthOffset,
                         velocitiesActive.getUnchecked(noteNumber),
                         aGlobalGain,
                         Forward,
                         Normal,
                         MainNote,
                         direct->prep->getSoundSet(), //set
                         direct->getId(),
                         0,     // start
                         0,     // length
                         prep->getAttack(),
                         prep->getDecay(),
                         prep->getSustain(),
                         prep->getRelease(),
                         tuner,
                         prep->getGainPtr(),
                         prep->getBlendronicGain(),
                         blendronic);
		}
		else
		{
			synth->keyOn(channel,
                         noteNumber,
                         synthNoteNumber,
                         synthOffset,
                         velocitiesActive.getUnchecked(noteNumber),
                         aGlobalGain,
                         Forward,
                         Normal,
                         MainNote,
                         prep->getSoundSet(), //set
                         direct->getId(),
                         0,     // start
                         0,     // length
                         prep->getAttack(),
                         prep->getDecay(),
                         prep->getSustain(),
                         prep->getRelease(),
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
void DirectProcessor::keyReleased(int noteNumber, float velocity, int channel, bool soundfont)
{
    if (!velocityCheck(noteNumber)) return;
    
    for (int i = 0; i<keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        //float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        synth->keyOff(channel,
                      MainNote,
                      direct->prep->getSoundSet(), //set
                      direct->getId(),
                      noteNumber,
                      t,
                      velocitiesActive.getUnchecked(noteNumber),
                      direct->prep->dGain.value * aGlobalGain,
                      true);
    }

    keyPlayed[noteNumber].clearQuick();
    keyPlayedOffset[noteNumber].clearQuick();
    
}

void DirectProcessor::playReleaseSample(int noteNumber, float velocity, int channel, bool soundfont)
{
    // CHECK THIS OUT WITH DAN (dont need loop right? can just do keyPlayed[noteNumber].getUnchecked(0);
    for (int i = 0; i<keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        //only play hammers/resonance for first note in layers of transpositions
        if(i==0)
        {
            float hGain = direct->prep->getHammerGain();
            float rGain = direct->prep->getResonanceGain();
            
            if (hGain > 0.0f)
            {
                
                /*
                if (soundfont)
                {
                    synth->keyOff(channel, HammerNote, direct->getId(), noteNumber, noteNumber, velocity, true);
                }
                else
                 */
                if (!soundfont)
                {
                    hammerSynth->keyOn  (channel,
                                       noteNumber,
                                       t,
                                       0,
                                       velocitiesActive.getUnchecked(noteNumber),
                                       hGain * HAMMER_GAIN_SCALE,
                                       Forward,
                                       Normal,
                                       HammerNote,
                                       direct->prep->getSoundSet(), //set
                                       direct->getId(),
                                       0,
                                       2000,
                                       3,
                                       3,
                                       tuner);
                }
                
            }
            
            if (rGain > 0.0f)
            {
                resonanceSynth->keyOn(channel,
                                      noteNumber,
                                      t,
                                      t_offset,
                                      velocitiesActive.getUnchecked(noteNumber),
                                      rGain * RES_GAIN_SCALE,
                                      Forward,
                                      Normal,
                                      ResonanceNote,
                                      direct->prep->getSoundSet(), //set
                                      direct->getId(),
                                      0,
                                      2000,
                                      3,
                                      3,
                                      tuner);
                
            }
        }
    }
}

bool DirectProcessor::velocityCheck(int noteNumber)
{
    DirectPreparation::Ptr prep = direct->prep;
    
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

void DirectProcessor::processBlock(int numSamples, int midiChannel, BKSampleLoadType type)
{
    sampleType = type;
    //tuner->processor->incrementAdaptiveClusterTime(numSamples);
}

void DirectPreparation::performModification(DirectModification* d, Array<bool> dirty)
{
    if (d->altMod && modded)
    {
        if (dirty[DirectGain]) dGain.unmodFrom(d->dGain);
        modded = false;
    }
    else
    {
        if (dirty[DirectTransposition]) dTransposition = d->getTransposition();
        if (dirty[DirectGain]) dGain.modTo(d->dGain);
        if (dirty[DirectResGain]) dResonanceGain = d->getResonanceGain();
        if (dirty[DirectHammerGain]) dHammerGain = d->getHammerGain();
        if (dirty[DirectBlendronicGain]) dBlendronicGain = d->getBlendronicGain();
        if (dirty[DirectTranspUsesTuning]) dTranspUsesTuning = d->getTranspUsesTuning();
        if (dirty[DirectADSR])
        {
            dAttack = d->getAttack();
            dDecay = d->getDecay();
            dSustain = d->getSustain();
            dRelease = d->getRelease();
        }
        
        if (dirty[DirectUseGlobalSoundSet]) dUseGlobalSoundSet = d->getUseGlobalSoundSet();
        
        if (dirty[DirectSoundSet])
        {
            dSoundSet = d->getSoundSet();
            dSoundSetName = d->getSoundSetName();
        }
        
        if (dirty[DirectVelocityMin]) velocityMin = d->getVelocityMin();
        if (dirty[DirectVelocityMax]) velocityMax = d->getVelocityMax();
        modded = true;
    }
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

