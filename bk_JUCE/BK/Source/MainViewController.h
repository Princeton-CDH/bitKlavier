/*
  ==============================================================================

    MainViewController.h
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef MAINVIEWCONTROLLER_H_INCLUDED
#define MAINVIEWCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKTextField.h"

#include "BKLabel.h"

//==============================================================================
/*
*/
class MainViewController    : public BKViewController
{
public:
    MainViewController(BKAudioProcessor&);
    ~MainViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    BKAudioProcessor& processor;
    int currentNostalgicLayer;
    
    // BKLabels
    OwnedArray<BKLabel> mainL;

    OwnedArray<BKTextField> mainTF;
    
    void textFieldDidChange(TextEditor&) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewController)
};


#endif  // MAINVIEWCONTROLLER_H_INCLUDED
