/*
  ==============================================================================

    DirectViewController.h
    Created: 30 Nov 2016 5:00:19pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef DIRECTVIEWCONTROLLER_H_INCLUDED
#define DIRECTVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKTextField.h"

#include "BKLabel.h"

#include "Direct.h"

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
    int currentDirectId;
    
    OwnedArray<BKLabel> directL;
    OwnedArray<BKTextField> directTF;
    
    void updateFields(int directId);
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override { };
    void bkButtonClicked            (Button* b)             override { };
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectViewController)
};


#endif  // DIRECTVIEWCONTROLLER_H_INCLUDED
