/*
  ==============================================================================

    PianoViewController.cpp
    Created: 27 Jan 2017 10:20:17am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PianoViewController.h"
#include "PluginEditor.h"


//==============================================================================
PianoViewController::PianoViewController(BKAudioProcessor& p):
processor(p),
pianoL(OwnedArray<BKLabel>()),
pianoCB(OwnedArray<BKComboBox>())
{
    // Labels
    pianoL.ensureStorageAllocated(cPianoParameterTypes.size());
    
    for (int i = 0; i < cPianoParameterTypes.size(); i++)
    {
        pianoL.set(i, new BKLabel());
        addAndMakeVisible(pianoL[i]);
        pianoL[i]->setName(cPianoParameterTypes[i]);
        pianoL[i]->setText(cPianoParameterTypes[i], NotificationType::dontSendNotification);
    }

    //Combo Boxes
    pianoCB.ensureStorageAllocated(cPianoParameterTypes.size());
    
    for (int i = 0; i < cPianoParameterTypes.size(); i++)
    {
        pianoCB.set(i, new BKComboBox());
        pianoCB[i]->setName(cPianoParameterTypes[i]);
        pianoCB[i]->addSeparator();
        pianoCB[i]->addListener(this);
        addAndMakeVisible(pianoCB[i]);
    }
    
    
    for (int i = 0; i < aMaxNumPianos; i++)
    {
        pianoCB[PianoCBPiano]->addItem(cPianoName[i], i+1);
    }
    
    pianoCB[PianoCBPiano]->setSelectedItemIndex(0);
    
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
        pianoL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    
    // CB
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cPianoParameterTypes.size(); n++)
        pianoCB[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
}

void PianoViewController::updateFields(void)
{
    
}

void PianoViewController::comboBoxDidChange (ComboBox* box)
{
    // Change piano
    if (box->getName() == cPianoParameterTypes[PianoCBPiano])
    {
        int whichPiano = box->getSelectedId();
        
        DBG("Current piano: " + String(whichPiano-1));
        
        processor.setCurrentPiano(whichPiano-1);
        
        updateFields();
    }
}

void PianoViewController::textFieldDidChange(TextEditor&)
{
    
}
