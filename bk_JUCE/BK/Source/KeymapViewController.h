/*
  ==============================================================================

    KeymapViewController.h
    Created: 9 Dec 2016 11:57:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef KEYMAPVIEWCONTROLLER_H_INCLUDED
#define KEYMAPVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKLabel.h"

#include "BKTextField.h"

//==============================================================================
/*
*/
class KeymapViewController    : public BKViewController, public ActionListener
{
public:
    KeymapViewController(BKAudioProcessor&);
    ~KeymapViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    int currentKeymapId;
    
    BKAudioProcessor& processor;
    
    OwnedArray<BKLabel> keymapL;
    OwnedArray<BKTextField> keymapTF;
    
    void updateFields(int keymapId);
    
    void textFieldDidChange(TextEditor&) override;
    
    void actionListenerCallback (const String& message) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeymapViewController)
};


#endif  // KEYMAPVIEWCONTROLLER_H_INCLUDED
