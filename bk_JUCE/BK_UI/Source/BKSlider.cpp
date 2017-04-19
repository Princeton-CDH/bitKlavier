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
    else
    {
        //setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 0, 0);
        if(sliderIsVertical) setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
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
    
    setLookAndFeel(&thisLookAndFeel);
    
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
    
    addSubSlider(-1);
    
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
        
        displaySlider = new BKSingleSlider(Slider::LinearBarVertical,
                                           sliderMin,
                                           sliderMax,
                                           sliderDefault,
                                           sliderIncrement,
                                           40,
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
    
    displaySlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0,0);
    displaySlider->addMouseListener(this, true);
    displaySlider->setName("DISPLAY");
    displaySlider->addListener(this);
    
    addAndMakeVisible(displaySlider);
    
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
    if(!arrangedHorizontally) editValsTextField->setSize(getWidth(), sliderHeight);
    else editValsTextField->setSize(sliderWidth, 20);
    editValsTextField->addListener(this);
    
    initSizes();
    
}

BKMultiSlider::~BKMultiSlider()
{
    
}

void BKMultiSlider::addSubSlider(int where)
{
    BKSingleSlider* newslider;
    
    //newslider->setLookAndFeel(&thisLookAndFeel);

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
    
    sliders.insert(where, newslider);
    addAndMakeVisible(newslider);
}

void BKMultiSlider::insertSubSlider(int where)
{
    addSubSlider(where + 1);
    numSliders++;
    initSizes();
}

void BKMultiSlider::deleteSubSlider(int where)
{
    sliders.remove(where);
    numSliders = sliders.size();
    initSizes();
}


void BKMultiSlider::mouseDrag(const MouseEvent& e)
{
    if(e.eventComponent == bigInvisibleSlider)
    {
        int which = whichSubSlider(e);
        if(which >= 0) {
            sliders[which]->setValue(currentInvisibleSliderValue);
            displaySlider->setValue(currentInvisibleSliderValue);
        }
    }
}

void BKMultiSlider::mouseDoubleClick (const MouseEvent &e)
{
    if(e.eventComponent == bigInvisibleSlider)
    {
        int which = whichSubSlider(e);
        //if(which >= 0) sliders[which]->showTextBox();
        //updatingTextBox = true;
        
        //highlight number for current slider
        StringArray tokens;
        tokens.addTokens(floatArrayToString(getAllValues()), false); //arrayFloatArrayToString
        int startPoint = 0;
        int endPoint;
        
        for(int i=0; i < which; i++) startPoint += tokens[i].length() + 1;
        endPoint = startPoint + tokens[which].length();
        
        editValsTextField->setVisible(true);
        editValsTextField->toFront(true);
        editValsTextField->setText(floatArrayToString(getAllValues())); //arrayFloatArrayToString
        if(arrangedHorizontally) editValsTextField->setSize(numSliders * sliders[0]->getWidth(), editValsTextField->getHeight());
        else editValsTextField->setSize(getWidth(), editValsTextField->getHeight());
        
        Range<int> highlightRange(startPoint, endPoint);
        editValsTextField->setHighlightedRegion(highlightRange);
    }
}

void BKMultiSlider::mouseDown (const MouseEvent &event)
{
    if(event.mouseWasClicked())
    {
        if(event.mods.isCtrlDown())
        {
            DBG("control down for slider " + String(whichSubSlider(event)));
            showModifyPopupMenu(whichSubSlider(event));
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
            int which = whichSubSlider(event);
            if(which >= 0) sliders[which]->setValue(sliderDefault);
        }
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
        displaySlider->setRange(sliderMin, sliderMax, sliderIncrement);
    }
}


void BKMultiSlider::resized()
{
    if(arrangedHorizontally)
    {
        //incDecSlider->setTopLeftPosition(0, 0);
        //editTextButton->setTopLeftPosition(0, getHeight() - editTextButton->getHeight());
        //editValsTextField->setTopLeftPosition(editTextButton->getRight(), getHeight() - editTextButton->getHeight());
        displaySlider->setTopLeftPosition(0, 0);
        
        editValsTextField->setTopLeftPosition(displaySlider->getWidth(), (getHeight() - editValsTextField->getHeight()) / 2);
        editValsTextField->setVisible(false);
        
        for (int i=0; i<sliders.size(); i++)
        {
            //sliders[i]->setTopLeftPosition(sliderWidth * i + editTextButton->getWidth(), 0);
            sliders[i]->setTopLeftPosition(sliderWidth * i + displaySlider->getRight(), 0);
        }
        
        if(!sliderIsBar) {
            //bigInvisibleSlider->setBounds(editTextButton->getWidth(), 0, sliders.getLast()->getRight(), sliders.getLast()->getHeight() - sliders.getLast()->getTextBoxHeight());
            bigInvisibleSlider->setBounds(0, 0, sliders.getLast()->getRight(), sliders.getLast()->getHeight() - sliders.getLast()->getTextBoxHeight());
        }
        else
        {
            //bigInvisibleSlider->setBounds(editTextButton->getWidth(), 0, sliders.getLast()->getRight(), sliders.getLast()->getHeight());
            bigInvisibleSlider->setBounds(displaySlider->getWidth(), 0, sliders.getLast()->getRight(), sliders.getLast()->getHeight());
            displaySlider->setBounds(0, 0, displaySlider->getWidth(), displaySlider->getHeight());
        }
    }
    else
    {
        //incDecSlider->setTopLeftPosition((int)(sliderWidth/2. - incDecSlider->getWidth()/2.), 0);
        //incDecSlider->setTopLeftPosition(0, 0);
        //editTextButton->setTopRightPosition(getRight(), 0);
        //editTextButton->setTopLeftPosition(incDecSlider->getRight(), 0);
        editValsTextField->setTopLeftPosition(0, 0);
        editValsTextField->setVisible(false);
        
        for (int i=0; i<sliders.size(); i++)
        {
            //sliders[i]->setTopLeftPosition(0, sliderHeight * i + incDecSlider->getHeight());
            sliders[i]->setTopLeftPosition(0, sliderHeight * i);
        }
        
        if(!sliderIsBar)
            //bigInvisibleSlider->setBounds(sliders.getLast()->getTextBoxWidth(), incDecSlider->getHeight(), sliders.getLast()->getRight() - sliders.getLast()->getTextBoxWidth(), sliders.getLast()->getBottom());
            bigInvisibleSlider->setBounds(sliders.getLast()->getTextBoxWidth(), 0, sliders.getLast()->getRight() - sliders.getLast()->getTextBoxWidth(), sliders.getLast()->getBottom());
        else {
            //bigInvisibleSlider->setBounds(0, incDecSlider->getHeight(), sliders.getLast()->getRight(), sliders.getLast()->getBottom());
            bigInvisibleSlider->setBounds(displaySlider->getWidth(), 0, sliders.getLast()->getRight(), sliders.getLast()->getBottom());
            displaySlider->setBounds(0, 0, sliders.getLast()->getRight(), sliders.getLast()->getBottom());
        }
    }
    
    
}

void BKMultiSlider::initSizes()
{
    if(arrangedHorizontally) {
        bigInvisibleSlider->setSize(numSliders * sliders[0]->getWidth(), bigInvisibleSlider->getHeight());
        //setSize(numSliders * sliders[0]->getWidth() + editTextButton->getWidth(), sliders[0]->getHeight());
        setSize(numSliders * sliders[0]->getWidth() + displaySlider->getWidth(), sliders[0]->getHeight());
        bigInvisibleSlider->toFront(false);
    }
    else
    {
        bigInvisibleSlider->setSize(bigInvisibleSlider->getWidth(), numSliders * sliders[0]->getWidth());
        //if(!sliderIsBar) setSize(sliders[0]->getWidth(), numSliders * sliders[0]->getHeight() + incDecSlider->getHeight());
        if(!sliderIsBar) setSize(sliders[0]->getWidth(), numSliders * sliders[0]->getHeight());
        //else setSize(sliders[0]->getWidth(), numSliders * sliders[0]->getHeight() + incDecSlider->getHeight());
        else setSize(sliders[0]->getWidth(), numSliders * sliders[0]->getHeight());
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
        if(numSliders > sliders.size()) addSubSlider(-1);
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
    
    //Array<float> newvals = stringToFloatArray(textEditor.getText());
    Array<Array<float>> newvals = stringToArrayFloatArray(textEditor.getText());
    
    numSliders = newvals.size();
    DBG("numSliders = " + String(numSliders));
    if(numSliders < 1) numSliders = 1;
    
    for(int i=0; i<numSliders; i++)
    {
        if(i >= sliders.size()) addSubSlider(-1);
        
        if(sliders[i]->getMaximum() < newvals[i][0]) sliders[i]->setRange(sliderMin, newvals[i][0], sliderIncrement);
        if(sliders[i]->getMinimum() > newvals[i][0]) sliders[i]->setRange(newvals[i][0], sliderMax, sliderIncrement);
        
        sliders[i]->setValue(newvals[i][0]);
        DBG("slider depth for " + String(i) + " = " + String(newvals[i].size()));
    }
    
    if(sliders.size() > numSliders) sliders.removeRange(numSliders, sliders.size());
    
    resetRanges();
    initSizes();
    //incDecSlider->setValue(numSliders, dontSendNotification);
}


void BKMultiSlider::buttonClicked (Button* button)
{
    //setSize(10 * sliders[0]->getWidth() + editTextButton->getWidth(), sliders[0]->getHeight());
    editValsTextField->setVisible(true);
    editValsTextField->toFront(true);
    editValsTextField->setText(floatArrayToString(getAllValues())); //arrayFloatArrayToString
    
    //editValsTextField->setSize(getWidth() - editTextButton->getWidth(), editValsTextField->getHeight());
    if(arrangedHorizontally) editValsTextField->setSize(numSliders * sliders[0]->getWidth(), editValsTextField->getHeight());
    else editValsTextField->setSize(getWidth(), editValsTextField->getHeight());    
}

//need to change to arrayFloatArrayToString
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


void BKMultiSlider::showModifyPopupMenu(int which) 
{
    PopupMenu m;
    m.setLookAndFeel (&getLookAndFeel());
    m.addItem (1, translate ("insert slider before"), true, false);
    m.addItem (2, translate ("insert slider after"), true, false);
    m.addItem (3, translate ("delete slider"), true, false);
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
            case 1:   ms->insertSubSlider(which - 1); break;
            case 2:   ms->insertSubSlider(which); break;
            case 3:   ms->deleteSubSlider(which); break;

            default:  break;
        }
    }
}

