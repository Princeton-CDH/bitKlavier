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
    
#if !JUCE_IOS
    addAndMakeVisible (resizer);
    
    resizer->setAlwaysOnTop(true);
#endif
    
    setSize(processor.uiWidth, processor.uiHeight);
    
    constrain->setSizeLimits(processor.uiWidth, processor.uiHeight, processor.uiWidth * 2, processor.uiHeight * 2);
    
    processor.initializeGallery();
    processor.updateState->pianoDidChangeForGraph = true;
}

BKAudioProcessorEditor::~BKAudioProcessorEditor()
{
    
}

void BKAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}


void BKAudioProcessorEditor::resized()
{
    
    viewPort.setBoundsRelative(0.0f,0.0f,1.0f,1.0f);
    
#if !JUCE_IOS
    resizer->setBounds(getWidth()-16, getHeight()-16, 16, 16);
#endif
    
    mvc.setSize(getWidth(), getHeight());
    mvc.resized();
    
}


//==============================================================================

