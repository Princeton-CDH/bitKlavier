/*
  ==============================================================================

    GalleryViewController.cpp
    Created: 27 Mar 2017 12:47:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "GalleryViewController.h"

GalleryViewController::GalleryViewController (BKAudioProcessor& p):
processor (p),
loadvc(new BKComponent()),
pvc(new BKComponent()),
pmvc(PreparationMapViewController::PtrArr())
{
    // Make PianoViewController component within plugin editor class.
    addAndMakeVisible(pvc);
    
    pianoMapL.setName("PianoMap");
    pianoMapL.setText("PianoMap", NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(pianoMapL);
    
    pianoMapTF.addListener(this);
    pianoMapTF.setName("PianoMap");
    pvc->addAndMakeVisible(pianoMapTF);
    
    pianoNameL.setName("PianoName");
    pianoNameL.setText("PianoName", NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(pianoNameL);
    
    pianoNameTF.addListener(this);
    pianoNameTF.setName("PianoName");
    pvc->addAndMakeVisible(pianoNameTF);
    
    modMapL.setName("ModMap");
    modMapL.setText("ModMap", NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(modMapL);
    
    modMapTF.addListener(this);
    modMapTF.setName("ModMap");
    pvc->addAndMakeVisible(modMapTF);
    
    resetMapL.setName("ResetMap");
    resetMapL.setText("ResetMap", NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(resetMapL);
    
    resetMapTF.addListener(this);
    resetMapTF.setName("ResetMap");
    pvc->addAndMakeVisible(resetMapTF);
    
    pianoL.setName(cPianoParameterTypes[0]);
    pianoL.setText(cPianoParameterTypes[0], NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(pianoL);
    
    pianoCB.setName(cPianoParameterTypes[0]);
    pianoCB.addSeparator();
    pianoCB.addListener(this);
    pvc->addAndMakeVisible(pianoCB);
    pianoCB.addItem("MyFirstPiano", 1);
    processor.gallery->getPiano(0)->setName("MyFirstPiano");
    pianoCB.addItem("New piano...",2);
    pianoCB.setSelectedId(0, NotificationType::dontSendNotification);
    
    // Piano + PianoMap initialization
    galleryL.setName("Galleries");
    galleryL.setText("Galleries", NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(galleryL);
    
    galleryCB.setName("Galleries");
    galleryCB.addSeparator();
    galleryCB.addListener(this);
    galleryCB.setSelectedId(0, dontSendNotification);
    pvc->addAndMakeVisible(galleryCB);
    
    
    // Load buttons
    loadButtons.ensureStorageAllocated(cBKSampleLoadTypes.size());
    
    for (int i = 0; i < cBKSampleLoadTypes.size(); i++)
    {
        loadButtons.set(i, new TextButton());
        
        loadButtons[i]->setName(cBKSampleLoadTypes[i]);
        loadButtons[i]->changeWidthToFitText();
        loadButtons[i]->setButtonText(cBKSampleLoadTypes[i]);
        loadButtons[i]->addListener(this);
        loadvc->addAndMakeVisible(loadButtons[i]);
    }
    
    
    saveButton.setName("Save");
    saveButton.changeWidthToFitText();
    saveButton.setButtonText("Save");
    saveButton.addListener(this);
    pvc->addAndMakeVisible(saveButton);
    
    /*
    loadButton.setName("Load");
    loadButton.changeWidthToFitText();
    loadButton.setButtonText("Load");
    loadButton.addListener(this);
    addAndMakeVisible(loadButton);
    
    loadJsonButton.setName("LoadJson");
    loadJsonButton.changeWidthToFitText();
    loadJsonButton.setButtonText("LoadJson");
    loadJsonButton.addListener(this);
    addAndMakeVisible(loadJsonButton);
     */
    
    addPMapButton.setName("Add");
    addPMapButton.changeWidthToFitText();
    addPMapButton.setButtonText("Add");
    addPMapButton.addListener(this);
    addAndMakeVisible(addPMapButton);
    
    removePMapButton.setName("Remove");
    removePMapButton.changeWidthToFitText();
    removePMapButton.setButtonText("Remove");
    removePMapButton.addListener(this);
    addAndMakeVisible(removePMapButton);
    
    removePianoButton.setName("RemovePiano");
    removePianoButton.changeWidthToFitText();
    removePianoButton.setButtonText("Remove");
    removePianoButton.addListener(this);
    pvc->addAndMakeVisible(removePianoButton);
    
    addPMapButton.setBounds(pvc->getX(),
                            pvc->getBottom() + gYSpacing,
                            50,
                            20);
    
    // Preparation Map initialization
    pmapH = cPrepMapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    pmvc.ensureStorageAllocated(12);
    
    fillGalleryCB();
    
    
}

void GalleryViewController::update(void)
{
    if (processor.updateState->galleryDidChange)
    {
        processor.updateState->galleryDidChange = false;
        switchGallery();
    }
    
    if (processor.updateState->pianoDidChange)
    {
        processor.updateState->pianoDidChange = false;
        switchPianos();
    }
    
}

GalleryViewController::~GalleryViewController()
{
    
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


void GalleryViewController::switchGallery()
{
    pianoCB.clear(dontSendNotification);
    for (int i = 0; i < processor.gallery->getPianos().size(); i++)     pianoCB.addItem(processor.gallery->getPiano(i)->getName(), i+1);
    pianoCB.addItem("New piano...", processor.gallery->getPianos().size()+1);
    pianoCB.setSelectedId(1);
    
    fillGalleryCB();
    
    
}

void GalleryViewController::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}



void GalleryViewController::resized()
{
    float loadvcH = (20 + gYSpacing) + 1.5 * gYSpacing;
    float pvcH = 6 * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    
    
    loadvc->setBounds(gComponentLeftOffset,
                      gComponentTopOffset,
                      gVCWidth,
                      loadvcH);
    
    pvc->setBounds(gComponentLeftOffset,
                   loadvc->getBottom()+gYSpacing,
                   gVCWidth,
                   pvcH);
    
    // Col 1
    // Load Buttons
    int bW = gVCWidth / 3.0 - gXSpacing;
    int bH = 20;
    
    for (int n = 0; n < cBKSampleLoadTypes.size(); n++) {
        loadButtons[n]->setBounds((bW + gXSpacing) * n + gXSpacing / 2.0,
                                  gYSpacing,
                                  bW,
                                  bH);
    }
    
    
    // Piano
    galleryL      .setTopLeftPosition(0,                                  gYSpacing);
    galleryCB     .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   galleryL.getY());
    
    pianoL      .setTopLeftPosition(0,                                  galleryL.getBottom() + gYSpacing);
    pianoCB     .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   pianoL.getY());
    
    
    saveButton.setBounds(galleryCB.getX()-55, galleryCB.getY(), 50, 20);
    removePianoButton.setBounds(pianoCB.getX()-55, pianoCB.getY(), 50, 20);
    
    pianoNameL  .setTopLeftPosition(0,                                  pianoL.getBottom() + gYSpacing);
    pianoNameTF .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   pianoNameL.getY());
    
    pianoMapL   .setTopLeftPosition(0,                                  pianoNameL.getBottom() + gYSpacing);
    pianoMapTF  .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   pianoMapL.getY());
    
    modMapL     .setTopLeftPosition(0,                                  pianoMapL.getBottom() + gYSpacing);
    modMapTF    .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   modMapL.getY());
    
    resetMapL     .setTopLeftPosition(0,                                  modMapL.getBottom() + gYSpacing);
    resetMapTF    .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   resetMapL.getY());
    
    upperLeft = loadvc->getBounds();
    

    addAndMakeVisible(loadvc);
    addAndMakeVisible(pvc);
    
    if (pmvc.size())
    {
        addPMapButton.setBounds(pvc->getX(), pmvc.getLast()->getBottom() + gYSpacing, 50, 20);
        removePMapButton.setBounds(addPMapButton.getRight() + gXSpacing, addPMapButton.getY(), 50, 20);
    }
    else
        addPMapButton.setBounds(pvc->getX(), pvc->getBottom() + gYSpacing, 50, 20);
    
    
    /*
    loadButton.setBounds(saveButton.getRight() + gXSpacing, getBottom() - 75, 50, 20);
    
    loadJsonButton.setBounds(loadButton.getRight() + gXSpacing, getBottom() - 75, 50, 20);
     */
    
    
}

void GalleryViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    //float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    if (name == "PianoMap")
    {
        tf.setText(processPianoMapString(text), false);
    }
    else if (name == "ModMap")
    {
        tf.setText(processModMapString(text), false);
    }
    else if (name == "ResetMap")
    {
        tf.setText(processResetMapString(text), false);
    }
    else if (name == "PianoName")
    {
        processor.currentPiano->setName(text);
        
        int selected = pianoCB.getSelectedId();
        if (selected != pianoCB.getNumItems()) pianoCB.changeItemText(selected, text);
        pianoCB.setSelectedId(selected, dontSendNotification );
        
        
    }
    
}

void GalleryViewController::bkButtonClicked (Button* b)
{
    if (b->getName() == "Save")
    {
        processor.saveGallery();
    }
    else if (b->getName() == "Load")
    {
        processor.loadGalleryDialog();
    }
    else if (b->getName() == "LoadJson")
    {
        processor.loadJsonGalleryDialog();
    }
    else if (b->getName() == "Add")
    {
        drawNewPreparationMap(processor.currentPiano->addPreparationMap());
    }
    else if (b->getName() == "Remove")
    {
        removeLastPreparationMap(processor.currentPiano->removeLastPreparationMap());
    }
    else if (b->getName() == "RemovePiano")
    {
        int selected = pianoCB.getSelectedId();
        
        processor.gallery->removePiano(selected-1);
        
        switchGallery();
    }
    else if (b->getName() == "Load Light")
    {
        processor.loadPianoSamples(BKLoadLite);
    }
    else if (b->getName() == "Load Medium")
    {
        processor.loadPianoSamples(BKLoadMedium);
    }
    else if (b->getName() == "Load Heavy")
    {
        processor.loadPianoSamples(BKLoadHeavy);
    }
}

void GalleryViewController::bkComboBoxDidChange            (ComboBox* box)
{
    // Change piano
    int which = box->getSelectedId();
    
    if (box->getName() == cPianoParameterTypes[PianoCBPiano])
    {
        // Add piano if New piano... pressed.
        if (which == pianoCB.getNumItems())
        {
            processor.gallery->addPiano();
            
            String newName = "Piano"+String(processor.gallery->getPianos().size());
            
            pianoCB.changeItemText(which, newName);
            processor.gallery->getPianos().getLast()->setName(newName);
            
            pianoCB.addItem("New piano...", which+1);
        }
        
        processor.setCurrentPiano(which-1);
    }
    else if (box->getName() == "Galleries")
    {
        String path = processor.galleryNames[which-1];
        if (path.endsWith(".xml"))          processor.loadGalleryFromPath(path);
        else  if (path.endsWith(".json"))   processor.loadJsonGalleryFromPath(path);
    }
    
}

void GalleryViewController::switchPianos(void)
{
    pmvc.clearQuick();
    
    for (int i = 0; i < processor.currentPiano->numPMaps; i++ )
    {
        pmvc.insert(i, new PreparationMapViewController(processor, i));
        
        addAndMakeVisible(pmvc[i]);
        
        if (i > 0) {
            pmvc[i]->setBounds(gComponentLeftOffset,
                               pmvc[i-1]->getBottom() + gYSpacing,
                               gVCWidth,
                               pmapH);
        } else {
            pmvc[i]->setBounds(gComponentLeftOffset,
                               pvc->getBottom() + gYSpacing,
                               gVCWidth,
                               pmapH);
        }
        
        
    }
    
    int lastId = processor.currentPiano->numPMaps - 1;
    
    if (lastId >= 0)
    {
        addPMapButton.setBounds(pmvc[lastId]->getX(),
                                pmvc[lastId]->getBottom() + gYSpacing,
                                50,
                                20);
        
        removePMapButton.setBounds(addPMapButton.getRight() + gXSpacing,
                                   pmvc[lastId]->getBottom() + gYSpacing,
                                   50,
                                   20);
        
        removePMapButton.setVisible(true);
    }
    else
    {
        addPMapButton.setBounds(pvc->getX(),
                                pvc->getBottom() + gYSpacing,
                                50,
                                20);
        
        removePMapButton.setVisible(false);
    }
    
    pianoCB.setSelectedId(processor.currentPiano->getId() + 1);
    
    String temp = "";
    Array<int> pianoMap = processor.currentPiano->pianoMap;
    for (int i = 0; i < pianoMap.size(); i++)
    {
        if (pianoMap[i] > 0) temp += (String(i) + ":" + String(pianoMap[i]) + " ");
    }
    
    pianoMapTF.setText(temp, false);
    
    
    String mm = processor.currentPiano->modificationMapsToString();
    modMapTF.setText(mm, false);
    
    pianoNameTF.setText(processor.currentPiano->getName(), false);
}


void GalleryViewController::removeLastPreparationMap(int Id)
{
    
    if (Id < 0) return;
    
    DBG("ID: " + String(Id));
    
    pmvc[Id]->setVisible(false);
    
    removeChildComponent(pmvc[Id]);
    
    pmvc.remove(Id);
    
    int newLastId = Id - 1;
    
    if (newLastId < 0)
    {
        addPMapButton.setBounds(pvc->getX(),
                                pvc->getBottom() + gYSpacing,
                                50,
                                20);
        
        removePMapButton.setVisible(false);
    }
    else
    {
        addPMapButton.setBounds(pmvc[newLastId]->getX(),
                                pmvc[newLastId]->getBottom() + gYSpacing,
                                50,
                                20);
        
        removePMapButton.setBounds(addPMapButton.getRight() + gXSpacing,
                                   addPMapButton.getY(),
                                   50,
                                   20);
        
        removePMapButton.setVisible(true);
        
    }
}

void GalleryViewController::drawNewPreparationMap(int Id)
{
    
    if (Id < 0) return;
    
    pmvc.insert(Id, new PreparationMapViewController(processor, Id));
    
    addAndMakeVisible(pmvc[Id]);
    
    if (Id > 0) {
        pmvc[Id]->setBounds(gComponentLeftOffset,
                            pmvc[Id-1]->getBottom() + gYSpacing,
                            gVCWidth,
                            pmapH);
    } else {
        pmvc[Id]->setBounds(gComponentLeftOffset,
                            pvc->getBottom() + gYSpacing,
                            gVCWidth,
                            pmapH);
    }
    
    addPMapButton.setBounds(pmvc[Id]->getX(),
                            pmvc[Id]->getBottom() + gYSpacing,
                            50,
                            20);
    
    if (processor.currentPiano->numPMaps)
    {
        removePMapButton.setBounds(addPMapButton.getRight() + gXSpacing,
                                   pmvc[Id]->getBottom() + gYSpacing,
                                   50,
                                   20);
        
        removePMapButton.setVisible(true);
    }
    else
    {
        removePMapButton.setVisible(false);
    }
    
}
