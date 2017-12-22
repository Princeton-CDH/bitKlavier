/*
  ==============================================================================

    BKGalleryLoader.h
    Created: 21 Dec 2017 10:37:07pm
    Author:  airship

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"

class BKAudioProcessor;

#ifdef LOADTHREAD

class BKXMLGalleryLoader : public Thread
{
public:
    
    BKXMLGalleryLoader(BKAudioProcessor& p):
    processor(p),
    Thread("xml_gallery_loader")
    {
        
    }
    
    ~BKXMLGalleryLoader()
    {
        
    }
    
private:
    
    void run(void) override;
    
    BKAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKXMLGalleryLoader)
    
}


class BKJSONGalleryLoader : public Thread
{
public:
    
    BKJSONGalleryLoader(BKAudioProcessor& p):
    processor(p),
    Thread("json_gallery_loader")
    {
        
    }
    
    ~BKJSONGalleryLoader()
    {
        
    }
    
private:
    
    void run(void) override;
    
    BKAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKXMLGalleryLoader)
    
}
#endif
