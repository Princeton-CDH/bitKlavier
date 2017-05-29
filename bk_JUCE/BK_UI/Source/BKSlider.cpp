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
    setSkewFactorFromMidPoint(sliderDefault);
    setValue(sliderDefault, dontSendNotification);
}



double BKSubSlider::getValueFromText	(const String & text )
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

BKMultiSlider::BKMultiSlider(BKMultiSliderType which)
{
    
    if(which == VerticalMultiSlider || which == VerticalMultiBarSlider) sliderIsVertical = true;
    else sliderIsVertical = false;
    
    if(which == VerticalMultiBarSlider || which == HorizontalMultiBarSlider) sliderIsBar = true;
    else sliderIsBar = false;
    
    if(which == HorizontalMultiSlider || which == HorizontalMultiBarSlider) arrangedHorizontally = true;
    else arrangedHorizontally = false;
    
    passiveSliderLookAndFeel.setColour(Slider::thumbColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    highlightedSliderLookAndFeel.setColour(Slider::thumbColourId, Colour::fromRGBA(250, 10, 50, 50));
    lastHighlightedSlider = 0;
    
    sliderMin = sliderMinDefault = -1.;
    sliderMax = sliderMaxDefault = 1.;
    sliderIncrement = 0.01;
    sliderDefault = 0.;
    
    numActiveSliders = 1;
    numDefaultSliders = 12;
    numVisibleSliders = 12;
    
    if(sliderIsVertical) {
        sliderWidth = 80;
        sliderHeight = 20;
    }
    else
    {
        sliderWidth = 20;
        sliderHeight = 60;
    }
    
    displaySliderWidth = 50;
    
    //subslider is oriented perpendicular to multislider orientation
    if(sliderIsVertical) {
        if(sliderIsBar) subsliderStyle = Slider::LinearBar;
        else subsliderStyle = Slider::LinearHorizontal;
    }
    else
    {
        if(sliderIsBar) subsliderStyle = Slider::LinearBarVertical;
        else subsliderStyle = Slider::LinearVertical;
    }
    
    //create the default sliders, with one active
    for(int i = 0; i<numDefaultSliders; i++)
    {
        if(i==0) addSlider(-1, true);
        else addSlider(-1, false);
    }
    
    if(arrangedHorizontally)
    {
        int tempheight = sliderHeight;

        bigInvisibleSlider = new BKSubSlider(Slider::LinearBarVertical,
                                                sliderMin,
                                                sliderMax,
                                                sliderDefault,
                                                sliderIncrement,
                                                numActiveSliders * 20,
                                                tempheight);
        
        displaySlider = new BKSubSlider(Slider::LinearBarVertical,
                                           sliderMin,
                                           sliderMax,
                                           sliderDefault,
                                           sliderIncrement,
                                           displaySliderWidth,
                                           tempheight);
        
        //DBG("done with horizontal setup");
    }
    else
    {
        int tempwidth;
 
        bigInvisibleSlider = new BKSubSlider(Slider::LinearBar,
                                                sliderMin,
                                                sliderMax,
                                                sliderDefault,
                                                sliderIncrement,
                                                tempwidth,
                                                numActiveSliders * sliderHeight);
    }
    
    bigInvisibleSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0,0);
    bigInvisibleSlider->setAlpha(0.0);
    bigInvisibleSlider->addMouseListener(this, true);
    bigInvisibleSlider->setName("BIG");
    bigInvisibleSlider->addListener(this);
    bigInvisibleSlider->setLookAndFeel(&activeSliderLookAndFeel);
    addAndMakeVisible(bigInvisibleSlider);
    
    displaySlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0,0);
    displaySlider->addMouseListener(this, true);
    displaySlider->setName("DISPLAY");
    displaySlider->addListener(this);
    displaySlider->setInterceptsMouseClicks(false, false);
    displaySlider->setLookAndFeel(&activeSliderLookAndFeel);
    addAndMakeVisible(displaySlider);
    
    showName.setInterceptsMouseClicks(false, true);
    addAndMakeVisible(showName);
    
    editValsTextField = new TextEditor();
    editValsTextField->setMultiLine(true);
    editValsTextField->setName("PARAMTXTEDIT");
    editValsTextField->addListener(this);
    addAndMakeVisible(editValsTextField);
    
    //DBG("done with constructor");
    
}

BKMultiSlider::~BKMultiSlider()
{
    
}


void BKMultiSlider::setTo(Array<float> newvals, NotificationType newnotify)
{
    numActiveSliders = newvals.size();
    if(numActiveSliders < 1) numActiveSliders = 1;
    
    if(numActiveSliders <= numDefaultSliders) numVisibleSliders = numDefaultSliders;
    else numVisibleSliders = numActiveSliders;
    
    deactivateAll(newnotify);
    
    for(int i=0; i<numVisibleSliders; i++)
    {
        if(i >= sliders.size()) addSlider(-1, true);

        BKSubSlider* refSlider = sliders[i]->operator[](0);
        if(refSlider != nullptr)
        {
            if(i<numActiveSliders)
            {
                if(refSlider->getMaximum() < newvals[i]) refSlider->setRange(sliderMin, newvals[i], sliderIncrement);
                if(refSlider->getMinimum() > newvals[i]) refSlider->setRange(newvals[i], sliderMax, sliderIncrement);
                
                refSlider->setValue(newvals[i], newnotify);
                refSlider->isActive(true);
                refSlider->setLookAndFeel(&activeSliderLookAndFeel);
            }
            else
            {
                refSlider->setValue(sliderDefault, newnotify);
                refSlider->isActive(false);
                refSlider->setLookAndFeel(&passiveSliderLookAndFeel);
            }
        }
    }

    cleanupSliderArray();
    resetRanges();
    resized();
    displaySlider->setValue(sliders[0]->operator[](0)->getValue());
}


void BKMultiSlider::setTo(Array<Array<float>> newvals, NotificationType newnotify)
{
    numActiveSliders = newvals.size();
    if(numActiveSliders < 1) numActiveSliders = 1;
    
    if(numActiveSliders <= numDefaultSliders) numVisibleSliders = numDefaultSliders;
    else numVisibleSliders = numActiveSliders;
    
    deactivateAll(newnotify);

    for(int i=0; i<numVisibleSliders; i++)
    {
        if(i >= sliders.size()) addSlider(-1, false);
        
        for(int j=0; j<newvals[i].size(); j++)
        {
            
            if(j >= sliders[i]->size()) addSubSlider(i, false);

            BKSubSlider* refSlider = sliders[i]->operator[](j);
            if(refSlider != nullptr)
            {
                if(i<numActiveSliders)
                {
                    if(refSlider->getMaximum() < newvals[i][j]) refSlider->setRange(sliderMin, newvals[i][j], sliderIncrement);
                    if(refSlider->getMinimum() > newvals[i][j]) refSlider->setRange(newvals[i][j], sliderMax, sliderIncrement);
                    
                    refSlider->setValue(newvals[i][j], newnotify);
                    refSlider->isActive(true);
                    refSlider->setLookAndFeel(&activeSliderLookAndFeel);
                }
                else
                {
                    refSlider->setValue(sliderDefault, newnotify);
                    refSlider->isActive(false);
                    refSlider->setLookAndFeel(&passiveSliderLookAndFeel);
                }
            }
        }
    }
    
    cleanupSliderArray();
    resetRanges();
    resized();
    displaySlider->setValue(sliders[0]->operator[](0)->getValue());
}



void BKMultiSlider::cleanupSliderArray()
{
    //sliders.getLock().enter();
    //doc says "...or preferably use an object of ScopedLockType as an RAII lock for it"
    //not sure if we should do that or not.
    //not sure if we have to do any locking at all, because it looks like OwnedArray does so for remove
    
    for(int i=sliders.size() - 1; i>=0; i--)
    {
        //remove sliders above numVisibleSliders
        if(i >= numVisibleSliders)
        {
            for(int j=sliders[i]->size() - 1; j>=0; j--)
            {
                BKSubSlider* refSlider = sliders[i]->operator[](j);
                if(refSlider != nullptr)
                {
                    removeChildComponent(refSlider);
                    sliders[i]->remove(j, true);
                }
            }
        }
        
        //remove inactive subsliders; will only do anything if the parent slider still exists
        for(int j=sliders[i]->size() - 1; j>=1; j--) //only remove subsliders....
        {
            BKSubSlider* refSlider = sliders[i]->operator[](j);
            if(refSlider != nullptr)
            {
                if(!refSlider->isActive())
                {
                    removeChildComponent(refSlider);
                }
            }

        }
    }
    
    //sliders.getLock().exit();
}


void BKMultiSlider::setMinMaxDefaultInc(std::vector<float> newvals)
{
    sliderMin = sliderMinDefault = newvals[0];
    sliderMax = sliderMaxDefault = newvals[1];
    sliderDefault = newvals[2];
    sliderIncrement = newvals[3];
    
    for(int i=0; i<sliders.size(); i++)
    {
        for(int j = 0; j<sliders[i]->size(); j++)
        {
            BKSubSlider* refSlider = sliders[i]->operator[](j);
            if(refSlider != nullptr)
            {
                refSlider->setMinMaxDefaultInc(newvals);
            }
        }
    }
    
    displaySlider->setMinMaxDefaultInc(newvals);
    bigInvisibleSlider->setMinMaxDefaultInc(newvals);
}


void BKMultiSlider::addSlider(int where, bool active)
{
    BKSubSlider* newslider;
    
    if(arrangedHorizontally) newslider     = new BKSubSlider(subsliderStyle,
                                                             sliderMin,
                                                             sliderMax,
                                                             sliderDefault,
                                                             sliderIncrement,
                                                             sliderWidth,
                                                             sliderHeight);
    else newslider                         = new BKSubSlider(subsliderStyle,
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
    
    newslider->isActive(active);
    
    listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                   getName(),
                   getAllActiveValues());
    
}


void BKMultiSlider::addSubSlider(int where, bool active)
{
    BKSubSlider* newslider;
    
    if(arrangedHorizontally) newslider     = new BKSubSlider(subsliderStyle,
                                                             sliderMin,
                                                             sliderMax,
                                                             sliderDefault,
                                                             sliderIncrement,
                                                             sliderWidth,
                                                             sliderHeight);
    else newslider                         = new BKSubSlider(subsliderStyle,
                                                             sliderMin,
                                                             sliderMax,
                                                             sliderDefault,
                                                             sliderIncrement,
                                                             sliderWidth,
                                                             sliderHeight);
    
    newslider->setRange(sliderMin, sliderMax, sliderIncrement);
    newslider->setValue(sliderDefault, dontSendNotification);
    newslider->addListener(this);

    OwnedArray<BKSubSlider> *newsliderArray = sliders[where];
    newsliderArray->add(newslider);
    sliders.set(where, newsliderArray);
    
    addAndMakeVisible(newslider);
    
    if(active)
        newslider->setLookAndFeel(&activeSliderLookAndFeel);
    else
        newslider->setLookAndFeel(&passiveSliderLookAndFeel);
    
    newslider->isActive(active);
    
    listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                   getName(),
                   getAllActiveValues());
}



void BKMultiSlider::deactivateSlider(int where, NotificationType notify)
{
    if(sliders.size() > 1) {
        
        //leave one active slider for first slider
        int startSlider;
        if(where == 0) startSlider = 1;
        else startSlider = 0;
        
        for(int i = sliders[where]->size() - 1; i >= startSlider; i--) {
            
            BKSubSlider* currentSlider = sliders[where]->operator[](i);
            if(currentSlider != nullptr) {
                currentSlider->isActive(false);
                currentSlider->setLookAndFeel(&passiveSliderLookAndFeel);
                if(i != 0) sliders[where]->remove(i); //remove all but first slider
            }
        }
        
        lastHighlightedSlider = 0;
        cleanupSliderArray();

        if(notify) {
            listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                           getName(),
                           getAllActiveValues());
        }
    }
}

void BKMultiSlider::deactivateAll(NotificationType notify)
{
    for(int i=0; i<sliders.size(); i++ )
    {
        deactivateSlider(i, notify);
    }
}


void BKMultiSlider::deactivateAllAfter(int where, NotificationType notify)
{
    for(int i=where+1; i<sliders.size(); i++ )
    {
        deactivateSlider(i, notify);
    }
}

void BKMultiSlider::deactivateAllBefore(int where, NotificationType notify)
{
    if (where > sliders.size()) where = sliders.size();
    for(int i=0; i<where; i++ )
    {
        deactivateSlider(i, notify);
    }
}

void BKMultiSlider::mouseDrag(const MouseEvent& e)
{
    if(e.eventComponent == bigInvisibleSlider)
    {
        int which = whichSlider(e);
        
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

                
                if(!currentSlider->isActive()){
                    currentSlider->isActive(true);
                    currentSlider->setLookAndFeel(&activeSliderLookAndFeel);
                    listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                                   getName(),
                                   getAllActiveValues());
                }
                else
                {
                    listeners.call(&BKMultiSliderListener::multiSliderValueChanged,
                                   getName(),
                                   whichActiveSlider(which),
                                   getOneSliderBank(which));
                }
            }
        }
    }
}

void BKMultiSlider::mouseMove(const MouseEvent& e)
{
    if(e.eventComponent == bigInvisibleSlider)
    {
        int which = whichSlider(e);
        int whichSub = whichSubSlider(which, e);
        
        if(which >= 0 && whichSub >= 0)
        {
            BKSubSlider* currentSlider = sliders[which]->operator[](whichSub);
            if (currentSlider != nullptr)
            {
                if(currentSlider->isActive())
                    displaySlider->setValue(currentSlider->getValue());
            }
        }
    }
}


void BKMultiSlider::mouseDoubleClick (const MouseEvent &e)
{
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
    tokens.addTokens(arrayFloatArrayToString(getAllActiveValues()), false); //arrayFloatArrayToString
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
    editValsTextField->setText(arrayFloatArrayToString(getAllActiveValues())); //arrayFloatArrayToString
    
    Range<int> highlightRange(startPoint, endPoint);
    editValsTextField->setHighlightedRegion(highlightRange);
}


void BKMultiSlider::mouseDown (const MouseEvent &event)
{
    if(event.mouseWasClicked())
    {
        currentSubSlider = whichSubSlider(whichSlider(event));
        
        if(event.mods.isCtrlDown())
        {
              showModifyPopupMenu(whichSlider(event));
        }
    }
}


void BKMultiSlider::mouseUp (const MouseEvent &event)
{
    if(event.mouseWasClicked())
    {
        if(event.mods.isShiftDown())
        {
            int which = whichSlider(event);
            if(which >= 0) {
                
                BKSubSlider* currentSlider = sliders[which]->operator[](0);
                if (currentSlider != nullptr)
                {
                    currentSlider->setValue(sliderDefault); //again, need to identify which subslider to get
                }

                displaySlider->setValue(sliderDefault);
                
                listeners.call(&BKMultiSliderListener::multiSliderValueChanged,
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
    int y = e.y;

    BKSubSlider* refSlider = sliders[0]->operator[](0);
    if (refSlider != nullptr)
    {
        int which;
        if(arrangedHorizontally) which = (x / refSlider->getWidth());
        else which = (y / refSlider->getHeight());
        
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
    
    if(arrangedHorizontally) {
        for(int i=0; i<sliders[which]->size(); i++)
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
    
    if(arrangedHorizontally) {
        for(int i=0; i<sliders[which]->size(); i++)
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
    }
    
    return whichSub;
}

int BKMultiSlider::whichActiveSlider (int which)
{
    int counter = 0;
    if(which > sliders.size()) which = sliders.size();
    
    for(int i=0; i<which; i++)
    {
        BKSubSlider* currentSlider = sliders[i]->operator[](0);
        if(currentSlider != nullptr)
            if(currentSlider->isActive()) counter++;
    }
    
    
    return counter;
}


void BKMultiSlider::resetRanges()
{

    double sliderMinTemp = sliderMinDefault;
    double sliderMaxTemp = sliderMaxDefault;
    
    for(int i = 0; i<sliders.size(); i++)
    {
        for(int j = 0; j<sliders[i]->size(); j++)
        {
            BKSubSlider* currentSlider = sliders[i]->operator[](j);
            if(currentSlider != nullptr)
            {
                if(currentSlider->getValue() > sliderMaxTemp) sliderMaxTemp = currentSlider->getValue();
                if(currentSlider->getValue() < sliderMinTemp) sliderMinTemp = currentSlider->getValue();
            }

        }
    }
    
    if( (sliderMax != sliderMaxTemp) || sliderMin != sliderMinTemp)
    {
        sliderMax = sliderMaxTemp;
        sliderMin = sliderMinTemp;
        
        for(int i = 0; i<sliders.size(); i++)
        {
            for(int j = 0; j<sliders[i]->size(); j++)
            {
                BKSubSlider* currentSlider = sliders[i]->operator[](j);
                if(currentSlider != nullptr)
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
    
    displaySlider->setBounds(area.removeFromLeft(displaySliderWidth).toNearestInt());
    editValsTextField->setBounds(area.toNearestInt());
    editValsTextField->setVisible(false);
    
    showName.setBounds(area.toNearestInt());
    showName.setJustificationType(Justification::topRight);
    showName.toFront(false);
    
    bigInvisibleSlider->setBounds(area.toNearestInt());
    
    sliderWidth = (float)area.getWidth() / numVisibleSliders;
    
    for (int i=0; i<numVisibleSliders; i++)
    {
        Rectangle<float> sliderArea (area.removeFromLeft(sliderWidth));
        for(int j=0; j<sliders[i]->size(); j++)
        {
            BKSubSlider* currentSlider = sliders[i]->operator[](j);
            if(currentSlider != nullptr)
            {
                currentSlider->setBounds(sliderArea.toNearestInt());
            }
        }
    }
    
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

        setTo(stringToArrayFloatArray(textEditor.getText()), sendNotification);
        resetRanges();
        resized();
        
        listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                       getName(),
                       getAllActiveValues());
        
    }
}


Array<Array<float>> BKMultiSlider::getAllValues()
{
    Array<Array<float>> currentvals;
    
    for(int i=0; i<sliders.size(); i++)
    {
        Array<float> toAdd;
        
        for(int j=0; j<sliders[i]->size(); j++)
        {
            BKSubSlider* currentSlider = sliders[i]->operator[](j);
            if(currentSlider != nullptr)
            {
                toAdd.add(currentSlider->getValue());
            }
        }
        
        currentvals.add(toAdd);
    }
    
    return currentvals;
}


Array<Array<float>> BKMultiSlider::getAllActiveValues()
{
    Array<Array<float>> currentvals;
    
    for(int i=0; i<sliders.size(); i++)
    {
        Array<float> toAdd;
        
        for(int j=0; j<sliders[i]->size(); j++)
        {
            BKSubSlider* currentSlider = sliders[i]->operator[](j);
            if(currentSlider != nullptr)
            {
                float valToAdd = currentSlider->getValue();
                if(fabs(valToAdd) < 0.000001) valToAdd = 0.;
                
                if(currentSlider->isActive())
                    toAdd.add(valToAdd);
            }
        }
        
        if(toAdd.size() > 0) currentvals.add(toAdd);
    }
    
    return currentvals;
}


Array<float> BKMultiSlider::getOneSliderBank(int which)
{
    Array<float> newvals;
    for(int i=0; i<sliders[which]->size(); i++)
    {
        BKSubSlider* currentSlider = sliders[which]->operator[](i);
        if(currentSlider != nullptr)
        {
            newvals.add(currentSlider->getValue());
        }
    }
    
    return newvals;
}


void BKMultiSlider::showModifyPopupMenu(int which) 
{
    PopupMenu m;
    m.setLookAndFeel (&getLookAndFeel());
    m.addItem (1, translate ("deactivate slider"), true, false);
    m.addItem (2, translate ("deactivate all after this"), true, false);
    m.addItem (3, translate ("deactivate all before this"), true, false);
    m.addSeparator();
    
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::forComponent (sliderModifyMenuCallback, this, which));
}

void BKMultiSlider::sliderModifyMenuCallback (const int result, BKMultiSlider* ms, int which)
{
    if (ms != nullptr)
    {
        switch (result)
        {
            case 1:   ms->deactivateSlider(which, sendNotification); break;
            case 2:   ms->deactivateAllAfter(which, sendNotification); break;
            case 3:   ms->deactivateAllBefore(which, sendNotification); break;

            default:  break;
        }
    }
}

void BKMultiSlider::setCurrentSlider(int activeSliderNum)
{
    //find activeSliderNum, highlight it as current, dehighlight all the others...
    //DBG("will highlight slider num " + String(activeSliderNum));
    
    int sliderNum = getActiveSlider(activeSliderNum);
    
    if(sliderNum != lastHighlightedSlider)
    {
        highlight(sliderNum);
        deHighlight(lastHighlightedSlider);
        lastHighlightedSlider = sliderNum;
        displaySlider->setValue(sliders[sliderNum]->operator[](0)->getValue());
    }
}

int BKMultiSlider::getActiveSlider(int sliderNum)
{
    int sliderCount = 0;
    
    for(int i = 0; i<sliders.size(); i++)
    {
        if(sliderCount == sliderNum && sliders[i]->operator[](0)->isActive())
            return i;
        
        if(sliders[i]->operator[](0)->isActive())
            sliderCount++;
    }
    
    return 0;
}

void BKMultiSlider::highlight(int activeSliderNum)
{
    //need to count through depth, but for now just the first one...
    for(int i=0; i<sliders[activeSliderNum]->size(); i++)
    {
        sliders[activeSliderNum]->operator[](i)->setLookAndFeel(&highlightedSliderLookAndFeel);
    }
}

void BKMultiSlider::deHighlight(int sliderNum)
{
    for(int i=0; i<sliders[sliderNum]->size(); i++)
    {
        sliders[sliderNum]->operator[](i)->setLookAndFeel(&activeSliderLookAndFeel);
    }

}



// ******************************************************************************************************************** //
// **************************************************  BKSingleSlider ************************************************** //
// ******************************************************************************************************************** //

BKSingleSlider::BKSingleSlider (String name, double min, double max, double def, double increment):
sliderName(name),
sliderMin(min),
sliderMax(max),
sliderDefault(def),
sliderIncrement(increment)
{
    
    textIsAbove = true;
    
    thisSlider.setSliderStyle(Slider::LinearHorizontal);
    thisSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    thisSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    thisSlider.setValue(sliderDefault, dontSendNotification);
    thisSlider.addListener(this);
    thisSlider.addMouseListener(this, true);
    addAndMakeVisible(thisSlider);
    
    showName.setText(name, dontSendNotification);
    showName.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(showName);
    
    valueTF.setText(String(sliderDefault));
    valueTF.addListener(this);
    addAndMakeVisible(valueTF);
    
}

void BKSingleSlider::sliderValueChanged (Slider *slider)
{
    if(slider == &thisSlider)
    {
        listeners.call(&BKSingleSliderListener::BKSingleSliderValueChanged,
                       getName(),
                       thisSlider.getValue());
        
        valueTF.setText(String(thisSlider.getValue()), dontSendNotification);
    }
}


void BKSingleSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    double newval = textEditor.getText().getDoubleValue();
    
    checkValue(newval);
    
    thisSlider.setValue(newval, sendNotification);
    
    listeners.call(&BKSingleSliderListener::BKSingleSliderValueChanged,
                   getName(),
                   thisSlider.getValue());
}

void BKSingleSlider::checkValue(double newval)
{
    if(newval > thisSlider.getMaximum()) {
        thisSlider.setRange(thisSlider.getMinimum(), newval, sliderIncrement);
    }
    
    if(newval < thisSlider.getMinimum()) {
        thisSlider.setRange(newval, thisSlider.getMaximum(), sliderIncrement);
    }
    
    if(newval <= sliderMax && thisSlider.getMaximum() > sliderMax)
    {
        thisSlider.setRange(thisSlider.getMinimum(), sliderMax, sliderIncrement);
    }
    
    if(newval >= sliderMin && thisSlider.getMinimum() < sliderMin)
    {
        thisSlider.setRange(sliderMin, thisSlider.getMaximum(), sliderIncrement);
    }
}

void BKSingleSlider::mouseUp(const MouseEvent &event)
{
    if(event.mouseWasClicked())
    {
        if(event.mods.isShiftDown())
        {
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

void BKSingleSlider::resized()
{
    if(textIsAbove)
    {
        Rectangle<int> area (getLocalBounds());
        
        //Rectangle<int> textSlab (area.removeFromTop(area.getHeight() / 3));
        Rectangle<int> textSlab (area.removeFromTop(20));
        //textSlab.removeFromTop(textSlab.getHeight() - 20);
        textSlab.removeFromRight(5);
        valueTF.setBounds(textSlab.removeFromRight(50));
        showName.setBounds(textSlab.removeFromRight(150));
        
        thisSlider.setBounds(area.removeFromTop(20));
    }
    else
    {
        Rectangle<int> area (getLocalBounds());
        
        //Rectangle<int> textSlab (area.removeFromBottom(area.getHeight() / 3));
        Rectangle<int> textSlab (area.removeFromBottom(20));
        //textSlab.removeFromBottom(textSlab.getHeight() - 20);
        textSlab.removeFromRight(5);
        valueTF.setBounds(textSlab.removeFromRight(50));
        showName.setBounds(textSlab.removeFromRight(100));
        
        thisSlider.setBounds(area.removeFromBottom(20));
    }
}

void BKSingleSlider::setValue(double newval, NotificationType notify)
{
    checkValue(newval);
    thisSlider.setValue(newval, notify);
    valueTF.setText(String(thisSlider.getValue()), notify);
}



// ******************************************************************************************************************** //
// **************************************************  BKRangeSlider ************************************************** //
// ******************************************************************************************************************** //

BKRangeSlider::BKRangeSlider (String name, double min, double max, double defmin, double defmax, double increment):
sliderName(name),
sliderMin(min),
sliderMax(max),
sliderDefaultMin(defmin),
sliderDefaultMax(defmax),
sliderIncrement(increment)
{
    
    showName.setText(sliderName, dontSendNotification);
    showName.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(showName);
    
    minValueTF.setText(String(sliderDefaultMin));
    minValueTF.setName("minvalue");
    minValueTF.addListener(this);
    addAndMakeVisible(minValueTF);
    
    maxValueTF.setText(String(sliderDefaultMax));
    maxValueTF.setName("maxvalue");
    maxValueTF.addListener(this);
    addAndMakeVisible(maxValueTF);
    
    minSlider.setSliderStyle(Slider::LinearHorizontal);
    minSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    minSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    minSlider.setValue(sliderDefaultMin, dontSendNotification);
    minSlider.addListener(this);
    minSlider.setLookAndFeel(&minSliderLookAndFeel);
    //minSlider.setInterceptsMouseClicks(false, true);
    minSliderLookAndFeel.setColour(Slider::trackColourId, Colour::fromRGBA(55, 105, 250, 50));
    addAndMakeVisible(minSlider);
    
    maxSlider.setSliderStyle(Slider::LinearHorizontal);
    maxSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    maxSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    maxSlider.setValue(sliderDefaultMax, dontSendNotification);
    maxSlider.addListener(this);
    maxSlider.setLookAndFeel(&maxSliderLookAndFeel);
    //maxSlider.setInterceptsMouseClicks(false, true);
    //maxSliderLookAndFeel.setColour(Slider::trackColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    maxSliderLookAndFeel.setColour(Slider::trackColourId, Colour::fromRGBA(55, 105, 250, 50));
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
}

void BKRangeSlider::setMinValue(double newval, NotificationType notify)
{
    checkValue(newval);
    minSlider.setValue(newval, notify);
    minValueTF.setText(String(minSlider.getValue()), dontSendNotification);
    rescaleMinSlider();
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
                maxSlider.setValue(invisibleSlider.getValue());
                maxValueTF.setText(String(maxSlider.getValue()), dontSendNotification);
            }
            else
            {
                minSlider.setValue(invisibleSlider.getValue());
                minValueTF.setText(String(minSlider.getValue()), dontSendNotification);
            }
            
            listeners.call(&BKRangeSliderListener::BKRangeSliderValueChanged,
                           getName(),
                           minSlider.getValue(),
                           maxSlider.getValue());
        }
    }
}

void BKRangeSlider::mouseDown (const MouseEvent &event)
{
    if(event.eventComponent == &invisibleSlider)
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
    }
}


void BKRangeSlider::sliderDragEnded(Slider *slider)
{
    newDrag = false;
}

void BKRangeSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    double newval = textEditor.getText().getDoubleValue();
    
    //adjusts min/max of sldiers as needed
    checkValue(newval);
    
    if(textEditor.getName() == "minvalue")
    {
        minSlider.setValue(newval, sendNotification);
        rescaleMinSlider();
    }
    else if(textEditor.getName() == "maxvalue")
    {
        maxSlider.setValue(newval, sendNotification);
        rescaleMaxSlider();
    }
    
    listeners.call(&BKRangeSliderListener::BKRangeSliderValueChanged,
                   getName(),
                   minSlider.getValue(),
                   maxSlider.getValue());
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
    
    Rectangle<int> topSlab (area.removeFromTop(area.getHeight() / 3));
    topSlab.removeFromTop(topSlab.getHeight() - 20);
    topSlab.removeFromRight(5);
    maxValueTF.setBounds(topSlab.removeFromRight(50));
    minValueTF.setBounds(topSlab.removeFromRight(50));
    showName.setBounds(topSlab.removeFromRight(100));

    Rectangle<int> sliderArea (area.removeFromTop(40));
    minSlider.setBounds(sliderArea);
    maxSlider.setBounds(sliderArea);
    invisibleSlider.setBounds(sliderArea);

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
    
    File file("~/bkImages/samplePic.png");
    FileInputStream inputStream(file);
    PNGImageFormat sampleImageLoader;
    sampleImageComponent.setImage(sampleImageLoader.decodeImage(inputStream));
    sampleImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    addAndMakeVisible(sampleImageComponent);
    
    wavedistanceName.setText("wave distance (ms)", dontSendNotification);
    wavedistanceName.setJustificationType(Justification::topRight);
    addAndMakeVisible(wavedistanceName);
    
    undertowName.setText("undertow (ms)", dontSendNotification);
    undertowName.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(undertowName);

    wavedistanceSlider = new Slider();
    wavedistanceSlider->setRange(sliderMin, sliderMax, sliderIncrement);
    wavedistanceSlider->setSliderStyle(Slider::SliderStyle::LinearBar);
    wavedistanceSlider->addListener(this);
    wavedistanceSlider->setSkewFactor(skewFactor);
    addAndMakeVisible(wavedistanceSlider);

    undertowSlider = new Slider();
    undertowSlider->setRange(sliderMin, sliderMax, sliderIncrement);
    undertowSlider->setSliderStyle(Slider::SliderStyle::LinearBar);
    undertowSlider->addListener(this);
    undertowSlider->setSkewFactor(skewFactor);
    addAndMakeVisible(undertowSlider);

    for(int i=0; i<maxSliders; i++)
    {
        displaySliders.insert(0, new Slider());
        Slider* newSlider = displaySliders.getUnchecked(0);
        
        newSlider->setRange(sliderMin, sliderMax, sliderIncrement);
        newSlider->setLookAndFeel(&displaySliderLookAndFeel);
        newSlider->setSliderStyle(BKSubSlider::SliderStyle::LinearBar);
        newSlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        newSlider->setInterceptsMouseClicks(false, false);
        newSlider->setSkewFactor(skewFactor);
        addAndMakeVisible(newSlider);
    }
    
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
    
    wavedistanceSlider->setBounds(area.removeFromTop(20));
    undertowSlider->setBounds(area.removeFromBottom(20));
    
    undertowName.setBounds(area);
    wavedistanceName.setBounds(area);
    
    sampleImageComponent.setBounds(area);
    
    for(int i=0; i<maxSliders; i++)
    {
        Slider* newSlider = displaySliders.getUnchecked(i);
        newSlider->setBounds(area);
    }
    
    
}

void BKWaveDistanceUndertowSlider::sliderDragEnded(Slider *slider)
{
    if(slider == wavedistanceSlider)
    {
        setWaveDistance(wavedistanceSlider->getValue(), dontSendNotification);
    }
    
    listeners.call(&BKWaveDistanceUndertowSliderListener::BKWaveDistanceUndertowSliderValueChanged,
                   "nSlider",
                   wavedistanceSlider->getValue(),
                   undertowSlider->getValue());
}

void BKWaveDistanceUndertowSlider::setWaveDistance(int newwavedist, NotificationType notify)
{
    wavedistanceSlider->setValue(newwavedist, notify);
    
    int xpos = wavedistanceSlider->getPositionOfValue(wavedistanceSlider->getValue());
    undertowSlider->setBounds(xpos, undertowSlider->getY(), getWidth() - xpos, undertowSlider->getHeight());
    undertowSlider->setRange(sliderMin, sliderMax - wavedistanceSlider->getValue(), sliderIncrement);

    for(int i=0; i<maxSliders; i++)
    {
        Slider* newSlider = displaySliders.getUnchecked(i);

        newSlider->setBounds(xpos, newSlider->getY(), getWidth() - xpos, newSlider->getHeight());
        newSlider->setRange(sliderMin, sliderMax - wavedistanceSlider->getValue(), sliderIncrement);
    }
    
}

void BKWaveDistanceUndertowSlider::setUndertow(int newundertow, NotificationType notify)
{
    undertowSlider->setValue(newundertow, notify);
}



// ******************************************************************************************************************** //
// **************************************************  BKLookAndFeel Stuff ******************************************** //
// ******************************************************************************************************************** //

void BKMultiSliderLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float minSliderPos, float maxSliderPos,
                                            const Slider::SliderStyle style, Slider& slider)
{
    g.fillAll (slider.findColour (Slider::backgroundColourId));
    
    if (style == Slider::LinearBar || style == Slider::LinearBarVertical)
    {
        const float fx = (float) x, fy = (float) y, fw = (float) width, fh = (float) height;
        
        Path p;
        
        if (style == Slider::LinearBarVertical)
            p.addRectangle (fx, sliderPos - 2, fw, 4); //band instead of bar, width of 4
        else
            //p.addRectangle (fx, fy, sliderPos - fx, fh);
            p.addRectangle (sliderPos - 2, fy, 4 , fh);
        
        Colour baseColour (slider.findColour (Slider::thumbColourId)
                           .withMultipliedSaturation (slider.isEnabled() ? 1.0f : 0.5f)
                           .withMultipliedAlpha (0.8f));
        
        g.setGradientFill (ColourGradient (baseColour.brighter (0.08f), 0.0f, 0.0f,
                                           baseColour.darker (0.08f), 0.0f, (float) height, false));
        g.fillPath (p);
        
        g.setColour (baseColour.darker (0.2f));
        
        
        if (style == Slider::LinearBarVertical)
            g.fillRect (fx, sliderPos, fw, 1.0f);
        else
            g.fillRect (sliderPos, fy, 1.0f, fh);
         
    }
    else
    {
        drawLinearSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        drawLinearSliderThumb (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}


void BKRangeMinSliderLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos,
                                       float minSliderPos,
                                       float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{
    {
        
        const auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);
        
        const Point<float> startPoint (slider.isHorizontal() ? x : width * 0.5f,
                                       slider.isHorizontal() ? height * 0.5f : height + y);
        
        const Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                                     slider.isHorizontal() ? startPoint.y : y);
        
        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
        
        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        const auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
        const auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;
        
        //minPoint = startPoint;
        //maxPoint = { kx, ky };

        minPoint = { kx, ky };
        maxPoint = endPoint;
        
        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath (valueTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
        
        g.setColour (slider.findColour (Slider::thumbColourId));
        const auto pointerColour = slider.findColour (Slider::thumbColourId);
        drawPointer (g, sliderPos - trackWidth,
                     //jmin (y + height - trackWidth * 2.0f, y + height * 0.5f),
                     //jmin (y + height - trackWidth * 1.0f, y + height * 0.5f),
                     y + height - trackWidth * 3.0f,
                     trackWidth * 2.0f, pointerColour, 4);
        
    }
}

void BKRangeMaxSliderLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                                    float sliderPos,
                                                    float minSliderPos,
                                                    float maxSliderPos,
                                                    const Slider::SliderStyle style, Slider& slider)
{
    {
        
        const auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);
        
        const Point<float> startPoint (slider.isHorizontal() ? x : width * 0.5f,
                                       slider.isHorizontal() ? height * 0.5f : height + y);
        
        const Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                                     slider.isHorizontal() ? startPoint.y : y);
        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        const auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
        const auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;
        
        //maxPoint = endPoint;
        //minPoint = { kx, ky };
        maxPoint = { kx, ky };
        minPoint = startPoint;
        
        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath (valueTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
        
        g.setColour (slider.findColour (Slider::thumbColourId));

        const auto pointerColour = slider.findColour (Slider::thumbColourId);
        const auto sr = jmin (trackWidth, (slider.isHorizontal() ? height : width) * 0.4f);
        drawPointer (g, sliderPos - sr,
                     //jmax (0.0f, y + height * 0.5f - trackWidth * 2.0f),
                     //jmax (0.0f, y + height * 0.5f - trackWidth * 4.0f),
                     y + height - trackWidth *6.0f,
                     trackWidth * 2.0f, pointerColour, 2);

        
        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        //g.setColour (slider.findColour (Slider::backgroundColourId));
        //g.strokePath (backgroundTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
    }
}



