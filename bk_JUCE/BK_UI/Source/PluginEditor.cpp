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
BKAudioProcessorEditor::BKAudioProcessorEditor (BKAudioProcessor& p):
AudioProcessorEditor (&p),
processor (p),
mvc(p),
constrain(new ComponentBoundsConstrainer()),
resizer(new ResizableCornerComponent (this, constrain))
{
    viewPort.setViewedComponent(&mvc);
    viewPort.setViewPosition(0, 0);
    addAndMakeVisible(viewPort);
    
    addAndMakeVisible (resizer);
    
    resizer->setAlwaysOnTop(true);
    
    constrain->setSizeLimits(gMainComponentMinWidth, gMainComponentMinHeight, gMainComponentWidth, gMainComponentHeight);
    
    mvc.setSize(gMainComponentWidth, gMainComponentHeight);
    setSize(gMainComponentWidth, gMainComponentHeight);
}

BKAudioProcessorEditor::~BKAudioProcessorEditor()
{
    
}

void BKAudioProcessorEditor::paint (Graphics& g)
{
    //g.fillAll(Colours::black);
}


void BKAudioProcessorEditor::resized()
{
    
    viewPort.setBoundsRelative(0.0f,0.0f,1.0f,1.0f);
    
    resizer->setBounds(getWidth()-16, getHeight()-16, 16, 16);
    
    mvc.setSize(getWidth(), getHeight());
    mvc.resized();
    
    
}


//==============================================================================

