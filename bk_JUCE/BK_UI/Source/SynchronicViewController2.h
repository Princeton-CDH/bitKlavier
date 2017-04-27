/*
  ==============================================================================

    SynchronicViewController2.h
    Created: 21 Apr 2017 11:17:47pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef SYNCHRONICVIEWCONTROLLER2_H_INCLUDED
#define SYNCHRONICVIEWCONTROLLER2_H_INCLUDED

#include "BKUtilities.h"
#include "PluginProcessor.h"
#include "BKListener.h"
#include "BKComponent.h"
#include "BKSlider.h"
#include "BKGraph.h"


class SynchronicViewController2    : public BKComponent, public BKListener, public BKMultiSliderListener
{
public:
    SynchronicViewController2(BKAudioProcessor&, BKItemGraph* theGraph);
    ~SynchronicViewController2();
    
    void paint (Graphics&) override;
    void resized() override;
    
    //void updateModFields(void);
    //void updateFields(void);
    
    void updateFields();
    
private:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    OwnedArray<BKLabel> synchronicL;
    OwnedArray<BKTextField> synchronicTF;
    OwnedArray<BKTextField> modSynchronicTF;
    
    OwnedArray<BKMultiSlider> paramSliders;
    
    BKComboBox selectCB;

    void bkTextFieldDidChange       (TextEditor&)           override { };
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override { };
    
    void multiSliderValueChanged(String name, int whichSlider, Array<float> values) override;
    void multiSliderAllValuesChanged(String name, Array<Array<float>> values) override;
    
    void fillSelectCB(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchronicViewController2)
};



#endif  // SYNCHRONICVIEWCONTROLLER2_H_INCLUDED
