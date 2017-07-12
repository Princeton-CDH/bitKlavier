/*
  ==============================================================================

    Gallery.cpp
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Gallery.h"

Gallery::Gallery(ScopedPointer<XmlElement> xml):
url(String::empty)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idCount.add(-1);
        idIndexList.set(i,Array<int>());
    }
    

    setStateFromXML(xml);
}

Gallery::Gallery(ScopedPointer<XmlElement> xml, BKSynthesiser* m, BKSynthesiser* r, BKSynthesiser* h, BKUpdateState::Ptr state):
updateState(state),
main(m),
res(r),
hammer(h),
url(String::empty)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idCount.add(-1);
        idIndexList.set(i,Array<int>());
    }

    setStateFromXML(xml);
}

Gallery::Gallery(var myJson):
url(String::empty)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idCount.add(-1);
        idIndexList.set(i,Array<int>());
    }

    setStateFromJson(myJson);
}

Gallery::Gallery(var myJson, BKSynthesiser* m, BKSynthesiser* r, BKSynthesiser* h, BKUpdateState::Ptr state):
updateState(state),
main(m),
res(r),
hammer(h),
url(String::empty)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idCount.add(-1);
        idIndexList.set(i,Array<int>());
    }
    
    setStateFromJson(myJson);
    
}

void Gallery::prepareToPlay (double sampleRate)
{
    for (int i = bkPianos.size(); --i >= 0;) bkPianos[i]->prepareToPlay(sampleRate);
    
    for (int i = tuning.size(); --i >= 0;)      tuning[i]->prepareToPlay(sampleRate);
    for (int i = tempo.size(); --i >= 0;)       tempo[i]->prepareToPlay(sampleRate);
    for (int i = synchronic.size(); --i >= 0;)  synchronic[i]->prepareToPlay(sampleRate);
    for (int i = nostalgic.size(); --i >= 0;)   nostalgic[i]->prepareToPlay(sampleRate);
    for (int i = direct.size(); --i >= 0;)      direct[i]->prepareToPlay(sampleRate);
    
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

