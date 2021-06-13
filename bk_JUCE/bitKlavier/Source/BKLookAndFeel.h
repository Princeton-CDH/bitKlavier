/*
  ==============================================================================

    BKLookAndFeel.h
    Created: 22 Nov 2016 10:43:57pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKLOOKANDFEEL_H_INCLUDED
#define BKLOOKANDFEEL_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class BKTextFieldLAF : public LookAndFeel_V3
{
public:
    
    BKTextFieldLAF()
    {
        setColour(TextEditor::backgroundColourId,   Colours::goldenrod);
        setColour(TextEditor::textColourId,         Colours::black);
        
        
    }
};

class BKButtonAndMenuLAF : public LookAndFeel_V4, public DeletedAtShutdown
{
public:
    
    BKButtonAndMenuLAF() :
    comboBoxJustification (Justification::centredLeft)
    {
        
        setColour(ComboBox::backgroundColourId, Colours::black);
        setColour(ComboBox::textColourId, Colours::antiquewhite);
        setColour(ComboBox::buttonColourId, Colours::black);
        setColour(ComboBox::outlineColourId, Colours::antiquewhite);
        setColour(ComboBox::arrowColourId, Colours::antiquewhite);
        
        setColour(PopupMenu::backgroundColourId, Colours::black);
        setColour(PopupMenu::textColourId, Colours::antiquewhite);
        
        setColour(TextButton::buttonColourId, Colours::black);
        setColour(TextButton::textColourOffId, Colours::antiquewhite);
        setColour(TextButton::textColourOnId, Colours::antiquewhite);
        setColour(TextEditor::backgroundColourId, Colours::black);
        setColour(TextEditor::textColourId, Colours::antiquewhite);
        setColour(TextEditor::outlineColourId, Colours::antiquewhite);
        
        setColour(Label::textColourId, Colours::antiquewhite);
        
        setColour(Slider::trackColourId, Colours::burlywood.withMultipliedBrightness(0.4));
        setColour(Slider::thumbColourId, Colours::lightgrey);
        setColour(Slider::backgroundColourId, Colours::black);
        setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
        
        setColour(ToggleButton::textColourId, Colours::antiquewhite);
        
        toggleTextToRight = true;
    
        //setColour(juce::LookAndFeel_V4::ColourScheme::outline, Colours::black);
        //getCurrentColourScheme().setUIColour(juce::LookAndFeel_V4::ColourScheme::widgetBackground, Colours::yellow);
        
    }
    
    ~BKButtonAndMenuLAF()
    {
//         setDefaultLookAndFeel(nullptr);
    }
    
    void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;
    
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
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                               float sliderPos,
                                               float minSliderPos,
                                               float maxSliderPos,
                                                   const Slider::SliderStyle style, Slider& slider) override;
    
    void drawToggleButton (Graphics& g, ToggleButton& button,
                                               bool isMouseOverButton, bool isButtonDown) override;
    
    Font getComboBoxFont (ComboBox&) override;
    Font getPopupMenuFont (void) override;
    Font getTextButtonFont (TextButton&, int buttonHeight)  override;
    Font getLabelFont (Label&) override;
    int getDefaultMenuBarHeight() override;
    void drawTooltip(Graphics& g, const String& text, int width, int height) override;
    
private:
    Justification comboBoxJustification;
    bool toggleTextToRight;
};

class BKWindowLAF : public BKButtonAndMenuLAF
{
public:
    
    BKWindowLAF()
    {
        setColour(ResizableWindow::backgroundColourId,   Colours::black);
        
        setColour(ListBox::textColourId, Colours::antiquewhite);
        setColour(ListBox::backgroundColourId, Colours::black);
        setColour(ListBox::outlineColourId, Colours::antiquewhite.withAlpha(0.6f));
        
    }
    
    Button* createDocumentWindowButton (int buttonType) override;
    void drawDocumentWindowTitleBar (DocumentWindow& window, Graphics& g,
                                     int w, int h, int titleSpaceX, int titleSpaceW,
                                     const Image* icon, bool drawTitleTextOnLeft) override;
    
    Font getComboBoxFont (ComboBox&) override;
    Font getPopupMenuFont (void) override;
    Font getTextButtonFont (TextButton&, int buttonHeight)  override;
    Font getLabelFont (Label&) override;
    
    void drawCallOutBoxBackground (CallOutBox& box, Graphics& g,
                                   const Path& path, Image& cachedImage) override;
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

class BKDisplaySliderLookAndFeel :  public BKButtonAndMenuLAF
{
    
public:
    
    BKDisplaySliderLookAndFeel()
    {
        setColour(Slider::trackColourId, Colours::deepskyblue.withMultipliedBrightness(0.75));
        setColour(Slider::thumbColourId, Colours::deepskyblue.withMultipliedAlpha(0.75));
        setColour(Slider::backgroundColourId, Colours::deepskyblue.withMultipliedAlpha(0.));
        setColour(Slider::textBoxTextColourId, Colours::deepskyblue.withMultipliedAlpha(0.));
    }
    ~BKDisplaySliderLookAndFeel() {}
    
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
