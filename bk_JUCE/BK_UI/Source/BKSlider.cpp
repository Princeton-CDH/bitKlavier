/*
  ==============================================================================

    BKSlider.cpp
    Created: 6 Apr 2017 9:50:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

// **************************************************  BKSubSlider ************************************************** //

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
        //setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 0, 0);
        if(sliderIsVertical) setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    }

    setRange(sliderMin, sliderMax, sliderIncrement);
    setValue(sliderDefault);
    
}


BKSubSlider::~BKSubSlider()
{
    
}


void BKSubSlider::valueChanged()
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


// **************************************************  BKMultiSlider ************************************************** //

BKMultiSlider::BKMultiSlider(BKMultiSliderType which)
{
    
    if(which == VerticalMultiSlider || which == VerticalMultiBarSlider) sliderIsVertical = true;
    else sliderIsVertical = false;
    
    if(which == VerticalMultiBarSlider || which == HorizontalMultiBarSlider) sliderIsBar = true;
    else sliderIsBar = false;
    
    if(which == HorizontalMultiSlider || which == HorizontalMultiBarSlider) arrangedHorizontally = true;
    else arrangedHorizontally = false;
    
    passiveSliderLookAndFeel.setColour(Slider::thumbColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    
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
    
    deactivateAll();
    
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
    resized();
}


void BKMultiSlider::setTo(Array<Array<float>> newvals, NotificationType newnotify)
{
    numActiveSliders = newvals.size();
    if(numActiveSliders < 1) numActiveSliders = 1;
    
    if(numActiveSliders <= numDefaultSliders) numVisibleSliders = numDefaultSliders;
    else numVisibleSliders = numActiveSliders;
    
    deactivateAll();

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
    resized();
}



void BKMultiSlider::cleanupSliderArray()
{
    sliders.getLock().enter();
    //doc says "...or preferably use an object of ScopedLockType as an RAII lock for it"
    //not sure if we should do that or not.
    
    for(int i=sliders.size() - 1; i>=0; i--)
    {
        //remove sliders above numVisibleSliders
        if(i >= numVisibleSliders)
        {
            for(int j=0; j<sliders[i]->size(); j++)
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
    
    sliders.getLock().exit();
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
    newslider->setValue(sliderDefault);
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
    newslider->setValue(sliderDefault);
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



void BKMultiSlider::deactivateSlider(int where)
{
    if(sliders.size() > 1) {
        for(int i=0; i<sliders[where]->size(); i++) {
            
            BKSubSlider* currentSlider = sliders[where]->operator[](i);
            if(currentSlider != nullptr) {
                currentSlider->isActive(false);
                currentSlider->setLookAndFeel(&passiveSliderLookAndFeel);
                currentSlider->setValue(sliderDefault);
            }
        }
        
        listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                       getName(),
                       getAllActiveValues());
    }
}

void BKMultiSlider::deactivateAll()
{
    for(int i=0; i<sliders.size(); i++ )
    {
        deactivateSlider(i);
    }
}


void BKMultiSlider::deactivateAllAfter(int where)
{
    for(int i=where+1; i<sliders.size(); i++ )
    {
        deactivateSlider(i);
    }
}

void BKMultiSlider::deactivateAllBefore(int where)
{
    if (where > sliders.size()) where = sliders.size();
    for(int i=0; i<where; i++ )
    {
        deactivateSlider(i);
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
                currentSlider->setValue(currentInvisibleSliderValue);
                displaySlider->setValue(currentInvisibleSliderValue);
                
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
        int whichSub = whichSubSlider(which);
        if(which >= 0 && whichSub >= 0)
        {
            BKSubSlider* currentSlider = sliders[which]->operator[](whichSub);
            if (currentSlider != nullptr)
            {
                displaySlider->setValue(currentSlider->getValue());
            }
        }
    }
}


void BKMultiSlider::mouseDoubleClick (const MouseEvent &e)
{
    if(e.eventComponent == bigInvisibleSlider)
    {
        int which = whichSlider(e);
        
        //highlight number for current slider
        StringArray tokens;
        tokens.addTokens(arrayFloatArrayToString(getAllActiveValues()), false); //arrayFloatArrayToString
        int startPoint = 0;
        int endPoint;
        
        for(int i=0; i < which; i++) startPoint += tokens[i].length() + 1;
        endPoint = startPoint + tokens[which].length();
        
        editValsTextField->setVisible(true);
        editValsTextField->toFront(true);
        editValsTextField->setText(arrayFloatArrayToString(getAllActiveValues())); //arrayFloatArrayToString
        
        Range<int> highlightRange(startPoint, endPoint);
        editValsTextField->setHighlightedRegion(highlightRange);
    }
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
    //DBG("normalizing slider ranges");
    
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
                if(currentSlider->isActive())
                    toAdd.add(currentSlider->getValue());
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
            case 1:   ms->deactivateSlider(which); break;
            case 2:   ms->deactivateAllAfter(which); break;
            case 3:   ms->deactivateAllBefore(which); break;

            default:  break;
        }
    }
}


BKSingleSlider::BKSingleSlider (String name, double min, double max, double def, double increment):
sliderName(name),
sliderMin(min),
sliderMax(max),
sliderDefault(def),
sliderIncrement(increment)
{
    
    thisSlider.setSliderStyle(Slider::LinearHorizontal);
    thisSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    thisSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    thisSlider.setValue(sliderDefault, dontSendNotification);
    thisSlider.addListener(this);
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
    
    if(newval > thisSlider.getMaximum()) {
        sliderMax = newval;
        thisSlider.setRange(thisSlider.getMinimum(), newval, sliderIncrement);
    }
    
    if(newval < thisSlider.getMinimum()) {
        sliderMin = newval;
        thisSlider.setRange(newval, thisSlider.getMaximum(), sliderIncrement);
    }
    
    thisSlider.setValue(newval, sendNotification);
    
    listeners.call(&BKSingleSliderListener::BKSingleSliderValueChanged,
                   getName(),
                   thisSlider.getValue());
}

void BKSingleSlider::resized()
{
    
    Rectangle<int> area (getLocalBounds());
    
    Rectangle<int> topSlab (area.removeFromTop(20));
    valueTF.setBounds(topSlab.removeFromRight(50));
    showName.setBounds(topSlab.removeFromRight(100));
    
    thisSlider.setBounds(area.removeFromTop(20));
}


BKRangeSlider::BKRangeSlider (String name, double min, double max, double defmin, double defmax, double increment):
sliderName(name),
sliderMin(min),
sliderMax(max),
sliderDefaultMin(defmin),
sliderDefaultMax(defmax),
sliderIncrement(increment)
{
    
    thisSlider.setSliderStyle(Slider::TwoValueHorizontal);
    thisSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    thisSlider.setRange(sliderMin, sliderMax, sliderIncrement);
    thisSlider.setMinValue(sliderDefaultMin, dontSendNotification);
    thisSlider.setMaxValue(sliderDefaultMax, dontSendNotification);
    thisSlider.addListener(this);
    addAndMakeVisible(thisSlider);
    
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
    
}

void BKRangeSlider::sliderValueChanged (Slider *slider)
{
    if(slider == &thisSlider)
    {
        listeners.call(&BKRangeSliderListener::BKRangeSliderValueChanged,
                       getName(),
                       thisSlider.getMinValue(),
                       thisSlider.getMaxValue());
        
        minValueTF.setText(String(thisSlider.getMinValue()), dontSendNotification);
        maxValueTF.setText(String(thisSlider.getMaxValue()), dontSendNotification);
    }
}


void BKRangeSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    double newval = textEditor.getText().getDoubleValue();
    
    if(newval > thisSlider.getMaximum()) {
        sliderMax = newval;
        thisSlider.setRange(thisSlider.getMinimum(), newval, sliderIncrement);
    }
    
    if(newval < thisSlider.getMinimum()) {
        sliderMin = newval;
        thisSlider.setRange(newval, thisSlider.getMaximum(), sliderIncrement);
    }
    
    if(textEditor.getName() == minValueTF.getName())
    {
        thisSlider.setMinValue(newval, sendNotification);
    }
    else if(textEditor.getName() == maxValueTF.getName())
    {
        thisSlider.setMaxValue(newval, sendNotification);
    }
    
    
    listeners.call(&BKRangeSliderListener::BKRangeSliderValueChanged,
                   getName(),
                   thisSlider.getMinValue(),
                   thisSlider.getMaxValue());
}

void BKRangeSlider::resized()
{
    
    Rectangle<int> area (getLocalBounds());
    
    Rectangle<int> topSlab (area.removeFromTop(20));
    maxValueTF.setBounds(topSlab.removeFromRight(50));
    minValueTF.setBounds(topSlab.removeFromRight(50));
    showName.setBounds(topSlab.removeFromRight(100));
    
    thisSlider.setBounds(area.removeFromTop(20));
}


/* ======================================================================================================== */

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
            p.addRectangle (fx, fy, sliderPos - fx, fh);
        
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



