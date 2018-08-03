/*
  ==============================================================================

    PreparationEditor.h
    Created: 13 Jun 2017 3:07:32pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"
#include "PluginProcessor.h"

#include "BKListener.h"
#include "BKComponent.h"



#include "SynchronicViewController.h"
#include "NostalgicViewController.h"
#include "DirectViewController.h"
#include "TuningViewController.h"
#include "TempoViewController.h"

#include "KeymapViewController.h"
#include "GeneralViewController.h"

#include "BKGraph.h"

class BKOvertop :  public Component
{
public:
    BKOvertop (BKAudioProcessor& p, BKItemGraph* theGraph):
    gvc(p, theGraph),
    kvc(p, theGraph),
    tvc(p, theGraph),
    ovc(p, theGraph),
    svc(p, theGraph),
    nvc(p, theGraph),
    dvc(p, theGraph),
    tvcm(p, theGraph),
    ovcm(p, theGraph),
    svcm(p, theGraph),
    nvcm(p, theGraph),
    dvcm(p, theGraph),
    avc(p,theGraph),
    cvc(p,theGraph),
    processor(p)
    {
        
        addChildComponent(gvc);
        addChildComponent(avc);
        addChildComponent(kvc);
        addChildComponent(cvc);

        addChildComponent(tvc);
        addChildComponent(ovc);
        addChildComponent(svc);
        addChildComponent(nvc);
        addChildComponent(dvc);
        
        addChildComponent(tvcm);
        addChildComponent(ovcm);
        addChildComponent(svcm);
        addChildComponent(nvcm);
        addChildComponent(dvcm);
        
    }
    
    ~BKOvertop()
    {
        setLookAndFeel(nullptr);
    }
    
    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll(Colours::transparentBlack);
    }
    
    void resized() override
    {
        Rectangle<int> area (getLocalBounds());
        
        kvc.setBounds(area);
        
        gvc.setBounds(area);
        
        avc.setBounds(area);
        
        tvc.setBounds(area);
        
        ovc.setBounds(area);
        
        svc.setBounds(area);
        
        nvc.setBounds(area);
        
        dvc.setBounds(area);
        
        tvcm.setBounds(area);
        
        ovcm.setBounds(area);
        
        svcm.setBounds(area);
        
        nvcm.setBounds(area);
        
        dvcm.setBounds(area);
        
#if JUCE_IOS
        cvc.setBounds(5, 5, area.getWidth()-10, area.getHeight() * 0.5f);
#else
        cvc.setBounds(area.getWidth() * 0.2,
                      area.getHeight() * 0.2,
                      area.getWidth() * 0.6,
                      area.getHeight() * 0.6);
#endif

    }
    
    inline void setCurrentDisplay(BKPreparationDisplay type)
    {

        currentDisplay = type;
        
        if (type == DisplayNil)
        {
            setVisible(false);
            setOpaque(false);
            
            return;
        }
        else
        {
            setOpaque(true);
            setVisible(true);
        }
        
        removeChildComponent(&kvc);
        removeChildComponent(&gvc);
        removeChildComponent(&avc);
        removeChildComponent(&cvc);
        
        removeChildComponent(&tvc);
        removeChildComponent(&dvc);
        removeChildComponent(&ovc);
        removeChildComponent(&svc);
        removeChildComponent(&nvc);
        
        removeChildComponent(&tvcm);
        removeChildComponent(&dvcm);
        removeChildComponent(&ovcm);
        removeChildComponent(&svcm);
        removeChildComponent(&nvcm);
        
        if (type == DisplayKeymap)
        {
            addAndMakeVisible(&kvc);
            kvc.fillSelectCB(-1,-1);
            kvc.update();
        }
        else if (type == DisplayGeneral)
        {
            addAndMakeVisible(&gvc);
            gvc.update();
        }
        else if (type == DisplayAbout)
        {
            addAndMakeVisible(&avc);
        }
        else if (type == DisplayComment)
        {
            addAndMakeVisible(&cvc);
            cvc.grabKeyboardFocus();
            cvc.update();
        }
        else if (type == DisplayTuning)
        {
            addAndMakeVisible(&tvc);
            tvc.fillSelectCB(-1,-1);
            tvc.update();
        }
        else if (type == DisplayTempo)
        {
            addAndMakeVisible(&ovc);
            ovc.fillSelectCB(-1,-1);
            ovc.update();
        }
        else if (type == DisplaySynchronic)
        {
            addAndMakeVisible(&svc);
            svc.fillSelectCB(-1,-1);
            svc.update();
        }
        else if (type == DisplayNostalgic)
        {
            addAndMakeVisible(&nvc);
            nvc.fillSelectCB(-1,-1);
            nvc.update();
        }
        else if (type == DisplayDirect)
        {
            addAndMakeVisible(&dvc);
            dvc.fillSelectCB(-1,-1);
            dvc.update();
        }
        else if (type == DisplayTuningMod)
        {
            addAndMakeVisible(&tvcm);
            tvcm.fillSelectCB(-1,-1);
            tvcm.update();
        }
        else if (type == DisplayTempoMod)
        {
            addAndMakeVisible(&ovcm);
            ovcm.fillSelectCB(-1,-1);
            ovcm.update();
        }
        else if (type == DisplaySynchronicMod)
        {
            addAndMakeVisible(&svcm);
            svcm.fillSelectCB(-1,-1);
            svcm.update();
        }
        else if (type == DisplayNostalgicMod)
        {
            addAndMakeVisible(&nvcm);
            nvcm.fillSelectCB(-1,-1);
            nvcm.update();
        }
        else if (type == DisplayDirectMod)
        {
            addAndMakeVisible(&dvcm);
            dvcm.fillSelectCB(-1,-1);
            dvcm.update();
        }
        
        //hideOrShow.toFront(false);
        
    }
    
    BKPreparationDisplay getCurrentDisplay()
    {
        return currentDisplay;
    }
    
    GeneralViewController gvc;
    AboutViewController avc;
    CommentViewController cvc;
    KeymapViewController kvc;
    
    
    
    TuningPreparationEditor tvc;
    TempoPreparationEditor ovc;
    SynchronicPreparationEditor svc;
    NostalgicPreparationEditor nvc;
    DirectPreparationEditor dvc;
    
    TuningModificationEditor tvcm;
    TempoModificationEditor ovcm;
    SynchronicModificationEditor svcm;
    NostalgicModificationEditor nvcm;
    DirectModificationEditor dvcm;
    
private:
    
    BKAudioProcessor& processor;
    
    BKPreparationDisplay currentDisplay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKOvertop);
    
    
};
