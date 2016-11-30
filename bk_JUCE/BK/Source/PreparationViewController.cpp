/*
  ==============================================================================

    PreparationViewController.cpp
    Created: 20 Nov 2016 11:06:14am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#include "PreparationViewController.h"

//==============================================================================
PreparationViewController::PreparationViewController()
{

}

PreparationViewController::~PreparationViewController()
{
    
}



void PreparationViewController::paint (Graphics& g)
{
    
}

void PreparationViewController::resized()
{

}

void PreparationViewController::textEditorFocusLost(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}

void PreparationViewController::textEditorReturnKeyPressed(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}

void PreparationViewController::textEditorEscapeKeyPressed(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}


void PreparationViewController::textEditorTextChanged(TextEditor& tf)
{
    shouldChange = true;
}
