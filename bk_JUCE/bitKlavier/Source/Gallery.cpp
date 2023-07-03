/*
  ==============================================================================

    Gallery.cpp
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Gallery.h"

#include "PluginProcessor.h"

Gallery::Gallery(BKAudioProcessor& p):
processor(p)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        used.add(Array<int>({-1}));
        idmap.add(new HashMap<int,int>());
    }
    for (int i = 0; i < PreparationTypeKeymap; i ++)
    {
        genericPrep.add(new ReferenceCountedArray<GenericPreparation>());
    }
    // DBG("idmap size: " + String(idmap.size()));
    
    general = new GeneralSettings();
    
    addDefaultPrepIfNotThere();
    
    isDirty = false;
}

Gallery::Gallery(XmlElement* xml, BKAudioProcessor& p):
processor(p)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        used.add( Array<int>( {-1} ) );
        idmap.add(new HashMap<int,int>());
    }
    for (int i = 0; i < PreparationTypeKeymap; i ++)
    {
        genericPrep.add(new ReferenceCountedArray<GenericPreparation>());
    }
    // DBG("idmap size: " + String(idmap.size()));
    
    general = new GeneralSettings();

    setStateFromXML(xml);
    
    addDefaultPrepIfNotThere();
    
    isDirty = false;
}

Gallery::Gallery(var myJson, BKAudioProcessor& p):
processor(p),
url(String())
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idcounts[i] = 1;
        idmap.add(new HashMap<int,int>());
    }
    for (int i = 0; i < PreparationTypeKeymap; i ++)
    {
        genericPrep.add(new ReferenceCountedArray<GenericPreparation>());
    }
    // DBG("idmap size: " + String(idmap.size()));
    
    general = new GeneralSettings();

    addDefaultPrepIfNotThere();
    
    setStateFromJson(myJson);
    
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        used.add(Array<int>({-1}));
    }
    
    isDirty = false;
}



void Gallery::prepareToPlay ()
{
    for (auto piano : bkPianos) piano->prepMap->prepareToPlay(general);
}

Gallery::~Gallery()
{
    bkPianos.clear();
}

void Gallery::resetPreparations(void)
{
    // Optimizations can be made here. Don't need to iterate through EVERY preparation.
//    for (int i = direct.size(); --i >= 0; )
//        direct[i]->prep->resetModdables();
    
    for (int i = nostalgic.size(); --i >= 0; )
        nostalgic[i]->prep->resetModdables();
    
//    for (int i = synchronic.size(); --i >= 0; )
//        synchronic[i]->prep->resetModdables();
    
//    for (int i = genericPrep[PreparationTypeResonance]->size(); --i >= 0; )
//        genericPrep[PreparationTypeResonance]->prep->resetModdables();
    
    for (int i = tuning.size(); --i >= 0; )
        tuning[i]->prep->resetModdables();
    
    for (int i = tempo.size(); --i >= 0; )
        tempo[i]->prep->resetModdables();

	for (int i = blendronic.size(); --i >= 0; )
        blendronic[i]->prep->resetModdables();
    
    for (auto objArr : genericPrep)
    {
        for (auto obj : *objArr )
        {
            obj->resetModdables();
        }
    }
}

