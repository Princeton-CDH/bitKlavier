/*
  ==============================================================================

    PreparationMapViewController.cpp
    Created: 8 Dec 2016 12:54:41am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PreparationMapViewController.h"

//==============================================================================
PreparationMapViewController::PreparationMapViewController(BKAudioProcessor& p):
processor(p),
prepMapL(OwnedArray<BKLabel>()),
prepMapTF(OwnedArray<BKTextField>())
{
    // Labels
    prepMapL = OwnedArray<BKLabel>();
    prepMapL.ensureStorageAllocated(cPrepMapParameterTypes.size());
    
    for (int i = 0; i < cPrepMapParameterTypes.size(); i++)
    {
        prepMapL.set(i, new BKLabel());
        addAndMakeVisible(prepMapL[i]);
        prepMapL[i]->setName(cPrepMapParameterTypes[i]);
        prepMapL[i]->setText(cPrepMapParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    prepMapTF = OwnedArray<BKTextField>();
    prepMapTF.ensureStorageAllocated(cPrepMapParameterTypes.size());
    
    for (int i = 0; i < cPrepMapParameterTypes.size()-cPrepMapCBType.size(); i++)
    {
        prepMapTF.set(i, new BKTextField());
        addAndMakeVisible(prepMapTF[i]);
        prepMapTF[i]->addListener(this);
        prepMapTF[i]->setName(cPrepMapParameterTypes[i+cPrepMapCBType.size()]);
    }
    
    
    for (int i = 0; i < cPrepMapCBType.size(); i++)
    {
        prepMapCB.set(i, new BKComboBox());
        prepMapCB[i]->setName(cPrepMapCBType[i]);
        prepMapCB[i]->addSeparator();
        prepMapCB[i]->addListener(this);
        addAndMakeVisible(prepMapCB[i]);
    }
    
    
    for (int i = 0; i < aMaxNumPianos; i++)
    {
        prepMapCB[PrepMapCBPiano]->addItem(cPianoName[i], i+1);
    }
    
    prepMapCB[PrepMapCBPiano]->setSelectedItemIndex(0);
    
    
    for (int i = 0; i < aMaxNumPreparationKeymaps; i++)
    {
        prepMapCB[PrepMapCBNumber]->addItem(cPreMapNumberName[i], i+1);
    }
    
    prepMapCB[PrepMapCBNumber]->setSelectedItemIndex(0);
    

    updateFields();
}

PreparationMapViewController::~PreparationMapViewController()
{
}

void PreparationMapViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void PreparationMapViewController::resized()
{
   
    
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cPrepMapParameterTypes.size(); n++)
    {
        prepMapL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cPrepMapCBType.size(); n++)
    {
        prepMapCB[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }
    
    for (int n = 0; n < cPrepMapParameterTypes.size()-cPrepMapCBType.size(); n++)
    {
        prepMapTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * (n+cPrepMapCBType.size()));
    }
    
}

String PreparationMapViewController::processPreparationString(String s)
{
    SynchronicPreparation::PtrArr sPrep = SynchronicPreparation::PtrArr();
    NostalgicPreparation::PtrArr nPrep = NostalgicPreparation::PtrArr();
    DirectPreparation::PtrArr dPrep = DirectPreparation::PtrArr();
    
    String temp = "";
    String out = "";
    
    bool inNumber = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar synchronicLC = 's';
    juce_wchar synchronicUC = 'S';
    juce_wchar nostalgicLC = 'n';
    juce_wchar nostalgicUC = 'N';
    juce_wchar directLC = 'd';
    juce_wchar directUC = 'D';
    
    BKPreparationType type = BKPreparationTypeNil;
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isSynchronic   = !CharacterFunctions::compare(c1, synchronicLC) || !CharacterFunctions::compare(c1, synchronicUC);
        bool isNostalgic    = !CharacterFunctions::compare(c1, nostalgicLC) || !CharacterFunctions::compare(c1, nostalgicUC);
        bool isDirect       = !CharacterFunctions::compare(c1, directLC) || !CharacterFunctions::compare(c1, directUC);
        
        bool isNumChar = CharacterFunctions::isDigit(c1);
        
        if (!isNumChar)
        {
            if (isSynchronic)
            {
                type = PreparationTypeSynchronic;
            }
            else if (isNostalgic)
            {
                type = PreparationTypeNostalgic;
            }
            else if (isDirect)
            {
                type = PreparationTypeDirect;
            }
            else if (inNumber)
            {
                if (type == PreparationTypeSynchronic)
                {
                    int prep = temp.getIntValue();
                    sPrep.add(processor.sPreparation[prep]);
                    out.append("S", 1);
                    out.append(String(prep), 3);
                }
                else if (type == PreparationTypeNostalgic)
                {
                    int prep = temp.getIntValue();
                    nPrep.add(processor.nPreparation[prep]);
                    out.append("N", 1);
                    out.append(String(prep), 3);
                }
                else if (type == PreparationTypeDirect)
                {
                    int prep = temp.getIntValue();
                    dPrep.add(processor.dPreparation[prep]);
                    out.append("D", 1);
                    out.append(String(prep), 3);
                }
                
                out.append(" ", 1);
                temp = "";
                type = BKPreparationTypeNil;
                
                inNumber = false;
            }
            
            continue;
        }
        else
        {
            inNumber = true;
            
            temp += c1;
        }
        
    }
    
    // pass arrays to prepMap
    processor.currentPiano->currentPMap->setSynchronicPreparations(sPrep);
    processor.currentPiano->currentPMap->setNostalgicPreparations(nPrep);
    processor.currentPiano->currentPMap->setDirectPreparations(dPrep);
    
    return out;
}

void PreparationMapViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();

    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");

    if (name == cPrepMapParameterTypes[PrepMapKeymapId+cPrepMapCBType.size()])
    {
        Keymap::Ptr km = processor.bkKeymaps[i];
        processor.currentPiano->currentPMap->setKeymap(km);
        
        sendActionMessage("keymap/update");
    }
    else if (name == cPrepMapParameterTypes[PrepMapPreparationId+cPrepMapCBType.size()])
    {
        DBG("Preparations: " + processPreparationString(text));
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}



void PreparationMapViewController::updateFields(void)
{
    // Set text.
    prepMapTF[PrepMapKeymapId]        ->setText( String(processor.currentPiano->currentPMap->getKeymapId()));
    prepMapTF[PrepMapPreparationId]   ->setText( processor.currentPiano->currentPMap->getPreparationIds());
    
}

void PreparationMapViewController::comboBoxChanged (ComboBox* box)
{
    // Change layer
    if (box->getName() == cPrepMapCBType[PrepMapCBNumber])
    {
        int whichPrepMap = box->getSelectedId();
        
        DBG("Current Prep Map: " + String(whichPrepMap));
        
        processor.currentPiano->currentPMap = processor.currentPiano->getPreparationMaps()[whichPrepMap-1];
        
        //remove inactive prepmaps
        for(int i=0; i<processor.currentPiano->activePMaps.size(); i++) {
            if(!processor.currentPiano->activePMaps[i]->isActive)
                processor.currentPiano->activePMaps.remove(i);
        }
        
        //add current prepmap to activePrepMaps
        processor.currentPiano->activePMaps.addIfNotAlreadyThere(processor.currentPiano->currentPMap);
        
        updateFields();
    }
    
    // Change piano
    if (box->getName() == cPrepMapCBType[PrepMapCBPiano])
    {
        int whichPiano = box->getSelectedId();
        
        DBG("Current piano: " + String(whichPiano-1));
        
        processor.currentPiano = processor.bkPianos[whichPiano-1];
        
        processor.currentPiano->currentPMap = processor.currentPiano->getPreparationMaps()[0];
        
        processor.currentPiano->activePMaps.addIfNotAlreadyThere(processor.currentPiano->currentPMap);
        
        prepMapCB[PrepMapCBNumber]->setSelectedItemIndex(0);
        
        updateFields();
    }
}


