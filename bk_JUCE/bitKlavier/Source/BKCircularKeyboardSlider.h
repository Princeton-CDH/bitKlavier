/*
  ==============================================================================

    BKCircularKeyboardSlider.h
    Created: 29 Jul 2018 2:10:31pm
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

class BKCircularKeyboardSlider :
public BKComponent,
public BKListener,
public BKKeymapKeyboardStateListener
#if JUCE_IOS
, public WantsBigOne
#endif
{
    
public:
    
    BKCircularKeyboardSlider(void);
    ~BKCircularKeyboardSlider(void)
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
    
    void setFundamental(int fund);
    
    void setAllValues(Array<float> newvals);
    void setActiveValues(Array<float> newvals);
    void setValues(Array<float> newvals);
    void setValuesAbsolute(Array<float> newvals);
    
    inline void setDimensionRatio(float r) { ratio = r; }
    
    inline Rectangle<float> getEditAllBounds(void) { return keyboardValsTextFieldOpen.getBounds().toFloat();}
    
private:
    
    String sliderName;
    BKLabel showName;
    
#if JUCE_IOS
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
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKCircularKeyboardSlider)
};
