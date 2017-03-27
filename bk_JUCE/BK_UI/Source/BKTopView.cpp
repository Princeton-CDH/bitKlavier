/*
  ==============================================================================

    BKTopView.cpp
    Created: 27 Mar 2017 12:24:08pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKTopView.h"

BKTopViewController::BKTopViewController (BKAudioProcessor& p):
processor (p),
loadvc(new BKComponent()),
pvc(new BKComponent()),
pmvc(PreparationMapViewController::PtrArr()),
kvc(p),
gvc(p),
svc(p),
nvc(p),
dvc(p),
tvc(p),
ovc(p)
{
    
    
    
}

void BKTopViewController::update(void)
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

BKTopViewController::~BKTopViewController()
{
    
}

void BKTopViewController::fillGalleryCB(void)
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


void BKTopViewController::switchGallery()
{
    pianoCB.clear(dontSendNotification);
    for (int i = 0; i < processor.gallery->getPianos().size(); i++)     pianoCB.addItem(processor.gallery->getPiano(i)->getName(), i+1);
    pianoCB.addItem("New piano...", processor.gallery->getPianos().size()+1);
    pianoCB.setSelectedId(1);
    
    fillGalleryCB();
    
    
}

void BKTopViewController::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}



void BKTopViewController::resized()
{
    float loadvcH = (20 + gYSpacing) + 1.5 * gYSpacing;
    float pvcH = 6 * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float kvcH = cKeymapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 *  gYSpacing;
    float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float tvcH = cTuningParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float ovcH = cTempoParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    
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
    
    addPMapButton.setBounds(pvc->getX(), pvc->getBottom() + gYSpacing, 50, 20);
    
    // Col 2
    kvc.setBounds(upperLeft.getRight() + gXSpacing,
                  upperLeft.getY(),
                  gVCWidth,
                  kvcH);
    
    gvc.setBounds(kvc.getX(),
                  kvc.getBottom() + gYSpacing,
                  gVCWidth,
                  gvcH);
    
    tvc.setBounds(kvc.getX(),
                  gvc.getBottom() + gYSpacing,
                  gVCWidth,
                  tvcH);
    
    dvc.setBounds(kvc.getX(),
                  tvc.getBottom() + gYSpacing,
                  gVCWidth,
                  dvcH);
    
    
    // Col 3
    
    svc.setBounds(kvc.getRight() + gXSpacing,
                  upperLeft.getY(),
                  gVCWidth,
                  svcH);
    
    nvc.setBounds(svc.getX(),
                  svc.getBottom() + gYSpacing,
                  gVCWidth,
                  nvcH);
    
    ovc.setBounds(nvc.getX(),
                  nvc.getBottom() + gYSpacing,
                  gVCWidth,
                  ovcH);
    
    saveButton.setBounds(getX() + gXSpacing, getBottom() - 75, 50, 20);
    
    loadButton.setBounds(saveButton.getRight() + gXSpacing, getBottom() - 75, 50, 20);
    
    loadJsonButton.setBounds(loadButton.getRight() + gXSpacing, getBottom() - 75, 50, 20);
    
    
}

void BKTopViewController::bkTextFieldDidChange(TextEditor& tf)
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

void BKTopViewController::bkButtonClicked (Button* b)
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

void BKTopViewController::bkComboBoxDidChange            (ComboBox* box)
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

void BKTopViewController::switchPianos(void)
{
    pmvc.clearQuick();
    
    for (int i = 0; i < processor.currentPiano->numPMaps; i++ )
    {
        pmvc.insert(i, new PreparationMapViewController(processor, i));
        
        addAndMakeVisible(pmvc[i]);
        
        pmvc[i]->addActionListener(&gvc);
        pmvc[i]->addActionListener(&svc);
        pmvc[i]->addActionListener(&nvc);
        pmvc[i]->addActionListener(&dvc);
        pmvc[i]->addActionListener(&kvc);
        pmvc[i]->addActionListener(&tvc);
        pmvc[i]->addActionListener(&ovc);
        
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


void BKTopViewController::removeLastPreparationMap(int Id)
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

void BKTopViewController::drawNewPreparationMap(int Id)
{
    
    if (Id < 0) return;
    
    pmvc.insert(Id, new PreparationMapViewController(processor, Id));
    
    addAndMakeVisible(pmvc[Id]);
    
    pmvc[Id]->addActionListener(&gvc);
    pmvc[Id]->addActionListener(&svc);
    pmvc[Id]->addActionListener(&nvc);
    pmvc[Id]->addActionListener(&dvc);
    pmvc[Id]->addActionListener(&kvc);
    pmvc[Id]->addActionListener(&tvc);
    pmvc[Id]->addActionListener(&ovc);
    
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
