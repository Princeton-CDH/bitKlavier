/*
==============================================================================

BKBufferView.cpp
Created: 17 Oct 2019 12:21:14pm
Author:  Matthew Wang

==============================================================================
*/

#include "BKBufferView.h"

//==============================================================================

struct BKBufferView::ChannelInfo
{
    ChannelInfo (BKBufferView& o, int bufferSize) : owner (o)
    {
        setBufferSize (bufferSize);
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
            subSample = owner.getSamplesPerBlock();
            value = Range<float> (newSample, newSample);
        }
        else
        {
            value = value.getUnionWith (newSample);
        }
        if (newSample != 0.0f) owner.setCurrentBlock(nextSample-1 < 0 ? levels.size()-1 : nextSample-1);
        owner.removeMarker(nextSample);
    }
    
    void setBufferSize (int newSize)
    {
        levels.removeRange (newSize, levels.size());
        levels.insertMultiple (-1, {}, newSize - levels.size());
        
        if (nextSample >= newSize)
            nextSample = 0;
    }
    
    BKBufferView& owner;
    Array<Range<float>> levels;
    Range<float> value;
    std::atomic<int> nextSample { 0 }, subSample { 0 };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelInfo)
};

//==============================================================================
BKBufferView::BKBufferView ()
: bufferSize(0),
numBlocks (1024),
inputSamplesPerBlock (256),
invInputSamplesPerBlock (1./256.),
lineSpacingInBlocks(256),
verticalZoom(0.5),
backgroundColour (Colours::black),
waveformColour (Colours::white),
markerColour (Colours::burlywood)
{
    setOpaque (true);
    setNumChannels (1);
    setRepaintRate (60);
}

BKBufferView::BKBufferView (int initialNumChannels)
: bufferSize(0),
numBlocks (1024),
inputSamplesPerBlock (256),
invInputSamplesPerBlock (1./256.),
lineSpacingInBlocks(256),
verticalZoom(0.5),
backgroundColour (Colours::black),
waveformColour (Colours::white),
markerColour (Colours::burlywood)
{
    setOpaque (true);
    setNumChannels (initialNumChannels);
    setRepaintRate (60);
}

BKBufferView::~BKBufferView()
{
}

void BKBufferView::setNumChannels (int numChannels)
{
    channels.clear();
    
    for (int i = 0; i < numChannels; ++i)
        channels.add (new ChannelInfo (*this, numBlocks));
}

void BKBufferView::setNumBlocks (int num)
{
    numBlocks = num;
    
    for (auto* c : channels)
        c->setBufferSize (num);
}

void BKBufferView::clear()
{
    for (auto* c : channels)
        c->clear();
}

void BKBufferView::pushBuffer (const float** d, int numChannels, int num)
{
    numChannels = jmin (numChannels, channels.size());
    setNumBlocks(num*invInputSamplesPerBlock);
    
    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked(i)->pushSamples (d[i], num);
}

void BKBufferView::pushBuffer (const AudioBuffer<float>& buffer)
{
    pushBuffer (buffer.getArrayOfReadPointers(),
                buffer.getNumChannels(),
                buffer.getNumSamples());
}

void BKBufferView::pushBuffer (const AudioSourceChannelInfo& buffer)
{
    auto numChannels = jmin (buffer.buffer->getNumChannels(), channels.size());
    
    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked(i)->pushSamples (buffer.buffer->getReadPointer (i, buffer.startSample),
                                               buffer.numSamples);
}

void BKBufferView::pushSample (const float* d, int numChannels)
{
    numChannels = jmin (numChannels, channels.size());
    
    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked(i)->pushSample (d[i]);
}

void BKBufferView::setSamplesPerBlock (int newSamplesPerPixel) noexcept
{
    inputSamplesPerBlock = newSamplesPerPixel;
    invInputSamplesPerBlock = 1. / (float) inputSamplesPerBlock;
}

void BKBufferView::setRepaintRate (int frequencyInHz)
{
    startTimerHz (frequencyInHz);
}

void BKBufferView::timerCallback()
{
    repaint();
}

void BKBufferView::setColours (Colour bk, Colour fg) noexcept
{
    backgroundColour = bk;
    waveformColour = fg;
    repaint();
}

void BKBufferView::paint (Graphics& g)
{
    g.fillAll (backgroundColour);
    
    auto r = getLocalBounds().toFloat();
    auto channelHeight = r.getHeight() / channels.size();
    
    g.setColour (waveformColour);
    
    for (auto* c : channels)
        paintChannel (g, r.removeFromTop (channelHeight),
                      c->levels.begin(), c->levels.size(), c->nextSample);
}

void BKBufferView::getChannelAsPath (Path& path, const Range<float>* levels,
                                                 int numLevels, int nextSample)
{
    path.preallocateSpace (4 * numLevels + 8);
    
    for (int i = 0; i < numLevels; ++i)
    {
        auto level = -(levels[(nextSample + i) % numLevels].getEnd());
        
        if (i == 0)
            path.startNewSubPath (0.0f, level);
        else
            path.lineTo ((float) i, level);
    }
    
    for (int i = numLevels; --i >= 0;)
        path.lineTo ((float) i, -(levels[(nextSample + i) % numLevels].getStart()));
    
    path.closeSubPath();
}

void BKBufferView::paintChannel (Graphics& g, Rectangle<float> area, const Range<float>* levels, int numLevels, int nextSample)
{
    int i = 0;
    g.setColour (waveformColour.withMultipliedBrightness(0.3f));
    g.fillRect(area.getX(), area.getCentreY(), area.getWidth(), 1.0f);
    for (float f = 0; f < numLevels; f += lineSpacingInBlocks * 0.25)
    {
        float x = f * (area.getRight() - area.getX()) * (1. / numLevels) + area.getX();
        
        if (i % 4 == 0)
        {
            g.setColour (waveformColour.withMultipliedBrightness(0.4f));
            g.fillRect(x, area.getY(), 1.0f, area.getHeight());
        }
        else
        {
            g.setColour (waveformColour.withMultipliedBrightness(0.2f));
            float inbetweenHeight = area.getHeight() * 0.125;
            g.fillRect(x, area.getCentreY()-inbetweenHeight, 1.0f, inbetweenHeight*2.0f);
        }
        i++;
    }
    
    Path p;
    getChannelAsPath (p, levels, numLevels, nextSample);
    
//    for (int i = 0; i < markers.size(); ++i)
//    {
//        if (markers[i] != 0)
//        {
//            float x = i * (area.getRight() - area.getX()) * (1. / numLevels) + area.getX();
//            g.setColour (markerColour);
//            g.fillRect(x, area.getY(), 1.0f, area.getHeight());
//        }
//    }
    
    g.setColour (waveformColour);
    g.fillPath (p, AffineTransform::fromTargetPoints (0.0f, -verticalZoom,        area.getX(), area.getY(),
                                                      0.0f, verticalZoom,         area.getX(), area.getBottom(),
                                                      (float) numLevels, -verticalZoom, area.getRight(), area.getY()));
}
    

