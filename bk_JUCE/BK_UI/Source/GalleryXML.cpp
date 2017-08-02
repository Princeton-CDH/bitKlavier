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
    
    bkPianos.clear();
    
    {
        
        if (xml != nullptr)
        {
            //general = new GeneralSettings();
            
            /* * * * * * * * * * * * * * */
            url = xml->getStringAttribute("url");

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
                
                if (e->hasTagName( vtagKeymap))
                {
                    addKeymap();
                    
                    String n = e->getStringAttribute("name");
                    
                    
                    
                    Keymap::Ptr newKeymap = bkKeymaps.getLast();
                    
                    newKeymap->setId(e->getStringAttribute("Id").getIntValue());
                    
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
                }
                else if (e->hasTagName ( vtagGeneral))
                {
                    general->setState(e);
                }
                else if (e->hasTagName( vtagTuning))
                {
                    addTuningWithId(-1);
                    
                    tuning.getLast()->setState(e);
                }
                else if (e->hasTagName( vtagModTuning))
                {
                    addTuningModWithId(-1);
                    
                    modTuning.getLast()->setState(e);
                }
                else if (e->hasTagName( vtagDirect))
                {
                    addDirectWithId(-1);
                    
                    direct.getLast()->setState(e, tuning);
                }
                else if (e->hasTagName( vtagModDirect))
                {
                    addDirectModWithId(-1);
                    
                    modDirect.getLast()->setState(e);
                }
                else if (e->hasTagName( vtagSynchronic))
                {
                    addSynchronicWithId(-1);
                    
                    synchronic.getLast()->setState(e, tuning, tempo);
                    
                }
                else if (e->hasTagName( vtagModSynchronic))
                {
                    addSynchronicModWithId(-1);
                
                    modSynchronic.getLast()->setState(e);
                    
                }
                else if (e->hasTagName( vtagTempo))
                {
                    addTempoWithId(-1);

                    tempo.getLast()->setState(e);
                    
                }
                else if (e->hasTagName( vtagModTempo))
                {
                    addTempoModWithId(-1);
                    
                    modTempo.getLast()->setState(e);
                    
                }
                else if (e->hasTagName( vtagNostalgic))
                {
                    addNostalgicWithId(-1);
                    
                    nostalgic.getLast()->setState(e, tuning, synchronic);
                }
                else if (e->hasTagName( vtagModNostalgic))
                {
                    addNostalgicModWithId(-1);
                    
                    modNostalgic.getLast()->setState(e);
                }
                else if (e->hasTagName(vtagPiano))
                {
                    addPianoWithId(-1);
                    
                    bkPianos.getLast()->setState(e);
                }
            }
        }
    }
    
}

