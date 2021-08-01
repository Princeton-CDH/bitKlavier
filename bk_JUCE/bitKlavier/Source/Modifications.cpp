/*
  ==============================================================================

    Modifications.cpp
    Created: 31 Jan 2017 3:09:23pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Modifications.h"

Modifications::Modifications():
directMods(DirectModification::PtrArr()),
synchronicMods(SynchronicModification::PtrArr()),
nostalgicMods(NostalgicModification::PtrArr()),
tuningMods(TuningModification::PtrArr()),
blendronicMods(BlendronicModification::PtrArr())
{
    
}

Modifications::~Modifications()
{
    
}


void Modifications::addSynchronicModification(SynchronicModification::Ptr m)
{
    synchronicMods.add(m);
}

void Modifications::removeSynchronicModification(SynchronicModification::Ptr m)
{
    for (int i = synchronicMods.size(); --i >= 0;)
    {
        if (synchronicMods[i] == m)
        {
            synchronicMods.remove(i);
            break;
        }
    }
}

void Modifications::removeSynchronicModification(int which)
{
    for (int i = synchronicMods.size(); --i >= 0;)
    {
        if (synchronicMods[i]->getId() == which)
        {
            synchronicMods.remove(i);
            break;
        }
    }
}

void Modifications::addNostalgicModification(NostalgicModification::Ptr m)
{
    nostalgicMods.add(m);
}

void Modifications::removeNostalgicModification(NostalgicModification::Ptr m)
{
    for (int i = nostalgicMods.size(); --i >= 0;)
    {
        if (nostalgicMods[i] == m)
        {
            nostalgicMods.remove(i);
            break;
        }
    }
}

void Modifications::removeNostalgicModification(int which)
{
    for (int i = nostalgicMods.size(); --i >= 0;)
    {
        if (nostalgicMods[i]->getId() == which)
        {
            nostalgicMods.remove(i);
            break;
        }
    }
}

void Modifications::addDirectModification(DirectModification::Ptr m)
{
    directMods.add(m);
}

void Modifications::addResonanceModification(ResonanceModification::Ptr m)
{
    resonanceMods.add(m);
}


void Modifications::removeDirectModification(DirectModification::Ptr m)
{
    for (int i = directMods.size(); --i >= 0;)
    {
        if (directMods[i] == m)
        {
            directMods.remove(i);
            break;
        }
    }
}

void Modifications::removeDirectModification(int which)
{
    for (int i = 0; i < directMods.size(); i++)
    {
        if (directMods[i]->getId() == which)
        {
            directMods.remove(i);
            break;
        }
    }
}

void Modifications::removeResonanceModification(ResonanceModification::Ptr m)
{
    for (int i = resonanceMods.size(); --i >= 0;)
    {
        if (resonanceMods[i] == m)
        {
            resonanceMods.remove(i);
            break;
        }
    }
}

void Modifications::removeResonanceModification(int which)
{
    for (int i = 0; i < resonanceMods.size(); i++)
    {
        if (resonanceMods[i]->getId() == which)
        {
            resonanceMods.remove(i);
            break;
        }
    }
}

void Modifications::addTuningModification(TuningModification::Ptr m)
{
    tuningMods.add(m);
}


void Modifications::removeTuningModification(TuningModification::Ptr m)
{
    for (int i = tuningMods.size(); --i >= 0;)
    {
        if (tuningMods[i] == m)
        {
            tuningMods.remove(i);
            break;
        }
    }
}

void Modifications::removeTuningModification(int which)
{
    for (int i = tuningMods.size(); --i >= 0;)
    {
        if (tuningMods[i]->getId() == which)
        {
            tuningMods.remove(i);
            break;
        }
    }
}

void Modifications::addBlendronicModification(BlendronicModification::Ptr b)
{
	blendronicMods.add(b);
}

void Modifications::removeBlendronicModification(BlendronicModification::Ptr b)
{
	for (int i = blendronicMods.size(); --i >= 0;)
	{
		if (blendronicMods[i] == b)
		{
			blendronicMods.remove(i);
			break;
		}
	}
}

void Modifications::removeBlendronicModification(int which)
{
	for (int i = blendronicMods.size(); --i >= 0;)
	{
		if (blendronicMods[i]->getId() == which)
		{
			blendronicMods.remove(i);
			break;
		}
	}
}


void Modifications::addTempoModification(TempoModification::Ptr m)
{
    tempoMods.add(m);
}

void Modifications::removeTempoModification(TempoModification::Ptr m)
{
    for (int i = tempoMods.size(); --i >= 0;)
    {
        if (tempoMods[i] == m)
        {
            tempoMods.remove(i);
            break;
        }
    }
}

void Modifications::removeTempoModification(int which)
{
    for (int i = tempoMods.size(); --i >= 0;)
    {
        if (tempoMods[i]->getId() == which)
        {
            tempoMods.remove(i);
            break;
        }
    }
}

SynchronicModification::PtrArr Modifications::getSynchronicModifications(void)
{
    return synchronicMods;
}

NostalgicModification::PtrArr Modifications::getNostalgicModifications(void)
{
    return nostalgicMods;
}

DirectModification::PtrArr Modifications::getDirectModifications(void)
{
    return directMods;
}

TuningModification::PtrArr Modifications::getTuningModifications(void)
{
    return tuningMods;
}

TempoModification::PtrArr Modifications::getTempoModifications(void)
{
    return tempoMods;
}

BlendronicModification::PtrArr Modifications::getBlendronicModifications(void)
{
	return blendronicMods;
}

void Modifications::clearModifications(void)
{
    synchronicMods.clear();
    nostalgicMods.clear();
    directMods.clear();
    tuningMods.clear();
    tempoMods.clear();
	blendronicMods.clear();
}

void Modifications::clearResets(void)
{
    synchronicResets.clear();
    nostalgicResets.clear();
    directResets.clear();
    tuningResets.clear();
    tempoResets.clear();
	blendronicResets.clear();
    synchronicModResets.clear();
    nostalgicModResets.clear();
    directModResets.clear();
    tuningModResets.clear();
    tempoModResets.clear();
    blendronicModResets.clear();
}
