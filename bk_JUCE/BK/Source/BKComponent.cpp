/*
 ==============================================================================
 
 BKComponent.cpp
 Created: 20 Nov 2016 11:06:14am
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#include "BKComponent.h"

//==============================================================================
BKComponent::BKComponent()
{
    
}

BKComponent::~BKComponent()
{
    
}



void BKComponent::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void BKComponent::resized()
{
    
}
