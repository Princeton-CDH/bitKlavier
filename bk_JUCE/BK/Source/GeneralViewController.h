/*
  ==============================================================================

    GeneralViewController.h
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef GeneralViewController_H_INCLUDED
#define GeneralViewController_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "General.h"

#include "BKViewController.h"

#include "BKTextField.h"

#include "BKLabel.h"

//==============================================================================
/*
*/
class GeneralViewController    : public BKViewController
{
public:
    GeneralViewController(BKAudioProcessor&);
    ~GeneralViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    BKAudioProcessor& processor;
    int currentNostalgicLayer;
    
    // BKLabels
    OwnedArray<BKLabel> generalL;

    OwnedArray<BKTextField> generalTF;
    
    void textFieldDidChange(TextEditor&) override;
    
    void updateFields(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneralViewController)
};


#endif  // GeneralViewController_H_INCLUDED
