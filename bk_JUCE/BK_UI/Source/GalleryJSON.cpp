/*
  ==============================================================================

    JSONGallery.cpp
    Created: 22 Mar 2017 12:31:10pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PluginProcessor.h"



Array<float> tempoAlreadyLoaded;

#define jsonGetValue(ID) data.getProperty(ID, "").getArray()->getFirst()
#define jsonGetProperty(ID) data.getProperty(ID, "")
#define jsonPropertyDoesExist(ID) (data.getProperty(ID, "") != String::empty)

#define jsonGetKeys(ID) \
isLayer = true; \
kvar = data.getProperty(ID, ""); \
keys.clearQuick(); \
if (kvar == String::empty) isLayer = false; \
if (isLayer) \
{\
for (int k = 0; k < kvar.size(); k++) keys.add(kvar[k]);\
}; \
if (!keys.size() || keys[0] == -1) isLayer = false;

void Gallery::setStateFromJson(var myJson)
{
    //general = new GeneralSettings();
    
    var pattr = myJson.getProperty("pattrstorage", "");
    
    String name = pattr.getProperty("name", "").toString();
    
    var pianos = pattr.getProperty("slots", "");
    
    int sId,nId,dId,tId,oId;
    Array<int> keys; var kvar;  bool isLayer; bool isOld = true; int modTuningCount = 0;
    
    
    bkKeymaps.add(new Keymap(0));
    bkKeymaps.add(new Keymap(1));
    // Default all on for
    for (int i = 0; i < 128; i++) bkKeymaps[1]->addNote(i);
    
    for (int i = 0; i <= 500; i++) //arbs
    {
        var piano = pianos.getProperty(String(i), 0);
        
        
        if (piano.equals(0)) break;
        else
        {
            addPiano();
            Piano::Ptr thisPiano = bkPianos.getLast();
            thisPiano->setId(i);
            
            String name = piano.getProperty("name", "").toString();
            thisPiano->setName(name);
            int pianoId = piano.getProperty("id", "");
            var data = piano.getProperty("data", "");
            
            // V2 test
            if(jsonPropertyDoesExist(jsonSynchronicLayerX+"1::"+"tempo"))   isOld = false;
            
            bool alreadyExists; int which;
            
            // TUNING
            String tx = "tuning::";
            int scale = jsonGetValue("tuningMenu");
            int fund = jsonGetValue("fundamentalMenu");
            int ais = jsonGetValue(tx+"adaptiveIntervalScale");
            int aas = jsonGetValue(tx+"adaptiveAnchorScale");
            int inv = jsonGetValue(tx+"adaptiveIntervalScale_verticalInversion");
            int clustThresh = jsonGetValue(tx+"clusterThresh");
            float offset = jsonGetValue(tx+"globalFundamentalOffset");
            int howMany = jsonGetValue(tx+"howMany");
            
            Array<float> custom;
            var cvar =  jsonGetProperty(tx+"customScale");
            for (int c = 0; c < cvar.size(); c++) custom.add(cvar[c]);
            
            
            // tuningMap, setTuning::fundOffset, etc. should be modifications
            
            TuningPreparation::Ptr defaultTuning = new TuningPreparation();
            
            defaultTuning->setTuning((TuningSystem)scale);
            
            defaultTuning->setFundamental((PitchClass)((12-fund)%12));
            
            defaultTuning->setCustomScaleCents(custom);
            
            defaultTuning->setAdaptiveHistory(howMany);
            
            defaultTuning->setAdaptiveAnchorScale((TuningSystem)aas);
            
            defaultTuning->setAdaptiveIntervalScale((TuningSystem)ais);
            
            defaultTuning->setAdaptiveInversional((bool)inv);
            
            defaultTuning->setAdaptiveClusterThresh(clustThresh);
            
            defaultTuning->setFundamentalOffset(offset);
            
            tId = addTuningIfNotAlreadyThere(defaultTuning);
            
            DirectPreparation::Ptr mainPianoPrep = new DirectPreparation(tuning[tId]);
            dId = addDirectIfNotAlreadyThere(mainPianoPrep);
            
            thisPiano->addPreparationMap();
            
            thisPiano->prepMaps[0]->setKeymap(bkKeymaps[1]);
            thisPiano->prepMaps[0]->addDirect(direct[dId]);
            if (scale == AdaptiveTuning || scale == AdaptiveAnchoredTuning) thisPiano->prepMaps[0]->addTuning(tuning[tId]);
            
            // Direct, Synchronic, Nostalgic preparations
            String dx = jsonDirectX;
            String nx = jsonNostalgicX;
            String sx = jsonSynchronicX;
            String ax = "adaptiveTempo::";
            
            for (int L = 0; L <= (isOld ? 1 : 12); L++)
            {
                if (!isOld)
                {
                    dx = jsonDirectLayerX + String(L) + "::";
                    nx = jsonNostalgicLayerX + String(L) + "::";
                    sx = jsonSynchronicLayerX + String(L) + "::";
                }
                
                jsonGetKeys(sx+"ATkeys");
                
                // Link adaptive
                if (isLayer)
                {
                    
                }
                
                // SYNCHRONIC
                jsonGetKeys(sx+"SynchronicKeys");
                
                if (isLayer)
                {
                    // Make tuning for this Synchronic layer.
                    PitchClass metroFundamental = (PitchClass)int(jsonGetValue("synchronic::metroFundamentalMenu"));
                    TuningSystem metroTuning = (TuningSystem)int(jsonGetValue("synchronic::metroTuningMenu"));
                    
                    TuningPreparation::Ptr tunePrep = new TuningPreparation(defaultTuning);
                    tunePrep->setTuning(metroTuning);
                    tunePrep->setFundamental((PitchClass)((12-metroFundamental)%12));
                    tId = addTuningIfNotAlreadyThere(tunePrep);
                    
                    TempoPreparation::Ptr tempoPrep = new TempoPreparation();
                    tempoPrep->setTempo(120);
                    oId = addTempoIfNotAlreadyThere(tempoPrep);
                    
                    
                    SynchronicPreparation::Ptr syncPrep = new SynchronicPreparation(tuning[tId], tempo[oId]);
                    
                    Array<float> accents;
                    var am =  jsonGetProperty(sx+"accentsList");
                    for (int c = 0; c < am.size(); c++) accents.add(am[c]);
                    
                    syncPrep->setAccentMultipliers(accents);
                    
                    Array<float> transp;
                    var tm =  jsonGetProperty(sx+"NoteTranspList");
                    for (int c = 0; c < tm.size(); c++) transp.add(tm[c]);
                    
                    Array<Array<float>> atransp;
                    atransp.add(transp);
                    
                    syncPrep->setTransposition(atransp);
                    
                    Array<float> beats;
                    var bm =  jsonGetProperty(sx+"lengthMultList");
                    for (int c = 0; c < bm.size(); c++) beats.add(bm[c]);
                    
                    syncPrep->setBeatMultipliers(beats);
                    
                    int clusterMin = jsonGetValue(sx+"clusterMin");
                    
                    syncPrep->setClusterMin(clusterMin);
                    
                    int clusterMax = jsonGetValue(sx+"clusterMax");
                    
                    syncPrep->setClusterMax(clusterMax);
                    
                    int clusterThresh = jsonGetValue(sx+"clusterThresh");
                    
                    syncPrep->setClusterThresh(clusterThresh);
                    
                    int howMany = jsonGetValue(sx+"howMany");
                    
                    syncPrep->setNumBeats(howMany);
                    
                    float tmp = jsonGetValue(sx+"tempo");
                    
                    int tmpId = 0;
                    if (tempoAlreadyLoaded.contains(tmp))
                    {
                        
                        for (int i = 0; i < tempo.size(); i++)
                        {
                            if (tempo[i]->sPrep->getTempo() == tmp)
                            {
                                
                                tmpId = i;
                                break;
                            }
                        }
                    }
                    else
                    {
                        addTempo();
                        tempoAlreadyLoaded.add(tmp);
                        tmpId = tempo.size()-1;
                        
                        tempo[tmpId]->sPrep->setTempo(tmp);
                        tempo[tmpId]->aPrep->setTempo(tmp);
                    }
                    //need to set System first?
                    //if(tempo[tmpId]->sPrep->getTempoSystem() == AdaptiveTempo1) thisPiano->prepMaps[0]->addTempo(tempo[tmpId]);
                    
                    DBG("tmpId: "+String(tmpId));
                    // FIX THIS
                    //syncPrep->setTempo(tempo[tmpId]);
                    
                    int syncMode = jsonGetValue(sx+"syncMode");
                    
                    if (syncMode < 0 || syncMode > 4) syncMode = 0;
                    
                    if (syncMode == 0) //last note sync / start
                    {
                        syncPrep->setMode(AnyNoteOnSync);
                        syncPrep->setBeatsToSkip(0);
                    }
                    else if (syncMode == 1) // first-note-sync
                    {
                        syncPrep->setMode(FirstNoteOnSync);
                        syncPrep->setBeatsToSkip(-1);
                    }
                    else if (syncMode == 2) // note-off-sync
                    {
                        syncPrep->setMode(LastNoteOffSync);
                        syncPrep->setBeatsToSkip(1);
                    }
                    else if (syncMode == 3) // note-off-start
                    {
                        syncPrep->setMode(LastNoteOffSync);
                        syncPrep->setBeatsToSkip(0);
                    }
                    else if (syncMode == 4) // first-note-start
                    {
                        syncPrep->setMode(FirstNoteOnSync);
                        syncPrep->setBeatsToSkip(0);
                    }
                    
                    Array<float> lens;
                    var lm = jsonGetProperty(sx+"NoteLengthMultList");
                    //for (int c = 0; c < lm.size(); c++) lens.add(lm[c]);
                    for (int c = 0; c < lm.size(); c++) lens.add((float)lm[c] * 50. * tmp/60000.);
                    
                    syncPrep->setLengthMultipliers(lens);
                    
                    sId = addSynchronicIfNotAlreadyThere(syncPrep);
                    
                    
                    // Make new keymap
                    Keymap::Ptr keymap = new Keymap(bkKeymaps.size());
                    for (int k = 0; k < keys.size(); k += 2 )   keymap->addNote(keys[k]);
                    
                    // Compare against current keymaps.
                    alreadyExists = false; which = 0;
                    for (int t = 0; t < bkKeymaps.size()-1; t++)
                    {
                        if (keymap->compare(bkKeymaps[t]))
                        {
                            alreadyExists = true;
                            which = t;
                        }
                    }
                    
                    Keymap::Ptr pmapkeymap = bkKeymaps[which];
                    if (!alreadyExists)
                    {
                        addKeymap(keymap);
                        pmapkeymap = bkKeymaps.getLast();
                        
                        DBG("adding keymap and prepmap");
                    }
                    
                    DBG("adding prep map");
                    
                    thisPiano->addPreparationMap();
                    thisPiano->prepMaps.getLast()->setKeymap(pmapkeymap);
                    thisPiano->prepMaps.getLast()->addSynchronic(synchronic[sId]);
                    
                    DBG("\nsynchronicPrep: " + String(sId) +
                        "\nlengths: " + floatArrayToString(lens) +
                        "\naccents: " + floatArrayToString(accents) +
                        "\ntransp: " + floatArrayToString(transp) +
                        "\nbeats: " + floatArrayToString(beats) +
                        "\nclusterMin: " + String(clusterMin) +
                        "\nclusterMax: " + String(clusterMax) +
                        "\nclusterThresh: " + String(clusterThresh) +
                        "\nhowMany: " + String(howMany) +
                        "\nfund: " + String(metroFundamental) +
                        "\ntuning: " + String(metroTuning) +
                        "\nsyncMode: " + String(syncMode) +
                        "\ntempo: " + String(tmp) + "\n");
                }
                
                // NOSTALGIC
                jsonGetKeys(nx+"NostalgicKeys");
                
                if (isLayer)
                {
                    
                    // NOSTALGIC TUNING
                    PitchClass revFundamental = (PitchClass)int(jsonGetValue("nostalgic::reverseFundamentalMenu"));
                    TuningSystem revTuning = (TuningSystem)int(jsonGetValue("nostalgic::reverseTuningMenu"));
                    
                    TuningPreparation::Ptr tunePrep = new TuningPreparation(defaultTuning);
                    
                    tunePrep->setTuning(revTuning);
                    
                    tunePrep->setFundamental((PitchClass)((12-revFundamental)%12));
                    
                    tId = addTuningIfNotAlreadyThere(tunePrep);
                    
                    NostalgicPreparation::Ptr nostPrep = new NostalgicPreparation(tuning[tId]);
                    
                    float gain = jsonGetValue(nx+"gain");
                    
                    nostPrep->setGain(gain);
                    
                    float memoryMult = jsonGetValue(nx+"memoryMult");
                    
                    nostPrep->setLengthMultiplier(memoryMult);
                    
                    float beatsToSkip = jsonGetValue(nx+"revBeatsToSkip");
                    
                    nostPrep->setBeatsToSkip(beatsToSkip);
                    
                    int reverseSyncMode = jsonGetValue(nx+"reverseSyncMode");
                    
                    int reverseSyncTarget = sId;
                    
                    if (!isOld) reverseSyncTarget = jsonGetValue(nx+"reverseSyncTarget");
                    
                    if (reverseSyncMode)
                    {
                        nostPrep->setSyncTarget(reverseSyncTarget);
                        nostPrep->setSyncTargetProcessor(synchronic[reverseSyncTarget]->processor);
                    }
                    
                    float transposition = jsonGetValue(nx+"transposition");
                    
                    Array<float> ntransp;
                    ntransp.add(transposition);
                    
                    nostPrep->setTransposition(ntransp);
                    
                    float undertow = jsonGetValue(nx+"undertow");
                    
                    nostPrep->setUndertow(undertow);
                    
                    float wavedistance = jsonGetValue(nx+"wavedistance");
                    
                    nostPrep->setWaveDistance(wavedistance);
                    
                    // Check if nostalgic[Id] already exists as another nostalgic.
                    nId = addNostalgicIfNotAlreadyThere(nostPrep);
                    
                    // Make new keymap
                    Keymap::Ptr keymap = new Keymap(bkKeymaps.size());
                    for (int k = 0; k < keys.size(); k += 2 )   keymap->addNote(keys[k]);
                    
                    // Compare against current keymaps.
                    alreadyExists = false; which = 0;
                    for (int t = 0; t < bkKeymaps.size()-1; t++)
                    {
                        if (keymap->compare(bkKeymaps[t]))
                        {
                            alreadyExists = true;
                            which = t;
                        }
                    }
                    
                    Keymap::Ptr pmapkeymap = bkKeymaps[which];
                    if (!alreadyExists)
                    {
                        addKeymap(keymap);
                        pmapkeymap = bkKeymaps.getLast();
                        
                        DBG("adding keymap and prepmap");
                    }
                    
                    DBG("adding prep map");
                    
                    thisPiano->addPreparationMap();
                    thisPiano->prepMaps.getLast()->setKeymap(pmapkeymap);
                    thisPiano->prepMaps.getLast()->addNostalgic(nostalgic[nId]);
                    
                    
                    
                    DBG("nostalgicPrep: " + String(nId));
                }
                
                
                // DIRECT
                jsonGetKeys(dx+"DirectKeys");
                
                
                if (isLayer)
                {
                    
                    DirectPreparation::Ptr dPrep = new DirectPreparation(tuning[0]);
                    
                    
                    if (!isOld)
                    {
                        float dGain     = jsonGetValue(dx+"directGain");
                        float dOverlay  = jsonGetValue(dx+"directOverlay");
                        float dTransp   = jsonGetValue(dx+"directTransp");
                        
                        dPrep->setGain(dGain);
                        
                        Array<float> transp;
                        transp.add(dTransp);
                        
                        dPrep->setTransposition(transp);
                        
                        DBG("keys: " + intArrayToString(keys) +
                            " dgain: " + String(dGain) +
                            " dOverlay: " + String(dOverlay) +
                            " dTransp: " + String(dTransp));
                    }
                    else
                    {
                        dPrep->setGain(0.0f);
                        
                        DBG("keys: " + intArrayToString(keys) +
                            " OFF");
                    }
                    
                    int dId = addDirectIfNotAlreadyThere(dPrep);
                    
                    // Make new keymap
                    Keymap::Ptr keymap = new Keymap(bkKeymaps.size());
                    for (int k = 0; k < keys.size(); k += 2 )   keymap->addNote(keys[k]);
                    
                    // Compare against current keymaps.
                    alreadyExists = false; which = 0;
                    for (int t = 0; t < bkKeymaps.size()-1; t++)
                    {
                        if (keymap->compare(bkKeymaps[t]))
                        {
                            alreadyExists = true;
                            which = t;
                        }
                    }
                    
                    Keymap::Ptr pmapkeymap = bkKeymaps[which];
                    if (!alreadyExists)
                    {
                        addKeymap(keymap);
                        pmapkeymap = bkKeymaps.getLast();
                        
                        DBG("adding keymap and prepmap");
                    }
                    
                    DBG("adding prep map");
                    
                    thisPiano->addPreparationMap();
                    thisPiano->prepMaps.getLast()->setKeymap(pmapkeymap);
                    thisPiano->prepMaps.getLast()->addDirect(direct[dId]);
                    
                    DBG("directPrep: " + String(dId));
                }
                
                
            }
            
            int key, pId;
            var presetmap = jsonGetProperty("presetmaps::presetMap");
            
            if (!(presetmap.size() % 3))
            {
                for (int j = 0; j < presetmap.size(); j += 3)
                {
                    pId = presetmap[j];
                    key = presetmap[j+2];
                    
                    if (key >= 0 && key < 128 && pId >= 1)
                    {
                        thisPiano->pianoMap.set(key, pId);
                    }
                }
            }
            
            
            String tun,note,notenum;
            
            var tuningmap = jsonGetProperty("tuning::tuningMap");
            if (!(tuningmap.size() % 3))
            {
                for (int j = 0; j < tuningmap.size(); j += 3)
                {
                    tun = tuningmap[j]; note = tuningmap[j+1]; notenum = tuningmap[j+2];
                    
                    PitchClass fund = letterNoteToPitchClass(note);
                    TuningSystem tscale = tuningStringToTuningSystem(tun);
                    int noteNumber = notenum.getIntValue();
                    
                    TuningModPreparation::Ptr myMod = new TuningModPreparation(modTuning.size());
                    
                    myMod->setParam(TuningFundamental, String(fund));
                    myMod->setParam(TuningScale, String(tscale));
                    
                    bool dontAdd = false; int whichTMod = 0;
                    for (int c = modTuning.size(); --c>=0;)
                    {
                        if (myMod->compare(modTuning[c]))
                        {
                            whichTMod = c;
                            dontAdd = true;
                            break;
                        }
                    }
                    
                    if (!dontAdd)
                    {
                        addTuningMod(myMod);
                        whichTMod = modTuning.size()-1;
                    }
                    
                    // Make new keymap
                    Keymap::Ptr keymap = new Keymap(bkKeymaps.size());
                    keymap->addNote(noteNumber);
                    
                    // Compare against current keymaps.
                    alreadyExists = false; which = 0;
                    for (int t = 0; t < bkKeymaps.size()-1; t++)
                    {
                        if (keymap->compare(bkKeymaps[t]))
                        {
                            alreadyExists = true;
                            which = t;
                        }
                    }
                    
                    Keymap::Ptr pmapkeymap = bkKeymaps[which];
                    if (!alreadyExists)
                    {
                        addKeymap(keymap);
                        pmapkeymap = bkKeymaps.getLast();
                        
                        DBG("adding keymap");
                    }

                    ItemMapper::Ptr thisTuning = new ItemMapper(PreparationTypeTuning, direct[dId]->getTuning()->getId());
                    ItemMapper::Ptr thisKeymap = new ItemMapper(PreparationTypeKeymap, pmapkeymap->getId());
                    ItemMapper::Ptr thisTuningMod = new ItemMapper(PreparationTypeTuningMod, whichTMod);
                    
                    thisPiano->add(thisTuning);
                    thisPiano->add(thisKeymap);
                    thisPiano->add(thisTuningMod);
                    
                    ++modTuningCount;
                    
                }
            }
        }
    }
    
    for (int k = tempo.size(); --k >= 0;)       tempo[k]->processor->setCurrentPlaybackSampleRate(bkSampleRate);
    for (int k = tuning.size(); --k >= 0;)      tuning[k]->processor->setCurrentPlaybackSampleRate(bkSampleRate);
    for (int k = synchronic.size(); --k >= 0;)  synchronic[k]->processor->setCurrentPlaybackSampleRate(bkSampleRate);
    for (int k = nostalgic.size(); --k >= 0;)   nostalgic[k]->processor->setCurrentPlaybackSampleRate(bkSampleRate);
    for (int k = direct.size(); --k >= 0;)      direct[k]->processor->setCurrentPlaybackSampleRate(bkSampleRate);
    
}


#if 0
tm = jsonGetProperty("tuning::nostalgicTuningSub::nostalgicTuningMap");
if (!(tm.size() % 3))
{
    for (int j = 0; j < tm.size(); j += 3)
    {
        tun = tm[j]; note = tm[j+1]; notenum = tm[j+2];
        
        PitchClass fund = letterNoteToPitchClass(note);
        TuningSystem tscale = tuningStringToTuningSystem(tun);
        int noteNumber = notenum.getIntValue();
        
        TuningModPreparation::Ptr myMod = new TuningModPreparation();
        
        myMod->setParam(TuningFundamental, String(fund));
        myMod->setParam(TuningScale, String(tscale));
        
        bool dontAdd = false; int whichTMod = 0;
        for (int c = modTuning.size(); --c>=0;)
        {
            if (myMod->compare(modTuning[c]))
            {
                whichTMod = c;
                dontAdd = true;
                break;
            }
        }
        
        if (!dontAdd)
        {
            addTuningMod(myMod);
            whichTMod = modTuning.size()-1;
        }
        
        int whichPrep = nostalgic[nId]->aPrep->getTuning()->getId();
        thisPiano->modificationMap[noteNumber]->addTuningModification(new TuningModification(noteNumber, whichPrep, TuningFundamental, String(fund), whichTMod));
        
        ++modTuningCount;
        
        
        
        
    }
}

tm = jsonGetProperty("tuning::synchronicTuningSub::synchronicTuningMap");
if (!(tm.size() % 3))
{
    for (int j = 0; j < tm.size(); j += 3)
    {
        tun = tm[j]; note = tm[j+1]; notenum = tm[j+2];
        
        PitchClass fund = letterNoteToPitchClass(note);
        TuningSystem tscale = tuningStringToTuningSystem(tun);
        int noteNumber = notenum.getIntValue();
        
        // create TuningModPreparation and TuningModification attached to key
        
        TuningModPreparation::Ptr myMod = new TuningModPreparation();
        
        myMod->setParam(TuningFundamental, String(fund));
        myMod->setParam(TuningScale, String(tscale));
        
        bool dontAdd = false; int whichTMod = 0;
        for (int c = modTuning.size(); --c>=0;)
        {
            if (myMod->compare(modTuning[c]))
            {
                whichTMod = c;
                dontAdd = true;
                break;
            }
        }
        
        if (!dontAdd)
        {
            addTuningMod(myMod);
            whichTMod = modTuning.size()-1;
        }
        
        int whichPrep = synchronic[sId]->aPrep->getTuning()->getId();
        
        thisPiano->modificationMap[noteNumber]->addTuningModification(new TuningModification(noteNumber, whichPrep, TuningFundamental, String(fund), whichTMod));
        
        ++modTuningCount;
        
        
        
        
    }
}
#endif
