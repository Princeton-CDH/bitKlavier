/*
  ==============================================================================

    NostalgicViewController.h
    Created: 30 Nov 2016 9:43:47am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef NOSTALGICVIEWCONTROLLER_H_INCLUDED
#define NOSTALGICVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKTextField.h"

#include "BKLabel.h"


//==============================================================================
/*
*/
class NostalgicViewController    : public BKViewController
{
public:
    NostalgicViewController(BKAudioProcessor&);
    ~NostalgicViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    BKAudioProcessor& processor;
    int currentNostalgicId;
    
    // BKLabels
    OwnedArray<BKLabel> nostalgicL;
    OwnedArray<BKTextField> nostalgicTF;
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override { };
    void bkButtonClicked            (Button* b)             override { };
    
    void updateFields(int nostalgicId);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicViewController)
};


#endif  // NOSTALGICVIEWCONTROLLER_H_INCLUDED
