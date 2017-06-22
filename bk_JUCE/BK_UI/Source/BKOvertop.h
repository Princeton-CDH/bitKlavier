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

class BKOvertop :  public Component, public BKListener, public KeyListener
{
public:
    BKOvertop (BKAudioProcessor& p, BKItemGraph* theGraph):
    processor(p),
    gvc(p),
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
    dvcm(p, theGraph)
    {
        addKeyListener(this);
        
        addAndMakeVisible(hideOrShow);
        hideOrShow.setName("hideOrShow");
        hideOrShow.addListener(this);
        hideOrShow.setButtonText(" X ");
        
        addChildComponent(gvc);
        addChildComponent(kvc);
        addChildComponent(tvc);
        addChildComponent(ovc);
        addChildComponent(svc);
        addChildComponent(nvc);
        addChildComponent(dvc);
    
    }
    
    ~BKOvertop()
    {
        
    }
    
    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll(Colour((uint8)0, (uint8)0, (uint8)0, 0.75f));
    }
    
    void resized() override
    {
        hideOrShow.setBounds(gXSpacing,gYSpacing, 20, 20);
        
        float X = gXSpacing; float Y = hideOrShow.getBottom()+gYSpacing;
        float width = getWidth() - 10;
        float height = getHeight() - 35;
        
        kvc.setBounds(X, Y, width, height);
        
        gvc.setBounds(X, Y, width, height);
        
        tvc.setBounds(X, Y, width, height);
        
        ovc.setBounds(X, Y, width, height);
        
        svc.setBounds(X, Y, width, height);
        
        nvc.setBounds(X, Y, width, height);
        
        dvc.setBounds(X, Y, width, height);
        
        tvcm.setBounds(X, Y, width, height);
        
        ovcm.setBounds(X, Y, width, height);
        
        svcm.setBounds(X, Y, width, height);
        
        nvcm.setBounds(X, Y, width, height);
        
        dvcm.setBounds(X, Y, width, height);
    }
    
    inline void setCurrentDisplay(BKPreparationDisplay type)
    {
        
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
            //kvc.update();
        }
        else if (type == DisplayGeneral)
        {
            addAndMakeVisible(&gvc);
            gvc.update();
        }
        else if (type == DisplayTuning)
        {
            addAndMakeVisible(&tvc);
            tvc.update();
        }
        else if (type == DisplayTempo)
        {
            addAndMakeVisible(&ovc);
            ovc.update();
        }
        else if (type == DisplaySynchronic)
        {
            addAndMakeVisible(&svc);
            svc.update();
        }
        else if (type == DisplayNostalgic)
        {
            addAndMakeVisible(&nvc);
            nvc.update();
        }
        else if (type == DisplayDirect)
        {
            addAndMakeVisible(&dvc);
            dvc.update();
        }
        else if (type == DisplayTuningMod)
        {
            addAndMakeVisible(&tvcm);
            tvcm.update();
        }
        else if (type == DisplayTempoMod)
        {
            addAndMakeVisible(&ovcm);
            ovcm.update();
        }
        else if (type == DisplaySynchronicMod)
        {
            addAndMakeVisible(&svcm);
            svcm.update();
        }
        else if (type == DisplayNostalgicMod)
        {
            addAndMakeVisible(&nvcm);
            nvcm.update();
        }
        else if (type == DisplayDirectMod)
        {
            addAndMakeVisible(&dvcm);
            dvcm.update();
        }
        
    }
    
    GeneralViewController gvc;
    
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
    
    TextButton hideOrShow;
    
    BKPreparationDisplay currentDisplay;
    
    void bkTextFieldDidChange       (TextEditor&)           override{};
    void bkMessageReceived          (const String& message) override{};
    void bkComboBoxDidChange        (ComboBox* box)         override{};
    
    inline void bkButtonClicked            (Button* b) override
    {
        String name = b->getName();
        
        if (name == "hideOrShow")
        {
            processor.updateState->setCurrentDisplay(DisplayNil);
        }
    }
    
    bool keyPressed (const KeyPress& e, Component*) override
    {
        if (e.isKeyCode(27)) // Escape
        {
            
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKOvertop);
    
    
};
