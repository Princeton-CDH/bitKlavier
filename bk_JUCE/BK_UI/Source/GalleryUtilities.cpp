/*
  ==============================================================================

    GalleryUtilities.cpp
    Created: 22 Mar 2017 3:53:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Gallery.h"


SynchronicProcessor::Ptr Gallery::getSynchronicProcessor(int id)
{
    for (int i = synchronic.size(); --i >= 0;)
    {
        if(synchronic[i]->getId() == id) {
            //DBG("got synchronic processor id " + String(id));
            return synchronic[i]->processor;
        }
    }
    
    //else
    DBG("synchronic processor not found, returning first processor");
    return synchronic[0]->processor;
}

NostalgicProcessor::Ptr Gallery::getNostalgicProcessor(int id)
{
    for (int i = nostalgic.size(); --i >= 0;)
    {
        if(nostalgic[i]->getId() == id) {
            //DBG("got nostalgic processor id " + String(id));
            return nostalgic[i]->processor;
        }
    }
    
    //else
    DBG("nostalgic processor not found, returning first processor");
    return nostalgic[0]->processor;
}

TuningProcessor::Ptr Gallery::getTuningProcessor(int id)
{
    for (int i = tuning.size(); --i >= 0;)
    {
        if(tuning[i]->getId() == id) {
            //DBG("got tuning processor id " + String(id));
            return tuning[i]->processor;
        }
    }
    
    //else
    DBG("tuning processor not found, returning first processor");
    return tuning[0]->processor;
}

TempoProcessor::Ptr Gallery::getTempoProcessor(int id)
{
    for (int i = tempo.size(); --i >= 0;)
    {
        if(tempo[i]->getId() == id) {
            //DBG("got tempo processor id " + String(id));
            return tempo[i]->processor;
        }
    }
    
    //else
    DBG("tempo processor not found, returning first processor");
    return tempo[0]->processor;
}

void Gallery::addPiano()
{
    int numPianos = bkPianos.size();
    bkPianos.add(new Piano(&synchronic, &nostalgic, &direct, &tuning, &tempo,
                           &modSynchronic, &modNostalgic, &modDirect, &modTuning, &modTempo,
                           &bkKeymaps, numPianos));
}

void Gallery::removePiano(int Id)
{
    bkPianos.remove(Id);
}

void Gallery::removeSynchronicModPreparation(int Id)
{
    modSynchronic.remove(Id);
}

void Gallery::removeNostalgicModPreparation(int Id)
{
    modNostalgic.remove(Id);
}

void Gallery::removeDirectModPreparation(int Id)
{
    modDirect.remove(Id);
}

void Gallery::removeTuningModPreparation(int Id)
{
    modTuning.remove(Id);
}

void Gallery::removeTempoModPreparation(int Id)
{
    modTempo.remove(Id);
}

void Gallery::removeSynchronic(int Id)
{
    synchronic.remove(Id);
}

void Gallery::removeNostalgic(int Id) 
{
    nostalgic.remove(Id);
}

void Gallery::removeDirect(int Id)
{
    direct.remove(Id);
}

void Gallery::removeTuning(int Id) 
{
    tuning.remove(Id);
}

void Gallery::removeTempo(int Id) 
{
    tempo.remove(Id);
}

void Gallery::addMod(BKPreparationType type)
{
    if (type == PreparationTypeDirect)
    {
        int numMod = modDirect.size();
        modDirect.add           (new DirectModPreparation(numMod));
    }
    else if (type == PreparationTypeNostalgic)
    {
        int numMod = modNostalgic.size();
        modNostalgic.add           (new NostalgicModPreparation(numMod));
    }
    else if (type == PreparationTypeSynchronic)
    {
        int numMod = modSynchronic.size();
        modSynchronic.add           (new SynchronicModPreparation(numMod));
    }
    else if (type == PreparationTypeTuning)
    {
        int numMod = modTuning.size();
        modTuning.add           (new TuningModPreparation(numMod));
    }
    else if (type == PreparationTypeTempo)
    {
        int numMod = modTempo.size();
        modTempo.add           (new TempoModPreparation(numMod));
    }
    
    
}

int Gallery::getNumMod(BKPreparationType type)
{
    if (type == PreparationTypeDirect)
    {
        return modDirect.size();
    }
    else if (type == PreparationTypeNostalgic)
    {
        return modNostalgic.size();
    }
    else if (type == PreparationTypeSynchronic)
    {
        return modSynchronic.size();
    }
    else if (type == PreparationTypeTuning)
    {
        return modTuning.size();
    }
    else if (type == PreparationTypeTempo)
    {
        return modTempo.size();
    }
    
    
}


void Gallery::addDirectMod()
{
    int numMod = modDirect.size();
    modDirect.add           (new DirectModPreparation(numMod));
}

void Gallery::addSynchronicMod()
{
    int numMod = modSynchronic.size();
    modSynchronic.add       (new SynchronicModPreparation(numMod));
}

void Gallery::addNostalgicMod()
{
    int numMod = modNostalgic.size();
    modNostalgic.add        (new NostalgicModPreparation(numMod));
}

void Gallery::addTuningMod()
{
    int numMod = modTuning.size();
    modTuning.add           (new TuningModPreparation(numMod));
}

void Gallery::addTempoMod()
{
    int numMod = modTempo.size();
    modTempo.add           (new TempoModPreparation(numMod));
}

void Gallery::addTuningMod(TuningModPreparation::Ptr tmod)
{
    modTuning.add           (tmod);
}

void Gallery::addTempoMod(TempoModPreparation::Ptr tmod)
{
    modTempo.add           (tmod);
}

void Gallery::addKeymap(void)
{
    int numKeymaps = bkKeymaps.size();
    bkKeymaps.add(new Keymap(numKeymaps));
}

void Gallery::addKeymap(Keymap::Ptr k)
{
    k->setId(bkKeymaps.size());
    bkKeymaps.add(k);
}

void Gallery::addSynchronic(void)
{
    int numSynchronic = synchronic.size();
    synchronic.add(new Synchronic(main, tuning[0], tempo[0], general, updateState, numSynchronic));
    synchronic.getLast()->prepareToPlay(bkSampleRate);
}

void Gallery::addSynchronic(SynchronicPreparation::Ptr sync)
{
    int numSynchronic = synchronic.size();
    synchronic.add(new Synchronic(main, sync, general, numSynchronic));
    synchronic.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

// Returns index of Synchronic to be added/configured.
int  Gallery::addSynchronicIfNotAlreadyThere(SynchronicPreparation::Ptr sync)
{
    bool alreadyThere = false; int which = 0;
    for (int i = 0; i < synchronic.size() - 1; i++)
    {
        if (sync->compare(synchronic[i]->sPrep))
        {
            alreadyThere = true;
            which = i;
            break;
        }
    }
    
    if (alreadyThere)   return which;
    else
    {
        addSynchronic(sync);
        return synchronic.size()-1;
    }
    
}

void Gallery::addNostalgic(void)
{
    int numNostalgic = nostalgic.size();
    nostalgic.add(new Nostalgic(main, tuning[0], updateState, numNostalgic));
    nostalgic.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

void Gallery::addNostalgic(NostalgicPreparation::Ptr nost)
{
    int numNostalgic = nostalgic.size();
    nostalgic.add(new Nostalgic(main, nost, numNostalgic));
    nostalgic.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

// Returns index of Nostalgic to be added/configured.
int  Gallery::addNostalgicIfNotAlreadyThere(NostalgicPreparation::Ptr nost)
{
    bool alreadyThere = false; int which = 0;
    for (int i = 0; i < nostalgic.size() - 1; i++)
    {
        if (nost->compare(nostalgic[i]->sPrep))
        {
            alreadyThere = true;
            which = i;
            break;
        }
    }
    
    if (alreadyThere)   return which;
    else
    {
        addNostalgic(nost);
        return nostalgic.size()-1;
    }
    
}

void Gallery::addTuning(void)
{
    int numTuning = tuning.size();
    tuning.add(new Tuning(numTuning, updateState));
    tuning.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

void Gallery::addTuning(TuningPreparation::Ptr tune)
{
    int numTuning = tuning.size();
    tuning.add(new Tuning(tune, numTuning, updateState));
    tuning.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

// Returns index of Tuning to be added/configured.
int  Gallery::addTuningIfNotAlreadyThere(TuningPreparation::Ptr tune)
{
    bool alreadyThere = false; int which = 0;
    for (int i = 0; i < tuning.size() - 1; i++)
    {
        if (tune->compare(tuning[i]->sPrep))
        {
            alreadyThere = true;
            which = i;
            break;
        }
    }
    
    if (alreadyThere)
    {
        return which;
    }
    else
    {
        addTuning(tune);
        return tuning.size()-1;
    }
    
}

void Gallery::addTempo(void)
{
    int numTempo = tempo.size();
    tempo.add(new Tempo(numTempo, updateState));
    tempo.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

void Gallery::addTempo(TempoPreparation::Ptr tmp)
{
    int numTempo = tempo.size();
    tempo.add(new Tempo(tmp, numTempo, updateState));
    tempo.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

// Returns index of Tempo to be added/configured.
int  Gallery::addTempoIfNotAlreadyThere(TempoPreparation::Ptr tmp)
{
    bool alreadyThere = false; int which = 0;
    for (int i = 0; i < tempo.size() - 1; i++)
    {
        if (tmp->compare(tempo[i]->sPrep))
        {
            alreadyThere = true;
            which = i;
            break;
        }
    }
    
    if (alreadyThere)
    {
        return which;
    }
    else
    {
        addTempo(tmp);
        return tempo.size()-1;
    }
    
}


void Gallery::addDirect(void)
{
    int numDirect = direct.size();
    direct.add(new Direct(main, res, hammer, tuning[0], updateState, numDirect));
    direct.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

void Gallery::addDirect(DirectPreparation::Ptr drct)
{
    int numDirect = direct.size();
    direct.add(new Direct(main, res, hammer, drct, numDirect));
    direct.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}
// Returns index of tuning to be added/configured.
int  Gallery::addDirectIfNotAlreadyThere(DirectPreparation::Ptr drct)
{
    bool alreadyThere = false; int which = 0;
    for (int i = 0; i < direct.size() - 1; i++)
    {
        if (drct->compare(direct[i]->sPrep))
        {
            alreadyThere = true;
            which = i;
            break;
        }
    }
    
    if (alreadyThere)   return which;
    else
    {
        addDirect(drct);
        return direct.size()-1;
    }
    
}
