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
    BKComboBox()
    {
        setSize(200,20);
        
        lookAndFeelChanged();
    }
    
    ~BKComboBox()
    {
    }


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKComboBox)
};


class BKEditableComboBoxListener
{
    
public:
    
    //BKSingleSliderListener() {}
    virtual ~BKEditableComboBoxListener() {};
    
    virtual void BKEditableComboBoxChanged(String name, int index) = 0;
};


class BKEditableComboBox :
public ComboBox,
public TextEditor::Listener
{
public:
    
    BKEditableComboBox()
    {
        setSize(200,20);
        
        lookAndFeelChanged();
        
        nameEditor.setName("NAMETXTEDIT");
        nameEditor.addListener(this);
    }
    
    ~BKEditableComboBox()
    {
    }
    
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    //void resized() override;
    void mouseDoubleClick(const MouseEvent& e) override;
    
    ListenerList<BKEditableComboBoxListener> listeners;
    void addMyListener(BKEditableComboBoxListener* listener)     { listeners.add(listener);      }
    void removeMyListener(BKEditableComboBoxListener* listener)  { listeners.remove(listener);   }
    
private:
    
    //BKComboBox thisComboBox;
    //selectCB.setSelectedItemIndex(processor.updateState->currentSynchronicId, notify);
    //selectCB.clear(dontSendNotification);
    //selectCB.addItem(name, i+1);
    //selectCB.getNumItems()-1)
    //selectCB.changeItemText(selected, text);
    
    BKTextField nameEditor;
    
    void showModifyPopupMenu();
    static void controlClickCallback (const int result, BKEditableComboBox* cbox);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEditableComboBox)
};


#endif  // BKMENU_H_INCLUDED
