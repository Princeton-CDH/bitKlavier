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
    A4tuningReferenceFrequencySlider->setJustifyRight(false);
    A4tuningReferenceFrequencySlider->addMyListener(this);
    addAndMakeVisible(A4tuningReferenceFrequencySlider);
    
    tempoMultiplierSlider = new BKSingleSlider("tempo multiplier", 0.25, 4., 1., 0.01);
    tempoMultiplierSlider->setSkewFactorFromMidPoint(1.);
    tempoMultiplierSlider->setJustifyRight(false);
    tempoMultiplierSlider->addMyListener(this);
    addAndMakeVisible(tempoMultiplierSlider);
    
    invertSustainB.addListener(this);
    invertSustainB.setToggleState(processor.gallery->getGeneralSettings()->getInvertSustain(), dontSendNotification);
    processor.setSustainInversion(processor.gallery->getGeneralSettings()->getInvertSustain());
    addAndMakeVisible(invertSustainB);
    
    invertSustainL.setText("invert sustain", dontSendNotification);
    addAndMakeVisible(invertSustainL);

#if JUCE_IOS
    tempoMultiplierSlider->addWantsBigOneListener(this);
    A4tuningReferenceFrequencySlider->addWantsBigOneListener(this);
#endif
    
    update();
    
}

#if JUCE_IOS
void GeneralViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

GeneralViewController::~GeneralViewController()
{
    setLookAndFeel(nullptr);
}

void GeneralViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void GeneralViewController::resized()
{
    Rectangle<int> area (getLocalBounds());

    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    
   
    
    A4tuningReferenceFrequencySlider->setBounds(hideOrShow.getX()+gXSpacing, hideOrShow.getBottom()+100,
                                                getWidth()/2.-10, gComponentSingleSliderHeight);
    
    tempoMultiplierSlider->setBounds(A4tuningReferenceFrequencySlider->getX(), A4tuningReferenceFrequencySlider->getBottom()+10,
                                                A4tuningReferenceFrequencySlider->getWidth(), A4tuningReferenceFrequencySlider->getHeight());
    
    invertSustainB.setBounds(tempoMultiplierSlider->getX()+5, tempoMultiplierSlider->getBottom() + 10,
                             tempoMultiplierSlider->getWidth() * 0.25, 30);
    
    invertSustainB.changeWidthToFitText();
    
    invertSustainL.setBounds(invertSustainB.getRight() +gXSpacing, invertSustainB.getY(),
                             150, 30);
    
   
    
    
    /*
    Rectangle<int> sliderSlice = leftColumn;
    sliderSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
     
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
     
    
    nextCenter = sliderSlice.getY() + sliderSlice.getHeight();
    invertSustainL.setBounds(sliderSlice.getX(),
                            nextCenter - gComponentSingleSliderHeight/2 + 8,
                            sliderSlice.getWidth()/2,
                            gComponentSingleSliderHeight*2);
    
    invertSustainB.setBounds(invertSustainL.getRight()+gXSpacing,
                             invertSustainL.getY(), invertSustainL.getHeight(), invertSustainL.getHeight());
     */
}


void GeneralViewController::bkTextFieldDidChange(TextEditor& tf)
{

}

void GeneralViewController::update(void)
{
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    
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
    else if (b == &invertSustainB)
    {
        bool inversion =  (bool) b->getToggleStateValue().toString().getIntValue();
        processor.setSustainInversion(inversion);
        processor.updateSustainState();
        
    }
}
