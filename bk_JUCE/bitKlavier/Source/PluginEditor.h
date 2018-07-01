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

#include "MainViewController.h"



//==============================================================================
/**
*/
class BKAudioProcessorEditor :  public AudioProcessorEditor
{
    
public:
    BKAudioProcessorEditor (BKAudioProcessor&);
    ~BKAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    
    BKAudioProcessor& processor;
    
    Viewport viewPort;
    
    MainViewController mvc;
    
    ScopedPointer<ComponentBoundsConstrainer> constrain;

    ScopedPointer<ResizableCornerComponent> resizerCorner;
    
    //BKButtonAndMenuLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessorEditor)
};




#endif  // PLUGINEDITOR_H_INCLUDED
