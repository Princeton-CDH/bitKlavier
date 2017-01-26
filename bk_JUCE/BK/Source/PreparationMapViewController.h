/*
  ==============================================================================

    PreparationMapViewController.h
    Created: 8 Dec 2016 12:54:41am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PreparationMapViewController_H_INCLUDED
#define PreparationMapViewController_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKViewController.h"

#include "BKLabel.h"

#include "BKTextField.h"

#include "BKMenu.h"

#include "PreparationMap.h"


//==============================================================================
/*
*/
class PreparationMapViewController    : public BKViewController, public ActionBroadcaster, public ComboBox::Listener
{
public:
    PreparationMapViewController(BKAudioProcessor&);
    ~PreparationMapViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    BKAudioProcessor& processor;
    
    OwnedArray<BKComboBox> prepMapCB;
    
    // BKLabels
    OwnedArray<BKLabel> prepMapL;
    OwnedArray<BKTextField> prepMapTF;
    
    String processPreparationString(String s);
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    
    void textFieldDidChange(TextEditor&) override;
    
    void switchToPrepMap(BKPreparationType type, int prepMap);
    
    void updateFields(void);
    
    void addPreparation(BKPreparationType type, int which);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreparationMapViewController)
};


#endif  // PreparationMapViewController_H_INCLUDED
