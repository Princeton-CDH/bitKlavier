/*
  ==============================================================================

    BKLevelMeter.cpp
    Created: 1 Apr 2017 5:16:13pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "BKLevelMeter.h"

BKLevelMeterComponent::BKLevelMeterComponent ()
{
    //startTimer (50);
    level = 0.;
    enabled.set(true);
}

BKLevelMeterComponent::~BKLevelMeterComponent()
{
}

void BKLevelMeterComponent::paint (Graphics& g)
{
    drawLevelMeter (g, getWidth(), getHeight(),
                    (float) exp (log (level) / 3.0));
}

void BKLevelMeterComponent::resized()
{
    
}

void BKLevelMeterComponent::timerCallback()
{
    //if (isShowing()) repaint();
}

void BKLevelMeterComponent::drawLevelMeter (Graphics& g, int width, int height, float level)
{
    g.setColour (Colours::black.withAlpha (0.7f));
    g.fillRoundedRectangle (0.0f, 0.0f, (float) width, (float) height, 3.0f);
    g.setColour (Colours::black.withAlpha (0.2f));
    g.drawRoundedRectangle (1.0f, 1.0f, width - 2.0f, height - 2.0f, 3.0f, 1.0f);
    
    const int totalBlocks = 14;
    const int numBlocks = roundToInt (totalBlocks * level);
    const float w = (width - 6.0f) / (float) totalBlocks;
    const float h = (height - 6.0f) / (float) totalBlocks;
    
    for (int i = 0; i < totalBlocks; ++i)
    {
        if (i >= numBlocks)
        {
            g.setColour (Colours::green.withAlpha (0.2f));
            if(i >= totalBlocks - 2) g.setColour(Colours::red.withAlpha (0.2f));
            else if(i >= totalBlocks - 4) g.setColour(Colours::yellow.withAlpha (0.2f));
        }
        else if(i < totalBlocks - 4) g.setColour(Colours::green.withAlpha (0.9f));
        else if(i < totalBlocks - 2) g.setColour(Colours::yellow.withAlpha (0.9f));
        else g.setColour(Colours::red.withAlpha (1.0f));

        if(height <= width)
            g.fillRect (3.0f + i * w + w * 0.1f, 3.0f, w * 0.8f, height - 6.0f);
        else
            g.fillRect (3.0f, height - ((i+1) * h + h * 0.1f), width - 6.0f, h * 0.8f);
    }
}

void BKLevelMeterComponent::updateLevel (double newlevel)
{
    const double decayFactor = 0.8;
    
    if (enabled.get())
    {
    
        if (newlevel > level)
            level = newlevel;
        else if (level > 0.001f)
            level *= decayFactor;
        else
            level = 0;
        
        if (isShowing()) repaint();

    }
}

void BKLevelMeterComponent::setEnabled (bool shouldBeEnabled) noexcept
{
    enabled.set (shouldBeEnabled ? 1 : 0);
    level = 0;
}

double BKLevelMeterComponent::getCurrentLevel() const noexcept
{
    jassert (enabled.get() != 0); // you need to call setEnabled (true) before using this!
    return level;
}
