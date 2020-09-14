/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "BKStandaloneWindow.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GraphicsConstants.h"

//==============================================================================
BKAudioProcessorEditor::BKAudioProcessorEditor (BKAudioProcessor& p):
AudioProcessorEditor (&p),
processor (p),
mvc(p, *this),
constrain(new ComponentBoundsConstrainer()),
resizer(new ResizableCornerComponent (this, constrain.get()))
{
    viewPort.setViewedComponent(&mvc);
    viewPort.setViewPosition(0, 0);
    addAndMakeVisible(viewPort);
    
#if JUCE_IOS
    setSize(processor.screenWidth, processor.screenHeight);
#else
    setSize((processor.screenWidth < DEFAULT_WIDTH) ? processor.screenWidth : DEFAULT_WIDTH,
            (processor.screenHeight < DEFAULT_HEIGHT) ? processor.screenHeight : DEFAULT_HEIGHT);

#endif
    constrain->setSizeLimits(DEFAULT_MIN_WIDTH, DEFAULT_MIN_HEIGHT, processor.screenWidth, processor.screenHeight);
    
#if !JUCE_IOS
    addAndMakeVisible(*resizer);
    resizer->setAlwaysOnTop(true);
#endif
    //processor.updateState->pianoDidChangeForGraph = true;
    
    resized();
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
    processor.paddingScalarX = (float)(getWidth() - DEFAULT_MIN_WIDTH) / (DEFAULT_WIDTH - DEFAULT_MIN_WIDTH);
    processor.paddingScalarY = (float)(getHeight() - DEFAULT_MIN_HEIGHT) / (DEFAULT_HEIGHT - DEFAULT_MIN_HEIGHT);
    
    viewPort.setBoundsRelative(0.0f,0.0f,1.0f,1.0f);
    
    mvc.setSize(getWidth(), getHeight());
    mvc.resized();
    
#if !JUCE_IOS
    resizer->setBounds(getWidth()-16, getHeight()-16, 16, 16);
#endif

}

void BKAudioProcessorEditor::showBKSettingsDialog(Button* button)
{
    Component* settings = new PreferencesComponent (*this);
    settings->setSize(200, 120);
    
    CallOutBox* preferences = &CallOutBox::launchAsynchronously (settings, button->getScreenBounds(), nullptr);
    preferences->setLookAndFeel(&laf);
}

bool BKAudioProcessorEditor::areTooltipsEnabled()
{
    return processor.areTooltipsEnabled();
}

Value BKAudioProcessorEditor::getTooltipsEnabled()
{
    return processor.getTooltipsEnabled();
}

bool BKAudioProcessorEditor::setTooltipsEnabled(bool enabled)
{
    processor.setTooltipsEnabled(enabled);
    return processor.areTooltipsEnabled();
}

bool BKAudioProcessorEditor::areHotkeysEnabled()
{
	return processor.areHotkeysEnabled();
}

Value BKAudioProcessorEditor::getHotkeysEnabled(void)
{
	return processor.getHotkeysEnabled();
}

bool BKAudioProcessorEditor::setHotkeysEnabled(bool enabled)
{
	processor.setHotkeysEnabled(enabled);
	return processor.areHotkeysEnabled();
}

//==============================================================================

