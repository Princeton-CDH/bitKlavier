/*
  ==============================================================================

    BKViewController.h
    Created: 20 Jun 2017 6:00:55pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#pragma once

#define AUTO_DELETE 0

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
    theGraph(theGraph),
    lastId(0)
    {
        
    }
    
    ~BKViewController()
    {
        
    }
    
    PopupMenu getPrepOptionMenu(void)
    {
        PopupMenu optionMenu;
        optionMenu.setLookAndFeel(&buttonsAndMenusLAF);
        
        optionMenu.addItem(1, "New");
        
        optionMenu.addItem(2, "Duplicate");
        optionMenu.addItem(3, "Delete");
        optionMenu.addSeparator();
        optionMenu.addItem(4, "Reset");
        optionMenu.addItem(5, "Clear");
        
        return optionMenu;
    }
    
    PopupMenu getModOptionMenu(void)
    {
        PopupMenu optionMenu;
        optionMenu.setLookAndFeel(&buttonsAndMenusLAF);
        
        optionMenu.addItem(1, "New");
        
        optionMenu.addItem(2, "Duplicate");
        optionMenu.addItem(3, "Delete");
        optionMenu.addSeparator();
        optionMenu.addItem(5, "Clear");
        /*
         optionMenu.addSeparator();
         optionMenu.addItem(4, "Import");
         optionMenu.addItem(5, "Export");
         */
        
        return optionMenu;
    }
    
protected:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    TextButton actionButton;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    BKButtonAndMenuLAF buttonsAndMenusLAF2;
    
    ImageComponent iconImageComponent;
    TextButton hideOrShow;
    
    int lastId;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKViewController)
};

