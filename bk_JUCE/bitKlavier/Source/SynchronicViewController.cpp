/*
  ==============================================================================

    SynchronicViewController.cpp
    Created: 21 Apr 2017 11:17:47pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "SynchronicViewController.h"
#include "BKUtilities.h"
#include "Keymap.h"

#include "Synchronic.h"

SynchronicViewController::SynchronicViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 4) // third argument => number of tabs
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::synchronic_icon_png, BinaryData::synchronic_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    //addAndMakeVisible(iconImageComponent);
    
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    // MultSliders
    paramSliders = OwnedArray<BKMultiSlider>();

    int idx = 0;
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        if ((cSynchronicDataTypes[i] == BKFloatArr || cSynchronicDataTypes[i] == BKArrFloatArr) && cSynchronicParameterTypes[i] != "ADSRs")
        {
            paramSliders.insert(idx, new BKMultiSlider());
            addAndMakeVisible(paramSliders[idx], ALL);
            paramSliders[idx]->setName(cSynchronicParameterTypes[idx+SynchronicTranspOffsets]);
            paramSliders[idx]->addMyListener(this);
#if JUCE_IOS
            paramSliders[idx]->addWantsBigOneListener(this);
#endif
            paramSliders[idx]->setMinMaxDefaultInc(cSynchronicDefaultRangeValuesAndInc[i]);
            
            if(paramSliders[idx]->getName() == "transpositions")
            {
                paramSliders[idx]->setAllowSubSlider(true);
                paramSliders[idx]->setSubSliderName("add transposition");
                paramSliders[idx]->setToolTipString("Determines pitch of sequenced notes or chords; control-click to add another voice, double-click to edit all or add additional sequence steps");
            }
            else if(paramSliders[idx]->getName() == "accents")
            {
                paramSliders[idx]->setToolTipString("Determines gain of sequenced pitches; double-click to edit all or add additional sequence steps");
            }
            else if(paramSliders[idx]->getName() == "sustain length multipliers")
            {
                paramSliders[idx]->setToolTipString("Determines duration of each sequenced note; double-click to edit all or add additional sequence steps");
            }
            else if(paramSliders[idx]->getName() == "beat length multipliers")
            {
                paramSliders[idx]->setToolTipString("Determines length of each sequenced beat as a factor of Synchronic tempo; double-click to edit all or add additional sequence steps");
            }
            
            idx++;
        }
    }
    
    //Envelope Sliders
    envelopeSliders = OwnedArray<BKADSRSlider>();
    for(int i = 0; i < 12; i++)
    {
        envelopeSliders.insert(i, new BKADSRSlider("e"+String(i)));
        envelopeSliders[i]->setToolTip("Provides ADSR settings for up to 12 sequenced steps");
        envelopeSliders[i]->setButtonText("");
        envelopeSliders[i]->toFront(false);
        //envelopeSliders[i]->setAlpha(0.5);
        envelopeSliders[i]->setDim(0.5);
        addAndMakeVisible(envelopeSliders[i], ALL);
    }
    envelopeSliders[0]->setBright();
    showADSR = false;
    visibleADSR = 0;
    
    // Target Control CBs
    targetControlCBs = OwnedArray<BKComboBox>();
    for (int i = TargetTypeSynchronicPatternSync; i <= TargetTypeSynchronicRotate; i++)
    {
        targetControlCBs.add(new BKComboBox()); // insert at the end of the array
        targetControlCBs.getLast()->setName(cKeymapTargetTypes[i]);
        targetControlCBs.getLast()->addListener(this);
        targetControlCBs.getLast()->setLookAndFeel(&comboBoxRightJustifyLAF);
        targetControlCBs.getLast()->addItem(cTargetNoteModes[TargetNoteMode::NoteOn], TargetNoteMode::NoteOn + 1);
        targetControlCBs.getLast()->addItem(cTargetNoteModes[TargetNoteMode::NoteOff], TargetNoteMode::NoteOff + 1);
        targetControlCBs.getLast()->addItem(cTargetNoteModes[TargetNoteMode::Both], TargetNoteMode::Both + 1);
        targetControlCBs.getLast()->setSelectedItemIndex(0, dontSendNotification);
        addAndMakeVisible(targetControlCBs.getLast(), ALL);
        
        targetControlCBLabels.add(new BKLabel());
        targetControlCBLabels.getLast()->setText(cKeymapTargetTypes[i], dontSendNotification);
        addAndMakeVisible(targetControlCBLabels.getLast(), ALL);
    }
    
    // border for Target Triggers
    targetControlsGroup.setName("targetGroup");
    targetControlsGroup.setText("Synchronic Target Triggers");
    targetControlsGroup.setTextLabelPosition(Justification::centred);
    targetControlsGroup.setAlpha(0.65);
    addAndMakeVisible(targetControlsGroup);

    // Shared combo boxes
    selectCB.setName("Synchronic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedId(1, dontSendNotification);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(&selectCB, ALL);
#if JUCE_MAC
    midiOutputSelectCB.setName("MIDI Output");
    midiOutputSelectCB.addSeparator();
    midiOutputSelectCB.addListener(this);
    midiOutputSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    midiOutputSelectCB.setSelectedItemIndex(0, dontSendNotification);
    midiOutputSelectCB.setTooltip("Select the MIDI output device for this preparation");
    addAndMakeVisible(&midiOutputSelectCB, ALL);
#endif
    // MODE
    modeSelectCB.setName("Mode");
    modeSelectCB.addSeparator();
    modeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    modeSelectCB.setSelectedItemIndex(0, dontSendNotification);
    modeSelectCB.setTooltip("Determines which aspect of MIDI signal triggers the Synchronic sequence");
    
    addAndMakeVisible(&modeSelectCB, ALL);
    
    modeLabel.setText("pulse triggered by", dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::centredRight);
    modeLabel.setTooltip("Determines which aspect of MIDI signal triggers the Synchronic sequence");
    addAndMakeVisible(&modeLabel, ALL);
    
    // ON OFF
    onOffSelectCB.setName("OnOff");
    onOffSelectCB.addSeparator();
    //onOffSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    onOffSelectCB.setLookAndFeel(&comboBoxRightJustifyLAF);
    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
    //onOffSelectCB.BKSetJustificationType(juce::Justification::centredLeft);
    onOffSelectCB.setSelectedItemIndex(0);
    onOffSelectCB.setTooltip("Determines whether MIDI note-on or note-off is used to measure cluster");
    
    onOffSelectCB.addItem("Key On", 1);
    onOffSelectCB.addItem("Key Off", 2);
    
    addAndMakeVisible(&onOffSelectCB, ALL);
    
    onOffLabel.setText("determines cluster", dontSendNotification);
    onOffLabel.setTooltip("Determines whether MIDI note On or note Off is used to measure cluster");
    addAndMakeVisible(&onOffLabel, ALL);
     
    //offsetParamStartToggle = new BKSingleSlider("skip first", 0, 1, 0, 1);
    offsetParamStartToggle.setButtonText ("skip first");
    offsetParamStartToggle.setTooltip("Indicates whether Synchronic will skip first column of sequenced parameters for first cycle");
    offsetParamStartToggle.setToggleState (true, dontSendNotification);
    addAndMakeVisible(&offsetParamStartToggle, ALL);
    
    howManySlider = std::make_unique<BKSingleSlider>("num pulses", cSynchronicNumBeats, 1, 100, 20, 1);
    howManySlider->setToolTipString("Indicates number of steps/repetitions in Synchronic pulse");
    howManySlider->setJustifyRight(false);
    addAndMakeVisible(*howManySlider, ALL);
    
    clusterThreshSlider = std::make_unique< BKSingleSlider>("cluster threshold", cSynchronicClusterThresh, 20, 2000, 200, 10);
    clusterThreshSlider->setToolTipString("successive notes spaced by less than this time (ms) are grouped as a cluster");
    clusterThreshSlider->setJustifyRight(true);
    addAndMakeVisible(*clusterThreshSlider, ALL);
    
    clusterMinMaxSlider = std::make_unique< BKRangeSlider>("cluster min/max", 1, 12, 3, 4, 1);
    clusterMinMaxSlider->setToolTipString("Sets Min and Max numbers of keys pressed to launch pulse; Min can be greater than Max");
    clusterMinMaxSlider->setJustifyRight(true);
    addAndMakeVisible(*clusterMinMaxSlider, ALL);
    
    clusterCapSlider = std::make_unique<BKSingleSlider>("cluster thickness", cSynchronicClusterCap, 1, 20, 8, 1);
    clusterCapSlider->setToolTipString("maximum number of notes in sounding pulse");
    clusterCapSlider->setJustifyRight(true);
    addAndMakeVisible(*clusterCapSlider, ALL);
    
    holdTimeMinMaxSlider = std::make_unique<BKRangeSlider>("hold min/max", 0., 12000., 0.0, 12000., 1);
    holdTimeMinMaxSlider->setToolTipString("Sets Min and Max time (ms) held to trigger pulses; Min can be greater than Max");
    holdTimeMinMaxSlider->setJustifyRight(true);
    addAndMakeVisible(*holdTimeMinMaxSlider, ALL);
    
    velocityMinMaxSlider = std::make_unique<BKRangeSlider>("velocity min/max", 0, 127, 0, 127, 1);
    velocityMinMaxSlider->setToolTipString("Sets Min and Max velocity (0-127) to trigger pulses; Min can be greater than Max");
    velocityMinMaxSlider->setJustifyRight(true);
    addAndMakeVisible(*velocityMinMaxSlider, ALL);
    
    gainSlider = std::make_unique<BKSingleSlider>("volume (dB)", cSynchronicGain, -100, 24, 0, 0.01, "-inf");
    gainSlider->setToolTipString("Overall volume of Synchronic pulse");
    gainSlider->setJustifyRight(false);
    gainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(*gainSlider, ALL);
    
    numClusterSlider = std::make_unique<BKSingleSlider>("num layers", cSynchronicNumClusters, 1, 10, 1, 1);
    numClusterSlider->setToolTipString("Number of simultaneous layers of pulses");
    numClusterSlider->setJustifyRight(false);
    addAndMakeVisible(*numClusterSlider, ALL);
    
    transpUsesTuning.setButtonText ("use Tuning?");
    transpUsesTuning.setTooltip("non-zero transpositions will be tuned using attached Tuning");
    transpUsesTuning.setToggleState (false, dontSendNotification);
    addAndMakeVisible(&transpUsesTuning, ALL);
    
    blendronicGainSlider = std::make_unique<BKSingleSlider>("blendronic send volume (dB)", cSynchronicBlendronicGain, -100, 24, 0, 0.01, "-inf");
    blendronicGainSlider->setToolTipString("Volume of Synchronic output to connected Blendronics");
    blendronicGainSlider->setJustifyRight(false);
    blendronicGainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(*blendronicGainSlider, ALL);
    
#if JUCE_IOS
    numClusterSlider->addWantsBigOneListener(this);
    howManySlider->addWantsBigOneListener(this);
    clusterThreshSlider->addWantsBigOneListener(this);
    clusterCapSlider->addWantsBigOneListener(this);
    gainSlider->addWantsBigOneListener(this);
    clusterMinMaxSlider->addWantsBigOneListener(this);
    holdTimeMinMaxSlider->addWantsBigOneListener(this);
    velocityMinMaxSlider->addWantsBigOneListener(this);
    blendronicGainSlider->addWantsBigOneListener(this);
#endif
    
    releaseVelocitySetsSynchronicToggle.addListener(this);
    releaseVelocitySetsSynchronicToggle.setLookAndFeel(&buttonsAndMenusLAF2); //need different one so toggle text can be on other side
    releaseVelocitySetsSynchronicToggle.setButtonText ("noteOff loudness");
    buttonsAndMenusLAF2.setToggleBoxTextToRightBool(false);
    releaseVelocitySetsSynchronicToggle.setToggleState (false, dontSendNotification);
    //addAndMakeVisible(releaseVelocitySetsSynchronicToggle); //possibly for future version, but it seems even keyboards that do noteOff velocity suck at it.
    
    addAndMakeVisible(&actionButton, ALL);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    alternateMod.setButtonText ("alternate mod");
    alternateMod.setTooltip("activating this mod will alternate between modding and reseting attached preparations");
    alternateMod.setToggleState (false, dontSendNotification);
    addChildComponent(&alternateMod, ALL);
    alternateMod.setLookAndFeel(&buttonsAndMenusLAF);
    
    envelopeName.setText("envelopes", dontSendNotification);
    //envelopeName.setTooltip("Provides ADSR settings for up to 12 sequenced steps");
    envelopeName.setJustificationType(Justification::centredRight);
    envelopeName.toBack();
    envelopeName.setInterceptsMouseClicks(false, true);
    addAndMakeVisible(&envelopeName, ALL);
    
    currentTab = 0;
    displayTab(currentTab);

}

void SynchronicViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}


void SynchronicViewController::invisible(void)
{
    gainSlider->setVisible(false);
    
    offsetParamStartToggle.setVisible(false);
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        paramSliders[i]->setVisible(false);
    }
    
    for (int i=0; i<targetControlCBs.size(); i++)
    {
        targetControlCBs[i]->setVisible(false);
        targetControlCBLabels[i]->setVisible(false);
    }
    targetControlsGroup.setVisible(false);
    
    envelopeName.setVisible(false);
    for(int i=envelopeSliders.size() - 1; i>=0; i--)
    {
        envelopeSliders[i]->setVisible(false);
    }
    
    clusterMinMaxSlider->setVisible(false);
    clusterThreshSlider->setVisible(false);
    clusterCapSlider->setVisible(false);
    holdTimeMinMaxSlider->setVisible(false);
    velocityMinMaxSlider->setVisible(false);
    numClusterSlider->setVisible(false);
    howManySlider->setVisible(false);
    modeLabel.setVisible(false);
    modeSelectCB.setVisible(false);
    onOffLabel.setVisible(false);
    onOffSelectCB.setVisible(false);
    releaseVelocitySetsSynchronicToggle.setVisible(false);
    transpUsesTuning.setVisible(false);
    blendronicGainSlider->setVisible(false);
}

void SynchronicViewController::displayShared(void)
{
    Rectangle<int> area (getBounds());
    
    iconImageComponent.setBounds(area);
    
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.0f * gPaddingConst * processor .paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    midiOutputSelectCB.setBounds(actionButton.getRight()+gXSpacing,
                                 actionButton.getY(),
                                 selectCB.getWidth(),
                                 selectCB.getHeight());
    
    alternateMod.setBounds(midiOutputSelectCB.getRight()+gXSpacing,
                           midiOutputSelectCB.getY(),
                           selectCB.getWidth(),
                           actionButton.getHeight());
    
    comboBoxSlice.removeFromLeft(gXSpacing);
    
    leftArrow.setBounds (0, getHeight() * 0.4, 50, 50);
    rightArrow.setBounds (getRight() - 50, getHeight() * 0.4, 50, 50);

}

void SynchronicViewController::displayTab(int tab)
{
    currentTab = tab;
    
    invisible();
    displayShared();
    
    int x0 = leftArrow.getRight() + gXSpacing;
    int y0 = hideOrShow.getBottom() + gYSpacing;
    int right = rightArrow.getX() - gXSpacing;
    int width = right - x0;
    int height = getHeight() - y0;
    
    if (tab == 0)
    {
        if (showADSR)
        {
            // DEAL WITH ENVELOPE; MOVE THIS TO NEW TAB, include the buttons as well
            for(int i = 0; i < envelopeSliders.size(); i++)
            {
                if(i == visibleADSR)    envelopeSliders[i]->setVisible(true);
                else                    envelopeSliders[i]->setVisible(false);
                
                envelopeSliders[i]->setAlpha(1.);
            }
            envelopeName.setVisible(false);
            envelopeSliders[visibleADSR]->setButtonText("close");
            
            
            envelopeSliders[visibleADSR]->setBounds(x0, y0+ 100, width, height-100);
            
            selectCB.toFront(false);
        }
        else
        {
            // DEAL WITH ENVELOPE
            for(int i = 0; i < envelopeSliders.size(); i++)
            {
                envelopeSliders[i]->setVisible(true);
                envelopeSliders[i]->setAlpha(0.5);
                envelopeSliders[i]->setButtonText(String(""));
                envelopeSliders[i]->resized();
            }
            //envelopeName.setVisible(true);
            
            if(envelopeSliders[visibleADSR]->isEnabled()) envelopeSliders[visibleADSR]->setActive();
            
            // SET VISIBILITY
            offsetParamStartToggle.setVisible(true);
            transpUsesTuning.setVisible(true);
            
            for (int i = 0; i < paramSliders.size(); i++)
            {
                paramSliders[i]->setVisible(true);
            }

            //envelopeName.setVisible(true);
            for(int i = envelopeSliders.size() - 1; i >= 0; i--)
            {
                envelopeSliders[i]->setVisible(true);
            }
            
            // SET BOUNDS
            int sliderHeight = height * 0.225f;
            int sliderWidth = width;
            
            for (int i = 0; i < paramSliders.size(); i++)
            {
                paramSliders[i]->setBounds(x0, y0 + i * sliderHeight, sliderWidth, sliderHeight - gYSpacing);
            }
            
            transpUsesTuning.setBounds(paramSliders[0]->getRight() - 120,
                                       paramSliders[0]->getBottom() - gComponentToggleBoxHeight,
                                       120, 30);
            transpUsesTuning.toFront(false);
            
            int envelopeWidth = (sliderWidth - 50) / 12;
            int envelopeHeight = height * 0.1f;
            
            for(int i = 0; i < envelopeSliders.size(); i++)
            {
                envelopeSliders[i]->setBounds(x0 + 50 + i * (envelopeWidth + 0.65), paramSliders.getLast()->getBottom() + gYSpacing,
                                              envelopeWidth, envelopeHeight - gYSpacing);
            }
            
            offsetParamStartToggle.setBounds(right - 100, selectCB.getY(), 100, 30);
            // transpUsesTuning.setBounds(offsetParamStartToggle.getX() - 4 * gXSpacing - 120, selectCB.getY(), 120, 30);
        }
        
        
        
        iconImageComponent.toBack();
    }
    else if (tab == 1)
    {
        // SET VISIBILITY
        gainSlider->setVisible(true);
        clusterMinMaxSlider->setVisible(true);
        clusterThreshSlider->setVisible(true);
        clusterCapSlider->setVisible(true);
        holdTimeMinMaxSlider->setVisible(true);
        velocityMinMaxSlider->setVisible(true);
        numClusterSlider->setVisible(true);
        howManySlider->setVisible(true);
        modeLabel.setVisible(true);
        modeSelectCB.setVisible(true);
        onOffLabel.setVisible(true);
        onOffSelectCB.setVisible(true);
        iconImageComponent.toBack();

        Rectangle<int> area (getBounds());
        //area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        area.removeFromTop(selectCB.getHeight() + 50 * processor.paddingScalarY + 4 + gYSpacing);
        area.removeFromRight(rightArrow.getWidth());
        area.removeFromLeft(leftArrow.getWidth());
        
        Rectangle<int> leftColumn (area.removeFromLeft(area.getWidth()* 0.5));
        
        leftColumn.removeFromRight(processor.paddingScalarX * 20);
        leftColumn.removeFromLeft(processor.paddingScalarX * 20);
        
        area.removeFromLeft(processor.paddingScalarX * 20); //area is now right column
        area.removeFromRight(processor.paddingScalarX * 20);
        
        Rectangle<int> modeSelectCBRect (leftColumn.removeFromTop(gComponentComboBoxHeight));
        Rectangle<int> modeSelectLabel (modeSelectCBRect.removeFromLeft(modeSelectCBRect.getWidth()*0.5));
        modeSelectCB.setBounds(modeSelectCBRect);
        modeLabel.setBounds(modeSelectLabel);
        
        Rectangle<int> onOffSelectCBRect (area.removeFromTop(gComponentComboBoxHeight));
        Rectangle<int> onOffSelectLabel (onOffSelectCBRect.removeFromRight(onOffSelectCBRect.getWidth()*0.5));
        onOffSelectCB.setBounds(onOffSelectCBRect);
        onOffLabel.setBounds(onOffSelectLabel);
        
        leftColumn.removeFromTop(gYSpacing + gComponentComboBoxHeight * 2 * processor.paddingScalarY);
        area.removeFromTop(gYSpacing + gComponentComboBoxHeight * 2 * processor.paddingScalarY);
        
        int columnHeight = leftColumn.getHeight();
        
        howManySlider->setBounds(leftColumn.removeFromTop(columnHeight / 4));
        numClusterSlider->setBounds(leftColumn.removeFromTop(columnHeight / 4));
        clusterCapSlider->setBounds(leftColumn.removeFromTop(columnHeight / 4));
        gainSlider->setBounds(leftColumn.removeFromTop(columnHeight / 4));
        
        clusterThreshSlider->setBounds(area.removeFromTop(columnHeight / 4));
        clusterMinMaxSlider->setBounds(area.removeFromTop(columnHeight / 4));
        holdTimeMinMaxSlider->setBounds(area.removeFromTop(columnHeight / 4));
        velocityMinMaxSlider->setBounds(area.removeFromTop(columnHeight / 4));
        
        releaseVelocitySetsSynchronicToggle.setVisible(true);
    }
    else if (tab == 2) // keymap target tab
    {
        // make the combo boxes visible
        for (int i = 0; i < targetControlCBs.size(); i++)
        {
            targetControlCBs[i]->setVisible(true);
            targetControlCBLabels[i]->setVisible(true);
        }
        
        targetControlsGroup.setVisible(true);
        
        Rectangle<int> area (getBounds());
        area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
        area.removeFromTop(selectCB.getHeight() + 50 * processor.paddingScalarY + 4 + gYSpacing);
        area.removeFromRight(rightArrow.getWidth());
        area.removeFromLeft(leftArrow.getWidth());
        
        area.removeFromTop((area.getHeight() - (targetControlCBs.size() / 2) * (gComponentComboBoxHeight + gYSpacing)) / 3.);
        
        // make four columns
        // textlabel | combobox | combobox | textlabel
    
        int wideColumnWidth = area.getWidth() * 0.3;
        int narrowColumnWidth = area.getWidth() * 0.2;
        
        Rectangle<int> column1 (area.removeFromLeft(wideColumnWidth));
        column1.removeFromRight(processor.paddingScalarX * 5);
        column1.removeFromLeft(processor.paddingScalarX * 20);
        
        Rectangle<int> column2 (area.removeFromLeft(narrowColumnWidth));
        column2.removeFromRight(processor.paddingScalarX * 5);
        column2.removeFromLeft(processor.paddingScalarX * 20);
        
        Rectangle<int> column4 (area.removeFromRight(wideColumnWidth));
        column4.removeFromRight(processor.paddingScalarX * 20);
        column4.removeFromLeft(processor.paddingScalarX * 5);
        
        Rectangle<int> column3 (area.removeFromRight(narrowColumnWidth));
        column3.removeFromRight(processor.paddingScalarX * 20);
        column3.removeFromLeft(processor.paddingScalarX * 5);
        
        for (int i = 0; i < targetControlCBs.size() / 2; i++)
        {
            targetControlCBs[i]->setLookAndFeel(&buttonsAndMenusLAF);
            
            targetControlCBs[i]->setBounds(column2.removeFromTop(gComponentComboBoxHeight));
            column2.removeFromTop(gYSpacing);
            
            targetControlCBLabels[i]->setBounds(column1.removeFromTop(gComponentComboBoxHeight));
            column1.removeFromTop(gYSpacing);
            
            targetControlCBLabels[i]->setJustificationType(juce::Justification::centredRight);
        }
        
        for (int i = targetControlCBs.size() / 2; i < targetControlCBs.size(); i++)
        {
            targetControlCBs[i]->setLookAndFeel(&comboBoxRightJustifyLAF);
            
            targetControlCBs[i]->setBounds(column3.removeFromTop(gComponentComboBoxHeight));
            column3.removeFromTop(gYSpacing);
            
            targetControlCBLabels[i]->setBounds(column4.removeFromTop(gComponentComboBoxHeight));
            column4.removeFromTop(gYSpacing);
        }
        
        targetControlsGroup.setBounds(targetControlCBLabels[0]->getX() - 4 * gXSpacing,
                                      targetControlCBLabels[0]->getY() - gComponentComboBoxHeight - 2 * gXSpacing,
                                      targetControlCBLabels[targetControlCBs.size() - 1]->getRight() - targetControlCBLabels[0]->getX() + 8 * gXSpacing,
                                      //column1.getWidth() + column2.getWidth() + column3.getWidth() + column4.getWidth() + 8 * gXSpacing,
                                      targetControlCBs[0]->getHeight() * (targetControlCBs.size() / 2) + 2 * gComponentComboBoxHeight + 4 * gYSpacing);
        
        /*
        Rectangle<int> leftColumn (area.removeFromLeft(area.getWidth() * 0.5));
        leftColumn.removeFromLeft(leftColumn.getWidth() * 0.5 * processor.paddingScalarX);
        
        leftColumn.removeFromRight(processor.paddingScalarX * 5);
        leftColumn.removeFromLeft(processor.paddingScalarX * 20);
        
        area.removeFromLeft(processor.paddingScalarX * 5); //area is now right column
        area.removeFromRight(processor.paddingScalarX * 20);
        
        int targetControlCBSection = (gComponentComboBoxHeight + gYSpacing) * targetControlCBs.size();
        leftColumn.removeFromTop((leftColumn.getHeight() - targetControlCBSection) / 3.);
        area.removeFromTop((area.getHeight() - targetControlCBSection) / 3.);
        
        for (int i=0; i<targetControlCBs.size(); i++)
        {
            targetControlCBs[i]->setBounds(leftColumn.removeFromTop(gComponentComboBoxHeight));
            leftColumn.removeFromTop(gYSpacing);
            
            targetControlCBLabels[i]->setBounds(area.removeFromTop(gComponentComboBoxHeight));
            area.removeFromTop(gYSpacing);
        }
        
        targetControlsGroup.setBounds(targetControlCBs[0]->getX() - 4 * gXSpacing,
                                      targetControlCBs[0]->getY() - gComponentComboBoxHeight - 2 * gXSpacing,
                                      targetControlCBs[0]->getWidth() * 2 + 8 * gXSpacing,
                                      targetControlCBs[0]->getHeight() * targetControlCBs.size() + 2 * gComponentComboBoxHeight + 4 * gYSpacing);
         */
    }
    else if (tab == 3)
    {
        blendronicGainSlider->setVisible(true);
        
        Rectangle<int> area (getBounds());
        area.removeFromTop(selectCB.getHeight() + 70 * processor.paddingScalarY + 4 + gYSpacing);
        area.removeFromRight(rightArrow.getWidth());
        area.removeFromLeft(leftArrow.getWidth());
        area.removeFromRight(processor.paddingScalarX * 20);
        
        area.removeFromTop(processor.paddingScalarY * 30);
        blendronicGainSlider->setBounds(area.removeFromTop(gComponentStackedSliderHeight + processor.paddingScalarY * 30));
    }
}

void SynchronicViewController::resized()
{
    
    displayShared();
    displayTab(currentTab);
    
#if 0
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    
    if (!showADSR)
    {
        Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
        modeSlice.removeFromRight(gXSpacing);
        offsetParamStartToggle.setBounds(modeSlice.removeFromRight(modeSlice.getWidth()));
        
        
        Rectangle<float> envelopeSlice = area.removeFromBottom(gComponentComboBoxHeight + gPaddingConst * processor.paddingScalarY).toFloat();
        
        int tempHeight = (area.getHeight() - paramSliders.size() * (gYSpacing + gPaddingConst * processor.paddingScalarY)) / paramSliders.size();
        area.removeFromLeft(4 + 2.*gPaddingConst * processor.paddingScalarX);
        area.removeFromRight(gXSpacing);
        for(int i = 0; i < paramSliders.size(); i++)
        {
            area.removeFromTop(gYSpacing + gPaddingConst * processor.paddingScalarY);
            paramSliders[i]->setBounds(area.removeFromTop(tempHeight));
        }
        
        
        // envelopeSlice.removeFromTop(gPaddingConst * (1. - processor.paddingScalarY));
        envelopeSlice.removeFromTop(gPaddingConst * processor.paddingScalarY);
        envelopeSlice.removeFromRight(gXSpacing);
        envelopeName.setBounds(envelopeSlice.toNearestInt());
        
        float envWidth = (float)(paramSliders[0]->getWidth() - 50) / 12.;
        for(int i = envelopeSliders.size() - 1; i >= 0; i--)
        {
            Rectangle<float> envArea (envelopeSlice.removeFromRight(envWidth));
            envelopeSliders[i]->setBounds(envArea.toNearestInt());
        }
        
        //leftColumn.reduce(4 + 2.*gPaddingConst * paddingScalarX, 0);
        leftColumn.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
        //leftColumn.removeFromLeft(gXSpacing);
        
        leftColumn.removeFromTop(gPaddingConst * (1. - processor.paddingScalarY));
        leftColumn.removeFromBottom(gYSpacing * 2);
        int sliderHeight = leftColumn.getHeight() / 6.25;
        int sliderSpacing = (sliderHeight - gComponentSingleSliderHeight) * 0.5;
        
        gainSlider->setBounds(leftColumn.removeFromBottom(gComponentSingleSliderHeight));
        leftColumn.removeFromBottom(sliderSpacing);
        
        clusterMinMaxSlider->setBounds(leftColumn.removeFromBottom(gComponentSingleSliderHeight));
        leftColumn.removeFromBottom(sliderSpacing);
        
        clusterThreshSlider->setBounds(leftColumn.removeFromBottom(gComponentSingleSliderHeight));
        leftColumn.removeFromBottom(sliderSpacing);
        
        holdTimeMinMaxSlider->setBounds(leftColumn.removeFromBottom(gComponentSingleSliderHeight));
        leftColumn.removeFromBottom(sliderSpacing);
        
        velocityMinMaxSlider->setBounds(leftColumn.removeFromBottom(gComponentSingleSliderHeight));
        leftColumn.removeFromBottom(sliderSpacing);
        
        numClusterSlider->setBounds(leftColumn.removeFromBottom(gComponentSingleSliderHeight));
        leftColumn.removeFromBottom(sliderSpacing);
        
        howManySlider->setBounds(leftColumn.removeFromBottom(gComponentSingleSliderHeight));
        leftColumn.removeFromBottom(sliderSpacing);
        
        Rectangle<int> modeSlice2 = leftColumn.removeFromBottom(gComponentComboBoxHeight);
        modeSlice2.removeFromLeft(gXSpacing * 2);
        int modeSlice2Chunk = modeSlice2.getWidth() / 3;
        modeSlice2.removeFromRight(modeSlice2Chunk);
        modeLabel.setBounds(modeSlice2.removeFromRight(modeSlice2Chunk));
        modeSelectCB.setBounds(modeSlice2);

        Rectangle<int> slice3 = leftColumn.removeFromBottom(1.5*gComponentComboBoxHeight);
        slice3 = slice3.removeFromTop(gComponentComboBoxHeight);
        slice3.removeFromLeft(gXSpacing * 2);
        int slice3Chunk = slice3.getWidth() / 3;
        slice3.removeFromRight(slice3Chunk);
        onOffLabel.setBounds(slice3.removeFromRight(slice3Chunk));
        onOffSelectCB.setBounds(slice3);
        
        Rectangle<int> releaseToggleSlice = gainSlider->getBounds().removeFromTop(gComponentTextFieldHeight);
        releaseToggleSlice.removeFromRight(gYSpacing);
        releaseVelocitySetsSynchronicToggle.setBounds(releaseToggleSlice.removeFromRight(releaseToggleSlice.getWidth() * 0.5));

    }
    else
    {
        areaSave.removeFromTop(gYSpacing * 2 + 8.*gPaddingConst * processor.paddingScalarY);
        Rectangle<int> adsrSliderSlice = areaSave.removeFromTop(gComponentComboBoxHeight * 2 + gComponentSingleSliderHeight * 2 + gYSpacing * 3);
        envelopeSliders[visibleADSR]->setBounds(adsrSliderSlice);
        
        //areaSave.removeFromTop(gComponentComboBoxHeight * 2 + gYSpacing + 8.*gPaddingConst * processor.paddingScalarY);
        //ADSRSlider->setBounds(areaSave);
        
        selectCB.toFront(false);
    }
#endif
    repaint();
}

void SynchronicViewController::setShowADSR(String name, bool newval)
{
    showADSR = newval;
    
    for(int i =  0; i < envelopeSliders.size(); i++)
    {
        if(envelopeSliders[i]->getName() == name) visibleADSR = i;
    }

    resized();
}


#if JUCE_IOS
void SynchronicViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    rightArrow.setAlwaysOnTop(false);
    leftArrow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SynchronicPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

SynchronicPreparationEditor::SynchronicPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
SynchronicViewController(p, theGraph)
{
    fillSelectCB(-1,-1);
    
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    modeSelectCB.addListener(this);
    onOffSelectCB.addListener(this);
    offsetParamStartToggle.addListener(this);
    howManySlider->addMyListener(this);
    clusterThreshSlider->addMyListener(this);
    clusterCapSlider->addMyListener(this);
    clusterMinMaxSlider->addMyListener(this);
    holdTimeMinMaxSlider->addMyListener(this);
    velocityMinMaxSlider->addMyListener(this);
    transpUsesTuning.addListener(this);
    
    for (auto slider : paramSliders) slider->addMyListener(this);

    gainSlider->addMyListener(this);
    numClusterSlider->addMyListener(this);
    
    blendronicGainSlider->addMyListener(this);
    
    for(int i=0; i<envelopeSliders.size(); i++)
    {
        envelopeSliders[i]->addMyListener(this);
    }
    
    envelopeSliders[0]->adsrButton.triggerClick(); //initialize first envelope, since it is always active
    
    startTimer(30);
    
    fillModeSelectCB();
    
    fillMidiOutputSelectCB();
    
}

void SynchronicPreparationEditor::fillModeSelectCB()
{
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    modeSelectCB.clear(dontSendNotification);
    
    if (prep->onOffMode.value == KeyOn)
    {
        modeSelectCB.addItem("First Note-On", 1);
        modeSelectCB.addItem("Any Note-On", 2);
        modeSelectCB.addItem("First Note-Off", 3);
        modeSelectCB.addItem("Any Note-Off", 4);
        modeSelectCB.addItem("Last Note-Off", 5);
        modeSelectCB.setSelectedItemIndex(prep->sMode.value, dontSendNotification);
    }
    else
    {
        modeSelectCB.addItem("First Note-Off", 1);
        modeSelectCB.addItem("Any Note-Off", 2);
        modeSelectCB.addItem("Last Note-Off", 3);
        if (prep->sMode.value < FirstNoteOffSync)
        {
            prep->sMode.set(FirstNoteOffSync);
        }
        modeSelectCB.setSelectedItemIndex(prep->sMode.value-2, dontSendNotification);
    }
    
}


void SynchronicPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplaySynchronic)
    {
        SynchronicProcessor::Ptr sProcessor = processor.currentPiano->getSynchronicProcessor(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        // dim target comboboxes that aren't activated by a Keymap
        for (int i = TargetTypeSynchronicPatternSync; i <= TargetTypeSynchronicRotate; i++)
        {
            bool makeBright = false;
            for (auto km : sProcessor->getKeymaps())
                if (km->getTargetStates()[(KeymapTargetType) i] == TargetStateEnabled) makeBright = true;
            
            if (makeBright)
            {
                targetControlCBs[i - TargetTypeSynchronicPatternSync]->setAlpha(1.);
                targetControlCBLabels[i - TargetTypeSynchronicPatternSync]->setAlpha(1.);
                
                targetControlCBs[i - TargetTypeSynchronicPatternSync]->setEnabled(true);
            }
            else
            {
                targetControlCBs[i - TargetTypeSynchronicPatternSync]->setAlpha(0.25);
                targetControlCBLabels[i - TargetTypeSynchronicPatternSync]->setAlpha(0.25);
                
                targetControlCBs[i - TargetTypeSynchronicPatternSync]->setEnabled(false);
            }
        }
          
        SynchronicCluster::Ptr cluster = sProcessor->getClusters()[0];
        for (int i = 0; i < sProcessor->getClusters().size(); i++)
        {
            if (sProcessor->getClusters()[i]->getShouldPlay())
            {
                cluster = sProcessor->getClusters()[i];
                break;
            }
        }
        
        if (prep->onOffMode.value == KeyOn)
        {
            if (prep->sMode.value == AnyNoteOffSync || prep->sMode.value == LastNoteOffSync)
            {
                holdTimeMinMaxSlider->setBright();
                holdTimeMinMaxSlider->setEnabled(true);
            }
            else
            {
                holdTimeMinMaxSlider->setDim(gModAlpha);
                holdTimeMinMaxSlider->setEnabled(false);
            }
        }
        else
        {
            holdTimeMinMaxSlider->setBright();
            holdTimeMinMaxSlider->setEnabled(true);
        }
        
        //fillModeSelectCB();
        
        if (cluster == nullptr) return;
        
        if(cluster->getBeatCounter() < prep->sNumBeats.value && sProcessor->getPlayCluster())
            howManySlider->setDisplayValue(cluster->getBeatCounter());
        else howManySlider->setDisplayValue(0);
        
        if(sProcessor->getClusterThresholdTimer() < prep->sClusterThresh.value)
             clusterThreshSlider->setDisplayValue(sProcessor->getClusterThresholdTimer());
        else clusterThreshSlider->setDisplayValue(0);
        
        holdTimeMinMaxSlider->setDisplayValue(sProcessor->getHoldTimer());
        DBG("sProcessor->getOldestHoldTimer() = " + String(sProcessor->getHoldTimer()));
        velocityMinMaxSlider->setDisplayValue(sProcessor->getLastVelocity() * 127.);
        DBG("sProcessor->getLastVelocity() = " + String(sProcessor->getLastVelocity()));
        
        int maxTemp = 12; //arbitrary
        if(prep->sClusterMax.value > prep->sClusterMin.value) maxTemp = prep->sClusterMax.value;
        
        /*
        if(sProcessor->getNumKeysDepressed() <= maxTemp && sProcessor->getClusterThresholdTimer() < active->sClusterThresh.value)
            //clusterMinMaxSlider->setDisplayValue((float)sProcessor->getNumKeysDepressed() * active->sClusterMax.value / clusterMinMaxSlider->maxSlider.getMaximum());
            clusterMinMaxSlider->setDisplayValue(sProcessor->getNumKeysDepressed());
        else clusterMinMaxSlider->setDisplayValue(0);
         */
        
        if(sProcessor->getClusterThresholdTimer() < prep->sClusterThresh.value)
            clusterMinMaxSlider->setDisplayValue(cluster->getClusterSize()  * 12. / clusterMinMaxSlider->maxSlider.getMaximum()); //scaling not working when minmaxSlider max is > 12
        else clusterMinMaxSlider->setDisplayValue(0);
        
        //DBG("active->sClusterMax.value = " + String(active->sClusterMax.value) + "clusterMinMaxSlider->maxSlider.getMaximum() = " + String(clusterMinMaxSlider->maxSlider.getMaximum()));
        
        //DBG("cluster size = " + String(cluster->getClusterSize()));
        
        int counter = 0, size = 0;
        
        if (sProcessor != nullptr)
        {
            if (sProcessor->getPlayCluster())
            {
                for (int i = 0; i < paramSliders.size(); i++)
                {
                    if(paramSliders[i]->getName() == "beat length multipliers")
                    {
                        size = paramSliders[i]->getNumVisible();
                        counter = cluster->getBeatMultiplierCounterForDisplay();
                        paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                    }
                    else if(paramSliders[i]->getName() == "sustain length multipliers")
                    {
                        size = paramSliders[i]->getNumVisible();
                        counter = cluster->getLengthMultiplierCounterForDisplay();
                        paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                    }
                    else if(paramSliders[i]->getName() == "accents")
                    {
                        size = paramSliders[i]->getNumVisible();
                        counter = cluster->getAccentMultiplierCounterForDisplay();
                        paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                    }
                    else if(paramSliders[i]->getName() == "transpositions")
                    {
                        size = paramSliders[i]->getNumVisible();
                        counter = cluster->getTranspCounterForDisplay();
                        paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                    }
                }
            }
            else
            {
                for (int i = 0; i < paramSliders.size(); i++)
                {
                    paramSliders[i]->deHighlightCurrentSlider();
                }
            }
            
            for(int i = 0; i < envelopeSliders.size(); i++)
            {
                if(i == cluster->getEnvelopeCounter()) envelopeSliders[i]->setHighlighted();
                
                else if(prep->getEnvelopeOn(i))
                {
                    envelopeSliders[i]->setActive();
                    envelopeSliders[i]->setBright();
                }
                else
                {
                    envelopeSliders[i]->setPassive();
                    envelopeSliders[i]->setDim(gModAlpha);
                }
            }
        }
        
        if (prep->sGain.active) gainSlider->setValue(prep->sGain.value, dontSendNotification);
        if (prep->sBlendronicGain.active) blendronicGainSlider->setValue(prep->sBlendronicGain.value, dontSendNotification);
        if (prep->sNumBeats.active) howManySlider->setValue(prep->sNumBeats.value, dontSendNotification);
        if (prep->sClusterThresh.active) clusterThreshSlider->setValue(prep->sClusterThresh.value, dontSendNotification);
        if (prep->sClusterCap.active) clusterCapSlider->setValue(prep->sClusterCap.value, dontSendNotification);
        if (prep->numClusters.active) numClusterSlider->setValue(prep->numClusters.value, dontSendNotification);
    }
}


void SynchronicPreparationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
//    if (name == "accents")
//    {
//        prep    ->setAccentMultiplier(whichSlider, values[0]);
//    }
//    else if (name == "beat length multipliers")
//    {
//        prep    ->setBeatMultiplier(whichSlider, values[0]);
//    }
//    else if (name == "sustain length multipliers")
//    {
//        prep    ->setLengthMultiplier(whichSlider, values[0]);
//    }
//    else if (name == "transpositions")
//    {
//        prep    ->setSingleTransposition(whichSlider, values);
//    }
    
    //processor.updateState->synchronicPreparationDidChange = true;
    processor.updateState->editsMade = true;
}

void SynchronicPreparationEditor::multiSlidersDidChange(String name, Array<Array<float>> values, Array<bool> states)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i = 0; i < values.size(); i++) newvals.add(values[i][0]);

    if (!name.compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
    {
        prep    ->sAccentMultipliers.set(newvals);
        prep    ->sAccentMultipliersStates.set(states);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
    {
        prep    ->sBeatMultipliers.set(newvals);
        prep    ->sBeatMultipliersStates.set(states);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
    {
        prep    ->sLengthMultipliers.set(newvals);
        prep    ->sLengthMultipliersStates.set(states);
    }
    //pass original 2D array for transpositions
    else if (!name.compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
    {
        prep    ->sTransposition.set(values);
        prep    ->sTranspositionStates.set(states);
    }
    
    processor.updateState->editsMade = true;
}

/*
void SynchronicPreparationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i = 0; i < values.size(); i++) newvals.add(values[i][0]);

    //if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    if (!name.compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
    {
        prep    ->setAccentMultipliers(newvals);
        active  ->setAccentMultipliers(newvals);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
    {
        prep    ->setBeatMultipliers(newvals);
        active  ->setBeatMultipliers(newvals);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
    {
        prep    ->setLengthMultipliers(newvals);
        active  ->setLengthMultipliers(newvals);
    }
    //pass original 2D array for transpositions
    else if (!name.compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
    {
        prep    ->setTransposition(values);
        active  ->setTransposition(values);
    }
    
    //processor.updateState->synchronicPreparationDidChange = true;
}
 */

void SynchronicPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if(name == "num pulses") {
        DBG("got new how many " + String(val));
        prep->sNumBeats.set(val);
    }
    else if(name == "cluster threshold")
    {
        DBG("got new cluster threshold " + String(val));
        prep->sClusterThresh.set(val);
    }
    else if(name == "cluster thickness")
    {
        DBG("got new cluster thickness " + String(val));
        prep->sClusterCap.set(val);
    }
    else if(slider->getName() == gainSlider->getName())
    {
        DBG("gain " + String(val));
        prep->sGain.set(val);
    }
    else if(slider->getName() == blendronicGainSlider->getName())
    {
        prep->sBlendronicGain.set(val);
    }
    //else if(name == "num layers")
    else if(slider == numClusterSlider.get())
    {
        DBG("num layers " + String(val));
        prep->numClusters.set(val);
    }
    
    processor.updateState->editsMade = true;
}

void SynchronicPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if(name == "cluster min/max") {
        DBG("got new cluster min/max " + String(minval) + " " + String(maxval));
        prep->sClusterMin.set(minval);
        prep->sClusterMax.set(maxval);
    }
    else if(name == "hold min/max") {
        DBG("got new hold min/max " + String(minval) + " " + String(maxval));
        prep->holdMin.set(minval);
        prep->holdMax.set(maxval);
    }
    else if(name == "velocity min/max") {
        DBG("got new velocity min/max " + String(minval) + " " + String(maxval));
        prep->setVelocityMin(minval);
        prep->setVelocityMax(maxval);
    }
    
    processor.updateState->editsMade = true;
}

void SynchronicPreparationEditor::update(NotificationType notify)
{
    // DBG("SynchronicPreparationEditor::update " + String((int)notify));
    
    if (processor.updateState->currentSynchronicId < 0) return;
    
    fillModeSelectCB();
    
    fillMidiOutputSelectCB();
    
    SynchronicPreparation::Ptr prep   = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    // DEAL WITH ENVELOPE
    for(int i = 0; i < envelopeSliders.size(); i++)
    {
        envelopeSliders[i]->setAlpha(0.5);
        envelopeSliders[i]->setButtonText(String(""));
        envelopeSliders[i]->resized();
    }
    
    transpUsesTuning.setToggleState(prep->sTranspUsesTuning.value, notify);
    
    if(envelopeSliders[visibleADSR]->isEnabled()) envelopeSliders[visibleADSR]->setActive();
    
    if (prep != nullptr)
    {
        gainSlider->setValue(prep->sGain.value, notify);
        blendronicGainSlider->setValue(prep->sBlendronicGain.value, notify);
        
        selectCB.setSelectedId(processor.updateState->currentSynchronicId, notify);
        modeSelectCB.setSelectedItemIndex(prep->sMode.value, notify);
        onOffSelectCB.setSelectedItemIndex(prep->onOffMode.value, notify);
        //offsetParamStartToggle.setToggleState(prep->getOffsetParamToggle(), notify);
        offsetParamStartToggle.setToggleState(prep->sBeatsToSkip.value, notify);
        releaseVelocitySetsSynchronicToggle.setToggleState(prep->sReleaseVelocitySetsSynchronic.value, notify);
        howManySlider->setValue(prep->sNumBeats.value, notify);
        clusterThreshSlider->setValue(prep->sClusterThresh.value, notify);
        clusterCapSlider->setValue(prep->sClusterCap.value, notify);
        clusterMinMaxSlider->setMinValue(prep->sClusterMin.value, notify);
        clusterMinMaxSlider->setMaxValue(prep->sClusterMax.value, notify);
        
        holdTimeMinMaxSlider->setMinValue(prep->holdMin.value, notify);
        holdTimeMinMaxSlider->setMaxValue(prep->holdMax.value, notify);
        
        velocityMinMaxSlider->setMinValue(prep->velocityMin.value, notify);
        velocityMinMaxSlider->setMaxValue(prep->velocityMax.value, notify);
        
        numClusterSlider->setValue(prep->numClusters.value, notify);
                
        for (int i = TargetTypeSynchronicPatternSync; i <= TargetTypeSynchronicRotate; i++)
        {
            targetControlCBs[i - TargetTypeSynchronicPatternSync]->setSelectedItemIndex
                            (prep->getTargetTypeSynchronic(KeymapTargetType(i)), notify);
        }
        
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
            {
                // paramSliders[i]->setTo(prep->sAccentMultipliers, notify);
                paramSliders[i]->setToOnlyActive(prep->sAccentMultipliers.value,
                                                 prep->sAccentMultipliersStates.value, notify);
            }
            
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
            {
                paramSliders[i]->setToOnlyActive(prep->sBeatMultipliers.value,
                                                 prep->sBeatMultipliersStates.value, notify);
            }
            
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
            {
                paramSliders[i]->setToOnlyActive(prep->sLengthMultipliers.value,
                                                 prep->sLengthMultipliersStates.value, notify);
            }
            
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
            {
                paramSliders[i]->setToOnlyActive(prep->sTransposition.value,
                                                 prep->sTranspositionStates.value, notify);
            }
        }
        
        for(int i=0; i<envelopeSliders.size(); i++)
        {
            envelopeSliders[i]->setAttackValue(prep->getAttack(i), notify);
            envelopeSliders[i]->setDecayValue(prep->getDecay(i), notify);
            envelopeSliders[i]->setSustainValue(prep->getSustain(i), notify);
            envelopeSliders[i]->setReleaseValue(prep->getRelease(i), notify);
        }
    }
    
}

void SynchronicPreparationEditor::update()
{
    update(dontSendNotification);
}

void SynchronicPreparationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    DBG("BKADSRSliderValueChanged received " + name);
    
    int which = 0;
    for(int i=0; i<envelopeSliders.size(); i++)
    {
        if(envelopeSliders[i]->getName() == name) which = i;
    }
    
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    prep->sADSRs.base.set(which, Array<float>(attack, decay, sustain, release, true));
    prep->sADSRs.value.set(which, Array<float>(attack, decay, sustain, release, true));
    envelopeSliders[which]->setBright();
    
    processor.updateState->editsMade = true;
}

void SynchronicPreparationEditor::BKADSRButtonStateChanged(String name, bool shift, bool state)
{
    //DBG("BKADSRButtonStateChanged + " + String((int)state));
    
    int which = 0;
    for(int i=0; i<envelopeSliders.size(); i++)
    {
        if(envelopeSliders[i]->getName() == name) which = i;
    }
    
    if(shift)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        if(which != 0) //first envelope is always on...
        {
            //DBG("toggling " + String(which) + " " + String((int)state));
            prep->sADSRs.base[which].set(4, state ? 1 : 0);
            prep->sADSRs.value[which].set(4, state ? 1 : 0);
            if(state) envelopeSliders[which]->setBright();
            else envelopeSliders[which]->setDim(gModAlpha);
            
        }
        else
        {
            envelopeSliders[0]->setButtonToggle(true);
            envelopeSliders[0]->setBright();
        }
        
        processor.updateState->editsMade = true;
    }
    else
    {
        setShowADSR(name, !state);
        setSubWindowInFront(!state);
    }
}

void SynchronicPreparationEditor::closeSubWindow()
{
    for(int i=0; i<envelopeSliders.size(); i++)
    {
        envelopeSliders[i]->setIsButtonOnly(true);
        setShowADSR(envelopeSliders[i]->getName(), false);
    }
    DBG("SynchronicPreparationEditor::closeSubWindow()");
    setSubWindowInFront(false);
}

void SynchronicPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllSynchronic())
    {
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Synchronic"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeSynchronic, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentSynchronicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);

    lastId = selectedId;
}

void SynchronicPreparationEditor::fillMidiOutputSelectCB()
{
    SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    midiOutputSelectCB.clear(dontSendNotification);

    int Id = 1;
    midiOutputSelectCB.addItem("No MIDI Output Selected", Id);
    midiOutputSelectCB.setItemEnabled(Id++, true);
    for (auto device : processor.getMidiOutputDevices())
    {
        String name = device.name;

        if (name != String())  midiOutputSelectCB.addItem(name, Id);
        else                   midiOutputSelectCB.addItem("Device"+String(Id), Id);

        if (prep->midiOutput.value != nullptr)
        {
            if (name == prep->midiOutput.value->getName())
            {
                midiOutputSelectCB.setSelectedId(Id, NotificationType::dontSendNotification);
            }
        }
        else midiOutputSelectCB.setSelectedId(1, NotificationType::dontSendNotification);
        midiOutputSelectCB.setItemEnabled(Id++, true);
    }
}

int SynchronicPreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeSynchronic);
    
    return processor.gallery->getAllSynchronic().getLast()->getId();
}

int SynchronicPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeSynchronic, processor.updateState->currentSynchronicId);
    
    return processor.gallery->getAllSynchronic().getLast()->getId();
}

void SynchronicPreparationEditor::deleteCurrent(void)
{
    int SynchronicId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeSynchronic, SynchronicId);
    
    fillSelectCB(0, 0);
    
    selectCB.setSelectedItemIndex(0, dontSendNotification);
    int newId = selectCB.getSelectedId();
    
    setCurrentId(newId);
}

void SynchronicPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentSynchronicId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void SynchronicPreparationEditor::actionButtonCallback(int action, SynchronicPreparationEditor* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("New Synchronic Preparation");
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("Duplicate Synchronic Preparation");
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
        processor.saveGalleryToHistory("Delete Synchronic Preparation");
    }
    else if (action == 4)
    {
        processor.reset(PreparationTypeSynchronic, processor.updateState->currentSynchronicId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeSynchronic, processor.updateState->currentSynchronicId);
        vc->update();
        processor.saveGalleryToHistory("Clear Synchronic Preparation");
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentSynchronicId;
        Synchronic::Ptr prep = processor.gallery->getSynchronic(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->setName(name);
            vc->fillSelectCB(Id, Id);
            processor.saveGalleryToHistory("Rename Synchronic Preparation");
        }
        
        vc->update();
    }
    else if (action == 7)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentSynchronicId;
        Synchronic::Ptr prep = processor.gallery->getSynchronic(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeSynchronic, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeSynchronic, processor.updateState->currentSynchronicId, which);
        vc->update();
        processor.saveGalleryToHistory("Import Synchronic Preparation");
    }
}


void SynchronicPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    DBG("SynchronicPreparationEditor::bkComboBoxDidChange called " + box->getName());
    
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == "Mode")
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        if (prep->onOffMode.value == KeyOn)
        {
            prep    ->sMode.set((SynchronicSyncMode) index);
        }
        else if (prep->onOffMode.value == KeyOff)
        {
            prep    ->sMode.set((SynchronicSyncMode) (index+2));
        }
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;

        prep->sBeatsToSkip.set(toggleVal);
        
    }
    else if (name == "OnOff")
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep    ->onOffMode.set( (SynchronicOnOffMode) index);
        
        fillModeSelectCB();
        
    }
    else if (name == "MIDI Output")
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        if (index > 0)
        {
            prep    ->setMidiOutput(processor.getMidiOutputDevices()[index-1]);
        }
        else
        {
            prep    ->setMidiOutput(nullptr);
        }
        
        fillMidiOutputSelectCB();
    }
    else // target combo boxes from tab 3
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        for (int i=0; i<targetControlCBs.size(); i++)
        {
            if (box == targetControlCBs[i])
            {
                int selectedItem = targetControlCBs[i]->getSelectedId() - 1;
                DBG(targetControlCBs[i]->getName() + " " + cTargetNoteModes[selectedItem]);
                
                prep    ->setTargetTypeSynchronic(KeymapTargetType(i + TargetTypeSynchronicPatternSync), (TargetNoteMode)selectedItem);
            }
        }
    }
    
    processor.updateState->editsMade = true;
}

void SynchronicPreparationEditor::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    if (name == "Name")
    {
        Synchronic::Ptr synchronic = processor.gallery->getSynchronic(processor.updateState->currentSynchronicId);
        
        synchronic->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
    
    processor.updateState->editsMade = true;
}

void SynchronicPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Synchronic::Ptr synchronic = processor.gallery->getSynchronic(processor.updateState->currentSynchronicId);
    
    synchronic->setName(name);
    
    processor.updateState->editsMade = true;
}

void SynchronicPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        update();
    }
}

void SynchronicPreparationEditor::buttonClicked (Button* b)
{
    if (b == &offsetParamStartToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;

        prep->sBeatsToSkip.set(toggleVal);
        

    }
    else if (b == &releaseVelocitySetsSynchronicToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep->sReleaseVelocitySetsSynchronic.set(releaseVelocitySetsSynchronicToggle.getToggleState());
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
        
        for(int i=0; i<envelopeSliders.size(); i++)
        {
            envelopeSliders[i]->setIsButtonOnly(true);
            setShowADSR(envelopeSliders[i]->getName(), false);
        }
        setSubWindowInFront(false);
        
    }
    else if (b == &rightArrow)
    {
        arrowPressed(RightArrow);
        
        DBG("currentTab: " + String(currentTab));
        
        displayTab(currentTab);
    }
    else if (b == &leftArrow)
    {
        arrowPressed(LeftArrow);
        
        DBG("currentTab: " + String(currentTab));
        
        displayTab(currentTab);
    }
    else if (b == &actionButton)
    {
        bool single = processor.gallery->getAllSynchronic().size() == 2;
        getPrepOptionMenu(PreparationTypeSynchronic, single).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    else if (b == &transpUsesTuning)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        DBG("Synchronic transpUsesTuning = " + String((int)b->getToggleState()));
        prep->sTranspUsesTuning.set(b->getToggleState());
        
    }
}



// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SynchronicModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
SynchronicModificationEditor::SynchronicModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
SynchronicViewController(p, theGraph)
{
    numTabs = 2;
    
    greyOutAllComponents();
    
    fillSelectCB(-1,-1);
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    midiOutputSelectCB.addListener(this);
    modeSelectCB.addListener(this);
    onOffSelectCB.addListener(this);
    offsetParamStartToggle.addListener(this);
    howManySlider->addMyListener(this);
    clusterThreshSlider->addMyListener(this);
    clusterMinMaxSlider->addMyListener(this);
    clusterCapSlider->addMyListener(this);
    holdTimeMinMaxSlider->addMyListener(this);
    velocityMinMaxSlider->addMyListener(this);
    gainSlider->addMyListener(this);
    numClusterSlider->addMyListener(this);
    blendronicGainSlider->addMyListener(this);
    transpUsesTuning.addListener(this);
    
    for (auto slider : paramSliders) slider->addMyListener(this);
    
    howManySlider->displaySliderVisible(false);
    clusterThreshSlider->displaySliderVisible(false);
    clusterCapSlider->displaySliderVisible(false);
    clusterMinMaxSlider->displaySliderVisible(false);
    holdTimeMinMaxSlider->displaySliderVisible(false);
    velocityMinMaxSlider->displaySliderVisible(false);
    
    for(int i = 0; i < envelopeSliders.size(); i++) envelopeSliders[i]->addMyListener(this);
    
//    midiOutputSelectCB.addModdableComponentListener(this);
//    modeSelectCB.addModdableComponentListener(this);
//    onOffSelectCB.addModdableComponentListener(this);
//    offsetParamStartToggle.addModdableComponentListener(this);
    howManySlider->addModdableComponentListener(this);
    clusterThreshSlider->addModdableComponentListener(this);
//    clusterMinMaxSlider->addModdableComponentListener(this);
    clusterCapSlider->addModdableComponentListener(this);
//    holdTimeMinMaxSlider->addModdableComponentListener(this);
//    velocityMinMaxSlider->addModdableComponentListener(this);
    gainSlider->addModdableComponentListener(this);
    numClusterSlider->addModdableComponentListener(this);
    blendronicGainSlider->addModdableComponentListener(this);
//    transpUsesTuning.addModdableComponentListener(this);
    
//    for (auto slider : paramSliders) slider->addModdableComponentListener(this);
//
//    for(int i = 0; i < envelopeSliders.size(); i++) envelopeSliders[i]->addModdableComponentListener(this);
    
    alternateMod.addListener(this);
    alternateMod.setVisible(true);
}

void SynchronicModificationEditor::greyOutAllComponents()
{
    midiOutputSelectCB.setAlpha(gModAlpha);
    modeSelectCB.setAlpha(gModAlpha);
    modeLabel.setAlpha(gModAlpha);
    onOffSelectCB.setAlpha(gModAlpha);
    onOffLabel.setAlpha(gModAlpha);
    transpUsesTuning.setAlpha(gModAlpha);
    
    offsetParamStartToggle.setAlpha(gModAlpha);
    
    howManySlider->setDim(gModAlpha);
    clusterThreshSlider->setDim(gModAlpha);
    clusterCapSlider->setDim(gModAlpha);
    gainSlider->setDim(gModAlpha);
    numClusterSlider->setDim(gModAlpha);
    
    blendronicGainSlider->setDim(gModAlpha);
    
    clusterMinMaxSlider->setDim(gModAlpha);
    holdTimeMinMaxSlider->setDim(gModAlpha);
    velocityMinMaxSlider->setDim(gModAlpha);
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        paramSliders[i]->setAlpha(gModAlpha);
    }
    
    for(int i=0; i<envelopeSliders.size(); i++)
    {
        envelopeSliders[i]->setDim(gModAlpha);
    }
    envelopeName.setAlpha(gModAlpha);
}

void SynchronicModificationEditor::highlightModedComponents()
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    if(mod->getDirty(SynchronicMode))             modeSelectCB.setAlpha(1.);
    if(mod->getDirty(SynchronicOnOff))            onOffSelectCB.setAlpha(1.);
    if(mod->getDirty(SynchronicNumPulses))        howManySlider->setBright();
    if(mod->getDirty(SynchronicClusterThresh))    clusterThreshSlider->setBright();
    if(mod->getDirty(SynchronicClusterCap))       clusterCapSlider->setBright();
    if(mod->getDirty(SynchronicClusterMin))       clusterMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicClusterMax))       clusterMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicHoldMin))          holdTimeMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicHoldMax))          holdTimeMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicVelocityMin))      velocityMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicVelocityMax))      velocityMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicBeatsToSkip))      offsetParamStartToggle.setAlpha(1.);
    if(mod->getDirty(SynchronicGain))             gainSlider->setBright();
    if(mod->getDirty(SynchronicNumClusters))      numClusterSlider->setBright();
    if(mod->getDirty(SynchronicTranspUsesTuning)) transpUsesTuning.setAlpha(1.);
    if(mod->getDirty(SynchronicBlendronicGain))   blendronicGainSlider->setBright();

    if (mod->getDirty(SynchronicBeatMultipliers))
    {
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
                paramSliders[i]->setAlpha(1.);
        }
    }
    if (mod->getDirty(SynchronicLengthMultipliers))
    {
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
                paramSliders[i]->setAlpha(1.);
        }
    }
    if (mod->getDirty(SynchronicAccentMultipliers))
    {
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
                paramSliders[i]->setAlpha(1.);
        }
    }
    if (mod->getDirty(SynchronicTranspOffsets))
    {
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
                paramSliders[i]->setAlpha(1.);
        }
    }
    if (mod->getDirty(SynchronicADSRs))
    {
        Array<Array<float>> envs = mod->sADSRs.value;
        for(int i = 0; i < envelopeSliders.size(); i++)
        {
            if(envs[i][4]) envelopeSliders[i]->setBright();
            else envelopeSliders[i]->setDim(gModAlpha);
        }
        envelopeName.setAlpha(1.);
    }
}

void SynchronicModificationEditor::timerCallback()
{

}

void SynchronicModificationEditor::update(NotificationType notify)
{
    // DBG("SynchronicModificationEditor::update " + String((int)notify));
    
    if (processor.updateState->currentModSynchronicId < 0) return;
    
    fillModeSelectCB();
    fillMidiOutputSelectCB();
    
    greyOutAllComponents();
    highlightModedComponents();
    
    selectCB.setSelectedId(processor.updateState->currentModSynchronicId, notify);
    
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    if (mod != nullptr)
    {
        modeSelectCB.setSelectedItemIndex(mod->sMode.value, notify);
        onOffSelectCB.setSelectedItemIndex(mod->onOffMode.value, notify);
        
        gainSlider->setValue(mod->sGain.value, notify);
        blendronicGainSlider->setValue(mod->sBlendronicGain.value, notify);
        
        offsetParamStartToggle.setToggleState(mod->sBeatsToSkip.value, notify);
        howManySlider->setValue(mod->sNumBeats.value, notify);
        clusterThreshSlider->setValue(mod->sClusterThresh.value, notify);
        clusterCapSlider->setValue(mod->sClusterCap.value, notify);
        clusterMinMaxSlider->setMinValue(mod->sClusterMin.value, notify);
        clusterMinMaxSlider->setMaxValue(mod->sClusterMax.value, notify);
        holdTimeMinMaxSlider->setMinValue(mod->holdMin.value, notify);
        holdTimeMinMaxSlider->setMaxValue(mod->holdMax.value, notify);
        velocityMinMaxSlider->setMinValue(mod->velocityMin.value, notify);
        velocityMinMaxSlider->setMaxValue(mod->velocityMax.value, notify);
        numClusterSlider->setValue(mod->numClusters.value, notify);
        transpUsesTuning.setToggleState(mod->sTranspUsesTuning.value, notify);
        alternateMod.setToggleState(mod->altMod, dontSendNotification);
        
        for (int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
            {
                paramSliders[i]->setToOnlyActive(mod->sAccentMultipliers.value,
                                                 mod->sAccentMultipliersStates.value, notify);
            }
            else if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
            {
                paramSliders[i]->setToOnlyActive(mod->sBeatMultipliers.value,
                                                 mod->sBeatMultipliersStates.value, notify);
            }
            else if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
            {
                paramSliders[i]->setToOnlyActive(mod->sLengthMultipliers.value,
                                                 mod->sLengthMultipliersStates.value, notify);
            }
            else if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
            {
                paramSliders[i]->setToOnlyActive(mod->sTransposition.value,
                                                 mod->sTranspositionStates.value, notify);
            }
        }
        
        if(mod->getDirty(SynchronicADSRs))
        {
            Array<Array<float>> fvals = mod->sADSRs.value;
            for(int i=0; i<envelopeSliders.size(); i++)
            {
                if(fvals[i][4] > 0)
                {
                    envelopeSliders[fvals[i][5]]->setValue(fvals[i], notify);
                }
                envelopeSliders[i]->setValue(fvals[i], notify);
            }
        }
        else
        {
            for(int i=0; i<envelopeSliders.size(); i++)
            {
                envelopeSliders[i]->setPassive();
            }
        }
    }

}

void SynchronicModificationEditor::fillMidiOutputSelectCB()
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    midiOutputSelectCB.clear(dontSendNotification);
    
    int Id = 1;
    midiOutputSelectCB.addItem("No MIDI Output Selected", Id);
    midiOutputSelectCB.setItemEnabled(Id++, true);
    for (auto device : processor.getMidiOutputDevices())
    {
        String name = device.name;
        
        if (name != String())  midiOutputSelectCB.addItem(name, Id);
        else                   midiOutputSelectCB.addItem("Device"+String(Id), Id);
        
        if (mod->midiOutput.value != nullptr)
        {
            if (name == mod->midiOutput.value->getName())
            {
                midiOutputSelectCB.setSelectedId(Id, NotificationType::dontSendNotification);
            }
        }
        else midiOutputSelectCB.setSelectedId(1, NotificationType::dontSendNotification);
        midiOutputSelectCB.setItemEnabled(Id++, true);
    }
}

void SynchronicModificationEditor::fillModeSelectCB()
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    modeSelectCB.clear(dontSendNotification);
    
    if (mod->onOffMode.value == KeyOn)
    {
        modeSelectCB.addItem("First Note-On", 1);
        modeSelectCB.addItem("Any Note-On", 2);
        modeSelectCB.addItem("Last Note-Off", 3);
        modeSelectCB.addItem("Any Note-Off", 4);
    }
    else
    {
        modeSelectCB.addItem("First Note-Off", 1);
        modeSelectCB.addItem("Any Note-Off", 2);
        modeSelectCB.addItem("Last Note-Off", 3);
    }
    
    modeSelectCB.setSelectedItemIndex(mod->sMode.value, dontSendNotification);
}

void SynchronicModificationEditor::update()
{
    update(dontSendNotification);
}


void SynchronicModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getSynchronicModifications())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("SynchronicMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeSynchronic, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentSynchronicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}

void SynchronicModificationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    if (!name.compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
    {
        Array<float> accents = mod->sAccentMultipliers.value;
        accents.set(whichSlider, values[0]);
        
        mod->setDirty(SynchronicAccentMultipliers);
        mod->sAccentMultipliers.set(accents);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
    {
        Array<float> beats = mod->sBeatMultipliers.value;
        beats.set(whichSlider, values[0]);
        
        mod->setDirty(SynchronicBeatMultipliers);
        mod->sBeatMultipliers.set(beats);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
    {
        Array<float> lens = mod->sLengthMultipliers.value;
        lens.set(whichSlider, values[0]);
        
        mod->setDirty(SynchronicLengthMultipliers);
        mod->sLengthMultipliers.set(lens);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
    {
        Array<Array<float>> transp = mod->sTransposition.value;
        transp.set(whichSlider, values);
        
        mod->setDirty(SynchronicTranspOffsets);
        mod->sTransposition.set(transp);
    }
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == name) paramSliders[i]->setAlpha(1.);
    }
    
    updateModification();
}

void SynchronicModificationEditor::multiSlidersDidChange(String name, Array<Array<float>> values, Array<bool> states)
{
    /*
    for (int i = 0; i < values.size(); i++)
    {
        for (int j = 0; j < values[i].size(); j++)
            DBG("SynchronicModificationEditor::multiSlidersDidChange: val " + String(i) + " " + String(j) + " " + String(values[i].getUnchecked(j)));
    }
    
    for (int i = 0; i < states.size(); i++)
    {
        DBG("SynchronicModificationEditor::multiSlidersDidChange: state " + String((int)states.getUnchecked(i)));
    }
     */
    
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i=0; i<values.size(); i++) newvals.add(values[i][0]);
    
    if (!name.compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
    {
        mod->sAccentMultipliers.set(newvals);
        mod->sAccentMultipliersStates.set(states);
        mod->setDirty(SynchronicAccentMultipliers);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
    {
        mod->sBeatMultipliers.set(newvals);
        mod->sBeatMultipliersStates.set(states);
        mod->setDirty(SynchronicBeatMultipliers);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
    {
        mod->sLengthMultipliers.set(newvals);
        mod->sLengthMultipliersStates.set(states);
        mod->setDirty(SynchronicLengthMultipliers);
    }
    //pass original 2D array for transpositions
    else if (!name.compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
    {
        mod->sTransposition.set(values);
        mod->sTranspositionStates.set(states);
        mod->setDirty(SynchronicTranspOffsets);
    }
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == name) paramSliders[i]->setAlpha(1.);
    }
    
    updateModification();
}

/*
void SynchronicModificationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i=0; i<values.size(); i++) newvals.add(values[i][0]);
    
    if (!name.compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
    {
        mod->setAccentMultipliers(newvals);
        mod->setDirty(SynchronicAccentMultipliers);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
    {
        mod->setBeatMultipliers(newvals);
        mod->setDirty(SynchronicBeatMultipliers);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
    {
        mod->setLengthMultipliers(newvals);
        mod->setDirty(SynchronicLengthMultipliers);
    }
    //pass original 2D array for transpositions
    else if (!name.compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
    {
        mod->setTransposition(values);
        mod->setDirty(SynchronicTranspOffsets);
    }
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == name) paramSliders[i]->setAlpha(1.);
    }
    
    updateModification();
}
 */

void SynchronicModificationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    GeneralSettings::Ptr genmod = processor.gallery->getGeneralSettings();
    
    if(name == "num pulses")
    {
        mod->sNumBeats.set(val);
        mod->setDirty(SynchronicNumPulses);
        
        howManySlider->setBright();
    }
    else if(name == "cluster threshold")
    {
        mod->sClusterThresh.set(val);
        mod->setDirty(SynchronicClusterThresh);
        
        clusterThreshSlider->setBright();
    }
    else if(name == "cluster thickness")
    {
        mod->sClusterCap.set(val);
        mod->setDirty(SynchronicClusterCap);
        
        clusterCapSlider->setBright();
    }
    else if(slider->getName() == gainSlider->getName())
    {
        mod->sGain.set(val);
        
        mod->setDirty(SynchronicGain);
        gainSlider->setBright();
    }
    else if(name == "num clusters")
    {
        mod->numClusters.set(val);
        mod->setDirty(SynchronicNumClusters);
        numClusterSlider->setBright();
    }
    else if(slider->getName() == blendronicGainSlider->getName())
    {
        mod->sBlendronicGain.set(val);
        
        mod->setDirty(SynchronicBlendronicGain);
        blendronicGainSlider->setBright();
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    if(name == "cluster min/max")
    {
        mod->sClusterMin.set(minval);
        mod->setDirty(SynchronicClusterMin);
        
        mod->sClusterMax.set(maxval);
        mod->setDirty(SynchronicClusterMax);
        
        clusterMinMaxSlider->setBright();
    }
    else if (name == "hold min/max")
    {
        mod->holdMin.set(minval);
        mod->setDirty(SynchronicHoldMin);
        
        mod->holdMax.set(maxval);
        mod->setDirty(SynchronicHoldMax);
        
        holdTimeMinMaxSlider->setBright();
    }
    else if (name == "velocity min/max")
    {
        mod->setVelocityMin(minval);
        mod->setDirty(SynchronicVelocityMin);
        
        mod->setVelocityMax(maxval);
        mod->setDirty(SynchronicVelocityMax);
        
        velocityMinMaxSlider->setBright();
    }
    
    updateModification();
}


int SynchronicModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeSynchronicMod);
    
    return processor.gallery->getSynchronicModifications().getLast()->getId();
}

int SynchronicModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeSynchronicMod, processor.updateState->currentModSynchronicId);
    
    return processor.gallery->getSynchronicModifications().getLast()->getId();
}

void SynchronicModificationEditor::deleteCurrent(void)
{
    int oldId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeSynchronicMod, oldId);
    
    fillSelectCB(0, 0);
    
    selectCB.setSelectedItemIndex(0, dontSendNotification);
    int newId = selectCB.getSelectedId();
    
    setCurrentId(newId);
}

void SynchronicModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentModSynchronicId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void SynchronicModificationEditor::actionButtonCallback(int action, SynchronicModificationEditor* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("New Synchronic Modification");
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("Duplicate Synchronic Modification");
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
        processor.saveGalleryToHistory("Delete Synchronic Modification");
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeSynchronicMod, processor.updateState->currentModSynchronicId);
        vc->update();
        vc->updateModification();
        processor.saveGalleryToHistory("Clear Synchronic Modification");
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModSynchronicId;
        SynchronicModification::Ptr prep = processor.gallery->getSynchronicModification(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->setName(name);
            vc->fillSelectCB(Id, Id);
            processor.saveGalleryToHistory("Rename Synchronic Modification");
        }
        
        vc->update();
    }
    else if (action == 7)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModSynchronicId;
        SynchronicModification::Ptr prep = processor.gallery->getSynchronicModification(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeSynchronicMod, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeSynchronicMod, processor.updateState->currentModSynchronicId, which);
        vc->update();
        processor.saveGalleryToHistory("Import Synchronic Modification");
    }
}


void SynchronicModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    DBG("SynchronicModificationEditor::bkComboBoxDidChange " + box->getName());
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == "Mode")
    {
        SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
        
        mod->sMode.set((SynchronicSyncMode) index);
        mod->setDirty(SynchronicMode);
        
        // DO WE NEED THIS HERE IN MOD?
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        mod->sBeatsToSkip.set(toggleVal);
        mod->setDirty(SynchronicBeatsToSkip);
        
        updateModification();
        modeSelectCB.setAlpha(1.);
        
    }
    else if (name == "OnOff")
    {
        SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
        
        mod->onOffMode.set((SynchronicOnOffMode) index);
        mod->setDirty(SynchronicOnOff);
        
        updateModification();
        onOffSelectCB.setAlpha(1.);
        
        fillModeSelectCB();
    }
    else if (name == "MIDI Output")
    {
        SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
        
        if (index > 0)
        {
            mod->setMidiOutput(processor.getMidiOutputDevices()[index-1]);
            mod->setDirty(SynchronicMidiOutput);
        }
        else
        {
            mod->setMidiOutput(nullptr);
            mod->setDirty(SynchronicMidiOutput);
        }
        
        fillMidiOutputSelectCB();
    }
    
    processor.updateState->editsMade = true;
}

void SynchronicModificationEditor::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    if (name == "Name")
    {
        processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId)->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    mod->setName(name);
    
    updateModification();
}

void SynchronicModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        update();
    }
}

void SynchronicModificationEditor::buttonClicked (Button* b)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    if (b == &offsetParamStartToggle)
    {
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        mod->sBeatsToSkip.set(toggleVal);
        mod->setDirty(SynchronicBeatsToSkip);
        
        offsetParamStartToggle.setAlpha(1.);
        
    }
    else if (b == &releaseVelocitySetsSynchronicToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        // this for MODS
        // unimplemented for now
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
        
    }
    else if (b == &actionButton)
    {
        bool single = processor.gallery->getSynchronicModifications().size() == 2;
        getModOptionMenu(PreparationTypeSynchronicMod, single).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    else if (b == &rightArrow)
    {
        arrowPressed(RightArrow);
        
        DBG("currentTab: " + String(currentTab));
        
        displayTab(currentTab);
    }
    else if (b == &leftArrow)
    {
        arrowPressed(LeftArrow);
        
        DBG("currentTab: " + String(currentTab));
        
        displayTab(currentTab);
    }
    else if (b == &transpUsesTuning)
    {
        SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
        mod->sTranspUsesTuning.set(transpUsesTuning.getToggleState());
        mod->setDirty(SynchronicTranspUsesTuning);
        transpUsesTuning.setAlpha(1.);
    }
    else if (b == &alternateMod)
    {
        SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
        mod->altMod = alternateMod.getToggleState();
    }
    
    updateModification();
}

void SynchronicModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
    
    processor.updateState->editsMade = true;
}

void SynchronicModificationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);

    for(int i=0; i<envelopeSliders.size(); i++)
    {
        if(envelopeSliders[i]->getName() == name)
        {
            mod->sADSRs.base.set(i, Array<float>(attack, decay, sustain, release, true));
            mod->sADSRs.value.set(i, Array<float>(attack, decay, sustain, release, true));
            envelopeSliders[i]->setActive();
            envelopeSliders[i]->setBright();
        }
        else
        {  //A, D, S,  R,  inactive, which
            if(!mod->sADSRs.value[i][4])
            {
                mod->sADSRs.base.set(i, Array<float>(3, 3, 1, 30, false));
                mod->sADSRs.value.set(i, Array<float>(3, 3, 1, 30, false));
            }
        }
    }
    mod->setDirty(SynchronicADSRs);
    
    updateModification();
}

void SynchronicModificationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    DBG("SynchronicModificationEditor::BKADSRButtonStateChanged " + String((int)state));
    setShowADSR(name, !state);
    setSubWindowInFront(!state);
}
