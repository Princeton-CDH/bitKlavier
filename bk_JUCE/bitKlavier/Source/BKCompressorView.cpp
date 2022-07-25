/*
  ==============================================================================

    BKCompressorView.cpp
    Created: 18 Jul 2022 11:01:12am
    Author:  Davis Polito

  ==============================================================================
*/

#include "BKCompressorView.h"
void BKCompressorView::initWidgets()
{
    addAndMakeVisible(inGainLSlider);
    //inGainLSlider.reset(valueTreeState, "inputgain");
    inGainLSlider.setLabelText("Input");
    inGainLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                             Constants::Parameter::inputStart,
                                                                             Constants::Parameter::inputEnd,
                                                                             Constants::Parameter::inputInterval));
    addAndMakeVisible(makeupGainLSlider);
    //makeupGainLSlider.reset(valueTreeState, "makeup");
    makeupGainLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::makeupStart,
                                                                                 Constants::Parameter::makeupEnd,
                                                                                  Constants::Parameter::makeupInterval));
    makeupGainLSlider.setLabelText("Makeup");

    addAndMakeVisible(treshLSlider);
    //treshLSlider.reset(valueTreeState, "threshold");
    treshLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::thresholdStart,
                                                                                 Constants::Parameter::thresholdEnd,
                                                                                  Constants::Parameter::thresholdInterval));
    treshLSlider.setLabelText("Threshold");

    addAndMakeVisible(ratioLSlider);
    //ratioLSlider.reset(valueTreeState, "ratio");
    ratioLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::ratioStart,
                                                                                 Constants::Parameter::ratioEnd,
                                                                                  Constants::Parameter::makeupInterval));
    ratioLSlider.setLabelText("Ratio");

    addAndMakeVisible(kneeLSlider);
    //kneeLSlider.reset(valueTreeState, "knee");
    kneeLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::kneeStart,
                                                                                 Constants::Parameter::kneeEnd,
                                                                                  Constants::Parameter::kneeInterval));
    kneeLSlider.setLabelText("Knee");

    addAndMakeVisible(attackLSlider);
    //attackLSlider.reset(valueTreeState, "attack");
    attackLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::attackStart,
                                                                                 Constants::Parameter::attackEnd,
                                                                                  Constants::Parameter::attackInterval));
    attackLSlider.setLabelText("Attack");

    addAndMakeVisible(releaseLSlider);
    //releaseLSlider.reset(valueTreeState, "release");
    releaseLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::releaseStart,
                                                                                 Constants::Parameter::releaseEnd,
                                                                                  Constants::Parameter::releaseInterval));
    releaseLSlider.setLabelText("Release");

    addAndMakeVisible(mixLSlider);
    //mixLSlider.reset(valueTreeState, "mix");
    makeupGainLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::mixStart,
                                                                                 Constants::Parameter::mixEnd,
                                                                                  Constants::Parameter::mixInterval));
    mixLSlider.setLabelText("Mix");

    addAndMakeVisible(lahButton);
    //lahButton.setColour(TextButton::ColourIds::buttonColourId, Colour::fromRGB(245, 124, 0));
    lahButton.setButtonText("LookAhead");
    lahButton.setClickingTogglesState(true);
    lahButton.setToggleState(false, dontSendNotification);
    lahButton.setLookAndFeel(&laf);
    //lahAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "lookahead", lahButton));

    addAndMakeVisible(autoAttackButton);
    //autoAttackButton.setColour(TextButton::ColourIds::buttonColourId, Colour::fromRGB(245, 124, 0));
    autoAttackButton.setButtonText("AutoAttack");
    autoAttackButton.setClickingTogglesState(true);
    autoAttackButton.setToggleState(false, dontSendNotification);
    autoAttackButton.addListener(this);
    autoAttackButton.setLookAndFeel(&laf);
    //autoAttackAttachment.reset(
        //new AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "autoattack", autoAttackButton));

    addAndMakeVisible(autoReleaseButton);
    //autoReleaseButton.setColour(TextButton::ColourIds::buttonColourId, Colour::fromRGB(245, 124, 0));
    autoReleaseButton.setButtonText("AutoRelease");
    autoReleaseButton.setClickingTogglesState(true);
    autoReleaseButton.setToggleState(false, dontSendNotification);
    autoReleaseButton.addListener(this);
    autoReleaseButton.setLookAndFeel(&laf);
    //autoReleaseAttachment.reset(
        //new AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "autorelease", autoReleaseButton));

    addAndMakeVisible(autoMakeupButton);
    //autoMakeupButton.setColour(TextButton::ColourIds::buttonColourId, Colour::fromRGB(245, 124, 0));
    autoMakeupButton.setLookAndFeel(&laf);
    autoMakeupButton.setButtonText("Makeup");
    autoMakeupButton.setClickingTogglesState(true);
    autoMakeupButton.setToggleState(false, dontSendNotification);
    autoMakeupButton.addListener(this);
    //autoMakeupAttachment.reset(
        //new AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "automakeup", autoMakeupButton));

    addAndMakeVisible(powerButton);
    //powerButton.setColour(TextButton::ColourIds::buttonColourId, Colour::fromRGB(245, 124, 0));
    powerButton.setLookAndFeel(&laf);
//    powerButton.setImages(
//        Drawable::createFromImageData(BinaryData::power_white_svg, BinaryData::power_white_svgSize).get());
    powerButton.setClickingTogglesState(true);
    powerButton.setToggleState(true, dontSendNotification);
    powerButton.addListener(this);
    //powerAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "power", powerButton));

    addAndMakeVisible(meter);
    meter.setMode(Meter::Mode::GR);
}

void BKCompressorView::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);   // clear the background
}

void BKCompressorView::resized()
{
    auto area = getLocalBounds().reduced(Constants::Margins::big);
    
    const auto headerHeight = area.getHeight() / 10;
    const auto btnAreaWidth = area.getWidth() / 5;
    const auto btnBotHeight = area.getHeight() / 3;

    auto header = area.removeFromTop(headerHeight).reduced(Constants::Margins::small);
    auto lBtnArea = area.removeFromLeft(btnAreaWidth).reduced(Constants::Margins::small);
    auto rBtnArea = area.removeFromRight(btnAreaWidth).reduced(Constants::Margins::small);
    auto botBtnArea = area.removeFromBottom(btnBotHeight).reduced(Constants::Margins::medium);

    const FlexItem::Margin knobMargin = FlexItem::Margin(Constants::Margins::medium);
    const FlexItem::Margin knobMarginSmall = FlexItem::Margin(Constants::Margins::medium);
    const FlexItem::Margin buttonMargin = FlexItem::Margin(Constants::Margins::small, Constants::Margins::big,
                                                           Constants::Margins::small,
                                                           Constants::Margins::big);
    FlexBox headerBox;
    headerBox.flexWrap = FlexBox::Wrap::noWrap;
    headerBox.flexDirection = FlexBox::Direction::row;
    headerBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    headerBox.items.add(FlexItem(lahButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(FlexItem(autoAttackButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(FlexItem(autoReleaseButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(FlexItem(autoMakeupButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(FlexItem(powerButton).withFlex(1).withMargin(buttonMargin));
    headerBox.performLayout(header.toFloat());

    FlexBox leftBtnBox;
    leftBtnBox.flexWrap = FlexBox::Wrap::noWrap;
    leftBtnBox.flexDirection = FlexBox::Direction::column;
    leftBtnBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    leftBtnBox.items.add(FlexItem(attackLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.items.add(FlexItem(releaseLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.items.add(FlexItem(inGainLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.performLayout(lBtnArea.toFloat());
    
    FlexBox rightBtnBox;
    rightBtnBox.flexWrap = FlexBox::Wrap::noWrap;
    rightBtnBox.flexDirection = FlexBox::Direction::column;
    rightBtnBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    rightBtnBox.items.add(FlexItem(kneeLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.items.add(FlexItem(ratioLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.items.add(FlexItem(mixLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.performLayout(rBtnArea.toFloat());

    FlexBox botBtnBox;
    botBtnBox.flexWrap = FlexBox::Wrap::noWrap;
    botBtnBox.flexDirection = FlexBox::Direction::row;
    botBtnBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    botBtnBox.items.add(FlexItem(treshLSlider).withFlex(1).withMargin(knobMargin));
    botBtnBox.items.add(FlexItem(makeupGainLSlider).withFlex(1).withMargin(knobMargin));
    botBtnBox.performLayout(botBtnArea.toFloat());

    FlexBox meterBox;
    meterBox.flexWrap = FlexBox::Wrap::noWrap;
    meterBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    meterBox.items.add(FlexItem(meter).withFlex(1).withMargin(Constants::Margins::big));
    meterBox.performLayout(area.toFloat());
}


void BKCompressorView::buttonClicked(Button* b)
{
    if (b == &autoAttackButton)
    {
        processor.compressor.setAutoAttack(autoAttackButton.getToggleState());
        attackLSlider.setEnabled(!attackLSlider.isEnabled());
    }
    else if (b == &autoReleaseButton)
    {
        processor.compressor.setAutoRelease(b->isEnabled());
        releaseLSlider.setEnabled(!releaseLSlider.isEnabled());
        if (!b->isEnabled())
        {
            processor.compressor.setRelease(releaseLSlider.getValue());
        }
    }
    else if (b == &powerButton)
    {
        processor.compressor.setPower(powerButton.getToggleState());
        setGUIState(powerButton.getToggleState());
    }
    else if (b == &lahButton)
    {
        processor.compressor.setLookahead(lahButton.getToggleState());
    }
}

void BKCompressorView::LabeledSliderValueChanged(LabeledSlider* slider, String name, double val)
{
    LabeledSlider inGainLSlider;
    LabeledSlider makeupGainLSlider;
    LabeledSlider treshLSlider;
    LabeledSlider ratioLSlider;
    LabeledSlider kneeLSlider;
    LabeledSlider attackLSlider;
    LabeledSlider releaseLSlider;
    LabeledSlider mixLSlider;
    if (slider == &inGainLSlider)
    {
        processor.compressor.setInput(val);
    }
    else if (slider == &makeupGainLSlider)
    {
        processor.compressor.setMakeup(val);
    }
    else if (slider == &treshLSlider)
    {
        processor.compressor.setThreshold(val);
    }
    else if (slider == &ratioLSlider)
    {
        processor.compressor.setRatio(val);
    }
    else if (slider == &kneeLSlider)
    {
        processor.compressor.setKnee(val);
    }
    else if (slider == &attackLSlider)
    {
        processor.compressor.setAttack(val);
    }
    else if (slider == &releaseLSlider)
    {
        processor.compressor.setRelease(val);
    }
    else if (slider == &mixLSlider)
    {
        processor.compressor.setMix(val);
    }
}

void BKCompressorView::timerCallback()
{
    int m = meter.getMode();
    switch (m)
    {
    case Meter::Mode::IN:
        meter.update(processor.currentInput.get());
        break;
    case Meter::Mode::OUT:
        meter.update(processor.currentOutput.get());
        break;
    case Meter::Mode::GR:
        meter.update(processor.gainReduction.get());
        break;
    default:
        break;
    }
}
void BKCompressorView::setGUIState(bool powerState)
{
    inGainLSlider.setEnabled(powerState);
    treshLSlider.setEnabled(powerState);
    ratioLSlider.setEnabled(powerState);
    kneeLSlider.setEnabled(powerState);
    makeupGainLSlider.setEnabled(powerState);
    mixLSlider.setEnabled(powerState);
    meter.setEnabled(powerState);
    meter.setGUIEnabled(powerState);
    lahButton.setEnabled(powerState);
    autoMakeupButton.setEnabled(powerState);

    autoAttackButton.setEnabled(powerState);
    autoReleaseButton.setEnabled(powerState);

    if (!powerState)
    {
        attackLSlider.setEnabled(powerState);
        releaseLSlider.setEnabled(powerState);
    }
    else
    {
        attackLSlider.setEnabled(!autoAttackButton.getToggleState());
        releaseLSlider.setEnabled(!autoReleaseButton.getToggleState());
    }
}
