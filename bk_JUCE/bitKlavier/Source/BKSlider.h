/*
 ==============================================================================
 
 BKSlider.h
 Created: 6 Apr 2017 9:50:44pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

/* TODO
 
 1. need version of SingleSlider that can handle multiple values (for Nostalgic transposition)
 
 */

#ifndef BKSLIDER_H_INCLUDED
#define BKSLIDER_H_INCLUDED

#include "BKUtilities.h"
#include "BKComponent.h"
#include "BKLookAndFeel.h"
#include "BKUIComponents.h"

// ******************************************************************************************************************** //
// **************************************************  BKSubSlider **************************************************** //
// ******************************************************************************************************************** //

class BKSubSlider : public Slider
{
public:
    
    BKSubSlider (SliderStyle sstyle, double min, double max, double def, double increment, int width, int height);
    ~BKSubSlider();
    
    double getValueFromText    (const String & text ) override;
    bool isActive() { return active; }
    void isActive(bool newactive) {active = newactive; }
    void setMinMaxDefaultInc(std::vector<float> newvals);
    void setSkewFromMidpoint(bool sfm);
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    int sliderWidth, sliderHeight;
    
    bool sliderIsVertical;
    bool sliderIsBar;
    bool skewFromMidpoint;
    
    bool active;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSubSlider)
};




// ******************************************************************************************************************** //
// **************************************************  BKMultiSlider ************************************************** //
// ******************************************************************************************************************** //

class BKMultiSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
#if JUCE_IOS
, public WantsBigOne
#endif
{
    
public:
    
    BKMultiSlider(BKMultiSliderType which);
    ~BKMultiSlider();
    
    void addSlider(int where, bool active, NotificationType newnotify);
    void addSubSlider(int where, bool active, NotificationType newnotify);
    
    void deactivateSlider(int where, NotificationType notify);
    void deactivateAll(NotificationType notify);
    void deactivateAllAfter(int where, NotificationType notify);
    void deactivateAllBefore(int where, NotificationType notify);
    
    inline void setText(String text) { editValsTextField->setText(text, dontSendNotification); }
    
    inline TextEditor* getTextEditor(void)
    {
        return editValsTextField.get();
    }
    
    inline void dismissTextEditor(bool setValue = false)
    {
        if (setValue)   textEditorReturnKeyPressed(*editValsTextField);
        else            textEditorEscapeKeyPressed(*editValsTextField);
    }
    
    int whichSlider (const MouseEvent &e);
    int whichSubSlider (int which);
    int whichSubSlider (int which, const MouseEvent &e);
    int whichActiveSlider (int which);
    
    void mouseDrag(const MouseEvent &e) override;
    void mouseMove(const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    void mouseDown (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    
    void setTo(Array<float> newvals, NotificationType newnotify);
    void setTo(Array<Array<float>> newvals, NotificationType newnotify);
    void setMinMaxDefaultInc(std::vector<float> newvals);
    void setCurrentSlider(int activeSliderNum);
    
    void setAllowSubSlider(bool ss) { allowSubSliders = ss; }
    void setSubSliderName(String ssname) { subSliderName = ssname; }
    void setSkewFromMidpoint(bool sfm);
    
    void cleanupSliderArray();
    void resetRanges();
    
    inline int getNumActive(void) const noexcept { return numActiveSliders;}
    inline int getNumVisible(void) const noexcept { return numVisibleSliders;}
    
    void resized() override;
    
    class Listener
    {
        
    public:
        
        //BKMultiSlider::Listener() {}
        virtual ~Listener() {};
        
        virtual void multiSliderValueChanged(String name, int whichSlider, Array<float> values) = 0;
        virtual void multiSliderAllValuesChanged(String name, Array<Array<float>> values) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    void setName(String newName)                            { sliderName = newName; showName.setText(sliderName, dontSendNotification);        }
    String getName()                                        { return sliderName; }
    
    Array<Array<float>> getAllValues();
    Array<Array<float>> getAllActiveValues();
    Array<float> getOneSliderBank(int which);
    
    inline String getText(void){
        return editValsTextField->getText();
    }
    
private:
    
    BKMultiSliderLookAndFeel activeSliderLookAndFeel;
    BKMultiSliderLookAndFeel passiveSliderLookAndFeel;
    BKMultiSliderLookAndFeel highlightedSliderLookAndFeel;
    BKMultiSliderLookAndFeel displaySliderLookAndFeel;
    
    String sliderName;
    BKLabel showName;
    
    bool dragging;
    bool arrangedHorizontally;
    bool sliderIsVertical;
    bool sliderIsBar;
    int currentSubSlider;
    int lastHighlightedSlider;
    bool focusLostByEscapeKey;
    bool skewFromMidpoint;
    
    Slider::SliderStyle subsliderStyle;
    
    double currentInvisibleSliderValue;
    
    OwnedArray< OwnedArray<BKSubSlider>> sliders;
    ScopedPointer< BKSubSlider> displaySlider;
    ScopedPointer< BKSubSlider> bigInvisibleSlider;
    ScopedPointer< TextEditor> editValsTextField;
    
    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    
    int totalWidth;
    int sliderHeight;
    float sliderWidth;
    int displaySliderWidth;
    
    bool allowSubSliders;
    String subSliderName;
    
    int numActiveSliders;
    int numDefaultSliders;
    int numVisibleSliders;
    
    float clickedHeight;
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor&) override;
    
    void showModifyPopupMenu(int which);
    static void sliderModifyMenuCallback (const int result, BKMultiSlider* slider, int which);
    
    void highlight(int activeSliderNum);
    void deHighlight(int sliderNum);
    int getActiveSlider(int sliderNum);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};



// ******************************************************************************************************************** //
// **************************************************  BKSingleSlider ************************************************* //
// ******************************************************************************************************************** //


//basic horizontal slider with its own text box and label
//entering values in the text box will reset the range as needed
class BKSingleSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
#if JUCE_IOS
, public WantsBigOne
#endif
{
public:
    BKSingleSlider(String sliderName, double min, double max, double def, double increment);
    ~BKSingleSlider()
    {
        displaySlider->setLookAndFeel(nullptr);
        setLookAndFeel(nullptr);
        
    };
    
    Slider thisSlider;
    ScopedPointer<Slider> displaySlider;
    
    String sliderName;
    BKLabel showName;
    bool textIsAbove;
    
    BKTextEditor valueTF;
    
    inline String getText(void) { return valueTF.getText(); }
    inline void setText(String text, NotificationType notify = dontSendNotification) { valueTF.setText(text, notify);}
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    void setTextIsAbove(bool nt)    { textIsAbove = nt; }
    
    void setJustifyRight(bool jr)
    {
        justifyRight = jr;
        if (justifyRight) showName.setJustificationType(Justification::bottomRight);
        else showName.setJustificationType(Justification::bottomLeft);
    }
    
    void setValue(double newval, NotificationType notify);
    void checkValue(double newval);
    double getValue() {return thisSlider.getValue();}
    
    void setDisplayValue(double newval) { displaySlider->setValue(newval); }
    void displaySliderVisible(bool vis) { displaySlider->setVisible(vis); }
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &e) override;
    
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& textEditor) override;
    void resized() override;
    
    void setSkewFactor (double factor, bool symmetricSkew) { thisSlider.setSkewFactor(factor, symmetricSkew); }
    void setSkewFactorFromMidPoint (double sliderValueToShowAtMidPoint    ) { thisSlider.setSkewFactorFromMidPoint(sliderValueToShowAtMidPoint); }
    
    class Listener
    {
    public:
        
        //BKSingleSlider::Listener() {}
        virtual ~Listener() {};
        
        virtual void BKSingleSliderValueChanged(String name, double val) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }

    
    void setDim(float newAlpha);
    void setBright();
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    bool focusLostByEscapeKey;
    
    bool justifyRight;
    
    BKDisplaySliderLookAndFeel displaySliderLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)
    
};



// ******************************************************************************************************************** //
// **************************************************  BKRangeSlider ************************************************** //
// ******************************************************************************************************************** //


class BKRangeSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
#if JUCE_IOS
, public WantsBigOne
#endif
{
public:
    BKRangeSlider(String sliderName, double min, double max, double defmin, double defmax, double increment);
    ~BKRangeSlider()
    {
        minSlider.setLookAndFeel(nullptr);
        maxSlider.setLookAndFeel(nullptr);
        invisibleSlider.setLookAndFeel(nullptr);
        displaySlider->setLookAndFeel(nullptr);
    };
    
    Slider minSlider;
    Slider maxSlider;
    String minSliderName;
    String maxSliderName;
    
    Slider invisibleSlider;
    
    ScopedPointer<Slider> displaySlider;
    
    String sliderName;
    BKLabel showName;
    
    BKTextEditor minValueTF;
    BKTextEditor maxValueTF;
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    void setMinValue(double newval, NotificationType notify);
    void setMaxValue(double newval, NotificationType notify);
    void setIsMinAlwaysLessThanMax(bool im) { isMinAlwaysLessThanMax = im; }
    
    void setDisplayValue(double newval) { displaySlider->setValue(newval); }
    void displaySliderVisible(bool vis) { displaySlider->setVisible(vis); }
    
    void setJustifyRight(bool jr)
    {
        justifyRight = jr;
        if (justifyRight) showName.setJustificationType(Justification::bottomRight);
        else showName.setJustificationType(Justification::bottomLeft);
    }
    
    inline void setText(BKRangeSliderType which, String text)
    {
        if (which == BKRangeSliderMin)      minValueTF.setText(text, false);
        else if (which == BKRangeSliderMax) maxValueTF.setText(text, false);
    }
    
    inline TextEditor* getTextEditor(BKRangeSliderType which)
    {
        if (which == BKRangeSliderMin) return &minValueTF;
        if (which == BKRangeSliderMax) return &maxValueTF;
        
        return nullptr;
    }
    
    inline void dismissTextEditor(bool setValue = false)
    {
        if (setValue)
        {
            textEditorReturnKeyPressed(minValueTF);
            textEditorReturnKeyPressed(maxValueTF);
        }
        else
        {
            textEditorEscapeKeyPressed(minValueTF);
            textEditorEscapeKeyPressed(maxValueTF);
        }
    }
    
    void checkValue(double newval);
    void rescaleMinSlider();
    void rescaleMaxSlider();
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& textEditor) override;
    void resized() override;
    void sliderDragEnded(Slider *slider) override;
    void mouseDown (const MouseEvent &event) override;
    
    void setDim(float newAlpha);
    void setBright();
    
    class Listener
    {
        
    public:
        virtual ~Listener() {};
        
        virtual void BKRangeSliderValueChanged(String name, double min, double max) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefaultMin, sliderDefaultMax;
    double sliderIncrement;
    
    bool newDrag;
    bool clickedOnMinSlider;
    bool isMinAlwaysLessThanMax;
    bool focusLostByEscapeKey;
    bool justifyRight;
    
    BKRangeMinSliderLookAndFeel minSliderLookAndFeel;
    BKRangeMaxSliderLookAndFeel maxSliderLookAndFeel;
    BKDisplaySliderLookAndFeel displaySliderLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKRangeSlider)
    
};



// ******************************************************************************************************************** //
// *******************************************  BKWaveDistanceUndertowSlider ****************************************** //
// ******************************************************************************************************************** //


class BKWaveDistanceUndertowSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
#if JUCE_IOS
, public WantsBigOne
#endif
{
public:
    BKWaveDistanceUndertowSlider();
    ~BKWaveDistanceUndertowSlider()
    {
        for(int i=0; i<maxSliders; i++)
        {
            Slider* newSlider = displaySliders.getUnchecked(i);
            newSlider->setLookAndFeel(nullptr);
        }
    };
    
    ScopedPointer<Slider> wavedistanceSlider;
    ScopedPointer<Slider> undertowSlider;
    OwnedArray<Slider> displaySliders;
    
    String wavedistanceSliderName;
    String undertowSliderName;
    
    String sliderName;
    BKLabel wavedistanceName;
    BKLabel undertowName;
    
    BKTextEditor wavedistanceValueTF;
    BKTextEditor undertowValueTF;
    
    void setName(String newName)    { sliderName = newName; }
    String getName()                { return sliderName; }
    
    void updateSliderPositions(Array<int> newpositions);
    
    void sliderValueChanged (Slider *slider) override {};
    void resized() override;
    void sliderDragEnded(Slider *slider) override;
    void mouseDoubleClick (const MouseEvent &event) override;
    
    void setWaveDistance(int newwavedist, NotificationType notify);
    void setUndertow(int newundertow, NotificationType notify);
    
    void setDim(float newAlpha);
    void setBright();
    
    inline void setText(NostalgicParameterType which, String text)
    {
        //DBG("Nostalgic setText " + text);
        if (which == NostalgicUndertow)             undertowValueTF.setText(text, false);
        else if (which == NostalgicWaveDistance)    wavedistanceValueTF.setText(text, false);
    }
    
    inline TextEditor* getTextEditor(NostalgicParameterType which)
    {
        if (which == NostalgicUndertow) return &undertowValueTF;
        
        if (which == NostalgicWaveDistance) return &wavedistanceValueTF;
        
        return nullptr;
    }
    
    inline void dismissTextEditor(bool setValue = false)
    {
        //DBG("nostalgic text editor dismissed");
        if (setValue)
        {
            textEditorReturnKeyPressed(wavedistanceValueTF);
            textEditorReturnKeyPressed(undertowValueTF);
        }
        else
        {
            textEditorEscapeKeyPressed(wavedistanceValueTF);
            textEditorEscapeKeyPressed(undertowValueTF);
        }
    }
    
    class Listener
    {
        
    public:
        
        //BKRangeSlider::Listener() {}
        virtual ~Listener() {};
        
        virtual void BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    
    
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& textEditor) override;
    
private:
    double sliderMin, sliderMax;
    double sliderIncrement;
    
    int maxSliders;
    
    bool newDrag;
    bool clickedOnMinSlider;
    
    ImageComponent sampleImageComponent;
    
    bool focusLostByEscapeKey;
    
    //BKRangeMinSliderLookAndFeel minSliderLookAndFeel; //possibly need to remake for this
    //BKRangeMaxSliderLookAndFeel maxSliderLookAndFeel;
    BKMultiSliderLookAndFeel displaySliderLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKWaveDistanceUndertowSlider)
    
};



// ******************************************************************************************************************** //
// ************************************************** BKStackedSlider ************************************************* //
// ******************************************************************************************************************** //

class BKStackedSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
#if JUCE_IOS
, public WantsBigOne
#endif
{
public:
    
    BKStackedSlider(String sliderName, double min, double max, double defmin, double defmax, double def, double increment);
    
    ~BKStackedSlider()
    {
        topSlider->setLookAndFeel(nullptr);
        
        for(int i=0; i<numSliders; i++)
        {
            Slider* newSlider = dataSliders.operator[](i);
            if(newSlider != nullptr)
            {
                newSlider->setLookAndFeel(nullptr);
            }
        }
    };
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& textEditor) override;
    void mouseDown (const MouseEvent &event) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseMove(const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    
    inline BKTextEditor* getTextEditor(void)
    {
        return editValsTextField.get();
    }
    
    inline void dismissTextEditor(bool setValue = false)
    {
        if (setValue)   textEditorReturnKeyPressed(*editValsTextField);
        else            textEditorEscapeKeyPressed(*editValsTextField);
    }
    
    void setTo(Array<float> newvals, NotificationType newnotify);
    void setValue(Array<float> newvals, NotificationType newnotify) { setTo(newvals, newnotify); }
    void resetRanges();
    int whichSlider();
    int whichSlider(const MouseEvent& e);
    void addSlider(NotificationType newnotify);
    
    inline String getText(void) { return editValsTextField->getText(); }
    inline void setText(String text) { editValsTextField->setText(text, dontSendNotification); }
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    
    void resized() override;
    
    void setDim(float newAlpha);
    void setBright();
    
    class Listener
    {
        
    public:
        
        virtual ~Listener() {};
        
        virtual void BKStackedSliderValueChanged(String name, Array<float> val) = 0; //rewrite all this to pass "this" and check by slider ref instead of name?
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    
private:
    
    ScopedPointer<Slider> topSlider; //user interacts with this
    OwnedArray<Slider> dataSliders;  //displays data, user controls with topSlider
    Array<bool> activeSliders;
    
    ScopedPointer<BKTextEditor> editValsTextField;
    
    int numSliders;
    int numActiveSliders;
    int clickedSlider;
    float clickedPosition;
    
    String sliderName;
    BKLabel showName;
    bool justifyRight;
    
    BKMultiSliderLookAndFeel stackedSliderLookAndFeel;
    BKMultiSliderLookAndFeel topSliderLookAndFeel;
    
    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    double currentDisplaySliderValue;
    
    bool focusLostByEscapeKey;
    bool focusLostByNumPad;
    bool mouseJustDown;
    
    Array<float> getAllActiveValues();
    
    void showModifyPopupMenu();
    static void sliderModifyMenuCallback (const int result, BKStackedSlider* ss);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKStackedSlider)
};


// ******************************************************************************************************************** //
// **************************************************  BKADSRSlider ************************************************** //
// ******************************************************************************************************************** //


class BKADSRSlider :
public Component,
public BKSingleSlider::Listener,
public BKTextButton::Listener
#if JUCE_IOS
, public WantsBigOne
#endif
{
public:
    BKADSRSlider(String name);
    BKADSRSlider()
    {
        
    };
    
    ~BKADSRSlider()
    {
        setLookAndFeel(nullptr);
    };
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    
    void setButtonText(String buttonText)
    {
        adsrButton.setButtonText(buttonText);
    }
    
    void setAttackValue(int newval, NotificationType notify)
    {
        attackSlider->setValue(newval, notify);
    }
    void setDecayValue(int newval, NotificationType notify)
    {
        decaySlider->setValue(newval, notify);
    }
    void setSustainValue(float newval, NotificationType notify)
    {
        sustainSlider->setValue(newval, notify);
    }
    void setReleaseValue(int newval, NotificationType notify)
    {
        releaseSlider->setValue(newval, notify);
    }
    
    void setValue(Array<float> newvals, NotificationType notify)
    {
        setAttackValue(newvals[0], notify);
        setDecayValue(newvals[1], notify);
        setSustainValue(newvals[2], notify);
        setReleaseValue(newvals[3], notify);
        
        if(newvals.size() > 4)
        {
            if(newvals[4] > 0) setActive();
            else setPassive();
        }
    }
    
    int getAttackValue()    { return attackSlider->getValue(); }
    int getDecayValue()     { return decaySlider->getValue(); }
    float getSustainValue() { return sustainSlider->getValue(); }
    int getReleaseValue()   { return releaseSlider->getValue(); }
    
    void setIsButtonOnly(bool state) { isButtonOnly = state; }
    void setButtonToggle(bool state) {adsrButton.setToggleState(state, dontSendNotification);}
    bool getButtonToggle() { return adsrButton.getToggleState(); }
    
    void setHighlighted() { adsrButton.setToggleState(false, dontSendNotification); adsrButton.setLookAndFeel(&highlightedADSRLookAndFeel); }
    void setActive() { adsrButton.setToggleState(false, dontSendNotification); adsrButton.setLookAndFeel(&activeADSRLookAndFeel);}
    void setPassive() { adsrButton.setToggleState(true, dontSendNotification); adsrButton.setLookAndFeel(&passiveADSRLookAndFeel); }
    
    void setJustifyRight(bool jr)
    {
        justifyRight = jr;
        if (justifyRight) showName.setJustificationType(Justification::bottomRight);
        else showName.setJustificationType(Justification::bottomLeft);
    }
    
    void BKSingleSliderValueChanged(String name, double val) override;
    void buttonStateChanged (Button*) override;
    void buttonClicked (Button*) override;
    void mouseDown (const MouseEvent &event) override {};
    void mouseUp (const MouseEvent &event) override;

    
    void resized() override;
    
    void setDim(float newAlpha);
    void setBright();
    
    class Listener
    {
    public:
        virtual ~Listener() {};
        
        virtual void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) = 0;
        virtual void BKADSRButtonStateChanged(String name, bool mod, bool state) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    BKTextButton adsrButton;
    
    
private:
    
    ScopedPointer<BKSingleSlider> attackSlider;
    ScopedPointer<BKSingleSlider> decaySlider;
    ScopedPointer<BKSingleSlider> sustainSlider;
    ScopedPointer<BKSingleSlider> releaseSlider;
    
    String sliderName;
    BKLabel showName;
    
    BKButtonAndMenuLAF highlightedADSRLookAndFeel;
    BKButtonAndMenuLAF activeADSRLookAndFeel;
    BKButtonAndMenuLAF passiveADSRLookAndFeel;

    bool justifyRight;
    bool isButtonOnly;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKADSRSlider)
    
};




#endif  // BKSLIDER_H_INCLUDED

