/*
  ==============================================================================

    PianoViewController.h
    Created: 27 Jan 2017 10:20:17am
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

//==============================================================================
/*
 */
class PianoViewController       : public BKViewController, public ActionBroadcaster
{
public:
    PianoViewController(BKAudioProcessor&);
    ~PianoViewController();
    
    void paint (Graphics&) override;
    void resized() override;
    
private:
    
    BKAudioProcessor& processor;
    
    // BKLabels
    OwnedArray<BKLabel> pianoL;
    OwnedArray<BKComboBox> pianoCB;
    
    String processPreparationString(String s);
    
    void textFieldDidChange(TextEditor&) override;
    void comboBoxDidChange (ComboBox* box) override;
    
    void switchToPrepMap(BKPreparationType type, int prepMap);
    
    void updateFields(void);
    
    void addPreparation(BKPreparationType type, int which);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoViewController)
};


#endif  // PIANOVIEWCONTROLLER_H_INCLUDED
