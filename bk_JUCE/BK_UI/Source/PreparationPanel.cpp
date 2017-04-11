/*
  ==============================================================================

    PreparationPanel.cpp
    Created: 3 Apr 2017 10:05:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PreparationPanel.h"


PreparationPanel::PreparationPanel(BKAudioProcessor& p):
processor(p)
{
    setName ("PreparationPanel");
    
    for (int i = PreparationTypeDirect; i < BKPreparationTypeNil; i++)
    {
        preparationLabel.add(new Label(cPreparationTypes[i], cPreparationTypes[i]));
        addAndMakeVisible(preparationLabel[i]);
        
        sourceModel.add(new SourceItemListboxContents((BKPreparationType)i, processor));
        
        preparationListBox.add(new ListBox());
        preparationListBox[i]->setModel(sourceModel[i]);
        preparationListBox[i]->setMultipleSelectionEnabled(true);
        addAndMakeVisible(preparationListBox[i]);
    }
}

PreparationPanel::~PreparationPanel(void)
{
    
}

void PreparationPanel::refill(BKPreparationType type)
{
    
    sourceModel.set(type, new SourceItemListboxContents(type, processor));
    
    preparationListBox[type]->setModel(sourceModel[type]);
    
    preparationListBox[type]->repaint();
    
    repaint();
    
}


void PreparationPanel::refillAll(void)
{
    for (int i = PreparationTypeDirect; i < BKPreparationTypeNil; i++)
    {
        refill((BKPreparationType)i);
    }
    
}

#define SOME_MORE_PADDING 20
void PreparationPanel::resized(void)
{
    
    float rowHeight = preparationListBox[PreparationTypeDirect]->getRowHeight();
    
    float height = 2*(getHeight()/preparationListBox.size()) - rowHeight - SOME_MORE_PADDING;
    float width = getWidth() / 2.0f;
    
    preparationLabel[PreparationTypeDirect]->setBounds(0, 0, width, rowHeight);
    preparationListBox[PreparationTypeDirect]->setBounds(0, preparationLabel[PreparationTypeDirect]->getBottom(), width, height);
    
    preparationLabel[PreparationTypeDirectMod]->setBounds(width, 0, width, rowHeight);
    preparationListBox[PreparationTypeDirectMod]->setBounds(width, preparationLabel[PreparationTypeDirectMod]->getBottom(), width, height);
    
    for (int i = PreparationTypeDirect+1; i < BKPreparationTypeNil; i++)
    {
        if (i != PreparationTypeDirectMod)
        {
            preparationLabel[i]     ->setBounds(preparationListBox[i-1]->getX(),  preparationListBox[i-1]->getBottom(),
                                                width, rowHeight);
            preparationListBox[i]   ->setBounds(preparationListBox[i-1]->getX(),  preparationLabel[i]->getBottom(),
                                                width, height);
        }
    }
}





