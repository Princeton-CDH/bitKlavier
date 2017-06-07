/*
  ==============================================================================

    MainViewController.h
    Created: 27 Mar 2017 2:04:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef MAINVIEWCONTROLLER_H_INCLUDED
#define MAINVIEWCONTROLLER_H_INCLUDED

#include "PluginProcessor.h"
#include "BKListener.h"
#include "BKComponent.h"
#include "BKLevelMeter.h"

#include "SynchronicViewController.h"
#include "SynchronicViewController2.h"
#include "NostalgicViewController.h"
#include "NostalgicViewController2.h"
#include "DirectViewController.h"
#include "TuningViewController.h"
#include "TempoViewController.h"

#include "GeneralViewController.h"

#include "GalleryViewController.h"

#include "KeymapViewController.h"

#include "PreparationPanel.h"

#include "BKConstructionSite.h"

#include "BKGraph.h"

class MainViewController :  public Component, private Timer, public BKListener, public Slider::Listener
{
    
public:
    MainViewController (BKAudioProcessor&);
    ~MainViewController();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    
    
    
    BKAudioProcessor& processor;
    
    BKItemGraph theGraph;
    
    Viewport viewPort;
    
    BKConstructionSite  construction;
    
    GalleryViewController galvc;
    
    KeymapViewController kvc;
    TuningViewController tvc;
    GeneralViewController gvc;
    SynchronicViewController svc;
    NostalgicViewController nvc;
    DirectViewController dvc;
    TempoViewController ovc;
    
    GeneralSettings::Ptr gen;
    
    SynchronicViewController2 svc2;
    NostalgicViewController2 nvc2;
    
    OwnedArray<TextButton> buttons;
    ScopedPointer<Slider> mainSlider;
    
    ScopedPointer<BKLevelMeterComponent> levelMeterComponentL;
    ScopedPointer<BKLevelMeterComponent> levelMeterComponentR;
    
    ScopedPointer<PreparationPanel> preparationPanel;
    
    void timerCallback() override;
    
    void drawPreparationPanel(void);

    int timerCallbackCount;
    
    BKPreparationDisplay currentDisplay;
    
    void bkTextFieldDidChange       (TextEditor&)           override{};
    void bkMessageReceived          (const String& message) override{};
    void bkComboBoxDidChange        (ComboBox* box)         override{};
    void bkButtonClicked            (Button* b)             override;
    void sliderValueChanged         (Slider* slider)        override;
    
    void setCurrentDisplay(BKPreparationDisplay type);
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewController)
};

#endif  // MAINVIEWCONTROLLER_H_INCLUDED
