/*
  ==============================================================================

    DirectViewController.h
    Created: 30 Nov 2016 5:00:19pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef DIRECTVIEWCONTROLLER_H_INCLUDED
#define DIRECTVIEWCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKTextField.h"

#include "BKLabel.h"

//==============================================================================
/*
*/
class DirectViewController    : public BKViewController
{
public:
    DirectViewController(BKAudioProcessor&);
    ~DirectViewController();

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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectViewController)
};


#endif  // DIRECTVIEWCONTROLLER_H_INCLUDED
