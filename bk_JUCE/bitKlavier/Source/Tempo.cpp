/*
  ==============================================================================

    Tempo.cpp
    Created: 26 Feb 2017 11:38:35pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "Tempo.h"

#include "PluginProcessor.h"

#include "Modification.h"

void TempoPreparation::performModification(TempoModification* s, Array<bool> dirty)
{
    bool reverse = s->altMod && modded;
    
    if (dirty[TempoBPM]) sTempo.modify(s->sTempo, reverse);
    if (dirty[TempoSubdivisions]) subdivisions.modify(s->subdivisions, reverse);
    if (dirty[TempoSystem]) sWhichTempoSystem.modify(s->sWhichTempoSystem, reverse);
    if (dirty[AT1History]) at1History.modify(s->at1History, reverse);
    if (dirty[AT1Min]) at1Min.modify(s->at1Min, reverse);
    if (dirty[AT1Max]) at1Max.modify(s->at1Max, reverse);
    if (dirty[AT1Subdivisions]) at1Subdivisions.modify(s->at1Subdivisions, reverse);
    if (dirty[AT1Mode]) at1Mode.modify(s->at1Mode, reverse);
    
    modded = !reverse;
}

TempoProcessor::TempoProcessor(BKAudioProcessor& processor, Tempo::Ptr t):
processor(processor),
tempo(t),
keymaps(Keymap::PtrArr())
{
    atTimer = 0;
    atLastTime = 0;
    atDeltaHistory.ensureStorageAllocated(10);
    for (int i = 0; i < 10; i++)
    {
        atDeltaHistory.insert(0, (tempo->prep->getAdaptiveTempo1Subdivisions() * 60000.0/tempo->prep->getTempo()));
    }
    adaptiveTempoPeriodMultiplier = 1.;
    
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
}

TempoProcessor::~TempoProcessor()
{
}

void TempoProcessor::processBlock(int numSamples, int channel)
{
    atTimer += numSamples;
}

void TempoProcessor::keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an actual key press (not an inverted release) aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &velocities.getReference(noteNumber);
        for (int i = TargetTypeTempo; i < TargetTypeTuning; ++i)
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
    
    if (bVels->getUnchecked(TargetTypeTempo) < 0.f) return;
    
    DBG("adding adaptive tempo note" + String(noteNumber));
    atNewNote();
}

void TempoProcessor::keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &invertVelocities.getReference(noteNumber);
        for (int i = TargetTypeTempo; i < TargetTypeTuning; ++i)
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
    
    if (bVels->getUnchecked(TargetTypeTempo) < 0.f) return;
    
    atNewNoteOff();
}

//adaptive tempo functions
void TempoProcessor::atNewNote()
{
    if(tempo->prep->getAdaptiveTempo1Mode() == TimeBetweenNotes) atCalculatePeriodMultiplier();
    atLastTime = atTimer;
}

void TempoProcessor::atNewNoteOff()
{
    if(tempo->prep->getAdaptiveTempo1Mode() == NoteLength) atCalculatePeriodMultiplier();
}

//really basic, using constrained moving average of time-between-notes (or note-length)
void TempoProcessor::atCalculatePeriodMultiplier()
{

    DBG("tempo system = " + String(tempo->prep->getTempoSystem()));
    if(tempo->prep->getAdaptiveTempo1History() && tempo->prep->getTempoSystem() == AdaptiveTempo1) {
        
        atDelta = (atTimer - atLastTime) / (0.001 * processor.getCurrentSampleRate()); //fix this? make sampleRateMS
        //DBG("atTimer = " + String(atTimer) + " atLastTime = " + String(atLastTime));
        //DBG("atDelta = " + String(atDelta));
        //DBG("sampleRate = " + String(sampleRate));
        
        //constrain be min and max times between notes
        if (atDelta > tempo->prep->getAdaptiveTempo1Min() && atDelta < tempo->prep->getAdaptiveTempo1Max())
        {
            //insert delta at beginning of history
            atDeltaHistory.insert(0, atDelta);
            
            //eliminate oldest time difference
            atDeltaHistory.resize(tempo->prep->getAdaptiveTempo1History());
            
            //calculate moving average and then tempo period multiplier
            int totalDeltas = 0;
            for(int i = 0; i < atDeltaHistory.size(); i++) totalDeltas += atDeltaHistory.getUnchecked(i);
            float movingAverage = totalDeltas / tempo->prep->getAdaptiveTempo1History();
            
            adaptiveTempoPeriodMultiplier = movingAverage /
                                            tempo->prep->getBeatThreshMS() /
                                            tempo->prep->getAdaptiveTempo1Subdivisions();
            
            DBG("adaptiveTempoPeriodMultiplier = " + String(adaptiveTempoPeriodMultiplier));
        }
    }
}

void TempoProcessor::adaptiveReset()
{
    for (int i = 0; i < tempo->prep->getAdaptiveTempo1History(); i++)
    {
        atDeltaHistory.insert(0, (60000.0/tempo->prep->getTempo()));
    }
    adaptiveTempoPeriodMultiplier = 1.;
}

int TempoProcessor::getAtDelta()
{
    return atDelta = (atTimer - atLastTime) * 1000. / processor.getCurrentSampleRate();
}

#if BK_UNIT_TESTS

class TempoTests : public UnitTest
{
public:
	TempoTests() : UnitTest("Tempos", "Tempo") {}

	void runTest() override
	{
		beginTest("Tempo");

		for (int i = 0; i < 10; i++)
		{
			// create tempo preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random tempo preparation " + String(i);
			DBG("test consistency: " + name);

			TempoPreparation::Ptr tp1 = new TempoPreparation();

			Tempo t1(tp1, 1);
			t1.setName(name);

			ValueTree vt1 = t1.getState();

			XmlElement* xml = vt1.createXml();

			TempoPreparation::Ptr tp2 = new TempoPreparation();

			Tempo t2(tp2, 1);

			t2.setState(xml);
			t2.setName(name);

			ValueTree vt2 = t2.getState();

			expect(vt1.isEquivalentTo(vt2), "tempo prep value trees don't match");

			//expect(tp2->compare(tp1), tp1->getName() + " and " + tp2->getName() + " did not match.");
		}

		//test tuning wrapper class
		for (int i = 0; i < 10; i++)
		{
			// create tempo and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random tempo " + String(i);
			DBG("test consistency: " + name);

			Tempo m1(-1, true);
			m1.setName(name);

			ValueTree vt1 = m1.getState();

			XmlElement* xml = vt1.createXml();

			Tempo m2(-1, true);

			m2.setState(xml);
			m2.setName(name);

			ValueTree vt2 = m2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"tempo: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());
		}

	}
};

static TempoTests directTests;

class TempoModTests : public UnitTest
{
public:
	TempoModTests() : UnitTest("TempoMod", "TempoMod") {}

	void runTest() override
	{
		beginTest("TempoMod");

		for (int i = 0; i < 10; i++)
		{
			// create tempo mod preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random direct mod " + String(i);
			DBG("test consistency: " + name);

			TempoPreparation::Ptr tp1 = new TempoPreparation();
			TempoModification::Ptr tm1 = new TempoModification(tp1, 1);
			

			tm1->randomize();
			tm1->setName(name);

			ValueTree vt1 = tm1->getState();

			XmlElement* xml = vt1.createXml();

			TempoPreparation::Ptr tp2 = new TempoPreparation();
			TempoModification::Ptr tm2 = new TempoModification(tp2, 1);

			tm2->setState(xml);

			ValueTree vt2 = tm2->getState();

			expect(vt1.isEquivalentTo(vt2),
				"tempo mod: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());


			//expect(tm1->compare(tm2), "tempo mod: preparations do not match");
		}

	}
};

static TempoModTests tempoModTests;

#endif
