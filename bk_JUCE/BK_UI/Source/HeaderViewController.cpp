/*
  ==============================================================================

    HeaderViewController.cpp
    Created: 27 Mar 2017 12:47:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "HeaderViewController.h"

HeaderViewController::HeaderViewController (BKAudioProcessor& p):
processor (p)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    addAndMakeVisible(loadB);
    loadB.setButtonText("Load Samples");
    loadB.addListener(this);
    
    addAndMakeVisible(galleryB);
    galleryB.setButtonText("Gallery Action");
    galleryB.addListener(this);
    
    addAndMakeVisible(pianoB);
    pianoB.setButtonText("Piano Action");
    pianoB.addListener(this);
    
    // Gallery CB
    addAndMakeVisible(galleryCB);
    galleryCB.setName("galleryCB");
    galleryCB.addListener(this);
    //galleryCB.BKSetJustificationType(juce::Justification::centredRight);
    
    galleryCB.setSelectedId(0, dontSendNotification);
    lastGalleryCBId = galleryCB.getSelectedId();
    
    // Piano CB
    addAndMakeVisible(pianoCB);
    pianoCB.setName("pianoCB");
    pianoCB.addListener(this);
    pianoCB.addMyListener(this);
    pianoCB.BKSetJustificationType(juce::Justification::centredRight);
    
    pianoCB.addSeparator();
    pianoCB.addItem("New piano...",1);
    pianoCB.setSelectedId(0, dontSendNotification);
    
    galleryModalCallBackIsOpen = false;
    fillGalleryCB();
    
}

HeaderViewController::~HeaderViewController()
{
    
}

void HeaderViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void HeaderViewController::resized()
{
    float width = getWidth() / 7 - gXSpacing;
    
    Rectangle<int> area (getLocalBounds());
    area.reduce(0, gYSpacing);
    
    area.removeFromLeft(gXSpacing);
    galleryB.setBounds(area.removeFromLeft(width));
    
    area.removeFromLeft(gXSpacing);
    galleryCB.setBounds(area.removeFromLeft(2*width));
    
    area.removeFromRight(gXSpacing);
    pianoB.setBounds(area.removeFromRight(width));
    
    area.removeFromRight(gXSpacing);
    pianoCB.setBounds(area.removeFromRight(2*width));
    area.removeFromRight(gXSpacing);

    area.removeFromLeft(gXSpacing);
    loadB.setBounds(area);
}

PopupMenu HeaderViewController::getLoadMenu(void)
{
    PopupMenu loadMenu;
    loadMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    int count = 0;
    for (auto loadType : cBKSampleLoadTypes)
        loadMenu.addItem(++count, loadType);
    
    return loadMenu;
}

PopupMenu HeaderViewController::getPianoMenu(void)
{
    PopupMenu pianoMenu;
    pianoMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    pianoMenu.addItem(1, "New");
    pianoMenu.addItem(2, "Duplicate");
    pianoMenu.addItem(3, "Delete");
    //pianoMenu.addSeparator();
    //pianoMenu.addItem(3, "Settings");
    
    return pianoMenu;
}

#define SAVE_ID 1
#define SAVEAS_ID 2
#define OPEN_ID 3
#define CLEAN_ID 4
#define SETTINGS_ID 5
#define OPENOLD_ID 6

PopupMenu HeaderViewController::getGalleryMenu(void)
{
    PopupMenu galleryMenu;
    galleryMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    galleryMenu.addItem(SAVE_ID, "Save");
    galleryMenu.addItem(SAVEAS_ID, "Save as...");
    galleryMenu.addItem(OPEN_ID, "Open...");
    galleryMenu.addSeparator();
    galleryMenu.addItem(CLEAN_ID, "Clean");
    galleryMenu.addSeparator();
    galleryMenu.addItem(SETTINGS_ID, "Gallery Settings");
    galleryMenu.addSeparator();
    galleryMenu.addItem(OPENOLD_ID, "Open (legacy)...");
    
    return galleryMenu;
    
    
}

void HeaderViewController::loadMenuCallback(int result, HeaderViewController* gvc)
{
    gvc->processor.loadPianoSamples((BKSampleLoadType)(result-1));
}

void HeaderViewController::pianoMenuCallback(int result, HeaderViewController* hvc)
{
    BKAudioProcessor& processor = hvc->processor;
    if (result == 1) // New piano
    {
        int newId = processor.gallery->getNewId(PreparationTypePiano);
        
        processor.gallery->addTypeWithId(PreparationTypePiano, newId);
        
        String newName = "Piano"+String(newId);
        
        processor.gallery->getPianos().getLast()->setName(newName);
        
        hvc->fillPianoCB();
        
        processor.setCurrentPiano(newId);
    }
    else if (result == 2) // Duplicate
    {
        int newId = processor.gallery->duplicate(PreparationTypePiano, processor.currentPiano->getId());
        
        String newName = "Piano"+String(newId);
        
        hvc->fillPianoCB();
        
        processor.setCurrentPiano(newId);
    }
    else if (result == 3) // Remove piano
    {
        int pianoId = hvc->pianoCB.getSelectedId();
        int index = hvc->pianoCB.getSelectedItemIndex();
        
        if ((index == 0) && (hvc->pianoCB.getItemId(index+1) == -1)) return;
        
        processor.gallery->remove(PreparationTypePiano, pianoId);
        
        hvc->fillPianoCB();
        
        int newPianoId = hvc->pianoCB.getItemId(index);
        
        if (newPianoId == -1) newPianoId = hvc->pianoCB.getItemId(index-1);
        
        hvc->pianoCB.setSelectedId(newPianoId, dontSendNotification);
        
        processor.setCurrentPiano(newPianoId);
    }
    
}

void HeaderViewController::galleryMenuCallback(int result, HeaderViewController* gvc)
{
    if (result == SAVE_ID)
    {
        gvc->processor.saveGallery();
    }
    if (result == SAVEAS_ID)
    {
        gvc->processor.saveGalleryAs();
    }
    else if (result == OPEN_ID) // Load
    {
        gvc->processor.loadGalleryDialog();
    }
    else if (result == SETTINGS_ID) // open General settings
    {
        gvc->processor.updateState->setCurrentDisplay(DisplayGeneral);
    }
    else if (result == CLEAN_ID) // Clean
    {
        gvc->processor.gallery->clean();
    }
    else if (result == OPENOLD_ID) // Load (old)
    {
        gvc->processor.loadJsonGalleryDialog();
    }
}

void HeaderViewController::bkButtonClicked (Button* b)
{
    String name = b->getName();
    
    if (b == &loadB)
    {
        PopupMenu loadMenu = getLoadMenu();
        loadMenu.setLookAndFeel(&buttonsAndMenusLAF);
        
        loadMenu.showMenuAsync (PopupMenu::Options().withTargetComponent (&loadB), ModalCallbackFunction::forComponent (loadMenuCallback, this) );
    }
    else if (b == &pianoB)
    {
        getPianoMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&pianoB), ModalCallbackFunction::forComponent (pianoMenuCallback, this) );
    }
    else if (b == &galleryB)
    {
        getGalleryMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&galleryB), ModalCallbackFunction::forComponent (galleryMenuCallback, this) );
    }
    
    
}

void HeaderViewController::fillGalleryCB(void)
{
    
    if(!galleryModalCallBackIsOpen)
    {
        galleryCB.clear(dontSendNotification);
        
        PopupMenu* galleryCBPopUp = galleryCB.getRootMenu();
        
        int index = 0;
        bool creatingSubmenu = false;
        String submenuName;
        
        StringArray submenuNames;
        OwnedArray<PopupMenu> submenus;
        
        for (int i = 0; i < processor.galleryNames.size(); i++)
        {
            File thisFile(processor.galleryNames[i]);
            
            String galleryName = thisFile.getFileName().upToFirstOccurrenceOf(".xml", false, false);
            
            //moving on to new submenu, if there is one, add add last submenu to popup now that it's done
            if(creatingSubmenu && thisFile.getRelativePathFrom(File ("~/bkGalleries")).initialSectionNotContaining("/") != submenuName)
            {
                galleryCBPopUp->addSubMenu(submenuName, *submenus.getLast());
                creatingSubmenu = false;
            }
            
            //add toplevel item, if there is one
            if(thisFile.getFileName() == thisFile.getRelativePathFrom(File ("~/bkGalleries"))) //if the file is in the main galleries directory....
            {
                galleryCB.addItem(galleryName, i+1); //add to toplevel popup
            }
            
            //otherwise add to or create submenu with name of subfolder
            else
            {
                creatingSubmenu = true;
                
                submenuName = thisFile.getRelativePathFrom(File ("~/bkGalleries")).initialSectionNotContaining("/"); //name of submenu
                
                if(submenuNames.contains(submenuName)) //add to existing submenu
                {
                    PopupMenu* existingMenu = submenus.getUnchecked(submenuNames.indexOf(submenuName));
                    existingMenu->addItem(i + 1, galleryName);
                }
                else
                {
                    submenus.add(new PopupMenu());
                    submenuNames.add(submenuName);

                    submenus.getLast()->addItem(i + 1, galleryName);;
                }
            }
     
            if (thisFile.getFileName() == processor.currentGallery) index = i;
        }
        
        //add last submenu to popup, if there is one
        if(creatingSubmenu)
        {
            galleryCBPopUp->addSubMenu(submenuName, *submenus.getLast());
            creatingSubmenu = false;
        }

        galleryCB.setSelectedId(index+1, NotificationType::dontSendNotification);
        
        File selectedFile(processor.galleryNames[index]);
        processor.gallery->setURL(selectedFile.getFullPathName());
        
    }
}

void HeaderViewController::update(void)
{
    
}

void HeaderViewController::switchGallery()
{
    fillPianoCB();
    fillGalleryCB();
}

void HeaderViewController::fillPianoCB(void)
{
    pianoCB.clear(dontSendNotification);

    for (auto piano : processor.gallery->getPianos())
    {
        String name = piano->getName();
        
        DBG("string piano Id: " + String(piano->getId()));
        
        if (name != String::empty)  pianoCB.addItem(name,  piano->getId());
        else                        pianoCB.addItem("Piano" + String(piano->getId()), piano->getId());
    }
    
    pianoCB.addSeparator();
    pianoCB.addItem("New piano...", -1);

    pianoCB.setSelectedId(processor.currentPiano->getId(), dontSendNotification);
}

void HeaderViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
}

void HeaderViewController::BKEditableComboBoxChanged(String text, BKEditableComboBox* cb)
{
    processor.currentPiano->setName(text);
}


void HeaderViewController::bkComboBoxDidChange (ComboBox* cb)
{
    // Change piano
    String name = cb->getName();
    int Id = cb->getSelectedId();

    
    if (name == "pianoCB")
    {
        if (Id == -1)
        {
            processor.gallery->add(PreparationTypePiano);
            
            Id = processor.gallery->getPianos().getLast()->getId();
        }
        
        processor.setCurrentPiano(Id);
        
        fillPianoCB();
        
        update();
        
    }
    else if (name == "galleryCB")
    {
        galleryIsDirtyAlertResult = 1;

        if(processor.gallery->isGalleryDirty())
        {
            DBG("GALLERY IS DIRTY, CHECK FOR SAVE HERE");
            galleryModalCallBackIsOpen = true; //not sure, maybe should be doing some kind of Lock
            
            galleryIsDirtyAlertResult = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon,
                                          "The current gallery has changed.",
                                          "Do you want to save before switching galleries?",
                                          String(),
                                          String(),
                                          String(),
                                          0,
                                          //ModalCallbackFunction::forComponent (alertBoxResultChosen, this)
                                             nullptr);
        }

        if(galleryIsDirtyAlertResult == 0)
        {
            cb->setSelectedId(lastGalleryCBId, dontSendNotification);
            DBG("reverting to prior gallery");
        }
        else if(galleryIsDirtyAlertResult == 1)
        {
            DBG("saving gallery first");
            processor.saveGallery();
            
            String path = processor.galleryNames[cb->getSelectedItemIndex()];
            lastGalleryCBId = Id;
            
            if (path.endsWith(".xml"))          processor.loadGalleryFromPath(path);
            else  if (path.endsWith(".json"))   processor.loadJsonGalleryFromPath(path);
        }
        else if(galleryIsDirtyAlertResult == 2)
        {
            DBG("changing galleries without saving first");
            String path = processor.galleryNames[cb->getSelectedItemIndex()];
            lastGalleryCBId = Id;
            
            if (path.endsWith(".xml"))          processor.loadGalleryFromPath(path);
            else  if (path.endsWith(".json"))   processor.loadJsonGalleryFromPath(path);
        }
        
        galleryModalCallBackIsOpen = false;

    }
}

