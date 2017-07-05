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
    galleryCB.BKSetJustificationType(juce::Justification::centredRight);
    
    galleryCB.setSelectedId(0, dontSendNotification);
    
    // Piano CB
    addAndMakeVisible(pianoCB);
    pianoCB.setName("pianoCB");
    pianoCB.addListener(this);
    pianoCB.addMyListener(this);
    
    pianoCB.addItem("New piano...",1);
    pianoCB.setSelectedId(0, dontSendNotification);
    

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
    pianoMenu.addItem(2, "Delete");
    pianoMenu.addSeparator();
    pianoMenu.addItem(3, "Settings");
    
    return pianoMenu;
}

PopupMenu HeaderViewController::getGalleryMenu(void)
{
    PopupMenu galleryMenu;
    galleryMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    galleryMenu.addItem(1, "Save");
    galleryMenu.addItem(2, "Load");
    galleryMenu.addSeparator();
    galleryMenu.addItem(3, "Load (Old)");
    
    return galleryMenu;
    
    
}

void HeaderViewController::loadMenuCallback(int result, HeaderViewController* gvc)
{
    gvc->processor.loadPianoSamples((BKSampleLoadType)(result-1));
}

void HeaderViewController::pianoMenuCallback(int result, HeaderViewController* hvc)
{
    if (result == 1) // New piano
    {
        int index = hvc->processor.gallery->getNumPianos();
        
        int newId = hvc->processor.gallery->getNewId(PreparationTypePiano);
        
        hvc->processor.gallery->addPianoWithId(newId);
        
        String newName = "Piano"+String(newId);
        
        hvc->processor.gallery->getPianos().getLast()->setName(newName);
        
        hvc->pianoCB.changeItemText(index, newName);
        
        hvc->pianoCB.addItem("New piano...", index );
        
        hvc->pianoCB.setSelectedId(index);
        
        hvc->processor.setCurrentPiano(newId);
    }
    else if (result == 2) // Remove piano
    {
        int pianoId = hvc->pianoCB.getSelectedId()-1;
        
        hvc->processor.gallery->removePiano(pianoId);
        
        hvc->switchGallery();
    }
    else if (result == 3)
    {
        hvc->processor.updateState->setCurrentDisplay(DisplayGeneral);
    }
}

void HeaderViewController::galleryMenuCallback(int result, HeaderViewController* gvc)
{
    if (result == 1)
    {
        gvc->processor.saveGallery();
    }
    else if (result == 2) // Load
    {
        gvc->processor.loadGalleryDialog();
    }
    else if (result == 3) // Load (old)
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
    galleryCB.clear(dontSendNotification);
    
    int index = 0;
    
    for (int i = 0; i < processor.galleryNames.size(); i++)
    {
        File thisFile(processor.galleryNames[i]);
        galleryCB.addItem(thisFile.getFileName(), i+1);
        
        if (thisFile.getFileName() == processor.currentGallery) index = i;
    }
    
    galleryCB.setSelectedId(index+1, NotificationType::dontSendNotification);
    
}

void HeaderViewController::update(void)
{
    if (processor.updateState->galleryDidChange)
    {
        processor.updateState->galleryDidChange = false;
        
        switchGallery();
    }
}

void HeaderViewController::switchGallery()
{
    pianoCB.clear(dontSendNotification);
    for (int i = 0; i < processor.gallery->getPianos().size(); i++)     pianoCB.addItem(processor.gallery->getPiano(i)->getName(), i+1);
    pianoCB.addItem("New piano...", processor.gallery->getPianos().size()+1);
    
    pianoCB.setSelectedId(1, dontSendNotification);
    
    fillGalleryCB();
    
    
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


void HeaderViewController::bkComboBoxDidChange            (ComboBox* cb)
{
    // Change piano
    String name = cb->getName();
    int selectedId = cb->getSelectedId();
    
    if (name == "pianoCB")
    {
        // Add piano if New piano... pressed.
        if (selectedId == pianoCB.getNumItems())
        {
            processor.gallery->addPiano();
            
            String newName = "Piano"+String(processor.gallery->getPianos().size());
            
            pianoCB.changeItemText(selectedId, newName);
            processor.gallery->getPianos().getLast()->setName(newName);
            
            pianoCB.setSelectedId(selectedId);
            
            pianoCB.addItem("New piano...", selectedId+1);
        }
        
        processor.setCurrentPiano(selectedId-1);
        
        
    }
    else if (name == "galleryCB")
    {
        String path = processor.galleryNames[selectedId-1];
        if (path.endsWith(".xml"))          processor.loadGalleryFromPath(path);
        else  if (path.endsWith(".json"))   processor.loadJsonGalleryFromPath(path);
    }
}

