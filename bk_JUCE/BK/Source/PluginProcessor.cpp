
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BKPianoSampler.h"

#define NOST_KEY_OFF 1

//==============================================================================
BKAudioProcessor::BKAudioProcessor():
general                 (new GeneralSettings()),
mainPianoSynth          (general),
hammerReleaseSynth      (general),
resonanceReleaseSynth   (general)
{
    didLoadHammersAndRes            = false;
    didLoadMainPianoSamples         = false;
    pianoDidChange                  = false;
    directPreparationDidChange      = false;
    nostalgicPreparationDidChange   = false;
    tuningPreparationDidChange      = false;
    synchronicPreparationDidChange  = false;
    
    bkKeymaps       .ensureStorageAllocated(aMaxNumPreparationKeymaps);
    bkPianos        .ensureStorageAllocated(aMaxNumPianos);
    prevPianos      .ensureStorageAllocated(aMaxNumPianos);
    
    tuning          .ensureStorageAllocated(aMaxTuningPreparations);
    modTuning       .ensureStorageAllocated(aMaxTuningPreparations);
    
    synchronic      .ensureStorageAllocated(aMaxTotalPreparations);
    modSynchronic   .ensureStorageAllocated(aMaxTotalPreparations);
    
    nostalgic       .ensureStorageAllocated(aMaxTotalPreparations);
    modNostalgic    .ensureStorageAllocated(aMaxTotalPreparations);
    
    direct          .ensureStorageAllocated(aMaxTotalPreparations);
    modDirect       .ensureStorageAllocated(aMaxTotalPreparations);


    bkKeymaps.add(new Keymap(0));
    bkKeymaps.add(new Keymap(1));
    
    // Start with one of each kind of preparation.
    for (int i = 0; i < 2; i++)
    {
        addTuning(); // always create first tuning first
        addSynchronic();
        addNostalgic();
        addDirect();
        
        addDirectMod();
        addSynchronicMod();
        addNostalgicMod();
        addTuningMod();
        
        nostalgic[i]->sPrep->setSyncTargetProcessor(synchronic[0]->processor);
        nostalgic[i]->aPrep->setSyncTargetProcessor(synchronic[0]->processor);
    }

    // Make a bunch of pianos. Default to zeroth keymap.
    for (int i = 0 ; i < aMaxNumPianos; i++)
    {
        bkPianos.set(i, new Piano(synchronic, nostalgic, direct,
                                  bkKeymaps[0], i)); // initializing piano 0

    }
  
    // Initialize first piano.
    prevPiano = bkPianos[0];
    currentPiano = bkPianos[0];
    
    // Default all on for 
    for (int i = 0; i < 128; i++) bkKeymaps[1]->addNote(i);
    
}

void BKAudioProcessor::addDirectMod()
{
    modDirect.add           (new DirectModPreparation());
}

void BKAudioProcessor::addSynchronicMod()
{
    modSynchronic.add       (new SynchronicModPreparation());
}

void BKAudioProcessor::addNostalgicMod()
{
    modNostalgic.add        (new NostalgicModPreparation());
}

void BKAudioProcessor::addTuningMod()
{
    modTuning.add           (new TuningModPreparation());
}

void BKAudioProcessor::addKeymap(void)
{
    int numKeymaps = bkKeymaps.size();
    bkKeymaps.add(new Keymap(numKeymaps));
}

void BKAudioProcessor::addSynchronic(void)
{
    int numSynchronic = synchronic.size();
    synchronic.add(new Synchronic(&mainPianoSynth, tuning[0], general, numSynchronic));
    synchronic.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

void BKAudioProcessor::addNostalgic(void)
{
    int numNostalgic = nostalgic.size();
    nostalgic.add(new Nostalgic(&mainPianoSynth, tuning[0], numNostalgic));
    nostalgic.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

void BKAudioProcessor::addTuning(void)
{
    int numTuning = tuning.size();
    tuning.add(new Tuning(numTuning));
}

void BKAudioProcessor::addDirect(void)
{
    int numDirect = direct.size();
    direct.add(new Direct(&mainPianoSynth, &resonanceReleaseSynth, &hammerReleaseSynth, tuning[0], numDirect));
    direct.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}


void BKAudioProcessor::loadGallery(void)
{
     
    FileChooser myChooser ("Load gallery from file...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.xml");
    
    float f;
    int i;
    bool b;
    Array<float> fa;
    Array<int> fi;

    int pianoCount = 0, sPrepCount = 1, sModPrepCount = 1, nPrepCount = 1, nModPrepCount = 1, dPrepCount = 1, dModPrepCount = 1, tPrepCount = 1, tModPrepCount = 1, keymapCount = 1;
    
    if (myChooser.browseForFileToOpen())
    {
        File myFile (myChooser.getResult());
        
        ScopedPointer<XmlElement> xml (XmlDocument::parse (myFile));
        
        if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
        {
            
            /* * * * Clear gallery * * * */
            // Should thread this or prevent audio from happening
            synchronic.clearQuick();
            nostalgic.clearQuick();
            direct.clearQuick();
            tuning.clearQuick();
            
            addTuning();
            addSynchronic();
            addNostalgic();
            addDirect();
            
            modSynchronic.clearQuick();
            modNostalgic.clearQuick();
            modDirect.clearQuick();
            modTuning.clearQuick();
            
            addTuningMod();
            addSynchronicMod();
            addNostalgicMod();
            addDirectMod();
            
            bkKeymaps.clearQuick();
            bkPianos.clearQuick();
            
           
            /* * * * * * * * * * * * * * */
            
            bkKeymaps.set(0, new Keymap(0));
            
            // iterate through its sub-elements
            forEachXmlChildElement (*xml, e)
            {
                if (e->hasTagName( vtagKeymap + String(keymapCount)))
                {
                    int id = keymapCount++;
                    
                    bkKeymaps.set(id, new Keymap(id));
                    
                    Array<int> keys;
                    for (int k = 0; k < 128; k++)
                    {
                        String attr = e->getStringAttribute(ptagKeymap_key + String(k));
                        
                        if (attr == String::empty) break;
                        else
                        {
                            i = attr.getIntValue();
                            keys.add(i);
                        }
                    }
                    
                    bkKeymaps[id]->setKeymap(keys);
                    
                }
                else if (e->hasTagName ( vtagGeneral))
                {
                    f = e->getStringAttribute( ptagGeneral_globalGain ).getFloatValue();
                    general->setGlobalGain(f);
                    
                    f = e->getStringAttribute( ptagGeneral_hammerGain ).getFloatValue();
                    general->setHammerGain(f);
                    
                    f = e->getStringAttribute( ptagGeneral_resonanceGain ).getFloatValue();
                    general->setResonanceGain(f);
                    
                    f = e->getStringAttribute( ptagGeneral_directGain ).getFloatValue();
                    general->setDirectGain(f);
                    
                    f = e->getStringAttribute( ptagGeneral_nostalgicGain ).getFloatValue();
                    general->setNostalgicGain(f);
                    
                    f = e->getStringAttribute( ptagGeneral_tempoMultiplier ).getFloatValue();
                    general->setTempoMultiplier(f);
                    
                    f = e->getStringAttribute( ptagGeneral_tuningFund ).getFloatValue();
                    general->setTuningFundamental(f);
                    
                    b = (bool) e->getStringAttribute( ptagGeneral_resAndHammer ).getIntValue();
                    general->setResonanceAndHammer(b);
                    
                    b = (bool) e->getStringAttribute( ptagGeneral_invertSustain ).getIntValue();
                    general->setInvertSustain(b);
                    
                }
                else if (e->hasTagName( vtagTuningPrep + String(tPrepCount)))
                {
                    addTuning();
                    
                    int id = tuning.size() - 1;
                    
                    i = e->getStringAttribute( ptagTuning_scale).getIntValue();
                    tuning[id]->sPrep->setTuning((TuningSystem)i);
                    
                    i = e->getStringAttribute( ptagTuning_fundamental).getIntValue();
                    tuning[id]->sPrep->setFundamental((PitchClass)i);
                    
                    f = e->getStringAttribute( ptagTuning_offset).getFloatValue();
                    tuning[id]->sPrep->setFundamentalOffset(f);
                    
                    i = e->getStringAttribute( ptagTuning_adaptiveIntervalScale).getIntValue();
                    tuning[id]->sPrep->setAdaptiveIntervalScale((TuningSystem)i);
                    
                    i = e->getStringAttribute( ptagTuning_adaptiveAnchorScale).getIntValue();
                    tuning[id]->sPrep->setAdaptiveAnchorScale((TuningSystem)i);
                    
                    i = e->getStringAttribute( ptagTuning_adaptiveHistory).getIntValue();
                    tuning[id]->sPrep->setAdaptiveHistory(i);
                    
                    b = (bool) e->getStringAttribute( ptagTuning_adaptiveInversional).getIntValue();
                    tuning[id]->sPrep->setAdaptiveInversional(b);
                    
                    i = e->getStringAttribute( ptagTuning_adaptiveClusterThresh).getIntValue();
                    tuning[id]->sPrep->setAdaptiveClusterThresh(i);
                    
                    i = e->getStringAttribute( ptagTuning_adaptiveAnchorFund).getIntValue();
                    tuning[id]->sPrep->setAdaptiveAnchorFundamental((PitchClass)i);
                    
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
                            
                            tuning[id]->sPrep->setCustomScale(scale);
                        }
                        else if (sub->hasTagName(vTagTuning_absoluteOffsets))
                        {
                            Array<float> absolute;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    //DBG("reading new absolute val: " + String(f));
                                    absolute.add(f);
                                }
                            }
                            
                            tuning[id]->sPrep->setAbsoluteOffsets(absolute);
                        }
                    }
                    
                    // copy static to active
                    tuning[id]->aPrep->copy( tuning[id]->sPrep);
                    
                    ++tPrepCount;
                }else if (e->hasTagName( vtagTuningModPrep + String(tModPrepCount)))
                {
                    addTuningMod();
                    
                    int id = modTuning.size() - 1;
                    
                    String p = "";
                    
                    p = e->getStringAttribute( ptagTuning_scale);
                    modTuning[id]->setParam(TuningScale, p);
                    
                    p = e->getStringAttribute( ptagTuning_fundamental);
                    modTuning[id]->setParam(TuningFundamental, p);
                    
                    p = e->getStringAttribute( ptagTuning_offset);
                    modTuning[id]->setParam(TuningOffset, p);
                    
                    p = e->getStringAttribute( ptagTuning_adaptiveIntervalScale);
                    modTuning[id]->setParam(TuningA1IntervalScale, p);
                    
                    p = e->getStringAttribute( ptagTuning_adaptiveAnchorScale);
                    modTuning[id]->setParam(TuningA1AnchorScale, p);
                    
                    p = e->getStringAttribute( ptagTuning_adaptiveHistory);
                    modTuning[id]->setParam(TuningA1History, p);
                    
                    p = e->getStringAttribute( ptagTuning_adaptiveInversional);
                    modTuning[id]->setParam(TuningA1Inversional, p);
                    
                    p = e->getStringAttribute( ptagTuning_adaptiveClusterThresh);
                    modTuning[id]->setParam(TuningA1ClusterThresh, p);
                    
                    p = e->getStringAttribute( ptagTuning_adaptiveAnchorFund);
                    modTuning[id]->setParam(TuningA1AnchorFundamental, p);
                    
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
                            
                            modTuning[id]->setParam(TuningCustomScale, floatArrayToString(scale));
                        }
                        else if (sub->hasTagName(vTagTuning_absoluteOffsets))
                        {
                            Array<float> absolute;
                            String abs = "";
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    absolute.add(f);
                                    if (f != 0.0) abs += (String(k) + ":" + String(f) + " ");
                                }
                            }
                            
                            modTuning[id]->setParam(TuningAbsoluteOffsets, abs);
                        }
                    }
                    
                    ++tModPrepCount;
                }
                else if (e->hasTagName( vtagDirectPrep + String(dPrepCount)))
                {
                    addDirect();
                    
                    int id = direct.size()-1;
                    
                    i = e->getStringAttribute(ptagDirect_tuning).getIntValue();
                    direct[id]->sPrep->setTuning(tuning[i]);
                    
                    f = e->getStringAttribute(ptagDirect_gain).getFloatValue();
                    direct[id]->sPrep->setGain(f);
                    
                    f = e->getStringAttribute(ptagDirect_hammerGain).getFloatValue();
                    direct[id]->sPrep->setHammerGain(f);
                    
                    f = e->getStringAttribute(ptagDirect_resGain).getFloatValue();
                    direct[id]->sPrep->setResonanceGain(f);
                    
                    f = e->getStringAttribute(ptagDirect_transposition).getFloatValue();
                    direct[id]->sPrep->setTransposition(f);
                    
                    // copy static to active
                    direct[id]->aPrep->copy(direct[id]->sPrep);
                    
                    ++dPrepCount;
                }
                else if (e->hasTagName( vtagDirectModPrep + String(dModPrepCount)))
                {
                    addDirectMod();
                    
                    String p = "";
                    
                    int id = modDirect.size()-1;
                    
                    p = e->getStringAttribute(ptagDirect_tuning);
                    modDirect[id]->setParam(DirectTuning, p);
                    
                    p = e->getStringAttribute(ptagDirect_gain);
                    modDirect[id]->setParam(DirectGain, p);
                    
                    p = e->getStringAttribute(ptagDirect_hammerGain);
                    modDirect[id]->setParam(DirectHammerGain, p);
                    
                    p = e->getStringAttribute(ptagDirect_resGain);
                    modDirect[id]->setParam(DirectResGain, p);
                    
                    p = e->getStringAttribute(ptagDirect_transposition);
                    modDirect[id]->setParam(DirectTransposition, p);
                    
                    ++dModPrepCount;
                }
                else if (e->hasTagName( vtagSynchronicPrep + String(sPrepCount)))
                {
                    addSynchronic();
                    
                    int id = synchronic.size() - 1;
                    
                    i = e->getStringAttribute(ptagSynchronic_tuning).getIntValue();
                    synchronic[id]->sPrep->setTuning(tuning[i]);
                    
                    f = e->getStringAttribute(ptagSynchronic_tempo).getFloatValue();
                    synchronic[id]->sPrep->setTempo(f);
                    
                    i = e->getStringAttribute(ptagSynchronic_numBeats).getIntValue();
                    synchronic[id]->sPrep->setNumBeats(i);
                    
                    i = e->getStringAttribute(ptagSynchronic_clusterMin).getIntValue();
                    synchronic[id]->sPrep->setClusterMin(i);
                    
                    i = e->getStringAttribute(ptagSynchronic_clusterMax).getIntValue();
                    synchronic[id]->sPrep->setClusterMax(i);
                    
                    i = e->getStringAttribute(ptagSynchronic_clusterThresh).getIntValue();
                    synchronic[id]->sPrep->setClusterThresh(i);
                    
                    i = e->getStringAttribute(ptagSynchronic_mode).getIntValue();
                    synchronic[id]->sPrep->setMode((SynchronicSyncMode) i);
                    
                    i = e->getStringAttribute(ptagSynchronic_beatsToSkip).getIntValue();
                    synchronic[id]->sPrep->setBeatsToSkip(i);
                    
                    i = e->getStringAttribute(ptagSynchronic_at1Mode).getIntValue();
                    synchronic[id]->sPrep->setAdaptiveTempo1Mode((AdaptiveTempo1Mode)i);
                    
                    i = e->getStringAttribute(ptagSynchronic_at1History).getIntValue();
                    synchronic[id]->sPrep->setAdaptiveTempo1History(i);
                    
                    f = e->getStringAttribute(ptagSynchronic_at1Subdivisions).getFloatValue();
                    synchronic[id]->sPrep->setAdaptiveTempo1Subdivisions(f);
                    
                    f = e->getStringAttribute(ptagSynchronic_AT1Min).getFloatValue();
                    synchronic[id]->sPrep->setAdaptiveTempo1Min(f);
                    
                    f = e->getStringAttribute(ptagSynchronic_AT1Max).getFloatValue();
                    synchronic[id]->sPrep->setAdaptiveTempo1Max(f);
                    
                    
                    forEachXmlChildElement (*e, sub)
                    {
                        if (sub->hasTagName(vtagSynchronic_beatMults))
                        {
                            Array<float> beats;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    beats.add(f);
                                }
                            }
                            
                            synchronic[id]->sPrep->setBeatMultipliers(beats);
                            
                        }
                        else  if (sub->hasTagName(vtagSynchronic_accentMults))
                        {
                            Array<float> accents;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    accents.add(f);
                                }
                            }
                            
                            synchronic[id]->sPrep->setAccentMultipliers(accents);
                            
                        }
                        else  if (sub->hasTagName(vtagSynchronic_lengthMults))
                        {
                            Array<float> lens;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    lens.add(f);
                                }
                            }
                            
                            synchronic[id]->sPrep->setLengthMultipliers(lens);
                            
                        }
                        else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
                        {
                            Array<float> transp;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    transp.add(f);
                                }
                            }
                            
                            synchronic[id]->sPrep->setTranspOffsets(transp);
                        }
                    }
                    
                    synchronic[id]->aPrep->copy(synchronic[id]->sPrep);
                    
                    ++sPrepCount;
                    
                }
                else if (e->hasTagName( vtagSynchronicModPrep + String(sModPrepCount)))
                {
                    addSynchronicMod();
                    
                    String p = "";
                    
                    int id = modSynchronic.size() - 1;
                    
                    p = e->getStringAttribute(ptagSynchronic_tuning);
                    modSynchronic[id]->setParam(SynchronicTuning, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_tempo);
                    modSynchronic[id]->setParam(SynchronicTempo, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_numBeats);
                    modSynchronic[id]->setParam(SynchronicNumPulses, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_clusterMin);
                    modSynchronic[id]->setParam(SynchronicClusterMin, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_clusterMax);
                    modSynchronic[id]->setParam(SynchronicClusterMax, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_clusterThresh);
                    modSynchronic[id]->setParam(SynchronicClusterThresh, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_mode);
                    modSynchronic[id]->setParam(SynchronicMode, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_beatsToSkip);
                    modSynchronic[id]->setParam(SynchronicBeatsToSkip, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_at1Mode);
                    modSynchronic[id]->setParam(AT1Mode, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_at1History);
                    modSynchronic[id]->setParam(AT1History, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_at1Subdivisions);
                    modSynchronic[id]->setParam(AT1Subdivisions, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_AT1Min);
                    modSynchronic[id]->setParam(AT1Min, p);
                    
                    p = e->getStringAttribute(ptagSynchronic_AT1Max);
                    modSynchronic[id]->setParam(AT1Max, p);
                    
                    
                    forEachXmlChildElement (*e, sub)
                    {
                        if (sub->hasTagName(vtagSynchronic_beatMults))
                        {
                            Array<float> beats;
                            
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    beats.add(f);
                                }
                            }
                            
                            modSynchronic[id]->setParam(SynchronicBeatMultipliers, floatArrayToString(beats));
                            
                        }
                        else  if (sub->hasTagName(vtagSynchronic_accentMults))
                        {
                            Array<float> accents;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    accents.add(f);
                                }
                            }
                            
                            modSynchronic[id]->setParam(SynchronicAccentMultipliers, floatArrayToString(accents));
                            
                        }
                        else  if (sub->hasTagName(vtagSynchronic_lengthMults))
                        {
                            Array<float> lens;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    lens.add(f);
                                }
                            }
                            
                            modSynchronic[id]->setParam(SynchronicLengthMultipliers, floatArrayToString(lens));
                            
                        }
                        else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
                        {
                            Array<float> transp;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = sub->getStringAttribute(ptagFloat + String(k));
                                
                                if (attr == String::empty) break;
                                else
                                {
                                    f = attr.getFloatValue();
                                    transp.add(f);
                                }
                            }
                            
                            modSynchronic[id]->setParam(SynchronicTranspOffsets, floatArrayToString(transp));
                        }
                    }
                    
                    ++sModPrepCount;
                    
                }
                else if (e->hasTagName( vtagNostalgicPrep + String(nPrepCount)))
                {
                    addNostalgic();
                    
                    int id = nostalgic.size() - 1;
                    
                    i = e->getStringAttribute(ptagNostalgic_tuning).getIntValue();
                    nostalgic[id]->sPrep->setTuning(tuning[i]);
                    
                    i = e->getStringAttribute(ptagNostalgic_waveDistance).getIntValue();
                    nostalgic[id]->sPrep->setWaveDistance(i);
                    
                    i = e->getStringAttribute(ptagNostalgic_undertow).getIntValue();
                    nostalgic[id]->sPrep->setUndertow(i);
                    
                    f = e->getStringAttribute(ptagNostalgic_transposition).getFloatValue();
                    nostalgic[id]->sPrep->setTransposition(f);
                    
                    f = e->getStringAttribute(ptagNostalgic_lengthMultiplier).getFloatValue();
                    nostalgic[id]->sPrep->setLengthMultiplier(f);
                    
                    f = e->getStringAttribute(ptagNostalgic_beatsToSkip).getFloatValue();
                    nostalgic[id]->sPrep->setBeatsToSkip(f);
                    
                    f = e->getStringAttribute(ptagNostalgic_gain).getFloatValue();
                    nostalgic[id]->sPrep->setGain(f);
                    
                    i = e->getStringAttribute(ptagNostalgic_mode).getIntValue();
                    nostalgic[id]->sPrep->setMode((NostalgicSyncMode)i);
                    
                    i = e->getStringAttribute(ptagNostalgic_syncTarget).getIntValue();
                    nostalgic[id]->sPrep->setSyncTarget(i);
                    
                    nostalgic[id]->sPrep->setSyncTargetProcessor(synchronic[i]->processor);
                    nostalgic[id]->aPrep->setSyncTargetProcessor(synchronic[i]->processor);
                    
                    nostalgic[id]->aPrep->copy(nostalgic[id]->sPrep);
                    
                    ++nPrepCount;
                }
                else if (e->hasTagName( vtagNostalgicModPrep + String(nModPrepCount)))
                {
                    addNostalgicMod();
                    
                    String p = "";
                    
                    int id = modNostalgic.size() - 1;
                    
                    p = e->getStringAttribute(ptagNostalgic_tuning);
                    modNostalgic[id]->setParam(NostalgicTuning, p);
                    
                    p = e->getStringAttribute(ptagNostalgic_waveDistance);
                    modNostalgic[id]->setParam(NostalgicWaveDistance, p);
                    
                    p = e->getStringAttribute(ptagNostalgic_undertow);
                    modNostalgic[id]->setParam(NostalgicUndertow, p);
                    
                    p = e->getStringAttribute(ptagNostalgic_transposition);
                    modNostalgic[id]->setParam(NostalgicTransposition, p);
                    
                    p = e->getStringAttribute(ptagNostalgic_lengthMultiplier);
                    modNostalgic[id]->setParam(NostalgicLengthMultiplier, p);
                    
                    p = e->getStringAttribute(ptagNostalgic_beatsToSkip);
                    modNostalgic[id]->setParam(NostalgicBeatsToSkip, p);
                    
                    p = e->getStringAttribute(ptagNostalgic_gain);
                    modNostalgic[id]->setParam(NostalgicGain, p);
                    
                    p = e->getStringAttribute(ptagNostalgic_mode);
                    modNostalgic[id]->setParam(NostalgicMode, p);
                    
                    p = e->getStringAttribute(ptagNostalgic_syncTarget);
                    modNostalgic[id]->setParam(NostalgicSyncTarget, p);
                    
                    ++nModPrepCount;
                }
                else if (e->hasTagName( vtagPiano + String(pianoCount)))
                {
                    int whichPiano = pianoCount++;
                    
                    bkPianos.set(whichPiano, new Piano(synchronic, nostalgic, direct,
                                              bkKeymaps[0], whichPiano)); // initializing piano 0
                    
                    Piano::Ptr thisPiano = bkPianos[whichPiano];
                    
                    int pianoMapCount = 0, prepMapCount = 0, modDirectCount = 0, modSynchronicCount = 0, modNostalgicCount = 0, modTuningCount = 0;
                    
                    forEachXmlChildElement (*e, pc)
                    {
                        
                        if (pc->hasTagName( vtagPianoMap + String(pianoMapCount)))
                        {
                            // PianoMap
                            i = pc->getStringAttribute(ptagPianoMap_key).getIntValue();
                            int key = i;
                            
                            i = pc->getStringAttribute(ptagPianoMap_piano).getIntValue();
                            int piano = i;
                            
                            thisPiano->pianoMap.set(key, piano);
                            
                            ++pianoMapCount;
                        }
                        else if (pc->hasTagName( vtagPrepMap + String(prepMapCount)))
                        {
                            // PrepMap
                            i = pc->getStringAttribute(ptagPrepMap_keymapId).getIntValue();
                            Keymap::Ptr keymap = bkKeymaps[i];
                            
                            thisPiano->addPreparationMap(); // should clean up this functionality . pretty bad
                            
                            thisPiano->prepMaps[prepMapCount]->setKeymap(keymap);
                            
                            Synchronic::PtrArr sync;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = pc->getStringAttribute(ptagPrepMap_synchronicPrepId + String(k));
                                
                                if (attr == String::empty)  break;
                                else                        sync.add(synchronic[attr.getIntValue()]);
                                
                            }
                            thisPiano->prepMaps[prepMapCount]->setSynchronic(sync);
                            
                            Nostalgic::PtrArr nost;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = pc->getStringAttribute(ptagPrepMap_nostalgicPrepId + String(k));
                                
                                if (attr == String::empty)  break;
                                else                        nost.add(nostalgic[attr.getIntValue()]);
                                
                            }
                            bkPianos[whichPiano]->prepMaps[prepMapCount]->setNostalgic(nost);
                            
                            
                            Direct::PtrArr drct;
                            for (int k = 0; k < 128; k++)
                            {
                                String attr = pc->getStringAttribute(ptagPrepMap_directPrepId + String(k));
                                
                                if (attr == String::empty)  break;
                                else                        drct.add(direct[attr.getIntValue()]);
                                
                            }
                            bkPianos[whichPiano]->prepMaps[prepMapCount]->setDirect(drct);
                        
                            ++prepMapCount;
                        }
                        else if (pc->hasTagName( vtagModDirect + String(modDirectCount)))
                        {
                            // ModDirect
                            int k = pc->getStringAttribute(ptagModX_key).getIntValue();
                            int prep = pc->getStringAttribute(ptagModX_prep).getIntValue();
                            DirectParameterType type = (DirectParameterType)pc->getStringAttribute(ptagModX_type).getIntValue();
                            
                            String val = "";
                            if (cDirectDataTypes[type] == BKFloat) val = pc->getStringAttribute(ptagFloat);
                            else if (cDirectDataTypes[type] == BKInt) val = pc->getStringAttribute(ptagInt);
                            else if (cDirectDataTypes[type] == BKFloatArr) val = pc->getStringAttribute(ptagFloatArr);
                            else if (cDirectDataTypes[type] == BKBool)   val = pc->getStringAttribute(ptagBool);
                            else if (cDirectDataTypes[type] == BKIntArr)   val = pc->getStringAttribute(ptagIntArr);
                            
                            //DirectModification(int key, int whichPrep, DirectParameterType type, String val, int ident)
                            bkPianos[whichPiano]->modMap[k]->addDirectModification(new DirectModification(k, prep, type, val, modDirectCount));
                            
                            ++modDirectCount;
                        }
                        else if (pc->hasTagName( vtagModSynchronic + String(modSynchronicCount)))
                        {
                            // ModSynchronic
                            int k = pc->getStringAttribute(ptagModX_key).getIntValue();
                            int prep = pc->getStringAttribute(ptagModX_prep).getIntValue();
                            SynchronicParameterType type = (SynchronicParameterType)pc->getStringAttribute(ptagModX_type).getIntValue();
                            
                            String val = "";
                            if (cSynchronicDataTypes[type] == BKFloat) val = pc->getStringAttribute(ptagFloat);
                            else if (cSynchronicDataTypes[type] == BKInt) val = pc->getStringAttribute(ptagInt);
                            else if (cSynchronicDataTypes[type] == BKFloatArr) val = pc->getStringAttribute(ptagFloatArr);
                            else if (cSynchronicDataTypes[type] == BKBool)   val = pc->getStringAttribute(ptagBool);
                            else if (cSynchronicDataTypes[type] == BKIntArr)   val = pc->getStringAttribute(ptagIntArr);
                            
                            //DirectModification(int key, int whichPrep, DirectParameterType type, String val, int ident)
                            bkPianos[whichPiano]->modMap[k]->addSynchronicModification(new SynchronicModification(k, prep, type, val, modSynchronicCount));
                            
                            ++modSynchronicCount;
                        }
                        else if (pc->hasTagName( vtagModNostalgic + String(modNostalgicCount)))
                        {
                            // ModNostalgic
                            int k = pc->getStringAttribute(ptagModX_key).getIntValue();
                            int prep = pc->getStringAttribute(ptagModX_prep).getIntValue();
                            NostalgicParameterType type = (NostalgicParameterType)pc->getStringAttribute(ptagModX_type).getIntValue();
                            
                            String val = "";
                            if (cNostalgicDataTypes[type] == BKFloat) val = pc->getStringAttribute(ptagFloat);
                            else if (cNostalgicDataTypes[type] == BKInt) val = pc->getStringAttribute(ptagInt);
                            else if (cNostalgicDataTypes[type] == BKFloatArr) val = pc->getStringAttribute(ptagFloatArr);
                            else if (cNostalgicDataTypes[type] == BKBool)   val = pc->getStringAttribute(ptagBool);
                            else if (cNostalgicDataTypes[type] == BKIntArr)   val = pc->getStringAttribute(ptagIntArr);
                            
                            //DirectModification(int key, int whichPrep, DirectParameterType type, String val, int ident)
                            bkPianos[whichPiano]->modMap[k]->addNostalgicModification(new NostalgicModification(k, prep, type, val, modNostalgicCount));
                            
                            ++modNostalgicCount;
                            
                        }
                        else if (pc->hasTagName( vtagModTuning + String(modTuningCount)))
                        {
                            // ModTuning
                            int k = pc->getStringAttribute(ptagModX_key).getIntValue();
                            int prep = pc->getStringAttribute(ptagModX_prep).getIntValue();
                            TuningParameterType type = (TuningParameterType)pc->getStringAttribute(ptagModX_type).getIntValue();
                            
                            String val = "";
                            if (cTuningDataTypes[type] == BKFloat) val = pc->getStringAttribute(ptagFloat);
                            else if (cTuningDataTypes[type] == BKInt) val = pc->getStringAttribute(ptagInt);
                            else if (cTuningDataTypes[type] == BKFloatArr) val = pc->getStringAttribute(ptagFloatArr);
                            else if (cTuningDataTypes[type] == BKBool)   val = pc->getStringAttribute(ptagBool);
                            else if (cTuningDataTypes[type] == BKIntArr)   val = pc->getStringAttribute(ptagIntArr);
                            
                            //DirectModification(int key, int whichPrep, DirectParameterType type, String val, int ident)
                            bkPianos[whichPiano]->modMap[k]->addTuningModification(new TuningModification(k, prep, type, val, modTuningCount));
                            
                            ++modTuningCount;
                        }
                        
                    }

                }
            }
        }
        
        
        for (int k = synchronic.size(); --k >= 0;)  synchronic[k]->processor->setCurrentPlaybackSampleRate(bkSampleRate);
        for (int k = nostalgic.size(); --k >= 0;)   nostalgic[k]->processor->setCurrentPlaybackSampleRate(bkSampleRate);
        for (int k = direct.size(); --k >= 0;)      direct[k]->processor->setCurrentPlaybackSampleRate(bkSampleRate);
        
        currentPiano = bkPianos[0];
        updateUI();
        
    }
    
    
}

void BKAudioProcessor::updateUI(void)
{
    pianoDidChange = true;
    directPreparationDidChange = true;
    nostalgicPreparationDidChange = true;
    synchronicPreparationDidChange = true;
    tuningPreparationDidChange = true;
    generalSettingsDidChange = true;
}

void BKAudioProcessor::saveGallery(void)
{
    ValueTree galleryVT( vtagGallery);

    ValueTree generalVT( vtagGeneral);
    
    generalVT.setProperty( ptagGeneral_globalGain,       general->getGlobalGain(), 0);
    generalVT.setProperty( ptagGeneral_directGain,       general->getDirectGain(), 0);
    generalVT.setProperty( ptagGeneral_synchronicGain,   general->getSynchronicGain(), 0);
    generalVT.setProperty( ptagGeneral_nostalgicGain,    general->getNostalgicGain(), 0);
    generalVT.setProperty( ptagGeneral_resonanceGain,    general->getDirectGain(), 0);
    generalVT.setProperty( ptagGeneral_resonanceGain,    general->getResonanceGain(), 0);
    generalVT.setProperty( ptagGeneral_hammerGain,       general->getHammerGain(), 0);
    generalVT.setProperty( ptagGeneral_tempoMultiplier,  general->getTempoMultiplier(), 0);
    generalVT.setProperty( ptagGeneral_resAndHammer,     general->getResonanceAndHammer(), 0);
    generalVT.setProperty( ptagGeneral_invertSustain,    general->getInvertSustain(), 0);
    generalVT.setProperty( ptagGeneral_tuningFund,       general->getTuningFundamental(), 0);
    
    galleryVT.addChild(generalVT, -1, 0);
    
    // Preparations and keymaps must be first.
    // Tuning must be first of the preparations.
    for (int i = 0; i < tuning.size(); i++) galleryVT.addChild(tuning[i]->getState(), -1, 0);
    
    for (int i = 0; i < direct.size(); i++) galleryVT.addChild(direct[i]->getState(), -1, 0);
    
    for (int i = 0; i < synchronic.size(); i++) galleryVT.addChild(synchronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < nostalgic.size(); i++) galleryVT.addChild(nostalgic[i]->getState(), -1, 0);
    
    for (int i = 0; i < modTuning.size(); i++) galleryVT.addChild(modTuning[i]->getState(i), -1, 0);
    
    for (int i = 0; i < modDirect.size(); i++) galleryVT.addChild(modDirect[i]->getState(i), -1, 0);
    
    for (int i = 0; i < modSynchronic.size(); i++) galleryVT.addChild(modSynchronic[i]->getState(i), -1, 0);
    
    for (int i = 0; i < modNostalgic.size(); i++) galleryVT.addChild(modNostalgic[i]->getState(i), -1, 0);
    
    
    for (int i = 0; i < bkKeymaps.size(); i++)
    {
        ValueTree keys( vtagKeymap + String(i));
        int count = 0;
        for (auto key : bkKeymaps[i]->keys())
        {
            keys.setProperty(ptagKeymap_key + String(count++), key, 0);
        }
        
        galleryVT.addChild(keys, -1, 0);
    }
    
    // Pianos
    for (int piano = 0; piano < bkPianos.size(); piano++)
    {
        
        ValueTree pianoVT( vtagPiano + String(bkPianos[piano]->getId()));
        
        
        
        int pmapCount = 0;
        for (auto pmap : bkPianos[piano]->getPreparationMaps())
        {
            ValueTree pmapVT( vtagPrepMap+String(pmapCount++));
            pmapVT.setProperty( ptagPrepMap_keymapId, pmap->getKeymapId(), 0);
            
            int pcount = 0;
            for (auto prep : pmap->getDirect())
                pmapVT.setProperty(ptagPrepMap_directPrepId+String(pcount++), prep->getId(), 0);
            
            pcount = 0;
            for (auto prep : pmap->getNostalgic())
                pmapVT.setProperty(ptagPrepMap_nostalgicPrepId+String(pcount++), prep->getId(), 0);
            
            
            pcount = 0;
            for (auto prep : pmap->getSynchronic())
                pmapVT.setProperty(ptagPrepMap_synchronicPrepId+String(pcount++), prep->getId(), 0);
            
            
            pianoVT.addChild(pmapVT, -1, 0);
        }

        pmapCount = 0;
        
        // Iterate through all keys and write data from PianoMap and ModMap to ValueTree
        for (int key = 0; key < 128; key++)
        {
            if (bkPianos[piano]->pianoMap[key] != 0)
            {
                ValueTree pmapVT( vtagPianoMap + String(pmapCount++));
                
                pmapVT.setProperty( ptagPianoMap_key, key, 0);
                pmapVT.setProperty( ptagPianoMap_piano, bkPianos[piano]->pianoMap[key], 0);
                
                pianoVT.addChild(pmapVT, -1, 0);
            }
            
            int modCount = 0;
            
            for (auto mod : bkPianos[piano]->modMap[key]->getDirectModifications())
            {
                ValueTree modVT( vtagModDirect + String(modCount++));
                
                DirectParameterType type = mod->getParameterType();
                
                modVT.setProperty( ptagModX_key, key, 0);
                modVT.setProperty( ptagModX_type, type, 0);
                modVT.setProperty( ptagModX_prep, mod->getPrepId(), 0);
                
                
                BKParameterDataType bktype = cDirectDataTypes[type];
                
                if (bktype == BKInt)
                {
                    modVT.setProperty( ptagInt, mod->getModInt(), 0);
                }
                else if (bktype == BKFloat)
                {
                    modVT.setProperty( ptagFloat, mod->getModFloat(), 0);
                }
                
                pianoVT.addChild(modVT, -1, 0);
            }
            

            modCount = 0;
            
            for (auto mod : bkPianos[piano]->modMap[key]->getSynchronicModifications())
            {
                ValueTree modVT( vtagModSynchronic + String(modCount++));
                
                SynchronicParameterType type = mod->getParameterType();
                
                modVT.setProperty( ptagModX_key, key, 0);
                modVT.setProperty( ptagModX_type, type, 0);
                modVT.setProperty( ptagModX_prep, mod->getPrepId(), 0);
                
                BKParameterDataType bktype = cSynchronicDataTypes[type];
                if (bktype == BKInt)
                {
                    modVT.setProperty( ptagInt, mod->getModInt(), 0);
                }
                else if (bktype == BKFloat)
                {
                    modVT.setProperty( ptagFloat, mod->getModFloat(), 0);
                }
                else if (bktype == BKFloatArr)
                {
                    ValueTree faVT( ptagFloatArr);
                    int count = 0;
                    for (auto f : mod->getModFloatArr())   faVT.setProperty( ptagFloat+String(count++), f, 0);
                    modVT.addChild(faVT, -1, 0);
                }
                
                pianoVT.addChild(modVT, -1, 0);
            }
            
            modCount = 0;
            
            for (auto mod : bkPianos[piano]->modMap[key]->getNostalgicModifications())
            {
                ValueTree modVT( vtagModNostalgic + String(modCount++));
                
                NostalgicParameterType type = mod->getParameterType();
                
                modVT.setProperty( ptagModX_key, key, 0);
                modVT.setProperty( ptagModX_type, type, 0);
                modVT.setProperty( ptagModX_prep, mod->getPrepId(), 0);
                
                BKParameterDataType bktype = cNostalgicDataTypes[type];
                if (bktype == BKInt)
                {
                    modVT.setProperty( ptagInt, mod->getModInt(), 0);
                }
                else if (bktype == BKFloat)
                {
                    modVT.setProperty( ptagFloat, mod->getModFloat(), 0);
                }
                
                pianoVT.addChild(modVT, -1, 0);
            }
            
            modCount = 0;
            
            for (auto mod : bkPianos[piano]->modMap[key]->getTuningModifications())
            {
                ValueTree modVT( vtagModTuning + String(modCount++));
                
                TuningParameterType type = mod->getParameterType();
                
                modVT.setProperty( ptagModX_key, key, 0);
                modVT.setProperty( ptagModX_type, type, 0);
                modVT.setProperty( ptagModX_prep, mod->getPrepId(), 0);
                
                BKParameterDataType bktype = cTuningDataTypes[type];
                if (bktype == BKInt)
                {
                    modVT.setProperty( ptagInt, mod->getModInt(), 0);
                }
                else if (bktype == BKFloat)
                {
                    modVT.setProperty( ptagFloat, mod->getModFloat(), 0);
                }
                else if (bktype == BKBool)
                {
                    modVT.setProperty( ptagBool, mod->getModBool(), 0);
                }
                else if (bktype == BKFloatArr)
                {
                    ValueTree faVT( ptagFloatArr);
                    int count = 0;
                    for (auto f : mod->getModFloatArr())   faVT.setProperty( ptagFloat+String(count++), f, 0);
                    modVT.addChild(faVT, -1, 0);
                }
                
                pianoVT.addChild(modVT, -1, 0);
            }
            
        }
        
        galleryVT.addChild(pianoVT, -1, 0);
    }
    
    String xml = galleryVT.toXmlString();
    DBG(xml);
    
    FileChooser myChooser ("Save gallery to file...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.xml");
    
    
    XmlElement* myXML = galleryVT.createXml();
    
    if (myChooser.browseForFileToSave(true))
    {
        File myFile (myChooser.getResult());
        myXML->writeToFile(myFile, String::empty);
    }
    
    
    delete myXML;
    
    
}

void BKAudioProcessor::loadPianoSamples(BKSampleLoadType type)
{
    // TO IMPLEMENT: Should turn off all notes in the processors/synths before loading new samples.
    
    didLoadMainPianoSamples = false;
    
    BKSampleLoader::loadMainPianoSamples(&mainPianoSynth, type);
    
    didLoadMainPianoSamples = true;
    
    if (!didLoadHammersAndRes)
    {
        didLoadHammersAndRes = true;
        BKSampleLoader::loadHammerReleaseSamples(&hammerReleaseSynth);
        BKSampleLoader::loadResonanceReleaseSamples(&resonanceReleaseSynth);
    }
    
}

BKAudioProcessor::~BKAudioProcessor()
{
    
}

//==============================================================================
void BKAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    bkSampleRate = sampleRate;
    
    mainPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
    hammerReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    resonanceReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
       
    for (int i = aMaxNumPianos; --i >= 0;)
        bkPianos[i]->prepareToPlay(sampleRate);

    for (int i = synchronic.size(); --i >= 0;)  synchronic[i]->processor->setCurrentPlaybackSampleRate(sampleRate);
    for (int i = nostalgic.size(); --i >= 0;)   nostalgic[i]->processor->setCurrentPlaybackSampleRate(sampleRate);
    for (int i = direct.size(); --i >= 0;)      direct[i]->processor->setCurrentPlaybackSampleRate(sampleRate);
    
}


void BKAudioProcessor::performModifications(int noteNumber)
{
    Array<float> modfa;
    Array<int> modia;
    float modf;
    int   modi;
    bool  modb;
    
    /*
     TuningId = 0,
     TuningScale,
     TuningFundamental,
     TuningOffset,
     TuningA1IntervalScale,
     TuningA1Inversional,
     TuningA1AnchorScale,
     TuningA1AnchorFundamental,
     TuningA1ClusterThresh,
     TuningA1History,
     TuningCustomScale,
     TuningParameterTypeNil
     */
    
    TuningModification::PtrArr tMod = currentPiano->modMap[noteNumber]->getTuningModifications();
    for (int i = tMod.size(); --i >= 0;)
    {
        TuningPreparation::Ptr active = tuning[tMod[i]->getPrepId()]->aPrep;
        TuningParameterType type = tMod[i]->getParameterType();
        modf = tMod[i]->getModFloat();
        modi = tMod[i]->getModInt();
        modb = tMod[i]->getModBool();
        modfa = tMod[i]->getModFloatArr();
        modia = tMod[i]->getModIntArr();
        
        if (type == TuningScale)                    active->setTuning((TuningSystem)modi);
        else if (type == TuningFundamental)         active->setFundamental((PitchClass)modi);
        else if (type == TuningOffset)              active->setFundamentalOffset(modf);
        else if (type == TuningA1IntervalScale)     active->setAdaptiveIntervalScale((TuningSystem)modi);
        else if (type == TuningA1Inversional)       active->setAdaptiveInversional(modb);
        else if (type == TuningA1AnchorScale)       active->setAdaptiveAnchorScale((TuningSystem)modi);
        else if (type == TuningA1ClusterThresh)     active->setAdaptiveClusterThresh(modi);
        else if (type == TuningA1AnchorFundamental) active->setAdaptiveAnchorFundamental((PitchClass) modi);
        else if (type == TuningA1History)           active->setAdaptiveHistory(modi);
        else if (type == TuningCustomScale)         active->setCustomScale(modfa);
        else if (type == TuningAbsoluteOffsets)
        {
            for(int i = 0; i< modfa.size(); i+=2) {
                //DBG("modfa AbsoluteOffsets val = " + String(modfa[i]) + " " + String(modfa[i+1]));
                active->setAbsoluteOffset(modfa[i], modfa[i+1] * .01);
            }
        }
        else if (type == TuningResetKeymap)         active->getResetMap()->setKeymap(modia);
        
        tuningPreparationDidChange = true;
    }
    
    DirectModification::PtrArr dMod = currentPiano->modMap[noteNumber]->getDirectModifications();
    for (int i = dMod.size(); --i >= 0;)
    {
        DirectPreparation::Ptr active = direct[dMod[i]->getPrepId()]->aPrep;
        DirectParameterType type = dMod[i]->getParameterType();
        modf = dMod[i]->getModFloat();
        modi = dMod[i]->getModInt();
        modia = dMod[i]->getModIntArr();
        
        if (type == DirectTransposition)    active->setTransposition(modf);
        else if (type == DirectGain)        active->setGain(modf);
        else if (type == DirectHammerGain)  active->setHammerGain(modf);
        else if (type == DirectResGain)     active->setResonanceGain(modf);
        else if (type == DirectTuning)      active->setTuning(tuning[modi]);
        else if (type == DirectResetKeymap) active->getResetMap()->setKeymap(modia);
        
        directPreparationDidChange = true;
    }
    
    NostalgicModification::PtrArr nMod = currentPiano->modMap[noteNumber]->getNostalgicModifications();
    for (int i = nMod.size(); --i >= 0;)
    {
        NostalgicPreparation::Ptr active = nostalgic[nMod[i]->getPrepId()]->aPrep;
        NostalgicParameterType type = nMod[i]->getParameterType();
        modf = nMod[i]->getModFloat();
        modi = nMod[i]->getModInt();
        modia = nMod[i]->getModIntArr();
        
        if (type == NostalgicTransposition)         active->setTransposition(modf);
        else if (type == NostalgicGain)             active->setGain(modf);
        else if (type == NostalgicMode)             active->setMode((NostalgicSyncMode)modi);
        else if (type == NostalgicUndertow)         active->setUndertow(modi);
        else if (type == NostalgicSyncTarget)       active->setSyncTarget(modi);
        else if (type == NostalgicBeatsToSkip)      active->setBeatsToSkip(modf);
        else if (type == NostalgicWaveDistance)     active->setWaveDistance(modi);
        else if (type == NostalgicLengthMultiplier) active->setLengthMultiplier(modf);
        else if (type == NostalgicTuning)           active->setTuning(tuning[modi]);
        else if (type == NostalgicResetKeymap)      active->getResetMap()->setKeymap(modia);
        
        nostalgicPreparationDidChange = true;
    }
    
    SynchronicModification::PtrArr sMod = currentPiano->modMap[noteNumber]->getSynchronicModifications();
    for (int i = sMod.size(); --i >= 0;)
    {
        SynchronicPreparation::Ptr active = synchronic[sMod[i]->getPrepId()]->aPrep;
        SynchronicParameterType type = sMod[i]->getParameterType();
        modf = sMod[i]->getModFloat();
        modi = sMod[i]->getModInt();
        modfa = sMod[i]->getModFloatArr();
        modia = sMod[i]->getModIntArr();
        
        if (type == SynchronicTranspOffsets)            active->setTranspOffsets(modfa);
        else if (type == SynchronicTempo)               active->setTempo(modf);
        else if (type == SynchronicMode)                active->setMode((SynchronicSyncMode)modi);
        else if (type == SynchronicClusterMin)          active->setClusterMin(modi);
        else if (type == SynchronicClusterMax)          active->setClusterMax(modi);
        else if (type == SynchronicClusterThresh)       active->setClusterThresh(modi);
        else if (type == SynchronicNumPulses )          active->setNumBeats(modi);
        else if (type == SynchronicBeatsToSkip)         active->setBeatsToSkip(modi);
        else if (type == SynchronicBeatMultipliers)     active->setBeatMultipliers(modfa);
        else if (type == SynchronicLengthMultipliers)   active->setLengthMultipliers(modfa);
        else if (type == SynchronicAccentMultipliers)   active->setAccentMultipliers(modfa);
        else if (type == AT1Mode)                       active->setAdaptiveTempo1Mode((AdaptiveTempo1Mode) modi);
        else if (type == AT1Min)                        active->setAdaptiveTempo1Min(modf);
        else if (type == AT1Max)                        active->setAdaptiveTempo1Max(modf);
        else if (type == AT1History)                    active->setAdaptiveTempo1History(modi);
        else if (type == AT1Subdivisions)               active->setAdaptiveTempo1Subdivisions(modf);
        else if (type == SynchronicResetKeymap)         active->getResetMap()->setKeymap(modia);
        
        synchronicPreparationDidChange = true;
    }
}

void BKAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    buffer.clear();
    
    if (!didLoadMainPianoSamples) return;
    
    int time;
    MidiMessage m;
    
    int numSamples = buffer.getNumSamples();
    
    // Process all active prep maps in current piano
    for (int p = currentPiano->activePMaps.size(); --p >= 0;)
        currentPiano->activePMaps[p]->processBlock(numSamples, m.getChannel());
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        int noteNumber = m.getNoteNumber();
        float velocity = m.getFloatVelocity();
        int p, pm; // piano, prepmap
        
        channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            ++noteOnCount;
            
            if (allNotesOff)   allNotesOff = false;
            
            // Check PianoMap for whether piano should change due to key strike.
            int whichPiano = currentPiano->pianoMap[noteNumber] - 1;
            if (whichPiano >= 0 && whichPiano != currentPiano->getId()) setCurrentPiano(whichPiano);
            
            // check for tuning resets
            for (int i = tuning.size(); --i >= 0; )
            {
                if (tuning[i]->aPrep->getResetMap()->containsNote(noteNumber)) tuning[i]->reset();
                tuningPreparationDidChange = true;
            }
            
            // modifications
            performModifications(noteNumber);
            
            // Send key on to each pmap in current piano
            for (p = currentPiano->activePMaps.size(); --p >= 0;)
                currentPiano->activePMaps[p]->keyPressed(noteNumber, velocity, channel);
        }
        else if (m.isNoteOff())
        {
            
            // Send key off to each pmap in current piano
            for (p = currentPiano->activePMaps.size(); --p >= 0;)
                currentPiano->activePMaps[p]->keyReleased(noteNumber, velocity, channel);
            
            // This is to make sure note offs are sent to Direct and Nostalgic processors from previous pianos with holdover notes.
            for (p = prevPianos.size(); --p >= 0;) {
                for (pm = prevPianos[p]->activePMaps.size(); --pm >= 0;) {
                    prevPianos[p]->activePMaps[pm]->postRelease(noteNumber, velocity, channel);
                }
            }
            
            --noteOnCount;
            
        }
        else if (m.isController())
        {
            int controller = m.getControllerNumber();
            float value = m.getControllerValue() / 128.0f;
            
            if (controller == 1)
            {
                for (int n = synchronic.size(); --n >= 0;)
                    synchronic[n]->aPrep->setTempo((int)(15.0f + value * 3000.0f));
                
                synchronicPreparationDidChange = true;
            }
            else if (controller == 2)
            {
                for (int n = direct.size(); --n >= 0;)
                    direct[n]->aPrep->setTransposition(((value * 2.0)-1.0) * 24.0f);
                
                directPreparationDidChange = true;
            }
            else if (controller == 3)
            {
                for (int n = nostalgic.size(); --n >= 0;)
                    nostalgic[n]->aPrep->setTransposition(((value * 2.0)-1.0) * 24.0f);
                
                nostalgicPreparationDidChange = true;
            }
            
        }
    }
    
    // Sets some flags to determine whether to send noteoffs to previous pianos.
    if (!allNotesOff && !noteOnCount) {
        prevPianos.clearQuick();
        allNotesOff = true;
    }

    mainPianoSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    hammerReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    resonanceReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    
    
    
}

void  BKAudioProcessor::setCurrentPiano(int which)
{
    
    // Optimizations can be made here. Don't need to iterate through EVERY preparation. 
    for (int i = direct.size(); --i >= 0; ) {
        
        // Need to deal with previous transposition value to make sure notes turn off.
        direct[i]->aPrep->copy(direct[i]->sPrep);
    }
    
    for (int i = nostalgic.size(); --i >= 0; )
        nostalgic[i]->aPrep->copy(nostalgic[i]->sPrep);

    
    for (int i = synchronic.size(); --i >= 0; )
        synchronic[i]->aPrep->copy(synchronic[i]->sPrep);
    
    for (int i = tuning.size(); --i >= 0; )
        tuning[i]->aPrep->copy(tuning[i]->sPrep);
    
    
    if (noteOnCount)  prevPianos.addIfNotAlreadyThere(currentPiano);
    
    prevPiano = currentPiano;
    
    currentPiano = bkPianos[which];

    pianoDidChange = true;
    synchronicPreparationDidChange = true;
    nostalgicPreparationDidChange = true;
    directPreparationDidChange = true;
}

void BKAudioProcessor::releaseResources() {
    
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    //fileBuffer.setSize (0, 0);
    
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool BKAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) {
    
    // Reject any bus arrangements that are not compatible with your plugin
    
    const int numChannels = preferredSet.size();
    
#if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
#elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
#else
    if (numChannels != 1 && numChannels != 2)
        return false;
    
    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
#endif
    
    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif

SynchronicProcessor::Ptr BKAudioProcessor::getSynchronicProcessor(int id)
{
    for (int i = synchronic.size(); --i >= 0;)
    {
        if(synchronic[i]->getId() == id) {
            DBG("got synchronic processor id " + String(id));
            return synchronic[i]->processor;
        }
    }
    
    //else
    DBG("synchronic processor not found, returning first processor");
    return synchronic[0]->processor;
}


//==============================================================================
void BKAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
    
}

//==============================================================================
bool BKAudioProcessor::hasEditor() const
{
    
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BKAudioProcessor::createEditor()
{
    return new BKAudioProcessorEditor (*this);
}

//==============================================================================
void BKAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BKAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
const String BKAudioProcessor::getName() const {
    
    return JucePlugin_Name;
}

bool BKAudioProcessor::acceptsMidi() const {
    
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool BKAudioProcessor::producesMidi() const {
    
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double BKAudioProcessor::getTailLengthSeconds() const {
    
    return 0.0;
}

int BKAudioProcessor::getNumPrograms() {
    
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int BKAudioProcessor::getCurrentProgram() {
    
    return 0;
}

void BKAudioProcessor::setCurrentProgram (int index) {
    
}

const String BKAudioProcessor::getProgramName (int index) {
    
    return String("bitKlavier");
}

void BKAudioProcessor::changeProgramName (int index, const String& newName) {
    
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    
    return new BKAudioProcessor();
}

