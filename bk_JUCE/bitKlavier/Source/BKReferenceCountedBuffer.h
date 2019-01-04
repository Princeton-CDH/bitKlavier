/*
  ==============================================================================

    BKReferenceCountedBuffer.h
    Created: 13 Oct 2016 9:47:29am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKREFERENCECOUNTEDBUFFER_H_INCLUDED
#define BKREFERENCECOUNTEDBUFFER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
 Adapted from advanced looping tutorial.
 */
class BKReferenceCountedBuffer    : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<BKReferenceCountedBuffer> Ptr;
    
    BKReferenceCountedBuffer (const String& nameToUse,
                            int numChannels,
                            int numSamples);
    ~BKReferenceCountedBuffer();
    
    AudioSampleBuffer* getAudioSampleBuffer();
    void setAudioSampleBuffer(AudioSampleBuffer* from);

    int position;
    String name;
    
private:
    
    AudioSampleBuffer buffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKReferenceCountedBuffer)
};


#endif  // BKREFERENCECOUNTEDBUFFER_H_INCLUDED
