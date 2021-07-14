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
    
    void showGenSettings(int tab);
    
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
        defaultPathLabel("Piano (Litest-Heavy) search path:", "Piano (Litest-Heavy) search path:"),
        defaultPathButton("Set search path"),
        soundfontPathLabel("Soundfont search paths:", "Soundfont search paths:"),
        soundfontPathButton("Add search path"),
        customPathLabel("Sample search paths:", "Sample search paths:"),
        customPathButton("Add search path"),
        tooltipsLabel("Enable tooltips", "Enable tooltips"),
		hotkeysLabel("Enable hotkeys", "Enable hotkeys"),
        memoryMappingLabel("Enable direct-from-disk sample playback", "Enable direct-from-disk sample playback")
        {
            setOpaque (true);
            setWantsKeyboardFocus(true);
            
            //==============================================================================
            
            defaultPathLabel.setTooltip("Set the path to the default bitKlavier samples.");
            defaultPathLabel.setJustificationType(Justification::topLeft);
            addAndMakeVisible(defaultPathLabel);
            
            defaultPathButton.setTooltip("Open the file browser to select a folder.");
            defaultPathButton.addListener(this);
            addAndMakeVisible(defaultPathButton);
            
            defaultPathEditor.setTooltip("Set the path to the default bitKlavier samples.");
            defaultPathEditor.setMultiLine(true);
            defaultPathEditor.setText(owner.processor.defaultSamplesPath.getFullPathName(), dontSendNotification);
            defaultPathEditor.addListener(this);
            addAndMakeVisible(defaultPathEditor);
            
            //==============================================================================
            
            soundfontPathLabel.setTooltip("Set paths in which to search for soundfonts.");
            soundfontPathLabel.setJustificationType(Justification::topLeft);
            addAndMakeVisible(soundfontPathLabel);
            
            soundfontPathButton.setTooltip("Open the file browser to select folders to add to the search paths list.");
            soundfontPathButton.addListener(this);
            addAndMakeVisible(soundfontPathButton);
            
            soundfontPathEditor.setTooltip("Set paths in which to search for soundfonts.");
            soundfontPathEditor.setMultiLine(true);
            String text = owner.processor.soundfontsPaths.toString().replace(";", "; ");
            soundfontPathEditor.setText(text, dontSendNotification);
            soundfontPathEditor.addListener(this);
            addAndMakeVisible(soundfontPathEditor);
            
            //==============================================================================
            
            customPathLabel.setTooltip("Set paths in which to search for additional sample libraries. bitKlavier only look for .wav files with the naming format of \"C4v1\".");
            customPathLabel.setJustificationType(Justification::topLeft);
            addAndMakeVisible(customPathLabel);
            
            customPathButton.setTooltip("Open the file browser to select folders to add to the search paths list.");
            customPathButton.addListener(this);
            addAndMakeVisible(customPathButton);
            
            customPathEditor.setTooltip("Set paths in which to search for sample libraries. bitKlavier only looks for .wav files with the naming format of \"C4v1\".");
            customPathEditor.setMultiLine(true);
            text = owner.processor.customSamplesPaths.toString().replace(";", "; ");
            customPathEditor.setText(text, dontSendNotification);
            customPathEditor.addListener(this);
            addAndMakeVisible(customPathEditor);
            
            //==============================================================================
            
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
            defaultPathLabel.setBounds(slice.removeFromLeft(slice.getWidth()*0.6));
            defaultPathButton.setBounds(slice.withHeight(25));
            defaultPathEditor.setBounds(r.removeFromTop(24));
            r.removeFromTop(24);
            
            slice = r.removeFromTop(24);
            soundfontPathLabel.setBounds(slice.removeFromLeft(slice.getWidth()*0.6));
            soundfontPathButton.setBounds(slice.withHeight(25));
            soundfontPathEditor.setBounds(r.removeFromTop(72));
            r.removeFromTop(24);
            
            slice = r.removeFromTop(24);
            customPathLabel.setBounds(slice.removeFromLeft(slice.getWidth()*0.6));
            customPathButton.setBounds(slice.withHeight(25));
            customPathEditor.setBounds(r.removeFromTop(72));
            
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
            if (b == &defaultPathButton)
            {
                fc = std::make_unique<FileChooser> ("Select folders...",
                                                    File::getSpecialLocation (File::userHomeDirectory));
                
                fc->launchAsync (FileBrowserComponent::openMode |
                                 FileBrowserComponent::canSelectDirectories,
                                 [this] (const FileChooser& chooser)
                                 {
                    owner.processor.defaultSamplesPath = chooser.getResult();
                    defaultPathEditor.setText(owner.processor.defaultSamplesPath.getFullPathName());
                });
            }
            if (b == &soundfontPathButton)
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
                        owner.processor.soundfontsPaths.addIfNotAlreadyThere(result);
                    }
                    updateSoundfontsPaths();
                });
            }
            if (b == &customPathButton)
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
                        owner.processor.customSamplesPaths.addIfNotAlreadyThere(result);
                    }
                    updateCustomSamplesPaths();
                });
            }
        }
        
        void textEditorReturnKeyPressed(TextEditor& e) override
        {
            unfocusAllComponents();
        }
        
        void textEditorEscapeKeyPressed(TextEditor& e) override
        {
            unfocusAllComponents();
        }
        
        void textEditorFocusLost(TextEditor& e) override
        {
            if (&e == &defaultPathEditor)
            {
                owner.processor.defaultSamplesPath = e.getText();
                defaultPathEditor.setText(owner.processor.defaultSamplesPath.getFullPathName());
            }
            if (&e == &soundfontPathEditor)
            {
                owner.processor.soundfontsPaths = e.getText();
                updateSoundfontsPaths();
            }
            if (&e == &customPathEditor)
            {
                owner.processor.customSamplesPaths = e.getText();
                updateCustomSamplesPaths();
            }
        }
        
        void updateSoundfontsPaths()
        {
            owner.processor.soundfontsPaths.removeRedundantPaths();
            owner.processor.soundfontsPaths.removeNonExistentPaths();
            String text = owner.processor.soundfontsPaths.toString().replace(";", "; ");
            soundfontPathEditor.setText(text, dontSendNotification);
            owner.processor.collectSoundfonts();
        }
        
        void updateCustomSamplesPaths()
        {
            owner.processor.customSamplesPaths.removeRedundantPaths();
            owner.processor.customSamplesPaths.removeNonExistentPaths();
            String text = owner.processor.customSamplesPaths.toString().replace(";", "; ");
            customPathEditor.setText(text, dontSendNotification);
            owner.processor.collectCustomSamples();
        }
        
    private:
        //==============================================================================
        BKAudioProcessorEditor& owner;
        
        std::unique_ptr<FileChooser> fc;
        
        Label defaultPathLabel;
        TextButton defaultPathButton;
        TextEditor defaultPathEditor;
        
        Label soundfontPathLabel;
        TextButton soundfontPathButton;
        TextEditor soundfontPathEditor;
        
        Label customPathLabel;
        TextButton customPathButton;
        TextEditor customPathEditor;
        
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
    
//    class EqualizerComponent : public Component
//    {
//    public:
//        EqualizerComponent(BKAudioProcessorEditor& editor) : owner (editor) {
//            //setOpaque(true);
//            //setWantsKeyboardFocus(true);
//        }
//
//        void paint (Graphics& g) override {
//            g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
//        }
//
//        void resized() override {
//
//        }
//        
//    private:
//        BKAudioProcessorEditor& owner;
//
//        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizerComponent)
//    };
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessorEditor)
};




#endif  // PLUGINEDITOR_H_INCLUDED
