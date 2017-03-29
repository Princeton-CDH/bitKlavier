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
#include "BKListener.h"
#include "BKComponent.h"

//==============================================================================
/*
*/
class TuningViewController    : public BKComponent, public BKListener
{
public:
    TuningViewController(BKAudioProcessor&);
    ~TuningViewController();

    void paint (Graphics&) override;
    void resized() override;

    void updateModFields(void);
    void updateFields(void);
    
private:
    
    BKAudioProcessor& processor;
    int currentTuningId, currentModTuningId;
    
    OwnedArray<BKLabel> tuningL;
    OwnedArray<BKTextField> tuningTF;
    OwnedArray<BKTextField> modTuningTF;
    
    
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override { };
    void bkButtonClicked            (Button* b)             override { };
    
     void fillSelectCB(void);
  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningViewController)
};


#endif  // TUNINGVIEWCONTROLLER_H_INCLUDED
