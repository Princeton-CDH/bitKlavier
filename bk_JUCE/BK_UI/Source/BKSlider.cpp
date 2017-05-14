/*
  ==============================================================================

    BKSlider.cpp
    Created: 6 Apr 2017 9:50:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/


/*
 TODO
 1. RangeSlider: overlay two regular sliders, set thumbs to be up/down diamonds; so min can be > max
 2. have displaySlider updated while cycling through, displaying current value
 3. have multiSlider highlight lookandfeel that shows which slider is currently active
*/



// ******************************************************************************************************************** //
// **************************************************  BKSubSlider ************************************************** //
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
    resetRanges();
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
    resetRanges();
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

void BKRangeSlider::sliderValueChanged (Slider *slider)
{
  
    if(slider == &invisibleSlider)
    {
        
        if(newDrag)
        {
            newDrag = false;
            
            DBG("about to move slider");
            
            float refDistance = fabs(invisibleSlider.getValue() - minSlider.getValue());

            /*
            if (fabs(invisibleSlider.getValue() - maxSlider.getValue()) < refDistance)
            {
                currentSlider = &maxSlider;
            }
            else
               */ 
            {
                if(clickedOnMinSlider)
                    currentSlider = &minSlider;
                else
                    currentSlider = &maxSlider;
            }
        
        
            if (currentSlider == &maxSlider)
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
                DBG("clicked on minslider side");
            }
            else
            {
                clickedOnMinSlider = false;
                DBG("clicked on maxslider side");
            }
            
            newDrag = true;
        }
    }
}

void BKRangeSlider::mouseDrag (const MouseEvent &event)
{
    if(event.eventComponent == &invisibleSlider)
    {
        {
            if(event.y > invisibleSlider.getHeight() / 2.)
            {
                clickedOnMinSlider = true;
                DBG("drag on minslider side");
            }
            else
            {
                clickedOnMinSlider = false;
                DBG("drag on maxslider side");
            }
            
            newDrag = true;
        }
    }
}

void BKRangeSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    double newval = textEditor.getText().getDoubleValue();
    
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
    
    if(textEditor.getName() == "minvalue")
    {
        minSlider.setValue(newval, sendNotification);
        
        if(minSlider.getMinimum() < sliderMin &&
           minSlider.getValue() > sliderMin &&
           maxSlider.getValue() > sliderMin)
        {
            maxSlider.setRange(sliderMin, maxSlider.getMaximum(), sliderIncrement);
            minSlider.setRange(sliderMin, maxSlider.getMaximum(), sliderIncrement);
            invisibleSlider.setRange(sliderMin, maxSlider.getMaximum(), sliderIncrement);
        }
    }
    else if(textEditor.getName() == "maxvalue")
    {
        maxSlider.setValue(newval, sendNotification);
        
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
    
    listeners.call(&BKRangeSliderListener::BKRangeSliderValueChanged,
                   getName(),
                   minSlider.getValue(),
                   maxSlider.getValue());
}

 void BKRangeSlider::sliderDragEnded(Slider *slider)
{
    //newDrag = true;
    //DBG("slider drag ended");
    newDrag = false;
}

void BKRangeSlider::resized()
{
    
    Rectangle<int> area (getLocalBounds());
    
    Rectangle<int> topSlab (area.removeFromTop(20));
    maxValueTF.setBounds(topSlab.removeFromRight(50));
    minValueTF.setBounds(topSlab.removeFromRight(50));
    showName.setBounds(topSlab.removeFromRight(100));
    
    //thisSlider.setBounds(area.removeFromTop(20));
    Rectangle<int> sliderArea (area.removeFromTop(40));
    minSlider.setBounds(sliderArea);
    maxSlider.setBounds(sliderArea);
    invisibleSlider.setBounds(sliderArea);
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
        
        minPoint = startPoint;
        maxPoint = { kx, ky };

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
        
        maxPoint = endPoint;
        minPoint = { kx, ky };

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



