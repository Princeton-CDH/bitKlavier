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
lvc(p, p.currentPiano),
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
    float lvcH = cPianoParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float kvcH = cKeymapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) +  gYSpacing;
    float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) +  gYSpacing;
    float tvcH = cTuningParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) +  gYSpacing;
    
    Rectangle<int> upperLeft = Rectangle<int>(gComponentLeftOffset, gComponentTopOffset, gGeneralVCWidth, lvcH);
    
    lvc.setBounds(upperLeft);
    
    kvc.setBounds(upperLeft.getX(),
                  upperLeft.getBottom() + gYSpacing,
                  gKeymapVCWidth,
                  kvcH);
    
    gvc.setBounds(upperLeft.getX(),
                  kvc.getBottom() + gYSpacing,
                  gGeneralVCWidth,
                  gvcH);
 
    svc.setBounds(upperLeft.getRight() + gXSpacing,
                  upperLeft.getY(),
                  gSynchronicVCWidth,
                  svcH);
    
    nvc.setBounds(svc.getX(),
                  svc.getBottom() + gYSpacing,
                  gNostalgicVCWidth,
                  nvcH);
    
    dvc.setBounds(svc.getRight() + gXSpacing,
                  upperLeft.getY(),
                  gDirectVCWidth,
                  dvcH);
    
    tvc.setBounds(dvc.getX(),
                  dvc.getBottom() + gYSpacing,
                  gTuningVCWidth,
                  tvcH);
    
    
    
}
