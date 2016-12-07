/*
  ==============================================================================

    Library.cpp
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Library.h"

Library::Library()
{
    
}

Library::Library(Preset::Arr p):
presets(p)
{
    
}

Library::~Library()
{
    
}

void Library::writeToFile(String filepath)
{
    
}

Preset::Arr Library::readFromFile(String filepath)
{
    
    return presets;
}
