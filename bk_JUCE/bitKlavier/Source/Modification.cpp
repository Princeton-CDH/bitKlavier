/*
  ==============================================================================

    Modification.cpp
    Created: 21 Mar 2019 5:02:28pm
    Author:  airship

  ==============================================================================
*/

#include "Modification.h"
#include "PluginProcessor.h"


DirectModification::DirectModification(BKAudioProcessor& processor, int Id):
Modification(processor, Id, DirectParameterTypeNil),
prep(new DirectPreparation(-1))
{
    
}

ValueTree DirectModification::getState(void)
{
    ValueTree _prep(vtagModDirect);
    
    _prep.setProperty( "Id", Id, 0);
    _prep.setProperty( "name", _getName(), 0);
    _prep.setProperty("alt", altMod, 0);
    ValueTree dirtyVT( "dirty");
    int count = 0;
    for (auto b : dirty)
    {
        dirtyVT.setProperty( "d" + String(count++), (int)b, 0);
    }
    _prep.addChild(dirtyVT, -1, 0);
    
    _prep.addChild(prep->getState(), -1, 0);
    
    return _prep;
}

void DirectModification::setState(XmlElement* e)
{
    Id = e->getStringAttribute("Id").getIntValue();
    
    String n = e->getStringAttribute("name");
    
    altMod = e->getBoolAttribute("alt", false);
    
    if (n != String())     _setName(n);
    else                        _setName(String(Id));
    
    XmlElement* dirtyXml = e->getChildByName("dirty");
    XmlElement* paramsXml = e->getChildByName("params");
    
    if (dirtyXml != nullptr && paramsXml != nullptr)
    {
        dirty.clear();
        for (int k = 0; k < DirectParameterTypeNil; k++)
        {
            String attr = dirtyXml->getStringAttribute("d" + String(k));
            
            if (attr == String()) dirty.add(false);
            else
            {
                dirty.add((bool)attr.getIntValue());
            }
        }
        
        prep->setState(paramsXml);
        if (!prep->useGlobalSoundSet.value)
        {
            // comes in as "soundfont.sf2.subsound1"
            String name = prep->soundSetName.value;
            int Id = processor.findPathAndLoadSamples(name);
            prep->setSoundSet(Id);
        }
    }
    else
    {
        setStateOld(e);
    }
}

void DirectModification::setStateOld(XmlElement* e)
{
    reset();
    
    float f;
    DirectPreparation* d = dynamic_cast<DirectPreparation*>(prep.get());
    String p = e->getStringAttribute(ptagDirect_gain);
    if (p != "")
    {
        // Assuming everything in here is for backwards compatibility
        // should be fine to always set mod time to 0
        prep->defaultGain.setMod(p.getFloatValue());
        prep->defaultGain.setTime(0);
        setDirty(DirectGain);
    }
    
    p = e->getStringAttribute(ptagDirect_hammerGain);
    if (p != "")
    {
        d->dHammerGain.setMod(p.getFloatValue());
        d->dHammerGain.setTime(0);
        setDirty(DirectHammerGain);
    }
    
    p = e->getStringAttribute(ptagDirect_resGain);
    if (p != "")
    {
        d->dResonanceGain.setMod(p.getFloatValue());
        d->dResonanceGain.setTime(0);
        setDirty(DirectResGain);
    }
    
    for (auto sub : e->getChildIterator())
    {
        if (sub->hasTagName(vtagDirect_transposition))
        {
            Array<float> transp;
            
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    transp.add(f);
                }
            }
            
            d->dTransposition.set(transp);
            setDirty(DirectTransposition);
        }
        else if (sub->hasTagName(vtagDirect_ADSR))
        {
            Array<float> envelope;
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    envelope.add(f);
                }
            }
            
            d->setADSRvals(envelope);
            setDirty(DirectADSR);
        }
    }
}


SynchronicModification::SynchronicModification(BKAudioProcessor& processor, int Id):
Modification(processor, Id, SynchronicParameterTypeNil),
SynchronicPreparation()
{

}

ValueTree SynchronicModification::getState(void)
{
    ValueTree prep(vtagModSynchronic);
    
    prep.setProperty( "Id", Id, 0);
    prep.setProperty( "name", getName(), 0);
    prep.setProperty("alt", altMod, 0);
    ValueTree dirtyVT( "dirty");
    int count = 0;
    for (auto b : dirty)
    {
        dirtyVT.setProperty( "d" + String(count++), (int)b, 0);
    }
    prep.addChild(dirtyVT, -1, 0);
    
    prep.addChild(SynchronicPreparation::getState(), -1, 0);
    
    return prep;
}

void SynchronicModification::setState(XmlElement* e)
{
    Id = e->getStringAttribute("Id").getIntValue();
    
    String n = e->getStringAttribute("name");
    
    altMod = e->getBoolAttribute("alt", false);
    
    if (n != String())     setName(n);
    else                        setName(String(Id));
    
    XmlElement* dirtyXml = e->getChildByName("dirty");
    XmlElement* paramsXml = e->getChildByName("params");
    
    if (dirtyXml != nullptr && paramsXml != nullptr)
    {
        dirty.clear();
        for (int k = 0; k < SynchronicParameterTypeNil; k++)
        {
            String attr = dirtyXml->getStringAttribute("d" + String(k));
            
            if (attr == String()) dirty.add(false);
            else
            {
                dirty.add((bool)attr.getIntValue());
            }
        }
        
        SynchronicPreparation::setState(paramsXml);
        if (!sUseGlobalSoundSet.value)
        {
            // comes in as "soundfont.sf2.subsound1"
            String name = sSoundSetName.value;
            int Id = processor.findPathAndLoadSamples(name);
            setSoundSet(Id);
        }
    }
    else
    {
        setStateOld(e);
    }
}

void SynchronicModification::setStateOld(XmlElement* e)
{
    reset();
    
    float f;
    
    String p = e->getStringAttribute(ptagSynchronic_numBeats);
    if (p != "")
    {
        sNumBeats.set(p.getIntValue());
        setDirty(SynchronicNumPulses);
    }
    
    p = e->getStringAttribute(ptagSynchronic_clusterMin);
    if (p != "")
    {
        sClusterMin.set(p.getIntValue());
        setDirty(SynchronicClusterMin);
    }
    
    p = e->getStringAttribute(ptagSynchronic_clusterMax);
    if (p != "")
    {
        sClusterMax.set(p.getIntValue());
        setDirty(SynchronicClusterMax);
    }
    
    p = e->getStringAttribute(ptagSynchronic_clusterThresh);
    if (p != "")
    {
        setClusterThresh(p.getFloatValue());
        setDirty(SynchronicClusterThresh);
    }
    
    p = e->getStringAttribute(ptagSynchronic_mode);
    if (p != "")
    {
        sMode.set((SynchronicSyncMode) p.getIntValue());
        setDirty(SynchronicMode);
    }
    
    p = e->getStringAttribute(ptagSynchronic_beatsToSkip);
    if (p != "")
    {
        sBeatsToSkip.set(p.getIntValue());
        setDirty(SynchronicBeatsToSkip);
    }
    
    p = e->getStringAttribute(ptagSynchronic_gain);
    if (p != "")
    {
        sGain.set(p.getFloatValue());
        setDirty(SynchronicGain);
    }
    
    for (auto sub : e->getChildIterator())
    {
        if (sub->hasTagName(vtagSynchronic_beatMults))
        {
            Array<float> beats;
            
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    beats.add(f);
                }
            }
            
            sBeatMultipliers.set(beats);
            setDirty(SynchronicBeatMultipliers);
            
        }
        else  if (sub->hasTagName(vtagSynchronic_accentMults))
        {
            Array<float> accents;
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    accents.add(f);
                }
            }
            
            sAccentMultipliers.set(accents);
            setDirty(SynchronicAccentMultipliers);
            
        }
        else  if (sub->hasTagName(vtagSynchronic_lengthMults))
        {
            Array<float> lens;
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    lens.add(f);
                }
            }
            
            sLengthMultipliers.set(lens);
            setDirty(SynchronicLengthMultipliers);
            
        }
        else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
        {
            Array<Array<float>> atransp;
            int tcount = 0;
            for (auto asub : sub->getChildIterator())
            {
                if (asub->hasTagName("t"+String(tcount++)))
                {
                    Array<float> transp;
                    for (int k = 0; k < asub->getNumAttributes(); k++)
                    {
                        String attr = asub->getStringAttribute(ptagFloat + String(k));
                        
                        if (attr == String()) break;
                        else
                        {
                            f = attr.getFloatValue();
                            transp.add(f);
                        }
                    }
                    atransp.set(tcount-1, transp);
                }
            }
            
            sTransposition.set(atransp);
            setDirty(SynchronicTranspOffsets);
            
        }
        else  if (sub->hasTagName(vtagSynchronic_ADSRs))
        {
            Array<Array<float>> aenvs;
            int tcount = 0;
            for (auto asub : sub->getChildIterator())
            {
                if (asub->hasTagName("e"+String(tcount++)))
                {
                    Array<float> envs;
                    for (int k = 0; k < asub->getNumAttributes(); k++)
                    {
                        String attr = asub->getStringAttribute(ptagFloat + String(k));
                        
                        if (attr == String()) break;
                        else
                        {
                            f = attr.getFloatValue();
                            envs.add(f);
                        }
                    }
                    aenvs.set(tcount-1, envs);
                }
            }
            
            sADSRs.set(aenvs);
            setDirty(SynchronicADSRs);
        }
    }
}

NostalgicModification::NostalgicModification(BKAudioProcessor& processor, int Id):
Modification(processor, Id, NostalgicParameterTypeNil),
NostalgicPreparation()
{

}

ValueTree NostalgicModification::getState(void)
{
    ValueTree prep(vtagModNostalgic);
    
    prep.setProperty( "Id", Id, 0);
    prep.setProperty( "name", getName(), 0);
    prep.setProperty("alt", altMod, 0);
    ValueTree dirtyVT( "dirty");
    int count = 0;
    for (auto b : dirty)
    {
        dirtyVT.setProperty( "d" + String(count++), (int)b, 0);
    }
    prep.addChild(dirtyVT, -1, 0);
    
    prep.addChild(NostalgicPreparation::getState(), -1, 0);
    
    return prep;
}

void NostalgicModification::setState(XmlElement* e)
{
    Id = e->getStringAttribute("Id").getIntValue();
    
    String n = e->getStringAttribute("name");
    
    altMod = e->getBoolAttribute("alt", false);
    
    if (n != String())     setName(n);
    else                        setName(String(Id));
    
    XmlElement* dirtyXml = e->getChildByName("dirty");
    XmlElement* paramsXml = e->getChildByName("params");
    
    if (dirtyXml != nullptr && paramsXml != nullptr)
    {
        dirty.clear();
        for (int k = 0; k < NostalgicParameterTypeNil; k++)
        {
            String attr = dirtyXml->getStringAttribute("d" + String(k));
            
            if (attr == String()) dirty.add(false);
            else
            {
                dirty.add((bool)attr.getIntValue());
            }
        }
        
        NostalgicPreparation::setState(paramsXml);
        if (!nUseGlobalSoundSet.value)
        {
            // comes in as "soundfont.sf2.subsound1"
            String name = nSoundSetName.value;
            int Id = processor.findPathAndLoadSamples(name);
            setSoundSet(Id);
        }
    }
    else
    {
        setStateOld(e);
    }
}

void NostalgicModification::setStateOld(XmlElement* e)
{
    reset();
    
    float f;
    
    String p = e->getStringAttribute(ptagNostalgic_waveDistance);
    if (p != "")
    {
        nWaveDistance.set(p.getIntValue());
        setDirty(NostalgicWaveDistance);
    }
    
    p = e->getStringAttribute(ptagNostalgic_undertow);
    if (p != "")
    {
        nUndertow.set(p.getIntValue());
        setDirty(NostalgicUndertow);
    }
    
    for (auto sub : e->getChildIterator())
    {
        if (sub->hasTagName(vtagNostalgic_transposition))
        {
            Array<float> transp;
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    transp.add(f);
                }
            }
            
            nTransposition.set(transp);
            setDirty(NostalgicTransposition);
            
        }
        
        if (sub->hasTagName(vtagNostalgic_reverseADSR))
        {
            Array<float> revADSR;
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    revADSR.add(f);
                }
            }
            
            setReverseADSRvals(revADSR);
            setDirty(NostalgicReverseADSR);
            
        }
        
        if (sub->hasTagName(vtagNostalgic_undertowADSR))
        {
            Array<float> undADSR;
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    undADSR.add(f);
                }
            }
            
            setUndertowADSRvals(undADSR);
            setDirty(NostalgicUndertowADSR);
            
        }
    }
    
    p = e->getStringAttribute(ptagNostalgic_lengthMultiplier);
    if (p != "")
    {
        nLengthMultiplier.set(p.getFloatValue());
        setDirty(NostalgicLengthMultiplier);
    }
    
    p = e->getStringAttribute(ptagNostalgic_beatsToSkip);
    if (p != "")
    {
        nBeatsToSkip.set(p.getFloatValue());
        setDirty(NostalgicBeatsToSkip);
    }
    
    p = e->getStringAttribute(ptagNostalgic_gain);
    if (p != "")
    {
        nGain.set(p.getFloatValue());
        setDirty(NostalgicGain);
    }
    
    p = e->getStringAttribute(ptagNostalgic_mode);
    if (p != "")
    {
        nMode.set((NostalgicSyncMode) p.getIntValue());
        setDirty(NostalgicMode);
    }
}

ResonanceModification::ResonanceModification(BKAudioProcessor& processor, int Id):
Modification(processor, Id, ResonanceParameterTypeNil),
prep(new ResonancePreparation(-1))
{
    
}

ValueTree ResonanceModification::getState(void)
{
    
    ValueTree _prep(vtagModResonance);
    
    _prep.setProperty( "Id", Id, 0);
    _prep.setProperty( "name", _getName(), 0);
    _prep.setProperty("alt", altMod, 0);
    ValueTree dirtyVT( "dirty");
    int count = 0;
    for (auto b : dirty)
    {
        dirtyVT.setProperty( "d" + String(count++), (int)b, 0);
    }
    _prep.addChild(dirtyVT, -1, 0);
    
    _prep.addChild(prep->getState(), -1, 0);
    
    return _prep;
}

void ResonanceModification::setState(XmlElement* e)
{
    
    Id = e->getStringAttribute("Id").getIntValue();
    
    String n = e->getStringAttribute("name");
    
    altMod = e->getBoolAttribute("alt", false);
    
    if (n != String())     _setName(n);
    else                        _setName(String(Id));
    
    XmlElement* dirtyXml = e->getChildByName("dirty");
    XmlElement* paramsXml = e->getChildByName("params");
    
    if (dirtyXml != nullptr && paramsXml != nullptr)
    {
        dirty.clear();
        for (int k = 0; k < NostalgicParameterTypeNil; k++)
        {
            String attr = dirtyXml->getStringAttribute("d" + String(k));
            
            if (attr == String()) dirty.add(false);
            else
            {
                dirty.add((bool)attr.getIntValue());
            }
        }
        
        prep->setState(paramsXml);
    
        if (!prep->useGlobalSoundSet.value)
        {
            // comes in as "soundfont.sf2.subsound1"
            String name = prep->soundSetName.value;
            int Id = processor.findPathAndLoadSamples(name);
            prep->setSoundSet(Id);
        }
    }
    /*
    else
    {
        setStateOld(e);
    }
     */
     
}

TempoModification::TempoModification(BKAudioProcessor& processor, int Id):
Modification(processor, Id, TempoParameterTypeNil),
TempoPreparation()
{
    
}

void TempoModification::setStateOld(XmlElement* e)
{
    reset();
    
    String p = e->getStringAttribute(ptagTempo_tempo);
    if (p != "")
    {
        setTempo(p.getFloatValue());
        setDirty(TempoBPM);
    }
    
    p = e->getStringAttribute(ptagTempo_system);
    if (p != "")
    {
        setTempoSystem((TempoType) p.getIntValue());
        setDirty(TempoSystem);
    }
    
    p = e->getStringAttribute(ptagTempo_at1Mode);
    if (p != "")
    {
        setAdaptiveTempo1Mode((AdaptiveTempo1Mode) p.getIntValue());
        setDirty(AT1Mode);
    }
    
    p = e->getStringAttribute(ptagTempo_at1History);
    if (p != "")
    {
        setAdaptiveTempo1History(p.getIntValue());
        setDirty(AT1History);
    }
    
    p = e->getStringAttribute(ptagTempo_at1Subdivisions);
    if (p != "")
    {
        setAdaptiveTempo1Subdivisions(p.getFloatValue());
        setDirty(AT1Subdivisions);
    }
    
    p = e->getStringAttribute(ptagTempo_at1Min);
    if (p != "")
    {
        setAdaptiveTempo1Min(p.getFloatValue());
        setDirty(AT1Min);
    }
    
    p = e->getStringAttribute(ptagTempo_at1Max);
    if (p != "")
    {
        setAdaptiveTempo1Max(p.getFloatValue());
        setDirty(AT1Max);
    }
}



TuningModification::TuningModification(BKAudioProcessor& processor, int Id):
Modification(processor, Id, TuningParameterTypeNil),
TuningPreparation()
{
    //Array<bool> tetherWeightsActive;
    //Array<bool> springWeightsActive;
    
    for(int i=0; i<128; i++) tetherWeightsActive.insert(i, false);
    for(int i=0; i<12; i++) springWeightsActive.insert(i, false);
}

void TuningModification::setStateOld(XmlElement* e)
{
    reset();
    
    String p; float f;
    
    p = e->getStringAttribute( ptagTuning_scale);
    if (p != "")
    {
        setScale((TuningSystem) p.getIntValue());
        setDirty(TuningScale);
    }
   
    p = e->getStringAttribute( ptagTuning_fundamental);
    if (p != "")
    {
        setFundamental((PitchClass) p.getIntValue());
        setDirty(TuningFundamental);
    }
    
    p = e->getStringAttribute( ptagTuning_offset);
    if (p != "")
    {
        setFundamentalOffset(p.getFloatValue());
        setDirty(TuningOffset);
    }
    
    p = e->getStringAttribute( ptagTuning_adaptiveIntervalScale);
    if (p != "")
    {
        setAdaptiveIntervalScale((TuningSystem) p.getIntValue());
        setDirty(TuningA1IntervalScale);
    }
    
    p = e->getStringAttribute( ptagTuning_adaptiveHistory);
    if (p != "")
    {
        setAdaptiveHistory( p.getIntValue());
        setDirty(TuningA1History);
    }
    
    p = e->getStringAttribute( ptagTuning_adaptiveInversional);
    if (p != "")
    {
        setAdaptiveInversional( (bool)p.getIntValue());
        setDirty(TuningA1Inversional);
    }
    
    p = e->getStringAttribute( ptagTuning_adaptiveClusterThresh);
    if (p != "")
    {
        setAdaptiveClusterThresh(p.getIntValue());
        setDirty(TuningA1ClusterThresh);
    }
    
    p = e->getStringAttribute( ptagTuning_adaptiveAnchorFund);
    if (p != "")
    {
        setAdaptiveAnchorFundamental( (PitchClass) p.getIntValue());
        setDirty(TuningA1AnchorFundamental);
    }
    
    p = e->getStringAttribute( ptagTuning_nToneRootCB);
    if (p != "")
    {
        setNToneRootPC(p.getIntValue());
        setDirty(TuningNToneRootCB);
    }
    
    p = e->getStringAttribute( ptagTuning_nToneRootOctaveCB);
    if (p != "")
    {
        setNToneRootOctave(p.getIntValue());
        setDirty(TuningNToneRootOctaveCB);
    }
    
    p = e->getStringAttribute( ptagTuning_nToneSemitoneWidth);
    if (p != "")
    {
        setNToneSemitoneWidth(p.getFloatValue());
        setDirty(TuningNToneSemitoneWidth);
    }
    
    p = e->getStringAttribute( "adaptiveSystem");
    if (p != "")
    {
        setAdaptiveType((TuningAdaptiveSystemType) p.getIntValue());
        setDirty(TuningAdaptiveSystem);
    }


    
    /*TuningSpringTetherStiffness,
     TuningSpringIntervalStiffness,
     TuningSpringRate,
     TuningSpringDrag,
     TuningSpringActive,
     TuningSpringTetherWeights,
     TuningSpringIntervalWeights,
     TuningSpringIntervalScale,*/
    
    p = e->getStringAttribute( ptagTuning_tetherStiffness);
    if (p != "")
    {
        getSpringTuning()->setTetherStiffness(p.getFloatValue());
        setDirty(TuningSpringTetherStiffness);
    }
    
    p = e->getStringAttribute( ptagTuning_intervalStiffness);
    if (p != "")
    {
        getSpringTuning()->setIntervalStiffness(p.getDoubleValue());
        setDirty(TuningSpringIntervalStiffness);
    }
    
    p = e->getStringAttribute( "tetherWeightGlobal");
    if (p != "")
    {
        getSpringTuning()->setTetherWeightGlobal(p.getDoubleValue());
        setDirty(TuningTetherWeightGlobal);
    }
    
    p = e->getStringAttribute( "tetherWeightGlobal2");
    if (p != "")
    {
        getSpringTuning()->setTetherWeightSecondaryGlobal(p.getDoubleValue());
        setDirty(TuningTetherWeightGlobal2);
    }
    
    p = e->getStringAttribute( "fundamentalSetsTether");
    if (p != "")
    {
        getSpringTuning()->setFundamentalSetsTether((bool)p.getIntValue());
        setDirty(TuningFundamentalSetsTether);
    }
    
    p = e->getStringAttribute( ptagTuning_rate);
    if (p != "")
    {
        getSpringTuning()->setRate(p.getDoubleValue());
        setDirty(TuningSpringRate);
    }
    
    p = e->getStringAttribute( ptagTuning_drag);
    if (p != "")
    {
        getSpringTuning()->setDrag(p.getDoubleValue());
        setDirty(TuningSpringDrag);
    }
    
    p = e->getStringAttribute( ptagTuning_active);
    bool springs = (bool) p.getIntValue();
    if (p != "")
    {
        getSpringTuning()->setActive(springs);
        setDirty(TuningSpringActive);
    }
    
    if (springs)
    {
        setAdaptiveType(AdaptiveSpring);
        setDirty(TuningAdaptiveSystem);
    }
    
    p = e->getStringAttribute( ptagTuning_intervalScale);
    if (p != "")
    {
        getSpringTuning()->setScaleId((TuningSystem)p.getIntValue());
        setDirty(TuningSpringIntervalScale);
    }
    
    p = e->getStringAttribute( ptagTuning_intervalScaleFundamental);
    if (p != "")
    {
        getSpringTuning()->setIntervalFundamental((PitchClass)p.getIntValue());
        setDirty(TuningSpringIntervalFundamental);
    }
    
    // custom scale
    for (auto sub : e->getChildIterator())
    {
        if (sub->hasTagName("twa"))
        {
            Array<bool> twa;
            for (int i = 0; i < 128; i++)
            {
                String attr = sub->getStringAttribute("w"+String(i));
                if (attr == String())  twa.add(false);
                else                        twa.add((bool)attr.getIntValue());
            }
            
            setTetherWeightsActive(twa);
        }
        else if (sub->hasTagName("swa"))
        {
            Array<bool> swa;
            for (int k = 0; k < 12; k++)
            {
                String attr = sub->getStringAttribute("w"+String(k));
                if (attr == String())  swa.add(false);
                else                        swa.add((bool)attr.getIntValue());
            }
            
            setSpringWeightsActive(swa);
        }
        else if (sub->hasTagName("tw"))
        {
            Array<bool> tw;
            Array<float> weights;
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute("w"+String(k));
                float val = attr.getFloatValue();
                
                weights.add(val);
            }
            
            getSpringTuning()->setTetherWeights(weights);
            setDirty(TuningSpringTetherWeights);
        }
        else if (sub->hasTagName("sw"))
        {
            Array<bool> sw;
            Array<float> weights;
            for (int k = 0; k < 12; k++)
            {
                String attr = sub->getStringAttribute("w"+String(k));
                float val = attr.getFloatValue();
                
                weights.add(val);
            }
            
            getSpringTuning()->setSpringWeights(weights);
            setDirty(TuningSpringIntervalWeights);
        }
        else if (sub->hasTagName(vtagTuning_customScale))
        {
            // Some old galleries have empty custom scale elements so we
            // need to handle for that by making sure empty custom scales
            // become 0 filled arrays and are not considered dirty for mods
            Array<float> scale;
            for (int i = 0; i < 12; ++i) scale.add(0.0);
            bool dirty = false;
            for (int k = 0; k < sub->getNumAttributes(); k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                f = attr.getFloatValue();
                scale.set(k, f);
                dirty = true;
            }
            
            setCustomScale(scale);
            if (dirty) setDirty(TuningCustomScale);
        }
        else if (sub->hasTagName(vTagTuning_absoluteOffsets))
        {
            Array<float> absolute;
            for (int k = 0; k < ABSOLUTE_OFFSET_SIZE; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                absolute.add(0.0);
                f = attr.getFloatValue() * 100.;
                absolute.set(k, f);
            }
            
            setAbsoluteOffsets(absolute);
            setDirty(TuningAbsoluteOffsets);
        }
    }
}


BlendronicModification::BlendronicModification(BKAudioProcessor& processor, int Id) :
Modification(processor, Id, BlendronicParameterTypeNil),
BlendronicPreparation()
{
    
}

void BlendronicModification::setStateOld(XmlElement* e)
{
    reset();
}

