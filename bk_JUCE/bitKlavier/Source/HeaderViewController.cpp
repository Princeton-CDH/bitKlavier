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
construction(c)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
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
    galleryCB.addMyListener(this);
    //galleryCB.BKSetJustificationType(juce::Justification::centredRight);
    
    galleryCB.setSelectedId(0, dontSendNotification);
    lastGalleryCBId = galleryCB.getSelectedId();
    
    // Piano CB
    addAndMakeVisible(pianoCB);
    pianoCB.setName("pianoCB");
    pianoCB.addListener(this);
    pianoCB.addMyListener(this);
    pianoCB.BKSetJustificationType(juce::Justification::centredRight);
    
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
    float width = getWidth() / 5 - gXSpacing;
    
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
}

PopupMenu HeaderViewController::getLoadMenu(void)
{
    PopupMenu loadMenu;
    loadMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    int count = 0;
    
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
    pianoMenu.addItem(2, "Duplicate");
    pianoMenu.addItem(3, "Delete");
    
    pianoMenu.addSeparator();
    pianoMenu.addSubMenu("Add...", getNewMenu());
    pianoMenu.addSubMenu("Edit...", getEditMenu());
    pianoMenu.addSeparator();
   
    //pianoMenu.addSeparator();
    //pianoMenu.addItem(3, "Settings");
    
    return pianoMenu;
}



PopupMenu HeaderViewController::getGalleryMenu(void)
{
    PopupMenu galleryMenu;
    galleryMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    galleryMenu.addSubMenu("Samples...", getLoadMenu());
    galleryMenu.addSeparator();
    galleryMenu.addItem(SETTINGS_ID, "Settings...");
    galleryMenu.addSeparator();
    galleryMenu.addItem(NEWGALLERY_ID, "New...");
    galleryMenu.addSeparator();
    galleryMenu.addItem(OPEN_ID, "Open...");
    galleryMenu.addItem(OPENOLD_ID, "Open (legacy)...");
    galleryMenu.addSeparator();
    
    String saveKeystroke = "(Cmd-S)";
    String saveAsKeystroke = "(Shift-Cmd-S)";
    
#if JUCE_WINDOWS
    saveKeystroke = "(Ctrl-S)";
	saveAsKeystroke = "(Shift-Ctrl-S)";
#endif
    
    galleryMenu.addItem(SAVE_ID, "Save " );
    galleryMenu.addItem(SAVEAS_ID, "Save as... ");
    galleryMenu.addSeparator();
    galleryMenu.addItem(CLEAN_ID, "Clean");
    galleryMenu.addSeparator();
    galleryMenu.addItem(DELETE_ID, "Delete");
    
    
    return galleryMenu;
    
}

PopupMenu HeaderViewController::getEditMenu(void)
{
    PopupMenu menu;
    menu.setLookAndFeel(&buttonsAndMenusLAF);
    
    menu.addItem(KEYMAP_EDIT_ID, "Keymap");
    menu.addItem(DIRECT_EDIT_ID, "Direct");
    menu.addItem(NOSTALGIC_EDIT_ID, "Nostalgic");
    menu.addItem(SYNCHRONIC_EDIT_ID, "Synchronic");
    menu.addItem(TUNING_EDIT_ID, "Tuning");
    menu.addItem(TEMPO_EDIT_ID, "Tempo");
    menu.addItem(DIRECTMOD_EDIT_ID, "Direct Mod");
    menu.addItem(NOSTALGICMOD_EDIT_ID, "Nostalgic Mod");
    menu.addItem(SYNCHRONICMOD_EDIT_ID, "Synchronic Mod");
    menu.addItem(TUNINGMOD_EDIT_ID, "Tuning Mod");
    menu.addItem(TEMPOMOD_EDIT_ID, "Tempo Mod");
    
    return menu;
}

PopupMenu HeaderViewController::getNewMenu(void)
{
    PopupMenu newMenu;
    newMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    newMenu.addItem(KEYMAP_ID, "Keymap (k)");
    newMenu.addItem(DIRECT_ID, "Direct (d)");
    newMenu.addItem(NOSTALGIC_ID, "Nostalgic (n)");
    newMenu.addItem(SYNCHRONIC_ID, "Synchronic (s)");
    newMenu.addItem(TUNING_ID, "Tuning (t)");
    newMenu.addItem(TEMPO_ID, "Tempo (m)");
    newMenu.addItem(MODIFICATION_ID, "Modification (c)");
    newMenu.addItem(PIANOMAP_ID, "Piano Map (p)");
    newMenu.addItem(RESET_ID, "Reset (r)");
    
    return newMenu;
}

void HeaderViewController::loadMenuCallback(int result, HeaderViewController* gvc)
{
    gvc->processor.loadPianoSamples((BKSampleLoadType)(result-1));
}

void HeaderViewController::pianoMenuCallback(int result, HeaderViewController* hvc)
{
    BKAudioProcessor& processor = hvc->processor;
    BKConstructionSite* construction = hvc->construction;
    
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
        
        if ((index == 0) && (hvc->pianoCB.getNumItems() == 1)) return;
        
        processor.gallery->remove(PreparationTypePiano, pianoId);
        
        hvc->fillPianoCB();
        
        int newPianoId = hvc->pianoCB.getItemId(index);
        
        if (newPianoId == 0) newPianoId = hvc->pianoCB.getItemId(index-1);
        
        hvc->pianoCB.setSelectedId(newPianoId, dontSendNotification);
        
        processor.setCurrentPiano(newPianoId);
    }
    else if (result == KEYMAP_ID)
    {
        construction->addItem(PreparationTypeKeymap, true);
    }
    else if (result == DIRECT_ID)
    {
        construction->addItem(PreparationTypeDirect, true);
    }
    else if (result == NOSTALGIC_ID)
    {
        construction->addItem(PreparationTypeNostalgic, true);
    }
    else if (result == SYNCHRONIC_ID)
    {
        construction->addItem(PreparationTypeSynchronic, true);
    }
    else if (result == TUNING_ID)
    {
        construction->addItem(PreparationTypeTuning, true);
    }
    else if (result == TEMPO_ID)
    {
        construction->addItem(PreparationTypeTempo, true);
    }
    else if (result == MODIFICATION_ID)
    {
        construction->addItem(PreparationTypeGenericMod, true);
    }
    else if (result == PIANOMAP_ID)
    {
        construction->addItem(PreparationTypePianoMap, true);
    }
    else if (result == RESET_ID)
    {
        construction->addItem(PreparationTypeReset, true);
    }
    
    // EDIT
    else if (result == KEYMAP_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeKeymap);
    }
    else if (result == DIRECT_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeDirect);
    }
    else if (result == NOSTALGIC_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeNostalgic);
    }
    else if (result == SYNCHRONIC_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeSynchronic);
    }
    else if (result == TUNING_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeTuning);
    }
    else if (result == TEMPO_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeTempo);
    }
    else if (result == DIRECTMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeDirectMod);
    }
    else if (result == NOSTALGICMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeNostalgicMod);
    }
    else if (result == SYNCHRONICMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeSynchronicMod);
    }
    else if (result == TUNINGMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeTuningMod);
    }
    else if (result == TEMPOMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeTempoMod);
    }
    
}

void HeaderViewController::galleryMenuCallback(int result, HeaderViewController* gvc)
{
    BKAudioProcessor& processor = gvc->processor;
    
    if (result == LOAD_LITE)
    {
        processor.loadPianoSamples(BKLoadLite);
    }
    else if (result == LOAD_MEDIUM)
    {
        processor.loadPianoSamples(BKLoadMedium);
    }
    else if (result == LOAD_HEAVY)
    {
        processor.loadPianoSamples(BKLoadHeavy);
    }
    else if (result == SAVE_ID)
    {
        processor.saveGallery();
    }
    if (result == SAVEAS_ID)
    {
        processor.saveGalleryAs();
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
        processor.deleteGallery();
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
        DBG(name);
        
        if (result == 1)
        {
            processor.createNewGallery(name);
        }
    }
}

void HeaderViewController::bkButtonClicked (Button* b)
{
    String name = b->getName();
    
    if (b == &pianoB)
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

void HeaderViewController::fillGalleryCB(void)
{
    
    if(!galleryModalCallBackIsOpen)
    {
        galleryCB.clear(dontSendNotification);
        
        File bkGalleries;
        
        File moreGalleries = File::getSpecialLocation(File::userDocumentsDirectory);
        
#if JUCE_MAC || JUCE_WINDOWS
        bkGalleries = bkGalleries.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("galleries");
#endif
        
#if JUCE_IOS
        
        bkGalleries = bkGalleries.getSpecialLocation(File::invokedExecutableFile).getParentDirectory().getChildFile("bitKlavier resources").getChildFile("galleries");
    
#endif
        
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
            if(creatingSubmenu && thisFile.getParentDirectory().getFileName() != submenuName)
            {
                galleryCBPopUp->addSubMenu(submenuName, *submenus.getLast());
                creatingSubmenu = false;
            }
            
            //add toplevel item, if there is one
            if(thisFile.getParentDirectory().getFileName() == bkGalleries.getFileName() ||
               thisFile.getParentDirectory().getFileName() == moreGalleries.getFileName()) //if the file is in the main galleries directory....
            {
                galleryCB.addItem(galleryName, i+1); //add to toplevel popup
            }
            
            //otherwise add to or create submenu with name of subfolder
            else
            {
                creatingSubmenu = true;
                
                submenuName = thisFile.getParentDirectory().getFileName(); //name of submenu
                
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
        
        DBG("pianoName: " + String(piano->getName()));
        
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

void HeaderViewController::BKEditableComboBoxChanged(String text, BKEditableComboBox* cb)
{
    if (cb == &pianoCB)
    {
        processor.currentPiano->setName(text);
    }
    else if (cb == &galleryCB)
    {
        processor.renameGallery(text);
        fillGalleryCB();
    }
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
        processor.saveGallery();
        
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
    int index = cb->getSelectedItemIndex();

    
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
            String path = processor.galleryNames[index];
            lastGalleryCBId = Id;
            
            if (path.endsWith(".xml"))          processor.loadGalleryFromPath(path);
            else  if (path.endsWith(".json"))   processor.loadJsonGalleryFromPath(path);
        }
        else
        {
            cb->setSelectedId(lastGalleryCBId, dontSendNotification);
        }
    }
}

