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

class BKConstructionSite;

#include "BKGraph.h"

#include "BKOvertop.h"

class SampleLoadView : public Component
{
public:
    SampleLoadView(BKAudioProcessor& p):
    processor(p)
    {
        addAndMakeVisible(progressSlider);
        progressSlider.setRange (0.0, 1.0, 0.001);
        progressSlider.setSliderStyle (Slider::LinearBarVertical);
    }
    
    ~SampleLoadView()
    {
        
    }
    
    void paint(Graphics& g)
    {
        g.fillAll(Colours::black.withAlpha(0.5f));
    }
    
    void resized(void)
    {
        progressSlider.setSize(processor.screenWidth * 0.75, processor.screenHeight * 0.5);
        progressSlider.setCentrePosition(getWidth() * 0.5, getHeight() * 0.5);
    }
    
private:
    Slider progressSlider;
    BKAudioProcessor& processor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleLoadView)
};

class MainViewController :
public Component,
private Timer,
public BKListener,
public Slider::Listener,
private KeyListener,
private BKKeymapKeyboardStateListener
{
    
public:
    MainViewController (BKAudioProcessor&);
    ~MainViewController();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    inline void mouseDrag(const MouseEvent& e) override
    {
        /*viewPort.autoScroll(e.x, e.y, 100, 150);*/
    }
    
    /*inline Viewport* getViewport(void) { return &viewPort;}*/
    
    void setDisplay(DisplayType type);
    
    Slider octaveSlider;
    //void setSliderLookAndFeel(BKButtonAndMenuLAF *laf);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    
    BKAudioProcessor& processor;
    
    GeneralSettings::Ptr gen;
    
    BKItemGraph theGraph;
    
    int keyStart, keyEnd;
    //Viewport viewPort;
    
    HeaderViewController header;
    BKConstructionSite  construction;

    BKOvertop overtop;
    ScopedPointer<DropShadower> overtopShadow;
    
    SampleLoadView loadView;
    
    //ImageComponent backgroundImageComponent;
    
    ScopedPointer<Slider> mainSlider;
    ScopedPointer<BKLevelMeterComponent> levelMeterComponentL;
    ScopedPointer<BKLevelMeterComponent> levelMeterComponentR;
    
    ScopedPointer<PreparationPanel> preparationPanel;
    
    ScopedPointer<BKKeymapKeyboardComponent> keyboard;
    BKKeymapKeyboardState keyboardState;
    Component *keyboardComponent;
    
    

    DisplayType display;
    
    bool initial;
    int initialWidth, initialHeight;
    /*
    void handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    */
    
    void handleNoteOn(BKKeymapKeyboardState* source, int midiNoteNumber, float velocity) override;
    void handleNoteOff(BKKeymapKeyboardState* source, int midiNoteNumber, float velocity) override;
    
    void timerCallback() override;
    
    void bkButtonClicked        (Button* b)                     override;
    void sliderValueChanged     (Slider* slider)                override;
    void mouseDown (const MouseEvent &event) override;
    
    void drawPreparationPanel(void);

    int timerCallbackCount;
    
    bool keyPressed (const KeyPress& e, Component*) override;
    
    bool isAddingFromMidiInput;
    
    BKButtonAndMenuLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewController)
};

#endif  // MAINVIEWCONTROLLER_H_INCLUDED
