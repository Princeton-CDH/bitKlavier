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
loadvc(new BKComponent()),
pvc(new BKComponent()),
pmvc(PreparationMapViewController::PtrArr()),
kvc(p),
gvc(p),
svc(p),
nvc(p),
dvc(p),
tvc(p)
{
    
    startTimerHz (50);
    
    // Make PianoViewController component within plugin editor class.
    addAndMakeVisible(pvc);
    addAndMakeVisible(gvc);
    addAndMakeVisible(svc);
    addAndMakeVisible(nvc);
    addAndMakeVisible(dvc);
    addAndMakeVisible(kvc);
    addAndMakeVisible(tvc);
    
    
    // Piano + PianoMap initialization
    
    pianoL.setName(cPianoParameterTypes[0]);
    pianoL.setText(cPianoParameterTypes[0], NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(pianoL);
    
    pianoCB.setName(cPianoParameterTypes[0]);
    pianoCB.addSeparator();
    pianoCB.addListener(this);
    pvc->addAndMakeVisible(pianoCB);
    for (int i = 0; i < aMaxNumPianos; i++) pianoCB.addItem(cPianoName[i], i+1);
    pianoCB.setSelectedItemIndex(0);
    
    pianoMapL.setName("PianoMap");
    pianoMapL.setText("PianoMap", NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(pianoMapL);
    
    
    pianoMapTF.addListener(this);
    pianoMapTF.setName("PianoMap");
    pvc->addAndMakeVisible(pianoMapTF);
    
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
    
    addPMapButton.setBounds(pvc->getX(),
                            pvc->getBottom() + gYSpacing,
                            50,
                            20);
    
    setSize(gMainComponentWidth,
            gMainComponentHeight);
    
    // Preparation Map initialization
    pmapH = cPrepMapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    pmvc.ensureStorageAllocated(12);

}

BKAudioProcessorEditor::~BKAudioProcessorEditor()
{
    
}

void BKAudioProcessorEditor::timerCallback()
{
    if (processor.pianoDidChange)
    {
        processor.pianoDidChange = false;
        switchPianos();

    }
    
    if (processor.preparationDidChange)
    {
        processor.preparationDidChange = false;
        dvc.updateFields();
        nvc.updateFields();
        svc.updateFields();
    }
    
}

void BKAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}



void BKAudioProcessorEditor::resized()
{
    float loadvcH = (20 + gYSpacing) + 1.5 * gYSpacing;
    float pvcH = 2 * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float kvcH = cKeymapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 *  gYSpacing;
    float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float tvcH = cTuningParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    
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
    pianoL      .setTopLeftPosition(0,                                  gYSpacing);
    pianoCB     .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   pianoL.getY());
    pianoMapL   .setTopLeftPosition(0,                                  pianoL.getBottom() + gYSpacing);
    pianoMapTF  .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   pianoMapL.getY());
    
    
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
    
    
    
}


String BKAudioProcessorEditor::processPianoMapString(const String& s)
{
    String temp = "";
    String out = "";
    
    bool isNumber = false;
    bool isKeymap = false;
    bool isColon  = false;
    bool isSpace = false;
    bool isEndOfString = false;
    
    bool itsAKeymap = false;
    bool itsAColon = false;
    bool itsASpace = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar colon = ':';
    juce_wchar keymap = 'k';
    
    Array<int> keys;
    
    for (auto map : processor.currentPiano->pianoMap)    map = 0;
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        isColon     = !CharacterFunctions::compare(c1, colon);
        isKeymap    = !CharacterFunctions::compare(c1, keymap);
        isNumber    = CharacterFunctions::isDigit(c1);
        isSpace     = CharacterFunctions::isWhitespace(c1);
        if (i==s.length()) isEndOfString = true;
        
        
        
        if (!isNumber)
        {
            if (!itsAColon && isColon)
            {
                itsAColon = true;
                if (itsAKeymap)
                    keys = processor.bkKeymaps[temp.getIntValue()]->keys();
                else
                    keys.add(temp.getIntValue());
                
                temp = "";
            }
            else if (!itsASpace && (isEndOfString || isSpace))
            {
                itsASpace = true;
                
                int whichPiano = temp.getIntValue();

                if (whichPiano > 0)
                {
                    // Set piano map parameters.
                    for (auto key : keys)
                    {
                        out += (String(key) + ":" +String(whichPiano) + " ");
                        processor.currentPiano->pianoMap.set(key, whichPiano);
                    }
                }
                
                itsAKeymap = false;
                itsAColon = false;
                
                temp = "";
                
                keys.clearQuick();
            }
            else if (!itsAKeymap && isKeymap)
            {
                itsAKeymap = true;
            }
            else
            {
                itsASpace = false;
                continue;
            }
        }
        else
        {
            itsASpace = false;
            temp += c1;
        }
        
    }
    
    return out;
}

void BKAudioProcessorEditor::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    //float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    if (name == "PianoMap")
    {
        tf.setText(processPianoMapString(text));
    }
    
}

void BKAudioProcessorEditor::bkButtonClicked (Button* b)
{
    if (b->getName() == "Add")
    {
        drawNewPreparationMap(processor.currentPiano->addPreparationMap());
    }
    else if (b->getName() == "Remove")
    {
        removeLastPreparationMap(processor.currentPiano->removeLastPreparationMap());
    }
    else if (b->getName() == "Load Lite")
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

void BKAudioProcessorEditor::bkComboBoxDidChange            (ComboBox* box)
{
    // Change piano
    if (box->getName() == cPianoParameterTypes[PianoCBPiano])
    {
        int whichPiano = box->getSelectedId();
        
        processor.setCurrentPiano(whichPiano-1);
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
        
    pianoMapTF.setText(temp);
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

//==============================================================================

