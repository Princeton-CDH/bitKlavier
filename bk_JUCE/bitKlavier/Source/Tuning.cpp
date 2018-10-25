/*
 ==============================================================================
 
 Tuning.cpp
 Created: 4 Dec 2016 3:40:27pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "Tuning.h"
#define ABSOLUTE_OFFSET_SIZE 256


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
    if(tuning->aPrep->getTuning() == AdaptiveTuning || tuning->aPrep->getTuning() == AdaptiveAnchoredTuning)
    {
        float lastNoteOffset = adaptiveCalculate(midiNoteNumber);
        
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
        if(tuning->aPrep->getTuning() == CustomTuning) currentTuning = tuning->aPrep->getCustomScale();
        else currentTuning = tuning->tuningLibrary.getUnchecked(tuning->aPrep->getTuning());
        
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
    if(tuning->aPrep->getTuning() == CustomTuning) currentTuning = tuning->aPrep->getCustomScale();
    else currentTuning = tuning->tuningLibrary.getUnchecked(tuning->aPrep->getTuning());
    
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
    TuningSystem currentTuning = tuning->getCurrentTuning();
    
    if (currentTuning == AdaptiveTuning || currentTuning == AdaptiveAnchoredTuning) {
        
        if(clusterTime <= (tuning->aPrep->getAdaptiveClusterThresh() * sampleRate * 0.001))
            clusterTime += numSamples;
        
    }
}

void TuningProcessor::keyReleased(int midiNoteNumber)
{
    TuningSystem currentTuning = tuning->getCurrentTuning();
    
    if (currentTuning == SpringTuning)
    {
        tuning->getSpringTuning()->removeNote(midiNoteNumber);
    }
}


//add note to the adaptive tuning history, update adaptive fundamental
void TuningProcessor::keyPressed(int midiNoteNumber)
{
    adaptiveHistoryCounter++;
    
    TuningSystem currentTuning = tuning->getCurrentTuning();

    if (currentTuning == AdaptiveTuning)
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
    else if (currentTuning == AdaptiveAnchoredTuning)
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
    else if (currentTuning == SpringTuning)
    {
        tuning->getSpringTuning()->addNote(midiNoteNumber);
    }
    
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

ValueTree Tuning::getState(void)
{
    
    ValueTree prep(vtagTuning);
    
    prep.setProperty( "Id",Id, 0);
    prep.setProperty( "name",                           name, 0);
    prep.setProperty( ptagTuning_scale,                 sPrep->getTuning(), 0);
    prep.setProperty( ptagTuning_fundamental,           sPrep->getFundamental(), 0);
    prep.setProperty( ptagTuning_offset,                sPrep->getFundamentalOffset(), 0 );
    prep.setProperty( ptagTuning_adaptiveIntervalScale, sPrep->getAdaptiveIntervalScale(), 0 );
    prep.setProperty( ptagTuning_adaptiveInversional,   sPrep->getAdaptiveInversional(), 0 );
    prep.setProperty( ptagTuning_adaptiveAnchorScale,   sPrep->getAdaptiveAnchorScale(), 0 );
    prep.setProperty( ptagTuning_adaptiveAnchorFund,    sPrep->getAdaptiveAnchorFundamental(), 0 );
    prep.setProperty( ptagTuning_adaptiveClusterThresh, (int)sPrep->getAdaptiveClusterThresh(), 0 );
    prep.setProperty( ptagTuning_adaptiveHistory,       sPrep->getAdaptiveHistory(), 0 );
    
    prep.setProperty( ptagTuning_nToneRoot,             sPrep->getNToneRoot(), 0);
    prep.setProperty( ptagTuning_nToneSemitoneWidth,    sPrep->getNToneSemitoneWidth(), 0 );
    
    ValueTree scale( vtagTuning_customScale);
    int count = 0;
    for (auto note : sPrep->getCustomScale())
        scale.setProperty( ptagFloat + String(count++), note, 0 );
    prep.addChild(scale, -1, 0);
    
    ValueTree absolute( vTagTuning_absoluteOffsets);
    count = 0;
    for (auto note : sPrep->getAbsoluteOffsets())
    {
        if(note != 0.) absolute.setProperty( ptagFloat + String(count), note, 0 );
        count++;
    }
    prep.addChild(absolute, -1, 0);
    
    prep.addChild(stuning->getState(), -1, 0);
    
    return prep;
    
}

ValueTree TuningModPreparation::getState(void)
{
    ValueTree prep(vtagModTuning);
    
    prep.setProperty( "Id",Id, 0);
    
    String p = getParam(TuningScale);
    if (p != String::empty) prep.setProperty( ptagTuning_scale, p.getIntValue(), 0);
    
    p = getParam(TuningFundamental);
    if (p != String::empty) prep.setProperty( ptagTuning_fundamental,           p.getIntValue(), 0);
    
    p = getParam(TuningOffset);
    if (p != String::empty) prep.setProperty( ptagTuning_offset,                p.getFloatValue(), 0 );
    
    p = getParam(TuningA1IntervalScale);
    if (p != String::empty) prep.setProperty( ptagTuning_adaptiveIntervalScale, p.getIntValue(), 0 );
    
    p = getParam(TuningA1Inversional);
    if (p != String::empty) prep.setProperty( ptagTuning_adaptiveInversional,   (bool)p.getIntValue(), 0 );
    
    p = getParam(TuningA1AnchorScale);
    if (p != String::empty) prep.setProperty( ptagTuning_adaptiveAnchorScale,   p.getIntValue(), 0 );
    
    p = getParam(TuningA1AnchorFundamental);
    if (p != String::empty) prep.setProperty( ptagTuning_adaptiveAnchorFund,    p.getIntValue(), 0 );
    
    p = getParam(TuningA1ClusterThresh);
    if (p != String::empty) prep.setProperty( ptagTuning_adaptiveClusterThresh, p.getIntValue(), 0 );
    
    p = getParam(TuningA1History);
    if (p != String::empty) prep.setProperty( ptagTuning_adaptiveHistory,       p.getIntValue(), 0 );
    
    p = getParam(TuningNToneRootCB);
    if (p != String::empty) prep.setProperty( ptagTuning_nToneRootCB,           p.getIntValue(), 0 );
    
    p = getParam(TuningNToneRootOctaveCB);
    if (p != String::empty) prep.setProperty( ptagTuning_nToneRootOctaveCB,     p.getIntValue(), 0 );
    
    p = getParam(TuningNToneSemitoneWidth);
    if (p != String::empty) prep.setProperty( ptagTuning_nToneSemitoneWidth,    p.getFloatValue(), 0 );

    
    ValueTree scale( vtagTuning_customScale);
    int count = 0;
    p = getParam(TuningCustomScale);
    if (p != String::empty)
    {
        Array<float> scl = stringToFloatArray(p);
        for (auto note : scl)
            scale.setProperty( ptagFloat + String(count++), note, 0 );
    }
    prep.addChild(scale, -1, 0);
    
    ValueTree absolute( vTagTuning_absoluteOffsets);
    count = 0;
    p = getParam(TuningAbsoluteOffsets);
    if (p != String::empty)
    {
        Array<float> offsets = stringOrderedPairsToFloatArray(p, 128);
        for (auto note : offsets)
        {
            if(note != 0.) absolute.setProperty( ptagFloat + String(count), note * .01, 0 );
            count++;
        }
    }
    prep.addChild(absolute, -1, 0);
    
    return prep;
}

void TuningModPreparation::setState(XmlElement* e)
{
    String p = "";
    
    float f;
    
    Id = e->getStringAttribute("Id").getIntValue();
    
    String n = e->getStringAttribute("name");
    
    if (n != String::empty)     name = n;
    else                        name = "tm"+String(Id);
    
    p = e->getStringAttribute( ptagTuning_scale);
    setParam(TuningScale, p);
    
    p = e->getStringAttribute( ptagTuning_fundamental);
    setParam(TuningFundamental, p);
    
    p = e->getStringAttribute( ptagTuning_offset);
    setParam(TuningOffset, p);
    
    p = e->getStringAttribute( ptagTuning_adaptiveIntervalScale);
    setParam(TuningA1IntervalScale, p);
    
    p = e->getStringAttribute( ptagTuning_adaptiveAnchorScale);
    setParam(TuningA1AnchorScale, p);
    
    p = e->getStringAttribute( ptagTuning_adaptiveHistory);
    setParam(TuningA1History, p);
    
    p = e->getStringAttribute( ptagTuning_adaptiveInversional);
    setParam(TuningA1Inversional, p);
    
    p = e->getStringAttribute( ptagTuning_adaptiveClusterThresh);
    setParam(TuningA1ClusterThresh, p);
    
    p = e->getStringAttribute( ptagTuning_adaptiveAnchorFund);
    setParam(TuningA1AnchorFundamental, p);
    
    p = e->getStringAttribute( ptagTuning_nToneRootCB);
    setParam(TuningNToneRootCB, p);
    
    p = e->getStringAttribute( ptagTuning_nToneRootOctaveCB);
    setParam(TuningNToneRootOctaveCB, p);
    
    p = e->getStringAttribute( ptagTuning_nToneSemitoneWidth);
    setParam(TuningNToneSemitoneWidth, p);
    
    // custom scale
    forEachXmlChildElement (*e, sub)
    {
        if (sub->hasTagName(vtagTuning_customScale))
        {
            Array<float> scale;
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String::empty) break;
                else
                {
                    f = attr.getFloatValue();
                    scale.add(f);
                }
            }
            
            setParam(TuningCustomScale, floatArrayToString(scale));
        }
        else if (sub->hasTagName(vTagTuning_absoluteOffsets))
        {
            Array<float> absolute;
            absolute.ensureStorageAllocated(ABSOLUTE_OFFSET_SIZE);
            String abs = "";
            
            for (int k = 0; k < ABSOLUTE_OFFSET_SIZE; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                f = attr.getFloatValue() * 100.;
                absolute.set(k, f);
                if (f != 0.0) abs += (String(k) + ":" + String(f) + " ");
                
            }
            
            setParam(TuningAbsoluteOffsets, abs);
        }
    }
}

void Tuning::setState(XmlElement* e)
{
    int i; float f; bool b;
    
    String n = e->getStringAttribute("name");
    
    if (n != String::empty)     name = n;
    else                        name = "Tuning"+String(Id);

    Id = e->getStringAttribute("Id").getIntValue();
    
    i = e->getStringAttribute( ptagTuning_scale).getIntValue();
    sPrep->setTuning((TuningSystem)i);
    
    i = e->getStringAttribute( ptagTuning_fundamental).getIntValue();
    sPrep->setFundamental((PitchClass)i);
    
    f = e->getStringAttribute( ptagTuning_offset).getFloatValue();
    sPrep->setFundamentalOffset(f);
    
    i = e->getStringAttribute( ptagTuning_adaptiveIntervalScale).getIntValue();
    sPrep->setAdaptiveIntervalScale((TuningSystem)i);
    
    i = e->getStringAttribute( ptagTuning_adaptiveAnchorScale).getIntValue();
    sPrep->setAdaptiveAnchorScale((TuningSystem)i);
    
    i = e->getStringAttribute( ptagTuning_adaptiveHistory).getIntValue();
    sPrep->setAdaptiveHistory(i);
    
    b = (bool) e->getStringAttribute( ptagTuning_adaptiveInversional).getIntValue();
    sPrep->setAdaptiveInversional(b);
    
    i = e->getStringAttribute( ptagTuning_adaptiveClusterThresh).getIntValue();
    sPrep->setAdaptiveClusterThresh(i);
    
    i = e->getStringAttribute( ptagTuning_adaptiveAnchorFund).getIntValue();
    sPrep->setAdaptiveAnchorFundamental((PitchClass)i);
    
    i = e->getStringAttribute( ptagTuning_nToneRoot).getIntValue();
    if(i > 0) sPrep->setNToneRoot(i);
    else sPrep->setNToneRoot(60);
    
    f = e->getStringAttribute( ptagTuning_nToneSemitoneWidth).getFloatValue();
    if(f > 0) sPrep->setNToneSemitoneWidth(f);
    else sPrep->setNToneSemitoneWidth(100);
    
    // custom scale
    forEachXmlChildElement (*e, sub)
    {
        if (sub->hasTagName("springtuning"))
        {
            stuning->setState(sub);
        }
        else if (sub->hasTagName(vtagTuning_customScale))
        {
            Array<float> scale;
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String::empty) break;
                else
                {
                    f = attr.getFloatValue();
                    scale.add(f);
                }
            }
            
            sPrep->setCustomScale(scale);
        }
        else if (sub->hasTagName(vTagTuning_absoluteOffsets))
        {
            Array<float> absolute;
            absolute.ensureStorageAllocated(ABSOLUTE_OFFSET_SIZE);
            for (int k = 0; k < ABSOLUTE_OFFSET_SIZE; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                f = attr.getFloatValue();
                absolute.set(k, f);
                
            }
            
            sPrep->setAbsoluteOffsets(absolute);
        }
    }
    
    // copy static to active
    aPrep->copy( sPrep);
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

static TuningTests directTests;

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
			TuningModPreparation::Ptr tm1 = new TuningModPreparation(tp1, 1);


			tm1->randomize();
			tm1->setName(name);

			ValueTree vt1 = tm1->getState();

			ScopedPointer<XmlElement> xml = vt1.createXml();

			TuningPreparation::Ptr tp2 = new TuningPreparation();
			TuningModPreparation::Ptr tm2 = new TuningModPreparation(tp2, 1);

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
