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
 -- expose basic parameters to top; max/min, etc... perhaps subSlider size
 -- display additional values and main value for clusters (like transposition vals [t1 t2 t3])?
 -- add control-click for opening edit window and selecting value where clicked, remove edit and +/- buttons
 -- add keystrokes where you can add/subtract sliders inside the multislider...\
 -- look and feel, with version of bar that is just a band, or multiple bands
    -- be able to click on one of multiple bands to move it
 -- remove number from display
 -- allow narrowing of sliders as numSlider increases
 -- displayVal slider that shows current value and also hovering value (hover over a slider in the multislider)
 -- highlight currentVal slider when synhronic is active
 -- possibly have faded out inactive sliders filling out a default width; dragging over them activates... ? have minimumSlidersToDisplay = 16, grey-out inactive ones
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



class BKMultiSlider : public BKComponent, public Slider::Listener, public Button::Listener, public TextEditor::Listener
{
    
public:
    
    BKMultiSlider(BKMultiSliderType which);
    ~BKMultiSlider();

    void addSubSlider();
    void mouseDrag(const MouseEvent &e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    void mouseUp (const MouseEvent &event) override;
    int  whichSubSlider (const MouseEvent &e);
    void resetRanges();
    void resized() override;
    void initSizes();
    Array<float> getAllValues();
    
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
    
    TextButton* editTextButton;
    TextEditor* editValsTextField;
    
    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    
    int sliderWidth, sliderHeight;
    
    int numSliders = 1;
    
    void sliderValueChanged (Slider *slider) override;
    void buttonClicked (Button* button) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};


class BKSliderLookAndFeel : public LookAndFeel_V3
{
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                      float sliderPos, float minSliderPos, float maxSliderPos,
                      const Slider::SliderStyle style, Slider& slider) override;
};
 
#endif  // BKSLIDER_H_INCLUDED
