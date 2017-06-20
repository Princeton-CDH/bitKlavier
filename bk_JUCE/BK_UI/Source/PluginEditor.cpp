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
    addKeyListener(this);
    
    viewPort.setViewedComponent(&mvc);
    viewPort.setViewPosition(0, 0);
    addAndMakeVisible(viewPort);
    
    addAndMakeVisible (resizer);
    
    resizer->setAlwaysOnTop(true);
    
    constrain->setSizeLimits(400, 150, 2000, 1500);
    
    mvc.setSize(gMainComponentWidth, gMainComponentHeight);
    setSize(gMainComponentWidth, gMainComponentHeight);
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
    
    resizer->setBounds(getWidth()-16, getHeight()-16, 16, 16);
    
    mvc.setSize(getWidth(), getHeight());
    mvc.resized();
    
    
}

bool BKAudioProcessorEditor::keyPressed (const KeyPress& e, Component*)
{
    DBG(e.getKeyCode());
    
    int code = e.getKeyCode();
    
    if (code == KeyPress::escapeKey)
    {
        mvc.escapePressed();
    }
    else if (code == KeyPress::deleteKey)
    {
        mvc.deletePressed();
    }
    else if (code == KeyPress::backspaceKey)
    {
        mvc.deletePressed();
    }
    else if (code == KeyPress::upKey)
    {
        if (e.getModifiers().isCommandDown())
            mvc.align(0);
        else
            mvc.arrowPressed(0, e.getModifiers().isShiftDown());
    }
    else if (code == KeyPress::rightKey)
    {   if (e.getModifiers().isCommandDown())
            mvc.align(1);
        else
            mvc.arrowPressed(1, e.getModifiers().isShiftDown());
    }
    else if (code == KeyPress::downKey)
    {
        if (e.getModifiers().isCommandDown())
            mvc.align(2);
        else
            mvc.arrowPressed(2, e.getModifiers().isShiftDown());
    }
    else if (code == KeyPress::leftKey)
    {
        if (e.getModifiers().isCommandDown())
            mvc.align(3);
        else
            mvc.arrowPressed(3, e.getModifiers().isShiftDown());
    }
    else if (code == KeyPress::tabKey)
    {
        mvc.tabPressed();
    }
}


//==============================================================================

