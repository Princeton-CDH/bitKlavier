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
 -- allow narrowing of sliders as numSlider increases
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


class BKSliderLookAndFeel : public LookAndFeel_V3
{

public:
    
    BKSliderLookAndFeel()
    {
        //setColour (TextButton::buttonColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    }
    ~BKSliderLookAndFeel() {}
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style, Slider& slider) override;
};


class BKSingleSlider : public Slider
{
public:
    BKSingleSlider (SliderStyle sstyle, double min, double max, double def, double increment, int width, int height);
    ~BKSingleSlider();

    void valueChanged() override;
    double getValueFromText	(const String & text ) override;
    bool isActive() { return active; }
    void isActive(bool newactive) {active = newactive; }
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    int sliderWidth, sliderHeight;
    
    bool sliderIsVertical;
    bool sliderIsBar;
    
    bool active;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)
};


class BKMultiSliderListener
{
    
public:
    
    BKMultiSliderListener() {}
    virtual ~BKMultiSliderListener() {};
    
    virtual void multiSliderValueChanged(String name, int whichSlider, float value) = 0;
    virtual void multiSliderAllValuesChanged(String name, Array<Array<float>> values) = 0;
};


class BKMultiSlider :
public BKComponent,
public Slider::Listener,
public Button::Listener,
public TextEditor::Listener
{
    
public:
    
    BKMultiSlider(BKMultiSliderType which);
    ~BKMultiSlider();

    void addSlider(int where, int depth, bool active);
    void addSubSlider(int where, int depth, bool active);
    void insertSlider(int where);
    void deleteSlider(int where);
    void deactivateSlider(int where);
    
    int whichSlider (const MouseEvent &e);
    int whichSubSlider (int which);
    
    void mouseDrag(const MouseEvent &e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    void mouseDown (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;
    
    void resetRanges();
    void resized() override;
    void initSizes();
    
    ListenerList<BKMultiSliderListener> listeners;
    void addMyListener(BKMultiSliderListener* listener)     { listeners.add(listener);      }
    void removeMyListener(BKMultiSliderListener* listener)  { listeners.remove(listener);   }
    void setName(String newName)                            { sliderName = newName; showName.setText(sliderName, dontSendNotification);        }
    String getName()                                        { return sliderName; }
    
    Array<Array<float>> getAllValues();
    
private:
    
    BKSliderLookAndFeel activeSliderLookAndFeel;
    BKSliderLookAndFeel passiveSliderLookAndFeel;
    String sliderName;
    BKLabel showName;

    bool dragging;
    bool arrangedHorizontally;
    bool sliderIsVertical;
    bool sliderIsBar;
    bool updatingTextBox = false;
    int currentSubSlider;
    
    Slider::SliderStyle subsliderStyle;
    
    double currentInvisibleSliderValue;
    
    OwnedArray<OwnedArray<BKSingleSlider>> sliders;
    
    BKSingleSlider* displaySlider;
    BKSingleSlider* bigInvisibleSlider;
    //Slider* incDecSlider;
    
    //TextButton* editTextButton;
    TextEditor* editValsTextField;
    
    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    
    int totalWidth;
    int sliderWidth, sliderHeight;
    
    int numSliders;
    
    void sliderValueChanged (Slider *slider) override;
    void buttonClicked (Button* button) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    
    void showModifyPopupMenu(int which);
    static void sliderModifyMenuCallback (const int result, BKMultiSlider* slider, int which);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};
 
#endif  // BKSLIDER_H_INCLUDED
