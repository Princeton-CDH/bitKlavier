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
PreparationMapViewController::PreparationMapViewController(BKAudioProcessor& p, int Id):
Id(Id),
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
    
    for (int i = 0; i < cPrepMapParameterTypes.size(); i++)
    {
        prepMapTF.set(i, new BKTextField());
        addAndMakeVisible(prepMapTF[i]);
        prepMapTF[i]->addListener(this);
        prepMapTF[i]->setName(cPrepMapParameterTypes[i]);
    }
    
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
    
    for (int n = 0; n < cPrepMapParameterTypes.size(); n++)
    {
        prepMapTF[n]->setTopLeftPosition(tfX,
                                         gYSpacing + tfY * n);
    }
    
    
    
}

String PreparationMapViewController::processPreparationString(String s)
{
    Synchronic::PtrArr sync = Synchronic::PtrArr();
    Nostalgic::PtrArr nost = Nostalgic::PtrArr();
    Direct::PtrArr dire = Direct::PtrArr();
    
    sync.ensureStorageAllocated(12);
    
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
                    
                    sync.add(processor.synchronic[prep]);
                    
                    out.append("S", 1);
                    out.append(String(prep), 3);
                }
                else if (type == PreparationTypeNostalgic)
                {
                    int prep = temp.getIntValue();
                    
                    nost.add(processor.nostalgic[prep]);
                    
                    out.append("N", 1);
                    out.append(String(prep), 3);
                }
                else if (type == PreparationTypeDirect)
                {
                    int prep = temp.getIntValue();
                    
                    dire.add(processor.direct[prep]);
                    
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
    processor.currentPiano->prepMaps[Id]->setSynchronicPreparations(sync);
    processor.currentPiano->prepMaps[Id]->setNostalgicPreparations(nost);
    processor.currentPiano->prepMaps[Id]->setDirectPreparations(dire);
    
    return out;
}

void PreparationMapViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();

    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");

    if (name == cPrepMapParameterTypes[PrepMapKeymapId])
    {
        Keymap::Ptr km = processor.bkKeymaps[i];

        processor.currentPiano->prepMaps[Id]->setKeymap(km);
        
        sendActionMessage("keymap/update");
    }
    else if (name == cPrepMapParameterTypes[PrepMapPreparationId])
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
    prepMapTF[PrepMapKeymapId]        ->setText( String(processor.currentPiano->prepMaps[Id]->getKeymapId()));
    prepMapTF[PrepMapPreparationId]   ->setText( processor.currentPiano->prepMaps[Id]->getPreparationIds());
    
}


