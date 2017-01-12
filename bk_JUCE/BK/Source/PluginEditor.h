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

#include "SynchronicViewController.h"
#include "NostalgicViewController.h"
#include "DirectViewController.h"
#include "TuningViewController.h"

#include "GeneralViewController.h"

#include "PreparationsMapViewController.h"
#include "KeymapViewController.h"


//==============================================================================
/**
*/
class BKAudioProcessorEditor : public AudioProcessorEditor,
                                private TextEditor::Listener

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
    
    PreparationsMapViewController         lvc;
    KeymapViewController        kvc;
    GeneralViewController       gvc;
    SynchronicViewController    svc;
    NostalgicViewController     nvc;
    DirectViewController        dvc;
    TuningViewController        tvc;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
