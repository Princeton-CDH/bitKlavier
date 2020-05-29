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
    
    struct ChannelInfo
    {
        ChannelInfo (int bufferSize, int numBlocks) :
        bufferSize(bufferSize),
        numBlocks(numBlocks),
        samplesPerBlock(bufferSize/numBlocks)
        {
            setNumBlocks (numBlocks);
            clear();
        }
        
        void clear() noexcept
        {
            levels.fill ({});
            value = {};
            subSample = 0;
        }
        
        void pushSamples (const float* inputSamples, int num) noexcept
        {
            for (int i = 0; i < num; ++i)
                pushSample (inputSamples[i]);
        }
        
        void pushSample (float newSample) noexcept
        {
            if (--subSample <= 0)
            {
                if (++nextSample == levels.size())
                    nextSample = 0;
                
                levels.getReference (nextSample) = value;
                subSample = samplesPerBlock;
                value = Range<float> (newSample, newSample);
            }
            else
            {
                value = value.getUnionWith (newSample);
            }
        }
        
        void setNumBlocks (int newSize)
        {
            levels.removeRange (newSize, levels.size());
            levels.insertMultiple (-1, {}, newSize - levels.size());
            
            if (nextSample >= newSize)
                nextSample = 0;
        }
        
        void setBufferSize (int size)
        {
            bufferSize = size;
            samplesPerBlock = bufferSize / numBlocks;
        }
        
        int getBufferSize (void) { return bufferSize; }
    
        int getSamplesPerBlock (void) { return samplesPerBlock; }
        
        Array<Range<float>> levels;
        Range<float> value;
        std::atomic<int> nextSample { 0 }, subSample { 0 };
        int bufferSize;
        int numBlocks;
        int samplesPerBlock;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelInfo)
    };
    
    void setAudio(OwnedArray<ChannelInfo>* audio) { channels = audio; }
    void setSmoothing(ChannelInfo* smooth) { smoothing = smooth; }
    
    inline void setMaxDelayLength (float maxDelay) { maxDelayLength = maxDelay; }

    inline void setLineSpacing(uint64 spacing)
    {
        if (spacing == 0) return;
        lineSpacingInBlocks = spacing * (1.0f / smoothing->getSamplesPerBlock());
    }
    inline void setPulseOffset(float off) { pulseOffset = off * (1.0f / smoothing->getSamplesPerBlock()); }
    inline void setVerticalZoom(float zoom) { verticalZoom = zoom; }
    inline void setHorizontalZoom(float zoom) { horizontalZoom = zoom; }
    inline void setMarkers(Array<uint64> m) { markers = m; }
    inline void clearMarkers() { markers.clear(); }
    inline void setPlayheads(Array<uint64> p) { playheads = p; }
    inline void resetPhase() { offset = 0.0f; prevLevel = 0.0f; }
    
    void setColours (Colour backgroundColour, Colour waveformColour) noexcept;
    void setRepaintRate (int frequencyInHz);
    virtual void paintChannel (Graphics&, Rectangle<float> bounds,
                               const Range<float>* levels, int numLevels, int nextSample);
    void getChannelAsPath (Path& result, const Range<float>* levels, int numLevels, int nextSample);
    virtual void paintSmoothing (Graphics&, Rectangle<float> bounds,
                               const Range<float>* levels, int numLevels, int nextSample);
    void getSmoothingAsPath (Path& result, const Range<float>* levels, int numLevels, int nextSample);
    
    void paint (Graphics&) override;
    
    void sliderValueChanged (Slider *slider) override;
    
private:
    
    OwnedArray<ChannelInfo>* channels;
    ChannelInfo* smoothing;
    
    float lineSpacingInBlocks;
    float currentLevel, prevLevel;
    float scroll, offset;
    float pulseOffset;
    float verticalZoom, horizontalZoom;
    
    std::unique_ptr<Slider> verticalZoomSlider;
    std::unique_ptr<Slider> horizontalZoomSlider;
    
    double verticalZoomSliderMin, verticalZoomSliderMax;
    double horizontalZoomSliderMin, horizontalZoomSliderMax;
    double sliderIncrement;
    
    float maxDelayLength;
    
    BlendronicSmoothBase smoothBase;
    BlendronicSmoothScale smoothScale;
    
    Array<uint64> markers, playheads;
    uint64 prevPlayhead;
    Colour backgroundColour, waveformColour, markerColour, playheadColour;
    
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlendronicDisplay)
};

#endif
