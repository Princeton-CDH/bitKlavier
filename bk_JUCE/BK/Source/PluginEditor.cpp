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
BKAudioProcessorEditor::BKAudioProcessorEditor (BKAudioProcessor& p):
AudioProcessorEditor (&p),
processor (p),
lvc(p, p.currentLayer),
kvc(p),
gvc(p),
svc(p),
nvc(p),
dvc(p),
tvc(p)
{
    addAndMakeVisible(gvc);
    addAndMakeVisible(svc);
    addAndMakeVisible(nvc);
    addAndMakeVisible(dvc);
    addAndMakeVisible(lvc);
    addAndMakeVisible(kvc);
    addAndMakeVisible(tvc);
    
    lvc.addActionListener(&gvc);
    lvc.addActionListener(&svc);
    lvc.addActionListener(&nvc);
    lvc.addActionListener(&dvc);
    lvc.addActionListener(&kvc);
    lvc.addActionListener(&tvc);
    
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
    float lvcH = cLayerParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float kvcH = cKeymapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) +  gYSpacing;
    float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) +  gYSpacing;
    float tvcH = cTuningParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) +  gYSpacing;
    
    
    
    gvc.setBounds(gComponentLeftOffset,
                  gComponentTopOffset,
                  gGeneralVCWidth,
                  gvcH);
 
    lvc.setBounds(gComponentLeftOffset + gGeneralVCWidth + gXSpacing,
                  gComponentTopOffset,
                  gLayerVCWidth,
                  lvcH);
    
    kvc.setBounds(gComponentLeftOffset + gGeneralVCWidth + gXSpacing,
                  gComponentTopOffset + lvcH + gYSpacing,
                  gKeymapVCWidth,
                  kvcH);
    
    dvc.setBounds(gComponentLeftOffset + gGeneralVCWidth + gXSpacing,
                  gComponentTopOffset + lvcH + kvcH + 2*gYSpacing,
                  gDirectVCWidth,
                  dvcH);
    
    tvc.setBounds(gComponentLeftOffset + gGeneralVCWidth + gXSpacing,
                  gComponentTopOffset + lvcH + kvcH + dvcH + 3*gYSpacing,
                  gTuningVCWidth,
                  tvcH);
    
    svc.setBounds(gComponentLeftOffset + gGeneralVCWidth + gLayerVCWidth + (2 * gXSpacing),
                  gComponentTopOffset,
                  gSynchronicVCWidth,
                  svcH);
    
    nvc.setBounds(gComponentLeftOffset + gGeneralVCWidth + gLayerVCWidth + (2 * gXSpacing),
                  gComponentTopOffset + svcH + gYSpacing,
                  gNostalgicVCWidth,
                  nvcH);
 
    
    
    
    
    
    
     
    
}
