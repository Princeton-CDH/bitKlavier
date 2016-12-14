/*
  ==============================================================================

    TuningViewController.h
    Created: 14 Dec 2016 12:25:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef TUNINGVIEWCONTROLLER_H_INCLUDED
#define TUNINGVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKTextField.h"

#include "BKLabel.h"

//==============================================================================
/*
*/
class TuningViewController    : public BKViewController, public ActionListener
{
public:
    TuningViewController(BKAudioProcessor&);
    ~TuningViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    BKAudioProcessor& processor;
    int currentTuningId;
    
    OwnedArray<BKLabel> tuningL;
    OwnedArray<BKTextField> tuningTF;
    
    void updateFields(int tuningId);
    
    void textFieldDidChange(TextEditor&) override;
    
    void actionListenerCallback (const String& message) override;
  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningViewController)
};


#endif  // TUNINGVIEWCONTROLLER_H_INCLUDED
