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
somethingIsBeingDraggedOver(false)
{
    // Row 1: Keymaps
    // First row
    addAndMakeVisible(keymapSelectL);
    keymapSelectL.setName("Keymap");
    keymapSelectL.setText("Keymap", NotificationType::dontSendNotification);
    
    addAndMakeVisible(keymapSelectCB);
    keymapSelectCB.setName("Keymap");
    keymapSelectCB.addSeparator();
    keymapSelectCB.addListener(this);
    keymapSelectCB.setSelectedItemIndex(0);
    
    // Row 2: Preparations
    addAndMakeVisible(prepMapL);
    prepMapL.setName("Preparations");
    prepMapL.setText("Preparations", NotificationType::dontSendNotification);
    
    addAndMakeVisible(prepMapTF);
    prepMapTF.addListener(this);
    prepMapTF.setName("Preparations");
    
    fillKeymapSelectCB();
    updateFields();
}

PreparationMapViewController::~PreparationMapViewController()
{
}

void PreparationMapViewController::fillKeymapSelectCB(void)
{
    keymapSelectCB.clear(dontSendNotification);
    
    Keymap::PtrArr keymaps = processor.gallery->getKeymaps();
    
    for (int i = 0; i < keymaps.size(); i++)
    {
        String name = keymaps[i]->getName();
        if (name != String::empty)  keymapSelectCB.addItem(name, i+1);
        else                        keymapSelectCB.addItem(String(i+1), i+1);
    }
    
    keymapId = processor.currentPiano->prepMaps[Id]->getKeymap()->getId();
    keymapSelectCB.setSelectedItemIndex(keymapId, NotificationType::dontSendNotification);
    
}

void PreparationMapViewController::bkComboBoxDidChange        (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Keymap")
    {
        keymapId = box->getSelectedItemIndex();
        processor.currentPiano->prepMaps[Id]->setKeymap(processor.gallery->getKeymap(keymapId));
        updateFields();
    }
}

//==============================================================================
// These methods implement the DragAndDropTarget interface, and allow our component
// to accept drag-and-drop of objects from other Juce components..

bool PreparationMapViewController::isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/)
{
    // should check drag source to see if its what we want...
    return true;
}

void PreparationMapViewController::itemDragEnter (const SourceDetails& /*dragSourceDetails*/) 
{
    somethingIsBeingDraggedOver = true;
    repaint();
}

void PreparationMapViewController::itemDragMove (const SourceDetails& /*dragSourceDetails*/)
{
}

void PreparationMapViewController::itemDragExit (const SourceDetails& /*dragSourceDetails*/)
{
    somethingIsBeingDraggedOver = false;
    repaint();
}

void PreparationMapViewController::itemDropped (const SourceDetails& dragSourceDetails)
{
    String received = dragSourceDetails.description.toString();
    DBG("Items dropped: " + received);
    
    Array<int> data = stringToIntArray(received);
    
    if (data[0] == PreparationTypeDirect)
    {
        for (int i = 1; i < data.size(); i++)
        {
            prepMapTF.setText( prepMapTF.getText() + " " + addDirectPreparation(data[i]) , dontSendNotification);
            
            
        }
    }
    
    
    
    somethingIsBeingDraggedOver = false;
    repaint();
}

void PreparationMapViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
    
    // draw a red line around the comp if the user's currently dragging something over it..
    if (somethingIsBeingDraggedOver)
    {
        g.setColour (Colours::red);
        g.drawRect (getLocalBounds(), 3);
    }
}

void PreparationMapViewController::resized()
{
   
    
    // Row1
    keymapSelectL.setTopLeftPosition(0, gYSpacing);
    keymapSelectCB.setTopLeftPosition(keymapSelectL.getRight()+gXSpacing, keymapSelectL.getY());
    
    prepMapL.setTopLeftPosition(0, keymapSelectL.getBottom() + gYSpacing);
    prepMapTF.setTopLeftPosition(prepMapL.getRight()+gXSpacing, prepMapL.getY());
    
}

String PreparationMapViewController::addDirectPreparation(int prep)
{
    String out = "";
    
    if (prep <= processor.gallery->getNumDirect()-1)
    {
        Direct::Ptr thePrep = processor.gallery->getDirect(prep);
        
        processor.currentPiano->prepMaps[Id]->addDirect(thePrep);
        
        out += "D" + String(prep);
    }
    
    return out;
}

String PreparationMapViewController::processPreparationString(String s)
{
    Synchronic::PtrArr sync = Synchronic::PtrArr();
    Nostalgic::PtrArr nost = Nostalgic::PtrArr();
    Direct::PtrArr dire = Direct::PtrArr();
    Tempo::PtrArr tempo = Tempo::PtrArr();
    Tuning::PtrArr tuning = Tuning::PtrArr();
    
    
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
    juce_wchar tempoLC = 'm'; //M for metronome
    juce_wchar tempoUC = 'M';
    juce_wchar tuningLC = 't';
    juce_wchar tuningUC = 'T';
    
    BKPreparationType type = BKPreparationTypeNil;
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isSynchronic   = !CharacterFunctions::compare(c1, synchronicLC) || !CharacterFunctions::compare(c1, synchronicUC);
        bool isNostalgic    = !CharacterFunctions::compare(c1, nostalgicLC) || !CharacterFunctions::compare(c1, nostalgicUC);
        bool isDirect       = !CharacterFunctions::compare(c1, directLC) || !CharacterFunctions::compare(c1, directUC);
        bool isTempo        = !CharacterFunctions::compare(c1, tempoLC) || !CharacterFunctions::compare(c1, tempoUC);
        bool isTuning       = !CharacterFunctions::compare(c1, tuningLC) || !CharacterFunctions::compare(c1, tuningUC);
        
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
            else if (isTempo)
            {
                type = PreparationTypeTempo;
            }
            else if (isTuning)
            {
                type = PreparationTypeTuning;
            }
            else if (inNumber)
            {
                if (type == PreparationTypeSynchronic)
                {
                    int prep = temp.getIntValue();
                    
                    if (prep <= processor.gallery->getNumSynchronic()-1)
                    {
                        sync.add(processor.gallery->getSynchronic(prep));
                        
                        out.append("S", 1);
                        out.append(String(prep), 3);
                    }
                    
                }
                else if (type == PreparationTypeNostalgic)
                {
                    int prep = temp.getIntValue();
                    
                    if (prep <= processor.gallery->getNumNostalgic()-1)
                    {
                        nost.add(processor.gallery->getNostalgic(prep));
                    
                        out.append("N", 1);
                        out.append(String(prep), 3);
                    }
                }
                else if (type == PreparationTypeDirect)
                {
                    int prep = temp.getIntValue();
                    
                    if (prep <= processor.gallery->getNumDirect()-1)
                    {
                        dire.add(processor.gallery->getDirect(prep));
                        
                        out.append("D", 1);
                        out.append(String(prep), 3);
                    }
                }
                else if (type == PreparationTypeTempo)
                {
                    int prep = temp.getIntValue();
                    
                    if (prep <= processor.gallery->getNumTempo()-1)
                    {
                        tempo.add(processor.gallery->getTempo(prep));
                        
                        out.append("M", 1);
                        out.append(String(prep), 3);
                    }
                }
                else if (type == PreparationTypeTuning)
                {
                    int prep = temp.getIntValue();
                    
                    if (prep <= processor.gallery->getNumTuning()-1)
                    {
                        tuning.add(processor.gallery->getTuning(prep));
                        
                        out.append("T", 1);
                        out.append(String(prep), 3);
                    }
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
    processor.currentPiano->prepMaps[Id]->setSynchronic (sync);
    processor.currentPiano->prepMaps[Id]->setNostalgic  (nost);
    processor.currentPiano->prepMaps[Id]->setDirect  (dire);
    processor.currentPiano->prepMaps[Id]->setTempo      (tempo);
    processor.currentPiano->prepMaps[Id]->setTuning     (tuning);
    
    return out;
}

void PreparationMapViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();

    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");

    if (name == "Preparations")
    {
        tf.setText(processPreparationString(text));
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}



void PreparationMapViewController::updateFields(void)
{
    // Set text.
    prepMapTF.setText( processor.currentPiano->prepMaps[Id]->getPreparationIds());
    
}


