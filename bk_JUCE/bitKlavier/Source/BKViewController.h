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
    BigOne(void):
    space("Space"),
    ok("Ok"),
    lb("["),
    rb("]"),
    neg("-")
    {
        
        addAndMakeVisible(bigOne);
        bigOne.setSelectAllWhenFocused(true);
        bigOne.setName("bigOne");
        bigOne.addListener(this);
        bigOne.setColour(TextEditor::ColourIds::backgroundColourId, Colours::black.withAlpha(0.9f));
        bigOne.setMultiLine(true);
        
        bigOne.setKeyboardType(TextInputTarget::VirtualKeyboardType::decimalKeyboard);
        bigOne.setInputRestrictions(10000, "0123456789 []()-.");
        bigOne.setSelectAllWhenFocused(true);
        
        addAndMakeVisible(bigOneLabel);
        bigOneLabel.setName("bigOneLabel");
        bigOneLabel.setColour(TextEditor::ColourIds::backgroundColourId, Colours::black.withAlpha(0.9f));
        bigOneLabel.setColour(TextEditor::ColourIds::textColourId, Colours::antiquewhite);

        bigOneLabel.setJustificationType(Justification::centredLeft);
        
        addAndMakeVisible(space);
        space.addListener(this);
        space.setWantsKeyboardFocus(false);
        
        addAndMakeVisible(ok);
        ok.addListener(this);
        ok.setWantsKeyboardFocus(false);
        
        addAndMakeVisible(lb);
        lb.addListener(this);
        lb.setWantsKeyboardFocus(false);
        
        addAndMakeVisible(rb);
        rb.addListener(this);
        rb.setWantsKeyboardFocus(false);
        
        addAndMakeVisible(neg);
        neg.addListener(this);
        neg.setWantsKeyboardFocus(false);
    
    }
    
    ~BigOne()
    {
        
    }
    
    void bkButtonClicked(Button* button) override
    {
        String text = bigOne.getText();
 
        DBG("text: " + text);
        
        String insert = "";
        
        if (button == &ok)
        {
            setAndHide();
        }
        else
        {
            if (button == &space)       insert = " ";
            else if (button == &neg)    insert = "-";
            else if (button == &lb)     insert = "[";
            else if (button == &rb)     insert = "]";
        
            bigOne.insertTextAtCaret(insert);

            DBG("new text: " +  bigOne.getText());
            
            // Give focus back to keyboard
            bigOne.grabKeyboardFocus();
        }
    }
    
    void bkTextFieldTextEntered (TextEditor& tf) override
    {
        if (tf.getName() == "bigOne")
        {
            setText(tf.getText());
        }
    }
    
    void setAndHide(void)
    {
        target->setText(bigOne.getText(), sendNotification);
        hide();
    }
    
    void bkTextFieldDidChange   (TextEditor& tf) override
    {
        if (tf.getName() == "bigOne")
        {
            setAndHide();
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
        int perRow = 2, perCol = 3;
        int s = 2;
        int buttonDim = (getHeight() * 0.4f)  / perCol - s;
    
        bigOneLabel.setBounds(getWidth() * 0.05 * 0.5, getHeight() * 0.05 * 0.5, getWidth() * 0.95, getHeight() * 0.05);
        
        bigOne.setBounds(bigOneLabel.getX(), bigOneLabel.getBottom(), bigOneLabel.getWidth() - perRow * (buttonDim - gXSpacing), getHeight() * 0.4f);
        
        lb.setBounds(bigOne.getRight() + gXSpacing, bigOne.getY(), buttonDim, buttonDim);
        rb.setBounds(lb.getRight() + s, lb.getY(), buttonDim, buttonDim);
        
        space.setBounds(lb.getX(), lb.getBottom()+s, buttonDim*2, buttonDim);
        
        neg.setBounds(lb.getX(), space.getBottom()+s, buttonDim, buttonDim);
        ok.setBounds(rb.getX(), neg.getY(), buttonDim, buttonDim);
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
    
    inline void display(TextEditor* tf, String name, Rectangle<int> rect)
    {
        target = tf;
        
        setBounds(rect);
        
        setVisible(true);
        toFront(true);
        
        bigOneLabel.setText(name, dontSendNotification);
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
    
    BKTextButton neg, space, ok, lb, rb;
    
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

