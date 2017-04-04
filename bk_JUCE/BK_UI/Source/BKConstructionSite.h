/*
  ==============================================================================

    BKConstructionSite.h
    Created: 4 Apr 2017 5:46:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKCONSTRUCTIONSITE_H_INCLUDED
#define BKCONSTRUCTIONSITE_H_INCLUDED

#include "BKUtilities.h"

#include "BKListener.h"
#include "BKComponent.h"

class BKConstructionSite : public BKDraggableComponent, public DragAndDropTarget, public BKListener
{
public:
    BKConstructionSite();
    ~BKConstructionSite();

    void paint(Graphics& g) override;
    void resized(void) override;
    
private:
    
    // BKListener interface
    void bkTextFieldDidChange       (TextEditor&)           override { };
    void bkComboBoxDidChange        (ComboBox* box)         override { };
    void bkButtonClicked            (Button* b)             override { };
    void bkMessageReceived          (const String& message) override { };
    
    // Drag interface
    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragEnter (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragMove (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragExit (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDropped (const SourceDetails& dragSourceDetails) override;
    
    JUCE_LEAK_DETECTOR(BKConstructionSite)
};



#endif  // BKCONSTRUCTIONSITE_H_INCLUDED
