/*
  ==============================================================================

    NostalgicViewController.h
    Created: 30 Nov 2016 9:43:47am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef NOSTALGICVIEWCONTROLLER_H_INCLUDED
#define NOSTALGICVIEWCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

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
    int currentNostalgicLayer;
    
    // BKLabels
    BKLabel nNumLayersL;
    BKLabel nCurrentLayerL;
    BKLabel nKeymapL;
    OwnedArray<BKLabel> nostalgicL;
    
    // Text Fields
    BKTextField nNumLayersTF;
    BKTextField nCurrentLayerTF;
    BKTextField nKeymapTF;
    OwnedArray<BKTextField> nostalgicTF;
    
    void textFieldDidChange(TextEditor&) override;
    
    void updateFieldsToLayer(int numLayer);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicViewController)
};


#endif  // NOSTALGICVIEWCONTROLLER_H_INCLUDED
