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

#include "BKComponent.h"
#include "BKListener.h"

//==============================================================================
/**
*/
class BKAudioProcessorEditor :  public AudioProcessorEditor, public BKListener, private Timer
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
    
    void timerCallback() override;
    
    BKAudioProcessor& processor;
    
    
    BKComponent*                          loadvc;
    BKComponent*                          pvc;
    PreparationMapViewController::PtrArr  pmvc;
    KeymapViewController                  kvc;
    GeneralViewController                 gvc;
    SynchronicViewController              svc;
    NostalgicViewController               nvc;
    DirectViewController                  dvc;
    TuningViewController                  tvc;

    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override{};
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override;
    
    
    String                              processPianoMapString(const String& message);
    String                              processModMapString(const String& message);
    
    TextButton                          addPMapButton;
    TextButton                          removePMapButton;
    TextButton                          saveButton, loadButton, loadJsonButton;
    
    OwnedArray<TextButton>              loadButtons;
    
    BKLabel                             pianoL;
    BKComboBox                          pianoCB;

    BKLabel                             pianoMapL;
    BKTextField                         pianoMapTF;
    
    BKLabel                             modMapL;
    BKTextField                         modMapTF;
    
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessorEditor)
};




#endif  // PLUGINEDITOR_H_INCLUDED
