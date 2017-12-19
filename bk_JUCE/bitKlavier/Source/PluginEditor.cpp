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
    
    //laf = new BKButtonAndMenuLAF();
    //mvc.setSliderLookAndFeel(&laf); 
    
#if !JUCE_IOS
    addAndMakeVisible (resizer);
    
    resizer->setAlwaysOnTop(true);
#endif
    
#if JUCE_IOS
    setSize(processor.screenWidth, processor.screenHeight);
    
    constrain->setSizeLimits(processor.screenWidth, processor.screenHeight, processor.screenWidth, processor.screenHeight);
#else
    setSize((processor.screenWidth < DEFAULT_WIDTH) ? processor.screenWidth : DEFAULT_WIDTH,
            (processor.screenHeight < DEFAULT_HEIGHT) ? processor.screenHeight : DEFAULT_HEIGHT);
    
    constrain->setSizeLimits(DEFAULT_MIN_WIDTH, DEFAULT_MIN_HEIGHT, DEFAULT_MIN_WIDTH * 8, DEFAULT_MIN_HEIGHT * 8);
#endif
    
    //processor.updateState->pianoDidChangeForGraph = true;
}

BKAudioProcessorEditor::~BKAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void BKAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}


void BKAudioProcessorEditor::resized()
{
    processor.paddingScalarX = (float)(getTopLevelComponent()->getWidth() - DEFAULT_MIN_WIDTH) / (DEFAULT_WIDTH - DEFAULT_MIN_WIDTH);
    processor.paddingScalarY = (float)(getTopLevelComponent()->getHeight() - DEFAULT_MIN_HEIGHT) / (DEFAULT_HEIGHT - DEFAULT_MIN_HEIGHT);
    
    viewPort.setBoundsRelative(0.0f,0.0f,1.0f,1.0f);
    
#if !JUCE_IOS
    resizer->setBounds(getWidth()-16, getHeight()-16, 16, 16);
#endif
    
    mvc.setSize(getWidth(), getHeight());
    mvc.resized();
    
}


//==============================================================================

