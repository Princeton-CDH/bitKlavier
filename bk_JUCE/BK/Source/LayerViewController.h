/*
  ==============================================================================

    LayerViewController.h
    Created: 8 Dec 2016 12:54:41am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LAYERVIEWCONTROLLER_H_INCLUDED
#define LAYERVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKLabel.h"

#include "BKTextField.h"

#include "BKMenu.h"

#include "Layer.h"


//==============================================================================
/*
*/
class LayerViewController    : public BKViewController, public ActionBroadcaster, public ComboBox::Listener
{
public:
    LayerViewController(BKAudioProcessor& , Layer::Ptr);
    ~LayerViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    BKAudioProcessor& processor;
    
    Layer::Ptr current;
    
    OwnedArray<BKComboBox> layerCB;
    
    // BKLabels
    OwnedArray<BKLabel> layerL;
    OwnedArray<BKTextField> layerTF;
    
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    
    void textFieldDidChange(TextEditor&) override;
    
    void switchToLayer(BKPreparationType type, int layer);
    
    void updateFields(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayerViewController)
};


#endif  // LAYERVIEWCONTROLLER_H_INCLUDED
