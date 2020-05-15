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
    
    atOnsetHistory.ensureStorageAllocated(tempo->aPrep->maxIterationDepth);
    for (int i = 0; i < tempo->aPrep->maxIterationDepth; i++)
    {
        atOnsetHistory.add(0);
    }
    
    atReleaseHistory.ensureStorageAllocated(tempo->aPrep->maxIterationDepth);
    for (int i = 0; i < tempo->aPrep->maxIterationDepth; i++)
    {
        atReleaseHistory.add(0);
    }
    
    atDeltaHistory.ensureStorageAllocated(10);
    for (int i = 0; i < 10; i++)
    {
        atDeltaHistory.add(1.);
    }
    
    lastNoteOnsets.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        atDeltaHistory.add(1);
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
    atNewNote(noteNumber);
}

void TempoProcessor::keyReleased(int noteNumber, int channel)
{
    atNewNoteOff(noteNumber);
}

//adaptive tempo functions
void TempoProcessor::atNewNote(int noteNumber)
{
    lastNoteOnsets.set(noteNumber, atTimer);
    DBG("tempo system = " + String(tempo->aPrep->getTempoSystem()));
    if(tempo->aPrep->getAdaptiveTempoHistorySize() && tempo->aPrep->getTempoSystem() == AdaptiveTempo)
    {
        if(tempo->aPrep->getAdaptiveTempoMode().getUnchecked(TimeBetweenOnsets))
        {
            // How many onsets are we looking back at?
            for (int i = 0; i < tempo->aPrep->iterationDepth; i++)
            {
                uint64 pastOnset = atOnsetHistory.getUnchecked(i);
                if (pastOnset > 0)
                {
                    atDelta = (atTimer - pastOnset) / (0.001 * processor.getCurrentSampleRate());
                    atCalculatePeriodMultiplier();
                }
            }
            
            tempo->aPrep->setTempo(tempo->aPrep->getTempo() / adaptiveTempoPeriodMultiplier);
        }
        
    }
    atOnsetHistory.insert(0, atTimer);
    atOnsetHistory.resize(tempo->aPrep->maxIterationDepth); // placeholder value, we'll want a max iterations or just set to iteration depth if we don't care about smoothly increasing that
    
    
}

void TempoProcessor::atNewNoteOff(int noteNumber)
{
    DBG("tempo system = " + String(tempo->aPrep->getTempoSystem()));
    if(tempo->aPrep->getAdaptiveTempoHistorySize() && tempo->aPrep->getTempoSystem() == AdaptiveTempo)
    {
        if (tempo->aPrep->getAdaptiveTempoMode().getUnchecked(NoteLength))
        {
            // I guess just always do one iteration for note length, not sure what else could make sense
            // Maybe add TimeReleaseSinceOnset and TimeOnsetSinceRelease? (compare a release to onset history or vice versa)
            uint64 onset = lastNoteOnsets.getUnchecked(noteNumber);
            if (onset > 0)
            {
                atDelta = (atTimer - onset) / (0.001 * processor.getCurrentSampleRate());
                atCalculatePeriodMultiplier();
            }
        }
        if (tempo->aPrep->getAdaptiveTempoMode().getUnchecked(TimeBetweenReleases))
        {
            // How many releases are we looking back at?
            for (int i = 0; i < tempo->aPrep->iterationDepth; i++)
            {
                uint64 pastRelease = atReleaseHistory.getUnchecked(i);
                if (pastRelease > 0)
                {
                    atDelta = (atTimer - pastRelease) / (0.001 * processor.getCurrentSampleRate());
                    atCalculatePeriodMultiplier();
                }
            }
        }
        tempo->aPrep->setTempo(tempo->aPrep->getTempo() / adaptiveTempoPeriodMultiplier);

    }
    atReleaseHistory.insert(0, atTimer);
    atReleaseHistory.resize(tempo->aPrep->maxIterationDepth);
    lastNoteOnsets.set(noteNumber, 0);
}

//really basic, using constrained moving average of time-between-notes (or note-length)
void TempoProcessor::atCalculatePeriodMultiplier()
{
    // Should think about what subdivisions actually mean for this system...
    
    // Get the beat duration (corresponds to tempo)
    float beatMS = tempo->aPrep->getBeatThreshMS();
    
    // Get the pulse duration (corresponds to tempo times subdivisions)
    float pulseMS = beatMS / tempo->aPrep->getAdaptiveTempoSubdivisions();
    
    // Quantize the delta to the nearest subdivision
    float quant = roundf(atDelta / pulseMS) * pulseMS;
//        float error = atDelta - quant;
    
    
    // If the quantized delta is within the set range then we can adjust the tempo
    if (quant > tempo->aPrep->getAdaptiveTempoMin() && quant < tempo->aPrep->getAdaptiveTempoMax())
    {
        // Turn the delta into a multiplier
        float multiplier = atDelta / quant;
        
        // Insert delta at beginning of history
        atDeltaHistory.insert(0, multiplier);
        // Eliminate oldest time difference
        atDeltaHistory.resize(tempo->aPrep->getAdaptiveTempoHistorySize() * tempo->aPrep->iterationDepth);
        
        
        // Get the metric value of the delta (if quarter == 1 then half == eighth == 2, dotted half == triplet == 3)
        // This will probably result in weird behavior with non-whole number subdivisions (should we even allow for that?)
        int val = fmax(beatMS / quant, quant / beatMS);
        // Get the corresponding weight and keep a history of weights in parallel to the deltas
        float weight = 0.0f;
        if (val < tempo->aPrep->getAdaptiveTempoWeights().size())
        {
            weight = tempo->aPrep->getAdaptiveTempoWeights().getUnchecked(val);
        }
        atWeightHistory.insert(0, weight);
        atWeightHistory.resize(tempo->aPrep->getAdaptiveTempoHistorySize() * tempo->aPrep->iterationDepth);
        
        
        
        // Calculate an exponential moving average or regular moving average
        
        // Always calculate the EMA if we're not set to use it
        // EMA
        
        if (!tempo->aPrep->getUseWeights())
        {
            weight = 1.0f;
        }

        // Not sure if this is right for a weighted EMA
        // Should be good for when weight is 0 or 1 but the curve might be weird?
        float alpha = tempo->aPrep->getAdaptiveTempoAlpha();
        emaSum = weight * multiplier + (1 - alpha * weight) * emaSum;
        emaCount = weight + (1 - alpha * weight) * emaCount;
        exponentialMovingAverage = emaSum / emaCount;
    
        adaptiveTempoPeriodMultiplier = exponentialMovingAverage;
        
        
        
        // Only calculate the regular moving average if we're not using the exponential
        // MA
        if (!tempo->aPrep->getUseExponential())
        {
            float totalDeltas = 0;
            float totalWeights = 0;
            for(int i = 0; i < atDeltaHistory.size(); i++)
            {
                float delta = atDeltaHistory.getUnchecked(i);
                if (delta > 0)
                {
                    if (tempo->aPrep->getUseWeights())
                    {
                        delta *= atWeightHistory.getUnchecked(i);
                        totalWeights += atWeightHistory.getUnchecked(i);
                    }
                    else totalWeights++;
                    totalDeltas += delta;
                }
            }
            
            float movingAverage;
            if (totalWeights > 0) movingAverage = totalDeltas / totalWeights;
            else movingAverage = adaptiveTempoPeriodMultiplier;
        
            adaptiveTempoPeriodMultiplier = movingAverage;
        }
    }
    
    DBG("adaptiveTempoPeriodMultiplier = " + String(adaptiveTempoPeriodMultiplier));
}

void TempoProcessor::adaptiveReset()
{
    for (int i = 0; i < tempo->aPrep->getAdaptiveTempoHistorySize() * tempo->aPrep->iterationDepth; i++)
    {
        atDeltaHistory.insert(0, 0.0f);
    }
    adaptiveTempoPeriodMultiplier = 1.;
}

int TempoProcessor::getAtDelta()
{
    return atDelta = (atTimer - atOnsetHistory.getUnchecked(0)) * 1000. / processor.getCurrentSampleRate();
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
