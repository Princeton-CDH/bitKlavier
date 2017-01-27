/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GraphicsConstants.h"


//==============================================================================
BKAudioProcessorEditor::BKAudioProcessorEditor (BKAudioProcessor& p):
AudioProcessorEditor (&p),
processor (p),
pvc(p),
pmvc(PreparationMapViewController::PtrArr()),
kvc(p),
gvc(p),
svc(p),
nvc(p),
dvc(p),
tvc(p)
{
    float pvcH = cPianoParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    
    upperLeft = Rectangle<int>(gComponentLeftOffset, gComponentTopOffset, gVCWidth, pvcH);
    
    pmapH = cPrepMapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    
    pmvc.ensureStorageAllocated(12);
    
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

void BKAudioProcessorEditor::removeLastPreparationMap(int Id)
{
    
    if (Id < 0) return;
    
    DBG("ID: " + String(Id));
    
    pmvc[Id]->setVisible(false);
    
    pvc.removeActionListener(pmvc[Id]);
    
    removeChildComponent(pmvc[Id]);
    
    pmvc.remove(Id);
    
    int newLastId = Id - 1;
    
    if (newLastId > 0)
    {
        addPMapButton.setBounds(pmvc[newLastId]->getX(),
                                pmvc[newLastId]->getBottom() + gYSpacing,
                                50,
                                20);
        
        removePMapButton.setBounds(addPMapButton.getRight() + gXSpacing,
                                   addPMapButton.getY(),
                                   50,
                                   20);
    }
    else
    {
        addPMapButton.setBounds(upperLeft.getX(),
                                upperLeft.getBottom() + gYSpacing,
                                50,
                                20);
        
        
        removePMapButton.setVisible(false);
    }
}

void BKAudioProcessorEditor::drawNewPreparationMap(int Id)
{
    
    if (Id < 0) return;
    
    pmvc.insert(Id, new PreparationMapViewController(processor, Id));
    
    addAndMakeVisible(pmvc[Id]);
    
    pvc.addActionListener(pmvc[Id]);
    
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
    float kvcH = cKeymapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 *  gYSpacing;
    float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float tvcH = cTuningParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    
    // Col 1
    pvc.setBounds(upperLeft);
    
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
