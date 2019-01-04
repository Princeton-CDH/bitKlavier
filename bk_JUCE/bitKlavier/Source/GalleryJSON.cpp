/*
  ==============================================================================

    JSONGallery.cpp
    Created: 22 Mar 2017 12:31:10pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "BKGraph.h"

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
    var pattr = myJson.getProperty("pattrstorage", "");
    
    String name = pattr.getProperty("name", "").toString();
    
    var pianos = pattr.getProperty("slots", "");
    
    int sId,nId,tId,oId;
    Array<int> keys; var kvar;  bool isLayer; bool isOld = true;
    
    addKeymap();
    Keymap::Ptr emptyKeymap = bkKeymaps.getLast();
    
    addKeymap();
    // Default all on for first keymap
    for (int i = 0; i < 128; i++) bkKeymaps.getLast()->addNote(i);
    Keymap::Ptr defaultKeymap = bkKeymaps.getLast();
    
    int pianoMapId = 1;
    
    addDirect();
    addTuning();
    addTempo();
    addSynchronic();
    addNostalgic();

    Direct::Ptr defaultDirect = direct.getLast();
    Tuning::Ptr defaultTuning = tuning.getLast();
    Tempo::Ptr defaultTempo = tempo.getLast();
    Synchronic::Ptr defaultSynchronic = synchronic.getLast();
    Nostalgic::Ptr defaultNostalgic = nostalgic.getLast();
    
    

    for (int i = 1; i <= 500; i++) //arbs
    {
        var piano = pianos.getProperty(String(i), 0);
        
        if (piano.equals(0)) break;
        else
        {
            Tuning::Ptr directTuning;
            Tuning::Ptr synchronicTuning;
            Tuning::Ptr nostalgicTuning;
            Synchronic::Ptr synchronicTarget;
            Nostalgic::Ptr nostalgicTarget;
            BKItem* directTuningItem;
            BKItem* nostalgicTuningItem;
            BKItem* synchronicTuningItem;
            
            addPianoWithId(i);
            Piano::Ptr thisPiano = bkPianos.getLast();
    
            
            String name = piano.getProperty("name", "").toString();
            thisPiano->setName(name);
            
            var data = piano.getProperty("data", "");
            
            // V2 test
            if(jsonPropertyDoesExist(jsonSynchronicLayerX+"1::"+"tempo"))   isOld = false;
            
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
            
            Array<float> custom({0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.});
            var cvar =  jsonGetProperty(tx+"customScale");
    
            for (int c = 0; c < cvar.size(); c++) custom.set(c, cvar[c]);
        
            TuningPreparation::Ptr defaultTuning = new TuningPreparation();
            
            defaultTuning->setScale((TuningSystem)scale);
            
            defaultTuning->setFundamental((PitchClass)((12-fund)%12));
            
            defaultTuning->setCustomScaleCents(custom);
            
            defaultTuning->setAdaptiveHistory(howMany);
            
            defaultTuning->setAdaptiveAnchorScale((TuningSystem)aas);
            
            defaultTuning->setAdaptiveIntervalScale((TuningSystem)ais);
            
            defaultTuning->setAdaptiveInversional((bool)inv);
            
            defaultTuning->setAdaptiveClusterThresh(clustThresh);
            
            defaultTuning->setFundamentalOffset(offset);
            
            directTuning = matches(defaultTuning);
            
            if (directTuning == nullptr)
            {
                addTuning(defaultTuning);
                directTuning = tuning.getLast();
            }
            
            
            directTuningItem = thisPiano->itemWithTypeAndId(PreparationTypeTuning, directTuning->getId());
            
            if (directTuningItem == nullptr)
            {
                directTuningItem = new BKItem(PreparationTypeTuning, directTuning->getId(), processor);
                
                directTuningItem->setPianoTarget(-1);
                
                directTuningItem->setItemName("Tuning" + String(directTuning->getId()));
                
                directTuningItem->setTopLeftPosition(10, 10);
                
                directTuningItem->setActive(true);
                
                thisPiano->items.add(directTuningItem);
            }
            
            BKItem* directItem = thisPiano->itemWithTypeAndId(PreparationTypeDirect, defaultDirect->getId());
            
            if (directItem == nullptr)
            {
                directItem = new BKItem(PreparationTypeDirect, defaultDirect->getId(), processor);
                
                directItem->setPianoTarget(-1);
                
                directItem->setItemName("Direct" + String(defaultDirect->getId()));
                
                directItem->setTopLeftPosition(10, 10);
                
                directItem->setActive(true);
                
                thisPiano->items.add(directItem);
            }
            
            BKItem* keymapItem = thisPiano->itemWithTypeAndId(PreparationTypeKeymap, defaultKeymap->getId());
            
            if (keymapItem == nullptr)
            {
                keymapItem = new BKItem(PreparationTypeKeymap, defaultKeymap->getId(), processor);
                
                keymapItem->setPianoTarget(-1);
                
                keymapItem->setItemName("Keymap" + String(defaultKeymap->getId()));
                
                keymapItem->setTopLeftPosition(10, 10);
                
                keymapItem->setActive(true);
                
                thisPiano->items.add(keymapItem);
            }
            
            keymapItem->addConnection(directItem);
            directItem->addConnection(keymapItem);
            
            directTuningItem->addConnection(directItem);
            directItem->addConnection(directTuningItem);
            
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
                    
                    TuningPreparation::Ptr tuningPrep = new TuningPreparation(defaultTuning);
                    tuningPrep->setScale(metroTuning);
                    tuningPrep->setFundamental((PitchClass)((12-metroFundamental)%12));
                    
                    
                    synchronicTuning = matches(tuningPrep);
                    if (synchronicTuning == nullptr)
                    {
                        addTuning(tuningPrep);
                        synchronicTuning = tuning.getLast();
                    }
                    tId = synchronicTuning->getId();
                    
                    SynchronicPreparation::Ptr syncPrep = new SynchronicPreparation();
                    
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
                    
                    for (int i = 0; i < tempo.size(); i++)
                    {
                        if (tempo[i]->sPrep->getTempo() == tmp)
                        {
                            tmpId = i;
                            break;
                        }
                    }
                    
                    TempoPreparation::Ptr tempoPrep = new TempoPreparation();
                    tempoPrep->setTempo(tmp);
                    Tempo::Ptr thisTempo = matches(tempoPrep);
                    if (thisTempo == nullptr)
                    {
                        addTempo(tempoPrep);
                        thisTempo = tempo.getLast();
                    }
                    oId = thisTempo->getId();
                    
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
                    
                    Synchronic::Ptr thisSynchronic = matches(syncPrep);
                    if (thisSynchronic == nullptr)
                    {
                        addSynchronic(syncPrep);
                        thisSynchronic = synchronic.getLast();
                    }
                    synchronicTarget = thisSynchronic;
                    sId = thisSynchronic->getId();
                    
                    BKItem* synchronicItem = thisPiano->itemWithTypeAndId(PreparationTypeSynchronic, thisSynchronic->getId());
                    
                    if (synchronicItem == nullptr)
                    {
                        synchronicItem = new BKItem(PreparationTypeSynchronic, thisSynchronic->getId(), processor);
                        
                        synchronicItem->setPianoTarget(-1);
                        
                        synchronicItem->setItemName("Synchronic" + String(thisSynchronic->getId()));
                        
                        synchronicItem->setTopLeftPosition(10, 10);
                        
                        synchronicItem->setActive(true);
                        
                        thisPiano->items.add(synchronicItem);
                    }
                    
                    synchronicTuningItem = thisPiano->itemWithTypeAndId(PreparationTypeTuning, synchronicTuning->getId());
                    
                    if (synchronicTuningItem == nullptr)
                    {
                        synchronicTuningItem = new BKItem(PreparationTypeTuning, synchronicTuning->getId(), processor);
                        
                        synchronicTuningItem->setPianoTarget(-1);
                        
                        synchronicTuningItem->setItemName("Tuning" + String(synchronicTuning->getId()));
                        
                        synchronicTuningItem->setTopLeftPosition(10, 10);
                        
                        synchronicTuningItem->setActive(true);
                        
                        thisPiano->items.add(synchronicTuningItem);
                    }
                    
                    BKItem* tempoItem = thisPiano->itemWithTypeAndId(PreparationTypeTempo, thisTempo->getId());
                    
                    if (tempoItem == nullptr)
                    {
                        tempoItem = new BKItem(PreparationTypeTempo, thisTempo->getId(), processor);
                        
                        tempoItem->setPianoTarget(-1);
                        
                        tempoItem->setItemName("Tempo" + String(thisTempo->getId()));
                        
                        tempoItem->setTopLeftPosition(10, 10);
                        
                        tempoItem->setActive(true);
                        
                        thisPiano->items.add(tempoItem);
                    }
                    
                    // Make new keymap
                    Keymap::Ptr thisKeymap;
                    if (keys.size() <= 1)
                    {
                        thisKeymap = emptyKeymap;
                    }
                    else
                    {
                        Keymap::Ptr testKeymap = new Keymap();
                        for (int k = 0; k < keys.size(); k += 2 )   testKeymap->addNote(keys[k]);
                        thisKeymap = matches(testKeymap);
                        
                        if (thisKeymap == nullptr)
                        {
                            addKeymap();
                            thisKeymap = bkKeymaps.getLast();
                            for (int k = 0; k < keys.size(); k += 2 )
                            {
                                if (keys[k] != -1) thisKeymap->addNote(keys[k]);
                            }
                        }
                    }
                    
                    BKItem* keymapItem = thisPiano->itemWithTypeAndId(PreparationTypeKeymap, thisKeymap->getId());
                    
                    if (keymapItem == nullptr)
                    {
                        keymapItem = new BKItem(PreparationTypeKeymap, thisKeymap->getId(), processor);
                        
                        keymapItem->setPianoTarget(-1);
                        
                        keymapItem->setItemName("Keymap" + String(thisKeymap->getId()));
                        
                        keymapItem->setTopLeftPosition(10, 10);
                        
                        keymapItem->setActive(true);
                        
                        thisPiano->items.add(keymapItem);
                    }
        
                    // ATTACH KEYMAP AND SYNCHRONIC
                    keymapItem->addConnection(synchronicItem);
                    synchronicItem->addConnection(keymapItem);
                    
                    tempoItem->addConnection(synchronicItem);
                    synchronicItem->addConnection(tempoItem);
                    
                    synchronicTuningItem->addConnection(synchronicItem);
                    synchronicItem->addConnection(synchronicTuningItem);
                    
                    
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
                    tunePrep->setScale(revTuning);
                    tunePrep->setFundamental((PitchClass)((12-revFundamental)%12));
                    
                    nostalgicTuning = matches(tunePrep);
                    
                    if (nostalgicTuning == nullptr)
                    {
                        addTuning(tunePrep);
                        nostalgicTuning = tuning.getLast();
                    }
                    tId = nostalgicTuning->getId();
                    
                    NostalgicPreparation::Ptr nostPrep = new NostalgicPreparation();
                    
                    float gain = jsonGetValue(nx+"gain");
                    
                    nostPrep->setGain(gain);
                    
                    float memoryMult = jsonGetValue(nx+"memoryMult");
                    
                    nostPrep->setLengthMultiplier(memoryMult);
                    
                    float undertow = jsonGetValue(nx+"undertow");
                    
                    nostPrep->setUndertow(undertow);
                    
                    float wavedistance = jsonGetValue(nx+"wavedistance");
                    
                    nostPrep->setWaveDistance(wavedistance);
                    
                    float transp = jsonGetValue(nx+"transposition");
                    
                    Array<float> transps; transps.add(transp);
                    nostPrep->setTransposition(transps);
                    
                    float beatsToSkip = jsonGetValue(nx+"revBeatsToSkip");
                    
                    nostPrep->setBeatsToSkip(beatsToSkip);
                    
                    int reverseSyncMode = jsonGetValue(nx+"reverseSyncMode");
                    
                    int reverseSyncTarget = sId;
                    
                    if (!isOld) reverseSyncTarget = jsonGetValue(nx+"reverseSyncTarget");
                    
                    Synchronic::Ptr thisSynchronic;
                    
                    if (reverseSyncMode)
                    {
                        thisSynchronic = synchronicTarget;
                    }
                    else
                    {
                        // CONNECT TO DEFAULT SYNCHRONIC
                        thisSynchronic = defaultSynchronic;
                    }
                    
                    Nostalgic::Ptr thisNostalgic = matches(nostPrep);
                    if (thisNostalgic == nullptr)
                    {
                        addNostalgic(nostPrep);
                        thisNostalgic = nostalgic.getLast();
                    }
                    nId = thisNostalgic->getId();
                    
                    BKItem* nostalgicItem = thisPiano->itemWithTypeAndId(PreparationTypeNostalgic, thisNostalgic->getId());
                    
                    if (nostalgicItem == nullptr)
                    {
                        nostalgicItem = new BKItem(PreparationTypeNostalgic, thisNostalgic->getId(), processor);
                        
                        nostalgicItem->setPianoTarget(-1);
                        
                        nostalgicItem->setItemName("Nostalgic" + String(thisNostalgic->getId()));
                        
                        nostalgicItem->setTopLeftPosition(10, 10);
                        
                        nostalgicItem->setActive(true);
                        
                        thisPiano->items.add(nostalgicItem);
                    }
                    
                    nostalgicTuningItem = thisPiano->itemWithTypeAndId(PreparationTypeTuning, nostalgicTuning->getId());
                    
                    if (nostalgicTuningItem == nullptr)
                    {
                        nostalgicTuningItem = new BKItem(PreparationTypeTuning, nostalgicTuning->getId(), processor);
                        
                        nostalgicTuningItem->setPianoTarget(-1);
                        
                        nostalgicTuningItem->setItemName("Tuning" + String(nostalgicTuning->getId()));
                        
                        nostalgicTuningItem->setTopLeftPosition(10, 10);
                        
                        nostalgicTuningItem->setActive(true);
                        
                        thisPiano->items.add(nostalgicTuningItem);
                    }
                    
                    BKItem* synchronicItem = thisPiano->itemWithTypeAndId(PreparationTypeSynchronic, thisSynchronic->getId());
                    
                    if (synchronicItem == nullptr)
                    {
                        synchronicItem = new BKItem(PreparationTypeSynchronic, thisSynchronic->getId(), processor);
                        
                        synchronicItem->setPianoTarget(-1);
                        
                        synchronicItem->setItemName("Synchronic" + String(thisSynchronic->getId()));
                        
                        synchronicItem->setTopLeftPosition(10, 10);
                        
                        synchronicItem->setActive(true);
                        
                        thisPiano->items.add(synchronicItem);
                    }
                    
                    // Make new keymap
                    Keymap::Ptr thisKeymap;
                    if (keys.size() <= 1)
                    {
                        thisKeymap = emptyKeymap;
                    }
                    else
                    {
                        Keymap::Ptr testKeymap = new Keymap();
                        for (int k = 0; k < keys.size(); k += 2 )   testKeymap->addNote(keys[k]);
                        thisKeymap = matches(testKeymap);
                        
                        if (thisKeymap == nullptr)
                        {
                            addKeymap();
                            thisKeymap = bkKeymaps.getLast();
                            for (int k = 0; k < keys.size(); k += 2 )
                            {
                                if (keys[k] != -1) thisKeymap->addNote(keys[k]);
                            }
                        }
                    }
                
                    BKItem* keymapItem = thisPiano->itemWithTypeAndId(PreparationTypeKeymap, thisKeymap->getId());
                    
                    if (keymapItem == nullptr)
                    {
                        keymapItem = new BKItem(PreparationTypeKeymap, thisKeymap->getId(), processor);
                        
                        keymapItem->setPianoTarget(-1);
                        
                        keymapItem->setItemName("Keymap" + String(thisKeymap->getId()));
                        
                        keymapItem->setTopLeftPosition(10, 10);
                        
                        keymapItem->setActive(true);
                        
                        thisPiano->items.add(keymapItem);
                    }
                    

                    // CONNECT KEYMAP AND NOSTALGIC AND TUNING
                    keymapItem->addConnection(nostalgicItem);
                    nostalgicItem->addConnection(keymapItem);
                    
                    nostalgicTuningItem->addConnection(nostalgicItem);
                    nostalgicItem->addConnection(nostalgicTuningItem);
                    
                    synchronicItem->addConnection(nostalgicItem);
                    nostalgicItem->addConnection(synchronicItem);
                    
                    DBG("nostalgicPrep: " + String(nId));
                }
                
                
                // DIRECT
                jsonGetKeys(dx+"DirectKeys");
                
                
                if (isLayer)
                {
                    DirectPreparation::Ptr dPrep = new DirectPreparation();
                    
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
                    
                    Direct::Ptr thisDirect = matches(dPrep);
                    if (thisDirect == nullptr)
                    {
                        addDirect(dPrep);
                        thisDirect = direct.getLast();
                        
                    }
                    
                    BKItem* directItem = thisPiano->itemWithTypeAndId(PreparationTypeDirect, thisDirect->getId());
                    
                    if (directItem == nullptr)
                    {
                        directItem = new BKItem(PreparationTypeDirect, thisDirect->getId(), processor);
                        
                        directItem->setPianoTarget(-1);
                        
                        directItem->setItemName("Direct" + String(thisDirect->getId()));
                        
                        directItem->setTopLeftPosition(10, 10);
                        
                        directItem->setActive(true);
                        
                        thisPiano->items.add(directItem);
                    }
                    
                    // Make new keymap
                    Keymap::Ptr thisKeymap;
                    if (keys.size() <= 1)
                    {
                        thisKeymap = emptyKeymap;
                    }
                    else
                    {
                        Keymap::Ptr testKeymap = new Keymap();
                        for (int k = 0; k < keys.size(); k += 2 )   testKeymap->addNote(keys[k]);
                        thisKeymap = matches(testKeymap);
                        
                        if (thisKeymap == nullptr)
                        {
                            addKeymap();
                            thisKeymap = bkKeymaps.getLast();
                            for (int k = 0; k < keys.size(); k += 2 )
                            {
                                if (keys[k] != -1) thisKeymap->addNote(keys[k]);
                            }
                        }
                    }
                    
                    BKItem* keymapItem = thisPiano->itemWithTypeAndId(PreparationTypeKeymap, thisKeymap->getId());
                    
                    if (keymapItem == nullptr)
                    {
                        keymapItem = new BKItem(PreparationTypeKeymap, thisKeymap->getId(), processor);
                        
                        keymapItem->setPianoTarget(-1);
                        
                        keymapItem->setItemName("Keymap" + String(thisKeymap->getId()));
                        
                        keymapItem->setTopLeftPosition(10, 10);
                        
                        keymapItem->setActive(true);
                        
                        thisPiano->items.add(keymapItem);
                    }
                    
                    thisPiano->items.add(directTuningItem);
                    
                    keymapItem->addConnection(directItem);
                    directItem->addConnection(keymapItem);
                    
                    directTuningItem->addConnection(directItem);
                    directItem->addConnection(directTuningItem);
                    
                    
                    
                    DBG("directPrep: " + String(thisDirect->getId()));
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
                        // Make new keymap
                        Keymap::Ptr thisKeymap;

                        Keymap::Ptr testKeymap = new Keymap();
                        testKeymap->addNote(key);
                        
                        thisKeymap = matches(testKeymap);
                        
                        if (thisKeymap == nullptr)
                        {
                            addKeymap(testKeymap);
                            thisKeymap = bkKeymaps.getLast();
                        }
                        
                        
                        // MAKE PIANO MAP ITEM AND CONNECT TO KEYMAP
                        
                        int thisPianoMapId = pianoMapId++;
                        
                        BKItem* pianoMapItem = thisPiano->itemWithTypeAndId(PreparationTypePianoMap, thisPianoMapId);
                        
                        if (pianoMapItem == nullptr)
                        {
                            pianoMapItem = new BKItem(PreparationTypePianoMap, thisPianoMapId, processor);
                            
                            pianoMapItem->setPianoTarget(pId);
                            
                            pianoMapItem->setItemName("PianoMap" + String(thisPianoMapId));
                            
                            pianoMapItem->setTopLeftPosition(10, 10);
                            
                            pianoMapItem->setActive(true);
                            
                            thisPiano->items.add(pianoMapItem);
                        }
            
                        BKItem* keymapItem = thisPiano->itemWithTypeAndId(PreparationTypeKeymap, thisKeymap->getId());
                        
                        if (keymapItem == nullptr)
                        {
                            keymapItem = new BKItem(PreparationTypeKeymap, thisKeymap->getId(), processor);
                            
                            keymapItem->setPianoTarget(-1);
                            
                            keymapItem->setItemName("Keymap" + String(thisKeymap->getId()));
                            
                            keymapItem->setTopLeftPosition(10, 10);
                            
                            keymapItem->setActive(true);
                            
                            thisPiano->items.add(keymapItem);
                        }
                        
                        keymapItem->addConnection(pianoMapItem);
                        pianoMapItem->addConnection(keymapItem);
                        
                    }
                }
            }
            
            
            String tun,note,notenum;
            
            // IF mapType < 3 THEN MAYBE GETS EM ALL
            for (int mapType = 0; mapType < 1; mapType++)
            {
                var tuningmap;
                
                if (mapType == 0)       tuningmap = jsonGetProperty("tuning::tuningMap");
                else if (mapType == 1)  tuningmap = jsonGetProperty("tuning::nostalgicTuningSub::nostalgicTuningMap");
                else if (mapType == 2)  tuningmap = jsonGetProperty("tuning::synchronicTuningSub::synchronicTuningMap");
                
                if (tuningmap.toString() == "<empty>") continue;
                
                if (!(tuningmap.size() % 3))
                {
                    for (int j = 0; j < tuningmap.size(); j += 3)
                    {
                        tun = tuningmap[j]; note = tuningmap[j+1]; notenum = tuningmap[j+2];
                        
                        PitchClass fund = letterNoteToPitchClass(note);
                        TuningSystem tscale = tuningStringToTuningSystem(tun);
                        int noteNumber = notenum.getIntValue();
                        
                        
                        TuningModPreparation::Ptr testMod = new TuningModPreparation(-1);
                        
                        testMod->setParam(TuningFundamental, String(fund));
                        testMod->setParam(TuningScale, String(tscale));
                        
                        TuningModPreparation::Ptr thisTuningMod = matches(testMod);
                        
                        if (thisTuningMod == nullptr)
                        {
                            addTuningMod(testMod);
                            thisTuningMod = modTuning.getLast();
                        }
                        
                        // Make new keymap
                        Keymap::Ptr thisKeymap;
                        
                        Keymap::Ptr testKeymap = new Keymap();
                        testKeymap->addNote(noteNumber);
                        thisKeymap = matches(testKeymap);
                        
                        if (thisKeymap == nullptr)
                        {
                            addKeymap(testKeymap);
                            thisKeymap = bkKeymaps.getLast();
                        }
                        
                        
                        BKItem* modItem = thisPiano->itemWithTypeAndId(PreparationTypeTuningMod, thisTuningMod->getId());
                        
                        if (modItem == nullptr)
                        {
                            modItem = new BKItem(PreparationTypeTuningMod, thisTuningMod->getId(), processor);
                            
                            modItem->setPianoTarget(-1);
                            
                            modItem->setItemName("TuningMod" + String(thisTuningMod->getId()));
                            
                            modItem->setTopLeftPosition(10, 10);
                            
                            modItem->setActive(true);
                            
                            thisPiano->items.add(modItem);
                        }
                        
                        BKItem* keymapItem = thisPiano->itemWithTypeAndId(PreparationTypeKeymap, thisKeymap->getId());
                        
                        if (keymapItem == nullptr)
                        {
                            keymapItem = new BKItem(PreparationTypeKeymap, thisKeymap->getId(), processor);
                            
                            keymapItem->setPianoTarget(-1);
                            
                            keymapItem->setItemName("Keymap" + String(thisKeymap->getId()));
                            
                            keymapItem->setTopLeftPosition(10, 10);
                            
                            keymapItem->setActive(true);
                            
                            thisPiano->items.add(keymapItem);
                        }
                        
                        // CONNECT TUNINGMOD AND KEYMAP AND directTuning
                        keymapItem->addConnection(modItem);
                        modItem->addConnection(keymapItem);
                        
                        if (mapType == 0)
                        {
                            directTuningItem->addConnection(modItem);
                            modItem->addConnection(directTuningItem);
                        }
                        else if (mapType == 1)
                        {
                            nostalgicTuningItem->addConnection(modItem);
                            modItem->addConnection(nostalgicTuningItem);
                        }
                        else if (mapType == 2)
                        {
                            synchronicTuningItem->addConnection(modItem);
                            modItem->addConnection(synchronicTuningItem);
                        }
                        
                        
                    }
                }
            }
        }
    }
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
        thisPiano->modificationMap[noteNumber]->addTuningModification(new TuningModification(noteNumber, whichPrep, TuningFundamental, String(fund), whichTMod);
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
