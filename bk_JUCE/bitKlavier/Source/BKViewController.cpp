/*
  ==============================================================================

    BKViewController.cpp
    Created: 31 Oct 2017 1:09:55pm
    Author:  airship

  ==============================================================================
*/

#include "BKViewController.h"

#include "KeymapViewController.h"


BKViewController::BKViewController(BKAudioProcessor& p, BKItemGraph* theGraph, int nt):
processor(p),
theGraph(theGraph),
lastId(0),
numTabs(nt),
bigOne(p),
leftArrow(LeftArrow),
rightArrow(RightArrow)
{
    addAndMakeVisible(hideOrShow);
    hideOrShow.setAlwaysOnTop(true);
    hideOrShow.setName("hideOrShow");
    hideOrShow.addListener(this);
    
    addChildComponent(bigOne);
    
    subWindowInFront = false;
    
    addAndMakeVisible(leftArrow);
    leftArrow.setAlwaysOnTop(true);
    leftArrow.setName("leftArrow");
    leftArrow.addListener(this);
    
    addAndMakeVisible(rightArrow);
    rightArrow.setAlwaysOnTop(true);
    rightArrow.setName("rightArrow");
    rightArrow.addListener(this);

    if (numTabs == 1)
    {
        leftArrow.setVisible(false);
        rightArrow.setVisible(false);
    }
}

BKViewController::~BKViewController()
{
    
}

PopupMenu BKViewController::getExportedPrepsMenu(BKPreparationType type)
{
    BKPopupMenu menu;
    processor.collectPreparations();
    StringArray* names = processor.exportedPreparations[type];
    for (int i = 0; i < names->size(); i++)
    {
        menu.addItem(i+100, names->getReference(i));
    }
    
    return std::move(menu);
}

PopupMenu BKViewController::getPrepOptionMenu(BKPreparationType type, bool singlePrep)
{
    BKPopupMenu optionMenu;
    
    optionMenu.addItem(1, "New");
    optionMenu.addItem(2, "Duplicate");
    optionMenu.addItem(6, "Rename");
    optionMenu.addItem(3, "Delete", !singlePrep);
    optionMenu.addSeparator();
    optionMenu.addItem(7, "Export");
    
    PopupMenu exported = getExportedPrepsMenu(type);
    optionMenu.addSubMenu("Import...", exported);
    
    optionMenu.addSeparator();
    optionMenu.addItem(4, "Reset");
    optionMenu.addItem(5, "Clear");
    
    return std::move(optionMenu);
}

PopupMenu BKViewController::getModOptionMenu(BKPreparationType type, bool singleMod)
{
    BKPopupMenu optionMenu;
    
    optionMenu.addItem(1, "New");
    optionMenu.addItem(2, "Duplicate");
    optionMenu.addItem(6, "Rename");
    optionMenu.addItem(3, "Delete", !singleMod);
    optionMenu.addSeparator();
    optionMenu.addItem(7, "Export");
    optionMenu.addSubMenu("Import...", getExportedPrepsMenu(type));
    optionMenu.addSeparator();
    optionMenu.addItem(5, "Reset");

    return std::move(optionMenu);
}


void BKViewController::moddableComponentEditorOpened(ModdableComponent* mc)
{
    processor.updateState->setCurrentDisplay(DisplayModdable);
    processor.updateState->currentModdableIdentifier = mc->getIdentifier();
    setSubWindowInFront(false);
}
