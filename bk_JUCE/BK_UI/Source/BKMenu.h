/*
  ==============================================================================

    BKMenu.h
    Created: 9 Dec 2016 3:31:21pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKMENU_H_INCLUDED
#define BKMENU_H_INCLUDED

#include "BKLookAndFeel.h"
#include "BKUtilities.h"
#include "BKComponent.h"

//==============================================================================
/*
*/
class BKComboBox : public ComboBox
{
public:
    BKComboBox() :
    thisJustification (Justification::centredLeft)
    {
        setSize(200,20);
        
        //lookAndFeelChanged();
        setLookAndFeel(&thisLAF);
        
    }
    
    ~BKComboBox()
    {
    }
    
    void BKSetJustificationType (Justification justification)
    {
        thisJustification = justification;
        thisLAF.setComboBoxJustificationType(justification);
    }


private:
    
    BKButtonAndMenuLAF thisLAF;
    Justification thisJustification;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKComboBox)
};


class BKEditableComboBoxListener;

class BKEditableComboBox :
public ComboBox,
public TextEditor::Listener
{
public:
    
    BKEditableComboBox() :
    thisJustification (Justification::centredLeft)
    {
        setSize(200,20);
        
        //lookAndFeelChanged();
        setLookAndFeel(&thisLAF);

        nameEditor.setName("NAMETXTEDIT");
        addAndMakeVisible(nameEditor);
        nameEditor.toBack();
        nameEditor.setAlpha(0.);
        nameEditor.setOpaque(true);
        nameEditor.addListener(this);
        
        lastItemId = 0;

    }
    
    ~BKEditableComboBox()
    {
    }
    
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost (TextEditor& textEditor) override;
    void mouseDoubleClick(const MouseEvent& e) override;
    void textEditorEscapeKeyPressed (TextEditor &) override;
    
    void mouseDown(const MouseEvent& e) override
    {
        DBG("editable combobox mousedown");
        ComboBox::mouseDown(e);
    }
    
    void focusLost(FocusChangeType cause) override
    {
        DBG("editable combobox FOCUS LOST");
        //Component::prevFocused = nullptr;
        //getTopLevelComponent()->unfocusAllComponents();
        //Component::modalStateFinished(0);
        //exitModalState(-1);
        //hidePopup();
    }

    
    ListenerList<BKEditableComboBoxListener> listeners;
    void addMyListener(BKEditableComboBoxListener* listener)     { listeners.add(listener);      }
    void removeMyListener(BKEditableComboBoxListener* listener)  { listeners.remove(listener);   }
    
    void BKSetJustificationType (Justification justification)
    {
        thisJustification = justification;
        thisLAF.setComboBoxJustificationType(justification);
    }
    
private:
        
    BKTextField nameEditor;
    bool focusLostByEscapeKey;
    int lastItemId;
    
    BKButtonAndMenuLAF thisLAF;
    Justification thisJustification;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEditableComboBox)
};

class BKEditableComboBoxListener
{
    
public:
    
    virtual ~BKEditableComboBoxListener() {};
    
    virtual void BKEditableComboBoxChanged(String text, BKEditableComboBox*) = 0;
};



#endif  // BKMENU_H_INCLUDED
