/*
  ==============================================================================

    SynchronicView.h
    Created: 15 Nov 2016 4:02:15pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef SYNCHRONICVIEWCONTROLLER_H_INCLUDED
#define SYNCHRONICVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKTextField.h"

#include "BKLabel.h"

//==============================================================================
/*
*/
class SynchronicViewController    : public BKViewController, public ActionListener
{
public:
    SynchronicViewController(BKAudioProcessor&);
    ~SynchronicViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    
    
private:
    BKAudioProcessor& processor;
    int currentSynchronicId;
    
    OwnedArray<BKLabel> synchronicL;
    OwnedArray<BKTextField> synchronicTF;
    
    void textFieldDidChange(TextEditor&) override;
    
    void updateFields(int synchronicId);
    
    void actionListenerCallback (const String& message) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchronicViewController)
};


#endif  // SYNCHRONICVIEWCONTROLLER_H_INCLUDED
