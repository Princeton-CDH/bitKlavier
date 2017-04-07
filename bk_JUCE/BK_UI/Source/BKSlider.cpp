/*
  ==============================================================================

    BKSlider.cpp
    Created: 6 Apr 2017 9:50:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "BKSlider.h"

BKSingleSlider::BKSingleSlider ()
{
    setSliderStyle (Slider::LinearBarVertical);
    DBG("set slider style " + String(getSliderStyle()));
    
    setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    
    sliderMin = -1.;
    sliderMax = 1.;
    sliderDefault = 0.;
    sliderIncrement = 0.01;
    
    setRange(sliderMin, sliderMax, sliderIncrement);
    setValue(sliderDefault);
    setSize(50, 200);
 
}


BKSingleSlider::~BKSingleSlider()
{
    
}


void BKSingleSlider::valueChanged()
{
    //DBG("slider val changed: " + String(getValue()));
}

double BKSingleSlider::getValueFromText	(const String & text )
{
    //DBG("slider string: " + text);
    double newval = text.getDoubleValue();
    
    if(newval > getMaximum()) setRange(getMinimum(), newval, sliderIncrement);
    if(newval < getMinimum()) setRange(newval, getMaximum(), sliderIncrement);
    
    return newval;
}

