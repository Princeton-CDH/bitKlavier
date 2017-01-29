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
class PianoViewController       : public BKViewController
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
    
    void bkComboBoxDidChange        (ComboBox* box)         override;
    
    void bkTextFieldDidChange       (TextEditor&)           override { };
    void bkButtonClicked            (Button* b)             override { };
    void bkMessageReceived          (const String& message) override { };
    
    void updateFields(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoViewController)
};


#endif  // PIANOVIEWCONTROLLER_H_INCLUDED
