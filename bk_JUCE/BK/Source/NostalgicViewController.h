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

#include "BKListener.h"
#include "BKComponent.h"



//==============================================================================
/*
*/
class NostalgicViewController    : public BKComponent, public BKListener
{
public:
    NostalgicViewController(BKAudioProcessor&);
    ~NostalgicViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    void updateFields(void);

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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicViewController)
};


#endif  // NOSTALGICVIEWCONTROLLER_H_INCLUDED
