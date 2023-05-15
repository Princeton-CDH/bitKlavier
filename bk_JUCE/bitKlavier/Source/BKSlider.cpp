/*
 ==============================================================================
 
 BKSlider.cpp
 Created: 6 Apr 2017 9:50:44pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

// ******************************************************************************************************************** //
// **************************************************  BKSubSlider **************************************************** //
// ******************************************************************************************************************** //

//used in BKMultSlider

#include "BKSlider.h"

BKSubSlider::BKSubSlider (SliderStyle sstyle, double min, double max, double def, double increment, int width, int height):
sliderMin(min),
sliderMax(max),
sliderDefault(def),
sliderIncrement(increment),
sliderWidth(width),
sliderHeight(height)
{
    
    setSliderStyle(sstyle);
    active = true;
    
    if(sstyle == LinearVertical || sstyle == LinearBarVertical) sliderIsVertical = true;
    else sliderIsVertical = false;
    
    if(sstyle == LinearBarVertical || sstyle == LinearBar) sliderIsBar = true;
    else sliderIsBar = false;
    
    if(!sliderIsBar)
    {
        if(sliderIsVertical) setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
        else setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, false, 50, 20);
    }
    else
    {
        if(sliderIsVertical) setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    }
    
    setRange(sliderMin, sliderMax, sliderIncrement);
    setValue(sliderDefault, dontSendNotification);
    setSkewFromMidpoint(true);
    
}

BKSubSlider::~BKSubSlider()
{
    
}

void BKSubSlider::setMinMaxDefaultInc(std::vector<float> newvals)
{
    sliderMin = newvals[0];
    sliderMax = newvals[1];
    sliderDefault = newvals[2];
    sliderIncrement = newvals[3];
    setRange(sliderMin, sliderMax, sliderIncrement);
    if(skewFromMidpoint) setSkewFactorFromMidPoint(sliderDefault);
    else setSkewFactor (1., false);
    setValue(sliderDefault, dontSendNotification);
}

void BKSubSlider::setSkewFromMidpoint(bool sfm)
{
    skewFromMidpoint = sfm;
    
    if(skewFromMidpoint) setSkewFactorFromMidPoint(sliderDefault);
    else setSkewFactor (1., false);
}



double BKSubSlider::getValueFromText(const String & text )
{
    double newval = text.getDoubleValue();
    
    if(newval > getMaximum()) {
        sliderMax = newval;
        setRange(getMinimum(), newval, sliderIncrement);
    }
    
    if(newval < getMinimum()) {
        sliderMin = newval;
        setRange(newval, getMaximum(), sliderIncrement);
    }
    
    return newval;
}



// ******************************************************************************************************************** //
// **************************************************  BKMultiSlider ************************************************** //
// ******************************************************************************************************************** //

BKMultiSlider::BKMultiSlider()
{

    // initialize stuff
    passiveSliderLookAndFeel.setColour(Slider::thumbColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    highlightedSliderLookAndFeel.setColour(Slider::thumbColourId, Colours::red.withSaturation(1.));
    activeSliderLookAndFeel.setColour(Slider::thumbColourId, Colours::goldenrod.withMultipliedAlpha(0.75));
    displaySliderLookAndFeel.setColour(Slider::thumbColourId, Colours::red.withMultipliedAlpha(0.5));
    lastHighlightedSlider = 0;
    
    sliderMin = sliderMinDefault = -1.;
    sliderMax = sliderMaxDefault = 1.;
    sliderIncrement = 0.01;
    sliderDefault = 0.;
    numDefaultSliders = 12;
    allowSubSliders = false;
    subSliderName = "add subslider";
    sliderWidth = 20;
    sliderHeight = 60;
    displaySliderWidth = 80;
    clickedHeight = 0.;
    subsliderStyle = Slider::LinearBarVertical;
    
    // the bigInvisibleSlider sits on top of the individual sliders
    // it's used to set the values of the slider that the mouse is nearest
    bigInvisibleSlider = std::make_unique<BKSubSlider>(Slider::LinearBarVertical,
                                         sliderMin,
                                         sliderMax,
                                         sliderDefault,
                                         sliderIncrement,
                                         20,
                                         sliderHeight);
    
    // this displays the current value of whatever slider the mouse is nearest
    // is placed to the left of the array of sliders
    displaySlider = std::make_unique<BKSubSlider>(Slider::LinearBarVertical,
                                    sliderMin,
                                    sliderMax,
                                    sliderDefault,
                                    sliderIncrement,
                                    displaySliderWidth,
                                    sliderHeight);
    
    bigInvisibleSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0,0);
    bigInvisibleSlider->setAlpha(0.0);
    bigInvisibleSlider->addMouseListener(this, true);
    bigInvisibleSlider->setName("BIG");
    bigInvisibleSlider->addListener(this);
    bigInvisibleSlider->setLookAndFeel(&activeSliderLookAndFeel);
    addAndMakeVisible(*bigInvisibleSlider);
    
    displaySlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0,0);
    displaySlider->addMouseListener(this, true);
    displaySlider->setName("DISPLAY");
    displaySlider->addListener(this);
    displaySlider->setInterceptsMouseClicks(false, false);
    displaySlider->setLookAndFeel(&displaySliderLookAndFeel);
    addAndMakeVisible(*displaySlider);
    
    showName.setInterceptsMouseClicks(false, true);
    addAndMakeVisible(showName);
    
    // for editing the slider values by text
    editValsTextField = std::make_unique<TextEditor>();
    editValsTextField->setMultiLine(true);
    editValsTextField->setName("PARAMTXTEDIT");
    editValsTextField->setColour(TextEditor::highlightColourId, Colours::darkgrey);
    editValsTextField->addListener(this);
    addAndMakeVisible(*editValsTextField);
    
#if JUCE_IOS
    editValsTextField->setReadOnly(true);
    editValsTextField->setCaretVisible(true);
    editValsTextField->setSelectAllWhenFocused(false);
#endif
    
    // for rotating the slider values
    rotateButton = std::make_unique<ImageButton>();
    rotateButton->setImages(false, true, true,
                            ImageCache::getFromMemory(BinaryData::rotatearrow_png, BinaryData::rotatearrow_pngSize), 0.8f, Colours::transparentBlack,
                            Image(), 1.0f, Colours::transparentBlack,
                            Image(), 0.6, Colours::transparentBlack);
    rotateButton->setVisible(true);
    rotateButton->setName("ROTATE");
    rotateButton->setTooltip("Rotate values");
    rotateButton->addListener(this);
    addAndMakeVisible(*rotateButton);
    
    //create the default sliders, with one active
    initializeSliderVals(numDefaultSliders);
    
    // draw them! ready to go....
    drawSliders(dontSendNotification);
    
}

BKMultiSlider::~BKMultiSlider()
{
    
}

// initialize the slider; it should have no less than numDefaultSliders, all set to sliderDefault value
void BKMultiSlider::initializeSliderVals(int howmany)
{
    if (howmany < numDefaultSliders) howmany = numDefaultSliders;
    
    // slider values
    allSliderVals.clear();
    allSliderVals.ensureStorageAllocated(howmany);
    for (int i = 0; i < howmany; i++)
        allSliderVals.insert(i, {sliderDefault});
    
    // slider states (whether they are active or not (passive))
    whichSlidersActive.clearQuick();
    whichSlidersActive.ensureStorageAllocated(howmany);
    whichSlidersActive.set(0, true);
    for (int i = 1; i < howmany; i++)
        whichSlidersActive.set(i, false);

}


void BKMultiSlider::updateSliderVal(int which, int whichSub, float val)
{
    if (which < allSliderVals.size() && which >= 0)
    {
        Array<float> stemp = allSliderVals.getUnchecked(which);
        stemp.set(whichSub, val);
        allSliderVals.set(which, stemp);
        whichSlidersActive.set(which, true);
    }
}


void BKMultiSlider::printSliderVals()
{
    for (int i = 0; i < allSliderVals.size(); i++)
    {
        for (int j = 0; j < allSliderVals[i].size(); j++)
        {
            DBG("slider # " + String(i) + " subslider # " + String(j) +  " = " + String(allSliderVals[i][j]) + " isActive = " + String((int)whichSlidersActive[i]));
        }
    }
}


inline void BKMultiSlider::dismissTextEditor(bool setValue)
{
    if (setValue)   textEditorReturnKeyPressed(*editValsTextField);
    else            textEditorEscapeKeyPressed(*editValsTextField);
}


void BKMultiSlider::drawSliders(NotificationType newnotify)
{
    
    sliders.clearQuick(true);
    
    // rebuild display slider array
    for(int i = 0; i < allSliderVals.size(); i++)
    {
        if(i >= sliders.size()) addSlider(-1, false, newnotify);
        
        for(int j = 0; j < allSliderVals[i].size(); j++)
        {
            
            if(j >= sliders[i]->size()) addSubSlider(i, false, newnotify);
            
            BKSubSlider* refSlider = sliders[i]->operator[](j);
            if(refSlider != nullptr)
            {
                if(refSlider->getMaximum() < allSliderVals[i][j]) refSlider->setRange(sliderMin, allSliderVals[i][j], sliderIncrement);
                if(refSlider->getMinimum() > allSliderVals[i][j]) refSlider->setRange(allSliderVals[i][j], sliderMax, sliderIncrement);
                
                if (whichSlidersActive[i]) {
                    refSlider->setValue(allSliderVals[i][j], newnotify);
                    refSlider->setLookAndFeel(&activeSliderLookAndFeel);
                }
                else {
                    refSlider->setLookAndFeel(&passiveSliderLookAndFeel);
                }
                
                refSlider->setSkewFromMidpoint(skewFromMidpoint);

            }
        }
    }
    
    resetRanges();
    resized();
    displaySlider->setValue(allSliderVals[0][0]);
}


void  BKMultiSlider::setTo(Array<Array<float>> newvals, Array<bool> newactives, NotificationType newnotify)
{
    initializeSliderVals(newvals.size());
    
    for (int i = 0; i < newvals.size() && i < newactives.size(); i++)
    {
        allSliderVals.set(i, newvals[i]);

        if(newactives[i]) whichSlidersActive.set(i, true);
        else whichSlidersActive.set(i, false);
    }

    drawSliders(newnotify);
}

// when the client sends an array of only the active slider values, this will construct the complete array
// of slider values, including inactive sliders, and then call setTo
void BKMultiSlider::setToOnlyActive(Array<Array<float>> newActiveVals, Array<bool> newactives, NotificationType newnotify)
{
    Array<Array<float>> allvals;
    int inc = 0;
    
    for (int i = 0; i < newactives.size() && inc < newActiveVals.size(); i++)
    {
        if (newactives[i]) allvals.set(i, newActiveVals[inc++]);
        else allvals.set(i, {0});
    }

    setTo(allvals, newactives, newnotify);
}


void BKMultiSlider::setToOnlyActive(Array<float> newActiveVals, Array<bool> newactives, NotificationType newnotify)
{
    Array<Array<float>> allvals;
    for (int i = 0; i < newActiveVals.size(); i++)
    {
        allvals.set(i, {newActiveVals.getUnchecked(i)});
    }
    
    setToOnlyActive(allvals, newactives, newnotify);
}

void BKMultiSlider::setMinMaxDefaultInc(std::vector<float> newvals)
{
    sliderMin = sliderMinDefault = newvals[0];
    sliderMax = sliderMaxDefault = newvals[1];
    sliderDefault = newvals[2];
    sliderIncrement = newvals[3];
    
    for(int i = 0; i < sliders.size(); i++)
    {
        for(int j = 0; j < sliders[i]->size(); j++)
        {
            BKSubSlider* refSlider = sliders[i]->operator[](j);
            if(refSlider != nullptr)
            {
                refSlider->setMinMaxDefaultInc(newvals);
                refSlider->setSkewFromMidpoint(skewFromMidpoint);
            }
        }
    }
    
    displaySlider->setMinMaxDefaultInc(newvals);
    bigInvisibleSlider->setMinMaxDefaultInc(newvals);
    displaySlider->setSkewFromMidpoint(skewFromMidpoint);
    bigInvisibleSlider->setSkewFromMidpoint(skewFromMidpoint);
}


void BKMultiSlider::setSkewFromMidpoint(bool sfm)
{
    skewFromMidpoint = sfm;
    
    for(int i = 0; i < sliders.size(); i++)
    {
        for(int j = 0; j < sliders[i]->size(); j++)
        {
            BKSubSlider* refSlider = sliders[i]->operator[](j);
            if(refSlider != nullptr)
            {
                refSlider->setSkewFromMidpoint(skewFromMidpoint);
            }
        }
    }
    
    displaySlider->setSkewFromMidpoint(skewFromMidpoint);
    bigInvisibleSlider->setSkewFromMidpoint(skewFromMidpoint);
}


void BKMultiSlider::addSlider(int where, bool active, NotificationType newnotify)
{
    BKSubSlider* newslider;
    
    newslider      = new BKSubSlider(subsliderStyle,
                                     sliderMin,
                                     sliderMax,
                                     sliderDefault,
                                     sliderIncrement,
                                     sliderWidth,
                                     sliderHeight);
    
    newslider->setRange(sliderMin, sliderMax, sliderIncrement);
    newslider->setValue(sliderDefault, dontSendNotification);
    newslider->addListener(this);
    
    if(where < 0)
    {
        sliders.add(new OwnedArray<BKSubSlider>);
        sliders.getLast()->add(newslider);
    }
    else
    {
        sliders.insert(where, new OwnedArray<BKSubSlider>);
        sliders[where]->add(newslider);
    }
    
    addAndMakeVisible(newslider);
    
    if(active)
        newslider->setLookAndFeel(&activeSliderLookAndFeel);
    else
        newslider->setLookAndFeel(&passiveSliderLookAndFeel);
    
    if(newnotify == sendNotification)
    {
        listeners.call(&BKMultiSlider::Listener::multiSliderAllValuesChanged,
                       getName(),
                       getAllActiveValues(),
                       whichSlidersActive);
    }
    
}

void  BKMultiSlider::addActiveSubSlider(int which, NotificationType newnotify)
{
    
    if (which < allSliderVals.size() && which >= 0)
    {
        // get current array of values for this slider
        Array<float> stemp = allSliderVals.getUnchecked(which);
        
        // find slot for next one
        int whichSub = stemp.size();
        
        // calculate the value to set it at, based on where clicked
        float newval = bigInvisibleSlider->proportionOfLengthToValue( 1. - (clickedHeight / this->getHeight()));
        
        // update the state arrays
        stemp.set(whichSub, newval);
        allSliderVals.set(which, stemp);
        whichSlidersActive.set(which, true);
        
        // make the subSlider
        addSubSlider(which, true, newnotify);
    }
    
}

void BKMultiSlider::addSubSlider(int where, bool active, NotificationType newnotify)
{
    BKSubSlider* newslider;
    
    newslider = new BKSubSlider(subsliderStyle,
                                sliderMin,
                                sliderMax,
                                sliderDefault,
                                sliderIncrement,
                                sliderWidth,
                                sliderHeight);
        
    newslider->setRange(sliderMin, sliderMax, sliderIncrement);
    newslider->setValue(newslider->proportionOfLengthToValue( 1. - (clickedHeight / this->getHeight())), dontSendNotification);
    newslider->addListener(this);
    
    OwnedArray<BKSubSlider> *newsliderArray = sliders[where];
    newsliderArray->add(newslider);
    sliders.set(where, newsliderArray);
    
    addAndMakeVisible(newslider);
    
    if(active)
        newslider->setLookAndFeel(&activeSliderLookAndFeel);
    else
        newslider->setLookAndFeel(&passiveSliderLookAndFeel);
    
    if(newnotify == sendNotification)
    {
        listeners.call(&BKMultiSlider::Listener::multiSliderAllValuesChanged,
                       getName(),
                       getAllActiveValues(),
                       whichSlidersActive);
    }
}

void BKMultiSlider::deactivateSlider(int where, NotificationType notify)
{
    if (where > 0 && where < whichSlidersActive.size())
    {
        whichSlidersActive.set(where, false);
        drawSliders(notify);
    }
}


void BKMultiSlider::deactivateAll(NotificationType notify)
{
    for(int i = 0; i < sliders.size(); i++ )
    {
        deactivateSlider(i, notify);
    }
}


void BKMultiSlider::deactivateAllAfter(int where, NotificationType notify)
{
    for(int i = where+1; i < sliders.size(); i++ )
    {
        deactivateSlider(i, notify);
    }
}


void BKMultiSlider::deactivateAllBefore(int where, NotificationType notify)
{
    if (where > sliders.size()) where = sliders.size();
    for(int i = 0; i < where; i++ )
    {
        deactivateSlider(i, notify);
    }
}


// mouseDrag: updates the values of all sliders that the user drags over
void BKMultiSlider::mouseDrag(const MouseEvent& e)
{
    if(e.eventComponent == bigInvisibleSlider.get())
    {
        int which = whichSlider(e);
        if(e.mods.isShiftDown()) updateSliderVal(which, currentSubSlider, round(currentInvisibleSliderValue));
        else updateSliderVal(which, currentSubSlider, currentInvisibleSliderValue);
        
        if(which >= 0) {
            
            BKSubSlider* currentSlider = sliders[which]->operator[](currentSubSlider);
            if (currentSlider != nullptr)
            {
                if(e.mods.isShiftDown())
                {
                    currentSlider->setValue(round(currentInvisibleSliderValue));
                    displaySlider->setValue(round(currentInvisibleSliderValue));
                }
                else
                {
                    currentSlider->setValue(currentInvisibleSliderValue);
                    displaySlider->setValue(currentInvisibleSliderValue);
                }
                
                if(whichSlidersActive[which]){
                    currentSlider->setLookAndFeel(&activeSliderLookAndFeel);
                    listeners.call(&BKMultiSlider::Listener::multiSliderAllValuesChanged,
                                   getName(),
                                   getAllActiveValues(),
                                   whichSlidersActive);
                }
                else
                {
                    listeners.call(&BKMultiSlider::Listener::multiSliderValueChanged,
                                   getName(),
                                   whichActiveSlider(which),
                                   getOneSliderBank(which));
                }
            }
        }
    }
}


// mouseMove: updates the displaySlider to show the value of the slider that the pointer is nearest
void BKMultiSlider::mouseMove(const MouseEvent& e)
{
    if(e.eventComponent == bigInvisibleSlider.get())
    {
        int which = whichSlider(e);
        int whichSub = whichSubSlider(which, e);
        
        if (which < allSliderVals.size() && which >= 0)
        {
            if (whichSub < allSliderVals[which].size() && whichSub >= 0)
            {
                if (whichSlidersActive[which]) displaySlider->setValue(allSliderVals[which][whichSub]);
            }
        }
    }
}


// mouseDoubleClick: opens text window for editing slider values directly
void BKMultiSlider::mouseDoubleClick (const MouseEvent &e)
{
#if JUCE_IOS
    hasBigOne = true;
    editValsTextField->setText(arrayActiveFloatArrayToString(allSliderVals, whichSlidersActive), dontSendNotification);
    WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, editValsTextField.get(), sliderName);
#else
//#endif
    int which = whichSlider(e);
    int whichSave = which;
    
    //account for subsliders
    which += whichSubSlider(which, e);
    for (int i=0; i<whichSave; i++)
    {
        if(sliders[i]->size() > 0)
        {
            which += (sliders[i]->size() - 1);
        }
    }
    
    //highlight number for current slider
    StringArray tokens;
    tokens.addTokens(arrayActiveFloatArrayToString(allSliderVals, whichSlidersActive), false); //arrayFloatArrayToString
    
    int startPoint = 0;
    int endPoint;
    
    //need to skip brackets
    int numBrackets = 0;
    for(int i=0; i<=which + numBrackets; i++)
    {
        if(tokens[i] == "[" || tokens[i] == "]") numBrackets++;
    }
    
    for(int i=0; i < which + numBrackets; i++) {
        if(tokens[i] == "[") startPoint += 1;
        else if(tokens[i] == "]") startPoint += 2;
        else startPoint += tokens[i].length() + 1;
    }
    endPoint = startPoint + tokens[which + numBrackets].length();
    
    editValsTextField->setVisible(true);
    editValsTextField->toFront(true);
    editValsTextField->setText(arrayActiveFloatArrayToString(allSliderVals, whichSlidersActive));
    editValsTextField->setWantsKeyboardFocus(true);
    editValsTextField->grabKeyboardFocus();
    
    Range<int> highlightRange(startPoint, endPoint);
    editValsTextField->setHighlightedRegion(highlightRange);
    
    focusLostByEscapeKey = false;
#endif
}


// mouseDown: determines which subslider the mouseDown is nearest
//            checks to see if ctrl is down, for contextual menu
void BKMultiSlider::mouseDown (const MouseEvent &event)
{
    if (event.mouseWasClicked())
    {
        currentSubSlider = whichSubSlider(whichSlider(event));
        clickedHeight = event.y;
        
        if(event.mods.isCtrlDown())
        {
            showModifyPopupMenu(whichSlider(event));
        }
    }
}


// mouseUp: on shift-click, slider will be set to default value
void BKMultiSlider::mouseUp (const MouseEvent &event)
{
    if(event.mouseWasClicked())
    {
        if(event.mods.isShiftDown())
        {
            int which = whichSlider(event);
            
            if(which >= 0) {
                
                if (sliders[which]->size() == 0) addSubSlider(which, false, sendNotification);
                
                updateSliderVal(which, currentSubSlider, sliderDefault);
                
                BKSubSlider* currentSlider = sliders[which]->operator[](0);
                if (currentSlider != nullptr)
                {
                    currentSlider->setValue(sliderDefault); //again, need to identify which subslider to get
                }
                
                displaySlider->setValue(sliderDefault);
                
                listeners.call(&BKMultiSlider::Listener::multiSliderValueChanged,
                               getName(),
                               whichActiveSlider(which),
                               getOneSliderBank(which));
            }
        }
    }
}


int BKMultiSlider::whichSlider (const MouseEvent &e)
{
    int x = e.x;
    
    BKSubSlider* refSlider = sliders[0]->operator[](0);
    if (refSlider != nullptr)
    {
        int which = (x / refSlider->getWidth());
        if (which >= 0 && which < sliders.size()) return which;
    }
    
    return -1;
}


int BKMultiSlider::whichSubSlider (int which)
{
    if(which < 0) return 0;
    
    int whichSub = 0;
    float refDistance;
    
    BKSubSlider* refSlider = sliders[which]->operator[](0);
    if(refSlider != nullptr)
    {
        refDistance = fabs(refSlider->getValue() - currentInvisibleSliderValue);
    }
    
    for(int i = 0; i < sliders[which]->size(); i++)
    {
        BKSubSlider* currentSlider = sliders[which]->operator[](i);
        if(currentSlider != nullptr) {
            float tempDistance = fabs(currentSlider->getValue() - currentInvisibleSliderValue);
            if(tempDistance < refDistance)
            {
                whichSub = i;
                refDistance = tempDistance;
            }
        }
    }
    
    return whichSub;
}


int BKMultiSlider::whichSubSlider (int which, const MouseEvent &e)
{
    if(which < 0) return 0;
    
    int whichSub = 0;
    float refDistance;
    
    BKSubSlider* refSlider = sliders[which]->operator[](0);
    if(refSlider != nullptr)
    {
        refDistance = fabs(refSlider->getPositionOfValue(refSlider->getValue()) - e.y);
    }
    
    for(int i = 0; i < sliders[which]->size(); i++)
    {
        BKSubSlider* currentSlider = sliders[which]->operator[](i);
        if(currentSlider != nullptr) {
            float tempDistance = fabs(currentSlider->getPositionOfValue(currentSlider->getValue()) - e.y);
            if(tempDistance < refDistance)
            {
                whichSub = i;
                refDistance = tempDistance;
            }
        }
    }

    return whichSub;
}

// given a slider location 'which', how many active sliders
// are there up to and including 'which'
int BKMultiSlider::whichActiveSlider (int which)
{
    int counter = 0;
    if(which > whichSlidersActive.size()) which = whichSlidersActive.size();
    
    for(int i = 0; i < which; i++)
    {
        if(whichSlidersActive[which]) counter++;
    }
    
    return counter;
}


void BKMultiSlider::resetRanges()
{
    double sliderMinTemp = sliderMinDefault;
    double sliderMaxTemp = sliderMaxDefault;
    
    for (int i = 0; i < sliders.size(); i++)
    {
        for (int j = 0; j < sliders[i]->size(); j++)
        {
            BKSubSlider* currentSlider = sliders[i]->operator[](j);
            if (currentSlider != nullptr)
            {
                if (currentSlider->getValue() > sliderMaxTemp) sliderMaxTemp = currentSlider->getValue();
                if (currentSlider->getValue() < sliderMinTemp) sliderMinTemp = currentSlider->getValue();
            }
        }
    }
    
    if ((sliderMax != sliderMaxTemp) || sliderMin != sliderMinTemp)
    {
        sliderMax = sliderMaxTemp;
        sliderMin = sliderMinTemp;
        
        for (int i = 0; i < sliders.size(); i++)
        {
            for (int j = 0; j < sliders[i]->size(); j++)
            {
                BKSubSlider* currentSlider = sliders[i]->operator[](j);
                if (currentSlider != nullptr)
                {
                    currentSlider->setRange(sliderMin, sliderMax, sliderIncrement);
                }
            }
        }
        
        bigInvisibleSlider->setRange(sliderMin, sliderMax, sliderIncrement);
        displaySlider->setRange(sliderMin, sliderMax, sliderIncrement);
    }
}


void BKMultiSlider::resized()
{
    
    Rectangle<float> area (getLocalBounds().toFloat());
    Rectangle<float> bounds = area;
    
    displaySlider->setBounds(area.removeFromLeft(displaySliderWidth).toNearestInt());
    editValsTextField->setBounds(area.toNearestInt());
    editValsTextField->setVisible(false);
    
    Rectangle<float> nameSlab (area);
    nameSlab.removeFromTop(gYSpacing / 2.).removeFromRight(gXSpacing);
    showName.setBounds(nameSlab.toNearestInt());
    showName.setJustificationType(Justification::topRight);
    
    bigInvisibleSlider->setBounds(area.toNearestInt());

    sliderWidth = (float)area.getWidth() / sliders.size();
    
    for (int i = 0; i < sliders.size(); i++)
    {
        Rectangle<float> sliderArea (area.removeFromLeft(sliderWidth));
        for(int j = 0; j < sliders[i]->size(); j++)
        {
            BKSubSlider* currentSlider = sliders[i]->operator[](j);
            if(currentSlider != nullptr)
            {
                currentSlider->setBounds(sliderArea.toNearestInt());
            }
        }
    }
    
    Rectangle<float> rotateButtonBounds (bounds.getBottomLeft(), bounds.getBottomLeft().translated(displaySliderWidth*0.2, -displaySliderWidth*0.2));
    rotateButton->setBounds(rotateButtonBounds.toNearestInt());
    
    bigInvisibleSlider->toFront(false);
}


void BKMultiSlider::sliderValueChanged (Slider *slider)
{
    if (slider->getName() == "BIG")
    {
        currentInvisibleSliderValue = slider->getValue();
    }
}


void BKMultiSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == editValsTextField->getName())
    {
        editValsTextField->setVisible(false);
        editValsTextField->toBack();
        
        String ins = textEditor.getText();
        setTo(stringToArrayFloatArray(ins), slashToFalse(ins), sendNotification);
          
        listeners.call(&BKMultiSlider::Listener::multiSliderAllValuesChanged,
                       getName(),
                       getAllActiveValues(),
                       whichSlidersActive);
    }
}


void BKMultiSlider::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    if(textEditor.getName() == editValsTextField->getName())
    {
        focusLostByEscapeKey = true;
        editValsTextField->setVisible(false);
        editValsTextField->toBack();
        unfocusAllComponents();
    }
}


void BKMultiSlider::textEditorTextChanged(TextEditor& tf)
{
#if JUCE_IOS
    if (hasBigOne)
    {
        hasBigOne = false;
        textEditorReturnKeyPressed(tf);
    }
#endif
}


void BKMultiSlider::textEditorFocusLost(TextEditor& textEditor)
{
#if !JUCE_IOS
    if(textEditor.getName() == editValsTextField->getName())
    {
        editValsTextField->setVisible(false);
        editValsTextField->toBack();
        
        if(!focusLostByEscapeKey)
        {
            String ins = textEditor.getText();
            setTo(stringToArrayFloatArray(ins), slashToFalse(ins), sendNotification);
            
            listeners.call(&BKMultiSlider::Listener::multiSliderAllValuesChanged,
                           getName(),
                           getAllActiveValues(),
                           whichSlidersActive);
        }
    }
#endif
}


void BKMultiSlider::buttonClicked(Button* button)
{
    if (button->getName() == "ROTATE")
    {
        Array<Array<float>> values = getAllActiveValues();
        Array<float> swap = values.getLast();
        values.insert(0, swap);
        values.removeLast();
        setToOnlyActive(values, whichSlidersActive, sendNotification);
    }
}


Array<Array<float>> BKMultiSlider::getAllActiveValues()
{
    Array<Array<float>> allvals;

    for (int i = 0; (i < whichSlidersActive.size()) && (i < allSliderVals.size()); i++)
    {
        if (whichSlidersActive[i]) allvals.add(allSliderVals.getUnchecked(i));
    }
    
    return allvals;
}


Array<float> BKMultiSlider::getOneSliderBank(int which)
{
    if (which < allSliderVals.size() && which >= 0)
    {
        return allSliderVals[which];
    }
    
    return {0};
}


void BKMultiSlider::showModifyPopupMenu(int which)
{
    BKPopupMenu m;
    m.addItem (1, translate ("deactivate slider"), true, false);
    m.addItem (2, translate ("deactivate all after this"), true, false);
    m.addItem (3, translate ("deactivate all before this"), true, false);
    if(allowSubSliders) m.addItem (4, translate (subSliderName), true, false);
    m.addSeparator();
    
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::forComponent (sliderModifyMenuCallback, this, which));
}


void BKMultiSlider::sliderModifyMenuCallback (const int result, BKMultiSlider* ms, int which)
{
    if (ms == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    switch (result)
    {
        case 1: ms->deactivateSlider(which, sendNotification);
                break;
            
        case 2: ms->deactivateAllAfter(which, sendNotification);
                break;
            
        case 3: ms->deactivateAllBefore(which, sendNotification);
                break;
            
        case 4: ms->addActiveSubSlider(which, sendNotification);
                //ms->addSubSlider(which, true, sendNotification);
                ms->resized();
                break;
            
        default:  break;
    }
}


// find activeSliderNum, highlight it as current, dehighlight all the others...
void BKMultiSlider::setCurrentSlider(int activeSliderNum)
{
    // dim all inactive sliders
    for (int i = 1; i < allSliderVals.size(); i++)
    {
        for(int j = 0; j < sliders[i]->size(); j++)
        {
            if (!whichSlidersActive[i]) sliders[i]->operator[](j)->setLookAndFeel(&passiveSliderLookAndFeel);
        }
    }
    
    if (lastHighlightedSlider < sliders.size())
    {
        deHighlight(lastHighlightedSlider);
    }

    int sliderNum = getActiveSlider(activeSliderNum);
    highlight(sliderNum);
    displaySlider->setValue(sliders[sliderNum]->operator[](0)->getValue());
    
    lastHighlightedSlider = sliderNum;
}


int BKMultiSlider::getActiveSlider(int sliderNum)
{
    int sliderCount = 0;
    
    for (int i = 0; i < whichSlidersActive.size(); i++)
    {
        if (whichSlidersActive[i]) {
            if (sliderCount == sliderNum) return i;
            sliderCount++;
        }
    }
    return 0;
}


void BKMultiSlider::highlight(int activeSliderNum)
{
    //need to count through depth, but for now just the first one...
    for(int i = 0; i < sliders[activeSliderNum]->size(); i++)
    {
        sliders[activeSliderNum]->operator[](i)->setLookAndFeel(&highlightedSliderLookAndFeel);
    }
}


void BKMultiSlider::deHighlight(int sliderNum)
{
    for(int i = 0; i < sliders[sliderNum]->size(); i++)
    {
        sliders[sliderNum]->operator[](i)->setLookAndFeel(&activeSliderLookAndFeel);
    }
    
}

void BKMultiSlider::deHighlightCurrentSlider()
{
    for(int i = 0; i < sliders[lastHighlightedSlider]->size(); i++)
    {
        sliders[lastHighlightedSlider]->operator[](i)->setLookAndFeel(&activeSliderLookAndFeel);
    }
    
}



// ****************************************************************************************** //
// ********************************** BKSingleSlider **************************************** //
// ****************************************************************************************** //

BKSingleSlider::BKSingleSlider (String name, String Id, double min, double max, double def, double increment, String minDisplay):
ModdableComponent(Id, false),
//moddableOptionsButton("moddableOptions", 0.75f, Colours::antiquewhite),
sliderName(name),
sliderMin(min),
sliderMax(max),
sliderDefault(def),
sliderIncrement(increment),
minDisplay(minDisplay)
{
    textIsAbove = true;
    justifyRight = true;
    
    thisSlider.setColour(Slider::thumbColourId, Colours::goldenrod);
    thisSlider.setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.25));
    thisSlider.setColour(Slider::backgroundColourId, Colours::goldenrod.withMultipliedAlpha(0.1));
    thisSlider.setSliderStyle(Slider::LinearHorizontal);
    thisSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    thisSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    thisSlider.setValue(sliderDefault, dontSendNotification);
    thisSlider.addListener(this);
    thisSlider.addMouseListener(this, true);
    addAndMakeVisible(thisSlider);
    
    showName.setText(name, dontSendNotification);
    if (justifyRight) showName.setJustificationType(Justification::bottomRight);
    else showName.setJustificationType(Justification::bottomLeft);
    showName.addMouseListener(this, true);
    showName.setTooltip("tooltip text");
    addAndMakeVisible(showName);
    
    valueTF.setText(String(sliderDefault, 2, false), dontSendNotification);
    valueTF.addListener(this);
    valueTF.setSelectAllWhenFocused(true);
#if JUCE_IOS
    valueTF.setReadOnly(true);
#endif
    valueTF.addMouseListener(this, true);
    valueTF.setColour(TextEditor::highlightColourId, Colours::darkgrey);
    addAndMakeVisible(valueTF);
    
    moddableOptionsButton.setButtonText("...");
    moddableOptionsButton.setLookAndFeel(&buttonLAF);
    moddableOptionsButton.addListener(this);
    moddableOptionsButton.setTooltip("Edit modification settings for this slider");
    addAndMakeVisible(moddableOptionsButton);
    
    displaySlider = std::make_unique<Slider>();
    displaySlider->setRange(min, max, increment);
    displaySlider->setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    displaySlider->setLookAndFeel(&displaySliderLookAndFeel);
    displaySlider->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(*displaySlider);
    
    sliderTextResolution = -1;

}

void BKSingleSlider::setDim(float alphaVal)
{
    thisSlider.setAlpha(alphaVal);
    showName.setAlpha(alphaVal);
    valueTF.setAlpha(alphaVal);
    moddableOptionsButton.setAlpha(alphaVal);
}

void BKSingleSlider::setBright()
{
    thisSlider.setAlpha(1.);
    showName.setAlpha(1.);
    valueTF.setAlpha(1.);
    moddableOptionsButton.setAlpha(1.);
}

void BKSingleSlider::sliderValueChanged (Slider *slider)
{
    if(slider == &thisSlider)
    {
        if (thisSlider.getValue() <= sliderMin && !minDisplay.isEmpty())
        {
            valueTF.setText(minDisplay, dontSendNotification);
        }
        else
        {
            if(sliderTextResolution < 0) valueTF.setText(String(thisSlider.getValue()), dontSendNotification);
            else if (sliderTextResolution == 0) valueTF.setText(String((int) thisSlider.getValue()), dontSendNotification);
            else valueTF.setText(String(thisSlider.getValue(), sliderTextResolution), dontSendNotification);
        }
        listeners.call(&BKSingleSlider::Listener::BKSingleSliderValueChanged,
                       this,
                       getName(),
                       thisSlider.getValue());
    }
}

void BKSingleSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    String textval = textEditor.getText();
    double newval = textval.getDoubleValue();
    if (textval == minDisplay) newval = sliderMin;
    checkValue(newval);
    thisSlider.setValue(newval, sendNotification);
    
    unfocusAllComponents();
    
    listeners.call(&BKSingleSlider::Listener::BKSingleSliderValueChanged,
                   this,
                   getName(),
                   thisSlider.getValue());
}

void BKSingleSlider::textEditorTextChanged(TextEditor& textEditor)
{
    focusLostByEscapeKey = false;
#if JUCE_IOS
    if (hasBigOne)
    {
        hasBigOne = false;
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}

void BKSingleSlider::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    focusLostByEscapeKey = true;
    unfocusAllComponents();
}

void BKSingleSlider::textEditorFocusLost(TextEditor& textEditor)
{
#if !JUCE_IOS
//    if(!focusLostByEscapeKey)
//    {
//        textEditorReturnKeyPressed(textEditor);
//    }
#endif
}

void BKSingleSlider::checkValue(double newval)
{
    if(newval > thisSlider.getMaximum()) {
        thisSlider.setRange(thisSlider.getMinimum(), newval, sliderIncrement);
        displaySlider->setRange(thisSlider.getMinimum(), newval, sliderIncrement);
    }
    
    if(newval < thisSlider.getMinimum()) {
        thisSlider.setRange(newval, thisSlider.getMaximum(), sliderIncrement);
        displaySlider->setRange(newval, thisSlider.getMaximum(), sliderIncrement);
    }
    
    if(newval <= sliderMax && thisSlider.getMaximum() > sliderMax)
    {
        thisSlider.setRange(thisSlider.getMinimum(), sliderMax, sliderIncrement);
        displaySlider->setRange(thisSlider.getMinimum(), sliderMax, sliderIncrement);
    }
    
    if(newval >= sliderMin && thisSlider.getMinimum() < sliderMin)
    {
        thisSlider.setRange(sliderMin, thisSlider.getMaximum(), sliderIncrement);
        displaySlider->setRange(sliderMin, thisSlider.getMaximum(), sliderIncrement);
    }
}

void BKSingleSlider::mouseDown(const MouseEvent& e)
{
#if JUCE_IOS
    if (e.eventComponent != &thisSlider)
    {
        hasBigOne = true;
        WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, &valueTF, sliderName);
    }
#endif

    if (e.mods.isAltDown())
        ModdableComponent::listeners.call(&ModdableComponent::Listener::moddableComponentEditorOpened, this);
}


void BKSingleSlider::mouseUp(const MouseEvent &event)
{
    if(event.mouseWasClicked())
    {
        if(event.mods.isShiftDown())
        {
            checkValue(sliderDefault);
            thisSlider.setValue(sliderDefault, sendNotification);
            
        }
    }
}

void BKSingleSlider::mouseDrag(const MouseEvent &e)
{
    if(e.mods.isShiftDown())
    {
        thisSlider.setValue(round(thisSlider.getValue()), sendNotification);
    }
}

void BKSingleSlider::buttonClicked(Button* b)
{
    if (b->getName() == moddableOptionsButton.getName())
    {
        ModdableComponent::listeners.call(&ModdableComponent::Listener::moddableComponentEditorOpened, this);
    }
}

void BKSingleSlider::resized()
{
    if(textIsAbove)
    {
        Rectangle<int> area (getLocalBounds());
        Rectangle<int> textSlab (area.removeFromTop(gComponentTextFieldHeight));

        if(justifyRight)
        {
            if (!ModdableComponent::listeners.isEmpty())
            {
                Rectangle<int> buttonArea (textSlab.removeFromLeft(gComponentTextFieldHeight*2));
                moddableOptionsButton.setBounds(buttonArea);
            }
            textSlab.removeFromRight(gComponentSingleSliderXOffset);
            valueTF.setBounds(textSlab.removeFromRight(85));
            showName.setBounds(textSlab.removeFromLeft(getWidth() - 75));
        }
        else
        {
            if (!ModdableComponent::listeners.isEmpty())
            {
                Rectangle<int> buttonArea (textSlab.removeFromRight(gComponentTextFieldHeight*2));
                moddableOptionsButton.setBounds(buttonArea);
            }
            textSlab.removeFromLeft(gComponentSingleSliderXOffset);
            valueTF.setBounds(textSlab.removeFromLeft(75));
            showName.setBounds(textSlab.removeFromRight(getWidth() - 75));
        }
        
        thisSlider.setBounds(area.removeFromTop(gComponentSingleSliderHeight - gComponentTextFieldHeight));
        
        Rectangle<int> displaySliderArea = thisSlider.getBounds();
        displaySliderArea.reduce(gComponentSingleSliderXOffset, 0);
        displaySlider->setBounds(displaySliderArea.removeFromBottom(gComponentSingleSliderXOffset));
    }
    else
    {
        Rectangle<int> area (getLocalBounds());
        
        //Rectangle<int> textSlab (area.removeFromBottom(area.getHeight() / 3));
        Rectangle<int> textSlab (area.removeFromBottom(20));
        //textSlab.removeFromBottom(textSlab.getHeight() - 20);
        textSlab.removeFromRight(5);
        valueTF.setBounds(textSlab.removeFromRight(75));
        showName.setBounds(textSlab.removeFromRight(75));
        
        thisSlider.setBounds(area.removeFromBottom(20));
    }
}

void BKSingleSlider::setValue(double newval, NotificationType notify)
{
    checkValue(newval);
    thisSlider.setValue(newval, notify);
    if (thisSlider.getValue() == sliderMin && !minDisplay.isEmpty())
    {
        valueTF.setText(minDisplay, dontSendNotification);
    }
    else {
        if(sliderTextResolution < 0) valueTF.setText(String(thisSlider.getValue()), notify);
        else if (sliderTextResolution == 0) valueTF.setText(String((int) thisSlider.getValue()), notify);
        else valueTF.setText(String(thisSlider.getValue(), sliderTextResolution), notify);
    }
}

void BKSingleSlider::setValue(double newval, int numDecimalPoints, NotificationType notify)
{
    checkValue(newval);
    thisSlider.setValue(newval, notify);
    if (thisSlider.getValue() == sliderMin && !minDisplay.isEmpty())
    {
        valueTF.setText(minDisplay, dontSendNotification);
    }
    else valueTF.setText(String(thisSlider.getValue(), numDecimalPoints), notify);
}


// ********************************************************************************************* //
// ************************  BKRangeSlider ***************************************************** //
// ********************************************************************************************* //

BKRangeSlider::BKRangeSlider (String name, double min, double max, double defmin, double defmax, double increment):
sliderName(name),
sliderMin(min),
sliderMax(max),
sliderDefaultMin(defmin),
sliderDefaultMax(defmax),
sliderIncrement(increment)
{
    
    justifyRight = true;
    
    showName.setText(sliderName, dontSendNotification);
    if(justifyRight) showName.setJustificationType(Justification::bottomRight);
    else showName.setJustificationType(Justification::bottomLeft);
    addAndMakeVisible(showName);
    
    minValueTF.setText(String(sliderDefaultMin));
    minValueTF.setName("minvalue");
    minValueTF.addListener(this);
    minValueTF.setSelectAllWhenFocused(true);
    minValueTF.setColour(TextEditor::highlightColourId, Colours::darkgrey);
    addAndMakeVisible(minValueTF);
    
    maxValueTF.setText(String(sliderDefaultMax));
    maxValueTF.setName("maxvalue");
    maxValueTF.addListener(this);
    maxValueTF.setSelectAllWhenFocused(true);
    maxValueTF.setColour(TextEditor::highlightColourId, Colours::darkgrey);
    addAndMakeVisible(maxValueTF);
    
    minSlider.setSliderStyle(Slider::LinearHorizontal);
    minSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    minSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    minSlider.setValue(sliderDefaultMin, dontSendNotification);
    minSlider.addListener(this);
    minSlider.setLookAndFeel(&minSliderLookAndFeel);
    //minSlider.setInterceptsMouseClicks(false, true);
    //minSliderLookAndFeel.setColour(Slider::trackColourId, Colour::fromRGBA(55, 105, 250, 50));
    addAndMakeVisible(minSlider);
    
    maxSlider.setSliderStyle(Slider::LinearHorizontal);
    maxSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    maxSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    maxSlider.setValue(sliderDefaultMax, dontSendNotification);
    maxSlider.addListener(this);
    maxSlider.setLookAndFeel(&maxSliderLookAndFeel);
    //maxSlider.setInterceptsMouseClicks(false, true);
    //maxSliderLookAndFeel.setColour(Slider::trackColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    //maxSliderLookAndFeel.setColour(Slider::trackColourId, Colour::fromRGBA(55, 105, 250, 50));
    addAndMakeVisible(maxSlider);
    
    invisibleSlider.setSliderStyle(Slider::LinearHorizontal);
    invisibleSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    invisibleSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    invisibleSlider.setValue(sliderDefaultMin, dontSendNotification);
    invisibleSlider.setAlpha(0.0);
    invisibleSlider.addListener(this);
    invisibleSlider.addMouseListener(this, true);
    //invisibleSlider.setInterceptsMouseClicks(true, true);
    addAndMakeVisible(invisibleSlider);
    
    newDrag = false;
    isMinAlwaysLessThanMax = false;
    
    displaySlider = std::make_unique<Slider>();
    displaySlider->setRange(min, max, increment);
    displaySlider->setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    displaySlider->setLookAndFeel(&displaySliderLookAndFeel);
    displaySlider->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(*displaySlider);
    
#if JUCE_IOS
    maxValueTF.setReadOnly(true);
    maxValueTF.addMouseListener(this, true);
    
    minValueTF.setReadOnly(true);
    minValueTF.addMouseListener(this, true);
#endif
}

void BKRangeSlider::setDim(float alphaVal)
{
    minSlider.setAlpha(alphaVal);
    maxSlider.setAlpha(alphaVal);
    showName.setAlpha(alphaVal);
    minValueTF.setAlpha(alphaVal);
    maxValueTF.setAlpha(alphaVal);
}

void BKRangeSlider::setBright()
{
    minSlider.setAlpha(1.);
    maxSlider.setAlpha(1.);
    showName.setAlpha(1.);
    minValueTF.setAlpha(1.);
    maxValueTF.setAlpha(1.);
}


void BKRangeSlider::setMinValue(double newval, NotificationType notify)
{
    checkValue(newval);
    minSlider.setValue(newval, notify);
    minValueTF.setText(String(minSlider.getValue()), dontSendNotification);
    rescaleMinSlider();
   // displaySlider->setRange(sliderMin, sliderMax, sliderIncrement);
}


void BKRangeSlider::setMaxValue(double newval, NotificationType notify)
{
    checkValue(newval);
    maxSlider.setValue(newval, notify);
    maxValueTF.setText(String(maxSlider.getValue()), dontSendNotification);
    rescaleMaxSlider();
    
}


void BKRangeSlider::sliderValueChanged (Slider *slider)
{
    
    if(slider == &invisibleSlider)
    {
        if(newDrag)
        {
            if(!clickedOnMinSlider)
            {
                maxSlider.setValue(invisibleSlider.getValue(), dontSendNotification);
                maxValueTF.setText(String(maxSlider.getValue()), dontSendNotification);
                if(isMinAlwaysLessThanMax)
                    if(maxSlider.getValue() < minSlider.getValue())
                        setMinValue(maxSlider.getValue(), dontSendNotification);
                //displaySlider->setRange(0,invisibleSlider.getValue(), sliderIncrement);
            }
            else
            {
                minSlider.setValue(invisibleSlider.getValue(), dontSendNotification);
                minValueTF.setText(String(minSlider.getValue()), dontSendNotification);
                if(isMinAlwaysLessThanMax)
                    if(minSlider.getValue() > maxSlider.getValue())
                        setMaxValue(minSlider.getValue(), dontSendNotification);
            }
            
            listeners.call(&BKRangeSlider::Listener::BKRangeSliderValueChanged,
                           getName(),
                           minSlider.getValue(),
                           maxSlider.getValue());
        }
    }
}


void BKRangeSlider::mouseDown (const MouseEvent &event)
{
    Component* ec = event.eventComponent;
    
    if (ec == &invisibleSlider)
    {
        if(event.mouseWasClicked())
        {
            if(event.y > invisibleSlider.getHeight() / 2.)
            {
                clickedOnMinSlider = true;
            }
            else
            {
                clickedOnMinSlider = false;
            }
            
            newDrag = true;
        }
        
        unfocusAllComponents();
    }
    
#if JUCE_IOS
    else if (ec == &minValueTF)
    {
        hasBigOne = true;
        WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, &minValueTF, "cluster min");
    }
    else if (ec == &maxValueTF)
    {
        hasBigOne = true;
        WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, &maxValueTF, "cluster max");
    }
#endif
    
}


void BKRangeSlider::sliderDragEnded(Slider *slider)
{
    newDrag = false;
    unfocusAllComponents();
}


void BKRangeSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    double newval = textEditor.getText().getDoubleValue();
    
    //adjusts min/max of sldiers as needed
    checkValue(newval);
    
    if(textEditor.getName() == "minvalue")
    {
        minSlider.setValue(newval, sendNotification);
        if(isMinAlwaysLessThanMax)
            if(minSlider.getValue() > maxSlider.getValue())
                setMaxValue(minSlider.getValue(), dontSendNotification);
        rescaleMinSlider();
    }
    else if(textEditor.getName() == "maxvalue")
    {
        maxSlider.setValue(newval, sendNotification);
        if(isMinAlwaysLessThanMax)
            if(maxSlider.getValue() < minSlider.getValue())
                setMinValue(maxSlider.getValue(), dontSendNotification);
        rescaleMaxSlider();
    }
    
    unfocusAllComponents();
    
    listeners.call(&BKRangeSlider::Listener::BKRangeSliderValueChanged,
                   getName(),
                   minSlider.getValue(),
                   maxSlider.getValue());
}


void BKRangeSlider::textEditorTextChanged(TextEditor& textEditor)
{
    focusLostByEscapeKey = false;
    
#if JUCE_IOS
    if (hasBigOne)
    {
        hasBigOne = false;
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}


void BKRangeSlider::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    focusLostByEscapeKey = true;
    unfocusAllComponents();
}


void BKRangeSlider::textEditorFocusLost(TextEditor& textEditor)
{
#if !JUCE_IOS
    if(!focusLostByEscapeKey)
    {
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}


void BKRangeSlider::checkValue(double newval)
{
    if(newval > maxSlider.getMaximum()) {
        maxSlider.setRange(minSlider.getMinimum(), newval, sliderIncrement);
        minSlider.setRange(minSlider.getMinimum(), newval, sliderIncrement);
        invisibleSlider.setRange(minSlider.getMinimum(), newval, sliderIncrement);
    }
    
    if(newval < minSlider.getMinimum()) {
        maxSlider.setRange(newval, maxSlider.getMaximum(), sliderIncrement);
        minSlider.setRange(newval, maxSlider.getMaximum(), sliderIncrement);
        invisibleSlider.setRange(newval, maxSlider.getMaximum(), sliderIncrement);
    }
}


void BKRangeSlider::rescaleMinSlider()
{
    if(minSlider.getMinimum() < sliderMin &&
       minSlider.getValue() > sliderMin &&
       maxSlider.getValue() > sliderMin)
    {
        maxSlider.setRange(sliderMin, maxSlider.getMaximum(), sliderIncrement);
        minSlider.setRange(sliderMin, maxSlider.getMaximum(), sliderIncrement);
        invisibleSlider.setRange(sliderMin, maxSlider.getMaximum(), sliderIncrement);
        
    }
}

void BKRangeSlider::rescaleMaxSlider()
{
    
    if(maxSlider.getMaximum() > sliderMax &&
       maxSlider.getValue() < sliderMax &&
       minSlider.getValue() < sliderMax
       )
    {
        maxSlider.setRange(minSlider.getMinimum(), sliderMax, sliderIncrement);
        minSlider.setRange(minSlider.getMinimum(), sliderMax, sliderIncrement);
        invisibleSlider.setRange(minSlider.getMinimum(), sliderMax, sliderIncrement);
        
    }
}

void BKRangeSlider::resized()
{
    
    Rectangle<int> area (getLocalBounds());
    Rectangle<int> topSlab (area.removeFromTop(gComponentTextFieldHeight));
    
    if(justifyRight)
    {
        topSlab.removeFromRight(5);
        maxValueTF.setBounds(topSlab.removeFromRight(75));
        topSlab.removeFromRight(gXSpacing);
        minValueTF.setBounds(topSlab.removeFromLeft(75));
        showName.setBounds(topSlab.removeFromRight(getWidth() - 150));
    }
    else
    {
        topSlab.removeFromLeft(5);
        minValueTF.setBounds(topSlab.removeFromLeft(75));
        topSlab.removeFromLeft(gXSpacing);
        maxValueTF.setBounds(topSlab.removeFromLeft(75));
        showName.setBounds(topSlab.removeFromLeft(getWidth() - 150));
    }
    
    Rectangle<int> sliderArea (area.removeFromTop(40));
    minSlider.setBounds(sliderArea);
    maxSlider.setBounds(sliderArea);
    invisibleSlider.setBounds(sliderArea);
    
    Rectangle<int> displaySliderArea = maxSlider.getBounds();
    displaySliderArea.reduce(8, 0);
    displaySlider->setBounds(displaySliderArea.removeFromBottom(8));
    
}

// ******************************************************************************************************************** //
// *******************************************  BKWaveDistanceUndertowSlider ****************************************** //
// ******************************************************************************************************************** //


BKWaveDistanceUndertowSlider::BKWaveDistanceUndertowSlider()
{
    
    maxSliders = 10;
    
    sliderMin = 0;
    sliderMax = 20000;
    sliderIncrement = 1;
    
    float skewFactor = 0.7;
    
    sampleImageComponent.setImage(ImageCache::getFromMemory(BinaryData::samplePic_png, BinaryData::samplePic_pngSize));
    sampleImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    sampleImageComponent.setTooltip("Provides real-time visualization of each independent Nostalgic wave");
    addAndMakeVisible(sampleImageComponent);
    
    wavedistanceSlider = std::make_unique<Slider>();
    wavedistanceSlider->addMouseListener(this, true);
    wavedistanceSlider->setRange(sliderMin, sliderMax, sliderIncrement);
    wavedistanceSlider->setSliderStyle(Slider::SliderStyle::LinearBar);
    wavedistanceSlider->setLookAndFeel(&displaySliderLookAndFeel);
    wavedistanceSlider->setTextBoxIsEditable(false);
    wavedistanceSlider->setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.5));
    displaySliderLookAndFeel.setColour(Slider::thumbColourId, Colours::goldenrod.withMultipliedAlpha(0.5));
    wavedistanceSlider->addListener(this);
    wavedistanceSlider->setSkewFactor(skewFactor);
    addAndMakeVisible(*wavedistanceSlider);
    
    undertowSlider = std::make_unique<Slider>();
    undertowSlider->addMouseListener(this, true);
    undertowSlider->setRange(sliderMin, sliderMax, sliderIncrement);
    undertowSlider->setSliderStyle(Slider::SliderStyle::LinearBar);
    undertowSlider->setLookAndFeel(&displaySliderLookAndFeel);
    undertowSlider->setTextBoxIsEditable(false);
    undertowSlider->setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.5));
    undertowSlider->addListener(this);
    undertowSlider->setSkewFactor(skewFactor);
    addAndMakeVisible(*undertowSlider);
    
    for(int i=0; i<maxSliders; i++)
    {
        displaySliders.insert(0, new Slider());
        Slider* newSlider = displaySliders.getUnchecked(0);
        
        newSlider->setRange(sliderMin, sliderMax, sliderIncrement);
        newSlider->setLookAndFeel(&displaySliderLookAndFeel);
        newSlider->setSliderStyle(BKSubSlider::SliderStyle::LinearBar);
        newSlider->setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.5));
        // displaySliderLookAndFeel.setColour(Slider::thumbColourId, Colours::deepskyblue.withMultipliedAlpha(0.75));
        newSlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        newSlider->setInterceptsMouseClicks(false, false);
        newSlider->setSkewFactor(skewFactor);
        addAndMakeVisible(newSlider);
    }
    
    undertowValueTF.setName("ut");
    undertowValueTF.addListener(this);
    addChildComponent(undertowValueTF);
    
    wavedistanceValueTF.setName("wd");
    wavedistanceValueTF.addListener(this);
    addChildComponent(wavedistanceValueTF);
    
    wavedistanceName.setText("wave distance (ms)", dontSendNotification);
    wavedistanceName.setJustificationType(Justification::topRight);
    addAndMakeVisible(wavedistanceName);
    
    undertowName.setText("undertow (ms)", dontSendNotification);
    undertowName.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(undertowName);
}



void BKWaveDistanceUndertowSlider::setDim(float alphaVal)
{
    wavedistanceName.setAlpha(alphaVal);
    undertowName.setAlpha(alphaVal);
    wavedistanceSlider->setAlpha(alphaVal);
    undertowSlider->setAlpha(alphaVal);
    
    for(int i=0; i<maxSliders; i++)
    {
        Slider* newSlider = displaySliders.getUnchecked(i);
        newSlider->setAlpha(0.);
    }
    
}

void BKWaveDistanceUndertowSlider::setBright()
{
    wavedistanceName.setAlpha(1.);
    undertowName.setAlpha(1.);
    wavedistanceSlider->setAlpha(1.);
    undertowSlider->setAlpha(1.);
}


void BKWaveDistanceUndertowSlider::updateSliderPositions(Array<int> newpositions)
{
    
    if(newpositions.size() > maxSliders) newpositions.resize(maxSliders);
    
    for(int i=0; i<newpositions.size(); i++)
    {
        displaySliders.getUnchecked(i)->setValue(newpositions.getUnchecked(i) - wavedistanceSlider->getValue());
        displaySliders.getUnchecked(i)->setVisible(true);
    }
    
    for(int i=newpositions.size(); i<displaySliders.size(); i++)
    {
        displaySliders.getUnchecked(i)->setVisible(false);
    }
}


void BKWaveDistanceUndertowSlider::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    
    
#if JUCE_IOS
    wavedistanceSlider->setBounds(area.removeFromTop(getHeight() * 0.2));
    undertowSlider->setBounds(area.removeFromBottom(getHeight() * 0.2));
#else
    wavedistanceSlider->setBounds(area.removeFromTop(getHeight() * 0.1));
    undertowSlider->setBounds(area.removeFromBottom(getHeight() * 0.1));
    //wavedistanceSlider->setBounds(area.removeFromTop(gComponentSingleSliderHeight));
    //undertowSlider->setBounds(area.removeFromBottom(gComponentSingleSliderHeight));
#endif
    
    wavedistanceValueTF.setBounds(wavedistanceSlider->getBounds());
    
    //int xpos = wavedistanceSlider->getPositionOfValue(wavedistanceSlider->getValue());
    
    //undertowSlider->setBounds(xpos, undertowSlider->getY(), getWidth() - xpos, undertowSlider->getHeight());
    
    undertowValueTF.setBounds(undertowSlider->getBounds());
    
    //undertowName.setBounds(area);
    //wavedistanceName.setBounds(area);
    undertowName.setBounds(
                           undertowSlider->getRight() - undertowSlider->getWidth() / 4,
                           undertowSlider->getY() - undertowSlider->getHeight(),
                           undertowSlider->getWidth() / 4,
                           undertowSlider->getHeight());
    wavedistanceName.setBounds(
                               wavedistanceSlider->getRight() - wavedistanceSlider->getWidth() / 4,
                               wavedistanceSlider->getBottom(),
                               wavedistanceSlider->getWidth() / 4,
                               wavedistanceSlider->getHeight());
    
    sampleImageComponent.setBounds(area);
    
    for(int i=0; i<maxSliders; i++)
    {
        Slider* newSlider = displaySliders.getUnchecked(i);
        newSlider->setBounds(area);
    }
    
    
}

void BKWaveDistanceUndertowSlider::mouseDoubleClick(const MouseEvent& e)
{
    Component* c = e.eventComponent->getParentComponent();
    
    if (c == wavedistanceSlider.get())
    {
#if JUCE_IOS
        hasBigOne = true;
        WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, &wavedistanceValueTF, "wave distance (ms)");
#else
        wavedistanceValueTF.setVisible(true);
        wavedistanceValueTF.grabKeyboardFocus();
#endif
    }
    else if (c == undertowSlider.get())
    {
#if JUCE_IOS
        hasBigOne = true;
        WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, &undertowValueTF, "undertow (ms)");
#else
        undertowValueTF.setVisible(true);
        undertowValueTF.grabKeyboardFocus();
#endif
    }
    
}

void BKWaveDistanceUndertowSlider::textEditorReturnKeyPressed(TextEditor& editor)
{
    double newval = editor.getText().getDoubleValue();
    
    //DBG("nostalgic wavedistance/undertow slider return key pressed");
    
    if (editor.getName() == "ut")
    {
        undertowSlider->setValue(newval, sendNotification);
    }
    else if (editor.getName() == "wd")
    {
        wavedistanceSlider->setValue(newval, sendNotification);
    }
    
    wavedistanceValueTF.setVisible(false);
    undertowValueTF.setVisible(false);
    
    setWaveDistance(wavedistanceSlider->getValue(), dontSendNotification);
    
    listeners.call(&BKWaveDistanceUndertowSlider::Listener::BKWaveDistanceUndertowSliderValueChanged,
                   "nSlider",
                   wavedistanceSlider->getValue(),
                   undertowSlider->getValue());
    
    unfocusAllComponents();
}

void BKWaveDistanceUndertowSlider::textEditorTextChanged(TextEditor& textEditor)
{
    focusLostByEscapeKey = false;
    
#if JUCE_IOS
    if (hasBigOne)
    {
        hasBigOne = false;
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}

void BKWaveDistanceUndertowSlider::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    //DBG("Nostalgic textEditorEscapeKeyPressed");
    focusLostByEscapeKey = true;
    unfocusAllComponents();
}

void BKWaveDistanceUndertowSlider::textEditorFocusLost(TextEditor& textEditor)
{
#if !JUCE_IOS
    //DBG("Nostalgic textEditorFocusLost");
    if(!focusLostByEscapeKey)
    {
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}

void BKWaveDistanceUndertowSlider::sliderDragEnded(Slider *slider)
{
    if(slider == wavedistanceSlider.get())
    {
        setWaveDistance(wavedistanceSlider->getValue(), dontSendNotification);
    }
    
    listeners.call(&BKWaveDistanceUndertowSlider::Listener::BKWaveDistanceUndertowSliderValueChanged,
                   "nSlider",
                   wavedistanceSlider->getValue(),
                   undertowSlider->getValue());
}

void BKWaveDistanceUndertowSlider::setWaveDistance(int newwavedist, NotificationType notify)
{
    wavedistanceSlider->setValue(newwavedist, notify);
    
    int xpos = wavedistanceSlider->getPositionOfValue(wavedistanceSlider->getValue());
    undertowSlider->setBounds(xpos, undertowSlider->getY(), getWidth() - xpos, undertowSlider->getHeight());
    double max = sliderMax - wavedistanceSlider->getValue();
    if (max <= sliderMin) max = sliderMin + 0.0001;
    undertowSlider->setRange(sliderMin, max, sliderIncrement);
    
    for(int i=0; i<maxSliders; i++)
    {
        Slider* newSlider = displaySliders.getUnchecked(i);
        
        newSlider->setBounds(xpos, newSlider->getY(), getWidth() - xpos, newSlider->getHeight());
        newSlider->setRange(sliderMin, max, sliderIncrement);
    }
    
}

void BKWaveDistanceUndertowSlider::setUndertow(int newundertow, NotificationType notify)
{
    undertowSlider->setValue(newundertow, notify);
}



// ******************************************************************************************************************** //
// ************************************************** BKStackedSlider ************************************************* //
// ******************************************************************************************************************** //


BKStackedSlider::BKStackedSlider(String sliderName, double min, double max, double defmin, double defmax, double def, double increment):
sliderName(sliderName),
sliderMin(min),
sliderMax(max),
sliderMinDefault(defmin),
sliderMaxDefault(defmax),
sliderDefault(def),
sliderIncrement(increment)
{
    
    showName.setText(sliderName, dontSendNotification);
    showName.setInterceptsMouseClicks(false, true);
    addAndMakeVisible(showName);

    editValsTextField = std::make_unique<BKTextEditor>();
    editValsTextField->setMultiLine(true);
    editValsTextField->setName("PARAMTXTEDIT");
    editValsTextField->addListener(this);
    editValsTextField->setColour(TextEditor::highlightColourId, Colours::darkgrey);
#if JUCE_IOS
    editValsTextField->setReadOnly(true);
    editValsTextField->setCaretVisible(true);
#endif
    addAndMakeVisible(*editValsTextField);
    editValsTextField->setVisible(false);
    
    numSliders = 12;
    numActiveSliders = 1;
    
    for(int i=0; i<numSliders; i++)
    {
        dataSliders.add(new Slider);
        Slider* newSlider = dataSliders.getLast();
        
        newSlider->setSliderStyle(Slider::LinearBar);
        newSlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        newSlider->setRange(sliderMin, sliderMax, sliderIncrement);
        newSlider->setValue(sliderDefault, dontSendNotification);
        newSlider->setLookAndFeel(&stackedSliderLookAndFeel);
        //newSlider->addListener(this);
        addAndMakeVisible(newSlider);
        if(i>0) {
            newSlider->setVisible(false);
            activeSliders.insert(i, false);
        }
        else activeSliders.insert(0, true);
    }
    
    topSlider = std::make_unique<Slider>();
    topSlider->setSliderStyle(Slider::LinearBar);
    topSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0,0);
    //topSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 50,50);
    topSlider->setRange(sliderMin, sliderMax, sliderIncrement);
    topSlider->setValue(sliderDefault, dontSendNotification);
    topSlider->addListener(this);
    topSlider->addMouseListener(this, true);
    topSlider->setLookAndFeel(&topSliderLookAndFeel);
    topSlider->setAlpha(0.);
    addAndMakeVisible(*topSlider);
    
    topSliderLookAndFeel.setColour(Slider::thumbColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.0f));
    stackedSliderLookAndFeel.setColour(Slider::thumbColourId, Colours::goldenrod.withMultipliedAlpha(0.95));
    
}

void BKStackedSlider::setDim(float alphaVal)
{
    showName.setAlpha(alphaVal);
    //topSlider->setAlpha(alphaVal);
    
    for(int i=0; i<numSliders; i++)
    {
        Slider* newSlider = dataSliders.operator[](i);
        if(newSlider != nullptr)
        {
            if(activeSliders.getUnchecked(i))
            {
                newSlider->setAlpha(alphaVal);
            }
        }
    }
}

void BKStackedSlider::setBright()
{
    showName.setAlpha(1.);
    //topSlider->setAlpha(1.);
    
    for(int i=0; i<numSliders; i++)
    {
        Slider* newSlider = dataSliders.operator[](i);
        if(newSlider != nullptr)
        {
            if(activeSliders.getUnchecked(i))
            {
                newSlider->setAlpha(1.);
            }
        }
    }
}

void BKStackedSlider::sliderValueChanged (Slider *slider)
{
    
}

void BKStackedSlider::addSlider(NotificationType newnotify)
{
    Array<float> sliderVals = getAllActiveValues();
    //sliderVals.add(sliderDefault);
    sliderVals.add(topSlider->proportionOfLengthToValue((double)clickedPosition / getWidth()));
    setTo(sliderVals, newnotify);
    topSlider->setValue(topSlider->proportionOfLengthToValue((double)clickedPosition / getWidth()), dontSendNotification);
    
    listeners.call(&BKStackedSlider::Listener::BKStackedSliderValueChanged,
                   getName(),
                   getAllActiveValues());
}

void BKStackedSlider::setTo(Array<float> newvals, NotificationType newnotify)
{
    
    int slidersToActivate = newvals.size();
    if(slidersToActivate > numSliders) slidersToActivate = numSliders;
    if(slidersToActivate <= 0)
    {
        slidersToActivate = 1;
        newvals.add(sliderDefault);
    }
    
    //activate sliders
    for(int i=0; i<slidersToActivate; i++)
    {
        
        Slider* newSlider = dataSliders.operator[](i);
        if(newSlider != nullptr)
        {
            if(newvals.getUnchecked(i) > sliderMax)
                newSlider->setRange(sliderMin, newvals.getUnchecked(i), sliderIncrement);
            
            if(newvals.getUnchecked(i) < sliderMin)
                newSlider->setRange(newvals.getUnchecked(i), sliderMax, sliderIncrement);
            
            newSlider->setValue(newvals.getUnchecked(i));
            newSlider->setVisible(true);
        }
        
        activeSliders.set(i, true);
    }
    
    //deactivate unused sliders
    for(int i=slidersToActivate; i<numSliders; i++)
    {
        
        Slider* newSlider = dataSliders.operator[](i);
        if(newSlider != nullptr)
        {
            newSlider->setValue(sliderDefault);
            newSlider->setVisible(false);
        }
        
        activeSliders.set(i, false);
    }
    
    //make sure there is one!
    if(slidersToActivate <= 0)
    {
        dataSliders.getFirst()->setValue(sliderDefault);
        activeSliders.set(0, true);
    }
    
    resetRanges();
    
    topSlider->setValue(dataSliders.getFirst()->getValue(), dontSendNotification);
}


void BKStackedSlider::mouseDown (const MouseEvent &event)
{
    if(event.mouseWasClicked())
    {
        clickedSlider = whichSlider();
        clickedPosition = event.x;
        
        mouseJustDown = true;
        
        if(event.mods.isCtrlDown())
        {
            showModifyPopupMenu();
        }
    }
}


void BKStackedSlider::mouseDrag(const MouseEvent& e)
{
    if(!mouseJustDown)
    {
        Slider* currentSlider = dataSliders.operator[](clickedSlider);
        if(currentSlider != nullptr)
        {
            if(e.mods.isShiftDown())
            {
                currentSlider->setValue(round(topSlider->getValue()));
                topSlider->setValue(round(topSlider->getValue()));
            }
            else {
                currentSlider->setValue(topSlider->getValue(), sendNotification);
            }
        }
    }
    else mouseJustDown = false;
    
}

void BKStackedSlider::mouseUp(const MouseEvent& e)
{
    DBG("BKStackedSlider::mouseUp");

    listeners.call(&BKStackedSlider::Listener::BKStackedSliderValueChanged,
                   getName(),
                   getAllActiveValues());
    
}

void BKStackedSlider::mouseMove(const MouseEvent& e)
{
    //topSlider->setValue(topSlider->proportionOfLengthToValue((double)e.x / getWidth()), dontSendNotification);
    topSlider->setValue(dataSliders.getUnchecked(whichSlider(e))->getValue());
    
    for(int i=0; i<dataSliders.size(); i++)
    {
        if(dataSliders.getUnchecked(whichSlider(e)) == dataSliders.getUnchecked(i))
            dataSliders.getUnchecked(i)->setTextBoxStyle(Slider::TextBoxLeft, false, 50, 50);
        else
            dataSliders.getUnchecked(i)->setTextBoxStyle(Slider::NoTextBox, false, 50, 50);
    }
}


void BKStackedSlider::mouseDoubleClick (const MouseEvent &e)
{
    //highlight number for current slider
    
#if JUCE_IOS
    hasBigOne = true;
    editValsTextField->setText(floatArrayToString(getAllActiveValues()), dontSendNotification);
    WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, editValsTextField.get(), sliderName);
#else
    StringArray tokens;
    tokens.addTokens(floatArrayToString(getAllActiveValues()), false); //arrayFloatArrayToString
    int startPoint = 0;
    int endPoint;
    
    int which = whichSlider();
    
    for(int i=0; i < which; i++) {
        startPoint += tokens[i].length() + 1;
    }
    endPoint = startPoint + tokens[which].length();
    
    editValsTextField->setVisible(true);
    editValsTextField->toFront(true);
    editValsTextField->grabKeyboardFocus();
    editValsTextField->setText(floatArrayToString(getAllActiveValues()), dontSendNotification); //arrayFloatArrayToString
    
    Range<int> highlightRange(startPoint, endPoint);
    editValsTextField->setHighlightedRegion(highlightRange);
    
    focusLostByEscapeKey = false;
#endif
    
}

void BKStackedSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == editValsTextField->getName())
    {
        editValsTextField->setVisible(false);
        editValsTextField->toBack();
        
        setTo(stringToFloatArray(textEditor.getText()), sendNotification);
        clickedSlider = 0;
        resized();
        
        listeners.call(&BKStackedSlider::Listener::BKStackedSliderValueChanged,
                       getName(),
                       getAllActiveValues());
        
    }
}

void BKStackedSlider::textEditorFocusLost(TextEditor& textEditor)
{
#if !JUCE_IOS
    if(!focusLostByEscapeKey) {
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}

void BKStackedSlider::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    if(textEditor.getName() == editValsTextField->getName())
    {
        focusLostByEscapeKey = true;
        editValsTextField->setVisible(false);
        editValsTextField->toBack();
        unfocusAllComponents();
    }
}

void BKStackedSlider::textEditorTextChanged(TextEditor& textEditor)
{
#if JUCE_IOS
    if (hasBigOne)
    {
        hasBigOne = false;
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}

void BKStackedSlider::resetRanges()
{
    
    double sliderMinTemp = sliderMinDefault;
    double sliderMaxTemp = sliderMaxDefault;
    
    for(int i = 0; i<dataSliders.size(); i++)
    {
        Slider* currentSlider = dataSliders.operator[](i);
        if(currentSlider != nullptr)
        {
            if(currentSlider->getValue() > sliderMaxTemp) sliderMaxTemp = currentSlider->getValue();
            if(currentSlider->getValue() < sliderMinTemp) sliderMinTemp = currentSlider->getValue();
        }
    }
    
    if( (sliderMax != sliderMaxTemp) || sliderMin != sliderMinTemp)
    {
        sliderMax = sliderMaxTemp;
        sliderMin = sliderMinTemp;
        
        for(int i = 0; i<dataSliders.size(); i++)
        {
            Slider* currentSlider = dataSliders.operator[](i);
            if(currentSlider != nullptr)
            {
                currentSlider->setRange(sliderMin, sliderMax, sliderIncrement);
            }
        }
        
        topSlider->setRange(sliderMin, sliderMax, sliderIncrement);
    }
}


Array<float> BKStackedSlider::getAllActiveValues()
{
    Array<float> currentVals;
    
    for(int i=0; i<dataSliders.size(); i++)
    {
        Slider* currentSlider = dataSliders.operator[](i);
        if(currentSlider != nullptr)
        {
            if(activeSliders.getUnchecked(i))
                currentVals.add(currentSlider->getValue());
        }
        
    }
    
    return currentVals;
}

int BKStackedSlider::whichSlider()
{
    float refDistance;
    int whichSub = 0;
    
    Slider* refSlider = dataSliders.getFirst();
    refDistance = fabs(refSlider->getValue() - topSlider->getValue());
    
    for(int i=1; i<dataSliders.size(); i++)
    {
        if(activeSliders.getUnchecked(i))
        {
            Slider* currentSlider = dataSliders.operator[](i);
            if(currentSlider != nullptr) {
                float tempDistance = fabs(currentSlider->getValue() - topSlider->getValue());
                if(tempDistance < refDistance)
                {
                    whichSub = i;
                    refDistance = tempDistance;
                }
            }
        }
    }
    //DBG("whichSlider = " + String(whichSub));
    
    return whichSub;
}

int BKStackedSlider::whichSlider(const MouseEvent& e)
{
    float refDistance;
    float topSliderVal = topSlider->proportionOfLengthToValue((double)e.x / getWidth());
    
    int whichSub = 0;
    
    Slider* refSlider = dataSliders.getFirst();
    refDistance = fabs(refSlider->getValue() - topSliderVal);
    
    for(int i=1; i<dataSliders.size(); i++)
    {
        if(activeSliders.getUnchecked(i))
        {
            Slider* currentSlider = dataSliders.operator[](i);
            if(currentSlider != nullptr) {
                float tempDistance = fabs(currentSlider->getValue() - topSliderVal);
                if(tempDistance < refDistance)
                {
                    whichSub = i;
                    refDistance = tempDistance;
                }
            }
        }
    }
    //DBG("whichSlider = " + String(whichSub));
    
    return whichSub;
}

void BKStackedSlider::showModifyPopupMenu()
{
    BKPopupMenu m;
    m.addItem (1, translate ("add transposition"), true, false);
    m.addSeparator();
    
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::forComponent (sliderModifyMenuCallback, this));
}

void BKStackedSlider::sliderModifyMenuCallback (const int result, BKStackedSlider* ss)
{
    if (ss == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    switch (result)
    {
        case 1:   ss->addSlider(sendNotification); break;
            
        default:  break;
    }
}


void BKStackedSlider::resized ()
{
    Rectangle<int> area (getLocalBounds());
    
    showName.setBounds(area.toNearestInt());
    showName.setJustificationType(Justification::topRight);
    showName.toFront(false);
    
    topSlider->setBounds(area);
    
    editValsTextField->setBounds(area);
    editValsTextField->setVisible(false);
    
    for(int i=0; i<numSliders; i++)
    {
        Slider* newSlider = dataSliders.getUnchecked(i);
        newSlider->setBounds(area);
    }
    
}


// ******************************************************************************************************************** //
// **************************************************  BKADSRSlider ************************************************** //
// ******************************************************************************************************************** //

BKADSRSlider::BKADSRSlider (String name):
sliderName(name)
{
    
    justifyRight = true;
    
    showName.setText(sliderName, dontSendNotification);
    if(justifyRight) showName.setJustificationType(Justification::bottomRight);
    else showName.setJustificationType(Justification::bottomLeft);
    //addAndMakeVisible(showName);
    
    attackSlider = std::make_unique<BKSingleSlider>("attack time (ms)", "attack time (ms)", 1, 1000, 10, 1);
    attackSlider->setSkewFactorFromMidPoint(200);
    attackSlider->setJustifyRight(true);
    attackSlider->addMyListener(this);
    attackSlider->setToolTipString("envelope attack time (ms)");
    addAndMakeVisible(*attackSlider);
    
    decaySlider = std::make_unique<BKSingleSlider>("decay time (ms)", "decay time (ms)", 1, 1000, 10, 1);
    decaySlider->setSkewFactorFromMidPoint(200);
    decaySlider->setJustifyRight(false);
    decaySlider->addMyListener(this);
    decaySlider->setToolTipString("envelope decay time (ms)");
    addAndMakeVisible(*decaySlider);
    
    sustainSlider = std::make_unique<BKSingleSlider>("sustain level (0-1)", "sustain level (0-1)", 0., 1., 1., 0.001);
    sustainSlider->setJustifyRight(true);
    sustainSlider->addMyListener(this);
    sustainSlider->setToolTipString("envelope sustain level (0-1)");
    addAndMakeVisible(*sustainSlider);
    
    releaseSlider = std::make_unique<BKSingleSlider>("release time (ms)", "release time (ms)", 1, 1000, 30, 1);
    releaseSlider->setSkewFactorFromMidPoint(200);
    releaseSlider->setJustifyRight(false);
    releaseSlider->addMyListener(this);
    releaseSlider->setToolTipString("envelope release time (ms)");
    addAndMakeVisible(*releaseSlider);
    
    adsrButton.setButtonText("ADSR");
    adsrButton.setClickingTogglesState(true);
    adsrButton.addListener(this);
    adsrButton.addMouseListener(this, false);
    addAndMakeVisible(adsrButton);
        
    passiveADSRLookAndFeel.setColour(TextButton::buttonColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    highlightedADSRLookAndFeel.setColour(TextButton::buttonColourId, Colours::red.withSaturation(1.));
    activeADSRLookAndFeel.setColour(TextButton::buttonColourId, Colours::goldenrod.withMultipliedAlpha(0.75));
    
    isButtonOnly = true;
    setButtonToggle(true);
    buttonMode = true;

}

void BKADSRSlider::setDim(float alphaVal)
{
    attackSlider->setAlpha(alphaVal);
    decaySlider->setAlpha(alphaVal);
    sustainSlider->setAlpha(alphaVal);
    releaseSlider->setAlpha(alphaVal);
    adsrButton.setAlpha(alphaVal);
    showName.setAlpha(alphaVal);
}

void BKADSRSlider::setBright()
{
    attackSlider->setAlpha(1.);
    decaySlider->setAlpha(1.);
    sustainSlider->setAlpha(1.);
    releaseSlider->setAlpha(1.);
    adsrButton.setAlpha(1.);
    showName.setAlpha(1.);
}


void BKADSRSlider::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    float sustainVal = sustainSlider->getValue();
    if (sustainVal > 1.) { sustainVal = 1.; sustainSlider->setValue(1., dontSendNotification); }
    if (sustainVal < 0.) { sustainVal = 0.; ; sustainSlider->setValue(0., dontSendNotification); }
    
    /*
    if(name == attackSlider->getName())
    {
        //DBG("attackSlider " + String(val));
    }
    else if(name == decaySlider->getName())
    {
        //DBG("decaySlider " + String(val));
    }
    else if(name == sustainSlider->getName())
    {
        //DBG("sustainSlider " + String(val));
    }
    else if(name == releaseSlider->getName())
    {
        //DBG("releaseSlider " + String(val));
    }
     */
            
    listeners.call(&BKADSRSlider::Listener::BKADSRSliderValueChanged,
                   getName(),
                   attackSlider->getValue(),
                   decaySlider->getValue(),
                   sustainVal,
                   releaseSlider->getValue());
}

void BKADSRSlider::buttonStateChanged (Button*)
{

    
}

void BKADSRSlider::mouseUp (const MouseEvent &event)
{
    if(event.mods.isShiftDown() || event.getLengthOfMousePress() > 500)
    {
        //DBG("shift is down");
        bool toggleState = adsrButton.getToggleState();
        DBG("BKADSRSlider::mouseUp = " + getName() + " " + String((int)toggleState));
        listeners.call(&BKADSRSlider::Listener::BKADSRButtonStateChanged, getName(), true, !toggleState);
    }
    else
    {
        //DBG("no shift down");
        if(isButtonOnly) isButtonOnly = false;
        else isButtonOnly = true;
        resized();
        listeners.call(&BKADSRSlider::Listener::BKADSRButtonStateChanged, getName(), false, isButtonOnly);
    }
}

void BKADSRSlider::buttonClicked (Button* b)
{
    //DBG("BKADSRSlider::buttonClicked");
    /*
    if(b->getName() == adsrButton.getName())
    {
        if(isButtonOnly) isButtonOnly = false;
        else isButtonOnly = true;
        resized();
        listeners.call(&BKADSRSlider::Listener::BKADSRButtonStateChanged, getName(), isButtonOnly);
    }
     */
}


void BKADSRSlider::resized()
{

    if(buttonMode)
    {
        Rectangle<int> area (getLocalBounds());
        
        if(!isButtonOnly)
        {
            area.removeFromTop(gComponentComboBoxHeight);
            Rectangle<int> bottomSlice = area.removeFromBottom(gComponentComboBoxHeight);
            
            int midSpace = (bottomSlice.getWidth() - gComponentLabelWidth) * 0.5;
            bottomSlice.removeFromLeft(midSpace);
            bottomSlice.removeFromRight(midSpace);
            adsrButton.setBounds(bottomSlice);
            
            area.removeFromTop(gYSpacing);
            
            Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
            
            attackSlider->setBounds(leftColumn.removeFromTop(gComponentSingleSliderHeight));
            decaySlider->setBounds(area.removeFromTop(gComponentSingleSliderHeight));
            leftColumn.removeFromTop(gYSpacing * 2);
            area.removeFromTop(gYSpacing * 2);
            sustainSlider->setBounds(leftColumn.removeFromTop(gComponentSingleSliderHeight));
            releaseSlider->setBounds(area.removeFromTop(gComponentSingleSliderHeight));
        }
        else
        {
            adsrButton.setBounds(area);
        }
    }
    else
    {
        Rectangle<int> area (getLocalBounds());
        
        area.removeFromTop(gYSpacing);
        
        Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
        
        attackSlider->setBounds(leftColumn.removeFromTop(gComponentSingleSliderHeight));
        decaySlider->setBounds(area.removeFromTop(gComponentSingleSliderHeight));
        leftColumn.removeFromTop(gYSpacing * 2);
        area.removeFromTop(gYSpacing * 2);
        sustainSlider->setBounds(leftColumn.removeFromTop(gComponentSingleSliderHeight));
        releaseSlider->setBounds(area.removeFromTop(gComponentSingleSliderHeight));
    }
}



