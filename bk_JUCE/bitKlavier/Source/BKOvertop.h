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
#include "BlendronicViewController.h"
#include "TuningViewController.h"
#include "TempoViewController.h"
#include "ResonanceViewController.h"

#include "KeymapViewController.h"
#include "GeneralViewController.h"

#include "BKGraph.h"

class BKOvertop :  public Component
{
public:
    BKOvertop (BKAudioProcessor& p, BKItemGraph* theGraph):
    gvc(p, theGraph),
    avc(p,theGraph),
    cvc(p,theGraph),
    movc(p,theGraph),
    kvc(p, theGraph),
    tvc(p, theGraph),
    ovc(p, theGraph),
    svc(p, theGraph),
    nvc(p, theGraph),
    dvc(p, theGraph),
    bvc(p, theGraph),
    rvc(p, theGraph),
    tvcm(p, theGraph),
    ovcm(p, theGraph),
    svcm(p, theGraph),
    nvcm(p, theGraph),
    dvcm(p, theGraph),
    bvcm(p, theGraph),
    processor(p)
    {
        
        addChildComponent(gvc);
        addChildComponent(avc);
        addChildComponent(kvc);
        addChildComponent(cvc);
        addChildComponent(movc);

        addChildComponent(tvc);
        addChildComponent(ovc);
        addChildComponent(svc);
        addChildComponent(nvc);
        addChildComponent(dvc);
        addChildComponent(bvc);
        addChildComponent(rvc);
        
        addChildComponent(tvcm);
        addChildComponent(ovcm);
        addChildComponent(svcm);
        addChildComponent(nvcm);
        addChildComponent(dvcm);
        addChildComponent(bvcm);
        
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
        
        bvc.setBounds(area);

        rvc.setBounds(area);
        
        tvcm.setBounds(area);
        
        ovcm.setBounds(area);
        
        svcm.setBounds(area);
        
        nvcm.setBounds(area);
        
        dvcm.setBounds(area);
        
        bvcm.setBounds(area);
        
#if JUCE_IOS
        cvc.setBounds(5, 5, area.getWidth()-10, area.getHeight() * 0.5f);
#else
        cvc.setBounds(area.getWidth() * 0.2,
                      area.getHeight() * 0.2,
                      area.getWidth() * 0.6,
                      area.getHeight() * 0.6);
#endif
        
        movc.setBounds(area);

    }
    
    
    inline void arrowPressed(BKArrowType arrow)
    {
        BKPreparationDisplay display = currentDisplay;
        
        if (display == DisplayKeymap)
        {
            kvc.arrowPressed(arrow);
        }
        else if (display == DisplayGeneral)
        {
            gvc.arrowPressed(arrow);
        }
        else if (display == DisplayAbout)
        {
            avc.arrowPressed(arrow);
        }
        else if (display == DisplayComment)
        {
            cvc.arrowPressed(arrow);
        }
        else if (display == DisplayModdable)
        {
            movc.arrowPressed(arrow);
        }
        else if (display == DisplayTuning)
        {
            tvc.arrowPressed(arrow);
        }
        else if (display == DisplayTempo)
        {
            ovc.arrowPressed(arrow);
        }
        else if (display == DisplaySynchronic)
        {
            svc.arrowPressed(arrow);
        }
        else if (display == DisplayNostalgic)
        {
            nvc.arrowPressed(arrow);
        }
        else if (display == DisplayDirect)
        {
            dvc.arrowPressed(arrow);
        }
        else if (display == DisplayBlendronic)
        {
            bvc.arrowPressed(arrow);
        }
        else if (display == DisplayResonance)
        {
            rvc.arrowPressed(arrow);
        }
        else if (display == DisplayTuningMod)
        {
            tvcm.arrowPressed(arrow);
        }
        else if (display == DisplayTempoMod)
        {
            ovcm.arrowPressed(arrow);
        }
        else if (display == DisplaySynchronicMod)
        {
            svcm.arrowPressed(arrow);
        }
        else if (display == DisplayNostalgicMod)
        {
            nvcm.arrowPressed(arrow);
        }
        else if (display == DisplayDirectMod)
        {
            dvcm.arrowPressed(arrow);
        }
        else if (display == DisplayBlendronicMod)
        {
            bvcm.arrowPressed(arrow);
        }
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
        removeChildComponent(&movc);
        
        removeChildComponent(&tvc);
        removeChildComponent(&dvc);
        removeChildComponent(&ovc);
        removeChildComponent(&svc);
        removeChildComponent(&nvc);
        removeChildComponent(&bvc);
        removeChildComponent(&rvc);
        
        removeChildComponent(&tvcm);
        removeChildComponent(&dvcm);
        removeChildComponent(&ovcm);
        removeChildComponent(&svcm);
        removeChildComponent(&nvcm);
        removeChildComponent(&bvcm);
        
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
        else if (type == DisplayModdable)
        {
            addAndMakeVisible(&movc);
            movc.grabKeyboardFocus();
            movc.update();
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
        else if (type == DisplayResonance)
        {
            addAndMakeVisible(&rvc);
            rvc.fillSelectCB(-1, -1);
            rvc.update();
        }
        else if (type == DisplayBlendronic)
        {
            addAndMakeVisible(&bvc);
            bvc.fillSelectCB(-1,-1);
            bvc.update();
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
        else if (type == DisplayBlendronicMod)
        {
            addAndMakeVisible(&bvcm);
            bvcm.fillSelectCB(-1,-1);
            bvcm.update();
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
    ModdableViewController movc;
    KeymapViewController kvc;
    
    TuningPreparationEditor tvc;
    TempoPreparationEditor ovc;
    SynchronicPreparationEditor svc;
    NostalgicPreparationEditor nvc;
    DirectPreparationEditor dvc;
    BlendronicPreparationEditor bvc;
    ResonancePreparationEditor rvc;
    
    TuningModificationEditor tvcm;
    TempoModificationEditor ovcm;
    SynchronicModificationEditor svcm;
    NostalgicModificationEditor nvcm;
    DirectModificationEditor dvcm;
    BlendronicModificationEditor bvcm;
    
private:
    
    BKAudioProcessor& processor;
    
    BKPreparationDisplay currentDisplay = DisplayNil;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKOvertop);
    
    
};
