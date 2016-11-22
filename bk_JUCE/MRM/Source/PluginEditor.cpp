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
:
    AudioProcessorEditor (&p),
    processor (p),
    svc(p)
{
    addAndMakeVisible(svc);
    
    setSize(gMainComponentWidth, gMainComponentHeight);

}

MrmAudioProcessorEditor::~MrmAudioProcessorEditor()
{
    
}

//==============================================================================
void MrmAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(gMainComponentColor);
    
}

void MrmAudioProcessorEditor::resized()
{
    svc.setBounds(gSynchronic_LeftOffset,gSynchronic_TopOffset,500,500);
    
}
