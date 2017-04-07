/*
  ==============================================================================

    BKSlider.h
    Created: 6 Apr 2017 9:50:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef BKSLIDER_H_INCLUDED
#define BKSLIDER_H_INCLUDED

#include "BKUtilities.h"
#include "BKComponent.h"

//class BKMultiSlider
//class BKSingleSlider

class BKSingleSlider : public Slider
{
public:
    BKSingleSlider ();
    ~BKSingleSlider();

    void valueChanged() override;
    double getValueFromText	(const String & text ) override;
    
    /*
    void mouseDown(const MouseEvent &e) override
    {
        for (auto listener : listeners)
        {
            listener->sliderMouseDown(e);
        }
    }
    
    void mouseDrag(const MouseEvent &e) override
    {
        for (auto listener : listeners)
        {
            listener->sliderMouseDrag(e);
        }
    }
    
    class MyListener
    {
    public:
        virtual ~MyListener() {}
        
        virtual void sliderMouseDown(const MouseEvent& e){};
        
        virtual void sliderMouseDrag(const MouseEvent& e){};
        
    private:
    };
    
    void addMyListener(MyListener* listener)
    {
        listeners.add(listener);
    }
    
    void removeMyListener(MyListener* listener)
    {
        for (int i = listeners.size(); --i >= 0; i++)
        {
            if (listeners[i] == listener)
            {
                listeners.remove(i);
                break;
            }
        }
        
    }
    */
    
private:
    
    /*OwnedArray<MyListener> listeners;*/

    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)
};



class BKMultiSlider : public BKComponent, public Slider::Listener /*, public BKSingleSlider::MyListener*/
{
    
public:
    
    BKMultiSlider()
    {
        
        
        for(int i=0; i<numSliders; i++) {
            BKSingleSlider* newslider = new BKSingleSlider();
            newslider->addListener(this);
            //newslider->addMyListener(this);
            //newslider->addMouseListener(this, true);
            sliders.add(newslider);
            addAndMakeVisible(newslider);
        }
        
        bigInvisibleSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0,0);
        bigInvisibleSlider.setAlpha(0.0);
        bigInvisibleSlider.addMouseListener(this, true);
        bigInvisibleSlider.setName("BIG");
        bigInvisibleSlider.addListener(this);
        
        addAndMakeVisible(bigInvisibleSlider);
        
        setSize(numSliders*sliders[0]->getWidth(), sliders[0]->getHeight());
    }
    
    ~BKMultiSlider()
    {
        
    }

    void mouseDown(const MouseEvent& e) override
    {
        
        /*
        if (!dragging)
        {
            int x = e.getEventRelativeTo(this).x;
            int y = e.getEventRelativeTo(this).y;
            
            DBG("START " + String(x) + " " + String(y));
            
            
            currentSlider = dynamic_cast<BKSingleSlider*> (getComponentAt(x, y));
            prevSlider = currentSlider;
            
            dragging = true;
        }
         */
    }
    
    void mouseDrag(const MouseEvent& e) override
    {
        int x = e.x;
        
        int which = (e.x / sliders[0]->getWidth());
        DBG("DRAGGING " + String(which) + String(e.x) + " " + String(e.y));
        
        if (which >= 0 && which < sliders.size())
        {
            sliders[which]->setValue(currentInvisibleSliderValue);
            
        }
        /*
        if (dragging)
        {
            int x = e.getEventRelativeTo(this).x;
            int y = e.getEventRelativeTo(this).y;
            
            DBG("DRAGGING " + String(e.getEventRelativeTo(this).x) + " " + String(e.getEventRelativeTo(this).y));
            
            prevSlider = currentSlider;
            currentSlider = dynamic_cast<BKSingleSlider*> (getComponentAt(x, y)->getComponentAt(x, y));
            
            if (currentSlider != prevSlider)
            {
                prevSlider->mouseUp(e);
                currentSlider->mouseDown(e);
            }
        }
         */
    }
    
    void mouseUp(const MouseEvent& e) override
    {
        /*
        if (dragging)
        {
            int x = e.getEventRelativeTo(this).x;
            int y = e.getEventRelativeTo(this).y;
            
            dragging = false;
            
            DBG("DONE DRAGGING");
        }
         */
        
    }
    
    
    void resized() override
    {
        for (int i=0; i<sliders.size(); i++)
        {
            sliders[i]->setTopLeftPosition(50*i, 0);
        }
        
        bigInvisibleSlider.setBounds(0, 0, sliders.getLast()->getRight(), sliders.getLast()->getBottom());
        
    }
    
private:
    bool dragging;
    
    double currentInvisibleSliderValue;
    
    OwnedArray<BKSingleSlider> sliders;
    
    BKSingleSlider bigInvisibleSlider;
    
    
    BKSingleSlider* currentSlider;
    BKSingleSlider* prevSlider;
    
    int numSliders = 10;
    
    void sliderValueChanged (Slider *slider) override
    {
        if (slider->getName() == "BIG")
        {
            currentInvisibleSliderValue = slider->getValue();
            DBG(currentInvisibleSliderValue);
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};


 
#endif  // BKSLIDER_H_INCLUDED
