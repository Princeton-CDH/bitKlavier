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

void Modifications::addNostalgicModification(NostalgicModification::Ptr m)
{
    nostalgicMods.add(m);
}

void Modifications::addDirectModification(DirectModification::Ptr m)
{
    directMods.add(m);
}

void Modifications::addTuningModification(TuningModification::Ptr m)
{
    tuningMods.add(m);
}

void Modifications::addTempoModification(TempoModification::Ptr m)
{
    tempoMods.add(m);
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
