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

#include "Preparation.h"


//==============================================================================
/*
*/
class PreparationMapViewController    : public BKComponent, public BKListener, public ReferenceCountedObject, public DragAndDropTarget

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
    
    void reset(void);

private:
    
    int Id;
    int keymapId;
    
    BKAudioProcessor& processor;
    
    bool somethingIsBeingDraggedOver;
    
    // BKLabels
    BKLabel             keymapSelectL;
    BKComboBox          keymapSelectCB;
    
    BKLabel             prepMapL;
    BKTextField         prepMapTF;
    
    String processPreparationString(String s);
    
    void switchToPrepMap(BKPreparationType type, int prepMap);
    
    void addPreparation(BKPreparationType type, int which);
    
    String addDirectPreparation(int prep);
    String addNostalgicPreparation(int prep);
    String addSynchronicPreparation(int prep);
    String addTempoPreparation(int prep);
    String addTuningPreparation(int prep);
    
    // BKListener interface
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override { };
    void bkMessageReceived          (const String& message) override { };
    
    // Drag interface
    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragEnter (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragMove (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragExit (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDropped (const SourceDetails& dragSourceDetails) override;
    
    void updateFields(void);
    void fillKeymapSelectCB(void);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreparationMapViewController)
};


#endif  // PreparationMapViewController_H_INCLUDED
