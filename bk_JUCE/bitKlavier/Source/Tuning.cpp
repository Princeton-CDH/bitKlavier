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
float TuningProcessor::getOffset(int midiNoteNumber)
{
    float lastNoteTuningTemp = lastNoteTuning;
    float lastNoteOffset;
 
    //do adaptive tunings if using
    if(tuning->aPrep->getTuning() == AdaptiveTuning || tuning->aPrep->getTuning() == AdaptiveAnchoredTuning)
    {
        float lastNoteOffset = adaptiveCalculate(midiNoteNumber);
        lastNoteTuning = midiNoteNumber + lastNoteOffset;
        lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
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
        
        
        lastNoteTuning = midiNoteNumber + lastNoteOffset;
        lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
        
        return lastNoteOffset;
    }

    //else do regular tunings
    Array<float> currentTuning;
    if(tuning->aPrep->getTuning() == CustomTuning) currentTuning = tuning->aPrep->getCustomScale();
    else currentTuning = tuning->tuningLibrary.getUnchecked(tuning->aPrep->getTuning());
    
    lastNoteOffset = (currentTuning[(midiNoteNumber - tuning->aPrep->getFundamental()) % currentTuning.size()] +
                      + tuning->aPrep->getAbsoluteOffsets().getUnchecked(midiNoteNumber) +
                      tuning->aPrep->getFundamentalOffset());
    
    
    lastNoteTuning = midiNoteNumber + lastNoteOffset;
    lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
    
    return lastNoteOffset;
    
}


//for keeping track of current cluster size
void TuningProcessor::processBlock(int numSamples)
{
    if(tuning->aPrep->getTuning() == AdaptiveTuning || tuning->aPrep->getTuning() == AdaptiveAnchoredTuning) {
        
        if(clusterTime <= tuning->aPrep->getAdaptiveClusterThresh() * sampleRate * 0.001) clusterTime += numSamples;
        
    }
}


//add note to the adaptive tuning history, update adaptive fundamental
void TuningProcessor::keyPressed(int midiNoteNumber)
{

    if(tuning->aPrep->getTuning() == AdaptiveTuning)
    {
        if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory() - 1)
        {
            adaptiveHistoryCounter = 0;
            adaptiveFundamentalFreq = adaptiveFundamentalFreq * adaptiveCalculateRatio(midiNoteNumber);
            adaptiveFundamentalNote = midiNoteNumber;
        }
        else adaptiveHistoryCounter++;
        
    }
    
    else if(tuning->aPrep->getTuning() == AdaptiveAnchoredTuning)
    {
        if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory() - 1)
        {
            adaptiveHistoryCounter = 0;
            
            const Array<float> anchorTuning = tuning->tuningLibrary.getUnchecked(tuning->aPrep->getAdaptiveAnchorScale());
            adaptiveFundamentalFreq = mtof(midiNoteNumber +
                                           anchorTuning[(midiNoteNumber + tuning->aPrep->getAdaptiveAnchorFundamental()) % anchorTuning.size()]
                                           );
            adaptiveFundamentalNote = midiNoteNumber;
        }
        else adaptiveHistoryCounter++;
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
    if (p != String::empty) prep.setProperty( ptagTuning_nToneSemitoneWidth,    p.getIntValue(), 0 );

    
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
    
    p = e->getStringAttribute( ptagTuning_nToneRoot);
    setParam(TuningNToneRoot, p);
    
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
            absolute.ensureStorageAllocated(128);
            String abs = "";
            
            for (int k = 0; k < 128; k++)
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
    
    i = e->getStringAttribute( ptagTuning_nToneSemitoneWidth).getIntValue();
    if(i > 0) sPrep->setNToneSemitoneWidth(i);
    else sPrep->setNToneSemitoneWidth(100);
    
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
            
            sPrep->setCustomScale(scale);
        }
        else if (sub->hasTagName(vTagTuning_absoluteOffsets))
        {
            Array<float> absolute;
            absolute.ensureStorageAllocated(128);
            for (int k = 0; k < 128; k++)
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


