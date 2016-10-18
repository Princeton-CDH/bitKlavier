/*
  ==============================================================================

    ReferenceCountedBuffer.cpp
    Created: 13 Oct 2016 9:47:29am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ReferenceCountedBuffer.h"

//==============================================================================
ReferenceCountedBuffer::ReferenceCountedBuffer (const String& nameToUse,
                                                int numChannels,
                                                int numSamples) :
position (0),
name (nameToUse),
buffer (numChannels, numSamples)
{

    DBG (String ("Buffer named '") + name + "' constructed. numChannels = " + String (numChannels) + ", numSamples = " + String (numSamples));
    
}

ReferenceCountedBuffer::~ReferenceCountedBuffer()
{
    DBG (String ("Buffer named '") + name + "' destroyed");
}

AudioSampleBuffer* ReferenceCountedBuffer::getAudioSampleBuffer()
{
    return &buffer;
}
