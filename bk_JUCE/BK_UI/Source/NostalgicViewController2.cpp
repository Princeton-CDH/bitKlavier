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
    
    startTimer(20);
}

void NostalgicViewController2::timerCallback()
{
    NostalgicProcessor::Ptr nProcessor = processor.gallery->getNostalgicProcessor(processor.updateState->currentNostalgicId);
    
    Array<int> currentPlayPositions = nProcessor->getPlayPositions();
    
    for(int i=0; i<currentPlayPositions.size(); i++)
    {
        DBG("nostalgic playback <= position " + String(i) + " " + String(currentPlayPositions.getUnchecked(i)));
    }
    
    Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
    
    for(int i=0; i<currentUndertowPositions.size(); i++)
    {
        DBG("nostalgic undertow => position " + String(i) + " " + String(currentUndertowPositions.getUnchecked(i)));
    }
    
}
