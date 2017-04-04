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

#include "BKListener.h"
#include "BKComponent.h"

#include "Direct.h"

//==============================================================================
/*
*/
class DirectViewController    : public BKComponent, public BKListener
{
public:
    DirectViewController(BKAudioProcessor&);
    ~DirectViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    void reset(void);
    void updateFields(void);
    void updateModFields(void);
    
private:
    BKAudioProcessor& processor;
    
    BKLabel selectL;
    BKComboBox selectCB;
    BKComboBox modSelectCB;
    
    BKLabel nameL;
    BKTextField nameTF;
    BKTextField modNameTF;
    
    OwnedArray<BKLabel> directL;
    OwnedArray<BKTextField> directTF;
    OwnedArray<BKTextField> modDirectTF;
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override { };
    
    void fillSelectCB(void);
    void fillModSelectCB(void);
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectViewController)
};


#endif  // DIRECTVIEWCONTROLLER_H_INCLUDED
