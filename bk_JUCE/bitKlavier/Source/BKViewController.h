/*
  ==============================================================================

    BKViewController.h
    Created: 20 Jun 2017 6:00:55pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#pragma once

#define AUTO_DELETE 0

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKListener.h"
#include "BKComponent.h"

#include "BKGraph.h"

#include "BKSlider.h"
#include "BKKeyboardSlider.h"

class BigOne : public Component, public BKListener
{
public:
    BigOne(void)
    {
        
        addAndMakeVisible(bigOne);
        bigOne.setSelectAllWhenFocused(true);
        bigOne.setName("bigOne");
        bigOne.addListener(this);
        bigOne.setColour(TextEditor::ColourIds::backgroundColourId, Colours::black.withAlpha(0.9f));
        bigOne.setMultiLine(true);
        
        bigOne.setKeyboardType(TextInputTarget::VirtualKeyboardType::textKeyboard);
        bigOne.setInputRestrictions(10000, "0123456789 []()-.");
        
        
        addAndMakeVisible(bigOneLabel);
        bigOneLabel.setName("bigOneLabel");
        bigOneLabel.setColour(TextEditor::ColourIds::backgroundColourId, Colours::black.withAlpha(0.9f));
        bigOneLabel.setColour(TextEditor::ColourIds::textColourId, Colours::antiquewhite);

        bigOneLabel.setJustificationType(Justification::centred);
    }
    
    ~BigOne()
    {
        
    }
    
    void bkTextFieldTextEntered (TextEditor& tf) override
    {
        if (tf.getName() == "bigOne")
        {
            setText(tf.getText());
        }
    }
    
    void bkTextFieldDidChange   (TextEditor& tf) override
    {
        if (tf.getName() == "bigOne")
        {
            target->setText(tf.getText(), sendNotification);
            hide();
        }
    };
    
    void paint(Graphics& g) override
    {
        g.fillAll(Colours::black);
        g.setColour(Colours::antiquewhite);
        g.drawRoundedRectangle(getBounds().toFloat(), 2,2);
        
        
    }
    
    void resized(void) override
    {
    }
    
    void setText(String text)
    {
        int len = text.length();
        float size = 300.0f/len;
        DBG("len size: " + String(len) + " " + String(size));
        
        //bigOne.setFont(Font(size > 15.0f ? size : 15.0f));
        bigOne.setFont(Font(20));
        
        bigOne.setJustification(juce::Justification::centredLeft);
        
        bigOne.setText(text, dontSendNotification);
    }
    
    inline void display(TextEditor* tf, Rectangle<int> rect)
    {
        target = tf;
        
        setBounds(rect);
        
        bigOneLabel.setBounds(getWidth() * 0.05 * 0.5, getHeight() * 0.05 * 0.5, getWidth() * 0.95, getHeight() * 0.05);
        bigOne.setBounds(bigOneLabel.getX(), bigOneLabel.getBottom(), bigOneLabel.getWidth(), getHeight() * 0.4f);
        
        setVisible(true);
        toFront(true);
        
        bigOneLabel.setText(target->getName(), dontSendNotification);
        bigOneLabel.toFront(true);
        
        bigOne.toFront(true);
        
        setText(target->getText());
        
        bigOne.grabKeyboardFocus();
    }
    
    inline void hide(void)
    {
        setVisible(false);
    }
    
protected:
    TextEditor* target;
    
    TextEditor bigOne;
    Label bigOneLabel;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BigOne)
};

class BKViewController    : public BKComponent, public BKListener
{
public:
    BKViewController(BKAudioProcessor& p, BKItemGraph* theGraph);
    
    ~BKViewController();
    
    PopupMenu getPrepOptionMenu(void);
    
    PopupMenu getModOptionMenu(void);
    
protected:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    BigOne bigOne;
    
    BKTextButton actionButton;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    BKButtonAndMenuLAF buttonsAndMenusLAF2;

    
    ImageComponent iconImageComponent;
    BKExitButton hideOrShow;

    int lastId;
    
private:
    BKRangeSliderType rangeType;
    NostalgicParameterType wdutType;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKViewController)
};

