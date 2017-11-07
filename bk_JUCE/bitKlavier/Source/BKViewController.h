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

#include "BKNumberPad.h"

class BKViewController    : public BKComponent, public BKListener, public BKNumberPad::Listener, public WantsKeyboardListener
{
public:
    BKViewController(BKAudioProcessor& p, BKItemGraph* theGraph);
    
    ~BKViewController();
    
    PopupMenu getPrepOptionMenu(void);
    
    PopupMenu getModOptionMenu(void);
    
    void bkSingleSliderWantsKeyboard(BKSingleSlider* slider) override;
    
    void bkStackedSliderWantsKeyboard(BKStackedSlider* slider) override;
    
    void multiSliderWantsKeyboard(BKMultiSlider* slider) override;
    
    void bkRangeSliderWantsKeyboard(BKRangeSlider* slider, BKRangeSliderType which) override;
    
    void bkWaveDistanceUndertowSliderWantsKeyboard(BKWaveDistanceUndertowSlider* slider, NostalgicParameterType type) override;
    
    void keyboardSliderWantsKeyboard(BKKeyboardSlider* slider, KSliderTextFieldType which) override;
    
    void textEditorWantsKeyboard(BKTextEditor* editor) override;
    
    void numberPadChanged(BKNumberPad*) override;
    
    void numberPadDismissed(BKNumberPad*) override;
    
protected:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    TextButton actionButton;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    BKButtonAndMenuLAF buttonsAndMenusLAF2;
    
    ImageComponent iconImageComponent;
    TextButton hideOrShow;
    
    BKNumberPad numberPad;
    
    BKSingleSlider* latched_BKSingleSlider;
    BKStackedSlider* latched_BKStackedSlider;
    BKRangeSlider* latched_BKRangeSlider;
    BKMultiSlider* latched_BKMultiSlider;
    BKWaveDistanceUndertowSlider* latched_BKWDUTSlider;
    BKKeyboardSlider* latched_BKKeyboardSlider;
    BKTextEditor* latched_BKTextEditor;
    
    int lastId;
    
private:
    BKRangeSliderType rangeType;
    NostalgicParameterType wdutType;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKViewController)
};

