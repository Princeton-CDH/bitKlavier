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
        equalizer
    };
    
    int arrowSpace = 50; // amount of space needed for arrows
    
    BKAudioProcessor& processor;
    int currentNostalgicLayer;
    
    // Settings UI components
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
    
    std::unique_ptr<BKSingleSlider> lowCutFreqSlider;
    std::unique_ptr<BKSingleSlider> lowCutSlopeSlider;
    
    std::unique_ptr<BKSingleSlider> peak1FreqSlider;
    std::unique_ptr<BKSingleSlider> peak1GainSlider;
    std::unique_ptr<BKSingleSlider> peak1QualitySlider;
    
    std::unique_ptr<BKSingleSlider> peak2FreqSlider;
    std::unique_ptr<BKSingleSlider> peak2GainSlider;
    std::unique_ptr<BKSingleSlider> peak2QualitySlider;
    
    std::unique_ptr<BKSingleSlider> peak3FreqSlider;
    std::unique_ptr<BKSingleSlider> peak3GainSlider;
    std::unique_ptr<BKSingleSlider> peak3QualitySlider;
    
    std::unique_ptr<BKSingleSlider> highCutFreqSlider;
    std::unique_ptr<BKSingleSlider> highCutSlopeSlider;
    
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
