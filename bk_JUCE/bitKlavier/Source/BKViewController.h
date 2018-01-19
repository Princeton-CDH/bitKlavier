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
    BigOne(BKAudioProcessor& p):
    space("Space"),
    ok("Ok"),
    lb("["),
    rb("]"),
    neg("-"),
    colon(":"),
    processor(p)
    {
        addAndMakeVisible(bigOne);
        bigOne.setSelectAllWhenFocused(true);
        bigOne.setName("bigOne");
        bigOne.addListener(this);
        bigOne.setColour(TextEditor::ColourIds::backgroundColourId, Colours::black.withAlpha(0.9f));
        bigOne.setMultiLine(true);
        
        bigOne.setKeyboardType(TextInputTarget::VirtualKeyboardType::decimalKeyboard);
        bigOne.setInputRestrictions(10000, "0123456789 :[]()-.");
        bigOne.setSelectAllWhenFocused(true);
        bigOne.setMouseClickGrabsKeyboardFocus(true);
        
        addAndMakeVisible(bigOneLabel);
        bigOneLabel.setName("bigOneLabel");
        bigOneLabel.setColour(TextEditor::ColourIds::backgroundColourId, Colours::black.withAlpha(0.9f));
        bigOneLabel.setColour(TextEditor::ColourIds::textColourId, Colours::antiquewhite);

        bigOneLabel.setJustificationType(Justification::centredLeft);
        
        addAndMakeVisible(ok);
        ok.addListener(this);
        ok.setWantsKeyboardFocus(false);
        
        addAndMakeVisible(lb);
        lb.addListener(this);
        lb.setWantsKeyboardFocus(false);
        
        addAndMakeVisible(rb);
        rb.addListener(this);
        rb.setWantsKeyboardFocus(false);
        
        if (processor.updateState->needsExtraKeys)
        {
            addAndMakeVisible(space);
            space.addListener(this);
            space.setWantsKeyboardFocus(false);
            
            addAndMakeVisible(neg);
            neg.addListener(this);
            neg.setWantsKeyboardFocus(false);
            
            addAndMakeVisible(colon);
            colon.addListener(this);
            colon.setWantsKeyboardFocus(false);
            
        }
    }
    
    ~BigOne()
    {
        
    }
    
    class Listener
    {
        
    public:
        virtual ~Listener() {};
        
        virtual void bigOneDismissed(void) {};
    };
    
    void addBigOneListener(BigOne::Listener* listener)
    {
        listeners.add(listener);
    }
    
    void removeBigOneListener(BigOne::Listener* listener)
    {
        listeners.remove(listener);
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
            else if (button == &colon)  insert = ":";
        
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
        int xoff = getWidth() * 0.025 * 0.5;
    
        bigOneLabel.setBounds(xoff, getHeight()*0.025*0.5, getWidth() - 2*xoff, getHeight() * 0.05);

        int width = bigOneLabel.getWidth();
        
        if (processor.updateState->needsExtraKeys)
        {
            int buttonDim = (getHeight() * 0.42f)  / perCol;
            
            width -= (perRow * (buttonDim + s));
            
            bigOne.setBounds(bigOneLabel.getX(), bigOneLabel.getBottom(), width, (buttonDim +s) * perCol);
            
            lb.setBounds(bigOne.getRight() + s, bigOne.getY(), buttonDim, buttonDim);
            rb.setBounds(lb.getRight() + s, lb.getY(), buttonDim, buttonDim);
            
            neg.setBounds(lb.getX(), lb.getBottom()+s, buttonDim, buttonDim);
            colon.setBounds(neg.getRight()+s, neg.getY(), buttonDim, buttonDim);
            
            space.setBounds(neg.getX(), neg.getBottom()+s, buttonDim, buttonDim);
            ok.setBounds(space.getRight()+s, space.getY(), buttonDim, buttonDim);
        }
        else
        {
            int buttonDim = (getHeight() * 0.365f)  / perCol;
            
            bigOne.setBounds(bigOneLabel.getX(), bigOneLabel.getBottom(), width - buttonDim - 2*s, (buttonDim +s) * perCol);
            
            lb.setBounds(bigOne.getRight() + s, bigOne.getY(), buttonDim, buttonDim);
            rb.setBounds(lb.getX(), lb.getBottom()+gYSpacing, buttonDim, buttonDim);
            ok.setBounds(rb.getX(), rb.getBottom()+gYSpacing, buttonDim, buttonDim);
        }
        
    }
    
    void setText(String text)
    {
        int len = text.length();
        float size = 300.0f/len;
        DBG("len size: " + String(len) + " " + String(size));
        
        //bigOne.setFont(Font(size > 15.0f ? size : 15.0f));
        
        bigOne.setFont(Font((len > 75) ? 16 : 25));
        
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
        listeners.call(&BigOne::Listener::bigOneDismissed);
        setVisible(false);
    }
    
protected:
    TextEditor* target;
    
    BKTextButton neg, space, ok, lb, rb, colon;
    TextEditor bigOne;
    Label bigOneLabel;
    
private:
    ListenerList<BigOne::Listener> listeners;
    
    BKAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BigOne)
};

class BKViewController    : public BKComponent, public BKListener, public BigOne::Listener
{
public:
    BKViewController(BKAudioProcessor& p, BKItemGraph* theGraph);
    
    ~BKViewController();
    
    PopupMenu getPrepOptionMenu(void);
    
    PopupMenu getModOptionMenu(void);
    
    inline void bigOneDismissed(void) override
    {
        hideOrShow.setAlwaysOnTop(true);
    }
    
protected:
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    BigOne bigOne;
    
    BKTextButton actionButton;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    BKButtonAndMenuLAF buttonsAndMenusLAF2;

    BKExitButton hideOrShow;
    ImageComponent iconImageComponent;
    
    

    int lastId;
    
private:
    BKRangeSliderType rangeType;
    NostalgicParameterType wdutType;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKViewController)
};

