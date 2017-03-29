/*
  ==============================================================================

    XMLGallery.cpp
    Created: 22 Mar 2017 12:31:17pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PluginProcessor.h"


ValueTree  Gallery::getState(void)
{
    ValueTree galleryVT( vtagGallery);
    
    galleryVT.addChild(general->getState(), -1, 0);
    
    // Preparations and keymaps must be first.
    // Tempo and Tuning must be first of the preparations.
    for (int i = 0; i < tempo.size(); i++)                  galleryVT.addChild( tempo[i]->getState(), -1, 0);
    
    for (int i = 0; i < tuning.size(); i++)                 galleryVT.addChild( tuning[i]->getState(), -1, 0);
    
    for (int i = 0; i < direct.size(); i++)                 galleryVT.addChild( direct[i]->getState(), -1, 0);
    
    for (int i = 0; i < synchronic.size(); i++)             galleryVT.addChild( synchronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < nostalgic.size(); i++)              galleryVT.addChild( nostalgic[i]->getState(), -1, 0);
    
    for (int i = 0; i < modTempo.size(); i++)               galleryVT.addChild( modTempo[i]->getState(i), -1, 0);
    
    for (int i = 0; i < modTuning.size(); i++)              galleryVT.addChild( modTuning[i]->getState(i), -1, 0);
    
    for (int i = 0; i < modDirect.size(); i++)              galleryVT.addChild( modDirect[i]->getState(i), -1, 0);
    
    for (int i = 0; i < modSynchronic.size(); i++)          galleryVT.addChild( modSynchronic[i]->getState(i), -1, 0);
    
    for (int i = 0; i < modNostalgic.size(); i++)           galleryVT.addChild( modNostalgic[i]->getState(i), -1, 0);
    
    for (int i = 0; i < bkKeymaps.size(); i++)              galleryVT.addChild( bkKeymaps[i]->getState(i), -1, 0);
    
    // Pianos
    for (int piano = 0; piano < bkPianos.size(); piano++)   galleryVT.addChild( bkPianos[piano]->getState(), -1, 0);
    
    return galleryVT;
    
    
}

void Gallery::setStateFromXML(ScopedPointer<XmlElement> xml)
{
    
    float f;
    int i;
    bool b;
    Array<float> fa;
    Array<int> fi;
    
    int pianoCount = 0, sPrepCount = 1, sModPrepCount = 1, nPrepCount = 1, nModPrepCount = 1, dPrepCount = 1, dModPrepCount = 1, tPrepCount = 1, tModPrepCount = 1, keymapCount = 1, tempoPrepCount = 1, tempoModPrepCount = 1;
    
    {
        
        if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
        {
            addTempo(); // should happen first
            addTuning();// should happen first
            addSynchronic();
            addNostalgic();
            addDirect();
            
            
            addTuningMod();
            addSynchronicMod();
            addNostalgicMod();
            addDirectMod();
            addTempoMod();
            
            general = new GeneralSettings();
            
            
            /* * * * * * * * * * * * * * */
            
            bkKeymaps.set(0, new Keymap(0));
            bkKeymaps[0]->setName("Empty");
            
            // iterate through its sub-elements
            forEachXmlChildElement (*xml, e)
            {
                if (e->hasTagName( vtagKeymap + String(keymapCount)))
                {
                    int id = keymapCount++;
                    
                    bkKeymaps.set(id, new Keymap(id));
                    
                    
                    String n = e->getStringAttribute("name");
                    
                    if (n != String::empty)     bkKeymaps[id]->setName(n);
                    else                        bkKeymaps[id]->setName(String(id+1));
                    
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
                    general->setState(e);
                }
                else if (e->hasTagName( vtagTuning + String(tPrepCount)))
                {
                    addTuning();
                    
                    int id = tuning.size() - 1;
                    
                    Tuning::Ptr thisTuning = tuning[id];
                    
                    thisTuning->setState(e);
                    
                    ++tPrepCount;
                }
                else if (e->hasTagName( vtagModTuning + String(tModPrepCount)))
                {
                    addTuningMod();
                    
                    int id = modTuning.size() - 1;
                    
                    TuningModPreparation::Ptr thisModTuning = modTuning[id];
                    
                    thisModTuning->setState(e);
                    
                    ++tModPrepCount;
                }
                else if (e->hasTagName( vtagDirect + String(dPrepCount)))
                {
                    addDirect();
                    
                    int id = direct.size()-1;
                    
                    Direct::Ptr thisDirect = direct[id];
                    
                    thisDirect->setState(e, tuning);
                    
                    ++dPrepCount;
                }
                else if (e->hasTagName( vtagModDirect + String(dModPrepCount)))
                {
                    addDirectMod();
                    
                    int id = modDirect.size()-1;
                    
                    DirectModPreparation::Ptr thisModDirect = modDirect[id];
                    
                    thisModDirect->setState(e);
                    
                    ++dModPrepCount;
                }
                else if (e->hasTagName( vtagSynchronic + String(sPrepCount)))
                {
                    addSynchronic();
                    
                    int id = synchronic.size() - 1;
                    
                    Synchronic::Ptr thisSynchronic = synchronic[id];
                    
                    thisSynchronic->setState(e, tuning, tempo);
                    
                    ++sPrepCount;
                    
                }
                else if (e->hasTagName( vtagModSynchronic + String(sModPrepCount)))
                {
                    addSynchronicMod();
                    
                    int id = modSynchronic.size() - 1;
                    
                    SynchronicModPreparation::Ptr thisModSynchronic = modSynchronic[id];
                    
                    thisModSynchronic->setState(e);
                    
                    ++sModPrepCount;
                    
                }
                else if (e->hasTagName( vtagTempo + String(tempoPrepCount)))
                {
                    addTempo();
                    
                    int id = tempo.size() - 1;
                    
                    Tempo::Ptr thisTempo = tempo[id];
                    
                    thisTempo->setState(e);
                    
                    ++tempoPrepCount;
                    
                }
                else if (e->hasTagName( vtagModTempo + String(tempoModPrepCount)))
                {
                    addTempoMod();
                    
                    int id = modTempo.size() - 1;
                    
                    TempoModPreparation::Ptr thisModTempo = modTempo[id];
                    
                    thisModTempo->setState(e);
                    
                    ++tempoModPrepCount;
                    
                }
                else if (e->hasTagName( vtagNostalgic + String(nPrepCount)))
                {
                    addNostalgic();
                    
                    int id = nostalgic.size() - 1;
                    
                    Nostalgic::Ptr thisNostalgic = nostalgic[id];
                    
                    thisNostalgic->setState(e, tuning, synchronic);
                    
                    ++nPrepCount;
                }
                else if (e->hasTagName( vtagModNostalgic + String(nModPrepCount)))
                {
                    addNostalgicMod();
                    
                    int id = modNostalgic.size() - 1;
                    
                    NostalgicModPreparation::Ptr thisModNostalgic = modNostalgic[id];
                    
                    thisModNostalgic->setState(e);
                    
                    ++nModPrepCount;
                }
                else if (e->hasTagName( vtagPiano + String(pianoCount)))
                {
                    int whichPiano = pianoCount++;
                    
                    bkPianos.set(whichPiano, new Piano(synchronic, nostalgic, direct, tuning, tempo,
                                                       modSynchronic, modNostalgic, modDirect, modTuning, modTempo,
                                                       bkKeymaps, whichPiano)); // initializing piano 0
                    
                    Piano::Ptr thisPiano = bkPianos[whichPiano];
                    
                    thisPiano->setState(e);
                    
                }
            }
        }
        
        for (int k = tuning.size(); --k >= 0;)      tuning[k]->prepareToPlay(bkSampleRate);
        for (int k = tempo.size(); --k >= 0;)       tempo[k]->prepareToPlay(bkSampleRate);
        for (int k = synchronic.size(); --k >= 0;)  synchronic[k]->prepareToPlay(bkSampleRate);
        for (int k = nostalgic.size(); --k >= 0;)   nostalgic[k]->prepareToPlay(bkSampleRate);
        for (int k = direct.size(); --k >= 0;)      direct[k]->prepareToPlay(bkSampleRate);
        
    }
    
    
}

