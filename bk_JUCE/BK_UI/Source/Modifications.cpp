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
tuningMods(TuningModification::PtrArr())
{
    
}

Modifications::~Modifications()
{
    
}



String  Modifications::stringRepresentation(void)
{
    String out = "";
    
    for (auto mod : directMods)
    {
        out += (String(mod->getNote())+ ":dm" + String(mod->getId()) + ":{" + String( mod->getPrepId()) + "} ");
    }
    
    for (auto mod : synchronicMods)
    {
        out += (String(mod->getNote())+ ":sm" + String(mod->getId()) + ":{" + String( mod->getPrepId()) + "} ");
    }
    
    for (auto mod : nostalgicMods)
    {
        out += (String(mod->getNote())+ ":nm" + String(mod->getId()) + ":{" + String( mod->getPrepId()) + "} ");
    }

    for (auto mod : tuningMods)
    {
        out += (String(mod->getNote())+ ":tm" + String(mod->getId()) + ":{" + String( mod->getPrepId()) + "} ");
    }
    
    return out;
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
        DBG("trying to remove: " + String(which));
        DBG("dmod ID: " + String(directMods[i]->getId()));
        if (directMods[i]->getId() == which)
        {
            directMods.remove(i);
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

void Modifications::clearModifications(void)
{
    synchronicMods.clear();
    nostalgicMods.clear();
    directMods.clear();
    tuningMods.clear();
    tempoMods.clear();
    
}

void Modifications::clearResets(void)
{
    synchronicReset.clear();
    nostalgicReset.clear();
    directReset.clear();
    tuningReset.clear();
    tempoReset.clear();
    
}
