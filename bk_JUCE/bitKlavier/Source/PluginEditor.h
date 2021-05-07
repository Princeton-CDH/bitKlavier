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

	bool areHotkeysEnabled(void);
	Value getHotkeysEnabled(void);
	bool setHotkeysEnabled(bool enabled);
    
    bool isMemoryMappingEnabled(void);
    Value getMemoryMappingEnabled(void);
    bool setMemoryMappingEnabled(bool enabled);
    
    void tooltipsChanged(void);
    
    void showBKSettingsDialog(Button* button);
    
    MainViewController& getMainViewController() { return mvc; }
    
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
    
    class PreferencesComponent : public Component,
                                 public TextEditor::Listener
    {
    public:
        PreferencesComponent (BKAudioProcessorEditor& editor)
        : owner (editor),
        tooltipsLabel  ("Show tooltips", "Show tooltips"),
		hotkeysLabel("Enable hotkeys", "Enable hotkeys"),
        memoryMappingLabel("Enable direct-from-disk sample playback", "Enable direct-from-disk sample playback"),
        tooltipsButton ("Enable tooltips"),
		hotkeysButton("Enable hotkeys"),
        memoryMappingButton("Enable direct-from-disk sample playback")
        {
            setOpaque (true);
            
            addAndMakeVisible(searchPathEditor);
            
            tooltipsButton.setClickingTogglesState (true);
            tooltipsButton.getToggleStateValue().referTo (owner.getTooltipsEnabled());
            tooltipsButton.onClick = [this] { owner.tooltipsChanged(); };
            addAndMakeVisible (tooltipsButton);
            addAndMakeVisible (tooltipsLabel);

			hotkeysButton.setClickingTogglesState(true);
            hotkeysButton.getToggleStateValue().referTo(owner.getHotkeysEnabled());
			addAndMakeVisible(hotkeysButton);
			addAndMakeVisible(hotkeysLabel);
            
            memoryMappingButton.setClickingTogglesState(true);
            memoryMappingButton.getToggleStateValue().referTo(owner.getMemoryMappingEnabled());
            addAndMakeVisible(memoryMappingButton);
            addAndMakeVisible(memoryMappingLabel);
        }
        
        void paint (Graphics& g) override
        {
            g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        }
        
        void resized() override
        {
            auto r = getLocalBounds();
            
            r.reduce(12, 12);
            searchPathEditor.setBounds(r.removeFromTop(48));
            memoryMappingButton.setBounds(r.removeFromTop(24));
            hotkeysButton.setBounds(r.removeFromTop(24));
            tooltipsButton.setBounds (r.removeFromTop(24));
        }
        
        void textEditorTextChanged(TextEditor& e) override
        {
            if (&e == &searchPathEditor)
            {

            }
        }
        
    private:
        //==============================================================================
        BKAudioProcessorEditor& owner;
        
        TextEditor searchPathEditor;
        
        Label tooltipsLabel;
		Label hotkeysLabel;
        Label memoryMappingLabel;
        ToggleButton tooltipsButton;
		ToggleButton hotkeysButton;
        ToggleButton memoryMappingButton;

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
