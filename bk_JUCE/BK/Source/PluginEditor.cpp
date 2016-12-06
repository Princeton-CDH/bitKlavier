/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GraphicsConstants.h"


//==============================================================================
BKAudioProcessorEditor::BKAudioProcessorEditor (BKAudioProcessor& p)
:
    AudioProcessorEditor (&p),
    processor (p),
    gvc(p),
    svc(p),
    nvc(p),
    dvc(p)
{
    addAndMakeVisible(gvc);
    addAndMakeVisible(svc);
    addAndMakeVisible(nvc);
    addAndMakeVisible(dvc);
    
    setSize(gMainComponentWidth,
            gMainComponentHeight);
    
}

BKAudioProcessorEditor::~BKAudioProcessorEditor()
{
    
}

//==============================================================================
void BKAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
    
}

void BKAudioProcessorEditor::resized()
{
    float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) +  gYSpacing;
    float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) +  gYSpacing;
    
    gvc.setBounds(gComponentLeftOffset,
                  gComponentTopOffset,
                  gGeneralVCWidth,
                  gvcH);
    
    svc.setBounds(gComponentLeftOffset + gGeneralVCWidth + (1 * gXSpacing),
                  gComponentTopOffset,
                  gSynchronicVCWidth,
                  svcH);
    
    nvc.setBounds(gComponentLeftOffset + gGeneralVCWidth + gSynchronicVCWidth + (2 * gXSpacing),
                  gComponentTopOffset,
                  gNostalgicVCWidth,
                  nvcH);
 
    dvc.setBounds(gComponentLeftOffset + gNostalgicVCWidth + gSynchronicVCWidth  + (2 * gXSpacing),
                  gComponentTopOffset + nvcH + gYSpacing,
                  gNostalgicVCWidth,
                  dvcH);
    
    
    
    
    
     
    
}
