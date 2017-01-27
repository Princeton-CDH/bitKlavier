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
class PreparationMapViewController    : public BKViewController, public ActionBroadcaster, public ActionListener, public ReferenceCountedObject, public TextButton::Listener

{
public:
    typedef ReferenceCountedObjectPtr<PreparationMapViewController>   Ptr;
    typedef Array<PreparationMapViewController::Ptr>                  PtrArr;
    typedef Array<PreparationMapViewController::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<PreparationMapViewController>                  Arr;
    typedef OwnedArray<PreparationMapViewController, CriticalSection> CSArr;
    
    PreparationMapViewController(BKAudioProcessor&, int Id);
    ~PreparationMapViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    int Id;
    
    BKAudioProcessor& processor;
    
    OwnedArray<BKComboBox> prepMapCB;
    
    // BKLabels
    OwnedArray<BKLabel> prepMapL;
    OwnedArray<BKTextField> prepMapTF;
    
    String processPreparationString(String s);
    
    void textFieldDidChange(TextEditor&) override;
    void comboBoxDidChange (ComboBox* box) override;
    
    void switchToPrepMap(BKPreparationType type, int prepMap);
    
    void updateFields(void);
    
    void addPreparation(BKPreparationType type, int which);
    
    void actionListenerCallback (const String& message) override;
    
    void buttonClicked (Button* b) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreparationMapViewController)
};


#endif  // PreparationMapViewController_H_INCLUDED
