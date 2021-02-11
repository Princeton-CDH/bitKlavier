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
    
    bool tooltipsAreEnabled(void);
    Value getTooltipsEnabled(void);
    bool setTooltipsEnabled(bool enabled);

    /*
	bool areKeystrokesEnabled(void);
	Value getKeystrokesEnabled(void);
	bool setKeystrokesEnabled(bool enabled);
    */

	bool areHotkeysEnabled(void);
	Value getHotkeysEnabled(void);
	bool setHotkeysEnabled(bool enabled);
    
    void tooltipsChanged(void);
    
    void showBKSettingsDialog(Button* button);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    
    BKAudioProcessor& processor;
    
    Viewport viewPort;
    
    MainViewController mvc;
    
    //BKButtonAndMenuLAF laf;
    
    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    std::unique_ptr<ResizableCornerComponent> resizer;
    std::unique_ptr<TooltipWindow> tipwindow;
    
    BKWindowLAF laf;
    
    class PreferencesComponent : public Component
    {
    public:
        PreferencesComponent (BKAudioProcessorEditor& editor)
        : owner (editor),
        tooltipsLabel  ("Show tooltips", "Show tooltips"),
		//keystrokesLabel("Enable keystrokes", "Enable keystrokes"),
		hotkeysLabel("Enable hotkeys", "Enable hotkeys"),
        tooltipsButton (""),
		//keystrokesButton(""),
		hotkeysButton("")
        {
            setOpaque (true);
            
            tooltipsButton.setClickingTogglesState (true);
            tooltipsButton.getToggleStateValue().referTo (owner.getTooltipsEnabled());
            tooltipsButton.onClick = [this] { owner.tooltipsChanged(); };
            
            addAndMakeVisible (tooltipsButton);
            addAndMakeVisible (tooltipsLabel);
            
            tooltipsLabel.attachToComponent (&tooltipsButton, true);

            /*
			keystrokesButton.setClickingTogglesState(true);
			keystrokesButton.getToggleStateValue().referTo(owner.getKeystrokesEnabled());

			addAndMakeVisible(keystrokesButton);
			addAndMakeVisible(keystrokesLabel);

			keystrokesLabel.attachToComponent(&keystrokesButton, true);
            */

			hotkeysButton.setClickingTogglesState(true);
			hotkeysButton.getToggleStateValue().referTo(owner.getHotkeysEnabled());

			addAndMakeVisible(hotkeysButton);
			addAndMakeVisible(hotkeysLabel);

			hotkeysLabel.attachToComponent(&hotkeysButton, true);
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
			//keystrokesButton.setBounds(r.removeFromTop(24));
			hotkeysButton.setBounds(r.removeFromTop(24));
        }
        
    private:
        //==============================================================================
        BKAudioProcessorEditor& owner;
        
        Label tooltipsLabel;
		//Label keystrokesLabel;
		Label hotkeysLabel;
        ToggleButton tooltipsButton;
		//ToggleButton keystrokesButton;
		ToggleButton hotkeysButton;

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
