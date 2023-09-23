/*
  ==============================================================================

    GeneralViewController.h
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"
#include "EqualizerGraph.h"
#include "BKCompressorView.h"
class CommentViewController :
public BKViewController
{
public:
    CommentViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~CommentViewController();
    
    void paint (Graphics&) override;
    
    void resized() override;
    
    void update();
    
private:
    
    TextEditor comment;
    TextButton ok, cancel;
    
    void bkTextFieldDidChange       (TextEditor&)               override;
    
    void bkComboBoxDidChange        (ComboBox* box)             override {};
    void bkButtonClicked            (Button* b)                 override;
    void bkMessageReceived          (const String& message)     override {};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CommentViewController)
};

class AboutViewController :
public BKViewController
{
public:
    AboutViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~AboutViewController();
    
    void paint (Graphics&) override;
    void resized() override;
    
private:
    
    Image image;
    RectanglePlacement placement;
    Rectangle<float> imageRect;
    
    BKTextEditor about;

    void bkTextFieldDidChange       (TextEditor&)               override {};
    void bkComboBoxDidChange        (ComboBox* box)             override {};
    void bkButtonClicked            (Button* b)                 override;
    void bkMessageReceived          (const String& message)     override {};
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutViewController)
};

//==============================================================================
/*
*/
class GeneralViewController :
public BKViewController,
public BKSingleSlider::Listener
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    GeneralViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~GeneralViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    void update(void);
    
#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
    
    inline void setTab(int tab) {
        currentTab = tab;
        displayTab(currentTab);
    }

private:
    void displayTab(int tab) override;
    void displayShared(void) override;
    void invisible(void) override;
    
    enum Tabs {
        settings,
        equalizer,
        compressor
    };
    
    int arrowSpace = 50; // amount of space needed for arrows
    
    BKAudioProcessor& processor;
    int currentNostalgicLayer;
    BKCompressorView compressorView;
    // Settings UI components
    std::unique_ptr<BKSingleSlider> pedalGainSlider;
    ToggleButton invertSustainB;
    ToggleButton noteOnSetsNoteOffVelocityB;
    std::unique_ptr<BKSingleSlider> A4tuningReferenceFrequencySlider;
    std::unique_ptr<BKSingleSlider> tempoMultiplierSlider;
    
    // Equalizer UI components
    TextButton lowCutButton;
    TextButton peak1Button;
    TextButton peak2Button;
    TextButton peak3Button;
    TextButton highCutButton;
    
    ImageComponent lowCutIcon;
    ImageComponent highCutIcon;
    ImageComponent peak1Icon;
    ImageComponent peak2Icon;
    ImageComponent peak3Icon;
    
    ToggleButton bypassToggle;
    TextButton resetToDefaultButton;
    
    std::unique_ptr<BKSingleSlider> lowCutFreqSlider;
    GroupComponent lowCutSlopeBorder;
    ToggleButton lowCutSlope12;
    ToggleButton lowCutSlope24;
    ToggleButton lowCutSlope36;
    ToggleButton lowCutSlope48;
    ToggleButton lowCutBypass;
    TextButton lowCutReset;
    
    std::unique_ptr<BKSingleSlider> peak1FreqSlider;
    std::unique_ptr<BKSingleSlider> peak1GainSlider;
    std::unique_ptr<BKSingleSlider> peak1QualitySlider;
    ToggleButton peak1Bypass;
    TextButton peak1Reset;
    
    std::unique_ptr<BKSingleSlider> peak2FreqSlider;
    std::unique_ptr<BKSingleSlider> peak2GainSlider;
    std::unique_ptr<BKSingleSlider> peak2QualitySlider;
    ToggleButton peak2Bypass;
    TextButton peak2Reset;
    
    std::unique_ptr<BKSingleSlider> peak3FreqSlider;
    std::unique_ptr<BKSingleSlider> peak3GainSlider;
    std::unique_ptr<BKSingleSlider> peak3QualitySlider;
    ToggleButton peak3Bypass;
    TextButton peak3Reset;
    
    std::unique_ptr<BKSingleSlider> highCutFreqSlider;
    GroupComponent highCutSlopeBorder;
    ToggleButton highCutSlope12;
    ToggleButton highCutSlope24;
    ToggleButton highCutSlope36;
    ToggleButton highCutSlope48;
    ToggleButton highCutBypass;
    TextButton highCutReset;
    
    enum RadioButtonID {
        lowCutButtons = 1001,
        highCutButtons = 1002
    };
    
    enum Filters {
        lowCut,
        peak1,
        peak2,
        peak3,
        highCut
    };
    
    int currentFilter = Filters::lowCut; // like currentTab but for filter
    void displayFilter(int filter);
    void invisibleFilters();
    void clearColors();
    
    EqualizerGraph eqGraph;
    
    void bkTextFieldDidChange       (TextEditor&)               override;
    void bkComboBoxDidChange        (ComboBox* box)             override { };
    void bkButtonClicked            (Button* b)                 override;
    void bkMessageReceived          (const String& message)     override { };
    void BKSingleSliderValueChanged (BKSingleSlider* slider, String name, double val)   override;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneralViewController)
};


class ModdableViewController :
public BKViewController,
public BKSingleSlider::Listener,
public Timer
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    ModdableViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~ModdableViewController();
    
    void paint (Graphics&) override;
    
    void resized() override;
    void timerCallback() override;
    
    void update();
    
#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
    
private:
    
    
    
    std::unique_ptr<BKSingleSlider> timeSlider;
    std::unique_ptr<BKSingleSlider> incSlider;
    std::unique_ptr<BKSingleSlider> maxIncSlider;
    
    void bkTextFieldDidChange       (TextEditor&)               override;
    void bkComboBoxDidChange        (ComboBox* box)             override {};
    void bkButtonClicked            (Button* b)                 override;
    void bkMessageReceived          (const String& message)     override {};
    void BKSingleSliderValueChanged (BKSingleSlider* slider, String name, double val)   override;
    
    ModdableBase* getCurrentModdable();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModdableViewController)
};
