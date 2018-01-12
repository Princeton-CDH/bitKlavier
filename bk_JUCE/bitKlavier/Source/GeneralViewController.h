/*
  ==============================================================================

    GeneralViewController.h
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"

//==============================================================================
/*
*/
class GeneralViewController :
public BKViewController,
public BKSingleSlider::Listener
{
public:
    GeneralViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~GeneralViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    void update(void);

private:
    //BKAudioProcessor& processor;
    int currentNostalgicLayer;
    
    // BKLabels
    OwnedArray<BKLabel> generalL;
    OwnedArray<BKTextField> generalTF;
    
    ScopedPointer<BKSingleSlider> A4tuningReferenceFrequencySlider; //A440
    ScopedPointer<BKSingleSlider> tempoMultiplierSlider;
    
    
    void bkTextFieldDidChange       (TextEditor&)               override;
    void bkComboBoxDidChange        (ComboBox* box)             override { };
    void bkButtonClicked            (Button* b)                 override;
    void bkMessageReceived          (const String& message)     override { };
    void BKSingleSliderValueChanged (String name, double val)   override;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneralViewController)
};

