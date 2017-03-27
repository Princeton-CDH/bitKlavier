/*
  ==============================================================================

    MainViewController.cpp
    Created: 27 Mar 2017 2:04:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "MainViewController.h"

//==============================================================================
MainViewController::MainViewController (BKAudioProcessor& p):
processor (p),
galvc(p),
kvc(p),
tvc(p),
gvc(p),
svc(p),
nvc(p),
dvc(p),
ovc(p),
timerCallbackCount(0)
{
    addAndMakeVisible(galvc);
    
    addAndMakeVisible(gvc);
    addAndMakeVisible(svc);
    addAndMakeVisible(nvc);
    addAndMakeVisible(dvc);
    addAndMakeVisible(kvc);
    addAndMakeVisible(tvc);
    addAndMakeVisible(ovc);
    
    
    startTimerHz (50);
    
}

MainViewController::~MainViewController()
{
    
}

void MainViewController::timerCallback()
{
    
    if (++timerCallbackCount >= 100)
    {
        timerCallbackCount = 0;
        processor.collectGalleries();
        galvc.fillGalleryCB();
    }
    
    galvc.update();
    
    if (processor.updateState->generalSettingsDidChange)
    {
        processor.updateState->generalSettingsDidChange = false;
        gvc.updateFields();
    }
    
    if (processor.updateState->directPreparationDidChange)
    {
        processor.updateState->directPreparationDidChange = false;
        dvc.updateFields();
    }
    
    if (processor.updateState->nostalgicPreparationDidChange)
    {
        processor.updateState->nostalgicPreparationDidChange = false;
        nvc.updateFields();
    }
    
    if (processor.updateState->synchronicPreparationDidChange)
    {
        processor.updateState->synchronicPreparationDidChange = false;
        svc.updateFields();
    }
    
    if (processor.updateState->tuningPreparationDidChange)
    {
        processor.updateState->tuningPreparationDidChange = false;
        tvc.updateFields();
    }
    
    if (processor.updateState->tempoPreparationDidChange)
    {
        processor.updateState->tempoPreparationDidChange = false;
        ovc.updateFields();
    }
    
}

void MainViewController::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}



void MainViewController::resized()
{
    
    float kvcH = cKeymapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 *  gYSpacing;
    float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
    float tvcH = cTuningParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    float ovcH = cTempoParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
    
    galvc.setBounds(0, 0, gVCWidth+2*gXSpacing, getHeight());
    
    // Col 2
    kvc.setBounds(galvc.getRight() + gXSpacing,
                  gYSpacing,
                  gVCWidth,
                  kvcH);
    
    gvc.setBounds(kvc.getX(),
                  kvc.getBottom() + gYSpacing,
                  gVCWidth,
                  gvcH);
    
    tvc.setBounds(kvc.getX(),
                  gvc.getBottom() + gYSpacing,
                  gVCWidth,
                  tvcH);
    
    dvc.setBounds(kvc.getX(),
                  tvc.getBottom() + gYSpacing,
                  gVCWidth,
                  dvcH);
    
    
    // Col 3
    
    svc.setBounds(kvc.getRight() + gXSpacing,
                  gYSpacing,
                  gVCWidth,
                  svcH);
    
    nvc.setBounds(svc.getX(),
                  svc.getBottom() + gYSpacing,
                  gVCWidth,
                  nvcH);
    
    ovc.setBounds(nvc.getX(),
                  nvc.getBottom() + gYSpacing,
                  gVCWidth,
                  ovcH);
    
}
