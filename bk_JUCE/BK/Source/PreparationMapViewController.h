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

#include "BKListener.h"
#include "BKComponent.h"

#include "PreparationMap.h"


//==============================================================================
/*
*/
class PreparationMapViewController    : public BKComponent, public BKListener, public ReferenceCountedObject

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
    
    void switchToPrepMap(BKPreparationType type, int prepMap);
    
    void addPreparation(BKPreparationType type, int which);
    
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override { };
    void bkButtonClicked            (Button* b)             override { };
    void bkMessageReceived          (const String& message) override { };
    
    void updateFields(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreparationMapViewController)
};


#endif  // PreparationMapViewController_H_INCLUDED
