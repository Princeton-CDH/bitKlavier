/*
==============================================================================

BlendronicDisplay.h
Created: 17 Oct 2019 12:21:14pm
Author:  Matthew Wang

==============================================================================
*/

#ifndef BLENDRONICDISPLAY_H_INCLUDED
#define BLENDRONICDISPLAY_H_INCLUDED

#include "BKUtilities.h"
#include "BKComponent.h"
#include "BKLookAndFeel.h"
#include "BKUIComponents.h"

class BlendronicDisplay :
public Component,
public Slider::Listener,
private Timer
{
public:
    BlendronicDisplay ();
    BlendronicDisplay (int initialNumChannels);

    ~BlendronicDisplay() override;
    
    void setNumChannels (int numChannels);
    void setNumBlocks (int num);
    void setSamplesPerBlock (int newNumInputSamplesPerBlock) noexcept;
    int getSamplesPerBlock() const noexcept { return inputSamplesPerBlock; }
    void clear();
    void pushBuffer (const AudioBuffer<float>& bufferToPush);
    void pushBuffer (const AudioSourceChannelInfo& bufferToPush);
    void pushBuffer (const float** channelData, int numChannels, int numSamples);
    void pushSample (const float* samplesForEachChannel, int numChannels);
    
    

    inline void setLineSpacing(float spacingInSamples)
    {
        if (spacingInSamples == 0) return;
        lineSpacingInBlocks = spacingInSamples * invInputSamplesPerBlock;
    }
    inline void setPulseOffset(float offset) { pulseOffset = offset * invInputSamplesPerBlock; }
    inline void setVerticalZoom(float zoom) { verticalZoom = zoom; }
    inline void setHorizontalZoom(float zoom) { horizontalZoom = zoom; }
    inline void setMarkers(Array<uint64> m) { markers = m; }
    inline void clearMarkers() { markers.clear(); }
    inline void setPlayheads(Array<uint64> p) { playheads = p; }
    
    void setColours (Colour backgroundColour, Colour waveformColour) noexcept;
    void setRepaintRate (int frequencyInHz);
    virtual void paintChannel (Graphics&, Rectangle<float> bounds,
                               const Range<float>* levels, int numLevels, int nextSample);
    void getChannelAsPath (Path& result, const Range<float>* levels, int numLevels, int nextSample);
    
    void paint (Graphics&) override;
    
    void sliderValueChanged (Slider *slider) override;
    
private:
    struct ChannelInfo;
    
    OwnedArray<ChannelInfo> channels;
    int bufferSize, numBlocks, inputSamplesPerBlock;
    float invInputSamplesPerBlock;
    float lineSpacingInBlocks;
    float pulseOffset;
    float verticalZoom, horizontalZoom;
    
    std::unique_ptr<Slider> verticalZoomSlider;
    std::unique_ptr<Slider> horizontalZoomSlider;
    
    double verticalZoomSliderMin, verticalZoomSliderMax;
    double horizontalZoomSliderMin, horizontalZoomSliderMax;
    double sliderIncrement;
    
    Array<uint64> markers, playheads;
    Colour backgroundColour, waveformColour, markerColour, playheadColour;
    
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlendronicDisplay)
};

#endif
