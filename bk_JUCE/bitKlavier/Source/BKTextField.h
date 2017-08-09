/*
  ==============================================================================

    BKTextField.h
    Created: 15 Nov 2016 4:19:33pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKTEXTFIELD_H_INCLUDED
#define BKTEXTFIELD_H_INCLUDED

#include "BKUtilities.h"
#include "BKLookAndFeel.h"

//==============================================================================

class BKTextField    : public TextEditor, public TextEditor::Listener
{
public:
    BKTextField()
    {
        setLookAndFeel(&laf);
        setSize(200,18);
        
        setEscapeAndReturnKeysConsumed(false);
    
        lookAndFeelChanged();
    }

    ~BKTextField()
    {
        
    }
    
    
    
private:
    BKTextFieldLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKTextField)
    
};


#endif  // BKTEXTFIELD_H_INCLUDED
