/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GraphicsConstants.h"


//==============================================================================
MrmAudioProcessorEditor::MrmAudioProcessorEditor (MrmAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
        
    setSize (gMainComponentWidth, gMainComponentHeight);
    
    addAndMakeVisible (&button1);
    button1.setButtonText ("1");
    button1.addListener (this);
    button1.setColour (TextButton::buttonColourId, Colours::black);
    button1.setEnabled(false);
    
}

MrmAudioProcessorEditor::~MrmAudioProcessorEditor()
{
    
}

void MrmAudioProcessorEditor::buttonClicked(Button *button)
{
}

//==============================================================================
void MrmAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(gMainComponentColor);
    
}

void MrmAudioProcessorEditor::resized()
{
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    button1.setBounds(10, 50, 30, getHeight() - 70);
}
