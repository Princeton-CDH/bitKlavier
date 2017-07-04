/*
  ==============================================================================

    BKViewController.h
    Created: 20 Jun 2017 6:00:55pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKListener.h"
#include "BKComponent.h"

#include "BKGraph.h"

#include "BKSlider.h"
#include "BKKeyboardSlider.h"

class BKViewController    : public BKComponent, public BKListener
{
public:
    BKViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
    processor(p),
    theGraph(theGraph)
    {
        
    }
    
    ~BKViewController()
    {
        
    }
    
    PopupMenu getOptionMenu(void)
    {
        PopupMenu optionMenu;
        optionMenu.setLookAndFeel(&buttonsAndMenusLAF);
        
        optionMenu.addItem(1, "New");
        optionMenu.addItem(2, "Save");
        optionMenu.addItem(3, "Delete");
        
        return optionMenu;
    }
    
    
    
protected:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    BKButtonAndMenuLAF buttonsAndMenusLAF2;
    
    ImageComponent iconImageComponent;
    TextButton hideOrShow;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKViewController)
};

