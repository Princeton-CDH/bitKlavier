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

#include "PreparationMapViewController.h"
#include "KeymapViewController.h"

#include "BKViewController.h"

//==============================================================================
/**
*/
class BKAudioProcessorEditor :  public AudioProcessorEditor,
                                private TextEditor::Listener,
                                private TextButton::Listener,
                                private ComboBox::Listener

{
    
public:
    BKAudioProcessorEditor (BKAudioProcessor&);
    ~BKAudioProcessorEditor();
    
    void drawNewPreparationMap(int Id);
    void removeLastPreparationMap(int Id);
    
    void switchPianos(void);

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    float pmapH;
    Rectangle<int> upperLeft;
    
    BKAudioProcessor& processor;
    
    
    
    Component*                          pvc;
    PreparationMapViewController::PtrArr pmvc;
    KeymapViewController                kvc;
    GeneralViewController               gvc;
    SynchronicViewController            svc;
    NostalgicViewController             nvc;
    DirectViewController                dvc;
    TuningViewController                tvc;

    void buttonClicked (Button* b) override;
    
    void comboBoxChanged            (ComboBox* comboBoxThatHasChanged) override;
    
    OwnedArray<BKLabel>                 pianoL;
    OwnedArray<BKComboBox>              pianoCB;
    
    TextButton                          addPMapButton;
    TextButton                          removePMapButton;
    
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessorEditor)
};




#endif  // PLUGINEDITOR_H_INCLUDED
