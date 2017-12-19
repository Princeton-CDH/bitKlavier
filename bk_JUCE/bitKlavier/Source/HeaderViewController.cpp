/*
  ==============================================================================

    HeaderViewController.cpp
    Created: 27 Mar 2017 12:47:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/



#include "HeaderViewController.h"


HeaderViewController::HeaderViewController (BKAudioProcessor& p, BKConstructionSite* c):
processor (p),
construction(c),
lastGalleryCBId(-1)
{
    setLookAndFeel(&buttonsAndMenusLAF);

    addAndMakeVisible(galleryB);
    galleryB.setButtonText("Gallery");
    galleryB.addListener(this);
    
    addAndMakeVisible(pianoB);
    pianoB.setButtonText("Piano");
    pianoB.addListener(this);
    
    addAndMakeVisible(editB);
    editB.setButtonText("Action");
    editB.addListener(this);
    
    // Gallery CB
    addAndMakeVisible(galleryCB);
    galleryCB.setName("galleryCB");
    galleryCB.addListener(this);
    //galleryCB.BKSetJustificationType(juce::Justification::centredRight);
    
    galleryCB.setLookAndFeel(&comboBoxLeftJustifyLAF);
    galleryCB.setSelectedId(0, dontSendNotification);
    
    // Piano CB
    addAndMakeVisible(pianoCB);
    pianoCB.setName("pianoCB");
    pianoCB.addListener(this);
    
#if JUCE_IOS || JUCE_MAC
    bot.setBounds(0,0,20,20);
    addAndMakeVisible(bot);
#endif
    
    pianoCB.setLookAndFeel(&comboBoxRightJustifyLAF);
    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
    //pianoCB.BKSetJustificationType(juce::Justification::centredRight);
    //pianoCB.setJustificationType(juce::Justification::centredRight);
    pianoCB.setSelectedId(0, dontSendNotification);
    
    galleryModalCallBackIsOpen = false;
    
    //loadDefaultGalleries();
    
    fillGalleryCB();
    fillPianoCB();
    processor.updateState->pianoDidChangeForGraph = true;
    
}

HeaderViewController::~HeaderViewController()
{
    galleryCB.setLookAndFeel(nullptr);
    pianoCB.setLookAndFeel(nullptr);

    setLookAndFeel(nullptr);
}

void HeaderViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void HeaderViewController::resized()
{
    float width = getWidth() / 7;
    
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
    editB.setBounds(area);
}

PopupMenu HeaderViewController::getLoadMenu(void)
{
    PopupMenu loadMenu;
    loadMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    loadMenu.addItem(LOAD_LITEST, "Lightest");
    loadMenu.addItem(LOAD_LITE, "Light");
    loadMenu.addItem(LOAD_MEDIUM, "Medium");
    loadMenu.addItem(LOAD_HEAVY, "Heavy");
    
    return loadMenu;
}

PopupMenu HeaderViewController::getPianoMenu(void)
{
    PopupMenu pianoMenu;
    pianoMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    pianoMenu.addItem(1, "New");
    pianoMenu.addSeparator();
    pianoMenu.addItem(2, "Duplicate");
    pianoMenu.addSeparator();
    pianoMenu.addItem(4, "Rename");
    pianoMenu.addSeparator();
    pianoMenu.addItem(3, "Remove");
    
    return pianoMenu;
}

PopupMenu HeaderViewController::getGalleryMenu(void)
{
    PopupMenu galleryMenu;
    galleryMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    galleryMenu.addSeparator();
    galleryMenu.addItem(NEWGALLERY_ID, "New");
    
    if (!processor.defaultLoaded)
    {
        galleryMenu.addSeparator();
        galleryMenu.addItem(RENAME_ID, "Rename");
        galleryMenu.addSeparator();
        galleryMenu.addItem(DELETE_ID, "Remove");
    }
    
    galleryMenu.addSeparator();
    
    if (!processor.defaultLoaded)   galleryMenu.addItem(SAVE_ID, "Save " );
    galleryMenu.addItem(SAVEAS_ID, "Save as");
    
#if !JUCE_IOS
    galleryMenu.addSeparator();
    galleryMenu.addItem(OPEN_ID, "Open");
    galleryMenu.addItem(OPENOLD_ID, "Open (legacy)");
#endif
    
    galleryMenu.addSeparator();
    galleryMenu.addItem(CLEAN_ID, "Clean");
    galleryMenu.addSeparator();
    galleryMenu.addSubMenu("Load", getLoadMenu());
    galleryMenu.addSeparator();
    galleryMenu.addItem(SETTINGS_ID, "Settings");
    
    // ~ ~ ~ share menu ~ ~ ~
#if JUCE_MAC
    PopupMenu shareMenu;
    
    shareMenu.addItem(SHARE_EMAIL_ID, "Email");
    galleryMenu.addSeparator();
    shareMenu.addItem(SHARE_MESSAGE_ID, "Messages");
    galleryMenu.addSeparator();
    shareMenu.addItem(SHARE_FACEBOOK_ID, "Facebook");
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    galleryMenu.addSeparator();
    galleryMenu.addSubMenu("Share", shareMenu);
#elif JUCE_IOS
    galleryMenu.addSeparator();
    galleryMenu.addItem(SHARE_MESSAGE_ID, "Share");
#endif
    
    
    
    return galleryMenu;
    
}

void HeaderViewController::pianoMenuCallback(int result, HeaderViewController* hvc)
{
    BKAudioProcessor& processor = hvc->processor;
    
    if (result == 1) // New piano
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addTextEditor("name", "My New Piano");
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            int newId = processor.gallery->getNewId(PreparationTypePiano);
            
            processor.gallery->addTypeWithId(PreparationTypePiano, newId);
            
            processor.gallery->getPianos().getLast()->setName(name);
            
            hvc->fillPianoCB();
            
            processor.setCurrentPiano(newId);
        }
    }
    else if (result == 2) // Duplicate
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addTextEditor("name", processor.currentPiano->getName() + " (2)");
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            int newId = processor.gallery->duplicate(PreparationTypePiano, processor.currentPiano->getId());
            
            processor.setCurrentPiano(newId);
            
            processor.currentPiano->setName(name);
            
            hvc->fillPianoCB();
        }
    }
    else if (result == 3) // Remove piano
    {
        
        int pianoId = hvc->pianoCB.getSelectedId();
        int index = hvc->pianoCB.getSelectedItemIndex();
        
        if ((index == 0) && (hvc->pianoCB.getNumItems() == 1)) return;
        
        processor.gallery->remove(PreparationTypePiano, pianoId);
        
        hvc->fillPianoCB();
        
        int newPianoId = hvc->pianoCB.getItemId(index);
        
        if (newPianoId == 0) newPianoId = hvc->pianoCB.getItemId(index-1);
        
        hvc->pianoCB.setSelectedId(newPianoId, dontSendNotification);
        
        processor.setCurrentPiano(newPianoId);
    }
    else if (result == 3) // Remove piano
    {
        
        int pianoId = hvc->pianoCB.getSelectedId();
        int index = hvc->pianoCB.getSelectedItemIndex();
        
        if ((index == 0) && (hvc->pianoCB.getNumItems() == 1)) return;
        
        processor.gallery->remove(PreparationTypePiano, pianoId);
        
        hvc->fillPianoCB();
        
        int newPianoId = hvc->pianoCB.getItemId(index);
        
        if (newPianoId == 0) newPianoId = hvc->pianoCB.getItemId(index-1);
        
        hvc->pianoCB.setSelectedId(newPianoId, dontSendNotification);
        
        processor.setCurrentPiano(newPianoId);
    }
    else if (result == 4) // Rename
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addTextEditor("name", processor.currentPiano->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            processor.currentPiano->setName(name);
        }
        
        hvc->fillPianoCB();
    }
    
}

void HeaderViewController::galleryMenuCallback(int result, HeaderViewController* gvc)
{
    BKAudioProcessor& processor = gvc->processor;
    
    if (result == RENAME_ID)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addTextEditor("name", processor.gallery->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
#if JUCE_IOS
            String lastName = processor.gallery->getName();
            processor.createGalleryWithName(name);
            processor.deleteGalleryWithName(lastName);
#else
            processor.renameGallery(name);
#endif
        }
        
        gvc->fillGalleryCB();
    }
    else if (result == SHARE_EMAIL_ID)
    {
        gvc->bot.share(processor.getCurrentGalleryPath(), 0);
    }
    else if (result == SHARE_MESSAGE_ID)
    {
        gvc->bot.share(processor.getCurrentGalleryPath(), 1);
    }
    else if (result == SHARE_FACEBOOK_ID)
    {
        gvc->bot.share(processor.getCurrentGalleryPath(), 2);
    }
    else if (result == LOAD_LITEST)
    {
        processor.gallery->sampleType = BKLoadLitest;
        processor.loadPianoSamples(BKLoadLitest);
    }
    else if (result == LOAD_LITE)
    {
        processor.gallery->sampleType = BKLoadLite;
        processor.loadPianoSamples(BKLoadLite);
    }
    else if (result == LOAD_MEDIUM)
    {
        processor.gallery->sampleType = BKLoadMedium;
        processor.loadPianoSamples(BKLoadMedium);
    }
    else if (result == LOAD_HEAVY)
    {
        processor.gallery->sampleType = BKLoadHeavy;
        processor.loadPianoSamples(BKLoadHeavy);
    }
    else if (result == SAVE_ID)
    {
        processor.saveGallery();
        
        processor.createGalleryWithName(processor.gallery->getName());
    }
    if (result == SAVEAS_ID)
    {
#if JUCE_IOS
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addTextEditor("name", processor.gallery->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            processor.createGalleryWithName(name);
        }
        
        gvc->fillGalleryCB();
#else
        processor.saveGalleryAs();
#endif
    }
    else if (result == OPEN_ID) // Load
    {
        processor.loadGalleryDialog();
    }
    else if (result == SETTINGS_ID) // open General settings
    {
        processor.updateState->setCurrentDisplay(DisplayGeneral);
    }
    else if (result == CLEAN_ID) // Clean
    {
        processor.gallery->clean();
    }
    else if (result == DELETE_ID) // Clean
    {
#if JUCE_IOS
        processor.deleteGalleryWithName(processor.gallery->getName());
        
        gvc->galleryCB.setSelectedItemIndex(0);
#else
        processor.deleteGallery();
#endif
    }
    else if (result == OPENOLD_ID) // Load (old)
    {
        processor.loadJsonGalleryDialog();
    }
    else if (result == NEWGALLERY_ID)
    {
        bool shouldContinue = gvc->handleGalleryChange();
        
        if (!shouldContinue) return;
        
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addTextEditor("name", "My New Gallery");
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            processor.createNewGallery(name);
        }
    }
}

void HeaderViewController::bkButtonClicked (Button* b)
{
    String name = b->getName();
    
    if (b == &editB)
    {
         getEditMenu(&buttonsAndMenusLAF, construction->getNumSelected()).showMenuAsync(PopupMenu::Options().withTargetComponent (b),
         ModalCallbackFunction::forComponent (construction->editMenuCallback, construction) );
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

void HeaderViewController::addGalleriesFromFolder(File folder)
{
    
}


void HeaderViewController::loadDefaultGalleries(void)
{
    if(!galleryModalCallBackIsOpen)
    {
        galleryCB.clear(dontSendNotification);
        
        PopupMenu* popupRoot = galleryCB.getRootMenu();
        
        String data,name, resource;
        int id = 1;
        
        int size;
        
        PopupMenu mikroetudes_menu, ns_etudes_menu, bk_examples_menu;
        
        //data = BinaryData::Basic_Piano_xml;
        for (int i = 0; i < BinaryData::namedResourceListSize; i++)
        {
            resource = BinaryData::namedResourceList[i];
            
            if (resource.contains("_xml"))
            {
                data = BinaryData::getNamedResource(BinaryData::namedResourceList[i], size);
                
                name = data.fromFirstOccurrenceOf("<gallery name=\"", false, true).upToFirstOccurrenceOf("\"", false, true);

                if (processor.mikroetudes.contains(name))       mikroetudes_menu.addItem(id++, name);
                else if (processor.ns_etudes.contains(name))    ns_etudes_menu.addItem(id++, name);
                else if (processor.bk_examples.contains(name))  bk_examples_menu.addItem(id++, name);
                else                                            galleryCB.addItem(name, id++);
                
            }
        }
        
        popupRoot->addSubMenu("Examples", bk_examples_menu);
        popupRoot->addSubMenu("Nostalgic Synchronic", ns_etudes_menu);
        popupRoot->addSubMenu("Mikroetudes", mikroetudes_menu);
        
        galleryCB.addSeparator();
        
        numberOfDefaultGalleryItems = galleryCB.getNumItems();
    }
}

void HeaderViewController::fillGalleryCB(void)
{
    
    if(!galleryModalCallBackIsOpen)
    {
        loadDefaultGalleries();
        
        File bkGalleries;
        
        File moreGalleries = File::getSpecialLocation(File::userDocumentsDirectory);
        
#if JUCE_MAC || JUCE_WINDOWS
        bkGalleries = bkGalleries.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("galleries");
#endif
        
        PopupMenu* galleryCBPopUp = galleryCB.getRootMenu();
        
        int id = numberOfDefaultGalleryItems, index = 0;
        bool creatingSubmenu = false;
        String submenuName;
        
        StringArray submenuNames;
        OwnedArray<PopupMenu> submenus;
        
        for (int i = 0; i < processor.galleryNames.size(); i++)
        {
            File thisFile(processor.galleryNames[i]);
            
            String galleryName = thisFile.getFileName().upToFirstOccurrenceOf(".xml", false, false);
            
            //moving on to new submenu, if there is one, add add last submenu to popup now that it's done
            if(creatingSubmenu && thisFile.getParentDirectory().getFileName() != submenuName)
            {
                galleryCBPopUp->addSubMenu(submenuName, *submenus.getLast());
                creatingSubmenu = false;
            }
            
            //add toplevel item, if there is one
            if(thisFile.getParentDirectory().getFileName() == bkGalleries.getFileName() ||
               thisFile.getParentDirectory().getFileName() == moreGalleries.getFileName() ||
               thisFile.getParentDirectory().getFileName() == moreGalleries.getChildFile("Inbox").getFileName()) //if the file is in the main galleries directory....
            {
                galleryCB.addItem(galleryName, ++id); //add to toplevel popup
            }
            
            //otherwise add to or create submenu with name of subfolder
            else
            {
                creatingSubmenu = true;
                
                submenuName = thisFile.getParentDirectory().getFileName(); //name of submenu
                
                if(submenuNames.contains(submenuName)) //add to existing submenu
                {
                    PopupMenu* existingMenu = submenus.getUnchecked(submenuNames.indexOf(submenuName));
                    existingMenu->addItem(++id, galleryName);
                }
                else
                {
                    submenus.add(new PopupMenu());
                    submenuNames.add(submenuName);
                    
                    submenus.getLast()->addItem(++id, galleryName);;
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
        
        // THIS IS WHERE NAME OF GALLERY DISPLAYED IS SET
        galleryCB.setSelectedId(lastGalleryCBId, NotificationType::dontSendNotification);
        if(lastGalleryCBId < 0)galleryCB.setText(processor.gallery->getName().upToFirstOccurrenceOf(".xml", false, true), dontSendNotification);

    }
}

void HeaderViewController::update(void)
{
    
}

void HeaderViewController::switchGallery()
{
    fillGalleryCB();
    fillPianoCB();
}

void HeaderViewController::fillPianoCB(void)
{
    pianoCB.clear(dontSendNotification);

    for (auto piano : processor.gallery->getPianos())
    {
        String name = piano->getName();
        
        if (name != String::empty)  pianoCB.addItem(name,  piano->getId());
        else                        pianoCB.addItem("Piano" + String(piano->getId()), piano->getId());
    }

    pianoCB.setSelectedId(processor.currentPiano->getId(), dontSendNotification);
}

void HeaderViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    DBG(text);
    
}

bool HeaderViewController::handleGalleryChange(void)
{
    String name = galleryCB.getName();
    
    bool shouldSwitch = false;
    
    galleryIsDirtyAlertResult = 2;
    
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
        DBG("cancelled");
    }
    else if(galleryIsDirtyAlertResult == 1)
    {
        DBG("saving gallery");
        
        
#if JUCE_IOS
        if (processor.defaultLoaded)
        {
            AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
            
            prompt.addTextEditor("name", processor.gallery->getName());
            
            prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
            prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
            
            int result = prompt.runModalLoop();
            
            String name = prompt.getTextEditorContents("name");
            
            if (result == 1)
            {
                processor.createGalleryWithName(name);
            }
        }
        else
        {
            processor.createGalleryWithName(processor.gallery->getName());
        }
        
        fillGalleryCB();
#else
        
        processor.saveGallery();
        
#endif
        
        shouldSwitch = true;
    }
    else if(galleryIsDirtyAlertResult == 2)
    {
        DBG("not saving");
        shouldSwitch = true;
    }
    
    galleryModalCallBackIsOpen = false;
    
    return shouldSwitch;
}


void HeaderViewController::bkComboBoxDidChange (ComboBox* cb)
{
    String name = cb->getName();
    int Id = cb->getSelectedId();

    if (name == "pianoCB")
    {
        processor.setCurrentPiano(Id);
        
        fillPianoCB();
        
        update();
        
    }
    else if (name == "galleryCB")
    {
        bool shouldSwitch = handleGalleryChange();
        
        if (shouldSwitch)
        {
            lastGalleryCBId = Id;
            int index = Id - 1;

            //if (index < numberOfDefaultGalleryItems)
            if(cb->getSelectedItemIndex() < numberOfDefaultGalleryItems)
            {
                int size;
                int index = Id - 1;
                String xmlData = CharPointer_UTF8 (BinaryData::getNamedResource(BinaryData::namedResourceList[index], size));
                
                processor.defaultLoaded = true;
                processor.defaultName = BinaryData::namedResourceList[index];
                
                processor.loadGalleryFromXml(XmlDocument::parse(xmlData));
                
                
            }
            else
            {
                index = index - numberOfDefaultGalleryItems;
                String path = processor.galleryNames[index];
                
                processor.defaultLoaded = false;
                processor.defaultName = "";
         
                if (path.endsWith(".xml"))          processor.loadGalleryFromPath(path);
                else  if (path.endsWith(".json"))   processor.loadJsonGalleryFromPath(path);
                
                DBG("HeaderViewController::bkComboBoxDidChange combobox text = " + galleryCB.getText());
            }
            
        }
        else
        {
            cb->setSelectedId(lastGalleryCBId, dontSendNotification);
        }
    }
}

