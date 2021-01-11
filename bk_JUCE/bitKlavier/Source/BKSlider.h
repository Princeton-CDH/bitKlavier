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
#include "BKLabel.h"

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

/*
 
 BKMultiSlider is a horizontal array of sliders.
 
 Each "slider" can have multiple sliders, facilitating, for instance, multiple transposition values
 
 The user can drag across the sliders to quickly set values
 
 The user can also enter slider values directly via a text editor:
    "1 [-1, -2, -3] 2 3" will create three sliders, and the second will have three overlaid sliders
 
 By default it has 12 sliders, but that can be increased by adding additional values via text entry
 
 Gaps in the slider array are possible, and indicated by a forward slash '/' in the text editor
 
 The API includes functions for setting the slider, either all at once (via text) or by individual slider (via mouse)
 and for deactivating individual sliders or sections of sliders.
 
*/

class BKMultiSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener,
public ImageButton::Listener
#if JUCE_IOS
, public WantsBigOne
#endif
{
    
public:
    
// *** Public Instance Methods *** //
    
    // constructor
    BKMultiSlider();
    ~BKMultiSlider();
        
    // when the client sends an array of only the active slider values, this will construct the complete array
    // of slider values, including inactive sliders, and then call setTo. So
    //      newActiveVals = {1, 2, 3}
    //      newactives = {true, false, false, true, true, false....... false}
    // will result in [1 / / 2 3 / / / / / / /], where / represents an inactive slider (gap)
    void setToOnlyActive(Array<Array<float>> newActiveVals, Array<bool> newactives, NotificationType newnotify);
    
    // as above, but takes a 1d array, for sliders that don't use subSliders
    void setToOnlyActive(Array<float> newActiveVals, Array<bool> newactives, NotificationType newnotify);
    
    // identifiers
    void setName(String newName){
        sliderName = newName;
        showName.setText(sliderName, dontSendNotification);
    }
    String getName() { return sliderName; }
    void setToolTipString(String newTip) { showName.setTooltip(newTip); bigInvisibleSlider->setTooltip(newTip); }

    // slider range and skew
    void setMinMaxDefaultInc(std::vector<float> newvals);
    void setSkewFromMidpoint(bool sfm);
    
    // highlight whichever slider is currently active in bK
    void setCurrentSlider(int activeSliderNum);
    void deHighlightCurrentSlider(void);
    inline int getNumVisible(void) const noexcept { return allSliderVals.size(); } //return numVisibleSliders;}
    
    // should this slider allow subsliders? (multiple transpositions, for instance)
    void setAllowSubSlider(bool ss) { allowSubSliders = ss; }
    
    // name of the subslider (add transposition, for instance)
    void setSubSliderName(String ssname) { subSliderName = ssname; }
    
    // listeners overrride these functions so they can get what they need from MultiSlider
    // Multislider will call these when values change, so the clients can update
    class Listener
    {
        public:
            
        virtual ~Listener() {};
        virtual void multiSliderValueChanged(String name, int whichSlider, Array<float> values) = 0;
        virtual void multiSliderAllValuesChanged(String name, Array<Array<float>> values, Array<bool> states) = 0;
    };
    
    // listeners
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
  
    
private:
    
// *** Private Instance Methods *** //

    // this is the main function for setting the multislider
    // the array of arrays is the slider values, with the inner array for subsliders (multiple sliders at one position
    // the array of bools sets which of the sliders is actually active
    void setTo(Array<Array<float>> newvals, Array<bool> newactives, NotificationType newnotify);
    
    // initialize the slider; it should have no less than numDefaultSliders, all set to sliderDefault value
    void initializeSliderVals(int howmany);
    
    // update and activate a particular slider
    void updateSliderVal(int which, int whichSub, float val);
    
    // print out the current vals, for debugging
    void printSliderVals();
    
    // draw the actual sliders
    void drawSliders(NotificationType newnotify);
    
    // return all values, including inactive sliders
    Array<Array<float>> getAllValues() { return allSliderVals; }
    
    // return only those sliders that are active
    Array<Array<float>> getAllActiveValues();
    
    // return the values for one slider
    Array<float> getOneSliderBank(int which);
    
    // inserts a slider into the multislider
    void addSlider(int where, bool active, NotificationType newnotify);
    
    // adds an additional slider at a particular index; it's possible to have multiple
    // sliders at one location, facilitating, for instance, multiple transpositions at one time
    void addSubSlider(int where, bool active, NotificationType newnotify);
    
    // as above, but updates the allSliderVals and whichSlidersActive first
    void addActiveSubSlider(int where, NotificationType newnotify);
    
    // these methods clear currently active sliders
    // exception: you can't clear the FIRST slider, as we need at least one active slider
    void deactivateSlider(int where, NotificationType notify);
    void deactivateAll(NotificationType notify);
    void deactivateAllAfter(int where, NotificationType notify);
    void deactivateAllBefore(int where, NotificationType notify);
    
    // slider values can be edited directly via a text editor
    // values in brackets will be collected at one index
    // for example: "0 [1 2 3] 4" will have three active sliders, and the second one will have three subsliders
    inline void setText(String text) { editValsTextField->setText(text, dontSendNotification); }
    inline TextEditor* getTextEditor(void) { return editValsTextField.get();}
    inline void dismissTextEditor(bool setValue = false);
    
    // identify which slider is mouseDowned
    int whichSlider (const MouseEvent &e);
    
    // identify which subSlider is closest to cursor after mouseDown on a particular slider
    int whichSubSlider (int which);
    
    // identify which subSlider is closest on mouseOver
    int whichSubSlider (int which, const MouseEvent &e);
    
    // update which slider is active after mouseDrag and mouseUp; to support dragging across multiple sliders
    int whichActiveSlider (int which);
    
    // for highlighting text related to sliders
    void highlight(int activeSliderNum);
    void deHighlight(int sliderNum);
    int getActiveSlider(int sliderNum);
    
    // rescale/normalize slider ranges
    void resetRanges();
    
    // mouse/text events
    void mouseDrag(const MouseEvent &e) override;
    void mouseMove(const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    void mouseDown (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor&) override;
    void buttonClicked(Button* button) override;
    void showModifyPopupMenu(int which);
    static void sliderModifyMenuCallback (const int result, BKMultiSlider* slider, int which);
    
    void resized() override;
    

// *** Instance Variables *** //
    
    // holds all the current values, including for sub sliders (hence the double array)
    Array<Array<float>> allSliderVals;
    
    // which of these sliders is active; the first is always true
    Array<bool> whichSlidersActive;
    
    // the invisible slider lays on top of all the sliders, its value
    // is used to set the values of the individual slider that the mouse is nearest
    double currentInvisibleSliderValue;
    
    // allow sliders to have multiple values
    bool allowSubSliders;
    
    // if allowSubSliders, what to call it "add [subSliderName]"
    String subSliderName;
    
    // to keep track of stuff
    int currentSubSlider;
    int lastHighlightedSlider;
    bool focusLostByEscapeKey;
    bool skewFromMidpoint;
    float clickedHeight;
    
    // default values
    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    
    // dimensions
    int totalWidth;
    int sliderHeight;
    float sliderWidth;
    int displaySliderWidth;

    // by default, how many sliders to show (12)
    int numDefaultSliders;
        
    // UI elements
    BKMultiSliderLookAndFeel activeSliderLookAndFeel;
    BKMultiSliderLookAndFeel passiveSliderLookAndFeel;
    BKMultiSliderLookAndFeel highlightedSliderLookAndFeel;
    BKMultiSliderLookAndFeel displaySliderLookAndFeel;
    String sliderName;
    BKLabel showName;
    Slider::SliderStyle subsliderStyle;
    OwnedArray<OwnedArray<BKSubSlider>> sliders;
    std::unique_ptr<BKSubSlider> displaySlider;
    std::unique_ptr<BKSubSlider> bigInvisibleSlider;
    std::unique_ptr<TextEditor> editValsTextField;
    std::unique_ptr<ImageButton> rotateButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};



// ******************************************************************************************************************** //
// **************************************************  BKSingleSlider ************************************************* //
// ******************************************************************************************************************** //


//basic horizontal slider with its own text box and label
//entering values in the text box will reset the range as needed
class BKSingleSlider :
public Component,
public ModdableComponent,
public Button::Listener,
public Slider::Listener,
public TextEditor::Listener
#if JUCE_IOS
, public WantsBigOne
#endif
{
public:
    BKSingleSlider(String sliderName, String Id, double min, double max, double def, double increment, String minDisplay = String());
    ~BKSingleSlider()
    {
        displaySlider->setLookAndFeel(nullptr);
        moddableOptionsButton.setLookAndFeel(nullptr);
        setLookAndFeel(nullptr);
    };
    
    Slider thisSlider;
    std::unique_ptr<Slider> displaySlider;
    
    TextButton moddableOptionsButton;
    
    String sliderName;
    BKLabel showName;
    bool textIsAbove;
    
    BKTextEditor valueTF;
    
    void setChangeNotificationOnlyOnRelease(bool yn) { thisSlider.setChangeNotificationOnlyOnRelease(yn);}
    
    inline String getText(void) { return valueTF.getText(); }
    inline void setText(String text, NotificationType notify = dontSendNotification) { valueTF.setText(text, notify);}
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    void setTextIsAbove(bool nt)    { textIsAbove = nt; }
    void setToolTipString(String newTip) { showName.setTooltip(newTip); thisSlider.setTooltip(newTip); valueTF.setTooltip(newTip);}
    
    void setJustifyRight(bool jr)
    {
        justifyRight = jr;
        if (justifyRight) showName.setJustificationType(Justification::bottomRight);
        else showName.setJustificationType(Justification::bottomLeft);
    }
    
    void setValue(double newval, NotificationType notify);
    void setValue(double newval, int numDecimalPoints, NotificationType notify);
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
    
    void buttonClicked(Button* b) override;
    
    void setSkewFactor (double factor, bool symmetricSkew) { thisSlider.setSkewFactor(factor, symmetricSkew); }
    void setSkewFactorFromMidPoint (double sliderValueToShowAtMidPoint    ) { thisSlider.setSkewFactorFromMidPoint(sliderValueToShowAtMidPoint); }
    
    void setSliderTextResolution(int res) {sliderTextResolution = res;}
    
    class Listener
    {
    public:
        
        //BKSingleSlider::Listener() {}
        virtual ~Listener() {};
        
        virtual void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) = 0;
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
    int sliderTextResolution;
    
    String minDisplay;
    
    bool focusLostByEscapeKey;
    
    bool justifyRight;
    
    BKDisplaySliderLookAndFeel displaySliderLookAndFeel;
    BKButtonAndMenuLAF buttonLAF;
    
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
    
    std::unique_ptr<Slider> displaySlider;
    
    String sliderName;
    BKLabel showName;
    
    BKTextEditor minValueTF;
    BKTextEditor maxValueTF;
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    void setToolTipString(String newTip) {  showName.setTooltip(newTip);
                                            invisibleSlider.setTooltip(newTip);
                                            minValueTF.setTooltip(newTip);
                                            maxValueTF.setTooltip(newTip); }
    
    void setMinValue(double newval, NotificationType notify);
    void setMaxValue(double newval, NotificationType notify);
    void setIsMinAlwaysLessThanMax(bool im) { isMinAlwaysLessThanMax = im; }

    double getMinValue() { return sliderMin; }
    double getMaxValue() { return sliderMax; }
    
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
        wavedistanceSlider->setLookAndFeel(nullptr);
        undertowSlider->setLookAndFeel(nullptr);
        for(int i=0; i<maxSliders; i++)
        {
            Slider* newSlider = displaySliders.getUnchecked(i);
            newSlider->setLookAndFeel(nullptr);
        }
    };
    
    std::unique_ptr<Slider> wavedistanceSlider;
    std::unique_ptr<Slider> undertowSlider;
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
    
    inline void setUndertowTooltip(String tip) {    undertowSlider->setTooltip(tip);
                                                    undertowName.setTooltip(tip);
                                                    undertowValueTF.setTooltip(tip); }
    
    inline void setWaveDistanceTooltip(String tip) {    wavedistanceSlider->setTooltip(tip);
                                                        wavedistanceName.setTooltip(tip);
                                                        wavedistanceValueTF.setTooltip(tip); }
    
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
    void setTooltip(String newTip)  { topSlider->setTooltip(newTip); showName.setTooltip(newTip); }
    
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
    
    std::unique_ptr<Slider> topSlider; //user interacts with this
    OwnedArray<Slider> dataSliders;  //displays data, user controls with topSlider
    Array<bool> activeSliders;
    
    std::unique_ptr<BKTextEditor> editValsTextField;
    
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
        attackSlider->setLookAndFeel(nullptr);
        decaySlider->setLookAndFeel(nullptr);
        sustainSlider->setLookAndFeel(nullptr);
        releaseSlider->setLookAndFeel(nullptr);
        adsrButton.setLookAndFeel(nullptr);
        
        setLookAndFeel(nullptr);
    };
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    void setToolTip(String newTip)  { adsrButton.setTooltip(newTip); }
    
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
    void setButtonToggle(bool state) { adsrButton.setToggleState(state, dontSendNotification);}
    bool getButtonToggle()           { return adsrButton.getToggleState(); }
    void setButtonMode(bool bmode)   { buttonMode = bmode; adsrButton.setVisible(bmode); }
    
    void setHighlighted() { adsrButton.setToggleState(false, dontSendNotification); adsrButton.setLookAndFeel(&highlightedADSRLookAndFeel); }
    void setActive() { adsrButton.setToggleState(false, dontSendNotification); adsrButton.setLookAndFeel(&activeADSRLookAndFeel);}
    void setPassive() { adsrButton.setToggleState(true, dontSendNotification); adsrButton.setLookAndFeel(&passiveADSRLookAndFeel); }
    
    void setJustifyRight(bool jr)
    {
        justifyRight = jr;
        if (justifyRight) showName.setJustificationType(Justification::bottomRight);
        else showName.setJustificationType(Justification::bottomLeft);
    }
    
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
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
    
    std::unique_ptr<BKSingleSlider> attackSlider;
    std::unique_ptr<BKSingleSlider> decaySlider;
    std::unique_ptr<BKSingleSlider> sustainSlider;
    std::unique_ptr<BKSingleSlider> releaseSlider;
    
    String sliderName;
    BKLabel showName;
    
    BKButtonAndMenuLAF highlightedADSRLookAndFeel;
    BKButtonAndMenuLAF activeADSRLookAndFeel;
    BKButtonAndMenuLAF passiveADSRLookAndFeel;

    bool justifyRight;
    bool isButtonOnly;
    
    bool buttonMode; //true by default; appears as button that can be clicked to open ADSR sliders

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKADSRSlider)
    
};




#endif  // BKSLIDER_H_INCLUDED

