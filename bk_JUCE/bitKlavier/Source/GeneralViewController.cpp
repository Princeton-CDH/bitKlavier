/*
  ==============================================================================

    GeneralViewController.cpp
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "GeneralViewController.h"

CommentViewController::CommentViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 1)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    removeChildComponent(&hideOrShow);
    
    addAndMakeVisible(comment);
    comment.setName("comment");
    comment.setMultiLine(true);
    comment.setWantsKeyboardFocus(true);
    
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
    comment.grabKeyboardFocus();
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
        if (processor.updateState->comment != comment.getText())
            processor.updateState->editsMade = true;
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
BKViewController(p, theGraph, 1)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    addAndMakeVisible(about);
    about.setEnabled(false);
    about.setJustification(Justification::centredTop);
    about.setMultiLine(true);
    String astring = "Welcome to bitKlavier v";
    astring.append(JucePlugin_VersionString, 8);
    astring += "! www.bitKlavier.com\n--\nbitKlavier was created by\nDan Trueman\n Mike Mulshine\nMatt Wang";
    about.setText(astring);

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
BKViewController(p, theGraph, 1)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::general_icon_png, BinaryData::general_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    A4tuningReferenceFrequencySlider = std::make_unique<BKSingleSlider>("A4 reference frequency", "A4 reference frequency", 415., 450., 440., 0.1);
    A4tuningReferenceFrequencySlider->setJustifyRight(false);
    A4tuningReferenceFrequencySlider->addMyListener(this);
    addAndMakeVisible(*A4tuningReferenceFrequencySlider);
    
    tempoMultiplierSlider = std::make_unique<BKSingleSlider>("tempo multiplier", "tempo multiplier", 0.25, 4., 1., 0.01);
    tempoMultiplierSlider->setSkewFactorFromMidPoint(1.);
    tempoMultiplierSlider->setJustifyRight(false);
    tempoMultiplierSlider->addMyListener(this);
    addAndMakeVisible(*tempoMultiplierSlider);
    
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

void GeneralViewController::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
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



//==============================================================================
ModdableViewController::ModdableViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 1)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    timeSlider = std::make_unique<BKSingleSlider>("mod time (ms)", "mod time (ms)", 0, 2000, 0, 1);
    timeSlider->setToolTipString("how long it will take to ramp to this mod value");
    timeSlider->setJustifyRight(false);
    timeSlider->addMyListener(this);
    addAndMakeVisible(*timeSlider);
    
    incSlider = std::make_unique<BKSingleSlider>("mod increment", "mod increment", -2.0f, 2.0f, 0.0, 0.001);
    incSlider->setToolTipString("how much to increment the mod value on each time the mod is triggered");
    incSlider->setJustifyRight(false);
    incSlider->addMyListener(this);
    addAndMakeVisible(*incSlider);
    
    maxIncSlider = std::make_unique<BKSingleSlider>("max times to increment", "max times to increment", 0, 10, 0, 1);
    maxIncSlider->setToolTipString("how many times maximum to increment the mod value");
    maxIncSlider->setJustifyRight(false);
    maxIncSlider->addMyListener(this);
    addAndMakeVisible(*maxIncSlider);

#if JUCE_IOS
    timeSlider->addWantsBigOneListener(this);
    incSlider->addWantsBigOneListener(this);
    maxIncSlider->addWantsBigOneListener(this);
#endif
    
    startTimerHz(10);
}

#if JUCE_IOS
void ModdableViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

ModdableViewController::~ModdableViewController()
{
    setLookAndFeel(nullptr);
}

void ModdableViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black.withAlpha(0.6f));
    
    Rectangle<int> area (getLocalBounds());
    area.reduce(area.getWidth() * 0.2f, area.getHeight() * 0.2f);
    
    g.setColour(Colours::black);
    g.fillRect(area);
    
    g.setColour(Colours::antiquewhite.withAlpha(0.6f));
    g.drawRect(area, 1);
}

void ModdableViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    area.reduce(area.getWidth() * 0.2f, area.getHeight() * 0.2f);
    
    Rectangle<int> hideOrShowBounds = area.reduced(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4).removeFromLeft(area.getWidth() * 0.5);
    hideOrShowBounds = hideOrShowBounds.removeFromTop(gComponentComboBoxHeight);
    hideOrShowBounds.removeFromRight(4 + 2.0f * gPaddingConst * processor .paddingScalarX);
    hideOrShowBounds.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(hideOrShowBounds.removeFromLeft(gComponentComboBoxHeight));
    
    area.reduce(area.getWidth() * 0.2f, area.getHeight() * 0.2f);
    
    int h = area.getHeight();
    
    timeSlider->setBounds(area.removeFromTop(h * 0.3f));
    incSlider->setBounds(area.removeFromTop(h * 0.3f));
    maxIncSlider->setBounds(area);
}


void ModdableViewController::bkTextFieldDidChange(TextEditor& tf)
{
    
}

void ModdableViewController::update(void)
{
    ModdableBase* mod = getCurrentModdable();
    if (mod == nullptr) return;
    
    timeSlider->setValue(mod->getTime(), dontSendNotification);
    incSlider->setValue(mod->getInc(), dontSendNotification);
    maxIncSlider->setValue(mod->getMaxNumberOfInc(), dontSendNotification);
    if (maxIncSlider->getValue() == 0.0) maxIncSlider->setText("no max");
}

void ModdableViewController::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    ModdableBase* mod = getCurrentModdable();
    if (mod == nullptr) return;
    
    if (name == timeSlider->getName())
    {
        mod->setTime(int(val));
    }
    if (name == incSlider->getName())
    {
        mod->setInc(val);
    }
    if (name == maxIncSlider->getName())
    {
        mod->setMaxNumberOfInc(val);
        if (maxIncSlider->getValue() == 0.0) maxIncSlider->setText("no max");
    }
}

void ModdableViewController::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(processor.updateState->previousDisplay);
    }
}

ModdableBase* ModdableViewController::getCurrentModdable()
{
    // Might be a good idea to standardize the naming/organization of preparations parameters
    // and sliders, etc so this could be streamlined. Or at least use constants for component names
    if (processor.updateState->previousDisplay == DisplayDirectMod)
    {
        DirectModification::Ptr mod = processor.gallery->getDirectModification(processor.updateState->currentModDirectId);
        
        if (processor.updateState->currentModdableIdentifier == cDirectGain)
            return &mod->dGain;
        else if (processor.updateState->currentModdableIdentifier == cDirectResonanceGain)
            return &mod->dResonanceGain;
        else if (processor.updateState->currentModdableIdentifier == cDirectHammerGain)
            return &mod->dHammerGain;
        else if (processor.updateState->currentModdableIdentifier == cDirectBlendronicGain)
            return &mod->dBlendronicGain;
    }
    else if (processor.updateState->previousDisplay == DisplayNostalgicMod)
    {
        NostalgicModification::Ptr mod = processor.gallery->getNostalgicModification(processor.updateState->currentModNostalgicId);
        
        if (processor.updateState->currentModdableIdentifier == cNostalgicGain)
            return &mod->nGain;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicBlendronicGain)
            return &mod->nBlendronicGain;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicLengthMultiplier)
            return &mod->nLengthMultiplier;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicBeatsToSkip)
            return &mod->nBeatsToSkip;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicClusterMin)
            return &mod->clusterMin;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicClusterThreshold)
            return &mod->clusterThreshold;
    }
    else if (processor.updateState->previousDisplay == DisplaySynchronicMod)
    {
        SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
        
        if (processor.updateState->currentModdableIdentifier == cSynchronicGain)
            return &mod->sGain;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicBlendronicGain)
            return &mod->sBlendronicGain;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicNumBeats)
            return &mod->sNumBeats;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicClusterThresh)
            return &mod->sClusterThresh;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicClusterCap)
            return &mod->sClusterCap;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicNumClusters)
            return &mod->numClusters;
    }
    else if (processor.updateState->previousDisplay == DisplayBlendronicMod)
    {
        BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
        
        if (processor.updateState->currentModdableIdentifier == cBlendronicOutGain)
            return &mod->outGain;
    }
    else if (processor.updateState->previousDisplay == DisplayTuningMod)
    {
        TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
        
        if (processor.updateState->currentModdableIdentifier == cSpringTuningRate)
            return &mod->getSpringTuning()->rate;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningDrag)
            return &mod->getSpringTuning()->drag;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningStiffness)
            return &mod->getSpringTuning()->stiffness;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningIntervalStiffness)
            return &mod->getSpringTuning()->intervalStiffness;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningTetherWeight)
            return &mod->getSpringTuning()->tetherWeightGlobal;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningTetherWeightSecondary)
            return &mod->getSpringTuning()->tetherWeightSecondaryGlobal;
        else if (processor.updateState->currentModdableIdentifier == cTuningAdaptiveClusterThresh)
            return &mod->tAdaptiveClusterThresh;
        else if (processor.updateState->currentModdableIdentifier == cTuningAdaptiveHistory)
            return &mod->tAdaptiveHistory;
        else if (processor.updateState->currentModdableIdentifier == cTuningToneSemitoneWidth)
            return &mod->nToneSemitoneWidth;
        else if (processor.updateState->currentModdableIdentifier == cTuningFundamentalOffset)
            return &mod->tFundamentalOffset;
    }
    else if (processor.updateState->previousDisplay == DisplayTempoMod)
    {
        TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);

        if (processor.updateState->currentModdableIdentifier == cTempoTempo)
            return &mod->sTempo;
        else if (processor.updateState->currentModdableIdentifier == cTempoSubdivisions)
            return &mod->subdivisions;
        else if (processor.updateState->currentModdableIdentifier == cTempoAT1History)
            return &mod->at1History;
        else if (processor.updateState->currentModdableIdentifier == cTempoAT1Subdivisions)
            return &mod->at1Subdivisions;
    }
    
    return nullptr;
}


void ModdableViewController::timerCallback()
{
//    ModdableBase* mod = getCurrentModdable();
}
