/*
  ==============================================================================

    ShareBot.h
    Created: 15 Nov 2017 2:47:58pm
    Author:  airship

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"
class ShareBot : public juce::Component
{
public:
    ShareBot(void);
    
    ~ShareBot(void);
    
    void share(String galleryPath, int where);
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShareBot)
    
};


