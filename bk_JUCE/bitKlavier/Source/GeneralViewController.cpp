/*
  ==============================================================================

    GeneralViewController.cpp
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "GeneralViewController.h"

CommentViewController::CommentViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p,theGraph)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    removeChildComponent(&hideOrShow);
    
    addAndMakeVisible(comment);
    comment.setName("comment");
    comment.setMultiLine(true);
    
    comment.setReturnKeyStartsNewLine(true);
    comment.addListener(this);
    comment.setText("Text here...");
#if JUCE_IOS
    comment.setScrollbarsShown(true);
#endif
    //comment.setColour(TextEditor::ColourIds::high, )
    
    comment.setColour(TextEditor::ColourIds::backgroundColourId, Colours::burlywood.withMultipliedBrightness(0.45f));
    comment.setColour(TextEditor::ColourIds::textColourId, Colours::antiquewhite);
    
    comment.setOpaque(false);
    
    addAndMakeVisible(ok);
    ok.setButtonText("Ok");
    ok.addListener(this);
    ok.setColour(TextEditor::ColourIds::backgroundColourId, Colours::black.withAlpha(0.5f));
    
    addAndMakeVisible(cancel);
    cancel.setButtonText("Cancel");
    cancel.addListener(this);
    cancel.setColour(TextEditor::ColourIds::backgroundColourId, Colours::red.withAlpha(0.2f));
}

CommentViewController::~CommentViewController()
{
    setLookAndFeel(nullptr);
}

void CommentViewController::update(void)
{
    comment.setText(processor.updateState->comment, dontSendNotification);
}

void CommentViewController::paint (Graphics& g)
{
    g.fillAll(Colours::transparentBlack);
}

void CommentViewController::resized(void)
{
    float heightUnit = (getHeight() - hideOrShow.getHeight()) * 0.1;
    
#if JUCE_IOS
    cancel.setBounds    (0,                 hideOrShow.getBottom(), getWidth()*0.5,         2*heightUnit);
    ok.setBounds        (getWidth()*0.5,    cancel.getY(),          getWidth()*0.5,     2*heightUnit);
    comment.setBounds   (0,                 cancel.getBottom(),     getWidth(),     8*heightUnit);
#else
    comment.setBounds   (0,                 hideOrShow.getBottom(), getWidth(),         9*heightUnit);
    cancel.setBounds    (comment.getX(),    comment.getBottom(),    getWidth()*0.5,     1*heightUnit);
    ok.setBounds        (cancel.getRight(), cancel.getY(),          getWidth()*0.5,     1*heightUnit);
#endif
}

void CommentViewController::bkTextFieldDidChange (TextEditor& tf)
{

}

void CommentViewController::bkButtonClicked (Button* b)
{
    if (b == &ok)
    {
        processor.updateState->comment = comment.getText();
        processor.updateState->commentDidChange = true;
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &cancel)
    {
        processor.updateState->comment = "";
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}

AboutViewController::AboutViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p,theGraph)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    addAndMakeVisible(about);
    about.setEnabled(false);
    about.setJustification(Justification::centredTop);
    about.setMultiLine(true);
    about.setText("Welcome to bitKlavier!\n\n\n\n   bitKlavier was created by Dan Trueman and Mike Mulshine at Princeton University.\n\n\n\n   For more information, visit www.bitKlavier.com.");
    
    image = ImageCache::getFromMemory(BinaryData::icon_png, BinaryData::icon_pngSize);
    
    placement = RectanglePlacement::centred;
}

AboutViewController::~AboutViewController()
{
    setLookAndFeel(nullptr);
}

void AboutViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    
    g.setOpacity (1.0f);
    
    g.drawImage (image, imageRect, placement);
}

void AboutViewController::resized(void)
{
    hideOrShow.setBounds(10,10,gComponentComboBoxHeight,gComponentComboBoxHeight);
    
    float imageZ = getHeight() * 0.5;
    float imageX = getWidth() * 0.5 - imageZ * 0.5;
    float imageY = 50;
    
    imageRect.setBounds(imageX, imageY, imageZ, imageZ);
    
    about.setBounds(10, imageRect.getBottom() + 20, getWidth() - 20, getBottom() - (imageRect.getBottom() + 20));
    
    repaint();
}

void AboutViewController::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}


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
    
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    
    invertSustainB.addListener(this);
    invertSustainB.setToggleState(gen->getInvertSustain(), dontSendNotification);
    processor.setSustainInversion(gen->getInvertSustain());
    addAndMakeVisible(invertSustainB);
    
    noteOnSetsNoteOffVelocityB.addListener(this);
    noteOnSetsNoteOffVelocityB.setToggleState(gen->getNoteOnSetsNoteOffVelocity(), dontSendNotification);
    addAndMakeVisible(noteOnSetsNoteOffVelocityB);
    
    invertSustainL.setText("invert sustain", dontSendNotification);
    addAndMakeVisible(invertSustainL);
    
    noteOnSetsNoteOffVelocityL.setText("noteOn velocity sets noteOff velocity", dontSendNotification);
    addAndMakeVisible(noteOnSetsNoteOffVelocityL);

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
    
    noteOnSetsNoteOffVelocityB.setBounds(tempoMultiplierSlider->getX()+5, invertSustainL.getBottom() + 10,
                             tempoMultiplierSlider->getWidth() * 0.25, 30);
    noteOnSetsNoteOffVelocityB.changeWidthToFitText();
    noteOnSetsNoteOffVelocityL.setBounds(noteOnSetsNoteOffVelocityB.getRight() +gXSpacing, noteOnSetsNoteOffVelocityB.getY(),
                             250, 30);
    
   
    
    
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
    invertSustainB.setToggleState(gen->getInvertSustain(), dontSendNotification);
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
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &invertSustainB)
    {
        //bool inversion =  (bool) b->getToggleStateValue().toString().getIntValue();
        bool inversion =  b->getToggleState();
        DBG("invert sustain button = " + String((int)inversion));
        processor.setSustainInversion(inversion);
        gen->setInvertSustain(inversion);
    }
    else if (b == &noteOnSetsNoteOffVelocityB)
    {
        bool newstate =  b->getToggleState();
        DBG("invert noteOnSetsNoteOffVelocity = " + String((int)newstate));
        gen->setNoteOnSetsNoteOffVelocity(newstate);
    }
}
