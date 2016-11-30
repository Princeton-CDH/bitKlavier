/*
  ==============================================================================

    PreparationViewController.cpp
    Created: 20 Nov 2016 11:06:14am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#include "BKViewController.h"

//==============================================================================
BKViewController::BKViewController()
{

}

BKViewController::~BKViewController()
{
    
}



void BKViewController::paint (Graphics& g)
{
    
}

void BKViewController::resized()
{

}

void BKViewController::textEditorFocusLost(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}

void BKViewController::textEditorReturnKeyPressed(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}

void BKViewController::textEditorEscapeKeyPressed(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}


void BKViewController::textEditorTextChanged(TextEditor& tf)
{
    shouldChange = true;
}
