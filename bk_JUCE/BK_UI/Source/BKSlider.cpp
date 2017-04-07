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
    s = new Slider();
    s->setSliderStyle (Slider::LinearBarVertical);
}


BKSingleSlider::~BKSingleSlider()
{
    
}

void BKSingleSlider::setRange(double min, double max)
{
    sliderMin = min;
    sliderMax = max;
    s->setRange(min, max);
}

void BKSingleSlider::setValue(double v, NotificationType n)
{
    sliderValue = v;
    
    if(sliderSnap) {
        if((std::abs(v - sliderDefault)) < sliderSnapRange) sliderValue = sliderDefault;
    }
    
    s->setValue(sliderValue, n);
}

void BKSingleSlider::setSnapping(bool snap, double range)
{
    sliderSnap = snap;
    sliderSnapRange = range;
}
