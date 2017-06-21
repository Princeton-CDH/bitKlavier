/*
  ==============================================================================

    BKLookAndFeel.h
    Created: 22 Nov 2016 10:43:57pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKLOOKANDFEEL_H_INCLUDED
#define BKLOOKANDFEEL_H_INCLUDED

#include "BKUtilities.h"

class BKTextFieldLAF : public LookAndFeel_V3
{
public:
    
    BKTextFieldLAF()
    {
        setColour(TextEditor::backgroundColourId,   Colours::goldenrod);
        setColour(TextEditor::textColourId,         Colours::black);
        
    }
};

class BKButtonAndMenuLAF : public LookAndFeel_V4
{
public:
    
    BKButtonAndMenuLAF()
    {
        setColour(ComboBox::backgroundColourId, Colours::black);
        setColour(ComboBox::textColourId, Colours::white);
        setColour(ComboBox::buttonColourId, Colours::black);

        setColour(PopupMenu::backgroundColourId, Colours::black);
        setColour(PopupMenu::textColourId, Colours::white);
        
        setColour(TextButton::buttonColourId, Colours::black);
        setColour(TextButton::textColourOffId, Colours::white);
        setColour(TextButton::textColourOnId, Colours::white);
        
        setColour(TextEditor::backgroundColourId, Colours::black);
        setColour(TextEditor::textColourId, Colours::white);
        
        setColour(Label::textColourId, Colours::white);
        
        //setColour(juce::LookAndFeel_V4::ColourScheme::outline, Colours::black);
        //getCurrentColourScheme().setUIColour(juce::LookAndFeel_V4::ColourScheme::widgetBackground, Colours::yellow);
        
        
    }
};

class BKMultiSliderLookAndFeel : public BKButtonAndMenuLAF
{
    
public:
    
    BKMultiSliderLookAndFeel()
    {
        setColour(Slider::backgroundColourId, Colours::darkgrey.withMultipliedAlpha(0.25));
        setColour(Slider::textBoxOutlineColourId, Colours::darkgrey.withMultipliedAlpha(0.1));
    }
    ~BKMultiSliderLookAndFeel() {}
    
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style, Slider& slider) override;
};

class BKRangeMinSliderLookAndFeel : public BKButtonAndMenuLAF
{
    
public:
    
    BKRangeMinSliderLookAndFeel()
    {
        //setColour (TextButton::buttonColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    }
    ~BKRangeMinSliderLookAndFeel() {}
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style, Slider& slider) override;
};

class BKRangeMaxSliderLookAndFeel : public BKButtonAndMenuLAF
{
    
public:
    
    BKRangeMaxSliderLookAndFeel()
    {
        //setColour (TextButton::buttonColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
    }
    ~BKRangeMaxSliderLookAndFeel() {}
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style, Slider& slider) override;
};

#endif  // BKLOOKANDFEEL_H_INCLUDED
