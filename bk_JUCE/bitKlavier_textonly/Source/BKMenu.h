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

//==============================================================================
/*
*/
class BKComboBox    : public ComboBox
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


#endif  // BKMENU_H_INCLUDED
