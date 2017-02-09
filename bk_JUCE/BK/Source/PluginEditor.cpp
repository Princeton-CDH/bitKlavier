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
    
    modMapL.setName("ModMap");
    modMapL.setText("ModMap", NotificationType::dontSendNotification);
    pvc->addAndMakeVisible(modMapL);
    
    modMapTF.addListener(this);
    modMapTF.setName("ModMap");
    pvc->addAndMakeVisible(modMapTF);
    
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
    addAndMakeVisible(saveButton);
    
    loadButton.setName("Load");
    loadButton.changeWidthToFitText();
    loadButton.setButtonText("Load");
    loadButton.addListener(this);
    addAndMakeVisible(loadButton);
    
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
    
    if (processor.directPreparationDidChange)
    {
        processor.directPreparationDidChange = false;
        dvc.updateFields();
    }
    
    if (processor.nostalgicPreparationDidChange)
    {
        processor.nostalgicPreparationDidChange = false;
        nvc.updateFields();
    }
    
    if (processor.synchronicPreparationDidChange)
    {
        processor.synchronicPreparationDidChange = false;
        svc.updateFields();
    }
    
    if (processor.tuningPreparationDidChange)
    {
        processor.tuningPreparationDidChange = false;
        tvc.updateFields();
    }
    
}

void BKAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}



void BKAudioProcessorEditor::resized()
{
    float loadvcH = (20 + gYSpacing) + 1.5 * gYSpacing;
    float pvcH = 3 * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
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
    modMapL     .setTopLeftPosition(0,                                  pianoMapL.getBottom() + gYSpacing);
    modMapTF    .setTopLeftPosition(gComponentLabelWidth + gXSpacing,   modMapL.getY());
    
    
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
    
    saveButton.setBounds(getX() + gXSpacing, getBottom() - 25, 50, 20);
    
    loadButton.setBounds(saveButton.getRight() + gXSpacing, getBottom() - 25, 50, 20);
    
    
    
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

String BKAudioProcessorEditor::processModMapString(const String& s)
{
    String temp = "";
    String out = "";
    
    bool isNumber = false;
    bool isMod = false;
    bool isKeymap = false;
    bool isColon  = false;
    bool isSpace = false;
    bool isEndOfString = false;
    
    bool isSynchronic = false;
    bool isNostalgic = false;
    bool isTuning = false;
    bool isDirect= false;
    
    bool isBracket;
    
    bool itsAMod = false;
    bool itsAKeymap = false;
    bool lastColon = false;
    bool lastBracket = false;
    bool itsASpace = false;
    bool itsADirect = false;
    bool itsASynchronic = false;
    bool itsATuning = false;
    bool itsANostalgic = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar keymap = 'k';
    juce_wchar colon = ':';
    juce_wchar modLC = 'm';
    juce_wchar modUC = 'M';
    
    
    juce_wchar synchronicLC = 's';
    juce_wchar synchronicUC = 'S';
    juce_wchar nostalgicLC = 'n';
    juce_wchar nostalgicUC = 'N';
    juce_wchar tuningLC = 't';
    juce_wchar tuningUC = 'T';
    juce_wchar directLC = 'd';
    juce_wchar directUC = 'D';
    juce_wchar lBracket = '{';
    juce_wchar rBracket = '}';
    juce_wchar lBracket2 = '[';
    juce_wchar rBracket2 = ']';
    juce_wchar lBracket3 = '(';
    juce_wchar rBracket3 = ')';
    
    Array<int> keys;
    
    
    
    for (auto map : processor.currentPiano->pianoMap)    map = 0;
    
    int numColons, numBrackets;
    numColons = 0; numBrackets = 0;
    int whichMod = -1;
    
    Array<int> whichPreps;
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        
        isSynchronic   = !CharacterFunctions::compare(c1, synchronicLC) || !CharacterFunctions::compare(c1, synchronicUC);
        isNostalgic    = !CharacterFunctions::compare(c1, nostalgicLC) || !CharacterFunctions::compare(c1, nostalgicUC);
        isDirect       = !CharacterFunctions::compare(c1, directLC) || !CharacterFunctions::compare(c1, directUC);
        isTuning       = !CharacterFunctions::compare(c1, tuningLC) || !CharacterFunctions::compare(c1, tuningUC);
 
        isBracket   = !CharacterFunctions::compare(c1, lBracket) || !CharacterFunctions::compare(c1, rBracket) ||
                !CharacterFunctions::compare(c1, lBracket2) || !CharacterFunctions::compare(c1, rBracket2) ||
                !CharacterFunctions::compare(c1, lBracket3) || !CharacterFunctions::compare(c1, rBracket3);
        isKeymap    = !CharacterFunctions::compare(c1, keymap);
        isColon     = !CharacterFunctions::compare(c1, colon);
        isMod       = !CharacterFunctions::compare(c1, modLC) || !CharacterFunctions::compare(c1, modUC);
        isNumber    = CharacterFunctions::isDigit(c1);
        isSpace     = CharacterFunctions::isWhitespace(c1);
        
        if (i==s.length()) isEndOfString = true;
        
        if (!isNumber)
        {
            if (isColon)
            {
                if (numColons == 0)
                {
                    if (itsAKeymap)
                        keys = processor.bkKeymaps[temp.getIntValue()]->keys();
                    else
                        keys.add(temp.getIntValue());
                }
                else if (numColons == 1)
                {
                    whichMod = temp.getIntValue();
                }
                
                temp = "";
                
                if (++numColons == 2) lastColon = true;
            }
            else if ((numBrackets == 1) && isSpace)
            {
                whichPreps.add(temp.getIntValue());
                temp = "";
            }
            else if (isBracket)
            {
                if (numBrackets == 1 && temp != "") whichPreps.add(temp.getIntValue());
                
                if (++numBrackets == 2) lastBracket = true;
            }
            else if (isSpace || isEndOfString)
            {
                itsASpace = true;
                
                // Set piano map parameters.
                // keys | mod id | prepIds
                for (auto key : keys)
                {
                    
                    if (itsADirect)
                    {
                        out += (String(key) + ":dm" +String(whichMod) + ":" + "{" + intArrayToString(whichPreps) + "} ");
                        
                        DirectModPreparation::Ptr dmod = processor.modDirect[whichMod];
                        
                        for (int n = cDirectParameterTypes.size(); --n >= 0; )
                        {
                            String param = dmod->getParam((DirectParameterType)n);
                            
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    processor.currentPiano->modMap[key]->addDirectModification(new DirectModification(key, prep, (DirectParameterType)n, param, whichMod));
                                    DBG("whichmod: " + String(whichMod) +" whichprep: " + String(prep) + " whichtype: " + cDirectParameterTypes[n] + " val: " +param);
                                }
                            }
                        }
                    }
                    else if (itsANostalgic)
                    {
                        out += (String(key) + ":nm" +String(whichMod) + ":" + "{" + intArrayToString(whichPreps) + "} ");
                        
                        NostalgicModPreparation::Ptr nmod = processor.modNostalgic[whichMod];
                        
                        for (int n = cNostalgicParameterTypes.size(); --n >= 0; )
                        {
                            String param = nmod->getParam((NostalgicParameterType)n);
                            
                            DBG("nostparam: " + param);
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    processor.currentPiano->modMap[key]->addNostalgicModification(new NostalgicModification(key, prep, (NostalgicParameterType)n, param, whichMod));
                                    DBG("nostprep: " + String(prep) + " whichtype: " + cNostalgicParameterTypes[n] + " val: " +param);
                                }
                            }
                        }
                    }
                    else if (itsASynchronic)
                    {
                        out += (String(key) + ":sm" +String(whichMod) + ":" + "{" + intArrayToString(whichPreps) + "} ");
                        
                        SynchronicModPreparation::Ptr smod = processor.modSynchronic[whichMod];
                        
                        for (int n = cSynchronicParameterTypes.size(); --n >= 0; )
                        {
                            String param = smod->getParam((SynchronicParameterType)n);
                            
                            DBG("syncparam: " + param);
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    processor.currentPiano->modMap[key]->addSynchronicModification(new SynchronicModification(key, prep, (SynchronicParameterType)n, param, whichMod));
                                    DBG("syncprep: " + String(prep) + " whichtype: " + cSynchronicParameterTypes[n] + " val: " +param);
                                }
                            }
                        }
                    }
                    else if (itsATuning)
                    {
                        out += (String(key) + ":tm" +String(whichMod) + ":" + "{" + intArrayToString(whichPreps) + "} ");
                        
                        TuningModPreparation::Ptr tmod = processor.modTuning[whichMod];
                        
                        for (int n = cTuningParameterTypes.size(); --n >= 0; )
                        {
                            String param = tmod->getParam((TuningParameterType)n);
                            
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    processor.currentPiano->modMap[key]->addTuningModification(new TuningModification(key, prep, (TuningParameterType)n, param, whichMod));
                                    DBG("tuneprep: " + String(prep) + " whichtype: " + cTuningParameterTypes[n] + " val: " +param);
                                }
                            }
                        }
                    }
                    
                    
                    
                }
                
                itsAKeymap = false;
                itsADirect = false;
                itsASynchronic = false;
                itsATuning = false;
                itsANostalgic = false;
                
                numColons = 0;
                numBrackets = 0;
                whichMod = -1;
                whichPreps.clearQuick();
                
                temp = "";
                
                keys.clearQuick();
            }
            else if (!itsAKeymap && isKeymap)
            {
                itsAKeymap = true;
            }
            else if (isDirect)
            {
                itsADirect = true;
            }
            else if (isSynchronic)
            {
                itsASynchronic = true;
            }
            else if (isTuning)
            {
                itsATuning = true;
            }
            else if (isNostalgic)
            {
                itsANostalgic = true;
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
        tf.setText(processPianoMapString(text), false);
    }
    else if (name == "ModMap")
    {
        tf.setText(processModMapString(text), false);
    }
    
}

void BKAudioProcessorEditor::bkButtonClicked (Button* b)
{
    if (b->getName() == "Save")
    {
        processor.saveGallery();
    }
    else if (b->getName() == "Load")
    {
        processor.loadGallery();
    }
    else if (b->getName() == "Add")
    {
        drawNewPreparationMap(processor.currentPiano->addPreparationMap());
    }
    else if (b->getName() == "Remove")
    {
        removeLastPreparationMap(processor.currentPiano->removeLastPreparationMap());
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
        
    pianoMapTF.setText(temp, false);
    
    String modMap = "";
    for (int i = 0; i < 128; i++)
    {
        
        String mod = processor.currentPiano->modMap[i]->stringRepresentation();
        if (mod != "") modMap += mod;
    }
    
    modMapTF.setText(modMap, false);
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

