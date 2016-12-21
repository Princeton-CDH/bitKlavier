/*
  ==============================================================================

    PianoViewController.cpp
    Created: 8 Dec 2016 12:54:41am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PianoViewController.h"

//==============================================================================
PianoViewController::PianoViewController(BKAudioProcessor& p, Piano::Ptr l):
processor(p),
current(l),
pianoL(OwnedArray<BKLabel>()),
pianoTF(OwnedArray<BKTextField>())
{
    // Labels
    pianoL = OwnedArray<BKLabel>();
    pianoL.ensureStorageAllocated(cPianoParameterTypes.size());
    
    for (int i = 0; i < cPianoParameterTypes.size(); i++)
    {
        pianoL.set(i, new BKLabel());
        addAndMakeVisible(pianoL[i]);
        pianoL[i]->setName(cPianoParameterTypes[i]);
        pianoL[i]->setText(cPianoParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    pianoTF = OwnedArray<BKTextField>();
    pianoTF.ensureStorageAllocated(cPianoParameterTypes.size());
    
    for (int i = 0; i < cPianoParameterTypes.size()-cPianoCBType.size(); i++)
    {
        pianoTF.set(i, new BKTextField());
        addAndMakeVisible(pianoTF[i]);
        pianoTF[i]->addListener(this);
        pianoTF[i]->setName(cPianoParameterTypes[i+cPianoCBType.size()]);
    }
    
    
    for (int i = 0; i < cPianoCBType.size(); i++)
    {
        pianoCB.set(i, new BKComboBox());
        pianoCB[i]->setName(cPianoCBType[i]);
        pianoCB[i]->addSeparator();
        pianoCB[i]->addListener(this);
        addAndMakeVisible(pianoCB[i]);
    }
    
    for (int i = 0; i < aMaxNumPianos; i++)
    {
        pianoCB[PianoCBNumber]->addItem(cPianoNumberName[i], i+1);
    }
    
    pianoCB[PianoCBNumber]->setSelectedItemIndex(0);
    
    updateFields();
}

PianoViewController::~PianoViewController()
{
}

void PianoViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void PianoViewController::resized()
{
   
    
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cPianoParameterTypes.size(); n++)
    {
        pianoL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cPianoCBType.size(); n++)
    {
        pianoCB[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }
    
    for (int n = 0; n < cPianoParameterTypes.size()-cPianoCBType.size(); n++)
    {
        pianoTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * (n+cPianoCBType.size()));
    }
    
}

String PianoViewController::processPreparationString(String s)
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
    
    // pass arrays to piano
    current->setSynchronicPreparations(sPrep);
    current->setNostalgicPreparations(nPrep);
    current->setDirectPreparations(dPrep);
    
    return out;
}

void PianoViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();

    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");

    if (name == cPianoParameterTypes[PianoKeymapId+1])
    {
        Keymap::Ptr km = processor.bkKeymaps[i];
        current->setKeymap(km);
        
        sendActionMessage("keymap/update");
    }
    else if (name == cPianoParameterTypes[PianoPreparationId+1])
    {
        DBG("PREPARATIONS: " + processPreparationString(text));
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}



void PianoViewController::updateFields(void)
{
    
    // Set text.
    pianoTF[PianoKeymapId]        ->setText( String(current->getKeymapId()));
    pianoTF[PianoPreparationId]   ->setText( String(current->getPreparationIds()));
    
}

void PianoViewController::comboBoxChanged (ComboBox* box)
{
    
    if (box->getName() == cPianoCBType[0])
    {
        int whichPiano = box->getSelectedId();
        
        DBG("Current Piano: " + String(whichPiano));
        
        current = processor.setCurrentPiano(whichPiano);
        
        updateFields();
    }
}


