/*
  ==============================================================================

    ReferenceCountedBuffer.h
    Created: 13 Oct 2016 9:47:29am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef REFERENCECOUNTEDBUFFER_H_INCLUDED
#define REFERENCECOUNTEDBUFFER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
 Adapted from advanced looping tutorial.
 */
class ReferenceCountedBuffer    : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;
    
    ReferenceCountedBuffer (const String& nameToUse,
                            int numChannels,
                            int numSamples);
    ~ReferenceCountedBuffer();
    
    AudioSampleBuffer* getAudioSampleBuffer();

    int position;
    String name;
    
private:
    
    AudioSampleBuffer buffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReferenceCountedBuffer)
};


#endif  // REFERENCECOUNTEDBUFFER_H_INCLUDED
