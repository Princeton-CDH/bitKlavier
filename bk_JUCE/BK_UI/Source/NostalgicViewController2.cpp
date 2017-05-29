/*
  ==============================================================================

    NostalgicViewController2.cpp
    Created: 24 May 2017 7:13:23pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "NostalgicViewController2.h"

NostalgicViewController2::NostalgicViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{
    nDisplaySlider.addMyListener(this);
    addAndMakeVisible(nDisplaySlider);
    
    startTimer(20);
}

void NostalgicViewController2::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void NostalgicViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    Rectangle<int> displayRow = area.removeFromTop(area.getHeight() * 0.5);
    
    nDisplaySlider.setBounds(displayRow);
}

void NostalgicViewController2::BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow)
{
    DBG("got new nostalgic slider vals " + String(wavedist) + " " + String(undertow));
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    prep    ->setWaveDistance(wavedist);
    active  ->setWaveDistance(wavedist);
    prep    ->setUndertow(undertow);
    active  ->setUndertow(undertow);
    
}

void NostalgicViewController2::updateFields(void)
{
    
    DBG("nostalgic vc2: updating fields");
    
    NostalgicPreparation::Ptr prep = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    nDisplaySlider.setWaveDistance(prep->getWavedistance(), dontSendNotification);
    nDisplaySlider.setUndertow(prep->getUndertow(), dontSendNotification);

}

void NostalgicViewController2::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        
        updateFields();
    }
}

void NostalgicViewController2::timerCallback()
{
    NostalgicProcessor::Ptr nProcessor = processor.gallery->getNostalgicProcessor(processor.updateState->currentNostalgicId);
    
    Array<int> currentPlayPositions = nProcessor->getPlayPositions();
    Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
    currentPlayPositions.addArray(currentUndertowPositions);
    
    nDisplaySlider.updateSliderPositions(currentPlayPositions);
    
}
