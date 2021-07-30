/*
  ==============================================================================

    BKAbsoluteKeyboardSlider.h
    Created: 29 Jul 2018 4:40:55pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"
#include "BKComponent.h"
#include "BKListener.h"
#include "BKKeyboard.h"
#include "BKKeyboardState.h"

#include "BKSlider.h"

#include "BKUIComponents.h"

class BKAbsoluteKeyboardSlider :
public BKComponent,
public BKListener,
public BKKeymapKeyboardStateListener
#if JUCE_IOS
, private juce::Slider::Listener,
public WantsBigOne
#endif
{
    
public:
    
    BKAbsoluteKeyboardSlider(bool toggles, bool needsOctaveSlider = false);
    ~BKAbsoluteKeyboardSlider()
    {
        keyboard = nullptr;
        setLookAndFeel(nullptr);
    };
    
    void resized() override;
    void paint (Graphics&) override;
    
    class Listener
    {
        
    public:
        virtual ~Listener() {};
        
        virtual void keyboardSliderChanged(String name, Array<float> values) = 0;
        
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener); }
    void removeMyListener(Listener* listener)  { listeners.remove(listener); }
    
    inline void setText(String text)
    {
        keyboardValueTF.setText(text, false);
    }
    
    inline TextEditor* getTextEditor(KSliderTextFieldType which)
    {
        if (which == KSliderAllValues) return keyboardValsTextField.get();
        if (which == KSliderThisValue) return &keyboardValueTF;
        
        return nullptr;
    }
    
    inline void dismissTextEditor(TextEditor* which, bool setValue = false)
    {
        if (setValue)
        {
            textEditorReturnKeyPressed(*which);
        }
        else
        {
            textEditorEscapeKeyPressed(*which);
        }
    }
    
    void setName(String newName) { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName() { return sliderName; }
    
    void setAvailableRange(int min, int max);
    void setValues(Array<float> newvals);
    
    void setMinMidMaxValues(float min, float mid, float max, int resolution)
    {
        if (min > mid || min > max || mid > max) {
            DBG("min must be < mid must be < max");
            return;
        }
        
        minRange = min;
        midRange = mid;
        maxRange = max;
        displayResolution = resolution;
        
        keyboard->setMinMidMaxValues(min, mid, max);
    }
    
    void setOctaveForMiddleC(int octave) { keyboard->setOctaveForMiddleC(octave);};

    inline void setDimensionRatio(float r) { ratio = r; }
    
    inline Rectangle<float> getEditAllBounds(void) { return keyboardValsTextFieldOpen.getBounds().toFloat();}
    
    void setDim(float newalpha)
    {
        showName.setAlpha(newalpha);
        keyboard->setAlpha(newalpha);
        keyboardValsTextFieldOpen.setAlpha(newalpha);
        keyboardValueTF.setAlpha(newalpha);
    }
    
    void setBright() { setDim(1.); }
    
private:
    
    String sliderName;
    BKLabel showName;

    
    bool needsOctaveSlider;
#if JUCE_IOS
    Slider octaveSlider;
    void sliderValueChanged     (Slider* slider)                override;
    BKButtonAndMenuLAF laf;
#endif
    
    float ratio;

    BKTextEditor keyboardValueTF;
    BKKeymapKeyboardState keyboardState;
    std::unique_ptr<Component> keyboardComponent;
    BKKeymapKeyboardComponent* keyboard;
    std::unique_ptr<BKTextEditor> keyboardValsTextField;
    TextButton keyboardValsTextFieldOpen;
    
    int keyboardSize, minKey, maxKey;
    int lastKeyPressed;
    
    float midRange;
    float minRange;
    float maxRange;
    int displayResolution; // how many decimal points
    
    void setActiveValsFromString(String s);
    void setActiveValsFromStringWithFundamentalOffset(String s);

    
    void handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber) override {};
    void handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber) override {};
    void handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    
    void bkMessageReceived (const String& message) override {};
    void bkComboBoxDidChange (ComboBox*) override {};
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& textEditor) override;
    void bkTextFieldDidChange (TextEditor& txt) override;
    void bkButtonClicked (Button* b) override;
    void mouseMove(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent& e) override;
    
    bool focusLostByEscapeKey;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAbsoluteKeyboardSlider)
};



