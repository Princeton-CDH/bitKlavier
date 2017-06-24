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
    
    BKButtonAndMenuLAF() :
    comboBoxJustification (Justification::centredLeft)
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
        
        toggleTextToRight = true;
        
        //setColour(juce::LookAndFeel_V4::ColourScheme::outline, Colours::black);
        //getCurrentColourScheme().setUIColour(juce::LookAndFeel_V4::ColourScheme::widgetBackground, Colours::yellow);
        
    }
    
    void positionComboBoxText (ComboBox& box, Label& label) override;
    void drawComboBox (Graphics& g, int width, int height, bool,
                                       int, int, int, int, ComboBox& box) override;
    
    void drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                                            const bool isSeparator, const bool isActive,
                                            const bool isHighlighted, const bool isTicked,
                                            const bool hasSubMenu, const String& text,
                                            const String& shortcutKeyText,
                                            const Drawable* icon, const Colour* const textColourToUse) override;
    
    void setComboBoxJustificationType (Justification justification)    { comboBoxJustification = justification; }
    void setToggleBoxTextToRightBool (bool ttr)    { toggleTextToRight = ttr; }
    
    void drawToggleButton (Graphics& g, ToggleButton& button,
                                               bool isMouseOverButton, bool isButtonDown) override;
    
private:
    Justification comboBoxJustification;
    bool toggleTextToRight;
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
        setColour(Slider::thumbColourId, Colours::goldenrod);
        setColour(Slider::backgroundColourId, Colours::goldenrod.withMultipliedAlpha(0.1));
        setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.35));
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
        setColour(Slider::thumbColourId, Colours::goldenrod);
        setColour(Slider::backgroundColourId, Colours::goldenrod.withMultipliedAlpha(0.1));
        setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.35));
        //trackColourId
    }
    ~BKRangeMaxSliderLookAndFeel() {}
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style, Slider& slider) override;
};

#endif  // BKLOOKANDFEEL_H_INCLUDED
