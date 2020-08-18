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

class BKAudioProcessorEditor;
class BKConstructionSite;

#include "BKGraph.h"

#include "BKOvertop.h"

class BKSplashScreen : public Component
{
public:
    BKSplashScreen(BKAudioProcessor& p):
    processor(p)
    {
        addAndMakeVisible(progressSlider);
        progressSlider.setRange (0.0, 1.0, 0.001);
        progressSlider.setSliderStyle (Slider::LinearBar );
        progressSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        progressSlider.setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.95));
        
        image = ImageCache::getFromMemory(BinaryData::logo_jpg, BinaryData::logo_jpgSize);
        placement = RectanglePlacement::centred;
        
#ifdef LABEL
        addAndMakeVisible(label);
        label.setText("Loading samples...", dontSendNotification);
#endif
    }
    
    ~BKSplashScreen()
    {
        
    }
    
    void setProgress(double progress)
    {
        progressSlider.setValue(progress, dontSendNotification);
    }
    
    void paint(Graphics& g)
    {
        g.fillAll(Colours::black);
        
        g.drawImage (image, getLocalBounds().toFloat(), placement);
    }
    
    void resized(void)
    {
        progressSlider.setSize(getWidth() * 0.875, 25);
        progressSlider.setCentrePosition(getWidth() * 0.5, getHeight() * 0.7);
#ifdef LABEL
        label.setBounds(progressSlider.getX(), progressSlider.getY() - 25 - gYSpacing, 100, 25);
#endif
    }
    
private:
    Slider progressSlider;
    BKAudioProcessor& processor;
    
#ifdef LABEL
    BKLabel label;
#endif
    
    Image image;
    RectanglePlacement placement;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSplashScreen)
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
    MainViewController (BKAudioProcessor&, BKAudioProcessorEditor&);
    ~MainViewController();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    inline void mouseDrag(const MouseEvent& e) override
    {
        /*viewPort.autoScroll(e.x, e.y, 100, 150);*/
    }
    
    /*inline Viewport* getViewport(void) { return &viewPort;}*/
    void toggleDisplay(void);
    
    Slider octaveSlider;
    //void setSliderLookAndFeel(BKButtonAndMenuLAF *laf);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    
    BKAudioProcessor& processor;
    BKAudioProcessorEditor& editor;
    
    GeneralSettings::Ptr gen;
    
    BKItemGraph theGraph;
    
    int keyStart, keyEnd;
    //Viewport viewPort;
    
    HeaderViewController header;
    BKConstructionSite  construction;

    BKOvertop overtop;
    std::unique_ptr<DropShadower> overtopShadow;
    
    BKSplashScreen splash;
    
    //ImageComponent backgroundImageComponent;
    
    Slider mainSlider;
    std::unique_ptr<BKLevelMeterComponent> levelMeterComponentL;
    std::unique_ptr<BKLevelMeterComponent> levelMeterComponentR;
    BKLevelMeterComponent testmeterComponent;
    
    std::unique_ptr<PreparationPanel> preparationPanel;
    
    std::unique_ptr<BKKeymapKeyboardComponent> keyboard;
    BKKeymapKeyboardState keyboardState;
    Component *keyboardComponent;
    
    BKComboBox sampleCB;
    BKComboBox instrumentCB;
    
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
    
    void bkComboBoxDidChange(ComboBox* cb) override;
    
    void fillSampleCB();
    void fillInstrumentCB();
    
    void drawPreparationPanel(void);

    int timerCallbackCount;
    
    bool keyPressed (const KeyPress& e, Component*) override;
    
    bool isAddingFromMidiInput;

	bool hotkeysEnabled;
	bool keystrokesEnabled;
    
    TextButton preferencesButton;

    ToggleButton tooltipsButton;
	ToggleButton keystrokesButton;
	ToggleButton hotkeysButton;
    
    ToggleButton globalSoundSetButton;

    ToggleButton sustainPedalButton;
    
    BKButtonAndMenuLAF laf;
    BKButtonAndMenuLAF comboBoxRightJustifyLAF;
    BKWindowLAF windowLAF;
    
    std::unique_ptr<TooltipWindow> tipwindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewController)
};

#endif  // MAINVIEWCONTROLLER_H_INCLUDED
