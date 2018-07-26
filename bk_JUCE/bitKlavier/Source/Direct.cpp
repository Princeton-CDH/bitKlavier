/*
  ==============================================================================

    Direct.cpp
    Created: 6 Dec 2016 12:46:37pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Direct.h"

DirectProcessor::DirectProcessor(Direct::Ptr direct,
                                 TuningProcessor::Ptr tuning,
                                 BKSynthesiser *s,
                                 BKSynthesiser *res,
                                 BKSynthesiser *ham):
synth(s),
resonanceSynth(res),
hammerSynth(ham),
direct(direct),
tuner(tuning)
{
    
}

DirectProcessor::~DirectProcessor(void)
{
    
}

void DirectProcessor::keyPressed(int noteNumber, float velocity, int channel)
{
    for (auto t : direct->aPrep->getTransposition())
    {
        float offset = t + tuner->getOffset(noteNumber), synthOffset = offset;
        int synthNoteNumber = noteNumber;
        
        //if (sampleType < BKLoadSoundfont)
        {
            synthNoteNumber += (int)offset;
            synthOffset     -= (int)offset;
        }

        synth->keyOn(channel,
                     noteNumber,
                     synthNoteNumber,
                     synthOffset,
                     velocity,
                     direct->aPrep->getGain() * aGlobalGain,
                     Forward,
                     Normal,
                     MainNote,
                     direct->getId(),
                     0,     // start
                     0,     // length
                     direct->aPrep->getAttack(),
                     direct->aPrep->getDecay(),
                     direct->aPrep->getSustain(),
                     direct->aPrep->getRelease());
        
        
        //store synthNoteNumbers by noteNumber
        keyPlayed[noteNumber].add(synthNoteNumber);
        keyPlayedOffset[noteNumber].add(synthOffset);
        
    }
}

#define HAMMER_GAIN_SCALE 0.02f
#define RES_GAIN_SCALE 0.2f
void DirectProcessor::keyReleased(int noteNumber, float velocity, int channel)
{
    for (int i = 0; i<keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        synth->keyOff(channel,
                      MainNote,
                      direct->getId(),
                      noteNumber,
                      t,
                      velocity,
                      true);
    }

    keyPlayed[noteNumber].clearQuick();
    keyPlayedOffset[noteNumber].clearQuick();
    
}

void DirectProcessor::playHammerResonance(int noteNumber, float velocity, int channel)
{
    for (int i = 0; i<keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        //only play hammers/resonance for first note in layers of transpositions
        if(i==0)
        {
            float hGain = direct->aPrep->getHammerGain();
            float rGain = direct->aPrep->getResonanceGain();
            
            if (hGain > 0.0f)
            {
                hammerSynth->keyOn(channel,
                                   noteNumber,
                                   t,
                                   0,
                                   velocity,
                                   hGain * HAMMER_GAIN_SCALE,
                                   Forward,
                                   Normal, 
                                   HammerNote,
                                   direct->getId(),
                                   0,
                                   2000,
                                   3,
                                   3 );
            }
            
            if (rGain > 0.0f)
            {
                resonanceSynth->keyOn(channel,
                                      noteNumber,
                                      t,
                                      t_offset,
                                      velocity,
                                      rGain * RES_GAIN_SCALE,
                                      Forward,
                                      Normal,
                                      ResonanceNote,
                                      direct->getId(),
                                      0,
                                      2000,
                                      3,
                                      3 );
                
            }
        }
    }
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
            String name = "random direct " + String(i);
            DBG("test consistency: " + name);
            
            DirectPreparation::Ptr dp1 = new DirectPreparation();
            
            Direct d1(dp1, 1);
            d1.setName(name);
            
            ValueTree vt1 = d1.getState();
            
            ScopedPointer<XmlElement> xml = vt1.createXml();
            
            DirectPreparation::Ptr dp2 = new DirectPreparation();
            
            Direct d2(dp2, 1);
            
            d2.setState(xml);
            d2.setName(name);
            
            ValueTree vt2 = d2.getState();
            
            expect(vt1.isEquivalentTo(vt2), "direct value trees don't match");
            
            expect(dp2->compare(dp1), dp1->getName() + " and " + dp2->getName() + " did not match.");
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
            DirectModPreparation::Ptr dm1 = new DirectModPreparation(dp1, 1);
            
            dm1->randomize();
            
            ValueTree vt1 = dm1->getState();
            
            ScopedPointer<XmlElement> xml = vt1.createXml();
            
            DirectPreparation::Ptr dp2 = new DirectPreparation();
            DirectModPreparation::Ptr dm2 = new DirectModPreparation(dp2, 1);
            
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

