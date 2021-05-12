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
                                 public Button::Listener,
                                 public TextEditor::Listener
    {
    public:
        PreferencesComponent (BKAudioProcessorEditor& editor)
        : owner (editor),
        searchPathLabel("Sample search paths:", "Sample search paths:"),
        pathAddButton("Add search path"),
        tooltipsLabel("Enable tooltips", "Enable tooltips"),
		hotkeysLabel("Enable hotkeys", "Enable hotkeys"),
        memoryMappingLabel("Enable direct-from-disk sample playback", "Enable direct-from-disk sample playback")
        {
            setOpaque (true);
            setWantsKeyboardFocus(true);
            
            searchPathLabel.setJustificationType(Justification::topLeft);
            addAndMakeVisible(searchPathLabel);
            pathAddButton.addListener(this);
            addAndMakeVisible(pathAddButton);
            
            searchPathEditor.setMultiLine(true);
            String text = owner.processor.sampleSearchPath.toString().replace(";", "; ");
            searchPathEditor.setText(text, dontSendNotification);
            searchPathEditor.addListener(this);
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
            Rectangle<int> slice = r.removeFromTop(24);
            searchPathLabel.setBounds(slice.removeFromLeft(slice.getWidth()*0.6));
            pathAddButton.setBounds(slice.withHeight(25));
            searchPathEditor.setBounds(r.removeFromTop(72));
            r.removeFromTop(24);
            slice = r.removeFromTop(24);
            hotkeysButton.setBounds(slice.removeFromLeft(24));
            hotkeysButton.changeWidthToFitText();
            hotkeysLabel.setBounds(slice);
            slice = r.removeFromTop(24);
            tooltipsButton.setBounds(slice.removeFromLeft(24));
            tooltipsButton.changeWidthToFitText();
            tooltipsLabel.setBounds(slice);
            slice = r.removeFromTop(24);
            memoryMappingButton.setBounds(slice.removeFromLeft(24));
            memoryMappingButton.changeWidthToFitText();
            memoryMappingLabel.setBounds(slice);
        }
        
        void buttonClicked(Button* b) override
        {
            if (b == &pathAddButton)
            {
                fc = std::make_unique<FileChooser> ("Add folders...",
                                                    File::getSpecialLocation (File::userHomeDirectory));
                
                fc->launchAsync (FileBrowserComponent::openMode |
                                 FileBrowserComponent::canSelectDirectories |
                                 FileBrowserComponent::canSelectMultipleItems,
                                 [this] (const FileChooser& chooser)
                                 {
                    for (auto result : chooser.getResults())
                    {
                        owner.processor.sampleSearchPath.addIfNotAlreadyThere(result);
                    }
                    updateSearchPath();
                });
            }
        }
        
        void textEditorReturnKeyPressed(TextEditor& e) override
        {
            if (&e == &searchPathEditor)
            {
                unfocusAllComponents();
            }
        }
        
        void textEditorEscapeKeyPressed(TextEditor& e) override
        {
            if (&e == &searchPathEditor)
            {
                unfocusAllComponents();
            }
        }
        
        void textEditorFocusLost(TextEditor& e) override
        {
            if (&e == &searchPathEditor)
            {
                owner.processor.sampleSearchPath = e.getText();
                updateSearchPath();
            }
        }
        
        void updateSearchPath()
        {
            owner.processor.sampleSearchPath.removeRedundantPaths();
            owner.processor.sampleSearchPath.removeNonExistentPaths();
            String text = owner.processor.sampleSearchPath.toString().replace(";", "; ");
            searchPathEditor.setText(text, dontSendNotification);
            owner.processor.collectCustomSamples();
        }
        
    private:
        //==============================================================================
        BKAudioProcessorEditor& owner;
        
        std::unique_ptr<FileChooser> fc;
        
        Label searchPathLabel;
        TextButton pathAddButton;
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
