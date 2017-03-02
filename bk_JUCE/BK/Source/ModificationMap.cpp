/*
  ==============================================================================

    ModificationMap.cpp
    Created: 31 Jan 2017 3:09:23pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "ModificationMap.h"

ModificationMap::ModificationMap():
directMods(DirectModification::PtrArr()),
synchronicMods(SynchronicModification::PtrArr()),
nostalgicMods(NostalgicModification::PtrArr()),
tuningMods(TuningModification::PtrArr())
{
    directMods.ensureStorageAllocated(1);
    synchronicMods.ensureStorageAllocated(1);
    nostalgicMods.ensureStorageAllocated(1);
    tuningMods.ensureStorageAllocated((1));
}

ModificationMap::~ModificationMap()
{
    
}

String  ModificationMap::stringRepresentation(void)
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

void ModificationMap::addSynchronicModification(SynchronicModification::Ptr m)
{
    synchronicMods.add(m);
}

void ModificationMap::addNostalgicModification(NostalgicModification::Ptr m)
{
    nostalgicMods.add(m);
}

void ModificationMap::addDirectModification(DirectModification::Ptr m)
{
    directMods.add(m);
}

void ModificationMap::addTuningModification(TuningModification::Ptr m)
{
    tuningMods.add(m);
}

SynchronicModification::PtrArr ModificationMap::getSynchronicModifications(void)
{
    return synchronicMods;
}

NostalgicModification::PtrArr ModificationMap::getNostalgicModifications(void)
{
    return nostalgicMods;
}

DirectModification::PtrArr ModificationMap::getDirectModifications(void)
{
    return directMods;
}

TuningModification::PtrArr ModificationMap::getTuningModifications(void)
{
    return tuningMods;
}

void ModificationMap::clearModifications(void)
{
    synchronicMods.clear();
    nostalgicMods.clear();
    directMods.clear();
    tuningMods.clear();
    
}
