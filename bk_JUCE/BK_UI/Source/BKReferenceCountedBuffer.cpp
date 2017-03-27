/*
  ==============================================================================

    ReferenceCountedBuffer.cpp
    Created: 13 Oct 2016 9:47:29am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BKReferenceCountedBuffer.h"

//==============================================================================
BKReferenceCountedBuffer::BKReferenceCountedBuffer (const String& nameToUse,
                                                int numChannels,
                                                int numSamples) :
position (0),
name (nameToUse),
buffer (numChannels, numSamples)
{

    //DBG (String ("Buffer named '") + name + "' constructed. numChannels = " + String (numChannels) + ", numSamples = " + String (numSamples));
    
}

BKReferenceCountedBuffer::~BKReferenceCountedBuffer()
{
    //DBG (String ("Buffer named '") + name + "' destroyed");
}

AudioSampleBuffer* BKReferenceCountedBuffer::getAudioSampleBuffer()
{
    return &buffer;
}
