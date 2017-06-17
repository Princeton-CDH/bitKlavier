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
#include "SynchronicViewController2.h"
#include "NostalgicViewController.h"
#include "NostalgicViewController2.h"
#include "DirectViewController.h"
#include "DirectViewController2.h"
#include "TuningViewController.h"
#include "TuningViewController2.h"
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
    //tvc(p, theGraph),
    //svc(p, &theGraph),
    //nvc(p, &theGraph),
    dvc(p, theGraph),
    ovc(p, theGraph),
    svc2(p, theGraph),
    nvc2(p, theGraph),
    tvc2(p, theGraph),
    dvc2(p, theGraph)
    {
        addKeyListener(this);
        
        addAndMakeVisible(hideOrShow);
        hideOrShow.setName("hideOrShow");
        hideOrShow.addListener(this);
        hideOrShow.setButtonText(" X ");
        
        addChildComponent(gvc);
        //addChildComponent(dvc);
        addChildComponent(kvc);
        addChildComponent(tvc2);
        addChildComponent(ovc);
        addChildComponent(svc2);
        addChildComponent(nvc2);
        addChildComponent(dvc2);
    
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
        
        tvc2.setBounds(X, Y, width, height);
        
        ovc.setBounds(X, Y, width, height);
        
        svc2.setBounds(X, Y, width, height);
        
        nvc2.setBounds(X, Y, width, height);
        
        dvc2.setBounds(X, Y, width, height);
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
        removeChildComponent(&tvc2);
        removeChildComponent(&dvc);
        removeChildComponent(&ovc);
        
        //svc.setVisible(false);
        //nvc.setVisible(false);
        
        removeChildComponent(&svc2);
        removeChildComponent(&nvc2);
        removeChildComponent(&dvc2);
        
        
        if (type == DisplayKeymap)
        {
            addAndMakeVisible(&kvc);
        }
        else if (type == DisplayTuning)
        {
            addAndMakeVisible(&tvc2);
        }
        else if (type == DisplayTempo)
        {
            addAndMakeVisible(&ovc);
        }
        else if (type == DisplaySynchronic)
        {
            addAndMakeVisible(&svc2);
        }
        else if (type == DisplayNostalgic)
        {
            addAndMakeVisible(&nvc2);
        }
        else if (type == DisplayDirect)
        {
            addAndMakeVisible(&dvc2);
        }
        else if (type == DisplayGeneral)
        {
            addAndMakeVisible(&gvc);
        }
        
    }
    
    GeneralViewController gvc;
    
    KeymapViewController kvc;
    TuningViewController2 tvc2;
    //SynchronicViewController svc;
    //NostalgicViewController nvc;
    DirectViewController dvc;
    TempoViewController ovc;
    
    SynchronicViewController2 svc2;
    NostalgicViewController2 nvc2;
    DirectViewController2 dvc2;
    
    
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
