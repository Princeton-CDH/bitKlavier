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
    
    bool areTooltipsEnabled(void);
    Value getTooltipsEnabled(void);
    bool setTooltipsEnabled(bool enabled);
    
    void showBKSettingsDialog(Button* button);
    void showAudioSettingsDialog(Button* button);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    
    BKAudioProcessor& processor;
    
    Viewport viewPort;
    
    MainViewController mvc;
    
    //BKButtonAndMenuLAF laf;
    
    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    std::unique_ptr<ResizableCornerComponent> resizer;
    
    BKWindowLAF laf;
    
    class PreferencesComponent : public Component
    {
    public:
        PreferencesComponent (BKAudioProcessorEditor& editor)
        : owner (editor),
        tooltipsLabel  ("Show tooltips", "Show tooltips"),
        tooltipsButton ("")
        {
            setOpaque (true);
            
            tooltipsButton.setClickingTogglesState (true);
            tooltipsButton.getToggleStateValue().referTo (owner.getTooltipsEnabled());
            
            addAndMakeVisible (tooltipsButton);
            addAndMakeVisible (tooltipsLabel);
            
            tooltipsLabel.attachToComponent (&tooltipsButton, true);
        }
        
        void paint (Graphics& g) override
        {
            g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        }
        
        void resized() override
        {
            auto r = getLocalBounds();
            
            r.removeFromTop(8);
            r.removeFromLeft(r.getWidth() * 0.5);
            r.removeFromRight(8);
            tooltipsButton.setBounds (r.removeFromTop(24));
        }
        
    private:
        //==============================================================================
        BKAudioProcessorEditor& owner;
        
        Label tooltipsLabel;
        ToggleButton tooltipsButton;
        // Other ideas for preferences
        // - Show keyboard contents when selected
        // - Preparation activity indicators
        // - UI Keyboard behavior (toggle / press)
        // - Colors
        
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreferencesComponent)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessorEditor)
};




#endif  // PLUGINEDITOR_H_INCLUDED
