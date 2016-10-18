/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class MrmAudioProcessorEditor : public AudioProcessorEditor,
                                private Button::Listener
{
    
public:
    MrmAudioProcessorEditor (MrmAudioProcessor&);
    ~MrmAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    MrmAudioProcessor& processor;
    // GUI components
    TextButton button1;

    void buttonClicked(Button *) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MrmAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
