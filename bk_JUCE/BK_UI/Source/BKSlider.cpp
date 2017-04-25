/*
  ==============================================================================

    BKSlider.cpp
    Created: 6 Apr 2017 9:50:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

// **************************************************  BKSingleSlider ************************************************** //

#include "BKSlider.h"

BKSingleSlider::BKSingleSlider (SliderStyle sstyle, double min, double max, double def, double increment, int width, int height):
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
    //setSize(sliderWidth, sliderHeight);
    
}


BKSingleSlider::~BKSingleSlider()
{
    
}


void BKSingleSlider::valueChanged()
{
    //DBG("slider val changed: " + String(getValue()));
    //setRange(sliderMin, sliderMax, sliderIncrement);
}



double BKSingleSlider::getValueFromText	(const String & text )
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
    
    numSliders = 12;
    
    if(sliderIsVertical) {
        sliderWidth = 80;
        sliderHeight = 20;
    }
    else
    {
        sliderWidth = 20;
        sliderHeight = 60;
    }
    
    
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
    
    for(int i = 0; i<numSliders; i++)
    {
        if(i==0) addSlider(-1, 0, true);
        else addSlider(-1, 0, false);
    }
    
    
    if(arrangedHorizontally)
    {
        int tempheight;
        if(!sliderIsBar) tempheight = sliders.getFirst()->getFirst()->getHeight() - sliders.getFirst()->getFirst()->getTextBoxHeight();
        else tempheight = sliders.getFirst()->getFirst()->getHeight();
        
        DBG("depth of first " + String(sliders.getUnchecked(0)->size()));
        DBG("tempheight = " + String(tempheight));
        
        bigInvisibleSlider = new BKSingleSlider(Slider::LinearBarVertical,
                                                sliderMin,
                                                sliderMax,
                                                sliderDefault,
                                                sliderIncrement,
                                                numSliders * sliders.getFirst()->getFirst()->getWidth(),
                                                tempheight);
        
        displaySlider = new BKSingleSlider(Slider::LinearBarVertical,
                                           sliderMin,
                                           sliderMax,
                                           sliderDefault,
                                           sliderIncrement,
                                           40,
                                           tempheight);
        
        DBG("done with horizontal setup");
    }
    else
    {
        int tempwidth;
        if(!sliderIsBar) tempwidth = sliders.getFirst()->getFirst()->getWidth() - sliders.getFirst()->getFirst()->getTextBoxWidth();
        else tempwidth = sliders.getFirst()->getFirst()->getWidth();
        
        bigInvisibleSlider = new BKSingleSlider(Slider::LinearBar,
                                                sliderMin,
                                                sliderMax,
                                                sliderDefault,
                                                sliderIncrement,
                                                tempwidth,
                                                numSliders * sliders.getFirst()->getFirst()->getHeight()); //LinearHorizontal, LinearBar
    }
    
    bigInvisibleSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0,0);
    bigInvisibleSlider->setAlpha(0.0);
    bigInvisibleSlider->addMouseListener(this, true);
    bigInvisibleSlider->setName("BIG");
    bigInvisibleSlider->addListener(this);
    bigInvisibleSlider->setLookAndFeel(&activeSliderLookAndFeel);
    //bigInvisibleSlider->setInterceptsMouseClicks(true, true);
    
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
    
 /*
    incDecSlider = new Slider();
    addAndMakeVisible (incDecSlider);
    incDecSlider->addMouseListener(this, true);
    incDecSlider->addListener(this);
    incDecSlider->setName("INCDEC");
    incDecSlider->setSliderStyle (Slider::IncDecButtons);
    incDecSlider->setRange (1., 1000., 1.0);
    incDecSlider->setIncDecButtonsMode (Slider::incDecButtonsNotDraggable);
    if(arrangedHorizontally) incDecSlider->setSize(sliderWidth, 20);
    else  incDecSlider->setSize(sliderWidth/2, sliderHeight);
    incDecSlider->setTextBoxStyle (Slider::NoTextBox, false, 90, 20);
    
    editTextButton = new TextButton();
    addAndMakeVisible (editTextButton);
    editTextButton->setButtonText ("edit");
    editTextButton->addListener (this);
    if(arrangedHorizontally) editTextButton->setSize(sliderWidth, 20);
    else editTextButton->setSize(sliderWidth/2, sliderHeight);
 */
  
    editValsTextField = new TextEditor();
    addAndMakeVisible(editValsTextField);
    //editValsTextField->setMultiLine(true);
    //editValsTextField->setScrollbarsShown(true);
    //editValsTextField->setSize(sliderWidth, sliderHeight);
    //if(!arrangedHorizontally) editValsTextField->setSize(getWidth(), sliderHeight);
    //else editValsTextField->setSize(sliderWidth, 20);
    editValsTextField->setMultiLine(true);
    editValsTextField->addListener(this);
    
    //initSizes();
    
    DBG("done with constructor");
    
}

BKMultiSlider::~BKMultiSlider()
{
    
}


void BKMultiSlider::setTo(Array<float> newvals)
{
    numSliders = newvals.size();
    DBG("new numsliders = " + String(numSliders));
    if(numSliders < 1) numSliders = 1;
    
    for(int i=0; i<numSliders; i++)
    {
        if(i >= sliders.size()) addSlider(-1, 0, true);
        
        if(sliders.getUnchecked(i)->getFirst()->getMaximum() < newvals[i]) sliders.getUnchecked(i)->getFirst()->setRange(sliderMin, newvals[i], sliderIncrement);
        if(sliders.getUnchecked(i)->getFirst()->getMinimum() > newvals[i]) sliders.getUnchecked(i)->getFirst()->setRange(newvals[i], sliderMax, sliderIncrement);
        
        sliders.getUnchecked(i)->getUnchecked(0)->setValue(newvals[i]);
        sliders.getUnchecked(i)->getUnchecked(0)->isActive(true);
        sliders.getUnchecked(i)->getUnchecked(0)->setLookAndFeel(&activeSliderLookAndFeel);
    }
    
    //if(sliders.size() > numSliders) sliders.removeRange(numSliders, sliders.size());
    
    resetRanges();
    resized();
}


void BKMultiSlider::setTo(Array<Array<float>> newvals)
{
    numSliders = newvals.size();
    DBG("new numsliders = " + String(numSliders));
    if(numSliders < 1) numSliders = 1;
    
    for(int i=0; i<numSliders; i++)
    {
        for(int j = 0; j<newvals[i].size(); j++)
        {
            if(i >= sliders.size()) addSlider(-1, j, true);
            else if(j >= sliders.getUnchecked(i)->size()) addSubSlider(i, j, true);
            
            if(sliders.getUnchecked(i)->getUnchecked(j)->getMaximum() < newvals[i][j]) sliders.getUnchecked(i)->getUnchecked(j)->setRange(sliderMin, newvals[i][j], sliderIncrement);
            if(sliders.getUnchecked(i)->getUnchecked(j)->getMinimum() > newvals[i][j]) sliders.getUnchecked(i)->getUnchecked(j)->setRange(newvals[i][j], sliderMax, sliderIncrement);
            
            sliders.getUnchecked(i)->getUnchecked(j)->setValue(newvals[i][j]);
            sliders.getUnchecked(i)->getUnchecked(j)->isActive(true);
            sliders.getUnchecked(i)->getUnchecked(j)->setLookAndFeel(&activeSliderLookAndFeel);
        }
    }
    
    //if(sliders.size() > numSliders) sliders.removeRange(numSliders, sliders.size());
    
    resetRanges();
    resized();
}


void BKMultiSlider::addSlider(int where, int depth, bool active)
{
    BKSingleSlider* newslider;

    if(arrangedHorizontally) newslider     = new BKSingleSlider(subsliderStyle,
                                                               sliderMin,
                                                               sliderMax,
                                                               sliderDefault,
                                                               sliderIncrement,
                                                               sliderWidth,
                                                               sliderHeight);
    else newslider                         = new BKSingleSlider(subsliderStyle,
                                                               sliderMin,
                                                               sliderMax,
                                                               sliderDefault,
                                                               sliderIncrement,
                                                               sliderWidth,
                                                               sliderHeight);
    
    newslider->setRange(sliderMin, sliderMax, sliderIncrement);
    newslider->addListener(this);

    if(where < 0) {
        sliders.add(new OwnedArray<BKSingleSlider>);
        sliders.getLast()->add(newslider);
    }
    else
    {
        sliders.insert(where, new OwnedArray<BKSingleSlider>);
        sliders.getUnchecked(where)->add(newslider);
    }
    
    DBG("sliders.size = " + String(sliders.size()));

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


void BKMultiSlider::addSubSlider(int where, int depth, bool active)
{
    BKSingleSlider* newslider;
    
    if(arrangedHorizontally) newslider     = new BKSingleSlider(subsliderStyle,
                                                                sliderMin,
                                                                sliderMax,
                                                                sliderDefault,
                                                                sliderIncrement,
                                                                sliderWidth,
                                                                sliderHeight);
    else newslider                         = new BKSingleSlider(subsliderStyle,
                                                                sliderMin,
                                                                sliderMax,
                                                                sliderDefault,
                                                                sliderIncrement,
                                                                sliderWidth,
                                                                sliderHeight);
    
    newslider->setRange(sliderMin, sliderMax, sliderIncrement);
    newslider->addListener(this);
    
    sliders.getUnchecked(where)->add(newslider);
    
    DBG("slider " + String(where) + " depth = " + String(sliders.getUnchecked(where)->size()));
    
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

void BKMultiSlider::insertSlider(int where)
{
    if(where < 0) where = 0;
    addSlider(where, 0, false);
    numSliders++;
    DBG("numSliders = " + String(numSliders));
    resized();
    
    listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                   getName(),
                   getAllActiveValues());
}

void BKMultiSlider::deleteSlider(int where)
{
    if(sliders.size() > 1)
    {
        sliders.remove(where);
        numSliders = sliders.size();
        resized();
        
        listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                       getName(),
                       getAllActiveValues());
    }
}

void BKMultiSlider::deactivateSlider(int where)
{
    if(sliders.size() > 1) {
        for(int i=0; i<sliders[where]->size(); i++) {
            BKSingleSlider *currentSlider = sliders[where]->getUnchecked(i);
            currentSlider->isActive(false);
            currentSlider->setLookAndFeel(&passiveSliderLookAndFeel);
        }
        
        listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                       getName(),
                       getAllActiveValues());
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
            BKSingleSlider *currentSlider = sliders[which]->getUnchecked(currentSubSlider);
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


void BKMultiSlider::mouseDoubleClick (const MouseEvent &e)
{
    if(e.eventComponent == bigInvisibleSlider)
    {
        //DBG("all vals " + arrayFloatArrayToString(getAllValues()));
        
        int which = whichSlider(e);
        
        //highlight number for current slider
        StringArray tokens;
        tokens.addTokens(arrayFloatArrayToString(getAllValues()), false); //arrayFloatArrayToString
        int startPoint = 0;
        int endPoint;
        
        for(int i=0; i < which; i++) startPoint += tokens[i].length() + 1;
        endPoint = startPoint + tokens[which].length();
        
        editValsTextField->setVisible(true);
        editValsTextField->toFront(true);
        editValsTextField->setText(arrayFloatArrayToString(getAllValues())); //arrayFloatArrayToString
        
        //if(arrangedHorizontally) editValsTextField->setSize(numSliders * sliders.getFirst()->getFirst()->getWidth(), editValsTextField->getHeight());
        //else editValsTextField->setSize(getWidth(), editValsTextField->getHeight());
        
        Range<int> highlightRange(startPoint, endPoint);
        editValsTextField->setHighlightedRegion(highlightRange);
    }
}

void BKMultiSlider::mouseDown (const MouseEvent &event)
{
    if(event.mouseWasClicked())
    {
        currentSubSlider = whichSubSlider(whichSlider(event));
        //DBG("current sub slider = " + String(currentSubSlider));
        
        if(event.mods.isCtrlDown())
        {
            //DBG("control down for slider " + String(whichSlider(event)));
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
            //DBG("shift clicked on slider");
            int which = whichSlider(event);
            if(which >= 0) {
                sliders[which]->getUnchecked(0)->setValue(sliderDefault); //again, need to identify which subslider to get
                
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
    
    int which;
    if(arrangedHorizontally) which = (x / sliders.getFirst()->getFirst()->getWidth());
    else which = (y / sliders.getFirst()->getFirst()->getHeight());
    
    if (which >= 0 && which < sliders.size()) return which;
    else return -1;
}

int BKMultiSlider::whichSubSlider (int which)
{
    if(which < 0) return 0;
    
    int whichSub = 0;
    float currentDistance = fabs(sliders.getUnchecked(which)->getFirst()->getValue() - currentInvisibleSliderValue);
    
    if(arrangedHorizontally) {
        for(int i=0; i<sliders.getUnchecked(which)->size(); i++)
        {
            float tempDistance = fabs(sliders.getUnchecked(which)->getUnchecked(i)->getValue() - currentInvisibleSliderValue);
            if(tempDistance < currentDistance)
            {
                whichSub = i;
                currentDistance = tempDistance;
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
        if(sliders.getUnchecked(i)->getFirst()->isActive()) counter++;
    
    return counter;
}

void BKMultiSlider::resetRanges()
{
    DBG("normalizing slider ranges");
    
    double sliderMinTemp = sliderMinDefault;
    double sliderMaxTemp = sliderMaxDefault;
    
    for(int i = 0; i<sliders.size(); i++)
    {
        for(int j = 0; j<sliders[i]->size(); j++)
        {
            if(sliders[i]->getUnchecked(j)->getValue() > sliderMaxTemp) sliderMaxTemp = sliders[i]->getUnchecked(j)->getValue();
            if(sliders[i]->getUnchecked(j)->getValue() < sliderMinTemp) sliderMinTemp = sliders[i]->getUnchecked(j)->getValue();
            //DBG("slider val " + String(sliders[i]->getUnchecked(j)->getValue()));
        }
    }
    
    if( (sliderMax != sliderMaxTemp) || sliderMin != sliderMinTemp)
    {
        sliderMax = sliderMaxTemp;
        sliderMin = sliderMinTemp;
        //DBG("new slider max/min " + String(sliderMax) + " " + String(sliderMin));
        
        for(int i = 0; i<sliders.size(); i++)
        {
            for(int j = 0; j<sliders[i]->size(); j++)
            {
                sliders[i]->getUnchecked(j)->setRange(sliderMin, sliderMax, sliderIncrement);
            }
        }
        
        bigInvisibleSlider->setRange(sliderMin, sliderMax, sliderIncrement);
        displaySlider->setRange(sliderMin, sliderMax, sliderIncrement);
    }
}

void BKMultiSlider::resized()
{
    
    Rectangle<int> area (getLocalBounds());
    displaySlider->setBounds(area.removeFromLeft(40));
    editValsTextField->setBounds(area);
    editValsTextField->setVisible(false);
    
    showName.setBounds(area);
    showName.toFront(false);
    
    bigInvisibleSlider->setBounds(area);
    
    sliderWidth = area.getWidth() / sliders.size();
    
    for (int i=0; i<sliders.size(); i++)
    {
        Rectangle<int> sliderArea (area.removeFromLeft(sliderWidth));
        for(int j=0; j<sliders.getUnchecked(i)->size(); j++)
        {
            sliders.getUnchecked(i)->getUnchecked(j)->setBounds(sliderArea);
            //sliders.getUnchecked(i)->getUnchecked(j)->setSize(sliderArea.getWidth(), sliderArea.getHeight());
        }
    }
    
    bigInvisibleSlider->toFront(false);
}


void BKMultiSlider::sliderValueChanged (Slider *slider)
{
    if (slider->getName() == "BIG")
    {
        currentInvisibleSliderValue = slider->getValue();
        //sendChangeMessage();
        //DBG(currentInvisibleSliderValue);
    }
    /*
    else if(slider->getName() == "INCDEC")
    {
        //DBG(slider->getValue());
        
        numSliders = slider->getValue();
        if(numSliders > sliders.size()) addSlider(-1);
        else sliders.removeLast();
        
        initSizes();
    }
     */
    else { //subsliders
        if(updatingTextBox) {
            updatingTextBox = false;
            resetRanges();
        }
    }
}

void BKMultiSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    editValsTextField->setVisible(false);
    editValsTextField->toBack();

    setTo(stringToArrayFloatArray(textEditor.getText()));

    listeners.call(&BKMultiSliderListener::multiSliderAllValuesChanged,
                   getName(),
                   getAllActiveValues());
    
    resetRanges();
    resized();
}


void BKMultiSlider::buttonClicked (Button* button)
{
    //setSize(10 * sliders.getFirst()->getWidth() + editTextButton->getWidth(), sliders.getFirst()->getHeight());
    editValsTextField->setVisible(true);
    editValsTextField->toFront(true);
    editValsTextField->setText(arrayFloatArrayToString(getAllValues())); //arrayFloatArrayToString
    
    //editValsTextField->setSize(getWidth() - editTextButton->getWidth(), editValsTextField->getHeight());
    //if(arrangedHorizontally) editValsTextField->setSize(numSliders * sliders.getFirst()->getFirst()->getWidth(), editValsTextField->getHeight());
    //else editValsTextField->setSize(getWidth(), editValsTextField->getHeight());
}


Array<Array<float>> BKMultiSlider::getAllValues()
{
    Array<Array<float>> currentvals;

    for(int i=0; i<sliders.size(); i++)
    {
        Array<float> toAdd;
        
        for(int j=0; j<sliders.getUnchecked(i)->size(); j++)
            toAdd.add(sliders.getUnchecked(i)->getUnchecked(j)->getValue());
        
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
        
        for(int j=0; j<sliders.getUnchecked(i)->size(); j++)
        {
            if(sliders.getUnchecked(i)->getUnchecked(j)->isActive())
               toAdd.add(sliders.getUnchecked(i)->getUnchecked(j)->getValue());
        }
        
        if(toAdd.size() > 0) currentvals.add(toAdd);
    }
    
    return currentvals;
}

Array<float> BKMultiSlider::getOneSliderBank(int which)
{
    Array<float> newvals;
    for(int i=0; i<sliders[which]->size(); i++)
        newvals.add(sliders[which]->getUnchecked(i)->getValue());
    return newvals;
}


void BKMultiSlider::showModifyPopupMenu(int which) 
{
    PopupMenu m;
    m.setLookAndFeel (&getLookAndFeel());
    m.addItem (1, translate ("insert slider before"), true, false);
    m.addItem (2, translate ("insert slider after"), true, false);
    m.addItem (3, translate ("delete slider"), true, false);
    m.addItem (4, translate ("deactivate slider"), true, false);
    m.addItem (5, translate ("deactivate all after this"), true, false);
    m.addItem (6, translate ("deactivate all before this"), true, false);
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
            case 1:   ms->insertSlider(which); break;
            case 2:   ms->insertSlider(which + 1); break;
            case 3:   ms->deleteSlider(which); break;
            case 4:   ms->deactivateSlider(which); break;
            case 5:   ms->deactivateAllAfter(which); break;
            case 6:   ms->deactivateAllBefore(which); break;

            default:  break;
        }
    }
}


/* ======================================================================================================== */

void BKSliderLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float minSliderPos, float maxSliderPos,
                                            const Slider::SliderStyle style, Slider& slider)
{
    g.fillAll (slider.findColour (Slider::backgroundColourId));
    
    if (style == Slider::LinearBar || style == Slider::LinearBarVertical)
    {
        const float fx = (float) x, fy = (float) y, fw = (float) width, fh = (float) height;
        
        Path p;
        
        if (style == Slider::LinearBarVertical)
            //p.addRectangle (fx, sliderPos, fw, 1.0f + fh - sliderPos);
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



