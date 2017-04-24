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

#include "BKListener.h"
#include "BKComponent.h"

#include "BKSlider.h"

#include "BKGraph.h"

//==============================================================================
/*
*/

class SynchronicViewController    : public BKComponent, public BKListener, public BKMultiSliderListener
{
public:
    SynchronicViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~SynchronicViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    void updateModFields(void);
    void updateFields(void);
    
private:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    OwnedArray<BKLabel> synchronicL;
    OwnedArray<BKTextField> synchronicTF;
    OwnedArray<BKTextField> modSynchronicTF;
    
    BKMultiSlider* sliderTest;
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override { };
    void bkButtonClicked            (Button* b)             override { };
    
    void multiSliderValueChanged(String name, int whichSlider, Array<float> values) override;
    void multiSliderAllValuesChanged(String name, Array<Array<float>> values) override { };
    
     void fillSelectCB(void);
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchronicViewController)
};


#endif  // SYNCHRONICVIEWCONTROLLER_H_INCLUDED
