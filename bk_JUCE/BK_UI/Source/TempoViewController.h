/*
  ==============================================================================

    TempoViewController.h
    Created: 5 Mar 2017 9:28:27am
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef TEMPOVIEWCONTROLLER_H_INCLUDED
#define TEMPOVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"
#include "PluginProcessor.h"
#include "BKListener.h"
#include "BKComponent.h"

#include "BKGraph.h"

class TempoViewController    : public BKComponent, public BKListener
{
public:
    TempoViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TempoViewController();
    
    void paint (Graphics&) override;
    void resized() override;
    
    void updateModFields(void);
    void updateFields(void);
    
private:
    
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    OwnedArray<BKLabel> tempoL;
    OwnedArray<BKTextField> tempoTF;
    OwnedArray<BKTextField> modTempoTF;
    
    
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override { };
    void bkButtonClicked            (Button* b)             override { };
    
     void fillSelectCB(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoViewController)
};


#endif  // TEMPOVIEWCONTROLLER_H_INCLUDED
