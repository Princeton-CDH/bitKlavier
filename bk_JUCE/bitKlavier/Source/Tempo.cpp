/*
  ==============================================================================

    Tempo.cpp
    Created: 26 Feb 2017 11:38:35pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "Tempo.h"

#include "PluginProcessor.h"

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
        atDeltaHistory.insert(0, 1.);
    }
    adaptiveTempoPeriodMultiplier = 1.;
    
    emaCount = 0;
    emaSum = 0;
}

TempoProcessor::~TempoProcessor()
{
}

void TempoProcessor::processBlock(int numSamples, int channel)
{
    atTimer += numSamples;
}

void TempoProcessor::keyPressed(int noteNumber, float velocity)
{
    DBG("adding adaptive tempo note" + String(noteNumber));
    atNewNote();
}

void TempoProcessor::keyReleased(int noteNumber, int channel)
{
    atNewNoteOff();
}

//adaptive tempo functions
void TempoProcessor::atNewNote()
{
    if(tempo->aPrep->getAdaptiveTempoMode().contains(TimeBetweenOnsets)) atCalculatePeriodMultiplier();
    atLastTime = atTimer;
}

void TempoProcessor::atNewNoteOff()
{
    if(tempo->aPrep->getAdaptiveTempoMode().contains(NoteLength) ||
       tempo->aPrep->getAdaptiveTempoMode().contains(TimeBetweenReleases)) atCalculatePeriodMultiplier();
}

//really basic, using constrained moving average of time-between-notes (or note-length)
void TempoProcessor::atCalculatePeriodMultiplier()
{

    DBG("tempo system = " + String(tempo->aPrep->getTempoSystem()));
    if(tempo->aPrep->getAdaptiveTempoHistorySize() && tempo->aPrep->getTempoSystem() == AdaptiveTempo) {
        
        atOnsetHistory.insert(0, atTimer);
        atOnsetHistory.resize(tempo->aPrep->getAdaptiveTempoHistorySize());
        
        atDelta = (atTimer - atLastTime) / (0.001 * processor.getCurrentSampleRate()); //fix this? make sampleRateMS
        //DBG("atTimer = " + String(atTimer) + " atLastTime = " + String(atLastTime));
        //DBG("atDelta = " + String(atDelta));
        //DBG("sampleRate = " + String(sampleRate));
        
        //constrain between min and max times between notes
        
        float beatMS = tempo->aPrep->getBeatThreshMS();
        float pulseMS = beatMS / tempo->aPrep->getAdaptiveTempoSubdivisions();
        float quant = roundf(atDelta / pulseMS) * pulseMS;
//        float error = atDelta - quant;
        int val = fmax(beatMS / quant, quant / beatMS);
        if (quant > tempo->aPrep->getAdaptiveTempoMin() && quant < tempo->aPrep->getAdaptiveTempoMax()
            && val < tempo->aPrep->getAdaptiveTempoWeights().size())
        {
            atDelta /= quant;
            //insert delta at beginning of history
            atDeltaHistory.insert(0, atDelta);
            //eliminate oldest time difference
            atDeltaHistory.resize(tempo->aPrep->getAdaptiveTempoHistorySize());
            
            float weight = tempo->aPrep->getAdaptiveTempoWeights().getUnchecked(val);
            atWeightHistory.insert(0, weight);
            atWeightHistory.resize(tempo->aPrep->getAdaptiveTempoHistorySize());
            
            //calculate moving average and then tempo period multiplier
            
            // MA
            float totalDeltas = 0;
            float totalWeights = 0;
            for(int i = 0; i < atDeltaHistory.size(); i++)
            {
                float delta = atDeltaHistory.getUnchecked(i);
                if (tempo->aPrep->getUseWeights()) delta *= atWeightHistory.getUnchecked(i);
                totalDeltas += delta;
                totalWeights += atWeightHistory.getUnchecked(i);
            }
            float movingAverage;
            if (tempo->aPrep->getUseWeights())
            {
                if (totalWeights > 0) movingAverage = totalDeltas / totalWeights;
                else movingAverage = adaptiveTempoPeriodMultiplier;
            }
            else movingAverage = totalDeltas / tempo->aPrep->getAdaptiveTempoHistorySize();
        
            adaptiveTempoPeriodMultiplier = movingAverage;
            
            // EMA
            float alpha = tempo->aPrep->getAdaptiveTempoAlpha();
            if (!tempo->aPrep->getUseWeights())
            {
                weight = 1.0f;
            }
            if (weight > 0)
            {
                emaSum = weight * atDelta + (1 - alpha) * emaSum;
                emaCount = weight + (1 - alpha) * emaCount;
                exponentialMovingAverage = emaSum / emaCount;
                
                if (tempo->aPrep->getUseExponential())
                {
                    adaptiveTempoPeriodMultiplier = exponentialMovingAverage;
                }
            }
        }
        
        DBG("adaptiveTempoPeriodMultiplier = " + String(adaptiveTempoPeriodMultiplier));
    }
}

void TempoProcessor::adaptiveReset()
{
    for (int i = 0; i < tempo->aPrep->getAdaptiveTempoHistorySize(); i++)
    {
        atDeltaHistory.insert(0, (60000.0/tempo->aPrep->getTempo()));
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
