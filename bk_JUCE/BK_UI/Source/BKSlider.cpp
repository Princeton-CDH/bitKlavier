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


// **************************************************  BKMultiSlider ************************************************** //

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
        sliderWidth = 80;
        sliderHeight = 20;
    }
    else
    {
        sliderWidth = 30;
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
    
    editValsTextField = new TextEditor();
    addAndMakeVisible(editValsTextField);
    editValsTextField->setSize(sliderWidth, sliderHeight);
    editValsTextField->addListener(this);
    
    initSizes();
    
}

BKMultiSlider::~BKMultiSlider()
{
    
}

void BKMultiSlider::addSubSlider()
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
    
    if (which >= 0 && which < sliders.size()) return which;
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
        //DBG("slider val " + String(sliders[i]->getValue()));
    }
    
    if( (sliderMax != sliderMaxTemp) || sliderMin != sliderMinTemp)
    {
        sliderMax = sliderMaxTemp;
        sliderMin = sliderMinTemp;
        //DBG("new slider max/min " + String(sliderMax) + " " + String(sliderMin));
        for(int i = 0; i<sliders.size(); i++) sliders[i]->setRange(sliderMin, sliderMax, sliderIncrement);
        bigInvisibleSlider->setRange(sliderMin, sliderMax, sliderIncrement);
    }
}


void BKMultiSlider::resized()
{
    if(arrangedHorizontally)
    {
        incDecSlider->setTopLeftPosition(0, 0);
        editTextButton->setTopLeftPosition(0, getHeight() - editTextButton->getHeight());
        editValsTextField->setTopLeftPosition(editTextButton->getRight(), getHeight() - editTextButton->getHeight());
        editValsTextField->setVisible(false);
        
        for (int i=0; i<sliders.size(); i++)
        {
            sliders[i]->setTopLeftPosition(sliderWidth * i + editTextButton->getWidth(), 0);
        }
        
        if(!sliderIsBar) {
            bigInvisibleSlider->setBounds(editTextButton->getWidth(), 0, sliders.getLast()->getRight(), sliders.getLast()->getHeight() - sliders.getLast()->getTextBoxHeight());
        }
        else
        {
            bigInvisibleSlider->setBounds(editTextButton->getWidth(), 0, sliders.getLast()->getRight(), sliders.getLast()->getHeight());
        }
    }
    else
    {
        //incDecSlider->setTopLeftPosition((int)(sliderWidth/2. - incDecSlider->getWidth()/2.), 0);
        incDecSlider->setTopLeftPosition(0, 0);
        editTextButton->setTopRightPosition(getRight(), 0);
        editValsTextField->setTopLeftPosition(0, 0);
        editValsTextField->setVisible(false);
        
        for (int i=0; i<sliders.size(); i++)
        {
            sliders[i]->setTopLeftPosition(0, sliderHeight * i + incDecSlider->getHeight());
        }
        
        if(!sliderIsBar)
            bigInvisibleSlider->setBounds(sliders.getLast()->getTextBoxWidth(), incDecSlider->getHeight(), sliders.getLast()->getRight() - sliders.getLast()->getTextBoxWidth(), sliders.getLast()->getBottom());
        else bigInvisibleSlider->setBounds(0, incDecSlider->getHeight(), sliders.getLast()->getRight(), sliders.getLast()->getBottom());
    }
    
    
}

void BKMultiSlider::initSizes()
{
    if(arrangedHorizontally) {
        bigInvisibleSlider->setSize(numSliders * sliders[0]->getWidth(), bigInvisibleSlider->getHeight());
        setSize(numSliders * sliders[0]->getWidth() + editTextButton->getWidth(), sliders[0]->getHeight());
        bigInvisibleSlider->toFront(false);
    }
    else
    {
        bigInvisibleSlider->setSize(bigInvisibleSlider->getWidth(), numSliders * sliders[0]->getWidth());
        if(!sliderIsBar) setSize(sliders[0]->getWidth(), numSliders * sliders[0]->getHeight() + incDecSlider->getHeight());
        else setSize(sliders[0]->getWidth(), numSliders * sliders[0]->getHeight() + incDecSlider->getHeight());
        bigInvisibleSlider->toFront(false);
    }
}

void BKMultiSlider::sliderValueChanged (Slider *slider)
{
    if (slider->getName() == "BIG")
    {
        currentInvisibleSliderValue = slider->getValue();
        //DBG(currentInvisibleSliderValue);
    }
    else if(slider->getName() == "INCDEC")
    {
        //DBG(slider->getValue());
        
        numSliders = slider->getValue();
        if(numSliders > sliders.size()) addSubSlider();
        else sliders.removeLast();
        
        initSizes();
    }
    else { //subsliders
        if(updatingTextBox) {
            updatingTextBox = false;
            resetRanges();
        }
    }
}

void BKMultiSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    DBG(textEditor.getText());
    editValsTextField->setVisible(false);
    editValsTextField->toBack();
    
    Array<float> newvals = stringToFloatArray(textEditor.getText());
    
    numSliders = newvals.size();
    if(numSliders < 1) numSliders = 1;
    
    for(int i=0; i<numSliders; i++)
    {
        if(i >= sliders.size()) addSubSlider();
        
        if(sliders[i]->getMaximum() < newvals[i]) sliders[i]->setRange(sliderMin, newvals[i], sliderIncrement);
        if(sliders[i]->getMinimum() > newvals[i]) sliders[i]->setRange(newvals[i], sliderMax, sliderIncrement);
        
        sliders[i]->setValue(newvals[i]);
        //DBG("new array val " + String(newvals[i]));
    }
    
    if(sliders.size() > numSliders) sliders.removeRange(numSliders, sliders.size());
    
    resetRanges();
    initSizes();
    incDecSlider->setValue(numSliders, dontSendNotification);
}


String BKMultiSlider::textWindowDialog()
{
    //#if JUCE_MODAL_LOOPS_PERMITTED //this is a problem, apparently; need to find a different way to do this.
    AlertWindow w ("Multislider Values",
                   "Enter slider values by hand here...",
                   AlertWindow::QuestionIcon);
    
    w.addTextEditor ("text", floatArrayToString(getAllValues()), "text field:");
    
    w.addButton ("OK",     1, KeyPress (KeyPress::returnKey, 0, 0));
    w.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey, 0, 0));
    
    if (w.runModalLoop() != 0) // is they picked 'ok'
    {
        String text = w.getTextEditorContents ("text");
        return text;
    }
    return("cancelled");
    //#endif
}

void BKMultiSlider::buttonClicked (Button* button)
{
    //setSize(10 * sliders[0]->getWidth() + editTextButton->getWidth(), sliders[0]->getHeight());
    editValsTextField->setVisible(true);
    editValsTextField->toFront(true);
    editValsTextField->setText(floatArrayToString(getAllValues()));
    
    //editValsTextField->setSize(getWidth() - editTextButton->getWidth(), editValsTextField->getHeight());
    editValsTextField->setSize(numSliders * sliders[0]->getWidth(), editValsTextField->getHeight());
    
    /*
    String newparams = textWindowDialog();
    if(newparams != "cancelled")
    {
        //Array<Array<float>> newvals = stringToArrayFloatArray(newparams); //for transposition lists, with [] vals
        Array<float> newvals = stringToFloatArray(newparams);

        numSliders = newvals.size();
        if(numSliders < 1) numSliders = 1;

        for(int i=0; i<numSliders; i++)
        {
            if(i >= sliders.size()) addSubSlider();
            
            if(sliders[i]->getMaximum() < newvals[i]) sliders[i]->setRange(sliderMin, newvals[i], sliderIncrement);
            if(sliders[i]->getMinimum() > newvals[i]) sliders[i]->setRange(newvals[i], sliderMax, sliderIncrement);
            
            sliders[i]->setValue(newvals[i]);
            //DBG("new array val " + String(newvals[i]));
        }
        
        if(sliders.size() > numSliders) sliders.removeRange(numSliders, sliders.size());
        
        resetRanges();
        initSizes();
        incDecSlider->setValue(numSliders, dontSendNotification);
    }
     */
    
}

Array<float> BKMultiSlider::getAllValues()
{
    Array<float> currentvals;
    currentvals.ensureStorageAllocated(sliders.size());
    
    for(int i=0; i< sliders.size(); i++)
    {
        currentvals.set(i, sliders[i]->getValue());
    }
    
    return currentvals;
}


