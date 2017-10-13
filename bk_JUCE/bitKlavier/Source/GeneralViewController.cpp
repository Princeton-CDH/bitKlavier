/*
  ==============================================================================

    GeneralViewController.cpp
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "GeneralViewController.h"

//==============================================================================
GeneralViewController::GeneralViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph) 
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::general_icon_png, BinaryData::general_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    A4tuningReferenceFrequencySlider = new BKSingleSlider("A4 reference frequency", 415., 450., 440., 0.1);
    //A4tuningReferenceFrequencySlider->setSkewFactorFromMidPoint(1.);
    A4tuningReferenceFrequencySlider->setJustifyRight(false);
    A4tuningReferenceFrequencySlider->addMyListener(this);
    addAndMakeVisible(A4tuningReferenceFrequencySlider);
    
    tempoMultiplierSlider = new BKSingleSlider("tempo multiplier", 0.25, 4., 1., 0.01);
    tempoMultiplierSlider->setSkewFactorFromMidPoint(1.);
    tempoMultiplierSlider->setJustifyRight(false);
    tempoMultiplierSlider->addMyListener(this);
    addAndMakeVisible(tempoMultiplierSlider);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.setButtonText(" X ");
    hideOrShow.addListener(this);
    
    /*
    // Labels
    generalL = OwnedArray<BKLabel>();
    generalL.ensureStorageAllocated(cGeneralParameterTypes.size());
    
    for (int i = 0; i < cGeneralParameterTypes.size(); i++)
    {
        generalL.set(i, new BKLabel());
        addAndMakeVisible(generalL[i]);
        generalL[i]->setName(cGeneralParameterTypes[i]);
        generalL[i]->setText(cGeneralParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    generalTF = OwnedArray<BKTextField>();
    generalTF.ensureStorageAllocated(cGeneralParameterTypes.size());
    
    for (int i = 0; i < cGeneralParameterTypes.size(); i++)
    {
        generalTF.set(i, new BKTextField());
        addAndMakeVisible(generalTF[i]);
        generalTF[i]->addListener(this);
        generalTF[i]->setName(cGeneralParameterTypes[i]);
    }
     */
    
    update();
    
}

GeneralViewController::~GeneralViewController()
{
}

void GeneralViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void GeneralViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - processor.uiMinWidth) / (processor.uiWidth - processor.uiMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - processor.uiMinHeight) / (processor.uiHeight - processor.uiMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    
    Rectangle<int> sliderSlice = leftColumn;
    sliderSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    
    int nextCenter = sliderSlice.getY() + sliderSlice.getHeight() / 4.;
    A4tuningReferenceFrequencySlider->setBounds(sliderSlice.getX(),
                                   nextCenter - gComponentSingleSliderHeight/2 + 8,
                                   sliderSlice.getWidth(),
                                   gComponentSingleSliderHeight);
    
    nextCenter = sliderSlice.getY() + sliderSlice.getHeight() / 2.;
    tempoMultiplierSlider->setBounds(sliderSlice.getX(),
                                          nextCenter - gComponentSingleSliderHeight/2 + 8,
                                          sliderSlice.getWidth(),
                                          gComponentSingleSliderHeight);
    
    /*
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;

    for (int n = 0; n < cGeneralParameterTypes.size(); n++)
    {
        generalL[n]->setTopLeftPosition(lX, gYSpacing + n * lY);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;

    for (int n = 0; n < cGeneralParameterTypes.size(); n++)
    {
        generalTF[n]->setTopLeftPosition(tfX, gYSpacing + n * tfY);
    }
     */
    
}

void GeneralViewController::bkTextFieldDidChange(TextEditor& tf)
{
    /*
    String text = tf.getText();
    String name = tf.getName();
    
    DBG(name + ": |" + text + "|");
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    
    if (name == cGeneralParameterTypes[GeneralTuningFundamental])
    {
        gen->setTuningFundamental(f);
    }
    else if (name == cGeneralParameterTypes[GeneralGlobalGain])
    {
        gen->setGlobalGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralSynchronicGain])
    {
        gen->setSynchronicGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralNostalgicGain])
    {
        gen->setNostalgicGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralDirectGain])
    {
        gen->setDirectGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralResonanceGain])
    {
        gen->setResonanceGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralHammerGain])
    {
        gen->setHammerGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralTempoMultiplier])
    {
        gen->setTempoMultiplier(f);
    }
    else if (name == cGeneralParameterTypes[GeneralResAndHammer])
    {
        gen->setResonanceAndHammer((bool)i);
    }
    else if (name == cGeneralParameterTypes[GeneralInvertSustain])
    {
        gen->setInvertSustain((bool)i);
    }
    else
    {
        // No other text field.
    }
     */
}

void GeneralViewController::update(void)
{
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    
    //generalTF[GeneralTuningFundamental] ->setText(   String( gen->getTuningFundamental()));
    //generalTF[GeneralGlobalGain]        ->setText(   String( gen->getGlobalGain()));
    //generalTF[GeneralSynchronicGain]    ->setText(   String( gen->getSynchronicGain()));
    //generalTF[GeneralNostalgicGain]     ->setText(   String( gen->getNostalgicGain()));
    //generalTF[GeneralDirectGain]        ->setText(   String( gen->getDirectGain()));
    //generalTF[GeneralResonanceGain]     ->setText(   String( gen->getResonanceGain()));
    //generalTF[GeneralHammerGain]        ->setText(   String( gen->getHammerGain()));
    //generalTF[GeneralTempoMultiplier]   ->setText(   String( gen->getTempoMultiplier()));
    //generalTF[GeneralResAndHammer]      ->setText(   String( gen->getResonanceAndHammer()));
    //generalTF[GeneralInvertSustain]     ->setText(   String( gen->getInvertSustain()));
    
    A4tuningReferenceFrequencySlider->setValue(gen->getTuningFundamental(), dontSendNotification);
    tempoMultiplierSlider->setValue(gen->getTempoMultiplier(), dontSendNotification);
    
}

void GeneralViewController::BKSingleSliderValueChanged(String name, double val)
{
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();

    if(name == A4tuningReferenceFrequencySlider->getName())
    {
        DBG("general tuning fundamental " + String(val));
        gen->setTuningFundamental(val);
    }
    else if(name == tempoMultiplierSlider->getName())
    {
        DBG("general tempo multiplier " + String(val));
        gen->setTempoMultiplier(val);
    }
}

void GeneralViewController::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}
