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
    
    galleryVT.setProperty("url", url, 0);
    
    ValueTree idCountVT( "idCount");
    
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idCountVT.setProperty( "i"+String(i), idCount[i], 0);
    }
    
    galleryVT.addChild(idCountVT, -1, 0);
    
    
    // Useful if we want to be able to reconfigure/organize menus of preparations.
    ValueTree idIndexVT ("idIndexList");
    
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        ValueTree vt ("index" + String(i));
        
        Array<int> index = idIndexList.getUnchecked(i);
        
        int count = 0;
        for (auto id : index)
        {
            vt.setProperty("i"+String(count++), id, 0);
        }
        
        idIndexVT.addChild(vt, -1, 0);
    }
    
    galleryVT.addChild(idIndexVT, -1, 0);
    
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
    
    galleryVT.setProperty("defaultPiano", getCurrentPiano(), 0);
    
    return galleryVT;
    
    
}

void Gallery::setStateFromXML(ScopedPointer<XmlElement> xml)
{
    
    float f;
    int i;
    bool b;
    Array<float> fa;
    Array<int> fi;
    
    bkPianos.clear();
    
    int pianoCount = 0, sPrepCount = 0, sModPrepCount = 0, nPrepCount = 0, nModPrepCount = 0, dPrepCount = 0, dModPrepCount = 0, tPrepCount = 0, tModPrepCount = 0, keymapCount = 0, tempoPrepCount = 0, tempoModPrepCount = 0;
    
    {
        
        if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
        {
            //general = new GeneralSettings();
            
            /* * * * * * * * * * * * * * */
            url = xml->getStringAttribute("url");
            setCurrentPiano(xml->getStringAttribute("defaultPiano").getIntValue());

            DBG("loaded url: " + url);
            
            // iterate through its sub-elements
            forEachXmlChildElement (*xml, e)
            {
                /*
                if (e->hasTagName("idCount"))
                {
                    
                    for (int idType = 0; idType < (BKPreparationTypeNil-1); idType++)
                    {
                        String attr = e->getStringAttribute("i"+String(idType));
                        
                        DBG("id" + String(idType) + " " + attr);
                        
                        i = attr.getIntValue();
                        
                        idCount.set(idType, i);
                        
                    }
                    
                    DBG("idCountsPOSTLOAD: " + intArrayToString(idCount));
                }
                */
                
                if (e->hasTagName( vtagKeymap + String(keymapCount)))
                {
                    addKeymap();
                    
                    String n = e->getStringAttribute("name");
                    
                    Keymap::Ptr newKeymap = bkKeymaps.getLast();
                    
                    if (n != String::empty)     newKeymap->setName(n);
                    else                        newKeymap->setName(String(newKeymap->getId()));
                    
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
                    
                    newKeymap->setKeymap(keys);
                    
                    newKeymap->editted = true;
                    
                    ++keymapCount;
                    
                }
                else if (e->hasTagName ( vtagGeneral))
                {
                    general->setState(e);
                }
                else if (e->hasTagName( vtagTuning + String(tPrepCount)))
                {
                    addTuning();
                    
                    tuning.getLast()->setState(e);
                    
                    ++tPrepCount;
                }
                else if (e->hasTagName( vtagModTuning + String(tModPrepCount)))
                {
                    addTuningMod();
                    
                    modTuning.getLast()->setState(e);
                    
                    ++tModPrepCount;
                }
                else if (e->hasTagName( vtagDirect + String(dPrepCount)))
                {
                    addDirect();
                    
                    direct.getLast()->setState(e, tuning);
                    
                    ++dPrepCount;
                }
                else if (e->hasTagName( vtagModDirect + String(dModPrepCount)))
                {
                    addDirectMod();
                    
                    modDirect.getLast()->setState(e);
                    
                    ++dModPrepCount;
                }
                else if (e->hasTagName( vtagSynchronic + String(sPrepCount)))
                {
                    addSynchronic();
                    
                    synchronic.getLast()->setState(e, tuning, tempo);
                    
                    ++sPrepCount;
                    
                }
                else if (e->hasTagName( vtagModSynchronic + String(sModPrepCount)))
                {
                    addSynchronicMod();
                
                    modSynchronic.getLast()->setState(e);
                    
                    ++sModPrepCount;
                    
                }
                else if (e->hasTagName( vtagTempo + String(tempoPrepCount)))
                {
                    addTempo();

                    tempo.getLast()->setState(e);
                    
                    ++tempoPrepCount;
                    
                }
                else if (e->hasTagName( vtagModTempo + String(tempoModPrepCount)))
                {
                    addTempoMod();
                    
                    modTempo.getLast()->setState(e);
                    
                    ++tempoModPrepCount;
                    
                }
                else if (e->hasTagName( vtagNostalgic + String(nPrepCount)))
                {
                    addNostalgic();
                    
                    nostalgic.getLast()->setState(e, tuning, synchronic);
                    
                    ++nPrepCount;
                }
                else if (e->hasTagName( vtagModNostalgic + String(nModPrepCount)))
                {
                    addNostalgicMod();
                    
                    modNostalgic.getLast()->setState(e);
                    
                    ++nModPrepCount;
                }
                else if (e->hasTagName( vtagPiano + String(pianoCount)))
                {
                    addPiano();
                    
                    bkPianos.getLast()->setState(e);
                    
                    ++pianoCount;
                    
                }
            }
        }
        
        for (int k = tuning.size(); --k >= 0;)      tuning[k]->prepareToPlay(bkSampleRate);
        for (int k = tempo.size(); --k >= 0;)       tempo[k]->prepareToPlay(bkSampleRate);
        for (int k = synchronic.size(); --k >= 0;)  synchronic[k]->prepareToPlay(bkSampleRate);
        for (int k = nostalgic.size(); --k >= 0;)   nostalgic[k]->prepareToPlay(bkSampleRate);
        for (int k = direct.size(); --k >= 0;)      direct[k]->prepareToPlay(bkSampleRate);
        
    }
    
    printPianoConfigurations();
    
}

