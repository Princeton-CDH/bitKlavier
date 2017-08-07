/*
  ==============================================================================

    GalleryUtilities.cpp
    Created: 22 Mar 2017 3:53:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Gallery.h"

#include "PluginProcessor.h"

#include "BKGraph.h"

void Gallery::addPiano()
{
    int newId = getNewId(PreparationTypePiano);
    bkPianos.add(new Piano(processor, newId));
}

void Gallery::addPianoWithId(int Id)
{
    bkPianos.add(new Piano(processor, Id));
}

void Gallery::removePiano(int Id)
{
    for (int i = bkPianos.size(); --i >= 0; )
    {
        if (bkPianos[i]->getId() == Id) bkPianos.remove(i);
    }
}

void Gallery::removeSynchronicModPreparation(int Id)
{
    for (int i = modSynchronic.size(); --i >= 0; )
    {
        if (modSynchronic[i]->getId() == Id) modSynchronic.remove(i);
    }
}

void Gallery::removeNostalgicModPreparation(int Id)
{
    for (int i = modNostalgic.size(); --i >= 0; )
    {
        if (modNostalgic[i]->getId() == Id) modNostalgic.remove(i);
    }
}

void Gallery::removeDirectModPreparation(int Id)
{
    for (int i = modDirect.size(); --i >= 0; )
    {
        if (modDirect[i]->getId() == Id) modDirect.remove(i);
    }
}

void Gallery::removeTuningModPreparation(int Id)
{
    for (int i = modTuning.size(); --i >= 0; )
    {
        if (modTuning[i]->getId() == Id) modTuning.remove(i);
    }
}

void Gallery::removeTempoModPreparation(int Id)
{
    for (int i = modTempo.size(); --i >= 0; )
    {
        if (modTempo[i]->getId() == Id) modTempo.remove(i);
    }
}

void Gallery::removeSynchronic(int Id)
{
    for (int i = synchronic.size(); --i >= 0; )
    {
        if (synchronic[i]->getId() == Id) synchronic.remove(i);
    }
}

void Gallery::removeNostalgic(int Id) 
{
    for (int i = nostalgic.size(); --i >= 0; )
    {
        if (nostalgic[i]->getId() == Id) nostalgic.remove(i);
    }
}

void Gallery::removeDirect(int Id)
{
    for (int i = direct.size(); --i >= 0; )
    {
        if (direct[i]->getId() == Id) direct.remove(i);
    }
}

void Gallery::removeTuning(int Id) 
{
    for (int i = tuning.size(); --i >= 0; )
    {
        if (tuning[i]->getId() == Id) tuning.remove(i);
    }
}

void Gallery::removeTempo(int Id) 
{
    for (int i = tempo.size(); --i >= 0; )
    {
        if (tempo[i]->getId() == Id) tempo.remove(i);
    }
}

void Gallery::removeKeymap(int Id)
{
    for (int i = bkKeymaps.size(); --i >= 0; )
    {
        if (bkKeymaps[i]->getId() == Id) bkKeymaps.remove(i);
    }
}


void Gallery::remove(BKPreparationType type, int Id)
{
    if (type == PreparationTypeDirect)
    {
        removeDirect(Id);
    }
    else if (type == PreparationTypeSynchronic)
    {
        removeSynchronic(Id);
    }
    else if (type == PreparationTypeNostalgic)
    {
        removeNostalgic(Id);
    }
    else if (type == PreparationTypeTuning)
    {
        removeTuning(Id);
    }
    else if (type == PreparationTypeTempo)
    {
        removeTempo(Id);
    }
    else if (type == PreparationTypeKeymap)
    {
        removeKeymap(Id);
    }
    if (type == PreparationTypeDirectMod)
    {
        removeDirectModPreparation(Id);
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        removeSynchronicModPreparation(Id);
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        removeNostalgicModPreparation(Id);
    }
    else if (type == PreparationTypeTuningMod)
    {
        removeTuningModPreparation(Id);
    }
    else if (type == PreparationTypeTempoMod)
    {
        removeTempoModPreparation(Id);
    }
    else if (type == PreparationTypePiano)
    {
        removePiano(Id);
    }
    
    setGalleryDirty(true);
}


void Gallery::add(BKPreparationType type)
{
    int newId = -1;
    if (type == PreparationTypeDirect)
    {
        addDirect();
        newId = direct.getLast()->getId();
    }
    else if (type == PreparationTypeSynchronic)
    {
        addSynchronic();
        newId = synchronic.getLast()->getId();
    }
    else if (type == PreparationTypeNostalgic)
    {
        addNostalgic();
        newId = nostalgic.getLast()->getId();
    }
    else if (type == PreparationTypeTuning)
    {
        addTuning();
        newId = tuning.getLast()->getId();
    }
    else if (type == PreparationTypeTempo)
    {
        addTempo();
        newId = tempo.getLast()->getId();
    }
    else if (type == PreparationTypeKeymap)
    {
        addKeymap();
        newId = bkKeymaps.getLast()->getId();
    }
    if (type == PreparationTypeDirectMod)
    {
        addDirectMod();
        newId = modDirect.getLast()->getId();
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        addSynchronicMod();
        newId = modSynchronic.getLast()->getId();
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        addNostalgicMod();
        newId = modNostalgic.getLast()->getId();
    }
    else if (type == PreparationTypeTuningMod)
    {
        addTuningMod();
        newId = modTuning.getLast()->getId();
    }
    else if (type == PreparationTypeTempoMod)
    {
        addTempoMod();
        newId = modTempo.getLast()->getId();
    }
    else if (type == PreparationTypePiano)
    {
        addPiano();
        newId = bkPianos.getLast()->getId();
    }
    
    prepareToPlay(bkSampleRate);
    
    setGalleryDirty(true);
}

int Gallery::getNum(BKPreparationType type)
{
    return  (type == PreparationTypeDirect) ? direct.size() :
    (type == PreparationTypeNostalgic) ? nostalgic.size() :
    (type == PreparationTypeSynchronic) ? synchronic.size() :
    (type == PreparationTypeTuning) ? tuning.size() :
    (type == PreparationTypeTempo) ? tempo.size() :
    (type == PreparationTypeDirectMod) ? modDirect.size() :
    (type == PreparationTypeNostalgicMod) ? modNostalgic.size() :
    (type == PreparationTypeSynchronicMod) ? modSynchronic.size() :
    (type == PreparationTypeTuningMod) ? modTuning.size() :
    (type == PreparationTypeTempoMod) ? modTempo.size() :
    (type == PreparationTypeKeymap) ? bkKeymaps.size() :
    -1;
    
}

void Gallery::addDirectMod()
{
    int newId = getNewId(PreparationTypeDirectMod);
    modDirect.add           (new DirectModPreparation(newId));
}

void Gallery::addDirectModWithId(int Id)
{
    modDirect.add           (new DirectModPreparation(Id));
}

void Gallery::addSynchronicMod()
{
    int newId = getNewId(PreparationTypeSynchronicMod);
    modSynchronic.add       (new SynchronicModPreparation(newId));
}

void Gallery::addSynchronicModWithId(int Id)
{
    modSynchronic.add       (new SynchronicModPreparation(Id));
}


void Gallery::addNostalgicMod()
{
    int newId = getNewId(PreparationTypeNostalgicMod);
    modNostalgic.add        (new NostalgicModPreparation(newId));
}

void Gallery::addNostalgicModWithId(int Id)
{
    modNostalgic.add        (new NostalgicModPreparation(Id));
}


void Gallery::addTuningMod()
{
    int newId = getNewId(PreparationTypeTuningMod);
    modTuning.add           (new TuningModPreparation(newId));
}

void Gallery::addTuningMod(TuningModPreparation::Ptr tmod)
{
    int newId = getNewId(PreparationTypeTuningMod);
    tmod->setId(newId);
    modTuning.add           (tmod);
}

void Gallery::addTuningModWithId(int Id)
{
    modTuning.add           (new TuningModPreparation(Id));
}

void Gallery::addTempoMod()
{
    int newId = getNewId(PreparationTypeTempoMod);
    modTempo.add           (new TempoModPreparation(newId));
}

void Gallery::addTempoModWithId(int Id)
{
    modTempo.add           (new TempoModPreparation(Id));
}

void Gallery::addTempoMod(TempoModPreparation::Ptr tmod)
{
    int newId = getNewId(PreparationTypeTempoMod);
    tmod->setId(newId);
    modTempo.add           (tmod);
}

void Gallery::addKeymap(void)
{
    int newId = getNewId(PreparationTypeKeymap);
    bkKeymaps.add(new Keymap(newId));
}

void Gallery::addKeymapWithId(int Id)
{
    bkKeymaps.add(new Keymap(Id));
}

void Gallery::addKeymap(Keymap::Ptr k)
{
    int newId = getNewId(PreparationTypeKeymap);
    k->setId(newId);
    bkKeymaps.add(k);
}

void Gallery::addSynchronic(void)
{
    int newId = getNewId(PreparationTypeSynchronic);
    synchronic.add(new Synchronic(&processor.mainPianoSynth, tuning[0], tempo[0], general, processor.updateState, newId));
}

void Gallery::addSynchronicWithId(int Id)
{
    synchronic.add(new Synchronic(&processor.mainPianoSynth, tuning[0], tempo[0], general, processor.updateState, Id));
}

void Gallery::addSynchronic(int Id, int tuningId, int tempoId)
{
    synchronic.add(new Synchronic(&processor.mainPianoSynth, getTuning(tuningId), getTempo(tempoId), general, processor.updateState, Id));
}

void Gallery::copy(BKPreparationType type, int from, int to)
{
    if (type == PreparationTypeDirect)
    {
        direct.getUnchecked(to)->copy(direct.getUnchecked(from));
    }
    else if (type == PreparationTypeSynchronic)
    {
        synchronic.getUnchecked(to)->copy(synchronic.getUnchecked(from));
    }
    else if (type == PreparationTypeNostalgic)
    {
        nostalgic.getUnchecked(to)->copy(nostalgic.getUnchecked(from));
    }
    else if (type == PreparationTypeTuning)
    {
        tuning.getUnchecked(to)->copy(tuning.getUnchecked(from));
    }
    else if (type == PreparationTypeTempo)
    {
        tempo.getUnchecked(to)->copy(tempo.getUnchecked(from));
    }
    else if (type == PreparationTypeKeymap)
    {
        bkKeymaps.getUnchecked(to)->copy(bkKeymaps.getUnchecked(from));
    }
    else if (type == PreparationTypeDirectMod)
    {
        (modDirect.getUnchecked(to))->copy(modDirect.getUnchecked(from));
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        modSynchronic.getUnchecked(to)->copy(modSynchronic.getUnchecked(from));
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        modNostalgic.getUnchecked(to)->copy(modNostalgic.getUnchecked(from));
    }
    else if (type == PreparationTypeTuningMod)
    {
        modTuning.getUnchecked(to)->copy(modTuning.getUnchecked(from));
    }
    else if (type == PreparationTypeTempoMod)
    {
        modTempo.getUnchecked(to)->copy(modTempo.getUnchecked(from));
    }
}

void Gallery::addTypeWithId(BKPreparationType type, int Id)
{
    if (type == PreparationTypeDirect)
    {
        addDirectWithId(Id);
    }
    else if (type == PreparationTypeSynchronic)
    {
        addSynchronicWithId(Id);
    }
    else if (type == PreparationTypeNostalgic)
    {
        addNostalgicWithId(Id);
    }
    else if (type == PreparationTypeTuning)
    {
        addTuningWithId(Id);
    }
    else if (type == PreparationTypeTempo)
    {
        addTempoWithId(Id);
    }
    else if (type == PreparationTypeKeymap)
    {
        addKeymapWithId(Id);
    }
    else if (type == PreparationTypeDirectMod)
    {
        addDirectModWithId(Id);
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        addSynchronicModWithId(Id);
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        addNostalgicModWithId(Id);
    }
    else if (type == PreparationTypeTuningMod)
    {
        addTuningModWithId(Id);
    }
    else if (type == PreparationTypeTempoMod)
    {
        addTempoModWithId(Id);
    }
    else if (type == PreparationTypePiano)
    {
        addPianoWithId(Id);
    }
    
    prepareToPlay(bkSampleRate);
    
    setGalleryDirty(true);
}


void Gallery::addSynchronic(SynchronicPreparation::Ptr sync)
{
    int newId = getNewId(PreparationTypeSynchronic);
    synchronic.add(new Synchronic(&processor.mainPianoSynth, sync, tuning.getFirst(), tempo.getFirst(), general, newId));
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
        return synchronic.getLast()->getId();
    }
}

void Gallery::addNostalgic(void)
{
    int newId = getNewId(PreparationTypeNostalgic);
    nostalgic.add(new Nostalgic(&processor.mainPianoSynth, tuning.getFirst(), synchronic.getFirst(), processor.updateState, newId));
}

void Gallery::addNostalgicWithId(int Id)
{
    nostalgic.add(new Nostalgic(&processor.mainPianoSynth, tuning.getFirst(), synchronic.getFirst(), processor.updateState, Id));
}

void Gallery::addNostalgic(int Id, int tuningId, int synchronicId)
{
    nostalgic.add(new Nostalgic(&processor.mainPianoSynth, getTuning(tuningId), getSynchronic(synchronicId), processor.updateState, Id));
}

void Gallery::addNostalgic(NostalgicPreparation::Ptr nost)
{
    int newId = getNewId(PreparationTypeNostalgic);
    nostalgic.add(new Nostalgic(&processor.mainPianoSynth, nost, tuning.getFirst(), synchronic.getFirst(), newId));
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
    int newId = getNewId(PreparationTypeTuning);
    tuning.add(new Tuning(newId, processor.updateState));
}

void Gallery::addTuningWithId(int Id)
{
    tuning.add(new Tuning(Id, processor.updateState));
}

void Gallery::addTuning(TuningPreparation::Ptr tune)
{
    int newId = getNewId(PreparationTypeTuning);
    tuning.add(new Tuning(tune, newId, processor.updateState));
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
        return tuning.getLast()->getId();
    }
}

void Gallery::addTempo(void)
{
    int newId = getNewId(PreparationTypeTempo);
    tempo.add(new Tempo(newId, processor.updateState));
}

void Gallery::addTempoWithId(int Id)
{
    tempo.add(new Tempo(Id, processor.updateState));
}

void Gallery::addTempo(TempoPreparation::Ptr tmp)
{
    int newId = getNewId(PreparationTypeTempo);
    tempo.add(new Tempo(tmp, newId, processor.updateState));
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
        return tempo.getLast()->getId();
    }
}


void Gallery::addDirect(void)
{
    int newId = getNewId(PreparationTypeDirect);
    direct.add(new Direct(processor.updateState, newId));
}

void Gallery::addDirectWithId(int Id)
{
    direct.add(new Direct(processor.updateState, Id));
}

void Gallery::addDirect(int Id, int tuningId)
{
    direct.add(new Direct(processor.updateState, Id));
}

void Gallery::addDirect(DirectPreparation::Ptr drct)
{
    int newId = getNewId(PreparationTypeDirect);
    direct.add(new Direct(drct, newId));
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
        return direct.getLast()->getId();
    }
}

// Deletes unused preparations
void Gallery::clean(void)
{
    for (int i = 0; i < BKPreparationTypeNil; i++) used.set(i, Array<int>({-1}));
    
    for (auto piano : bkPianos)
    {
        for (auto item : piano->getItems())
        {
            Array<int> thisUsed = used.getUnchecked(item->getType());
            
            thisUsed.add(item->getId());
            
            used.set(item->getType(), thisUsed);
        }
    }
    
    
    Array<int> thisUsed = used.getUnchecked(PreparationTypeDirect);
    for (int i = direct.size(); --i >= 0;)
    {
        if (!thisUsed.contains(direct[i]->getId()))
        {
            direct.remove(i);
        }
    }
    
    thisUsed = used.getUnchecked(PreparationTypeSynchronic);
    for (int i = synchronic.size(); --i >= 0;)
    {
        if (!thisUsed.contains(synchronic[i]->getId())) synchronic.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeNostalgic);
    for (int i = nostalgic.size(); --i >= 0;)
    {
        if (!thisUsed.contains(nostalgic[i]->getId())) nostalgic.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeTempo);
    for (int i = tempo.size(); --i >= 0;)
    {
        if (!thisUsed.contains(tempo[i]->getId())) tempo.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeTuning);
    for (int i = tuning.size(); --i >= 0;)
    {
        if (!thisUsed.contains(tuning[i]->getId())) tuning.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeKeymap);
    for (int i = bkKeymaps.size(); --i >= 0;)
    {
        if (!thisUsed.contains(bkKeymaps[i]->getId())) bkKeymaps.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeDirect);
    for (int i = modDirect.size(); --i >= 0;)
    {
        if (!thisUsed.contains(modDirect[i]->getId())) modDirect.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeSynchronicMod);
    for (int i = modSynchronic.size(); --i >= 0;)
    {
        if (!thisUsed.contains(modSynchronic[i]->getId())) modSynchronic.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeNostalgicMod);
    for (int i = modNostalgic.size(); --i >= 0;)
    {
        if (!thisUsed.contains(modNostalgic[i]->getId())) modNostalgic.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeTempoMod);
    for (int i = modTempo.size(); --i >= 0;)
    {
        if (!thisUsed.contains(modTempo[i]->getId())) modTempo.remove(i);
    }
    
    thisUsed = used.getUnchecked(PreparationTypeTuningMod);
    for (int i = modTuning.size(); --i >= 0;)
    {
        if (!thisUsed.contains(modTuning[i]->getId())) modTuning.remove(i);
    }
    
    setGalleryDirty(true);
}
