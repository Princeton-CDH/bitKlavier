/*
  ==============================================================================

    PreparationPanel.h
    Created: 3 Apr 2017 10:05:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PREPARATIONPANEL_H_INCLUDED
#define PREPARATIONPANEL_H_INCLUDED


#include "BKUtilities.h"

#include "BKComponent.h"

#include "PluginProcessor.h"


//==============================================================================
class PreparationPanel  : public Component, public DragAndDropContainer
{
public:
    PreparationPanel(BKAudioProcessor&);
    ~PreparationPanel(void);
    
    void refill(BKPreparationType type);
    void refillAll(void);
    
    void resized() override;
    
private:
    //==============================================================================
    

    
    struct SourceItemListboxContents  : public ListBoxModel
    {
        BKAudioProcessor& processor;
        
        StringArray preps;
        BKPreparationType type;
        int numRows;
        
        SourceItemListboxContents(BKPreparationType t, BKAudioProcessor& p):
        processor(p),
        type(t)
        {
            if (type == PreparationTypeDirect)
                setPreparations(processor.gallery->getAllDirectNames());
            else if (type == PreparationTypeSynchronic)
                setPreparations(processor.gallery->getAllSynchronicNames());
            else if (type == PreparationTypeNostalgic)
                setPreparations(processor.gallery->getAllNostalgicNames());
            else if (type == PreparationTypeTempo)
                setPreparations(processor.gallery->getAllTempoNames());
            else if (type == PreparationTypeTuning)
                setPreparations(processor.gallery->getAllTuningNames());
            else if (type == PreparationTypeKeymap)
                setPreparations(processor.gallery->getAllKeymapNames());
            else if (type == PreparationTypeDirectMod)
                setPreparations(processor.gallery->getAllDirectModNames());
            else if (type == PreparationTypeSynchronicMod)
                setPreparations(processor.gallery->getAllSynchronicModNames());
            else if (type == PreparationTypeNostalgicMod)
                setPreparations(processor.gallery->getAllNostalgicModNames());
            else if (type == PreparationTypeTempoMod)
                setPreparations(processor.gallery->getAllTempoModNames());
            else if (type == PreparationTypeTuningMod)
                setPreparations(processor.gallery->getAllTuningModNames());
            else if (type == PreparationTypePianoMap)
                setPreparations(StringArray("PianoMap"));
            else if (type == PreparationTypeReset)
                setPreparations(StringArray("Reset"));

        }
        
        void setPreparations(StringArray names)
        {
            preps = names;
            numRows = names.size();
        }
        
        // The following methods implement the necessary virtual functions from ListBoxModel,
        // telling the listbox how many rows there are, painting them, etc.
        int getNumRows() override
        {
            return numRows;
        }
        
        void listBoxItemClicked (int row, const MouseEvent&) override
        {
            // set selected Preparation
            DBG("clicked..." + String(row));
            
            setAndViewPreparation(type, row);
        }
        
        void setAndViewPreparation(BKPreparationType type, int which)
        {
            if (type == PreparationTypeDirect)
            {
                processor.updateState->currentDirectId = which;
                processor.updateState->directPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayDirect;
            }
            else if (type == PreparationTypeSynchronic)
            {
                processor.updateState->currentSynchronicId = which;
                processor.updateState->synchronicPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplaySynchronic;
            }
            else if (type == PreparationTypeNostalgic)
            {
                processor.updateState->currentNostalgicId = which;
                processor.updateState->nostalgicPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayNostalgic;
            }
            else if (type == PreparationTypeTuning)
            {
                processor.updateState->currentTuningId = which;
                processor.updateState->tuningPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayTuning;
            }
            else if (type == PreparationTypeTempo)
            {
                processor.updateState->currentTempoId = which;
                processor.updateState->tempoPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayTempo;
            }
            else if (type == PreparationTypeKeymap)
            {
                processor.updateState->currentKeymapId = which;
                processor.updateState->keymapDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayKeymap;
            }
            else if (type == PreparationTypeSynchronicMod)
            {
                processor.updateState->currentModSynchronicId = which;
                processor.updateState->synchronicPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplaySynchronic;
            }
            else if (type == PreparationTypeNostalgicMod)
            {
                processor.updateState->currentModNostalgicId = which;
                processor.updateState->nostalgicPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayNostalgic;
            }
            else if (type == PreparationTypeDirectMod)
            {
                processor.updateState->currentModDirectId = which;
                processor.updateState->directPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayDirect;
            }
            else if (type == PreparationTypeTuningMod)
            {
                processor.updateState->currentModTuningId = which;
                processor.updateState->tuningPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayTuning;
            }
            else if (type == PreparationTypeTempoMod)
            {
                processor.updateState->currentModTempoId = which;
                processor.updateState->tempoPreparationDidChange = true;
                
                processor.updateState->currentPreparationDisplay = DisplayTempo;
            }
            
            
            processor.updateState->displayDidChange = true;
            
        }
        
        void listBoxItemDoubleClicked (int row, const MouseEvent&) override
        {
             // set selected Preparation or something
            DBG("double clicked..." + String(row));
        }
        
        void paintListBoxItem (int rowNumber, Graphics& g,
                               int width, int height, bool rowIsSelected) override
        {
            if (rowIsSelected)
                g.fillAll (Colours::lightblue);
            
            g.setColour (Colours::black);
            g.setFont (height * 0.7f);
            
            g.drawText (preps[rowNumber],
                        5, 0, width, height,
                        Justification::centredLeft, true);
        }
        
        var getDragSourceDescription (const SparseSet<int>& selectedRows) override
        {
            // for our drag description, we'll just make a comma-separated list of the selected row
            // numbers - this will be picked up by the drag target and displayed in its box.
            StringArray which;
            
            which.add(String(type));
            
            for (int i = 0; i < selectedRows.size(); ++i)
                which.add(String(selectedRows[i]));
            
            return which.joinIntoString(" ");
        }
    };
    
    
    //==============================================================================
    
    BKAudioProcessor& processor;
    
    OwnedArray<Label> preparationLabel;

    OwnedArray<ListBox> preparationListBox;
    OwnedArray<SourceItemListboxContents> sourceModel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreparationPanel)
};

#endif  // PREPARATIONPANEL_H_INCLUDED
