/*
  ==============================================================================

    BKViewController.h
    Created: 20 Jun 2017 6:00:55pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#pragma once

#define AUTO_DELETE 0

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKListener.h"
#include "BKComponent.h"

#include "BKGraph.h"

#include "BKSlider.h"
#include "BKKeyboardSlider.h"

class BKViewController    : public BKComponent, public BKListener
{
public:
    BKViewController(BKAudioProcessor& p, BKItemGraph* theGraph);
    
    ~BKViewController();
    
    PopupMenu getPrepOptionMenu(void);
    
    PopupMenu getModOptionMenu(void);
    
protected:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    BKTextButton actionButton;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    BKButtonAndMenuLAF buttonsAndMenusLAF2;
    
    ImageComponent iconImageComponent;
    BKExitButton hideOrShow;

    int lastId;
    
private:
    BKRangeSliderType rangeType;
    NostalgicParameterType wdutType;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKViewController)
};

