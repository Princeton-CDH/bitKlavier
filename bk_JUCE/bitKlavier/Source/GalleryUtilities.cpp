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

void Gallery::addPiano(XmlElement* xml, OwnedArray<HashMap<int,int>>* map)
{
    DBG(xml->toString(XmlElement::TextFormat()));
    addPiano();
    Piano::Ptr piano = bkPianos.getLast();
    
    int Id = piano->getId();
    
    piano->setState(xml, map, idcounts);
    
    piano->setId(Id);
    
    piano->configure();
}

void Gallery::addPiano()
{
    int newId = getNewId(PreparationTypePiano);
    bkPianos.add(new Piano(processor, newId));
}

void Gallery::addPiano(Piano::Ptr thisPiano)
{
    int newId = getNewId(PreparationTypePiano);
    thisPiano->setId(newId);
    bkPianos.add(thisPiano);
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

void Gallery::removeSynchronicModification(int Id)
{
    for (int i = modSynchronic.size(); --i >= 0; )
    {
        if (modSynchronic[i]->getId() == Id) modSynchronic.remove(i);
    }
}

void Gallery::removeNostalgicModification(int Id)
{
    for (int i = modNostalgic.size(); --i >= 0; )
    {
        if (modNostalgic[i]->getId() == Id) modNostalgic.remove(i);
    }
}

void Gallery::removeResonanceModification(int Id)
{
    for (int i = modResonance.size(); --i >= 0; )
    {
        if (modResonance[i]->getId() == Id) modResonance.remove(i);
    }
}

void Gallery::removeDirectModification(int Id)
{
    for (int i = modDirect.size(); --i >= 0; )
    {
        if (modDirect[i]->getId() == Id) modDirect.remove(i);
    }
}

void Gallery::removeTuningModification(int Id)
{
    for (int i = modTuning.size(); --i >= 0; )
    {
        if (modTuning[i]->getId() == Id) modTuning.remove(i);
    }
}

void Gallery::removeTempoModification(int Id)
{
    for (int i = modTempo.size(); --i >= 0; )
    {
        if (modTempo[i]->getId() == Id) modTempo.remove(i);
    }
}

void Gallery::removeBlendronicModification(int Id)
{
	for (int i = modBlendronic.size(); --i >= 0; )
	{
		if (modBlendronic[i]->getId() == Id) modBlendronic.remove(i);
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

void Gallery::removeBlendronic(int Id)
{
	for (int i = blendronic.size(); --i >= 0; )
	{
		if (blendronic[i]->getId() == Id) blendronic.remove(i);
	}
}

void Gallery::removeResonance(int Id)
{
    for (int i = resonance.size(); --i >= 0;)
    {
        if (resonance[i]->getId() == Id) resonance.remove(i);
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
	else if (type == PreparationTypeBlendronic)
	{
		removeBlendronic(Id);
	}
    else if (type == PreparationTypeResonance)
    {
        removeResonance(Id);
    }
    else if (type == PreparationTypeKeymap)
    {
        removeKeymap(Id);
    }
    if (type == PreparationTypeDirectMod)
    {
        removeDirectModification(Id);
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        removeSynchronicModification(Id);
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        removeNostalgicModification(Id);
    }
    else if (type == PreparationTypeTuningMod)
    {
        removeTuningModification(Id);
    }
    else if (type == PreparationTypeTempoMod)
    {
        removeTempoModification(Id);
    }
	else if (type == PreparationTypeBlendronicMod)
	{
		removeBlendronicModification(Id);
	}
    else if (type == PreparationTypeResonanceMod)
    {
        removeResonanceModification(Id);
    }
    else if (type == PreparationTypePiano)
    {
        removePiano(Id);
    }
    
    setGalleryDirty(true);
}


String Gallery::iterateName(BKPreparationType type, String name)
{
    int num = 0;
    String original = name;
    
    name = name.upToFirstOccurrenceOf(" (", false, false);
    
    if (type == PreparationTypeDirect)
    {
        for (auto p : direct)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypeSynchronic)
    {
        for (auto p : synchronic)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypeNostalgic)
    {
        for (auto p : nostalgic)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypeTuning)
    {
        for (auto p : tuning)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypeTempo)
    {
        for (auto p : tempo)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
	else if (type == PreparationTypeBlendronic)
	{
		for (auto p : blendronic)   if (p->getName() == name || p->getName().startsWith(name + " (")) num++;
	}
    else if (type == PreparationTypeKeymap)
    {
        for (auto p : bkKeymaps)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    if (type == PreparationTypeDirectMod)
    {
        for (auto p : modDirect)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        for (auto p : modSynchronic)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        for (auto p : modNostalgic)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypeTuningMod)
    {
        for (auto p : modTuning)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypeTempoMod)
    {
        for (auto p : modTempo)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
	else if (type == PreparationTypeBlendronicMod)
	{
		for (auto p : modBlendronic)   if (p->getName() == name || p->getName().startsWith(name + " (")) num++;
	}
    else if (type == PreparationTypeResonance)
    {
        for (auto p : resonance) if (p->getName() == name || p->getName().startsWith(name + " (")) num++;
    }
    else if (type == PreparationTypeResonanceMod)
    {
        for (auto p : modResonance)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    else if (type == PreparationTypePiano)
    {
        for (auto p : bkPianos)   if (p->getName() == name || p->getName().startsWith(name+" (")) num++;
    }
    
    if (num > 1)    name += " (" + String(num) + ")";
    else            name = original;
    
    return name;
}

int Gallery::numWithSameNameAs(BKPreparationType type, int Id)
{
    int num = 0;
    String name;
    if (type == PreparationTypeDirect)
    {
        name = getDirect(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : direct)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    else if (type == PreparationTypeSynchronic)
    {
         name = getSynchronic(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : synchronic)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    else if (type == PreparationTypeNostalgic)
    {
         name = getNostalgic(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : nostalgic)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    else if (type == PreparationTypeTuning)
    {
         name = getTuning(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : tuning)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    else if (type == PreparationTypeTempo)
    {
         name = getTempo(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : tempo)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
	else if (type == PreparationTypeBlendronic)
	{
		name = getBlendronic(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
		for (auto p : blendronic)   if (p->getName() == name || p->getName().startsWith(name + " ")) num++;
	}
    else if (type == PreparationTypeKeymap)
    {
        name = getKeymap(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : bkKeymaps)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    if (type == PreparationTypeDirectMod)
    {
         name = getDirectModification(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : modDirect)
        {
            if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
        }
    }
    else if (type == PreparationTypeSynchronicMod)
    {
         name = getSynchronicModification(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : modSynchronic)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    else if (type == PreparationTypeNostalgicMod)
    {
         name = getNostalgicModification(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : modNostalgic)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    else if (type == PreparationTypeTuningMod)
    {
         name = getTuningModification(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : modTuning)     if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    else if (type == PreparationTypeTempoMod)
    {
         name = getTempoModification(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : modTempo)    if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
	else if (type == PreparationTypeBlendronicMod)
	{
		name = getBlendronicModification(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
		for (auto p : modBlendronic)   if (p->getName() == name || p->getName().startsWith(name + " ")) num++;
	}
    else if (type == PreparationTypeResonance)
    {
        name = getResonance(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : resonance)   if (p->getName() == name || p->getName().startsWith(name + " ")) num++;
    }
    else if (type == PreparationTypeResonanceMod)
    {
        name = getResonanceModification(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : modResonance)   if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    else if (type == PreparationTypePiano)
    {
         name = getPiano(Id)->getName().upToFirstOccurrenceOf(" (", false, false);
        for (auto p : bkPianos)    if (p->getName() == name || p->getName().startsWith(name+" ")) num++;
    }
    
    
    return num;
}

int Gallery::addCopy(BKPreparationType type, XmlElement* xml, int oldId)
{
    if (type == PreparationTypeDirect)
    {
        Direct::Ptr p = new Direct(-1);
        p->setState(xml);
        addDirect(p);
        if (p->getName() == "Direct " + String(oldId))
            p->setName("Direct " + String(p->getId()));
        else p->setName(iterateName(PreparationTypeDirect, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeSynchronic)
    {
        Synchronic::Ptr p = new Synchronic(-1);
        p->setState(xml);
        addSynchronic(p);
        if (p->getName() == "Synchronic " + String(oldId))
            p->setName("Synchronic " + String(p->getId()));
        else p->setName(iterateName(PreparationTypeSynchronic, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr p = new Nostalgic(-1);
        p->setState(xml);
        addNostalgic(p);
        if (p->getName() == "Nostalgic " + String(oldId))
            p->setName("Nostalgic " + String(p->getId()));
        else p->setName(iterateName(PreparationTypeNostalgic, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeTuning)
    {
        Tuning::Ptr p = new Tuning(-1);
        p->setState(xml);
        addTuning(p);
        if (p->getName() == "Tuning " + String(oldId))
            p->setName("Tuning " + String(p->getId()));
        else p->setName(iterateName(PreparationTypeTuning, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeTempo)
    {
        Tempo::Ptr p = new Tempo(-1);
        p->setState(xml);
        addTempo(p);
        if (p->getName() == "Tempo " + String(oldId))
            p->setName("Tempo " + String(p->getId()));
        else p->setName(iterateName(PreparationTypeTempo, p->getName()));
        return p->getId();
    }
	else if (type == PreparationTypeBlendronic)
	{
		Blendronic::Ptr p = new Blendronic(-1);
		p->setState(xml);
		addBlendronic(p);
        if (p->getName() == "Blendronic " + String(oldId))
            p->setName("Blendronic " + String(p->getId()));
        else p->setName(iterateName(PreparationTypeBlendronic, p->getName()));
        return p->getId();
	}
    else if (type == PreparationTypeResonance)
    {
        Resonance::Ptr p = new Resonance(-1);
        p->setState(xml);
        addResonance(p);
        if (p->getName() == "Resonance " + String(oldId))
            p->setName("Resonance " + String(p->getId()));
        else p->setName(iterateName(PreparationTypeResonance, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeKeymap)
    {
        Keymap::Ptr p = new Keymap(processor, -1);
        p->setState(xml);
        addKeymap(p);
        if (p->getName() == "Keymap " + String(oldId))
            p->setName("Keymap " + String(p->getId()));
        else p->setName(iterateName(PreparationTypeKeymap, p->getName()));
        return p->getId();
    }
    if (type == PreparationTypeDirectMod)
    {
        DirectModification::Ptr p = new DirectModification(processor, -1);
        p->setState(xml);
        addDirectMod(p);
        if (p->getName() == String(oldId))
            p->setName(String(p->getId()));
        else p->setName(iterateName(PreparationTypeDirectMod, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        SynchronicModification::Ptr p = new SynchronicModification(processor, -1);
        p->setState(xml);
        addSynchronicMod(p);
        if (p->getName() == String(oldId))
            p->setName(String(p->getId()));
        else p->setName(iterateName(PreparationTypeSynchronicMod, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        NostalgicModification::Ptr p = new NostalgicModification(processor, -1);
        p->setState(xml);
        addNostalgicMod(p);
        if (p->getName() == String(oldId))
            p->setName(String(p->getId()));
        else p->setName(iterateName(PreparationTypeNostalgicMod, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeResonanceMod)
    {
        ResonanceModification::Ptr p = new ResonanceModification(processor, -1);
        p->setState(xml);
        addResonanceMod(p);
        if (p->getName() == String(oldId))
            p->setName(String(p->getId()));
        else p->setName(iterateName(PreparationTypeResonanceMod, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeTuningMod)
    {
        TuningModification::Ptr p = new TuningModification(processor, -1);
        p->setState(xml);
        addTuningMod(p);
        if (p->getName() == String(oldId))
            p->setName(String(p->getId()));
        else p->setName(iterateName(PreparationTypeTuningMod, p->getName()));
        return p->getId();
    }
    else if (type == PreparationTypeTempoMod)
    {
        TempoModification::Ptr p = new TempoModification(processor, -1);
        p->setState(xml);
        addTempoMod(p);
        if (p->getName() == String(oldId))
            p->setName(String(p->getId()));
        else p->setName(iterateName(PreparationTypeTempoMod, p->getName()));
        return p->getId();
    }
	else if (type == PreparationTypeBlendronicMod)
	{
		BlendronicModification::Ptr p = new BlendronicModification(processor, -1);
		p->setState(xml);
		addBlendronicMod(p);
        if (p->getName() == String(oldId))
            p->setName(String(p->getId()));
        else p->setName(iterateName(PreparationTypeBlendronicMod, p->getName()));
        return p->getId();
	}
    else if (type == PreparationTypePiano)
    {
        Piano::Ptr p = new Piano(processor, -1);
        p->setState(xml, &idmap, idcounts);
        if (p->getName() == String(oldId))
            p->setName(String(p->getId()));
        else p->setName(iterateName(PreparationTypePiano, p->getName()));
        return p->getId();
    }
    return -1;
}


// Adds to end of preparation list
int Gallery::duplicate(BKPreparationType type, int Id)
{
    int newId = -1;
    if (type == PreparationTypeDirect)
    {
        Direct::Ptr toCopy = getDirect(Id);
        Direct::Ptr newOne = toCopy->duplicate();
        addDirect(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeDirect, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeSynchronic)
    {
        Synchronic::Ptr toCopy = getSynchronic(Id);
        Synchronic::Ptr newOne = toCopy->duplicate();
        addSynchronic(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeSynchronic, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr toCopy = getNostalgic(Id);
        Nostalgic::Ptr newOne = toCopy->duplicate();
        addNostalgic(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeNostalgic, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeTuning)
    {
        Tuning::Ptr toCopy = getTuning(Id);
        Tuning::Ptr newOne = toCopy->duplicate();
        addTuning(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeTuning, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeTempo)
    {
        Tempo::Ptr toCopy = getTempo(Id);
        Tempo::Ptr newOne = toCopy->duplicate();
        addTempo(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeTempo, newId))+")";
        newOne->setName(newName);
    }
	else if (type == PreparationTypeBlendronic)
	{
		Blendronic::Ptr toCopy = getBlendronic(Id);
		Blendronic::Ptr newOne = toCopy->duplicate();
		addBlendronic(newOne);
		newId = newOne->getId();
		String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " (" + String(numWithSameNameAs(PreparationTypeBlendronic, newId)) + ")";
		newOne->setName(newName);
	}
    else if (type == PreparationTypeResonance)
    {
        Resonance::Ptr toCopy = getResonance(Id);
        Resonance::Ptr newOne = toCopy->duplicate();
        addResonance(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " (" + String(numWithSameNameAs(PreparationTypeResonance, newId)) + ")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeKeymap)
    {
        Keymap::Ptr toCopy = getKeymap(Id);
        Keymap::Ptr newOne = toCopy->duplicate();
        addKeymap(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeKeymap, newId))+")";
        newOne->setName(newName);
    }
    if (type == PreparationTypeDirectMod)
    {
        DirectModification::Ptr toCopy = getDirectModification(Id);
        DirectModification::Ptr newOne = toCopy->duplicate();
        addDirectMod(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeDirectMod, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        SynchronicModification::Ptr toCopy = getSynchronicModification(Id);
        SynchronicModification::Ptr newOne = toCopy->duplicate();
        addSynchronicMod(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeSynchronicMod, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        NostalgicModification::Ptr toCopy = getNostalgicModification(Id);
        NostalgicModification::Ptr newOne = toCopy->duplicate();
        addNostalgicMod(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeNostalgicMod, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeResonanceMod)
    {
        ResonanceModification::Ptr toCopy = getResonanceModification(Id);
        ResonanceModification::Ptr newOne = toCopy->duplicate();
        addResonanceMod(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeResonanceMod, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeTuningMod)
    {
        TuningModification::Ptr toCopy = getTuningModification(Id);
        TuningModification::Ptr newOne = toCopy->duplicate();
        addTuningMod(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeTuningMod, newId))+")";
        newOne->setName(newName);
    }
    else if (type == PreparationTypeTempoMod)
    {
        TempoModification::Ptr toCopy = getTempoModification(Id);
        TempoModification::Ptr newOne = toCopy->duplicate();
        addTempoMod(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypeTempoMod, newId))+")";
        newOne->setName(newName);
    }
	else if (type == PreparationTypeBlendronicMod)
	{
		BlendronicModification::Ptr toCopy = getBlendronicModification(Id);
		BlendronicModification::Ptr newOne = toCopy->duplicate();
		addBlendronicMod(newOne);
		newId = newOne->getId();
		String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " (" + String(numWithSameNameAs(PreparationTypeBlendronicMod, newId)) + ")";
		newOne->setName(newName);
	}
    else if (type == PreparationTypePiano)
    {
        Piano::Ptr toCopy = getPiano(Id);
        Piano::Ptr newOne = toCopy->duplicate();
        addPiano(newOne);
        newId = newOne->getId();
        String newName = toCopy->getName().upToFirstOccurrenceOf(" (", false, false) + " ("+String(numWithSameNameAs(PreparationTypePiano, newId))+")";
        newOne->setName(newName);
    }
    
    prepareToPlay(processor.getCurrentSampleRate());
    
    setGalleryDirty(true);
    
    return newId;
}

// Adds to end of preparation list
int Gallery::add(BKPreparationType type)
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
	else if (type == PreparationTypeBlendronic)
	{
		addBlendronic();
		newId = blendronic.getLast()->getId();
	}
    else if (type == PreparationTypeResonance)
    {
        addResonance();
        newId = resonance.getLast()->getId();
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
    else if (type == PreparationTypeResonanceMod)
    {
        addResonanceMod();
        newId = modResonance.getLast()->getId();
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
	else if (type == PreparationTypeBlendronicMod)
	{
		addBlendronicMod();
		newId = tempo.getLast()->getId();
	}
    else if (type == PreparationTypePiano)
    {
        addPiano();
        newId = bkPianos.getLast()->getId();
    }
    
    prepareToPlay(processor.getCurrentSampleRate());
    
    setGalleryDirty(true);
    
    return newId;
}

int Gallery::getNum(BKPreparationType type)
{
    return  (type == PreparationTypeDirect) ? direct.size() :
    (type == PreparationTypeNostalgic) ? nostalgic.size() :
    (type == PreparationTypeSynchronic) ? synchronic.size() :
    (type == PreparationTypeTuning) ? tuning.size() :
    (type == PreparationTypeTempo) ? tempo.size() :
	(type == PreparationTypeBlendronic) ? blendronic.size() :
    (type == PreparationTypeResonance) ? resonance.size() :
    (type == PreparationTypeDirectMod) ? modDirect.size() :
    (type == PreparationTypeNostalgicMod) ? modNostalgic.size() :
    (type == PreparationTypeResonanceMod) ? modResonance.size() :
    (type == PreparationTypeSynchronicMod) ? modSynchronic.size() :
    (type == PreparationTypeTuningMod) ? modTuning.size() :
    (type == PreparationTypeTempoMod) ? modTempo.size() :
	(type == PreparationTypeBlendronicMod) ? modBlendronic.size() :
    (type == PreparationTypeKeymap) ? bkKeymaps.size() :
    -1;
    
}

void Gallery::addDirectMod()
{
    int newId = getNewId(PreparationTypeDirectMod);
    modDirect.add           (new DirectModification(processor, newId));
}

void Gallery::addDirectModWithId(int Id)
{
    modDirect.add           (new DirectModification(processor, Id));
}

void Gallery::addDirectMod(DirectModification::Ptr mod)
{
    int newId = getNewId(PreparationTypeDirectMod);
    mod->setId(newId);
    modDirect.add           (mod);
}

void Gallery::addSynchronicMod()
{
    int newId = getNewId(PreparationTypeSynchronicMod);
    modSynchronic.add       (new SynchronicModification(processor, newId));
}

void Gallery::addSynchronicModWithId(int Id)
{
    modSynchronic.add       (new SynchronicModification(processor, Id));
}

void Gallery::addSynchronicMod(SynchronicModification::Ptr mod)
{
    int newId = getNewId(PreparationTypeSynchronicMod);
    mod->setId(newId);
    modSynchronic.add           (mod);
}

void Gallery::addNostalgicMod()
{
    int newId = getNewId(PreparationTypeNostalgicMod);
    modNostalgic.add        (new NostalgicModification(processor, newId));
}

void Gallery::addNostalgicModWithId(int Id)
{
    modNostalgic.add        (new NostalgicModification(processor, Id));
}

void Gallery::addNostalgicMod(NostalgicModification::Ptr mod)
{
    int newId = getNewId(PreparationTypeNostalgicMod);
    mod->setId(newId);
    modNostalgic.add           (mod);
}

void Gallery::addResonanceMod()
{
    int newId = getNewId(PreparationTypeResonanceMod);
    modResonance.add(new ResonanceModification(processor, newId));
}

void Gallery::addResonanceModWithId(int Id)
{
    modResonance.add(new ResonanceModification(processor, Id));
}

void Gallery::addResonanceMod(ResonanceModification::Ptr mod)
{
    int newId = getNewId(PreparationTypeResonanceMod);
    mod->setId(newId);
    modResonance.add(mod);
}

void Gallery::addTuningMod()
{
    int newId = getNewId(PreparationTypeTuningMod);
    modTuning.add(new TuningModification(processor, newId));
}

void Gallery::addTuningMod(TuningModification::Ptr tmod)
{
    int newId = getNewId(PreparationTypeTuningMod);
    tmod->setId(newId);
    modTuning.add(tmod);
}

void Gallery::addTuningModWithId(int Id)
{
    modTuning.add(new TuningModification(processor, Id));
}

void Gallery::addTempoMod()
{
    int newId = getNewId(PreparationTypeTempoMod);
    modTempo.add(new TempoModification(processor, newId));
}

void Gallery::addTempoModWithId(int Id)
{
    modTempo.add(new TempoModification(processor, Id));
}

void Gallery::addTempoMod(TempoModification::Ptr tmod)
{
    int newId = getNewId(PreparationTypeTempoMod);
    tmod->setId(newId);
    modTempo.add(tmod);
}

void Gallery::addBlendronicMod()
{
	int newId = getNewId(PreparationTypeBlendronicMod);
	modBlendronic.add(new BlendronicModification(processor, newId));
}

void Gallery::addBlendronicModWithId(int Id)
{
	modBlendronic.add(new BlendronicModification(processor, Id));
}

void Gallery::addBlendronicMod(BlendronicModification::Ptr bmod)
{
	int newId = getNewId(PreparationTypeBlendronicMod);
	bmod->setId(newId);
	modBlendronic.add(bmod);
}

void Gallery::addKeymap(void)
{
    int newId = getNewId(PreparationTypeKeymap);
    bkKeymaps.add(new Keymap(processor, newId));
}

void Gallery::addKeymapWithId(int Id)
{
    bkKeymaps.add(new Keymap(processor, Id));
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
    synchronic.add(new Synchronic(newId));
}

void Gallery::addSynchronic(Synchronic::Ptr p)
{
    int newId = getNewId(PreparationTypeSynchronic);
    p->setId(newId);
    synchronic.add(p);
}

void Gallery::addSynchronicWithId(int Id)
{
    synchronic.add(new Synchronic(Id));
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
	else if (type == PreparationTypeBlendronic)
	{
		blendronic.getUnchecked(to)->copy(blendronic.getUnchecked(from));
	}
    else if (type == PreparationTypeResonance)
    {
        resonance.getUnchecked(to)->copy(resonance.getUnchecked(from));
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
    else if (type == PreparationTypeResonanceMod)
    {
        modResonance.getUnchecked(to)->copy(modResonance.getUnchecked(from));
    }
    else if (type == PreparationTypeTuningMod)
    {
        modTuning.getUnchecked(to)->copy(modTuning.getUnchecked(from));
    }
    else if (type == PreparationTypeTempoMod)
    {
        modTempo.getUnchecked(to)->copy(modTempo.getUnchecked(from));
    }
	else if (type == PreparationTypeBlendronicMod)
	{
		modBlendronic.getUnchecked(to)->copy(modBlendronic.getUnchecked(from));
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
	else if (type == PreparationTypeBlendronic)
	{
		addBlendronicWithId(Id);
	}
    else if (type == PreparationTypeResonance)
    {
        addResonanceWithId(Id);
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
    else if (type == PreparationTypeResonanceMod)
    {
        addResonanceModWithId(Id);
    }
    else if (type == PreparationTypeTuningMod)
    {
        addTuningModWithId(Id);
    }
    else if (type == PreparationTypeTempoMod)
    {
        addTempoModWithId(Id);
    }
	else if (type == PreparationTypeBlendronicMod)
	{
		addBlendronicModWithId(Id);
	}
    else if (type == PreparationTypePiano)
    {
        addPianoWithId(Id);
    }
    
    prepareToPlay(processor.getCurrentSampleRate());
    
    setGalleryDirty(true);
}


void Gallery::addSynchronic(SynchronicPreparation::Ptr sync)
{
    int newId = getNewId(PreparationTypeSynchronic);
    synchronic.add(new Synchronic(sync, newId));
}


void Gallery::addNostalgic(void)
{
    int newId = getNewId(PreparationTypeNostalgic);
    nostalgic.add(new Nostalgic(newId));
}

void Gallery::addNostalgic(Nostalgic::Ptr p)
{
    int newId = getNewId(PreparationTypeNostalgic);
    p->setId(newId);
    nostalgic.add(p);
}

void Gallery::addNostalgicWithId(int Id)
{
    nostalgic.add(new Nostalgic(Id));
}

void Gallery::addNostalgic(NostalgicPreparation::Ptr nost)
{
    int newId = getNewId(PreparationTypeNostalgic);
    nostalgic.add(new Nostalgic(nost, newId));
}

void Gallery::addTuning(void)
{
    int newId = getNewId(PreparationTypeTuning);
    tuning.add(new Tuning(newId));
}

void Gallery::addTuningWithId(int Id)
{
    tuning.add(new Tuning(Id));
}

void Gallery::addTuning(TuningPreparation::Ptr tune)
{
    int newId = getNewId(PreparationTypeTuning);
    tuning.add(new Tuning(tune, newId));
}

void Gallery::addTuning(Tuning::Ptr p)
{
    int newId = getNewId(PreparationTypeTuning);
    p->setId(newId);
    tuning.add(p);
}

void Gallery::addBlendronic(void)
{
	int newId = getNewId(PreparationTypeBlendronic);
	blendronic.add(new Blendronic(newId));
}

void Gallery::addBlendronicWithId(int Id)
{
	blendronic.add(new Blendronic(Id));
}

void Gallery::addBlendronic(BlendronicPreparation::Ptr blend)
{
	int newId = getNewId(PreparationTypeTuning);
	blendronic.add(new Blendronic(blend, newId));
}

void Gallery::addBlendronic(Blendronic::Ptr p)
{
    int newId = getNewId(PreparationTypeBlendronic);
    p->setId(newId);
    blendronic.add(p);
}

void Gallery::addResonanceWithId(int Id)
{
    resonance.add(new Resonance(Id));
}

void Gallery::addResonance(void)
{
    int newId = getNewId(PreparationTypeResonance);
    resonance.add(new Resonance(newId));
}

void Gallery::addResonance(Resonance::Ptr p)
{
    int newId = getNewId(PreparationTypeResonance);
    p->setId(newId);
    resonance.add(p);
}

void Gallery::addResonance(ResonancePreparation::Ptr res)
{
    int newId = getNewId(PreparationTypeResonance);
    resonance.add(new Resonance(res, newId));
}

void Gallery::addTempo(void)
{
    int newId = getNewId(PreparationTypeTempo);
    tempo.add(new Tempo(newId));
}

void Gallery::addTempoWithId(int Id)
{
    tempo.add(new Tempo(Id));
}

void Gallery::addTempo(TempoPreparation::Ptr tmp)
{
    int newId = getNewId(PreparationTypeTempo);
    tempo.add(new Tempo(tmp, newId));
}

void Gallery::addTempo(Tempo::Ptr p)
{
    int newId = getNewId(PreparationTypeTempo);
    p->setId(newId);
    tempo.add(p);
}


void Gallery::addDirect(Direct::Ptr p)
{
    int newId = getNewId(PreparationTypeDirect);
    p->setId(newId);
    direct.add(p);
}

void Gallery::addDirect(void)
{
    int newId = getNewId(PreparationTypeDirect);
    direct.add(new Direct(newId));
}

void Gallery::addDirectWithId(int Id)
{
    direct.add(new Direct(Id));
}

void Gallery::addDirect(DirectPreparation::Ptr drct)
{
    int newId = getNewId(PreparationTypeDirect);
    direct.add(new Direct(drct, newId));
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
    
    thisUsed = used.getUnchecked(PreparationTypeResonance);
    for (int i = resonance.size(); --i >= 0;)
    {
        if (!thisUsed.contains(resonance[i]->getId())) resonance.remove(i);
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

	thisUsed = used.getUnchecked(PreparationTypeBlendronic);
	for (int i = blendronic.size(); --i >= 0;)
	{
		if (!thisUsed.contains(blendronic[i]->getId())) blendronic.remove(i);
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
    
    //put stuff for resonance eventually

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
    
    thisUsed = used.getUnchecked(PreparationTypeResonanceMod);
    for (int i = modResonance.size(); --i >= 0;)
    {
        if (!thisUsed.contains(modResonance[i]->getId())) modResonance.remove(i);
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

	thisUsed = used.getUnchecked(PreparationTypeBlendronicMod);
	for (int i = modBlendronic.size(); --i >= 0;)
	{
		if (!thisUsed.contains(modBlendronic[i]->getId())) modBlendronic.remove(i);
	}
    
    setGalleryDirty(true);
}
