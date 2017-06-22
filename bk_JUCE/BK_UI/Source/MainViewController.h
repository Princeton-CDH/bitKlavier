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

#include "HeaderViewController.h"

#include "PreparationPanel.h"

#include "BKConstructionSite.h"

#include "BKGraph.h"

#include "BKOvertop.h"

class MainViewController :  public Component, private Timer, public BKListener, public Slider::Listener, private KeyListener
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
    
    GeneralSettings::Ptr gen;
    
    BKItemGraph theGraph;
    
    Viewport viewPort;
    
    HeaderViewController header;
    BKConstructionSite  construction;

    BKOvertop overtop;
    
    //ImageComponent backgroundImageComponent;
    
    ScopedPointer<Slider> mainSlider;
    
    ScopedPointer<BKLevelMeterComponent> levelMeterComponentL;
    ScopedPointer<BKLevelMeterComponent> levelMeterComponentR;
    
    ScopedPointer<PreparationPanel> preparationPanel;
    
    void timerCallback() override;
    
    void bkButtonClicked        (Button* b)                     override;
    void sliderValueChanged     (Slider* slider)                override;
    
    void drawPreparationPanel(void);

    int timerCallbackCount;
    
    
    bool keyPressed (const KeyPress& e, Component*);

    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewController)
};

#endif  // MAINVIEWCONTROLLER_H_INCLUDED
