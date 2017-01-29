/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GraphicsConstants.h"

#include "PianoViewController.h"



//==============================================================================
BKAudioProcessorEditor::BKAudioProcessorEditor (BKAudioProcessor& p):
AudioProcessorEditor (&p),
processor (p),
pvc(new PianoViewController (p)),
pmvc(PreparationMapViewController::PtrArr()),
kvc(p),
gvc(p),
svc(p),
nvc(p),
dvc(p),
tvc(p)
{
    pmapH = cPrepMapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    
    pmvc.ensureStorageAllocated(12);
    
    // Initialize piano stuff here.
    // Labels
    pianoL.ensureStorageAllocated(cPianoParameterTypes.size());
    
    for (int i = 0; i < cPianoParameterTypes.size(); i++)
    {
        pianoL.set(i, new BKLabel());
        pianoL[i]->setName(cPianoParameterTypes[i]);
        pianoL[i]->setText(cPianoParameterTypes[i], NotificationType::dontSendNotification);
        pvc->addAndMakeVisible(pianoL[i]);
    }
    
    //Combo Boxes
    pianoCB.ensureStorageAllocated(cPianoParameterTypes.size());
    
    for (int i = 0; i < cPianoParameterTypes.size(); i++)
    {
        pianoCB.set(i, new BKComboBox());
        pianoCB[i]->setName(cPianoParameterTypes[i]);
        pianoCB[i]->addSeparator();
        pianoCB[i]->addListener(this);
        pvc->addAndMakeVisible(pianoCB[i]);
    }
    
    for (int i = 0; i < aMaxNumPianos; i++)
    {
        pianoCB[PianoCBPiano]->addItem(cPianoName[i], i+1);
    }
    
    pianoCB[PianoCBPiano]->setSelectedItemIndex(0);
    
    // Make PianoViewController component within plugin editor class.
    addAndMakeVisible(pvc);
    addAndMakeVisible(gvc);
    addAndMakeVisible(svc);
    addAndMakeVisible(nvc);
    addAndMakeVisible(dvc);
    addAndMakeVisible(kvc);
    addAndMakeVisible(tvc);
    
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
    
    addPMapButton.setBounds(upperLeft.getX(),
                            upperLeft.getBottom() + gYSpacing,
                            50,
                            20);
    
    setSize(gMainComponentWidth,
            gMainComponentHeight);
    
}

void BKAudioProcessorEditor::buttonClicked (Button* b)
{
    if (b->getName() == "Add")
    {
        drawNewPreparationMap(processor.currentPiano->addPreparationMap());
    }
    else if (b->getName() == "Remove")
    {
        removeLastPreparationMap(processor.currentPiano->removeLastPreparationMap());
    }
}


BKAudioProcessorEditor::~BKAudioProcessorEditor()
{
    
}

void BKAudioProcessorEditor::comboBoxChanged            (ComboBox* box)
{
    // Change piano
    if (box->getName() == cPianoParameterTypes[PianoCBPiano])
    {
        int whichPiano = box->getSelectedId();
        
        DBG("Current piano: " + String(whichPiano-1));
        
        processor.setCurrentPiano(whichPiano-1);
        
        switchPianos();
    }
}

void BKAudioProcessorEditor::switchPianos(void)
{
    // Remove all pmaps from old piano.
    for (int i = processor.prevPiano->numPMaps; --i >= 0; )
    {
        pmvc[i]->setVisible(false);
        
        removeChildComponent(pmvc[i]);
        
        pmvc.remove(i);
    }
    
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
        
        if (i > 0) {
            pmvc[i]->setBounds(gComponentLeftOffset,
                                pmvc[i-1]->getBottom() + gYSpacing,
                                gVCWidth,
                                pmapH);
        } else {
            pmvc[i]->setBounds(gComponentLeftOffset,
                                upperLeft.getBottom() + gYSpacing,
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
    }
    
    if (lastId > 0)     removePMapButton.setVisible(true);
    else                removePMapButton.setVisible(false);
}


void BKAudioProcessorEditor::removeLastPreparationMap(int Id)
{
    
    if (Id < 0) return;
    
    DBG("ID: " + String(Id));
    
    pmvc[Id]->setVisible(false);
    
    removeChildComponent(pmvc[Id]);
    
    pmvc.remove(Id);
    
    int newLastId = Id - 1;
    
    if (newLastId < 0)
    {
        addPMapButton.setBounds(upperLeft.getX(),
                                upperLeft.getBottom() + gYSpacing,
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

void BKAudioProcessorEditor::drawNewPreparationMap(int Id)
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
    
    if (Id > 0) {
        pmvc[Id]->setBounds(gComponentLeftOffset,
                            pmvc[Id-1]->getBottom() + gYSpacing,
                            gVCWidth,
                            pmapH);
    } else {
        pmvc[Id]->setBounds(gComponentLeftOffset,
                            upperLeft.getBottom() + gYSpacing,
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

//==============================================================================
void BKAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
    
}



void BKAudioProcessorEditor::resized()
{
    float pvcH = cPianoParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float kvcH = cKeymapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 *  gYSpacing;
    float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float tvcH = cTuningParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    
    // Col 1
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cPianoParameterTypes.size(); n++)
        pianoL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    
    // CB
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cPianoParameterTypes.size(); n++)
        pianoCB[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    
    pvc->setBounds(gComponentLeftOffset,
                   gComponentTopOffset,
                   gVCWidth,
                   pvcH);
    
    upperLeft = pvc->getBounds();
    
    addAndMakeVisible(pvc);
    
    addPMapButton.setBounds(upperLeft.getX(), upperLeft.getBottom() + gYSpacing, 50, 20);
    
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
    
    
    
}
