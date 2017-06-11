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


class BKEditableComboBoxListener;

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
        addAndMakeVisible(nameEditor);
        nameEditor.toBack();
        nameEditor.setAlpha(0.);
        //nameEditor.setVisible(false);
        nameEditor.setOpaque(true);
        nameEditor.addListener(this);
    }
    
    ~BKEditableComboBox()
    {
    }
    
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void mouseDoubleClick(const MouseEvent& e) override;
    
    ListenerList<BKEditableComboBoxListener> listeners;
    void addMyListener(BKEditableComboBoxListener* listener)     { listeners.add(listener);      }
    void removeMyListener(BKEditableComboBoxListener* listener)  { listeners.remove(listener);   }
    
private:
        
    BKTextField nameEditor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEditableComboBox)
};

class BKEditableComboBoxListener
{
    
public:
    
    virtual ~BKEditableComboBoxListener() {};
    
    virtual void BKEditableComboBoxChanged(String text, BKEditableComboBox*) = 0;
};



#endif  // BKMENU_H_INCLUDED
