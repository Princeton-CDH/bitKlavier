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
BKAudioProcessorEditor::BKAudioProcessorEditor (BKAudioProcessor& p)
:
    AudioProcessorEditor (&p),
    processor (p),
    svc(p),
    nvc(p)
{
    addAndMakeVisible(svc);
    addAndMakeVisible(nvc);
    
    setSize(gMainComponentWidth,
            gMainComponentHeight);
    
}

BKAudioProcessorEditor::~BKAudioProcessorEditor()
{
    
}

//==============================================================================
void BKAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
    
}

void BKAudioProcessorEditor::resized()
{
    svc.setBounds(gComponentLeftOffset,
                  gComponentTopOffset,
                  500,
                  500);
    
    nvc.setBounds(gComponentLeftOffset + 500 + 5,
                  gComponentTopOffset,
                  500,
                  500);
     
    
}
