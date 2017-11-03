/*
  ==============================================================================

    BKViewController.cpp
    Created: 31 Oct 2017 1:09:55pm
    Author:  airship

  ==============================================================================
*/

#include "BKViewController.h"

#include "KeymapViewController.h"


BKViewController::BKViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph),
lastId(0)
{
    addChildComponent(numberPad);
    numberPad.setAlwaysOnTop(true);
    numberPad.addListener(this);
}

BKViewController::~BKViewController()
{
    
}

PopupMenu BKViewController::getPrepOptionMenu(void)
{
    PopupMenu optionMenu;
    optionMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    optionMenu.addItem(1, "New");
    
    optionMenu.addItem(2, "Duplicate");
    optionMenu.addItem(3, "Delete");
    optionMenu.addSeparator();
    optionMenu.addItem(4, "Reset");
    optionMenu.addItem(5, "Clear");
    
    return optionMenu;
}

PopupMenu BKViewController::getModOptionMenu(void)
{
    PopupMenu optionMenu;
    optionMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    optionMenu.addItem(1, "New");
    
    optionMenu.addItem(2, "Duplicate");
    optionMenu.addItem(3, "Delete");
    optionMenu.addSeparator();
    optionMenu.addItem(5, "Clear");
    /*
     optionMenu.addSeparator();
     optionMenu.addItem(4, "Import");
     optionMenu.addItem(5, "Export");
     */
    
    return optionMenu;
}

void BKViewController::bkSingleSliderWantsKeyboard(BKSingleSlider* slider)
{
    DBG("BEGIN: " + String(slider->getText()));
    
    //numberPadDismissed(&numberPad);
    
    latched_BKWDUTSlider = nullptr;
    latched_BKMultiSlider = nullptr;
    latched_BKRangeSlider = nullptr;
    latched_BKKeyboardSlider = nullptr;
    latched_BKStackedSlider = nullptr;
    latched_BKSingleSlider = slider;
    latched_BKTextEditor = nullptr;
    
    numberPad.setTarget(&slider->valueTF);
    
    float numberPadHeight = getHeight() - 2 * gYSpacing;
    float numberPadWidth = getWidth() / 2 - 2 * gXSpacing;
    numberPad.setSize(numberPadWidth, numberPadHeight);
    
    numberPad.setTopLeftPosition((slider->getX() > (getWidth() * 0.45)) ? gXSpacing : (0.5 * getWidth() + gXSpacing), gYSpacing);
    
    numberPad.setEnabled(NumberLBracket, false);
    numberPad.setEnabled(NumberRBracket, false);
    numberPad.setEnabled(NumberNegative, false);
    numberPad.setEnabled(NumberColon, false);
    numberPad.setEnabled(NumberSpace, false);
    
    numberPad.setText(String(slider->getText()));
    
    numberPad.setVisible(true);
}

void BKViewController::bkStackedSliderWantsKeyboard(BKStackedSlider* slider)
{
    
    DBG("BEGIN: " + slider->getText());
    
    //numberPadDismissed(&numberPad);
    
    latched_BKWDUTSlider = nullptr;
    latched_BKMultiSlider = nullptr;
    latched_BKRangeSlider = nullptr;
    latched_BKKeyboardSlider = nullptr;
    latched_BKStackedSlider = slider;
    latched_BKSingleSlider = nullptr;
    latched_BKTextEditor = nullptr;
    
    numberPad.setTarget(slider->getTextEditor());
    
    float numberPadHeight = getHeight() - 2 * gYSpacing;
    float numberPadWidth = getWidth() / 2 - 2 * gXSpacing;
    numberPad.setSize(numberPadWidth, numberPadHeight);
    
    DBG("sliderX: " + String(slider->getX()) + " width: " + String(getWidth()));
    numberPad.setTopLeftPosition((slider->getX() > (getWidth() * 0.45)) ? gXSpacing : (0.5 * getWidth() + gXSpacing), gYSpacing);
    
    
    numberPad.setEnabled(NumberLBracket, false);
    numberPad.setEnabled(NumberRBracket, false);
    numberPad.setEnabled(NumberNegative, true);
    numberPad.setEnabled(NumberColon, false);
    numberPad.setEnabled(NumberSpace, true);
    
    numberPad.setText(slider->getText());
    
    numberPad.setVisible(true);
}

void BKViewController::multiSliderWantsKeyboard(BKMultiSlider* slider)
{
    latched_BKWDUTSlider = nullptr;
    latched_BKMultiSlider = slider;
    latched_BKRangeSlider = nullptr;
    latched_BKKeyboardSlider = nullptr;
    latched_BKStackedSlider = nullptr;
    latched_BKSingleSlider = nullptr;
    latched_BKTextEditor = nullptr;
    
    numberPad.setTarget(slider->getTextEditor());
    
    float numberPadHeight = getHeight() - 2 * gYSpacing;
    float numberPadWidth = getWidth() / 2 - 2 * gXSpacing;
    numberPad.setSize(numberPadWidth, numberPadHeight);
    
    numberPad.setTopLeftPosition((slider->getX() > (getWidth() * 0.45)) ? gXSpacing : (0.5 * getWidth() + gXSpacing), gYSpacing);
    
    numberPad.setEnabled(NumberLBracket, true);
    numberPad.setEnabled(NumberRBracket, true);
    numberPad.setEnabled(NumberNegative, true);
    numberPad.setEnabled(NumberColon, false);
    numberPad.setEnabled(NumberSpace, true);
    
    numberPad.setText(String(slider->getText()));
    
    numberPad.setVisible(true);
}

void BKViewController::bkRangeSliderWantsKeyboard(BKRangeSlider* slider, BKRangeSliderType which)
{
    rangeType = which;
    latched_BKWDUTSlider = nullptr;
    latched_BKMultiSlider = nullptr;
    latched_BKRangeSlider = slider;
    latched_BKKeyboardSlider = nullptr;
    latched_BKStackedSlider = nullptr;
    latched_BKSingleSlider = nullptr;
    latched_BKTextEditor = nullptr;
    
    
    TextEditor* editor = slider->getTextEditor(which);
    
    numberPad.setTarget(editor);
    
    float numberPadHeight = getHeight() - 2 * gYSpacing;
    float numberPadWidth = getWidth() / 2 - 2 * gXSpacing;
    numberPad.setSize(numberPadWidth, numberPadHeight);
    
    numberPad.setTopLeftPosition((slider->getX() > (getWidth() * 0.45)) ? gXSpacing : (0.5 * getWidth() + gXSpacing), gYSpacing);
    
    numberPad.setEnabled(NumberLBracket, false);
    numberPad.setEnabled(NumberRBracket, false);
    numberPad.setEnabled(NumberNegative, false);
    numberPad.setEnabled(NumberColon, false);
    numberPad.setEnabled(NumberSpace, false);
    
    numberPad.setText(editor->getText());
    
    numberPad.setVisible(true);
}

void BKViewController::bkWaveDistanceUndertowSliderWantsKeyboard(BKWaveDistanceUndertowSlider* slider, NostalgicParameterType type)
{
    wdutType = type;
    latched_BKWDUTSlider = slider;
    latched_BKMultiSlider = nullptr;
    latched_BKRangeSlider = nullptr;
    latched_BKKeyboardSlider = nullptr;
    latched_BKStackedSlider = nullptr;
    latched_BKSingleSlider = nullptr;
    latched_BKTextEditor = nullptr;
    
    TextEditor* target = slider->getTextEditor(type);
    numberPad.setTarget(target);
    
    float numberPadHeight = getHeight() - 2 * gYSpacing;
    float numberPadWidth = getWidth() / 2 - 2 * gXSpacing;
    numberPad.setSize(numberPadWidth, numberPadHeight);
    
    numberPad.setTopLeftPosition((slider->getX() > (getWidth() * 0.45)) ? gXSpacing : (0.5 * getWidth() + gXSpacing), gYSpacing);
    
    numberPad.setEnabled(NumberLBracket, true);
    numberPad.setEnabled(NumberRBracket, true);
    numberPad.setEnabled(NumberNegative, true);
    numberPad.setEnabled(NumberColon, false);
    numberPad.setEnabled(NumberSpace, true);
    
    numberPad.setText(target->getText());
    
    numberPad.setVisible(true);
}

void BKViewController::keyboardSliderWantsKeyboard(BKKeyboardSlider* slider, KSliderTextFieldType which)
{
    latched_BKWDUTSlider = nullptr;
    latched_BKMultiSlider = nullptr;
    latched_BKRangeSlider = nullptr;
    latched_BKKeyboardSlider = slider;
    latched_BKStackedSlider = nullptr;
    latched_BKSingleSlider = nullptr;
    latched_BKTextEditor = nullptr;
    
    TextEditor* editor = slider->getTextEditor(which);
    
    numberPad.setTarget(editor);
    
    float numberPadHeight = getHeight() - 2 * gYSpacing;
    float numberPadWidth = getWidth() / 2 - 2 * gXSpacing;
    numberPad.setSize(numberPadWidth, numberPadHeight);
    
    if (which == KSliderAllValues)      numberPad.setTopLeftPosition((0.5 * getWidth() + gXSpacing), gYSpacing);
    else if (which == KSliderThisValue) numberPad.setTopLeftPosition(gXSpacing, gYSpacing);
    
    numberPad.setEnabled(NumberLBracket, false);
    numberPad.setEnabled(NumberRBracket, false);
    numberPad.setEnabled(NumberNegative, false);
    numberPad.setEnabled(NumberColon, true);
    numberPad.setEnabled(NumberSpace, true);
    
    numberPad.setText(String(editor->getText()));
    
    numberPad.setVisible(true);
}

void BKViewController::textEditorWantsKeyboard(BKTextEditor* editor)
{
    latched_BKWDUTSlider = nullptr;
    latched_BKMultiSlider = nullptr;
    latched_BKRangeSlider = nullptr;
    latched_BKKeyboardSlider = nullptr;
    latched_BKStackedSlider = nullptr;
    latched_BKSingleSlider = nullptr;
    latched_BKTextEditor = editor;
    
    numberPad.setTarget(editor);
    
    float numberPadHeight = getHeight() - 2 * gYSpacing;
    float numberPadWidth = getWidth() / 2 - 2 * gXSpacing;
    numberPad.setSize(numberPadWidth, numberPadHeight);
    
    numberPad.setTopLeftPosition((editor->getX() > (getWidth() * 0.45)) ? gXSpacing : (0.5 * getWidth() + gXSpacing), gYSpacing);
    
    numberPad.setEnabled(NumberLBracket, false);
    numberPad.setEnabled(NumberRBracket, false);
    numberPad.setEnabled(NumberNegative, false);
    numberPad.setEnabled(NumberColon, false);
    numberPad.setEnabled(NumberSpace, false);
    
    numberPad.setText(String(editor->getText()));
    
    numberPad.setVisible(true);
    
    
}

void BKViewController::numberPadChanged(BKNumberPad*)
{
    String text = numberPad.getText();
    
    DBG("CHANGED: " + text);
    
    if (latched_BKSingleSlider!= nullptr)       latched_BKSingleSlider->setText(text);
    if (latched_BKStackedSlider != nullptr)     latched_BKStackedSlider->setText(text);
    
    if (latched_BKWDUTSlider != nullptr)        latched_BKWDUTSlider->getTextEditor(wdutType)->setText(text);
    if (latched_BKRangeSlider != nullptr)       latched_BKRangeSlider->getTextEditor(rangeType)->setText(text);
    
    if (latched_BKKeyboardSlider != nullptr)    latched_BKKeyboardSlider->setText(text);
    if (latched_BKMultiSlider != nullptr)       latched_BKMultiSlider->setText(text);
    
    if (latched_BKTextEditor != nullptr)        latched_BKTextEditor->setText(text);
}

void BKViewController::numberPadDismissed(BKNumberPad*) 
{
    String text = numberPad.getText();
    
    DBG("DISMISSED: " + text);
    
    if (latched_BKSingleSlider != nullptr)      latched_BKSingleSlider      ->setValue(text.getDoubleValue(), sendNotification);
    if (latched_BKStackedSlider != nullptr)     latched_BKStackedSlider     ->dismissTextEditor(true);
    
    if (latched_BKRangeSlider != nullptr)       latched_BKRangeSlider       ->dismissTextEditor(true);
    
    if (latched_BKKeyboardSlider != nullptr)    latched_BKKeyboardSlider    ->dismissTextEditor(true);
    if (latched_BKMultiSlider != nullptr)       latched_BKMultiSlider       ->dismissTextEditor(true);
    
    if (latched_BKWDUTSlider != nullptr)        latched_BKWDUTSlider        ->dismissTextEditor(true);
    
    if (latched_BKTextEditor != nullptr)
    {
        ((BKViewController*)latched_BKTextEditor->getParentComponent())->bkTextFieldDidChange(*((TextEditor*)latched_BKTextEditor));
    }
    
    numberPad.setVisible(false);
}
