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
#include "BKComponent.h"

//class BKMultiSlider
//class BKSingleSlider

class BKSingleSlider : public Slider
{
public:
    BKSingleSlider ();
    ~BKSingleSlider();

    void valueChanged() override;
    double getValueFromText	(const String & text ) override;
    /*
    void mouseDown(const MouseEvent &e) override
    {
        DBG("mouse pos " + String(e.x) + " " + String(e.y));
    }
     */
    
private:

    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)
};



class BKMultiSlider : public BKComponent, public Slider::Listener
{
    
public:
    
    BKMultiSlider()
    {
        
        
        for(int i=0; i<numSliders; i++) {
            BKSingleSlider* newslider = new BKSingleSlider();
            newslider->addListener(this);
            sliders.add(newslider);
            addAndMakeVisible(newslider);
        }
        
        setSize(numSliders*sliders[0]->getWidth(), sliders[0]->getHeight());
        setInterceptsMouseClicks(true, true);
    }
    
    ~BKMultiSlider()
    {
        
    }

    
    /*
    void paint(Graphics &g) override
    {
        
    }
     */
    
    void mouseDown(const MouseEvent &e) override
    {
        DBG("mouse pos " + String(e.x) + " " + String(e.y));
    }
    
    
    void resized() override
    {
        for (int i=0; i<sliders.size(); i++)
        {
            sliders[i]->setTopLeftPosition(50*i, 0);
        }
        
    }
    
private:
    OwnedArray<BKSingleSlider> sliders;
    int numSliders = 10;
    
    void sliderValueChanged (Slider *slider) override
    {
        DBG("slider val changed " + String(slider->getValue()));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};


 
#endif  // BKSLIDER_H_INCLUDED
