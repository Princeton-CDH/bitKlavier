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

class BKOvertop :  public Component//, public BKListener, public KeyListener
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
        
        addChildComponent(gvc);
        addChildComponent(kvc);

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
        
    }
    
    //==============================================================================
    void paint (Graphics& g) override
    {
        //g.fillAll(Colour((uint8)0, (uint8)0, (uint8)0, 0.75f));
        //g.fillAll(Colours::burlywood);
        //g.fillAll(Colours::transparentBlack);
    }
    
    void resized() override
    {

        Rectangle<int> area (getLocalBounds());
        
        kvc.setBounds(area);
        
        gvc.setBounds(area);
        
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
            kvc.update();
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
        
        //hideOrShow.toFront(false);
        
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
    
    //TextButton hideOrShow;
    
    BKPreparationDisplay currentDisplay;
    
    /*
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
     */
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKOvertop);
    
    
};
