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
    inGainLSlider.addMyListener(this);
    inGainLSlider.setStringEnding(" dB");
    inGainLSlider.getSlider()->setValue(0.0f);
    addAndMakeVisible(makeupGainLSlider);
    
    //makeupGainLSlider.reset(valueTreeState, "makeup");
    makeupGainLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::makeupStart,
                                                                                 Constants::Parameter::makeupEnd,
                                                                                  Constants::Parameter::makeupInterval));
    makeupGainLSlider.setLabelText("Makeup");
    makeupGainLSlider.addMyListener(this);
    makeupGainLSlider.setStringEnding(" dB");
    addAndMakeVisible(treshLSlider);
    //treshLSlider.reset(valueTreeState, "threshold");
    treshLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::thresholdStart,
                                                                                 Constants::Parameter::thresholdEnd,
                                                                                  Constants::Parameter::thresholdInterval));
    treshLSlider.setLabelText("Threshold");
    treshLSlider.addMyListener(this);
    treshLSlider.setStringEnding(" dB");
    addAndMakeVisible(ratioLSlider);
    
    //ratioLSlider.reset(valueTreeState, "ratio");
    ratioLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::ratioStart,
                                                                                 Constants::Parameter::ratioEnd,
                                                                                  Constants::Parameter::makeupInterval));
    ratioLSlider.setLabelText("Ratio");
    ratioLSlider.addMyListener(this);
    ratioLSlider.setStringEnding(" : 1");
    
    addAndMakeVisible(kneeLSlider);
    //kneeLSlider.reset(valueTreeState, "knee");
    kneeLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::kneeStart,
                                                                                 Constants::Parameter::kneeEnd,
                                                                                  Constants::Parameter::kneeInterval));
    kneeLSlider.setLabelText("Knee");
    kneeLSlider.addMyListener(this);
    addAndMakeVisible(attackLSlider);
    
    //attackLSlider.reset(valueTreeState, "attack");
    attackLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::attackStart,
                                                                                 Constants::Parameter::attackEnd,
                                                                                  Constants::Parameter::attackInterval));
    attackLSlider.setLabelText("Attack");
    attackLSlider.addMyListener(this);
    attackLSlider.setStringEnding(" ms");
    addAndMakeVisible(releaseLSlider);
    
    //releaseLSlider.reset(valueTreeState, "release");
    releaseLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::releaseStart,
                                                                                 Constants::Parameter::releaseEnd,
                                                                                  Constants::Parameter::releaseInterval));
    releaseLSlider.setLabelText("Release");
    releaseLSlider.addMyListener(this);
    releaseLSlider.setStringEnding(" ms");
    addAndMakeVisible(mixLSlider);
    
    //mixLSlider.reset(valueTreeState, "mix");
    mixLSlider.getSlider()->setNormalisableRange(NormalisableRange<double>(
                                                                                 Constants::Parameter::mixStart,
                                                                                 Constants::Parameter::mixEnd,
                                                                                  Constants::Parameter::mixInterval));
    
    mixLSlider.setLabelText("Mix");
    mixLSlider.addMyListener(this);
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
    
    powerButton.setButtonText("Bypass");
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
    
    addAndMakeVisible(&actionButton);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    selectCB.addListener(this);
    
    
    //selectCB.addMyListener(this);
    //addAndMakeVisible(selectCB);
    name = "Default";
    fillSelectCB(-1,-1);
}

void BKCompressorView::fillSelectCB(int last, int current)
{
    numCompressorPresets = 1;
    selectCB.clear(dontSendNotification);
    selectCB.addItem("Default", numCompressorPresets++);
    selectCB.addItem("Slammin'", numCompressorPresets++);
    selectCB.setSelectedId(selectedPresetId);
    selectCB.addSeparator();
    File file;
    for (auto xmlIter : RangedDirectoryIterator(File(file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("preparations").getChildFile("Compressor")), true, "*.xml"))
    {
        File newFile (xmlIter.getFile());
        compressorNames.add(newFile.getFullPathName());
        selectCB.addItem(newFile.getFileNameWithoutExtension(), numCompressorPresets++);
    }
    //bkp.exportedPreparations
}

void BKCompressorView::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);   // clear the background
}

void BKCompressorView::resized()
{
    auto area_init = getLocalBounds();
    actionButton.setBounds(area_init.removeFromTop(area_init.getHeight()/10).removeFromLeft(area_init.getWidth()/10));
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

PopupMenu BKCompressorView::getExportedPrepsMenu()
{
    BKPopupMenu menu;
    bkp.collectPreparations();
    StringArray* names = bkp.exportedPreparations[PreparationTypeCompressor];
    for (int i = 0; i < names->size(); i++)
    {
        menu.addItem(i+100, names->getReference(i));
    }
    
    return std::move(menu);
}

PopupMenu BKCompressorView::getPrepOptionMenu( bool singlePrep)
{
    BKPopupMenu optionMenu;
    
    optionMenu.addItem(1, "New");
    optionMenu.addItem(2, "Duplicate");
    optionMenu.addItem(6, "Rename");
    optionMenu.addItem(3, "Delete", !singlePrep);
    optionMenu.addSeparator();
    optionMenu.addItem(7, "Export");
    
    PopupMenu exported = getExportedPrepsMenu();
    optionMenu.addSubMenu("Import...", exported);
    optionMenu.addSeparator();
    optionMenu.addItem(4, "Reset");
    
    return std::move(optionMenu);
}

void BKCompressorView::actionButtonCallback(int action, BKCompressorView* view)
{
    
    
    if (action == 1) //new
    {
    }
    else if (action == 2) //duplicate
    {
    }
    else if (action == 3) //delete
    {
    }
    else if (action == 4) //reset
    {
    }
    else if (action == 6) // rename
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
      
        
        prompt.addTextEditor("name",view->name);
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String _name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            //prep->setName(name);
            String oldname = view->name;
            String oldurl = view->url;
            view->name = _name;
            ValueTree preset = view->processor.getState();
            preset.setProperty("name",view->name,0);
            std::unique_ptr<XmlElement> myXML = preset.createXml();
            File file;
#if JUCE_IOS
            file = file.getSpecialLocation(File::userDocumentsDirectory).getFullPathName() + "/Compressor/" + view->name + ".xml";
#endif
#if JUCE_MAC
            file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("preparations").getFullPathName() + "/Compressor/" + view->name + ".xml";
#endif
#if JUCE_WINDOWS || JUCE_LINUX
            file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("preparations").getFullPathName() + "\\Compressor\\" + view->name + ".xml";
#endif
            
            myXML->writeTo(file, XmlElement::TextFormat());
            view->url = file.getFullPathName();
            File oldfile (oldurl);
            oldfile.deleteFile();
        }
        
     
    }
    else if (action == 7) //"export"
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
       
        
        prompt.addTextEditor("name", view->name);
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            view->name = name;
            //prep->setName(name);
            ValueTree preset = view->processor.getState();
            preset.setProperty("name",view->name,0);
            std::unique_ptr<XmlElement> myXML = preset.createXml();
            File file;
#if JUCE_IOS
            file = file.getSpecialLocation(File::userDocumentsDirectory).getFullPathName() + "/Compressor/" + view->name + ".xml";
#endif
#if JUCE_MAC
            file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("preparations").getFullPathName() + "/Compressor/" + view->name + ".xml";
#endif
#if JUCE_WINDOWS || JUCE_LINUX
            file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("preparations").getFullPathName() + "\\Compressor\\" + view->name + ".xml";
#endif
            
            myXML->writeTo(file, XmlElement::TextFormat());
            view->url = file.getFullPathName();
        }
    }
    else if (action >= 100) //import
    {
        File file;
        view->fillSelectCB(-1,-1);
       
           
        
        int which = action - 100;
       
        
    #if JUCE_IOS
        file  = file.getSpecialLocation(File::userDocumentsDirectory);
    #endif
    #if JUCE_MAC
        file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier");
    #endif
    #if JUCE_WINDOWS || JUCE_LINUX
        file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier");
    #endif
        
        file = file.getChildFile("preparations");
        file = file.getChildFile("compressor");
        file = file.getChildFile(view->compressorNames.getReference(which));
        view->selectedPresetId = which + view->builtInPresets + 1;
        view->selectCB.setSelectedId(view->selectedPresetId);
        view->name = file.getFileNameWithoutExtension();
        view->processor.setState(XmlDocument::parse(file).get());
        view->url = file.getFullPathName();
        view->update();
    }
}

void BKCompressorView::bkButtonClicked(Button* b)
{
    if (b == &autoAttackButton)
    {
        processor.compressor.setAutoAttack(autoAttackButton.getToggleState());
        attackLSlider.setEnabled(!attackLSlider.isEnabled());
    }
    else if (b == &autoReleaseButton)
    {
        processor.compressor.setAutoRelease(!b->isEnabled());
        releaseLSlider.setEnabled(!releaseLSlider.isEnabled());
        if (!b->isEnabled())
        {
            processor.compressor.setRelease(releaseLSlider.getValue());
        }
    }
    else if (b == &powerButton)
    {
        processor.compressor.setPower(powerButton.getToggleState());
        setGUIState(!powerButton.getToggleState());
    }
    else if (b == &lahButton)
    {
        processor.compressor.setLookahead(!lahButton.getToggleState());
    }
    else if (b == &autoMakeupButton)
    {
        processor.compressor.setAutoMakeup(!autoMakeupButton.getState());
    }
    else if (b == &actionButton)
    {
        getPrepOptionMenu(true).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    
}

void BKCompressorView::comboBoxChanged(ComboBox* cb)
{
    if(cb == &selectCB)
    {
        int id = selectCB.getSelectedId();
        if (id <= builtInPresets)
        {
            ///load builtins
        } else
        {
            
            name = selectCB.getItemText(id);
            id -= (builtInPresets +1);
            url = compressorNames.getReference(id);
            processor.setState(XmlDocument::parse(File(url)).get());
            
            update();
        }
    }
    return;
}
    
void BKCompressorView::LabeledSliderValueChanged(LabeledSlider* slider, String name, double val)
{
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
