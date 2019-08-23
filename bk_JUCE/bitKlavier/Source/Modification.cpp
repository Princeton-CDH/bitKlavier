/*
  ==============================================================================

    Modification.cpp
    Created: 21 Mar 2019 5:02:28pm
    Author:  airship

  ==============================================================================
*/

#include "Modification.h"

void DirectModification::setStateOld(XmlElement* e)
{
    reset();
    
    float f;
    
    String p = e->getStringAttribute(ptagDirect_gain);
    if (p != "")
    {
        setGain(p.getFloatValue());
        setDirty(DirectGain);
    }
    
    p = e->getStringAttribute(ptagDirect_hammerGain);
    if (p != "")
    {
        setHammerGain(p.getFloatValue());
        setDirty(DirectHammerGain);
    }
    
    p = e->getStringAttribute(ptagDirect_resGain);
    if (p != "")
    {
        setResonanceGain(p.getFloatValue());
        setDirty(DirectResGain);
    }
    
    forEachXmlChildElement (*e, sub)
    {
        if (sub->hasTagName(vtagDirect_transposition))
        {
            Array<float> transp;
            
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    transp.add(f);
                }
            }
            
            setTransposition(transp);
            setDirty(DirectTransposition);
        }
        else if (sub->hasTagName(vtagDirect_ADSR))
        {
            Array<float> envelope;
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    envelope.add(f);
                }
            }
            
            setADSRvals(envelope);
            setDirty(DirectADSR);
        }
    }
}

void SynchronicModification::setStateOld(XmlElement* e)
{
    reset();
    
    float f;
    
    String p = e->getStringAttribute(ptagSynchronic_numBeats);
    if (p != "")
    {
        setNumBeats(p.getIntValue());
        setDirty(SynchronicNumPulses);
    }
    
    p = e->getStringAttribute(ptagSynchronic_clusterMin);
    if (p != "")
    {
        setClusterMin(p.getIntValue());
        setDirty(SynchronicClusterMin);
    }
    
    p = e->getStringAttribute(ptagSynchronic_clusterMax);
    if (p != "")
    {
        setClusterMax(p.getIntValue());
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
        setMode((SynchronicSyncMode) p.getIntValue());
        setDirty(SynchronicMode);
    }
    
    p = e->getStringAttribute(ptagSynchronic_beatsToSkip);
    if (p != "")
    {
        setBeatsToSkip(p.getIntValue());
        setDirty(SynchronicBeatsToSkip);
    }
    
    p = e->getStringAttribute(ptagSynchronic_gain);
    if (p != "")
    {
        setGain(p.getFloatValue());
        setDirty(SynchronicGain);
    }
    
    forEachXmlChildElement (*e, sub)
    {
        if (sub->hasTagName(vtagSynchronic_beatMults))
        {
            Array<float> beats;
            
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    beats.add(f);
                }
            }
            
            setBeatMultipliers(beats);
            setDirty(SynchronicBeatMultipliers);
            
        }
        else  if (sub->hasTagName(vtagSynchronic_accentMults))
        {
            Array<float> accents;
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    accents.add(f);
                }
            }
            
            setAccentMultipliers(accents);
            setDirty(SynchronicAccentMultipliers);
            
        }
        else  if (sub->hasTagName(vtagSynchronic_lengthMults))
        {
            Array<float> lens;
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    lens.add(f);
                }
            }
            
            setLengthMultipliers(lens);
            setDirty(SynchronicLengthMultipliers);
            
        }
        else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
        {
            Array<Array<float>> atransp;
            int tcount = 0;
            forEachXmlChildElement (*sub, asub)
            {
                if (asub->hasTagName("t"+String(tcount++)))
                {
                    Array<float> transp;
                    for (int k = 0; k < 128; k++)
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
            
            setTransposition(atransp);
            setDirty(SynchronicTranspOffsets);
            
        }
        else  if (sub->hasTagName(vtagSynchronic_ADSRs))
        {
            Array<Array<float>> aenvs;
            int tcount = 0;
            forEachXmlChildElement (*sub, asub)
            {
                if (asub->hasTagName("e"+String(tcount++)))
                {
                    Array<float> envs;
                    for (int k = 0; k < 128; k++)
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
            
            setADSRs(aenvs);
            setDirty(SynchronicADSRs);
        }
    }
    
}

void NostalgicModification::setStateOld(XmlElement* e)
{
    reset();
    
    float f;
    
    String p = e->getStringAttribute(ptagNostalgic_waveDistance);
    if (p != "")
    {
        setWaveDistance(p.getIntValue());
        setDirty(NostalgicWaveDistance);
    }
    
    p = e->getStringAttribute(ptagNostalgic_undertow);
    if (p != "")
    {
        setUndertow(p.getIntValue());
        setDirty(NostalgicUndertow);
    }
    
    forEachXmlChildElement (*e, sub)
    {
        if (sub->hasTagName(vtagNostalgic_transposition))
        {
            Array<float> transp;
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    transp.add(f);
                }
            }
            
            setTransposition(transp);
            setDirty(NostalgicTransposition);
            
        }
        
        if (sub->hasTagName(vtagNostalgic_reverseADSR))
        {
            Array<float> revADSR;
            for (int k = 0; k < 128; k++)
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
            for (int k = 0; k < 128; k++)
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
        setLengthMultiplier(p.getFloatValue());
        setDirty(NostalgicLengthMultiplier);
    }
    
    p = e->getStringAttribute(ptagNostalgic_beatsToSkip);
    if (p != "")
    {
        setBeatsToSkip(p.getFloatValue());
        setDirty(NostalgicBeatsToSkip);
    }
    
    p = e->getStringAttribute(ptagNostalgic_gain);
    if (p != "")
    {
        setGain(p.getFloatValue());
        setDirty(NostalgicGain);
    }
    
    p = e->getStringAttribute(ptagNostalgic_mode);
    if (p != "")
    {
        setMode((NostalgicSyncMode) p.getIntValue());
        setDirty(NostalgicMode);
    }
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
    forEachXmlChildElement (*e, sub)
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
            Array<float> scale;
            for (int k = 0; k < 128; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                
                if (attr == String()) break;
                else
                {
                    f = attr.getFloatValue();
                    scale.add(f);
                }
            }
            
            setCustomScale(scale);
            setDirty(TuningCustomScale);
        }
        else if (sub->hasTagName(vTagTuning_absoluteOffsets))
        {
            Array<float> absolute;
            absolute.ensureStorageAllocated(ABSOLUTE_OFFSET_SIZE);
            
            for (int k = 0; k < ABSOLUTE_OFFSET_SIZE; k++)
            {
                String attr = sub->getStringAttribute(ptagFloat + String(k));
                f = attr.getFloatValue() * 100.;
                absolute.set(k, f);
                
            }
            
            setAbsoluteOffsets(absolute);
            setDirty(TuningAbsoluteOffsets);
        }
    }
}



