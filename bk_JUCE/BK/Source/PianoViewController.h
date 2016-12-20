/*
  ==============================================================================

    PianoViewController.h
    Created: 8 Dec 2016 12:54:41am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PIANOVIEWCONTROLLER_H_INCLUDED
#define PIANOVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKLabel.h"

#include "BKTextField.h"

#include "BKMenu.h"

#include "Piano.h"


//==============================================================================
/*
*/
class PianoViewController    : public BKViewController, public ActionBroadcaster, public ComboBox::Listener
{
public:
    PianoViewController(BKAudioProcessor& , Piano::Ptr);
    ~PianoViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    BKAudioProcessor& processor;
    
    Piano::Ptr current;
    
    OwnedArray<BKComboBox> pianoCB;
    
    // BKLabels
    OwnedArray<BKLabel> pianoL;
    OwnedArray<BKTextField> pianoTF;
    
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    
    void textFieldDidChange(TextEditor&) override;
    
    void switchToPiano(BKPreparationType type, int piano);
    
    void updateFields(void);
    
    void addPreparation(BKPreparationType type, int which);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoViewController)
};


#endif  // PIANOVIEWCONTROLLER_H_INCLUDED
