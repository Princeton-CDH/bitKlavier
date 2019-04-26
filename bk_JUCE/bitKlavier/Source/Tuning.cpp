/*
 ==============================================================================
 
 Tuning.cpp
 Created: 4 Dec 2016 3:40:27pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "Tuning.h"


TuningProcessor::TuningProcessor(Tuning::Ptr tuning):
tuning(tuning),
lastNoteTuning(0),
lastIntervalTuning(0)
{
}

TuningProcessor::~TuningProcessor()
{
}

//returns offsets; main callback
float TuningProcessor::getOffset(int midiNoteNumber, bool updateLastInterval)
{
    float lastNoteTuningTemp = lastNoteTuning;
    float lastNoteOffset;
 
    //do adaptive tunings if using
    if(tuning->aPrep->getAdaptiveType() == AdaptiveNormal || tuning->aPrep->getAdaptiveType() == AdaptiveAnchored)
    {
        float lastNoteOffset = adaptiveCalculate(midiNoteNumber) + tuning->aPrep->getFundamentalOffset(); // added getFundamentalOffset()
        
        if(updateLastInterval)
        {
            lastNoteTuning = midiNoteNumber + lastNoteOffset;
            lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
        }

        return lastNoteOffset;
    }
    
    
    //do nTone tuning if nToneSemitoneWidth != 100cents
    if(tuning->aPrep->getNToneSemitoneWidth() != 100)
    {
        lastNoteOffset = .01 * (midiNoteNumber - tuning->aPrep->getNToneRoot()) * (tuning->aPrep->getNToneSemitoneWidth() - 100);
        int midiNoteNumberTemp = round(midiNoteNumber + lastNoteOffset);
        
        Array<float> currentTuning;
        if(tuning->aPrep->getScale() == CustomTuning) currentTuning = tuning->aPrep->getCustomScale();
        else currentTuning = tuning->tuningLibrary.getUnchecked(tuning->aPrep->getScale());
        
        lastNoteOffset += (currentTuning[(midiNoteNumberTemp - tuning->aPrep->getFundamental()) % currentTuning.size()] +
                          + tuning->aPrep->getAbsoluteOffsets().getUnchecked(midiNoteNumber) +
                          tuning->aPrep->getFundamentalOffset());
        
        if(updateLastInterval)
        {
            lastNoteTuning = midiNoteNumber + lastNoteOffset;
            lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
        }
        
        return lastNoteOffset;
    }

    //else do regular tunings
    Array<float> currentTuning;
    if(tuning->aPrep->getScale() == CustomTuning) currentTuning = tuning->aPrep->getCustomScale();
    else currentTuning = tuning->tuningLibrary.getUnchecked(tuning->aPrep->getScale());
    
    lastNoteOffset = (currentTuning[(midiNoteNumber - tuning->aPrep->getFundamental()) % currentTuning.size()] +
                      + tuning->aPrep->getAbsoluteOffsets().getUnchecked(midiNoteNumber) +
                      tuning->aPrep->getFundamentalOffset());
    
    if(updateLastInterval)
    {
        lastNoteTuning = midiNoteNumber + lastNoteOffset;
        lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
    }
    
    return lastNoteOffset;
    
}


//for keeping track of current cluster size
void TuningProcessor::processBlock(int numSamples)
{
    TuningAdaptiveSystemType type = tuning->aPrep->getAdaptiveType();
    
    if (type == AdaptiveNormal || type == AdaptiveAnchored) {
        
        if(clusterTime <= (tuning->aPrep->getAdaptiveClusterThresh() * sampleRate * 0.001))
            clusterTime += numSamples;
    }
}

void TuningProcessor::keyReleased(int midiNoteNumber)
{
    tuning->aPrep->getSpringTuning()->removeNote(midiNoteNumber);
}


//add note to the adaptive tuning history, update adaptive fundamental
void TuningProcessor::keyPressed(int midiNoteNumber)
{
    adaptiveHistoryCounter++;
    
    TuningAdaptiveSystemType type = tuning->aPrep->getAdaptiveType();

    if (type == AdaptiveNormal)
    {
        //if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory() - 1)
        if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory())
        {
            adaptiveHistoryCounter = 0;
            adaptiveFundamentalFreq = adaptiveFundamentalFreq * adaptiveCalculateRatio(midiNoteNumber);
            adaptiveFundamentalNote = midiNoteNumber;
        }
        //else adaptiveHistoryCounter++;
        
    }
    else if (type == AdaptiveAnchored)
    {
        //if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory() - 1)
        if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory())
        {
            adaptiveHistoryCounter = 0;
            
            const Array<float> anchorTuning = tuning->tuningLibrary.getUnchecked(tuning->aPrep->getAdaptiveAnchorScale());
            adaptiveFundamentalFreq = mtof(midiNoteNumber +
                                           anchorTuning[(midiNoteNumber + tuning->aPrep->getAdaptiveAnchorFundamental()) % anchorTuning.size()]
                                           );
            adaptiveFundamentalNote = midiNoteNumber;

        }
        //else adaptiveHistoryCounter++;
    }
    
    tuning->aPrep->getSpringTuning()->addNote(midiNoteNumber);
    
    clusterTime = 0;
    
}

float TuningProcessor::adaptiveCalculateRatio(const int midiNoteNumber) const
{
    int tempnote = midiNoteNumber;
    float newnote;
    float newratio;
    
    const Array<float> intervalScale = tuning->tuningLibrary.getUnchecked(tuning->aPrep->getAdaptiveIntervalScale());
    
    if(!tuning->aPrep->getAdaptiveInversional() || tempnote >= adaptiveFundamentalNote)
    {
        
        while((tempnote - adaptiveFundamentalNote) < 0) tempnote += 12;
    
        newnote = midiNoteNumber + intervalScale[(tempnote - adaptiveFundamentalNote) % intervalScale.size()];
        newratio = intervalToRatio(newnote - adaptiveFundamentalNote);
        
        return newratio;
        
    }
    //else
    newnote = midiNoteNumber - intervalScale[(adaptiveFundamentalNote - tempnote) % intervalScale.size()];
    newratio = intervalToRatio(newnote - adaptiveFundamentalNote);
    
    return newratio;
}

float TuningProcessor::adaptiveCalculate(int midiNoteNumber) const
{
    float newnote = adaptiveFundamentalFreq * adaptiveCalculateRatio(midiNoteNumber);
    return ftom(newnote) - midiNoteNumber;
}

void TuningProcessor::adaptiveReset()
{
    adaptiveFundamentalNote = tuning->aPrep->getFundamental();
    adaptiveFundamentalFreq = mtof(adaptiveFundamentalNote);
    adaptiveHistoryCounter = 0;
    
}


ValueTree Tuning::getState(bool active)
{
    ValueTree prep(vtagTuning);
    
    prep.setProperty( "Id", Id, 0);
    prep.setProperty( "name", name, 0);
    
    prep.addChild(active ? aPrep->getState() : sPrep->getState(), -1, 0);
    
    return prep;
}

void Tuning::setState(XmlElement* e)
{
    Id = e->getStringAttribute("Id").getIntValue();
    
    String n = e->getStringAttribute("name");
    
    if (n != String::empty)     name = n;
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

#if BK_UNIT_TESTS

class TuningTests : public UnitTest
{
public:
	TuningTests() : UnitTest("Tunings", "Tuning") {}

	void runTest() override
	{
		beginTest("Tuning");

		for (int i = 0; i < 10; i++)
		{
			// create tuning preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random tuning prep " + String(i);
			DBG("test consistency: " + name);

			TuningPreparation::Ptr tp1 = new TuningPreparation();

			tp1->randomize();

			Tuning t1(tp1, 1);
			t1.setName(name);

			ValueTree vt1 = t1.getState();

			ScopedPointer<XmlElement> xml = vt1.createXml();

			TuningPreparation::Ptr tp2 = new TuningPreparation();

			Tuning t2(tp2, 1);

			t2.setState(xml);
			t2.setName(name);

			ValueTree vt2 = t2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"tuning prep: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());
		}

		for (int i = 0; i < 10; i++)
		{
			// create tuning and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random tuning " + String(i);
			DBG("test consistency: " + name);

			Tuning t1(-1, true);
			t1.setName(name);

			ValueTree vt1 = t1.getState();

			ScopedPointer<XmlElement> xml = vt1.createXml();

			Tuning t2(-1, true);

			t2.setState(xml);
			t2.setName(name);

			ValueTree vt2 = t2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"tuning: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());
		}
	}
};

static TuningTests tuningTests;

class TuningModTests : public UnitTest
{
public:
	TuningModTests() : UnitTest("TuningMod", "TuningMod") {}

	void runTest() override
	{
		beginTest("TuningMod");

		for (int i = 0; i < 10; i++)
		{
			// create tuning mod preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random direct mod " + String(i);
			DBG("test consistency: " + name);

			TuningPreparation::Ptr tp1 = new TuningPreparation();
			TuningModification::Ptr tm1 = new TuningModification(tp1, 1);


			tm1->randomize();
			tm1->setName(name);

			ValueTree vt1 = tm1->getState();

			ScopedPointer<XmlElement> xml = vt1.createXml();

			TuningPreparation::Ptr tp2 = new TuningPreparation();
			TuningModification::Ptr tm2 = new TuningModification(tp2, 1);

			tm2->setState(xml);

			ValueTree vt2 = tm2->getState();

			expect(vt1.isEquivalentTo(vt2),
				"tuning mod: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());


			//expect(tm1->compare(tm2), "tuning mod: preparations do not match");
		}

	}
};

static TuningModTests tuningModTests;

#endif
