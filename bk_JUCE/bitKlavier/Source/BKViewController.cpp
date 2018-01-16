/*
  ==============================================================================

    BKViewController.cpp
    Created: 31 Oct 2017 1:09:55pm
    Author:  airship

  ==============================================================================
*/

#include "BKViewController.h"

#include "KeymapViewController.h"


BKViewController::BKViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph),
lastId(0)
{
    addAndMakeVisible(hideOrShow);
    hideOrShow.toFront(false);
    hideOrShow.setName("hideOrShow");
    hideOrShow.addListener(this);
    
    addChildComponent(bigOne);
}

BKViewController::~BKViewController()
{
    
}

PopupMenu BKViewController::getPrepOptionMenu(void)
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

PopupMenu BKViewController::getModOptionMenu(void)
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

