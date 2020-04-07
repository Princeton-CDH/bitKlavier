/*
  ==============================================================================

    BKLookAndFeel.cpp
    Created: 20 Jun 2017 4:48:16pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "BKLookAndFeel.h"

void BKButtonAndMenuLAF::positionComboBoxText (ComboBox& box, Label& label)
{
    label.setFont (getComboBoxFont (box));
    
    if(comboBoxJustification == Justification::centredRight)
    {
        label.setBounds (30, 1,
                         box.getWidth() - 30,
                         box.getHeight() - 2);
        
        label.setJustificationType (Justification::centredRight);
    }
    else
    {
        label.setBounds (1, 1,
                         box.getWidth() - 30,
                         box.getHeight() - 2);
        
        label.setJustificationType (Justification::centredLeft);
    }
}

#define HEIGHT_IOS 22
#define HEIGHT 15
#define HORIZONTAL 0.75f
#define KERNING 0.25f
#define STYLE 0
#define IOS_RATIO 1.5f

Font BKButtonAndMenuLAF::getComboBoxFont (ComboBox& cb)
{
    Font font;

    font.setSizeAndStyle(fontHeight, STYLE, HORIZONTAL, KERNING);
    
    return font;
}

Font BKButtonAndMenuLAF::getPopupMenuFont (void)
{
    Font font;

    font.setSizeAndStyle(fontHeight, STYLE, HORIZONTAL, KERNING);
    
    return font;
}

Font BKButtonAndMenuLAF::getTextButtonFont (TextButton&, int buttonHeight)
{
    Font font;

    font.setSizeAndStyle(fontHeight, STYLE, HORIZONTAL, KERNING);
    
    return font;
}

Font BKButtonAndMenuLAF::getLabelFont (Label& label)
{
    Font font;

    font.setSizeAndStyle(fontHeight, STYLE, HORIZONTAL, KERNING);
    
    return font;
}

void BKButtonAndMenuLAF::drawTooltip(Graphics& g, const String& text, int width, int height)
{
    Rectangle<int> bounds (width, height);
    auto cornerSize = 5.0f;
    
    g.setColour (findColour (TooltipWindow::backgroundColourId));
    g.setOpacity(0.0f);
    g.fillRoundedRectangle (bounds.toFloat(), cornerSize);
    
    g.setColour (findColour (TooltipWindow::outlineColourId));
    g.setOpacity(0.0f);
    g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);
    
    const float tooltipFontSize = 13.0f;
    const int maxToolTipWidth = 400;
    
    AttributedString s;
    s.setJustification (Justification::centred);
    s.append (text, Font (tooltipFontSize, Font::bold), findColour (TooltipWindow::textColourId));
    
    TextLayout tl;
    tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
    tl.draw (g, { static_cast<float> (width), static_cast<float> (height) });
}


#define MENUBARHEIGHT_IOS 40
#define MENUBARHEIGHT  25

int BKButtonAndMenuLAF::getDefaultMenuBarHeight()
{
#if JUCE_IOS
    return MENUBARHEIGHT_IOS;
#else
    return MENUBARHEIGHT;
#endif
}

#define CORNER_SIZE 2.0f

void BKButtonAndMenuLAF::drawComboBox (Graphics& g, int width, int height, bool,
                                   int, int, int, int, ComboBox& box)
{
    //const auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 0.0f;
    const auto cornerSize = CORNER_SIZE;
    const Rectangle<int> boxBounds (0, 0, width, height);
    
    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);
    
    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 0.5f);
    
    Rectangle<int> arrowZone (width - 30, 0, 20, height);
    if(comboBoxJustification == Justification::centredRight)
        arrowZone = Rectangle<int> (gXSpacing, 0, 20, height);
    
    Path path;
    path.startNewSubPath (arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
    path.lineTo (static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
    path.lineTo (arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);
    
    g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath (path, PathStrokeType (2.0f));
}

void BKButtonAndMenuLAF::drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                                        const bool isSeparator, const bool isActive,
                                        const bool isHighlighted, const bool isTicked,
                                        const bool hasSubMenu, const String& text,
                                        const String& shortcutKeyText,
                                        const Drawable* icon, const Colour* const textColourToUse)
{
    if (isSeparator)
    {
        auto r  = area.reduced (5, 0);
        r.removeFromTop (roundToInt ((r.getHeight() * 0.5f) - 0.5f));
        
        g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.3f));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        auto textColour = (textColourToUse == nullptr ? findColour (PopupMenu::textColourId)
                           : *textColourToUse);
        
        auto r  = area.reduced (1);
        
        if (isHighlighted && isActive)
        {
            g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
            g.fillRect (r);
            
            g.setColour (findColour (PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
        }
        
        r.reduce (jmin (5, area.getWidth() / 20), 0);
        
        auto font = getPopupMenuFont();
        
        const auto maxFontHeight = r.getHeight() / 1.3f;
        
        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);
        
        g.setFont (font);
        
        auto iconArea = r.removeFromLeft (roundToInt (maxFontHeight)).toFloat();
        
        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
        }
        else if (isTicked)
        {
            const auto tick = getTickShape (1.0f);
            g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
        }
        
        if (hasSubMenu)
        {
            const auto arrowH = 0.6f * getPopupMenuFont().getAscent();
            
            const auto x = (float) r.removeFromRight ((int) arrowH).getX();
            const auto halfH = (float) r.getCentreY();
            
            Path path;
            path.startNewSubPath (x, halfH - arrowH * 0.5f);
            path.lineTo (x + arrowH * 0.6f, halfH);
            path.lineTo (x, halfH + arrowH * 0.5f);
            
            g.strokePath (path, PathStrokeType (2.0f));
        }
        
        r.removeFromRight (3);
        if(comboBoxJustification == Justification::centredRight)
            g.drawFittedText (text, r, Justification::centredRight, 1);
        else
            g.drawFittedText (text, r, Justification::centredLeft, 1);
        
        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);
            
            if(comboBoxJustification == Justification::centredRight)
                g.drawText (shortcutKeyText, r, Justification::centredRight, true);
            else
                g.drawText (shortcutKeyText, r, Justification::centredLeft, true);
        }
    }
}

void BKButtonAndMenuLAF::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                                               bool isMouseOverButton, bool isButtonDown)
{
    const auto cornerSize = CORNER_SIZE;
    const auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);
    
    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
    .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);
    
    if (isButtonDown || isMouseOverButton)
        baseColour = baseColour.contrasting (isButtonDown ? 0.2f : 0.05f);
    
    g.setColour (baseColour);
    
    if (button.isConnectedOnLeft() || button.isConnectedOnRight())
    {
        Path path;
        path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                  bounds.getWidth(), bounds.getHeight(),
                                  cornerSize, cornerSize,
                                  ! button.isConnectedOnLeft(),
                                  ! button.isConnectedOnRight(),
                                  ! button.isConnectedOnLeft(),
                                  ! button.isConnectedOnRight());
        
        g.fillPath (path);
        
        g.setColour (button.findColour (ComboBox::outlineColourId));
        g.strokePath (path, PathStrokeType (0.5f));
    }
    else
    {
        g.fillRoundedRectangle (bounds, cornerSize);
        
        g.setColour (button.findColour (ComboBox::outlineColourId));
        g.drawRoundedRectangle (bounds, cornerSize, 0.5f);
    }
}

void BKButtonAndMenuLAF::drawToggleButton (Graphics& g, ToggleButton& button,
                                       bool isMouseOverButton, bool isButtonDown)
{
    if(toggleTextToRight)
    {
        const auto fontSize = jmin (15.0f, button.getHeight() * 0.75f);
        const auto tickWidth = fontSize * 1.1f;
        
        drawTickBox (g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
                     tickWidth, tickWidth,
                     button.getToggleState(),
                     button.isEnabled(),
                     isMouseOverButton,
                     isButtonDown);
        
        g.setColour (button.findColour (ToggleButton::textColourId));
        g.setFont (fontSize);
        Font toggleFont;
        toggleFont.setSizeAndStyle(fontHeight, STYLE, HORIZONTAL, KERNING);
        g.setFont (toggleFont);
        
        if (! button.isEnabled())
            g.setOpacity (0.5f);
        
        const auto textX = roundToInt (tickWidth) + 10;
        
        g.drawFittedText (button.getButtonText(),
                          textX, 0,
                          button.getWidth() - textX - 2, button.getHeight(),
                          Justification::centredLeft, 10);
    }
    else
    {
        const auto fontSize = jmin (15.0f, button.getHeight() * 0.75f);
        const auto tickWidth = fontSize * 1.1f;
        
        drawTickBox (g, button,
                     (button.getWidth() - tickWidth - 2),
                     (button.getHeight() - tickWidth) * 0.5f,
                     tickWidth, tickWidth,
                     button.getToggleState(),
                     button.isEnabled(),
                     isMouseOverButton,
                     isButtonDown);
        
        g.setColour (button.findColour (ToggleButton::textColourId));
        g.setFont (fontSize);
        Font toggleFont;
        toggleFont.setSizeAndStyle(fontHeight, STYLE, HORIZONTAL, KERNING);
        g.setFont (toggleFont);
        
        if (! button.isEnabled())
            g.setOpacity (0.5f);
        
        const auto textX = roundToInt (tickWidth) + 10;
        
        g.drawFittedText (button.getButtonText(),
                          0,
                          0,
                          button.getWidth() - textX - 2,
                          button.getHeight(),
                          Justification::centredRight, 10);
        
    }
}

void BKButtonAndMenuLAF::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos,
                                       float minSliderPos,
                                       float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                    : Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
    }
    else
    {
        const auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        const auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);
        
        const auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);
        
        const juce::Point<float> startPoint (slider.isHorizontal() ? x : x + width * 0.5f,
                                       slider.isHorizontal() ? y + height * 0.5f : height + y);
        
        const juce::Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                                     slider.isHorizontal() ? startPoint.y : y);
        
        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
        
        Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;
        
        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : width * 0.5f,
                slider.isHorizontal() ? height * 0.5f : minSliderPos };
            
            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : width * 0.5f,
                    slider.isHorizontal() ? height * 0.5f : sliderPos };
            
            maxPoint = { slider.isHorizontal() ? maxSliderPos : width * 0.5f,
                slider.isHorizontal() ? height * 0.5f : maxSliderPos };
        }
        else
        {
            const auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
            const auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;
            
            minPoint = startPoint;
            maxPoint = { kx, ky };
        }
        
        const auto thumbWidth = trackWidth * 2.0f;
        
        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath (valueTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
        
        if (! isTwoVal)
        {
            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (isThreeVal ? thumbPoint : maxPoint));
        }
        
        if (isTwoVal || isThreeVal)
        {
            const auto sr = jmin (trackWidth, (slider.isHorizontal() ? height : width) * 0.4f);
            const auto pointerColour = slider.findColour (Slider::thumbColourId);
            
            if (slider.isHorizontal())
            {
                drawPointer (g, minSliderPos - sr,
                             jmax (0.0f, y + height * 0.5f - trackWidth * 2.0f),
                             trackWidth * 2.0f, pointerColour, 2);
                
                drawPointer (g, maxSliderPos - trackWidth,
                             jmin (y + height - trackWidth * 2.0f, y + height * 0.5f),
                             trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer (g, jmax (0.0f, x + width * 0.5f - trackWidth * 2.0f),
                             minSliderPos - trackWidth,
                             trackWidth * 2.0f, pointerColour, 1);
                
                drawPointer (g, jmin (x + width - trackWidth * 2.0f, x + width * 0.5f), maxSliderPos - sr,
                             trackWidth * 2.0f, pointerColour, 3);
            }
        }
    }
}

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
        
        const juce::Point<float> startPoint (slider.isHorizontal() ? x : width * 0.5f,
                                       slider.isHorizontal() ? height * 0.5f : height + y);
        
        const juce::Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                                     slider.isHorizontal() ? startPoint.y : y);
        
        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
        
        Path valueTrack;
        juce::Point<float> minPoint, maxPoint;
        
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
                     y + height + 1 - trackWidth * 3.0f,
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
        
        const juce::Point<float> startPoint (slider.isHorizontal() ? x : width * 0.5f,
                                       slider.isHorizontal() ? height * 0.5f : height + y);
        
        const juce::Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                                     slider.isHorizontal() ? startPoint.y : y);
        Path valueTrack;
        juce::Point<float> minPoint, maxPoint;
        
        const auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
        const auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;
        
        //maxPoint = endPoint;
        //minPoint = { kx, ky };
        maxPoint = { kx, ky };
        minPoint = startPoint;
        
        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        //g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (valueTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
        
        g.setColour (slider.findColour (Slider::thumbColourId));
        
        const auto pointerColour = slider.findColour (Slider::thumbColourId);
        const auto sr = jmin (trackWidth, (slider.isHorizontal() ? height : width) * 0.4f);
        drawPointer (g, sliderPos - sr,
                     //jmax (0.0f, y + height * 0.5f - trackWidth * 2.0f),
                     //jmax (0.0f, y + height * 0.5f - trackWidth * 4.0f),
                     //y + height - trackWidth *6.0f,
                     y + 4,
                     trackWidth * 2.0f, pointerColour, 2);
        
        
        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        //g.setColour (slider.findColour (Slider::backgroundColourId));
        //g.strokePath (backgroundTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));
    }
}


