/*
  ==============================================================================

    PreparationsMapViewController.cpp
    Created: 8 Dec 2016 12:54:41am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PreparationsMapViewController.h"

//==============================================================================
PreparationsMapViewController::PreparationsMapViewController(BKAudioProcessor& p, PreparationsMap::Ptr l):
processor(p),
current(l),
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
    
    for (int i = 0; i < aMaxNumPreparationKeymaps; i++)
    {
        prepMapCB[PrepMapCBNumber]->addItem(cPreMapNumberName[i], i+1);
    }
    
    prepMapCB[PrepMapCBNumber]->setSelectedItemIndex(0);
    
    updateFields();
}

PreparationsMapViewController::~PreparationsMapViewController()
{
}

void PreparationsMapViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void PreparationsMapViewController::resized()
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

String PreparationsMapViewController::processPreparationString(String s)
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
    current->setSynchronicPreparations(sPrep);
    current->setNostalgicPreparations(nPrep);
    current->setDirectPreparations(dPrep);
    
    return out;
}

void PreparationsMapViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();

    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");

    if (name == cPrepMapParameterTypes[PrepMapKeymapId+1])
    {
        Keymap::Ptr km = processor.bkKeymaps[i];
        current->setKeymap(km);
        
        sendActionMessage("keymap/update");
    }
    else if (name == cPrepMapParameterTypes[PrepMapPreparationId+1])
    {
        DBG("Preparations: " + processPreparationString(text));
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}



void PreparationsMapViewController::updateFields(void)
{
    // Set text.
    prepMapTF[PrepMapKeymapId]        ->setText( String(current->getKeymapId()));
    prepMapTF[PrepMapPreparationId]   ->setText( String(current->getPreparationIds()));
    
}

void PreparationsMapViewController::comboBoxChanged (ComboBox* box)
{
    
    if (box->getName() == cPrepMapCBType[0])
    {
        int whichPrepMap = box->getSelectedId();
        
        DBG("Current Prep Map: " + String(whichPrepMap));
        
        current = processor.setCurrentPrepKeymap(whichPrepMap);
        
        updateFields();
    }
}


