/*
  ==============================================================================

    GalleryViewController.cpp
    Created: 27 Mar 2017 12:47:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "GalleryViewController.h"

GalleryViewController::GalleryViewController (BKAudioProcessor& p):
processor (p)
{
    
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

GalleryViewController::~GalleryViewController()
{
    
}

void GalleryViewController::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}

void GalleryViewController::resized()
{
    float width = getWidth() / 7 - gXSpacing;
    float height = getHeight() - 2 * gYSpacing;
    
    loadB          .setBounds(gXSpacing, gYSpacing, width, height);
    
    galleryCB       .setBounds(loadB.getRight() + gXSpacing, gYSpacing, 2*width, height);
    
    galleryB .setBounds(galleryCB.getRight() + gXSpacing, gYSpacing, width, height);
    
    pianoCB         .setBounds(galleryB.getRight() + gXSpacing, gYSpacing, 2*width, height);
    
    pianoB   .setBounds(pianoCB.getRight() + gXSpacing, gYSpacing, width, height);
    
}

PopupMenu GalleryViewController::getLoadMenu(void)
{
    PopupMenu loadMenu;
    
    int count = 0;
    for (auto loadType : cBKSampleLoadTypes)
        loadMenu.addItem(++count, loadType);
    
    return loadMenu;
}

PopupMenu GalleryViewController::getPianoMenu(void)
{
    PopupMenu pianoMenu;
    
    pianoMenu.addItem(1, "New");
    pianoMenu.addItem(2, "Delete");
    pianoMenu.addSeparator();
    pianoMenu.addItem(3, "Settings");
    
    return pianoMenu;
}

PopupMenu GalleryViewController::getGalleryMenu(void)
{
    PopupMenu galleryMenu;
    
    galleryMenu.addItem(1, "Save");
    galleryMenu.addItem(2, "Load");
    galleryMenu.addSeparator();
    galleryMenu.addItem(3, "Load (Old)");
    
    return galleryMenu;
    
    
}

void GalleryViewController::loadMenuCallback(int result, GalleryViewController* gvc)
{
    gvc->processor.loadPianoSamples((BKSampleLoadType)(result-1));
}

void GalleryViewController::pianoMenuCallback(int result, GalleryViewController* gvc)
{
    if (result == 1) // New piano
    {
        gvc->processor.gallery->addPiano();
        
        int Id = gvc->processor.gallery->getPianos().size();
        String newName = "Piano"+String(Id);
        
        gvc->pianoCB.changeItemText(Id, newName);
        
        gvc->processor.gallery->getPianos().getLast()->setName(newName);
        
        gvc->pianoCB.addItem("New piano...", Id+1 );
        
        gvc->pianoCB.setSelectedId(Id);
        
        gvc->processor.setCurrentPiano(Id-1);
    }
    else if (result == 2) // Remove piano
    {
        int pianoId = gvc->pianoCB.getSelectedId()-1;
        
        gvc->processor.gallery->removePiano(pianoId);
        
        gvc->switchGallery();
    }
    else if (result == 3)
    {
        gvc->processor.updateState->currentPreparationDisplay =  DisplayGeneral;
        gvc->processor.updateState->displayDidChange = true;
    }
}

void GalleryViewController::galleryMenuCallback(int result, GalleryViewController* gvc)
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

void GalleryViewController::bkButtonClicked (Button* b)
{
    String name = b->getName();
    
    if (b == &loadB)
    {
        PopupMenu loadMenu = getLoadMenu();
        
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

void GalleryViewController::fillGalleryCB(void)
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

void GalleryViewController::update(void)
{
    if (processor.updateState->galleryDidChange)
    {
        processor.updateState->galleryDidChange = false;
        
        switchGallery();
    }
}

void GalleryViewController::switchGallery()
{
    pianoCB.clear(dontSendNotification);
    for (int i = 0; i < processor.gallery->getPianos().size(); i++)     pianoCB.addItem(processor.gallery->getPiano(i)->getName(), i+1);
    pianoCB.addItem("New piano...", processor.gallery->getPianos().size()+1);
    
    pianoCB.setSelectedId(1, dontSendNotification);
    
    fillGalleryCB();
    
    
}

void GalleryViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
}

void GalleryViewController::BKEditableComboBoxChanged(String text, BKEditableComboBox* cb)
{
    //String name = cb->getName();
    //int selectedId = cb->getSelectedId();
    
    processor.currentPiano->setName(text);
    
    //pianoCB.changeItemText(selectedId, text);
    
    //pianoCB.setSelectedId(selectedId);
    
    
    /*
     processor.currentPiano->setName(text);
     
     int selected = pianoCB.getSelectedId();
     if (selected != pianoCB.getNumItems()) pianoCB.changeItemText(selected, text);
     
     pianoCB.setSelectedId(selected, dontSendNotification );
    */
}


void GalleryViewController::bkComboBoxDidChange            (ComboBox* cb)
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

