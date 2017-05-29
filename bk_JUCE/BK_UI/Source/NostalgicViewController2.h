/*
  ==============================================================================

    NostalgicViewController2.h
    Created: 24 May 2017 7:13:23pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"
#include "PluginProcessor.h"
#include "BKListener.h"
#include "BKComponent.h"
#include "BKSlider.h"
#include "BKGraph.h"

class NostalgicViewController2 :
public BKComponent,
public BKWaveDistanceUndertowSliderListener,
//public BKListener,
//public BKSingleSliderListener,
//public BKEditableComboBoxListener,
//public SliderListener,
public Timer
{
public:
    NostalgicViewController2(BKAudioProcessor&, BKItemGraph* theGraph);
    ~NostalgicViewController2() {};
    
    BKWaveDistanceUndertowSlider nDisplaySlider;
    
    void paint (Graphics&) override;
    void resized() override;
    
    void updateFields();
    void bkMessageReceived (const String& message);
    //void updateFields(NotificationType notify);
    void timerCallback() override;
    void BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow) override;
    

private:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;

};
