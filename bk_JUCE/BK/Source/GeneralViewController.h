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

#include "BKListener.h"
#include "BKComponent.h"

//==============================================================================
/*
*/
class GeneralViewController    : public BKComponent, public BKListener
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
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkComboBoxDidChange        (ComboBox* box)         override { };
    void bkButtonClicked            (Button* b)             override { };
    void bkMessageReceived          (const String& message) override { };
    
    void updateFields(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneralViewController)
};


#endif  // GeneralViewController_H_INCLUDED
