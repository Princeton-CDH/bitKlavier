/*
  ==============================================================================

    Gallery.cpp
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Gallery.h"

#include "PluginProcessor.h"

Gallery::Gallery(ScopedPointer<XmlElement> xml, BKAudioProcessor& p):
processor(p),
url(String::empty)
{    
    general = new GeneralSettings();

    setStateFromXML(xml);
}

Gallery::Gallery(var myJson, BKAudioProcessor& p):
processor(p),
url(String::empty)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idCount.add(10);
    }
    
    general = new GeneralSettings();

    setStateFromJson(myJson);
}

void Gallery::prepareToPlay (double sampleRate)
{
    bkSampleRate = sampleRate;
    
    for (auto piano : bkPianos)     piano->prepareToPlay(bkSampleRate);
}

Gallery::~Gallery()
{
    
}

void Gallery::resetPreparations(void)
{
    // Optimizations can be made here. Don't need to iterate through EVERY preparation.
    for (int i = direct.size(); --i >= 0; )
        direct[i]->aPrep->copy(direct[i]->sPrep);
    
    for (int i = nostalgic.size(); --i >= 0; )
        nostalgic[i]->aPrep->copy(nostalgic[i]->sPrep);
    
    for (int i = synchronic.size(); --i >= 0; )
        synchronic[i]->aPrep->copy(synchronic[i]->sPrep);
    
    for (int i = tuning.size(); --i >= 0; )
        tuning[i]->aPrep->copy(tuning[i]->sPrep);
    
    for (int i = tempo.size(); --i >= 0; )
        tempo[i]->aPrep->copy(tempo[i]->sPrep);
}

