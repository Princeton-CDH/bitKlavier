/*
  ==============================================================================

    GeneralViewController.h
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"

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

private:
    //BKAudioProcessor& processor;
    int currentNostalgicLayer;
    
    ToggleButton invertSustainB;
    BKLabel     invertSustainL;
    
    ToggleButton noteOnSetsNoteOffVelocityB;
    BKLabel     noteOnSetsNoteOffVelocityL;
    
    std::unique_ptr<BKSingleSlider> A4tuningReferenceFrequencySlider; //A440
    std::unique_ptr<BKSingleSlider> tempoMultiplierSlider;
    
    
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
