/*
  ==============================================================================

    BKSlider.h
    Created: 6 Apr 2017 9:50:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef BKSLIDER_H_INCLUDED
#define BKSLIDER_H_INCLUDED

#include "BKUtilities.h"

//class BKMultiSlider
//class BKSingleSlider

class BKSingleSlider : public Slider
{
public:
    BKSingleSlider ();
    ~BKSingleSlider();
    
    void setDefault(double d) {sliderDefault = d;}
    void setRange(double min, double max);
    void setValue(double v, NotificationType n);
    void setSnapping(bool snap, double range);
    
private:
    
    Slider* s;
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderValue;
    bool sliderSnap;
    double sliderSnapRange;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)
};


 
#endif  // BKSLIDER_H_INCLUDED
