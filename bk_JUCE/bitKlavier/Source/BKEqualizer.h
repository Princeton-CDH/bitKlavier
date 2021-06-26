/*
  ==============================================================================

    BKEqualizer.h
    Created: 26 Jun 2021 4:47:49pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class BKEqualizer  : public juce::Component
{
public:
    BKEqualizer();
    ~BKEqualizer() override;
    
    inline void setPeakFreq(float peakFreq) { this->peakFreq = peakFreq; }
    inline void setPeakGain(float peakGain) { this->peakGain = peakGain; }
    inline void setPeakQuality(float peakQuality) {this->peakQuality = peakQuality; }

private:
    float peakFreq;
    float peakGain;
    float peakQuality;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEqualizer)
};
