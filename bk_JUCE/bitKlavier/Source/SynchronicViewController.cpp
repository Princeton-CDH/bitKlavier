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
BKViewController(p, theGraph, 2)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::synchronic_icon_png, BinaryData::synchronic_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    // MultSliders
    paramSliders = OwnedArray<BKMultiSlider>();

    int idx = 0;
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        if ((cSynchronicDataTypes[i] == BKFloatArr || cSynchronicDataTypes[i] == BKArrFloatArr) && cSynchronicParameterTypes[i] != "ADSRs")
        {
            paramSliders.insert(idx, new BKMultiSlider(HorizontalMultiBarSlider));
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
    for(int i=0; i<12; i++)
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
    
    selectCB.setName("Synchronic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedId(1, dontSendNotification);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(&selectCB, ALL);
    
    // MODE
    modeSelectCB.setName("Mode");
    modeSelectCB.addSeparator();
    modeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    modeSelectCB.setSelectedItemIndex(0);
    modeSelectCB.setTooltip("Determines which aspect of MIDI signal triggers the Synchronic sequence");
    
    
    addAndMakeVisible(&modeSelectCB, ALL);
    
    modeLabel.setText("triggers pulse", dontSendNotification);
    modeLabel.setTooltip("Determines which aspect of MIDI signal triggers the Synchronic sequence");
    addAndMakeVisible(&modeLabel, ALL);
    
    // ON OFF
    onOffSelectCB.setName("OnOff");
    onOffSelectCB.addSeparator();
    onOffSelectCB.BKSetJustificationType(juce::Justification::centredRight);
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
    buttonsAndMenusLAF.setToggleBoxTextToRightBool(false);
    offsetParamStartToggle.setToggleState (true, dontSendNotification);
    addAndMakeVisible(&offsetParamStartToggle, ALL);
    
    howManySlider = new BKSingleSlider("num pulses", 1, 100, 20, 1);
    howManySlider->setToolTipString("Indicates number of steps/repetitions in Synchronic pulse");
    howManySlider->setJustifyRight(false);
    addAndMakeVisible(howManySlider, ALL);
    
    clusterThreshSlider = new BKSingleSlider("cluster threshold", 20, 2000, 200, 10);
    clusterThreshSlider->setToolTipString("Indicates window of time (milliseconds) within which notes are grouped as a cluster");
    clusterThreshSlider->setJustifyRight(true);
    addAndMakeVisible(clusterThreshSlider, ALL);
    
    clusterMinMaxSlider = new BKRangeSlider("cluster min/max", 1, 12, 3, 4, 1);
    clusterMinMaxSlider->setToolTipString("Sets Min and Max numbers of keys pressed to launch pulse; Min can be greater than Max");
    clusterMinMaxSlider->setJustifyRight(true);
    addAndMakeVisible(clusterMinMaxSlider, ALL);
    
    holdTimeMinMaxSlider = new BKRangeSlider("hold min/max", 0., 12000., 0.0, 12000., 1);
    holdTimeMinMaxSlider->setToolTipString("Sets Min and Max time (ms) held to trigger pulses; Min can be greater than Max");
    holdTimeMinMaxSlider->setJustifyRight(true);
    addAndMakeVisible(holdTimeMinMaxSlider, ALL);
    
    velocityMinMaxSlider = new BKRangeSlider("velocity min/max", 0, 127, 0, 127, 1);
    velocityMinMaxSlider->setToolTipString("Sets Min and Max velocity (0-127) to trigger pulses; Min can be greater than Max");
    velocityMinMaxSlider->setJustifyRight(true);
    addAndMakeVisible(velocityMinMaxSlider, ALL);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.0001);
    gainSlider->setToolTipString("Overall volume of Synchronic pulse");
    gainSlider->setJustifyRight(false);
    gainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(gainSlider, ALL);
    
    numClusterSlider = new BKSingleSlider("num layers", 1, 10, 1, 1);
    numClusterSlider->setToolTipString("Number of simultaneous layers of pulses");
    numClusterSlider->setJustifyRight(false);
    addAndMakeVisible(numClusterSlider, ALL);
    
#if JUCE_IOS
    numClusterSlider->addWantsBigOneListener(this);
    howManySlider->addWantsBigOneListener(this);
    clusterThreshSlider->addWantsBigOneListener(this);
    gainSlider->addWantsBigOneListener(this);
    clusterMinMaxSlider->addWantsBigOneListener(this);
    holdTimeMinMaxSlider->addWantsBigOneListener(this);
    velocityMinMaxSlider->addWantsBigOneListener(this);
    
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
    
    envelopeName.setVisible(false);
    for(int i=envelopeSliders.size() - 1; i>=0; i--)
    {
        envelopeSliders[i]->setVisible(false);
    }
    
    clusterMinMaxSlider->setVisible(false);
    
    clusterThreshSlider->setVisible(false);
    
    holdTimeMinMaxSlider->setVisible(false);
    
    velocityMinMaxSlider->setVisible(false);
    
    numClusterSlider->setVisible(false);
    
    howManySlider->setVisible(false);
    
    modeLabel.setVisible(false);
    modeSelectCB.setVisible(false);
    
    onOffLabel.setVisible(false);
    onOffSelectCB.setVisible(false);
    
    releaseVelocitySetsSynchronicToggle.setVisible(false);
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
    
    int col1x = x0;
    int col2x = x0 + width * 0.5f;
    
    if (tab == 0)
    {
        if (showADSR)
        {
            // DEAL WITH ENVELOPE; MOVE THIS TO NEW TAB, include the buttons as well
            for(int i=0; i<envelopeSliders.size(); i++)
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
            for(int i=0; i<envelopeSliders.size(); i++)
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
            
            for (int i = 0; i < paramSliders.size(); i++)
            {
                paramSliders[i]->setVisible(true);
            }
            
            //envelopeName.setVisible(true);
            for(int i=envelopeSliders.size() - 1; i>=0; i--)
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
            
            int envelopeWidth = (sliderWidth - 50) / 12;
            int envelopeHeight = height * 0.1f;
            
            for(int i = 0; i < envelopeSliders.size(); i++)
            {
                envelopeSliders[i]->setBounds(x0 + 50 + i * (envelopeWidth + 0.65), paramSliders.getLast()->getBottom() + gYSpacing,
                                              envelopeWidth, envelopeHeight - gYSpacing);
            }
            
            offsetParamStartToggle.setBounds(right - 100, selectCB.getY(), 100, 30);
        }
        
        iconImageComponent.toBack();
    }
    else if (tab == 1)
    {
        // SET VISIBILITY
        gainSlider->setVisible(true);
        
        clusterMinMaxSlider->setVisible(true);
        
        clusterThreshSlider->setVisible(true);
        
        holdTimeMinMaxSlider->setVisible(true);
        
        velocityMinMaxSlider->setVisible(true);
        
        numClusterSlider->setVisible(true);
        
        howManySlider->setVisible(true);
        
        modeLabel.setVisible(true);
        modeSelectCB.setVisible(true);
        
        onOffLabel.setVisible(true);
        onOffSelectCB.setVisible(true);
        
        iconImageComponent.toBack();
        
        //releaseVelocitySetsSynchronicToggle.setVisible(true);
        
        // SET BOUNDS
        /*
        int sliderHeight = height * 0.2f;
        int sliderWidth = width * 0.5f - gXSpacing;
        
        int idx = 0;
        gainSlider->setBounds(col1x, y0 + (idx++) * sliderHeight, sliderWidth, sliderHeight - gYSpacing);
        clusterThreshSlider->setBounds(col1x, y0 + (idx++) * sliderHeight, sliderWidth, sliderHeight - gYSpacing);
        clusterMinMaxSlider->setBounds(col1x, y0 + (idx++) * sliderHeight, sliderWidth, sliderHeight - gYSpacing);
        holdTimeMinMaxSlider->setBounds(col1x, y0 + (idx++) * sliderHeight, sliderWidth, sliderHeight - gYSpacing);
        velocityMinMaxSlider->setBounds(col1x, y0 + (idx++) * sliderHeight, sliderWidth, sliderHeight - gYSpacing);
        
        idx = 0;
        howManySlider->setBounds(col2x, y0 + (idx++) * sliderHeight, sliderWidth, sliderHeight - gYSpacing);
        numClusterSlider->setBounds(col2x, y0 + (idx++) * sliderHeight, sliderWidth, sliderHeight - gYSpacing);
        
        
        modeSelectCB.setBounds(col2x, numClusterSlider->getBottom() + gYSpacing, 200, 30);
        modeLabel.setBounds(modeSelectCB.getRight()+gXSpacing, modeSelectCB.getY(), 150, 30);
        
        onOffSelectCB.setBounds(col2x, modeSelectCB.getBottom() + 40, 200, 30);
        onOffLabel.setBounds(modeSelectCB.getRight()+gXSpacing, onOffSelectCB.getY(), 150, 30);
        */
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
        
        int columnHeight = leftColumn.getHeight();
        
        Rectangle<int> modeSelectCBRect (leftColumn.removeFromTop(columnHeight / 4));
        Rectangle<int> modeSelectLabel (modeSelectCBRect.removeFromRight(modeSelectCBRect.getWidth()*0.5));
        modeSelectCB.setBounds(modeSelectCBRect.removeFromTop(gComponentComboBoxHeight));
        modeLabel.setBounds(modeSelectLabel.removeFromTop(gComponentComboBoxHeight));
        modeSelectCBRect.removeFromTop(gYSpacing);
        modeSelectLabel.removeFromTop(gYSpacing);
        onOffSelectCB.setBounds(modeSelectCBRect.removeFromTop(gComponentComboBoxHeight));
        onOffLabel.setBounds(modeSelectLabel.removeFromTop(gComponentComboBoxHeight));
        
        /*
        Rectangle<int> onOffSelectCBRect (leftColumn.removeFromTop(columnHeight / 5));
        onOffSelectCB.setBounds(onOffSelectCBRect.removeFromLeft(onOffSelectCBRect.getWidth()*0.5).removeFromTop(gComponentComboBoxHeight));
        onOffLabel.setBounds(onOffSelectCBRect.removeFromTop(gComponentComboBoxHeight));
        */
        
        howManySlider->setBounds(leftColumn.removeFromTop(columnHeight / 4));
        numClusterSlider->setBounds(leftColumn.removeFromTop(columnHeight / 4));
        gainSlider->setBounds(leftColumn.removeFromTop(columnHeight / 4));
        
        clusterThreshSlider->setBounds(area.removeFromTop(columnHeight / 4));
        clusterMinMaxSlider->setBounds(area.removeFromTop(columnHeight / 4));
        holdTimeMinMaxSlider->setBounds(area.removeFromTop(columnHeight / 4));
        velocityMinMaxSlider->setBounds(area.removeFromTop(columnHeight / 4));
        
        
        releaseVelocitySetsSynchronicToggle.setVisible(true);
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
        for(int i=envelopeSliders.size() - 1; i>=0; i--)
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
    
    for(int i=0; i<envelopeSliders.size(); i++)
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
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    modeSelectCB.addListener(this);
    onOffSelectCB.addListener(this);
    offsetParamStartToggle.addListener(this);
    howManySlider->addMyListener(this);
    clusterThreshSlider->addMyListener(this);
    clusterMinMaxSlider->addMyListener(this);
    holdTimeMinMaxSlider->addMyListener(this);
    velocityMinMaxSlider->addMyListener(this);

    gainSlider->addMyListener(this);
    numClusterSlider->addMyListener(this);
    
    for(int i=0; i<envelopeSliders.size(); i++)
    {
        envelopeSliders[i]->addMyListener(this);
    }
    
    envelopeSliders[0]->adsrButton.triggerClick(); //initialize first envelope, since it is always active
    
    startTimer(30);
    
}

void SynchronicPreparationEditor::fillModeSelectCB()
{
    SynchronicPreparation::Ptr prep = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    modeSelectCB.clear();
    
    if (prep->getOnOffMode() == KeyOn)
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
    
    modeSelectCB.setSelectedItemIndex(prep->getMode(), dontSendNotification);
}


void SynchronicPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplaySynchronic)
    {
        SynchronicProcessor::Ptr sProcessor = processor.currentPiano->getSynchronicProcessor(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        SynchronicCluster::Ptr cluster = sProcessor->getCluster(0);
        
        if (active->getOnOffMode() == KeyOn)
        {
            if (active->getMode() == AnyNoteOffSync || active->getMode() == LastNoteOffSync)
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
        
        fillModeSelectCB();
        
        if (cluster == nullptr) return;
        
        if(cluster->getBeatCounter() < active->getNumBeats() && sProcessor->getPlayCluster())
            howManySlider->setDisplayValue(cluster->getBeatCounter());
        else howManySlider->setDisplayValue(0);
        
        if(sProcessor->getClusterThresholdTimer() < active->getClusterThreshMS())
             clusterThreshSlider->setDisplayValue(sProcessor->getClusterThresholdTimer());
        else clusterThreshSlider->setDisplayValue(0);
        
        int maxTemp = 12; //arbitrary
        if(active->getClusterMax() > active->getClusterMin()) maxTemp = active->getClusterMax();
        if(sProcessor->getNumKeysDepressed() <= maxTemp && sProcessor->getClusterThresholdTimer() < active->getClusterThreshMS())
            clusterMinMaxSlider->setDisplayValue(sProcessor->getNumKeysDepressed());
        else clusterMinMaxSlider->setDisplayValue(0);
        
        int counter = 0, size = 0;
        
        if (sProcessor != nullptr)
        {
            for (int i = 0; i < paramSliders.size(); i++)
            {
                if(paramSliders[i]->getName() == "beat length multipliers")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = cluster->getBeatMultiplierCounter();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "sustain length multipliers")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = cluster->getLengthMultiplierCounter();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "accents")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = cluster->getAccentMultiplierCounter();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "transpositions")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = cluster->getTranspCounter();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
            }
            
            SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
            for(int i = 0; i < envelopeSliders.size(); i++)
            {
                if(i == cluster->getEnvelopeCounter()) envelopeSliders[i]->setHighlighted();
                
                else if(active->getEnvelopeOn(i))
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
    }
}


void SynchronicPreparationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (name == "accents")
    {
        prep    ->setAccentMultiplier(whichSlider, values[0]);
        active  ->setAccentMultiplier(whichSlider, values[0]);
    }
    else if (name == "beat length multipliers")
    {
        prep    ->setBeatMultiplier(whichSlider, values[0]);
        active  ->setBeatMultiplier(whichSlider, values[0]);
    }
    else if (name == "sustain length multipliers")
    {
        prep    ->setLengthMultiplier(whichSlider, values[0]);
        active  ->setLengthMultiplier(whichSlider, values[0]);
    }
    else if (name == "transpositions")
    {
        prep    ->setSingleTransposition(whichSlider, values);
        active  ->setSingleTransposition(whichSlider, values);
    }
    
    //processor.updateState->synchronicPreparationDidChange = true;

}

void SynchronicPreparationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i=0; i<values.size(); i++) newvals.add(values[i][0]);

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

void SynchronicPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if(name == "num pulses") {
        DBG("got new how many " + String(val));
        prep->setNumBeats(val);
        active->setNumBeats(val);
    }
    else if(name == "cluster threshold")
    {
        DBG("got new cluster threshold " + String(val));
        prep->setClusterThresh(val);
        active->setClusterThresh(val);
    }
    else if(name == "gain")
    {
        DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
    else if(name == "num clusters")
    {
        DBG("num clusters " + String(val));
        prep->setNumClusters(val);
        active->setNumClusters(val);
    }
}

void SynchronicPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if(name == "cluster min/max") {
        DBG("got new cluster min/max " + String(minval) + " " + String(maxval));
        prep->setClusterMin(minval);
        prep->setClusterMax(maxval);
        active->setClusterMin(minval);
        active->setClusterMax(maxval);
    }
    else if(name == "hold min/max") {
        DBG("got new hold min/max " + String(minval) + " " + String(maxval));
        prep->setHoldMin(minval);
        prep->setHoldMax(maxval);
        active->setHoldMin(minval);
        active->setHoldMax(maxval);
    }
    else if(name == "velocity min/max") {
        DBG("got new velocity min/max " + String(minval) + " " + String(maxval));
        prep->setVelocityMin(minval);
        prep->setVelocityMax(maxval);
        active->setVelocityMin(minval);
        active->setVelocityMax(maxval);
    }
}

void SynchronicPreparationEditor::update(NotificationType notify)
{
    if (processor.updateState->currentSynchronicId < 0) return;
    
    SynchronicPreparation::Ptr prep   = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (prep != nullptr)
    {
        
        selectCB.setSelectedId(processor.updateState->currentSynchronicId, notify);
        modeSelectCB.setSelectedItemIndex(prep->getMode(), notify);
        onOffSelectCB.setSelectedItemIndex(prep->getOnOffMode(), notify);
        offsetParamStartToggle.setToggleState(prep->getOffsetParamToggle(), notify);
        releaseVelocitySetsSynchronicToggle.setToggleState(prep->getReleaseVelocitySetsSynchronic(), notify);
        howManySlider->setValue(prep->getNumBeats(), notify);
        clusterThreshSlider->setValue(prep->getClusterThreshMS(), notify);
        clusterMinMaxSlider->setMinValue(prep->getClusterMin(), notify);
        clusterMinMaxSlider->setMaxValue(prep->getClusterMax(), notify);
        
        holdTimeMinMaxSlider->setMinValue(prep->getHoldMin(), notify);
        holdTimeMinMaxSlider->setMaxValue(prep->getHoldMax(), notify);
        
        velocityMinMaxSlider->setMinValue(prep->getVelocityMin(), notify);
        velocityMinMaxSlider->setMaxValue(prep->getVelocityMax(), notify);
        
        gainSlider->setValue(prep->getGain(), notify);
        numClusterSlider->setValue(prep->getNumClusters(), notify);
        
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
            {
                paramSliders[i]->setTo(prep->getAccentMultipliers(), notify);
            }
            
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
            {
                paramSliders[i]->setTo(prep->getBeatMultipliers(), notify);
            }
            
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
            {
                paramSliders[i]->setTo(prep->getLengthMultipliers(), notify);
            }
            
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
            {
                paramSliders[i]->setTo(prep->getTransposition(), notify);
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
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    prep->setAttack(which, attack);
    active->setAttack(which, attack);
    prep->setDecay(which, decay);
    active->setDecay(which, decay);
    prep->setSustain(which, sustain);
    active->setSustain(which, sustain);
    prep->setRelease(which, release);
    active->setRelease(which, release);
    
    prep->setEnvelopeOn(which, true);
    active->setEnvelopeOn(which, true);
    envelopeSliders[which]->setBright();
    
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
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        if(which != 0) //first envelope is always on...
        {
            //DBG("toggling " + String(which) + " " + String((int)state));
            prep->setEnvelopeOn(which, state);
            active->setEnvelopeOn(which, state);
            if(state) envelopeSliders[which]->setBright();
            else envelopeSliders[which]->setDim(gModAlpha);
            
        }
        else
        {
            envelopeSliders[0]->setButtonToggle(true);
            envelopeSliders[0]->setBright();
        }
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
        
        if (name != String::empty)  selectCB.addItem(name, Id);
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
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentSynchronicId = -1;
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
    BKAudioProcessor& processor = vc->processor;
    
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
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
        }
        
        vc->update();
    }
}


void SynchronicPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == "Mode")
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep    ->setMode((SynchronicSyncMode) index);
        active  ->setMode((SynchronicSyncMode) index);
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(prep->getMode() == FirstNoteOnSync || prep->getMode() == AnyNoteOnSync)
        {
            prep->setBeatsToSkip(toggleVal - 1);
            active->setBeatsToSkip(toggleVal - 1);
        }
        else
        {
            prep->setBeatsToSkip(toggleVal);
            active->setBeatsToSkip(toggleVal);
        }
    }
    else if (name == "OnOff")
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep    ->setOnOffMode( (SynchronicOnOffMode) index);
        active  ->setOnOffMode( (SynchronicOnOffMode) index);
        
        fillModeSelectCB();
        
    }
}

void SynchronicPreparationEditor::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    //SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    //SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (name == "Name")
    {
        Synchronic::Ptr synchronic = processor.gallery->getSynchronic(processor.updateState->currentSynchronicId);
        
        synchronic->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
}

void SynchronicPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Synchronic::Ptr synchronic = processor.gallery->getSynchronic(processor.updateState->currentSynchronicId);
    
    synchronic->setName(name);
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
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(prep->getMode() == FirstNoteOnSync || prep->getMode() == AnyNoteOnSync)
        {
            prep->setBeatsToSkip(toggleVal - 1);
            active->setBeatsToSkip(toggleVal - 1);
        }
        else
        {
            prep->setBeatsToSkip(toggleVal);
            active->setBeatsToSkip(toggleVal);
        }

    }
    else if (b == &releaseVelocitySetsSynchronicToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep->setReleaseVelocitySetsSynchronic(releaseVelocitySetsSynchronicToggle.getToggleState());
        active->setReleaseVelocitySetsSynchronic(releaseVelocitySetsSynchronicToggle.getToggleState());
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
        getPrepOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
}



// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SynchronicModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
SynchronicModificationEditor::SynchronicModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
SynchronicViewController(p, theGraph)
{
    greyOutAllComponents();
    
    fillSelectCB(-1,-1);
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    modeSelectCB.addListener(this);
    onOffSelectCB.addListener(this);
    offsetParamStartToggle.addListener(this);
    howManySlider->addMyListener(this);
    clusterThreshSlider->addMyListener(this);
    clusterMinMaxSlider->addMyListener(this);
    holdTimeMinMaxSlider->addMyListener(this);
    velocityMinMaxSlider->addMyListener(this);
    gainSlider->addMyListener(this);
    numClusterSlider->addMyListener(this);
    
    howManySlider->displaySliderVisible(false);
    clusterThreshSlider->displaySliderVisible(false);
    clusterMinMaxSlider->displaySliderVisible(false);
    holdTimeMinMaxSlider->displaySliderVisible(false);
    velocityMinMaxSlider->displaySliderVisible(false);
    
    for(int i = 0; i < envelopeSliders.size(); i++) envelopeSliders[i]->addMyListener(this);

}

void SynchronicModificationEditor::greyOutAllComponents()
{
    modeSelectCB.setAlpha(gModAlpha);
    onOffSelectCB.setAlpha(gModAlpha);
    offsetParamStartToggle.setAlpha(gModAlpha);
    
    howManySlider->setDim(gModAlpha);
    clusterThreshSlider->setDim(gModAlpha);
    clusterThreshSlider->setDim(gModAlpha);
    gainSlider->setDim(gModAlpha);
    numClusterSlider->setDim(gModAlpha);
    
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
    if(mod->getDirty(SynchronicClusterMin))       clusterMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicClusterMax))       clusterMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicHoldMin))          holdTimeMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicHoldMax))          holdTimeMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicVelocityMin))      velocityMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicVelocityMax))      velocityMinMaxSlider->setBright();
    if(mod->getDirty(SynchronicBeatsToSkip))      offsetParamStartToggle.setAlpha(1.);
    if(mod->getDirty(SynchronicGain))             gainSlider->setBright();
    if(mod->getDirty(SynchronicNumClusters))      numClusterSlider->setBright();

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
        Array<Array<float>> envs = mod->getADSRs();
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
    if (processor.updateState->currentModSynchronicId < 0) return;
    
    fillModeSelectCB();
    
    greyOutAllComponents();
    highlightModedComponents();
    
    selectCB.setSelectedId(processor.updateState->currentModSynchronicId, notify);
    
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    if (mod != nullptr)
    {
        modeSelectCB.setSelectedItemIndex(mod->getMode(), notify);
        
        onOffSelectCB.setSelectedItemIndex(mod->getOnOffMode(), notify);
        
        //FIXIT offsetParamStartToggle.setToggleState(prep->getOffsetParamToggle(), notify);
        //SynchronicBeatsToSkip determines whether to set this toggle
        offsetParamStartToggle.setToggleState(mod->getBeatsToSkip() + 1, notify);

        howManySlider->setValue(mod->getNumBeats(), notify);
        
        clusterThreshSlider->setValue(mod->getClusterThreshMS(), notify);
        
        clusterMinMaxSlider->setMinValue(mod->getClusterMin(), notify);
        
        clusterMinMaxSlider->setMaxValue(mod->getClusterMax(), notify);
        
        holdTimeMinMaxSlider->setMinValue(mod->getHoldMin(), notify);
        
        holdTimeMinMaxSlider->setMaxValue(mod->getHoldMax(), notify);
        
        velocityMinMaxSlider->setMinValue(mod->getVelocityMin(), notify);
        
        velocityMinMaxSlider->setMaxValue(mod->getVelocityMax(), notify);
        
        gainSlider->setValue(mod->getGain(), notify);

        numClusterSlider->setValue(mod->getNumClusters(), notify);
        
        for (int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicAccentMultipliers]))
            {
                paramSliders[i]->setTo(mod->getAccentMultipliers(), notify);
            }
            else if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
            {
                paramSliders[i]->setTo(mod->getBeatMultipliers(), notify);
            }
            else if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
            {
                paramSliders[i]->setTo(mod->getLengthMultipliers(), notify);
            }
            else if(!paramSliders[i]->getName().compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
            {
                paramSliders[i]->setTo(mod->getTransposition(), notify);
            }
        }
        
        if(mod->getDirty(SynchronicADSRs))
        {
            Array<Array<float>> fvals = mod->getADSRs();
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

void SynchronicModificationEditor::fillModeSelectCB()
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    modeSelectCB.clear();
    
    if (mod->getOnOffMode() == KeyOn)
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
    
    modeSelectCB.setSelectedItemIndex(mod->getMode(), dontSendNotification);
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
        
        if (name != String::empty)  selectCB.addItem(name, Id);
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
        Array<float> accents = mod->getAccentMultipliers();
        accents.set(whichSlider, values[0]);
        
        mod->setDirty(SynchronicAccentMultipliers);
        mod->setAccentMultipliers(accents);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicBeatMultipliers]))
    {
        Array<float> beats = mod->getBeatMultipliers();
        beats.set(whichSlider, values[0]);
        
        mod->setDirty(SynchronicBeatMultipliers);
        mod->setBeatMultipliers(beats);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicLengthMultipliers]))
    {
        Array<float> lens = mod->getLengthMultipliers();
        lens.set(whichSlider, values[0]);
        
        mod->setDirty(SynchronicLengthMultipliers);
        mod->setLengthMultipliers(lens);
    }
    else if (!name.compare(cSynchronicParameterTypes[SynchronicTranspOffsets]))
    {
        //paramSliders
        Array<Array<float>> transps = mod->getTransposition();
        transps.set(whichSlider, values);
        
        mod->setDirty(SynchronicTranspOffsets);
        mod->setTransposition(transps);
    }
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == name) paramSliders[i]->setAlpha(1.);
    }
    
    updateModification();
    
}

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

void SynchronicModificationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    GeneralSettings::Ptr genmod = processor.gallery->getGeneralSettings();
    
    if(name == "num pulses")
    {
        mod->setNumBeats(val);
        mod->setDirty(SynchronicNumPulses);
        
        howManySlider->setBright();
    }
    else if(name == "cluster threshold")
    {
        mod->setClusterThresh(val);
        mod->setDirty(SynchronicClusterThresh);
        
        clusterThreshSlider->setBright();
    }
    else if(name == "gain")
    {
        mod->setGain(val);
        mod->setDirty(SynchronicGain);
        
        gainSlider->setBright();
    }
    else if(name == "num clusters")
    {
        mod->setNumClusters(val);
        mod->setDirty(SynchronicNumClusters);
        
        numClusterSlider->setBright();
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
    
    if(name == "cluster min/max")
    {
        mod->setClusterMin(minval);
        mod->setDirty(SynchronicClusterMin);
        
        mod->setClusterMax(maxval);
        mod->setDirty(SynchronicClusterMax);
        
        clusterMinMaxSlider->setBright();
    }
    else if (name == "hold min/max")
    {
        mod->setHoldMin(minval);
        mod->setDirty(SynchronicHoldMin);
        
        mod->setHoldMax(maxval);
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
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentModSynchronicId = -1;
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
    BKAudioProcessor& processor = vc->processor;
    
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeSynchronicMod, processor.updateState->currentModSynchronicId);
        vc->update();
        vc->updateModification();
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
        }
        
        vc->update();
    }
}


void SynchronicModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
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
        
        mod->setMode((SynchronicSyncMode) index);
        mod->setDirty(SynchronicMode);
        
        // DO WE NEED THIS HERE IN MOD?
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(mod->getMode() == FirstNoteOnSync || mod->getMode()  == AnyNoteOnSync)
        {
            mod->setBeatsToSkip(toggleVal - 1);
            mod->setDirty(SynchronicBeatsToSkip);
        }
        else
        {
            mod->setBeatsToSkip(toggleVal);
            mod->setDirty(SynchronicBeatsToSkip);
        }
        
        updateModification();
        modeSelectCB.setAlpha(1.);
        
    }
    else if (name == "OnOff")
    {
        SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
        
        mod->setOnOffMode((SynchronicOnOffMode) index);
        mod->setDirty(SynchronicOnOff);
        
        updateModification();
        onOffSelectCB.setAlpha(1.);
        
        fillModeSelectCB();
    }
    
    
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
        
        if(mod->getMode() == FirstNoteOnSync || mod->getMode()  == AnyNoteOnSync)
        {
            mod->setBeatsToSkip(toggleVal - 1);
            mod->setDirty(SynchronicBeatsToSkip);
        }
        else
        {
            mod->setBeatsToSkip(toggleVal);
            mod->setDirty(SynchronicBeatsToSkip);
        }
        
        offsetParamStartToggle.setAlpha(1.);
        
    }
    else if (b == &releaseVelocitySetsSynchronicToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        // this for MODS
        // unimplemented for now
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getModOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    
    updateModification();
}

void SynchronicModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}

void SynchronicModificationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);

    Array<Array<float>> envs = mod->getADSRs();
    for(int i=0; i<envelopeSliders.size(); i++)
    {
        if(envelopeSliders[i]->getName() == name)
        {
            envs.set(i, {(float)attack, decay, sustain, release, 1, i});
            envelopeSliders[i]->setActive();
            envelopeSliders[i]->setBright();
        }
        else
        {                              //A, D, S,  R,  inactive, which
            if(!envs[i][4]) envs.set(i, {3, 3, 1., 30, 0,        i}); //create inactive default
        }
    }
    mod->setADSRs(envs);
    mod->setDirty(SynchronicADSRs);
    
    updateModification();
}

void SynchronicModificationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    DBG("SynchronicModificationEditor::BKADSRButtonStateChanged " + String((int)state));
    setShowADSR(name, !state);
    setSubWindowInFront(!state);
}




