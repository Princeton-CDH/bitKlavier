/*
 ==============================================================================
 
 Tuning.cpp
 Created: 4 Dec 2016 3:40:27pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "Tuning.h"
#include "PluginProcessor.h"
#include "Modification.h"

void TuningPreparation::performModification(TuningModification* p, Array<bool> dirty)
{
    bool reverse = p->altMod && modded;
    
    if (dirty[TuningScale]) tScale.modify(p->tScale, reverse);
    if (dirty[TuningFundamental]) tFundamental.modify(p->tFundamental, reverse);
    if (dirty[TuningOffset]) tFundamentalOffset.modify(p->tFundamentalOffset, reverse);
    if (dirty[TuningA1IntervalScale]) tAdaptiveIntervalScale.modify(p->tAdaptiveIntervalScale, reverse);
    if (dirty[TuningA1Inversional]) tAdaptiveInversional.modify(p->tAdaptiveInversional, reverse);
    if (dirty[TuningA1AnchorScale]) tAdaptiveAnchorScale.modify(p->tAdaptiveAnchorScale, reverse);
    if (dirty[TuningA1AnchorFundamental]) tAdaptiveAnchorFundamental.modify(p->tAdaptiveAnchorFundamental, reverse);
    if (dirty[TuningA1ClusterThresh]) tAdaptiveClusterThresh.modify(p->tAdaptiveClusterThresh, reverse);
    if (dirty[TuningA1History]) tAdaptiveHistory.modify(p->tAdaptiveHistory, reverse);
    if (dirty[TuningCustomScale])
    {
        // Why was this done this way?
        //            // tCustom = p->getCustomScale();
        //            Array<float> temp = p->getCustomScale();
        //            for (int i = 0; i < tCustom.size(); i++) tCustom.set(i, temp[i]);
        //            //tCustom = Array<float>(p->getCustomScale());
        //            tScale = CustomTuning;
        tCustom.modify(p->tCustom, reverse);
        tScale = CustomTuning;
    }
    if (dirty[TuningAbsoluteOffsets]) tAbsolute.modify(p->tAbsolute, reverse);
    if (dirty[TuningNToneSemitoneWidth]) nToneSemitoneWidth.modify(p->nToneSemitoneWidth, reverse);
    if (dirty[TuningNToneRootCB]) nToneRoot.modify(p->nToneRoot, reverse);
    if (dirty[TuningAdaptiveSystem]) adaptiveType.modify(p->adaptiveType, reverse);
    
    stuning->performModification(p->getSpringTuning(), dirty, reverse);
    
    if (adaptiveType == AdaptiveSpring)
    {
        stuning->setActive(true);
    }
    else
    {
        stuning->setActive(false);
    }
    
    modded = !reverse;
}




TuningProcessor::TuningProcessor(BKAudioProcessor& processor, Tuning::Ptr tuning):
processor(processor),
tuning(tuning),
keymaps(Keymap::PtrArr()),
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
    if(tuning->prep->getAdaptiveType() == AdaptiveNormal || tuning->prep->getAdaptiveType() == AdaptiveAnchored)
    {
        float lastNoteOffset = adaptiveCalculate(midiNoteNumber) + tuning->prep->getFundamentalOffset(); // added getFundamentalOffset()
        
        if(updateLastInterval)
        {
            lastNoteTuning = midiNoteNumber + lastNoteOffset;
            lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
        }

        return lastNoteOffset;
    }
    
    
    //do nTone tuning if nToneSemitoneWidth != 100cents
    if(tuning->prep->getNToneSemitoneWidth() != 100)
    {
        lastNoteOffset = .01 * (midiNoteNumber - tuning->prep->getNToneRoot()) * (tuning->prep->getNToneSemitoneWidth() - 100);
        int midiNoteNumberTemp = round(midiNoteNumber + lastNoteOffset);
        
        Array<float> currentTuning;
        if(tuning->prep->getScale() == CustomTuning) currentTuning = tuning->prep->getCustomScale();
        else currentTuning = tuning->tuningLibrary.getUnchecked(tuning->prep->getScale());
        
        lastNoteOffset += (currentTuning[(midiNoteNumberTemp - tuning->prep->getFundamental()) % currentTuning.size()]
                          + tuning->prep->getAbsoluteOffsets().getUnchecked(midiNoteNumber)
                          + tuning->prep->getFundamentalOffset());
        
        if(updateLastInterval)
        {
            lastNoteTuning = midiNoteNumber + lastNoteOffset;
            lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
        }
        
        return lastNoteOffset;
    }

    tuning->prep->print();
    //else do regular tunings
    Array<float> currentTuning;
    if(tuning->prep->getScale() == CustomTuning)
        currentTuning = tuning->prep->getCustomScale();
    else
        currentTuning = tuning->tuningLibrary.getUnchecked(tuning->prep->getScale());

    
    lastNoteOffset = (currentTuning[(midiNoteNumber - tuning->prep->getFundamental()) % currentTuning.size()]
                      + tuning->prep->getAbsoluteOffsets().getUnchecked(midiNoteNumber)
                      + tuning->prep->getFundamentalOffset());

    if(updateLastInterval)
    {
        lastNoteTuning = midiNoteNumber + lastNoteOffset;
        lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
    }
    
    return lastNoteOffset;
    
}

int TuningProcessor::getAdaptiveClusterTimer()
{
     return clusterTime * (1000.0 / processor.getCurrentSampleRate());
}


//for keeping track of current cluster size
void TuningProcessor::processBlock(int numSamples)
{
    TuningAdaptiveSystemType type = tuning->prep->getAdaptiveType();
    
    if (type == AdaptiveNormal || type == AdaptiveAnchored) {
        
        if(clusterTime <= (tuning->prep->getAdaptiveClusterThresh() * processor.getCurrentSampleRate() * 0.001))
            clusterTime += numSamples;
    }
}

void TuningProcessor::keyReleased(int midiNoteNumber)
{
    tuning->prep->getSpringTuning()->removeNote(midiNoteNumber);
}


//add note to the adaptive tuning history, update adaptive fundamental
void TuningProcessor::keyPressed(int midiNoteNumber)
{
    //DBG("TuningProcessor::keyPressed " + String(midiNoteNumber));
    
    adaptiveHistoryCounter++;
    
    TuningAdaptiveSystemType type = tuning->prep->getAdaptiveType();

    if (type == AdaptiveNormal)
    {
        //if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory() - 1)
        if (clusterTime * (1000.0 / processor.getCurrentSampleRate()) > tuning->prep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->prep->getAdaptiveHistory())
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
        if (clusterTime * (1000.0 / processor.getCurrentSampleRate()) > tuning->prep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->prep->getAdaptiveHistory())
        {
            adaptiveHistoryCounter = 0;
            
            const Array<float> anchorTuning = tuning->tuningLibrary.getUnchecked(tuning->prep->getAdaptiveAnchorScale());
            adaptiveFundamentalFreq = mtof(midiNoteNumber +
                                           anchorTuning[(midiNoteNumber + tuning->prep->getAdaptiveAnchorFundamental()) % anchorTuning.size()],
                                           globalTuningReference
                                           );
            adaptiveFundamentalNote = midiNoteNumber;

        }
        //else adaptiveHistoryCounter++;
    }
    
    tuning->prep->getSpringTuning()->addNote(midiNoteNumber);
    
    clusterTime = 0;
    
}

float TuningProcessor::adaptiveCalculateRatio(const int midiNoteNumber) const
{
    int tempnote = midiNoteNumber;
    float newnote;
    float newratio;
    
    const Array<float> intervalScale = tuning->tuningLibrary.getUnchecked(tuning->prep->getAdaptiveIntervalScale());
    
    if(!tuning->prep->getAdaptiveInversional() || tempnote >= adaptiveFundamentalNote)
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
    return ftom(newnote, globalTuningReference) - midiNoteNumber;
}

void TuningProcessor::adaptiveReset()
{
    adaptiveFundamentalNote = tuning->prep->getFundamental();
    adaptiveFundamentalFreq = mtof(adaptiveFundamentalNote, globalTuningReference);
    adaptiveHistoryCounter = 0;
    
}


ValueTree Tuning::getState(bool active)
{
    ValueTree vt(vtagTuning);
    
    vt.setProperty( "Id", Id, 0);
    vt.setProperty( "name", name, 0);
    
    vt.addChild(prep->getState(), -1, 0);
    
    return vt;
}

void Tuning::setState(XmlElement* e)
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

			XmlElement* xml = vt1.createXml();

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

			XmlElement* xml = vt1.createXml();

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

			XmlElement* xml = vt1.createXml();

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
