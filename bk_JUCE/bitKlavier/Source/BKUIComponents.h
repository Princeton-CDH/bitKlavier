/*
  ==============================================================================

    BKButton.h
    Created: 13 Nov 2017 1:46:54pm
    Author:  airship

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"
#include "BKLookAndFeel.h"

class BKStackedSlider;
class BKSingleSlider;
class BKMultiSlider;
class BKRangeSlider;
class BKWaveDistanceUndertowSlider;
class BKKeyboardSlider;
class BKTextEditor;

typedef enum BKRangeSliderType
{
    BKRangeSliderMin = 0,
    BKRangeSliderMax,
    BKRangeSliderNil
    
} BKRangeSliderType;

typedef enum BKADSRSliderType
{
    BKADSRAttackSlider = 0,
    BKADSRDecaySlider,
    BKADSRSustainSlider,
    BKADSRReleaseSlider,
    BKADSRSliderNil
    
} BKADSRSliderType;

typedef enum KSliderTextFieldType
{
    KSliderAllValues,
    KSliderThisValue,
    KSliderTextFieldTypeNil,
} KSliderTextFieldType;

typedef enum BKMultiSliderType {
    HorizontalMultiSlider = 0,
    VerticalMultiSlider,
    HorizontalMultiBarSlider,
    VerticalMultiBarSlider,
    BKMultiSliderTypeNil
} BKMultiSliderType;

class WantsBigOne
{
public:
    
    WantsBigOne(void)
    {
        
    }
    
    virtual ~WantsBigOne(void)
    {
        listeners.clear();
    }
    
    class Listener
    {
        
    public:
        virtual ~Listener() {};
        
        virtual void iWantTheBigOne(TextEditor*, String name) {};
    };
    
    bool hasBigOne;
    
    void addWantsBigOneListener(WantsBigOne::Listener* listener)
    {
        listeners.add(listener);
    }
    
    void removeWantsBigOneListener(WantsBigOne::Listener* listener)
    {
        listeners.remove(listener);
    }

    
protected:
    
    ListenerList<WantsBigOne::Listener> listeners;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WantsBigOne)
};

class BKTextEditor : public TextEditor
{
public:
    BKTextEditor(void):
    TextEditor()
    {
        Font font;
        
        font.setSizeAndStyle(fontHeight, 0, 0.75, 0.25);
        
        setFont(font);
#if JUCE_IOS
        setKeyboardType(TextInputTarget::VirtualKeyboardType::textKeyboard);
        setInputRestrictions(10000, "0123456789 []()-.:");
#endif
        
    }
    
    ~BKTextEditor(void){}
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKTextEditor)
    
};

class BKExitButton : public TextButton
{
public:
    BKExitButton(void):
    TextButton(),
    bright(true),
    hover(false)
    {
        placement = RectanglePlacement::centred;
        
        image = ImageCache::getFromMemory(BinaryData::bitKlavier_x_png, BinaryData::bitKlavier_x_pngSize);
    }
    
    ~BKExitButton(void)
    {
        
    }
    
    void paint(Graphics& g) override
    {
        float brightness = (bright ? 0.8f : 0.4f) * (hover ? 1.25f : 1.0f);
        g.setOpacity (brightness);
        g.drawImage (image, getLocalBounds().toFloat(), placement);
    }
    
    void resized(void) override
    {
        image.rescaled((getWidth() > 2) ? getWidth() : 2, (getHeight() > 2) ? getHeight() : 2);
    }
private:
    
    /*
    void mouseDown(const MouseEvent& e) override
    {
        bright = false;
        repaint();
    }
    
    void mouseUp(const MouseEvent& e) override
    {
        bright = true;
        repaint();
    }
    
    void mouseExit(const MouseEvent& e) override
    {
        bright = true;
        hover = false;
        repaint();
    }
    
    void mouseEnter(const MouseEvent& e) override
    {
        hover = true;
        repaint();
    }
     */
    
    Image image;
    RectanglePlacement placement;
    
    bool bright;
    bool hover;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKExitButton)
    
};

class BKTextButton : public TextButton
{
public:
    BKTextButton(String text = ""):
    TextButton(text)
    {
        setLookAndFeel(&laf);
    }

    
    ~BKTextButton(void)
    {
        setLookAndFeel(nullptr);
    }
    
private:
    
    BKButtonAndMenuLAF laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKTextButton)
    
};
