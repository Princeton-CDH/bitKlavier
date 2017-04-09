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

/*
 TODO
 -- expose basic parameters to top; max/min, etc... perhaps slider size
 -- snapping
 -- add text box for creating/copy/pasting directly
 */

typedef enum BKMultiSliderType {
    HorizontalMultiSlider = 0,
    VerticalMultiSlider,
    HorizontalMultiBarSlider,
    VerticalMultiBarSlider,
    BKMultiSliderTypeNil
} BKMultiSliderType;


class BKSingleSlider : public Slider
{
public:
    BKSingleSlider (SliderStyle sstyle, double min, double max, double def, double increment, int width, int height);
    ~BKSingleSlider();

    void valueChanged() override;
    double getValueFromText	(const String & text ) override;
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    int sliderWidth, sliderHeight;
    
    bool sliderIsVertical;
    bool sliderIsBar;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)
};



class BKMultiSlider : public BKComponent, public Slider::Listener
{
    
public:
    
    BKMultiSlider(BKMultiSliderType which);
    ~BKMultiSlider();

    void addSubSlider();
    void mouseDrag(const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    int whichSubSlider (const MouseEvent &e);
    void resetRanges();
    void resized() override;
    void initSizes();
    
private:
    bool dragging;
    bool arrangedHorizontally;
    bool sliderIsVertical;
    bool sliderIsBar;
    bool updatingTextBox = false;
    
    Slider::SliderStyle subsliderStyle;
    
    double currentInvisibleSliderValue;
    
    OwnedArray<BKSingleSlider> sliders;
    
    BKSingleSlider* bigInvisibleSlider;
    Slider* incDecSlider;
    
    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    
    int sliderWidth, sliderHeight;
    
    int numSliders = 1;
    
    void sliderValueChanged (Slider *slider) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};


 
#endif  // BKSLIDER_H_INCLUDED
