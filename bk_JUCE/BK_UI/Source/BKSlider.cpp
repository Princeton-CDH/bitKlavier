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
