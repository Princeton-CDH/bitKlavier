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
    
    void setRange(double min, double max)
    {
        sliderMin = min;
        sliderMax = max;
        s->setRange(min, max);
    }
    
    void setDefault(double d) {sliderDefault = d;}
    void setValue(double v, NotificationType n)
    {
        sliderValue = v;
        
        if(sliderSnap) {
            if((std::abs(v - sliderDefault)) < sliderSnapRange) sliderValue = sliderDefault;
        }
        
        s->setValue(sliderValue, n);
    }
    
    void setSnapping(bool snap, double range)
    {
        sliderSnap = snap;
        sliderSnapRange = range;
    }
    
private:
    
    Slider* s;
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderValue;
    bool sliderSnap;
    double sliderSnapRange;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)
};

/*
struct SnappingSlider  : public Slider
{
    double snapValue (double attemptedValue, DragMode dragMode) override
    {
        if (dragMode == notDragging)
            return attemptedValue;  // if they're entering the value in the text-box, don't mess with it.
        
        if (attemptedValue > 40 && attemptedValue < 60)
            return 50.0;
        
        return attemptedValue;
    }
};
*/
 
#endif  // BKSLIDER_H_INCLUDED
