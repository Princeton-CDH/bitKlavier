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
            DBG("got synchronic processor id " + String(id));
            return synchronic[i]->processor;
        }
    }
    
    //else
    DBG("synchronic processor not found, returning first processor");
    return synchronic[0]->processor;
}

void Gallery::addPiano()
{
    int numPianos = bkPianos.size();
    bkPianos.add(new Piano(synchronic, nostalgic, direct, tuning, tempo,
                           modSynchronic, modNostalgic, modDirect, modTuning, modTempo,
                           bkKeymaps, numPianos));
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

void Gallery::addDirectMod()
{
    modDirect.add           (new DirectModPreparation());
}

void Gallery::addSynchronicMod()
{
    modSynchronic.add       (new SynchronicModPreparation());
}

void Gallery::addNostalgicMod()
{
    modNostalgic.add        (new NostalgicModPreparation());
}

void Gallery::addTuningMod()
{
    modTuning.add           (new TuningModPreparation());
}

void Gallery::addTuningMod(TuningModPreparation::Ptr tmod)
{
    modTuning.add           (tmod);
}

void Gallery::addTempoMod()
{
    modTempo.add           (new TempoModPreparation());
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
    tuning.add(new Tuning(numTuning));
    tuning.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

void Gallery::addTuning(TuningPreparation::Ptr tune)
{
    int numTuning = tuning.size();
    tuning.add(new Tuning(tune, numTuning));
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
    tempo.add(new Tempo(numTempo));
    tempo.getLast()->processor->setCurrentPlaybackSampleRate(bkSampleRate);
}

void Gallery::addTempo(TempoPreparation::Ptr tmp)
{
    int numTempo = tempo.size();
    tempo.add(new Tempo(tmp, numTempo));
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
