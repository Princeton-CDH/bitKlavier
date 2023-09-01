/*
  ==============================================================================

    HeaderViewController.cpp
    Created: 27 Mar 2017 12:47:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PluginEditor.h"

#include "HeaderViewController.h"
#include "MainViewController.h"
HeaderViewController::HeaderViewController (BKAudioProcessor& p, BKConstructionSite* c):
processor (p),
construction(c)
{
    setLookAndFeel(new BKButtonAndMenuLAF());

    addAndMakeVisible(galleryB);
    galleryB.setButtonText("Gallery");
    galleryB.setTooltip("Create, duplicate, rename, share current Gallery. Also access bitKlavier settings");
    galleryB.addListener(this);

    addAndMakeVisible(pianoB);
    pianoB.setButtonText("Piano");
    pianoB.setTooltip("Create, duplicate, rename, or delete current Piano");
    pianoB.addListener(this);
    
    addAndMakeVisible(editB);
    editB.setButtonText("Action");
    editB.setTooltip("Add a preparation. Also, stop all internal bitKlavier signals");
    editB.addListener(this);
    
    // Gallery CB
    addAndMakeVisible(galleryCB);
    galleryCB.setName("galleryCB");
    galleryCB.setTooltip("Select and load saved bitKlavier Galleries. Indicates currently loaded Gallery");
    galleryCB.addListener(this);
    //galleryCB.BKSetJustificationType(juce::Justification::centredRight);
    BKButtonAndMenuLAF* comboBoxLeftJustifyLAF = new BKButtonAndMenuLAF();
    galleryCB.setLookAndFeel(comboBoxLeftJustifyLAF);
    galleryCB.setSelectedId(0, dontSendNotification);
    galleryCB.beforeOpen = [this] { processor.collectGalleries(); fillGalleryCB(); };
    
    // Piano CB
    addAndMakeVisible(pianoCB);
    pianoCB.setName("pianoCB");
    pianoCB.setTooltip("Select and load saved bitKlavier Pianos. Indicates currently loaded Piano");
    pianoCB.addListener(this);
    pianoCB.beforeOpen = [this] { processor.collectPianos(); fillPianoCB(); };
    
#if JUCE_IOS || JUCE_MAC
    bot.setBounds(0,0,20,20);
    addAndMakeVisible(bot);
#endif
    
    BKButtonAndMenuLAF* comboBoxRightJustifyLAF = new BKButtonAndMenuLAF();
    pianoCB.setLookAndFeel(comboBoxRightJustifyLAF);
    comboBoxRightJustifyLAF->setComboBoxJustificationType(juce::Justification::centredRight);

    pianoCB.setSelectedId(0, dontSendNotification);
    
    galleryModalCallBackIsOpen = false;
    
    //loadDefaultGalleries();
    
    fillGalleryCB();
    fillPianoCB();
    processor.updateState->pianoDidChangeForGraph = true;
    
    addMouseListener(this, true);
}

HeaderViewController::~HeaderViewController()
{
    PopupMenu::dismissAllActiveMenus();
    galleryCB.setLookAndFeel(nullptr);
    pianoCB.setLookAndFeel(nullptr);
    
    pianoB.setLookAndFeel(nullptr);
    galleryB.setLookAndFeel(nullptr);
    editB.setLookAndFeel(nullptr);
    
    setLookAndFeel(nullptr);
    
    galleryB.removeListener(this);
    pianoB.removeListener(this);
    editB.removeListener(this);
    galleryCB.removeListener(this);
    pianoCB.removeListener(this);
    
    removeMouseListener(this);
}

void HeaderViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void HeaderViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    area.reduce(2, gYSpacing);
    
    float width = area.getWidth() / 7;
    
    galleryB.setBounds(area.removeFromLeft(width));
    
    area.removeFromLeft(gXSpacing);
    galleryCB.setBounds(area.removeFromLeft(2*width));
    
    area.removeFromRight(1);
    pianoB.setBounds(area.removeFromRight(width));
    
    area.removeFromRight(gXSpacing);
    pianoCB.setBounds(area.removeFromRight(2*width));
    area.removeFromRight(gXSpacing);
    
    area.removeFromLeft(gXSpacing);
    editB.setBounds(area);
}

void HeaderViewController::mouseDown(const MouseEvent &event)
{

}

PopupMenu HeaderViewController::getLoadMenu(void)
{
    BKPopupMenu loadMenu;
    
    loadMenu.addItem(LOAD_LITEST,   "Lightest", processor.globalSampleType != BKLoadLitest, processor.globalSampleType == BKLoadLitest);
    loadMenu.addItem(LOAD_LITE,     "Light", processor.globalSampleType != BKLoadLite, processor.globalSampleType == BKLoadLite);
    loadMenu.addItem(LOAD_MEDIUM,   "Medium", processor.globalSampleType != BKLoadMedium, processor.globalSampleType == BKLoadMedium);
    loadMenu.addItem(LOAD_HEAVY,    "Heavy", processor.globalSampleType != BKLoadHeavy, processor.globalSampleType == BKLoadHeavy);
    
    loadMenu.addSeparator();
    
#if JUCE_IOS
    
    loadMenu.addItem(SF_DEFAULT_0, "Rhodes", true, false);
    loadMenu.addItem(SF_DEFAULT_1, "Harpsichord", true, false);
    loadMenu.addItem(SF_DEFAULT_2, "Drums", true, false);
    loadMenu.addItem(SF_DEFAULT_3, "Saw", true, false);
    loadMenu.addItem(SF_DEFAULT_4, "Electric Bass", true, false);
    loadMenu.addItem(SF_DEFAULT_5, "Acoustic Kit", true, false);
    loadMenu.addItem(SF_DEFAULT_6, "Organ", true, false);
    
    loadMenu.addSeparator();
    
#endif
    
    int i = 0;
    for (auto sf : processor.soundfontNames)
    {
        String sfName = sf.fromLastOccurrenceOf("/", false, true).upToFirstOccurrenceOf(".sf", false, true);
        sfName = sfName.replace("%20", " ");
        loadMenu.addItem(SOUNDFONT_ID + (i++), sfName);
    }
    
    return std::move(loadMenu);
}

PopupMenu HeaderViewController::getExportedPianoMenu(void)
{
    BKPopupMenu menu;
    
    StringArray names = processor.exportedPianos;
    for (int i = 0; i < names.size(); i++)
    {
        menu.addItem(i+100, names[i]);
    }
    
    return std::move(menu);
}

PopupMenu HeaderViewController::getPianoMenu(void)
{
    BKPopupMenu pianoMenu;
    
    pianoMenu.addItem(1, "New");
    pianoMenu.addItem(6, "Duplicate");
    pianoMenu.addItem(2, "Linked Copy");
    pianoMenu.addItem(4, "Rename");
    pianoMenu.addItem(3, "Remove");
    pianoMenu.addSeparator();
    pianoMenu.addItem(7, "Export");
    
    PopupMenu exported = getExportedPianoMenu();
    pianoMenu.addSubMenu("Import...", exported);
    
    return std::move(pianoMenu);
}

PopupMenu HeaderViewController::getGalleryMenu(void)
{
    BKPopupMenu galleryMenu;
    
    galleryMenu.addItem(NEWGALLERY_ID, "New");
    
    if (processor.wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
    {
        galleryMenu.addItem(SAVE_ID, "Save" + gSaveShortcut);
        galleryMenu.addItem(SAVEAS_ID, "Save as" + gSaveAsShortcut);
    }
    else
    {
        galleryMenu.addItem(SAVE_ID, "Save");
        galleryMenu.addItem(SAVEAS_ID, "Save as");
    }
    
    if (!processor.defaultLoaded)
    {
        galleryMenu.addItem(RENAME_ID, "Rename");
        galleryMenu.addItem(DELETE_ID, "Remove");
    }
    
#if JUCE_IOS
    galleryMenu.addSeparator();
    galleryMenu.addItem(EXPORT_ID, "Export");
    galleryMenu.addItem(IMPORT_ID, "Import");
#else
    galleryMenu.addItem(OPEN_ID, "Open");
    galleryMenu.addItem(OPENOLD_ID, "Open (legacy)");
#endif
    
    galleryMenu.addSeparator();
    galleryMenu.addItem(ITERATOR_ID, "Piano Iterator");
    
    galleryMenu.addSeparator();
    galleryMenu.addItem(CLEAN_ID, "Clean");
    
#if JUCE_IOS
    galleryMenu.addSeparator();
    galleryMenu.addSubMenu("Load samples...", getLoadMenu());
#endif
    
    // ~ ~ ~ share menu ~ ~ ~
    if (!processor.defaultLoaded)
    {
#if JUCE_MAC
    BKPopupMenu shareMenu;
    
    shareMenu.addItem(SHARE_EMAIL_ID, "Email");
    galleryMenu.addSeparator();
    shareMenu.addItem(SHARE_MESSAGE_ID, "Messages");
    galleryMenu.addSeparator();
    //shareMenu.addItem(SHARE_FACEBOOK_ID, "Facebook");
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    galleryMenu.addSeparator();
    galleryMenu.addSubMenu("Share", shareMenu);
#elif JUCE_IOS
    galleryMenu.addSeparator();
    galleryMenu.addItem(SHARE_MESSAGE_ID, "Share");
#endif
    }
    
    galleryMenu.addSeparator();
    galleryMenu.addItem(SETTINGS_ID, "Settings");

    galleryMenu.addSeparator();
    galleryMenu.addItem(ABOUT_ID, "About bitKlavier...");
    
    return std::move(galleryMenu);
    
}

void HeaderViewController::pianoMenuCallback(int res, HeaderViewController* hvc)
{
    if (hvc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = hvc->processor;
    
    if (res == 1) // New piano
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
            
            processor.saveGalleryToHistory("Add Piano");
        }
    }
    else if (res == 2) // Linked Copy
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addTextEditor("name", processor.currentPiano->getName() + " Linked Copy");
        
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
            //processor.setClipboard(clipboard)
            processor.saveGalleryToHistory("Linked Copy Piano");
        }
    }
    else if (res == 3) // Remove piano
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
        
        processor.saveGalleryToHistory("Remove Piano");
    }
    else if (res == 4) // Rename
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
    else if (res == 6) // Duplicate
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addTextEditor("name", processor.currentPiano->getName() + " Copy");
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            BKItem::PtrArr clipboard = processor.getClipboard();
            
            BKConstructionSite* construction = hvc->construction;
            construction->selectAll();
            construction->copy();

            int newId = processor.gallery->getNewId(PreparationTypePiano);
            processor.gallery->addTypeWithId(PreparationTypePiano, newId);
            processor.gallery->getPianos().getLast()->setName(name);
            hvc->fillPianoCB();
            processor.setCurrentPiano(newId);
            
            construction->paste(false);
            
            processor.setClipboard(clipboard);
            
            processor.saveGalleryToHistory("Duplicate Piano");
        }
    }
    else if (res == 7)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.currentPiano->getId();
        Piano::Ptr piano = processor.currentPiano;
        
        prompt.addTextEditor("name", piano->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPiano(Id, name);
        }
    }
    else if (res >= 100)
    {
        int which = res - 100;
        processor.importPiano(processor.currentPiano->getId(), which);
    }
}

void HeaderViewController::galleryMenuCallback(int result, HeaderViewController* gvc)
{
    if (gvc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
	BKAudioProcessor& processor = gvc->processor;

	if (result == RENAME_ID)
	{
		AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);

		prompt.addTextEditor("name", processor.gallery->getName().upToFirstOccurrenceOf(".xml", false, false));

		prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
		prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));

		int result = prompt.runModalLoop();

		String name = prompt.getTextEditorContents("name");

		if (result == 1)
		{
			processor.renameGallery(name);
		}

		gvc->fillGalleryCB();
	}
#if (JUCE_MAC || JUCE_IOS)
	else if (result == SHARE_EMAIL_ID)
	{
		gvc->bot.share(processor.gallery->getURL(), 0);
	}
	else if (result == SHARE_MESSAGE_ID)
	{
		gvc->bot.share(processor.gallery->getURL(), 1);
	}
	else if (result == SHARE_FACEBOOK_ID)
	{
		gvc->bot.share(processor.gallery->getURL(), 2);
	}
#endif
    else if (result == LOAD_LITEST)
    {
        processor.loadSamples(BKLoadLitest);
    }
    else if (result == LOAD_LITE)
    {
        processor.loadSamples(BKLoadLite);
    }
    else if (result == LOAD_MEDIUM)
    {
        processor.loadSamples(BKLoadMedium);
    }
    else if (result == LOAD_HEAVY)
    {
        processor.loadSamples(BKLoadHeavy);
    }
    else if (result >= SF_DEFAULT_0 && result < SOUNDFONT_ID)
    {
        processor.loadSamples(BKLoadSoundfont, "default.sf" + String(result - SF_DEFAULT_0), 0);
    }
    else if (result == SAVE_ID && !processor.defaultLoaded)
    {
        processor.saveCurrentGallery();
        
        //processor.createGalleryWithName(processor.gallery->getName());
    }
    else if (result >= SOUNDFONT_ID && result < MIDIOUT_ID)
    {
        processor.loadSamples(BKLoadSoundfont, processor.soundfontNames[result-SOUNDFONT_ID], 0);
    }
    else if (result == SAVEAS_ID || (result == SAVE_ID && processor.defaultLoaded))
    {
#if JUCE_IOS
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon, gvc);
        
        prompt.addTextEditor("name", processor.gallery->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        
        prompt.setTopLeftPosition(gvc->getWidth() / 2, gvc->getBottom() + gYSpacing);
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            String url = File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName() + "/" + name.upToFirstOccurrenceOf(".xml", false, false) + ".xml";
            processor.writeCurrentGalleryToURL(url);
        }
        
        gvc->fillGalleryCB();
#else
        processor.saveCurrentGalleryAs();
#endif
    }
    else if (result == EXPORT_ID)
    {
#if JUCE_IOS
        processor.exportCurrentGallery();
#endif
    }
    else if (result == IMPORT_ID)
    {
#if JUCE_IOS
        processor.importCurrentGallery();
#endif
    }
    else if (result == OPEN_ID) // Load
    {
        processor.loadGalleryDialog();
        //auto comp = getParentComponent();
        
    }
    else if (result == ITERATOR_ID) // open Piano Iterator
    {
        DBG("iterator button check");
        processor.getBKEditor()->showPianoIteratorDialog();
    }
    else if (result == SETTINGS_ID) // open General settings
    {
        processor.getBKEditor()->showGenSettings(0);
    }
    else if (result == CLEAN_ID) // Clean
    {
        processor.gallery->clean();
    }
    else if (result == DELETE_ID) // Delete
    {
        AlertWindow prompt("", "Are you sure you want to delete this gallery?", AlertWindow::AlertIconType::QuestionIcon);
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            processor.deleteGallery();
        }
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
    else if (result == ABOUT_ID)
    {
        processor.updateState->setCurrentDisplay(DisplayAbout);
    }
//    else if (result >= MIDIOUT_ID)
//    {
//        // Try to open the output selected from the Gallery menu
//        Array<MidiDeviceInfo> availableOutputs = MidiOutput::getAvailableDevices();
//        MidiDeviceInfo outputInfo = availableOutputs[result - MIDIOUT_ID];
//        std::unique_ptr<MidiOutput> output = MidiOutput::openDevice(outputInfo.identifier);
//        // If the output successfully opens
//        if (output) {
//            DBG("MIDI output set to " + outputInfo.name);
//            processor.midiOutput = std::move(output);
//        }
//        else {
//            DBG("Could not open MIDI output device");
//        }
//    }
}

void HeaderViewController::bkButtonClicked (Button* b)
{
    String name = b->getName();
    
    if (b == &editB)
    {
        if (processor.wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
        {
             getEditMenuStandalone(new BKButtonAndMenuLAF(), construction->getNumSelected()).showMenuAsync(PopupMenu::Options().withTargetComponent (b), ModalCallbackFunction::forComponent(BKConstructionSite::editMenuCallback, construction) );
        }
        else
        {
            getEditMenu(new BKButtonAndMenuLAF(), construction->getNumSelected()).showMenuAsync(PopupMenu::Options().withTargetComponent(b), ModalCallbackFunction::forComponent(BKConstructionSite::editMenuCallback, construction) );
        }
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
        
        PopupMenu mikroetudes_menu, ns_etudes_menu, bk_examples_menu, machines_menu;
        
        //data = BinaryData::Basic_Piano_xml;
        for (int i = 0; i < BinaryData::namedResourceListSize; i++)
        {
            resource = BinaryData::namedResourceList[i];
            
            if (resource.contains("_xml") && !std::regex_match(resource.toStdString(),std::regex("^Compressor_.*$")))
            {
                data = BinaryData::getNamedResource(BinaryData::namedResourceList[i], size);
                
                name = data.fromFirstOccurrenceOf("<gallery name=\"", false, true).upToFirstOccurrenceOf("\"", false, true);

                if (processor.mikroetudes.contains(name))                   mikroetudes_menu.addItem(id++, name);
                else if (processor.ns_etudes.contains(name))                ns_etudes_menu.addItem(id++, name);
                else if (processor.bk_examples.contains(name))              bk_examples_menu.addItem(id++, name);
                else if (processor.machines_for_listening.contains(name))   machines_menu.addItem(id++, name);
                else                                                        galleryCB.addItem(name, id++);
                
            }
        }
        
        popupRoot->addSubMenu("Examples", bk_examples_menu);
        popupRoot->addSubMenu("Nostalgic Synchronic", ns_etudes_menu);
        popupRoot->addSubMenu("Mikroetudes", mikroetudes_menu);
        popupRoot->addSubMenu("Machines for Listening", machines_menu);
        
        galleryCB.addSeparator();
        
        numberOfDefaultGalleryItems = galleryCB.getNumItems();
    }
}
#if (!JUCE_IOS)
void HeaderViewController::fillGalleryCB(void)
{
    if (processor.gallery == nullptr) return;
    
    if(!galleryModalCallBackIsOpen)
    {
        loadDefaultGalleries();
        
        // File bkGalleries;
        
        // File moreGalleries = File::getSpecialLocation(File::userDocumentsDirectory);
        
#if (JUCE_MAC)
        // bkGalleries = bkGalleries.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
		char divChar = '/';
#endif
#if (JUCE_WINDOWS || JUCE_LINUX)
        // bkGalleries = bkGalleries.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
		char divChar = '\\';
#endif
        
        PopupMenu* galleryCBPopUp = galleryCB.getRootMenu();
        
        int id = numberOfDefaultGalleryItems, index = 0;
        
		StringArray nameStack; //used for naviagting between levels of submenu during loading
		OwnedArray<PopupMenu> submenus; //2d arrray of submenus modelling the recursive depth of submenu connections
		StringArray submenuNames; //lists the submenu names so far; used to check against 
		OwnedArray<Array<int>> parentIds; //2d int array used to record recursive depth of indices for parent menus
		OwnedArray<Array<int>> childIds; //2d int array used to record recursive depth of indices for child menus

		//first pass through the list: create menus and add galleries to them
		for (int i = 0; i < processor.galleryNames.size(); i++)
		{
			File thisFile(processor.galleryNames[i]);
			String galleryName = thisFile.getFileName().fromLastOccurrenceOf(File::getSeparatorString(), false, true).upToFirstOccurrenceOf(".xml", false, false);
          
            
			String galleryPath = String(thisFile.getFullPathName());
			int gallerySplitIndex = galleryPath.indexOf("galleries") + 10;
			StringArray galleryFolders;
			int divIndex = galleryPath.indexOfChar(gallerySplitIndex, divChar);
			//split the filename into individual strings for folders
            String parent = thisFile.getParentDirectory().getFileName();
           
            if (((divIndex == -1) && (parent != "galleries")) || !galleryPath.contains("galleries"))
            {
                galleryFolders.add(thisFile.getParentDirectory().getFileName());
            }
            else {
                while (divIndex != -1)
                {
                    //DBG("Adding " + galleryPath.substring(gallerySplitIndex, divIndex) + " to galleryFolders");
                    galleryFolders.add(galleryPath.substring(gallerySplitIndex, divIndex));
                    gallerySplitIndex = divIndex + 1;
                    divIndex = galleryPath.indexOfChar(gallerySplitIndex, divChar);
                }
                
            }
			if (!submenus.isEmpty())
			{
				StringRef poppedMenuName = nameStack[nameStack.size() - 1];
				while ((!galleryFolders.contains(poppedMenuName)) && (!nameStack.isEmpty()))
				{
					nameStack.remove(nameStack.size() - 1);
					poppedMenuName = nameStack[nameStack.size() - 1];
				}
			}
			//submenu creation for nested folders
			if (galleryFolders.size() >= 1) 
			{
				//iterate through the folder names and create submenus if they don't exist
				for (int j = 0; j < galleryFolders.size(); j++)
				{
					if (!submenuNames.contains(galleryFolders[j]))
					{
						if (parentIds.size() <= j)
						{
							parentIds.add(new Array<int>());
							childIds.add(new Array<int>());
						}
						submenus.add(new PopupMenu());
						nameStack.add(galleryFolders[j]);
						submenuNames.add(galleryFolders[j]);
						int childIdToBeAdded = submenus.size() - 1;
						childIds[j]->add(childIdToBeAdded); // adds the current submenu id to the childids list

						//adds the id of the current submenu's parent menu to the list of parent ids
						if (j == 0)
						{
							parentIds[j]->add(-1);
							//DBG("Assigning submenu " + galleryFolders[j] + " to main menu parent");
						}
						else
						{
							parentIds[j]->add(submenuNames.indexOf(galleryFolders[j - 1]));
							//DBG("Assigning submenu " + galleryFolders[j] + " to parent menu " + galleryFolders[j - 1]);
						}
					}
				}
				//add gallery to its appropriate submenu
				int submenuIndex = submenuNames.indexOf(galleryFolders[galleryFolders.size() - 1]);
				submenus[submenuIndex]->addItem(++id, galleryName);
				DBG("Successfully added " + galleryName + " to submenu " + galleryFolders[galleryFolders.size() - 1]);
			}
			//if there are no submenus, just add the gallery to the main menu
			else
			{
				galleryCB.addItem(galleryName, ++id);
				DBG("Successfully added " + galleryName + " to main menu");
			}
			if (thisFile.getFullPathName() == processor.currentGalleryPath)
			{
				index = i;
				lastGalleryCBId = id;
			}
		}

		//pass through the submenus to add the nested submenus to their parent menus
		for (int i = parentIds.size() - 1; i >= 0; i--)
		{
			for (int j = 0; j < parentIds[i]->size(); j++)
			{
				int childId = childIds[i]->getReference(j);
				int parentId = parentIds[i]->getReference(j);
				if (i == 0)
				{
					galleryCBPopUp->addSubMenu(submenuNames[childId], *submenus[childId]);
					//DBG("Adding " + submenuNames[childId] + " submenu to the main menu");
				}
				else
				{
					submenus[parentId]->addSubMenu(submenuNames[childId], *submenus[childId]);
					//DBG("Adding " + submenuNames[childId] + " submenu to parent menu " + submenuNames[parentId]);
				}
			}
		}

        // THIS IS WHERE NAME OF GALLERY DISPLAYED IS SET
        galleryCB.setSelectedId(lastGalleryCBId, NotificationType::dontSendNotification);
        galleryCB.setText(processor.gallery->getName().upToFirstOccurrenceOf(".xml", false, true), NotificationType::dontSendNotification);
    }
}

#else //now IOS
void HeaderViewController::fillGalleryCB(void)
{
    if (processor.gallery == nullptr) return;
    
    if(!galleryModalCallBackIsOpen)
    {
        loadDefaultGalleries();
        
        File bkGalleries;
        File moreGalleries = File::getSpecialLocation(File::userDocumentsDirectory);
 
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
            
            if (thisFile.getFullPathName() == processor.currentGalleryPath)
            {
                index = i;
                lastGalleryCBId = id;
            }
        }
        
        //add last submenu to popup, if there is one
        if(creatingSubmenu)
        {
            galleryCBPopUp->addSubMenu(submenuName, *submenus.getLast());
            creatingSubmenu = false;
        }
        
        // THIS IS WHERE NAME OF GALLERY DISPLAYED IS SET
        galleryCB.setSelectedId(lastGalleryCBId, NotificationType::dontSendNotification);
        galleryCB.setText(processor.gallery->getName().upToFirstOccurrenceOf(".xml", false, true), NotificationType::dontSendNotification);
    }
}
#endif

void HeaderViewController::update(void)
{
    if (processor.updateState->currentDisplay == DisplayNil)
    {
        editB.setEnabled(true);
        pianoB.setEnabled(true);
        galleryB.setEnabled(true);
        pianoCB.setEnabled(true);
        galleryCB.setEnabled(true);
    }
    else
    {
        editB.setEnabled(false);
        pianoB.setEnabled(false);
        galleryB.setEnabled(false);
        pianoCB.setEnabled(false);
        galleryCB.setEnabled(false);
    }
}

void HeaderViewController::switchGallery()
{
    fillGalleryCB();
    fillPianoCB();
}

void HeaderViewController::fillPianoCB(void)
{
    if (processor.gallery == nullptr) return;
    
    pianoCB.clear(dontSendNotification);

	//now alphabetizes the list (turning into string) before putting it into the combo box
	StringArray nameIDStrings = StringArray();

	//concatenate name and ID into a single string using the newline character (something a user couldn't type) as a separator, then sorts the list of strings
	for (auto piano : processor.gallery->getPianos())
	{
		String name = piano->getName();
		String idString = String(piano->getId());
		if (name != String()) nameIDStrings.add(name + char(10) + idString);
		else nameIDStrings.add("Piano" + String(idString) + char(10) + idString);
	}

	nameIDStrings.sort(false);

	//separates the combined string into name and ID parts, then adds to the combo box
	for (auto combo : nameIDStrings)
	{
		int separatorIndex = combo.indexOfChar(char(10));
		String name = combo.substring(0, separatorIndex);
		int id = combo.substring(separatorIndex + 1).getIntValue();
		pianoCB.addItem(name, id);
	}

	//old code
    /*for (auto piano : processor.gallery->getPianos())
    {
        String name = piano->getName();
        
        if (name != String())  pianoCB.addItem(name,  piano->getId());
        else                        pianoCB.addItem("Piano" + String(piano->getId()), piano->getId());
    }*/
    if (processor.currentPiano == nullptr) jassert("bad");
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
    
    if(processor.gallery->iteratorIsEnabled)
    {
    
        int iterPianos = processor.gallery->getPianoIteratorOrder().getNumItems();
        if(iterPianos > 0) processor.gallery->setGalleryDirty(true);
    }
    
    if(processor.gallery->isGalleryDirty())
    {
        DBG("GALLERY IS DIRTY, CHECK FOR SAVE HERE");
        galleryModalCallBackIsOpen = true; //not sure, maybe should be doing some kind of Lock
        
        galleryIsDirtyAlertResult = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon,
                                                                     "The current gallery has changed.",
                                                                     "Do you want to save first?",
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
        
        if (processor.defaultLoaded)
        {
            AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
            
            prompt.addTextEditor("name", processor.gallery->getName());
            
            prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
            prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
            
            int result = prompt.runModalLoop();
            
            String name = prompt.getTextEditorContents("name").upToFirstOccurrenceOf(".xml", false, false);
            
            if (result == 1)
            {
#if JUCE_IOS
                File newFile = File::getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
                File newFile = File::getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
                File newFile = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
                
                String newURL = newFile.getFullPathName() + name + ".xml";
                processor.writeCurrentGalleryToURL(newURL);
            }
        }
        else
        {
            processor.saveCurrentGallery();
        }
        
        fillGalleryCB();
        
        shouldSwitch = true;
    }
    else if(galleryIsDirtyAlertResult == 2)
    {
        // DBG("not saving");
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
        
        processor.saveGalleryToHistory("Change Piano");
        
        std::vector<Piano::Ptr>::iterator it =  std::find_if(processor.gallery->getPianoIteratorOrder().modelData.begin(),processor.gallery->getPianoIteratorOrder().modelData.end(), [&](const auto& val)
                             { return val->getId() == Id;});
        DBG("index" + String( std::distance(processor.gallery->getPianoIteratorOrder().modelData.begin(), it)));
        processor.gallery->currentPianoIndex = std::distance(processor.gallery->getPianoIteratorOrder().modelData.begin(), it);
        processor.updateState->currentIteratorPiano = processor.gallery->currentPianoIndex;
        processor.updateState->updateIterator = true;
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
                String xmlData = CharPointer_UTF8 (BinaryData::getNamedResource(BinaryData::namedResourceList[index], size));
                
                processor.defaultLoaded = true;
                processor.defaultName = BinaryData::namedResourceList[index];
                processor.currentGalleryPath = processor.defaultName;
                processor.loadGalleryFromXml(XmlDocument::parse(xmlData).get(),"");
            }
            else
            {
                index = index - numberOfDefaultGalleryItems;
				//processor.galleryNames.sortNatural();
                String path = processor.galleryNames[index];
                
                /*
                // ok this kludge at least makes it work, but not the best, as duplicate names or substrings could get mismapped
                for (auto tpath : processor.galleryNames)
                {
                    if (tpath.contains(galleryCB.getText()))
                        path = tpath;
                }
                 */
                
                processor.defaultLoaded = false;
                processor.defaultName = "";
                processor.currentGalleryPath = path;
                if (path.endsWith(".xml"))          processor.loadGalleryFromPath(path);
                else  if (path.endsWith(".json"))   processor.loadJsonGalleryFromPath(path);
                
                DBG("HeaderViewController::bkComboBoxDidChange combobox text / id / path = "
                    + galleryCB.getText() + " "
                    + String(index) + " "
                    + path);
            }
            
        }
        else
        {
            cb->setSelectedId(lastGalleryCBId, dontSendNotification);
        }
        MainViewController* mvc = dynamic_cast<MainViewController*> (getParentComponent());
        if(mvc)
            mvc->fillSampleCB();
        if(processor.getBKEditor()->iteratorDoc != nullptr)
        {
            processor.getBKEditor()->iteratorDoc->closeButtonPressed();
            processor.getBKEditor()->iteratorDoc = nullptr;
        }
    }
}
