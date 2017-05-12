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


typedef enum BKMultiSliderType {
    HorizontalMultiSlider = 0,
    VerticalMultiSlider,
    HorizontalMultiBarSlider,
    VerticalMultiBarSlider,
    BKMultiSliderTypeNil
} BKMultiSliderType;


class BKMultiSliderLookAndFeel : public LookAndFeel_V3
{

public:
    
    BKMultiSliderLookAndFeel()
    {
        //setColour (TextButton::buttonColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    }
    ~BKMultiSliderLookAndFeel() {}
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style, Slider& slider) override;
};


class BKSubSlider : public Slider
{
public:
    
    BKSubSlider (SliderStyle sstyle, double min, double max, double def, double increment, int width, int height);
    ~BKSubSlider();

    void valueChanged() override;
    double getValueFromText	(const String & text ) override;
    bool isActive() { return active; }
    void isActive(bool newactive) {active = newactive; }
    void setMinMaxDefaultInc(std::vector<float> newvals);
        
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    int sliderWidth, sliderHeight;
    
    bool sliderIsVertical;
    bool sliderIsBar;
    
    bool active;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSubSlider)
};


class BKMultiSliderListener
{
    
public:
    
    //BKMultiSliderListener() {}
    virtual ~BKMultiSliderListener() {};
    
    virtual void multiSliderValueChanged(String name, int whichSlider, Array<float> values) = 0;
    virtual void multiSliderAllValuesChanged(String name, Array<Array<float>> values) = 0;
};


class BKMultiSlider :
public BKComponent,
public Slider::Listener,
public TextEditor::Listener
{
    
public:
    
    BKMultiSlider(BKMultiSliderType which);
    ~BKMultiSlider();

    void addSlider(int where, bool active);
    void addSubSlider(int where, bool active);

    void deactivateSlider(int where);
    void deactivateAll();
    void deactivateAllAfter(int where);
    void deactivateAllBefore(int where);
    
    int whichSlider (const MouseEvent &e);
    int whichSubSlider (int which);
    int whichActiveSlider (int which);
    
    void mouseDrag(const MouseEvent &e) override;
    void mouseMove(const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    void mouseDown (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;
    
    void setTo(Array<float> newvals, NotificationType newnotify);
    void setTo(Array<Array<float>> newvals, NotificationType newnotify);
    void setMinMaxDefaultInc(std::vector<float> newvals);
    
    void cleanupSliderArray();
    void resetRanges();
    
    void resized() override;
    
    ListenerList<BKMultiSliderListener> listeners;
    void addMyListener(BKMultiSliderListener* listener)     { listeners.add(listener);      }
    void removeMyListener(BKMultiSliderListener* listener)  { listeners.remove(listener);   }
    void setName(String newName)                            { sliderName = newName; showName.setText(sliderName, dontSendNotification);        }
    String getName()                                        { return sliderName; }
    
    Array<Array<float>> getAllValues();
    Array<Array<float>> getAllActiveValues();
    Array<float> getOneSliderBank(int which);
    
private:
    
    BKMultiSliderLookAndFeel activeSliderLookAndFeel;
    BKMultiSliderLookAndFeel passiveSliderLookAndFeel;
    String sliderName;
    BKLabel showName;

    bool dragging;
    bool arrangedHorizontally;
    bool sliderIsVertical;
    bool sliderIsBar;
    int currentSubSlider;
    
    Slider::SliderStyle subsliderStyle;
    
    double currentInvisibleSliderValue;
    
    OwnedArray<OwnedArray<BKSubSlider>> sliders;
    ScopedPointer<BKSubSlider> displaySlider;
    ScopedPointer<BKSubSlider> bigInvisibleSlider;
    ScopedPointer<TextEditor> editValsTextField;
    
    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    
    int totalWidth;
    int sliderHeight;
    float sliderWidth;
    int displaySliderWidth;
    
    int numActiveSliders;
    int numDefaultSliders;
    int numVisibleSliders;
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    
    void showModifyPopupMenu(int which);
    static void sliderModifyMenuCallback (const int result, BKMultiSlider* slider, int which);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};


class BKSingleSliderListener
{
    
public:
    
    //BKSingleSliderListener() {}
    virtual ~BKSingleSliderListener() {};
    
    virtual void BKSingleSliderValueChanged(String name, double val) = 0;
};

//basic horizontal slider with its own text box and label
//entering values in the text box will reset the range as needed
class BKSingleSlider :
public BKComponent,
public Slider::Listener,
public TextEditor::Listener
{
public:
    BKSingleSlider(String sliderName, double min, double max, double def, double increment);
    ~BKSingleSlider() {};
    
    Slider thisSlider;
    
    String sliderName;
    Label showName;
    
    TextEditor valueTF;
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void resized() override;
    
    ListenerList<BKSingleSliderListener> listeners;
    void addMyListener(BKSingleSliderListener* listener)     { listeners.add(listener);      }
    void removeMyListener(BKSingleSliderListener* listener)  { listeners.remove(listener);   }
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)

};


class BKRangeSliderListener
{
    
public:
    
    //BKRangeSliderListener() {}
    virtual ~BKRangeSliderListener() {};
    
    virtual void BKRangeSliderValueChanged(String name, double min, double max) = 0;
};


class BKRangeSlider :
public BKComponent,
public Slider::Listener,
public TextEditor::Listener
{
public:
    BKRangeSlider(String sliderName, double min, double max, double defmin, double defmax, double increment);
    ~BKRangeSlider() {};
    
    Slider thisSlider;
    
    String sliderName;
    Label showName;
    
    TextEditor minValueTF;
    TextEditor maxValueTF;
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void resized() override;
    
    ListenerList<BKRangeSliderListener> listeners;
    void addMyListener(BKRangeSliderListener* listener)     { listeners.add(listener);      }
    void removeMyListener(BKRangeSliderListener* listener)  { listeners.remove(listener);   }
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefaultMin, sliderDefaultMax;
    double sliderIncrement;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKRangeSlider)
    
};



#endif  // BKSLIDER_H_INCLUDED
