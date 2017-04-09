/*
  ==============================================================================

    BKSlider.cpp
    Created: 6 Apr 2017 9:50:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

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
    
    if(sstyle == LinearVertical || sstyle == LinearBarVertical) sliderIsVertical = true;
    else sliderIsVertical = false;
    
    if(sstyle == LinearBarVertical || sstyle == LinearBar) sliderIsBar = true;
    else sliderIsBar = false;
    
    if(!sliderIsBar)
    {
        if(sliderIsVertical) setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
        else setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, false, 50, 20);
    }

    setRange(sliderMin, sliderMax, sliderIncrement);
    setValue(sliderDefault);
    setSize(sliderWidth, sliderHeight);
 
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

BKMultiSlider::BKMultiSlider(BKMultiSliderType which)
{
    
    if(which == VerticalMultiSlider || which == VerticalMultiBarSlider) sliderIsVertical = true;
    else sliderIsVertical = false;
    
    if(which == VerticalMultiBarSlider || which == HorizontalMultiBarSlider) sliderIsBar = true;
    else sliderIsBar = false;
    
    if(which == HorizontalMultiSlider || which == HorizontalMultiBarSlider) arrangedHorizontally = true;
    else arrangedHorizontally = false;
    
    sliderMin = sliderMinDefault = -1.;
    sliderMax = sliderMaxDefault = 1.;
    sliderIncrement = 0.01;
    sliderDefault = 0.;
    
    if(sliderIsVertical) {
        sliderWidth = 200;
        sliderHeight = 20;
    }
    else
    {
        sliderWidth = 50;
        sliderHeight = 200;
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
    
    
    addSubSlider();
    
    if(arrangedHorizontally)
    {
        int tempheight;
        if(!sliderIsBar) tempheight = sliders[0]->getHeight() - sliders[0]->getTextBoxHeight();
        else tempheight = sliders[0]->getHeight();
        
        bigInvisibleSlider = new BKSingleSlider(Slider::LinearBarVertical,
                                                sliderMin,
                                                sliderMax,
                                                sliderDefault,
                                                sliderIncrement,
                                                numSliders * sliders[0]->getWidth(),
                                                tempheight);
    }
    else
    {
        int tempwidth;
        if(!sliderIsBar) tempwidth = sliders[0]->getWidth() - sliders[0]->getTextBoxWidth();
        else tempwidth = sliders[0]->getWidth();
        
        bigInvisibleSlider = new BKSingleSlider(Slider::LinearBar,
                                                sliderMin,
                                                sliderMax,
                                                sliderDefault,
                                                sliderIncrement,
                                                tempwidth,
                                                numSliders * sliders[0]->getHeight()); //LinearHorizontal, LinearBar
    }
    
    bigInvisibleSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0,0);
    bigInvisibleSlider->setAlpha(0.0);
    bigInvisibleSlider->addMouseListener(this, true);
    bigInvisibleSlider->setName("BIG");
    bigInvisibleSlider->addListener(this);
    //bigInvisibleSlider->setInterceptsMouseClicks(true, true);
    
    addAndMakeVisible(bigInvisibleSlider);
    
    incDecSlider = new Slider();
    addAndMakeVisible (incDecSlider);
    incDecSlider->addMouseListener(this, true);
    incDecSlider->addListener(this);
    incDecSlider->setName("INCDEC");
    incDecSlider->setSliderStyle (Slider::IncDecButtons);
    incDecSlider->setRange (1., 100., 1.0);
    incDecSlider->setIncDecButtonsMode (Slider::incDecButtonsNotDraggable);
    if(arrangedHorizontally) incDecSlider->setSize(sliderWidth, 20);
    else  incDecSlider->setSize(50, sliderHeight);
    incDecSlider->setTextBoxStyle (Slider::NoTextBox, false, 90, 20);
    
    //setInterceptsMouseClicks(false, true);
    
    initSizes();
    
}

BKMultiSlider::~BKMultiSlider()
{
    
}

void BKMultiSlider::addSubSlider()
{
    BKSingleSlider* newslider;
    //SliderStyle sstyle, double min, double max, double def, double increment, int width, int height
    if(arrangedHorizontally) newslider = new BKSingleSlider(subsliderStyle,
                                                            sliderMin,
                                                            sliderMax,
                                                            sliderDefault,
                                                            sliderIncrement,
                                                            sliderWidth,
                                                            sliderHeight);
    else  newslider = new BKSingleSlider(subsliderStyle,
                                         sliderMin,
                                         sliderMax,
                                         sliderDefault,
                                         sliderIncrement,
                                         sliderWidth,
                                         sliderHeight); //LinearHorizontal, LinearBar
    
    newslider->setRange(sliderMin, sliderMax, sliderIncrement);
    newslider->addListener(this);
    
    sliders.add(newslider);
    addAndMakeVisible(newslider);
}

void BKMultiSlider::mouseDrag(const MouseEvent& e)
{
    if(e.eventComponent == bigInvisibleSlider)
    {
        int which = whichSubSlider(e);
        if(which >= 0) sliders[which]->setValue(currentInvisibleSliderValue);
    }
}

void BKMultiSlider::mouseDoubleClick (const MouseEvent &e)
{
    if(e.eventComponent == bigInvisibleSlider)
    {
        int which = whichSubSlider(e);
        if(which >= 0) sliders[which]->showTextBox();
        updatingTextBox = true;
    }
}

int BKMultiSlider::whichSubSlider (const MouseEvent &e)
{
    int x = e.x;
    int y = e.y;
    
    int which;
    if(arrangedHorizontally) which = (x / sliders[0]->getWidth());
    else which = (y / sliders[0]->getHeight());
    
    if (which >= 0 && which < sliders.size())
        return which;
    else return -1;
}

void BKMultiSlider::resetRanges()
{
    DBG("normalizing slider ranges");
    
    double sliderMinTemp = sliderMinDefault;
    double sliderMaxTemp = sliderMaxDefault;
    
    for(int i = 0; i<sliders.size(); i++)
    {
        if(sliders[i]->getValue() > sliderMaxTemp) sliderMaxTemp = sliders[i]->getValue();
        if(sliders[i]->getValue() < sliderMinTemp) sliderMinTemp = sliders[i]->getValue();
    }
    
    if( (sliderMax != sliderMaxTemp) || sliderMin != sliderMinTemp)
    {
        sliderMax = sliderMaxTemp;
        sliderMin = sliderMinTemp;
        for(int i = 0; i<sliders.size(); i++) sliders[i]->setRange(sliderMin, sliderMax, sliderIncrement);
        bigInvisibleSlider->setRange(sliderMin, sliderMax, sliderIncrement);
    }
}


void BKMultiSlider::resized()
{
    if(arrangedHorizontally)
    {
        for (int i=0; i<sliders.size(); i++)
        {
            sliders[i]->setTopLeftPosition(sliderWidth * i, incDecSlider->getHeight());
        }
        
        if(!sliderIsBar) {
            bigInvisibleSlider->setBounds(0, incDecSlider->getHeight(), sliders.getLast()->getRight(), sliders.getLast()->getHeight() - sliders.getLast()->getTextBoxHeight());
            //incDecSlider->setTopLeftPosition((int)(bigInvisibleSlider->getWidth()/2. - incDecSlider->getWidth()/2.), sliders.getLast()->getBottom());
            incDecSlider->setTopLeftPosition(0, 0);
        }
        else
        {
            bigInvisibleSlider->setBounds(0, incDecSlider->getHeight(), sliders.getLast()->getRight(), sliders.getLast()->getHeight());
            //incDecSlider->setTopLeftPosition((int)(bigInvisibleSlider->getWidth()/2. - incDecSlider->getWidth()/2.), bigInvisibleSlider->getBottom());
            incDecSlider->setTopLeftPosition(0, 0);
        }
    }
    else
    {
        for (int i=0; i<sliders.size(); i++)
        {
            sliders[i]->setTopLeftPosition(0, sliderHeight * i + incDecSlider->getHeight());
        }
        
        if(!sliderIsBar)
            bigInvisibleSlider->setBounds(sliders.getLast()->getTextBoxWidth(), incDecSlider->getHeight(), sliders.getLast()->getRight() - sliders.getLast()->getTextBoxWidth(), sliders.getLast()->getBottom());
        else bigInvisibleSlider->setBounds(0, incDecSlider->getHeight(), sliders.getLast()->getRight(), sliders.getLast()->getBottom());
        
        //incDecSlider->setTopLeftPosition((int)(sliderWidth/2. - incDecSlider->getWidth()/2.), bigInvisibleSlider->getBottom());
        incDecSlider->setTopLeftPosition((int)(sliderWidth/2. - incDecSlider->getWidth()/2.), 0);
    }
}

void BKMultiSlider::initSizes()
{
    if(arrangedHorizontally) {
        bigInvisibleSlider->setSize(numSliders * sliders[0]->getWidth(), bigInvisibleSlider->getHeight());
        setSize(numSliders * sliders[0]->getWidth(), sliders[0]->getHeight() + incDecSlider->getHeight());
        bigInvisibleSlider->toFront(false);
    }
    else
    {
        bigInvisibleSlider->setSize(bigInvisibleSlider->getWidth(), numSliders * sliders[0]->getWidth());
        if(!sliderIsBar) setSize(sliders[0]->getWidth() + sliders[0]->getTextBoxWidth(), numSliders * sliders[0]->getHeight() + incDecSlider->getHeight());
        else setSize(sliders[0]->getWidth(), numSliders * sliders[0]->getHeight() + incDecSlider->getHeight());
        bigInvisibleSlider->toFront(false);
    }
}

void BKMultiSlider::sliderValueChanged (Slider *slider)
{
    if (slider->getName() == "BIG")
    {
        currentInvisibleSliderValue = slider->getValue();
        DBG(currentInvisibleSliderValue);
    }
    else if(slider->getName() == "INCDEC")
    {
        DBG(slider->getValue());
        
        numSliders = slider->getValue();
        if(numSliders > sliders.size()) addSubSlider();
        else sliders.removeLast();
        
        initSizes();
    }
    else {
        if(updatingTextBox) {
            updatingTextBox = false;
            resetRanges();
        }
    }
}

