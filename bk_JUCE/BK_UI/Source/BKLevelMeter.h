/*
  ==============================================================================

    BKLevelMeter.h
    Created: 1 Apr 2017 5:16:13pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef BKLEVELMETER_H_INCLUDED
#define BKLEVELMETER_H_INCLUDED

#include "BKUtilities.h"

class JUCE_API  BKLevelMeterComponent  : public Component,
                                        public ChangeBroadcaster,
                                        private Timer
{
public:
    
    BKLevelMeterComponent ();
    
    /** Destructor. */
    ~BKLevelMeterComponent();
    
    void drawLevelMeter (Graphics& g, int width, int height, float level);
    //float level;
    
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    //struct LevelMeter
    //{
        //LevelMeter() noexcept;
    void updateLevel (const float* const*, int numChannels, int numSamples) noexcept;
    void updateLevel (double newlevel);
    void setEnabled (bool) noexcept;
    double getCurrentLevel() const noexcept;
    
    Atomic<int> enabled;
    double level;
    //};
    
    //LevelMeter meter;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKLevelMeterComponent)
};




#endif  // BKLEVELMETER_H_INCLUDED
