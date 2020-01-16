/*
==============================================================================

BlendronicDisplay.cpp
Created: 17 Oct 2019 12:21:14pm
Author:  Matthew Wang

==============================================================================
*/

#include "BlendronicDisplay.h"

//==============================================================================

struct BlendronicDisplay::ChannelInfo
{
    ChannelInfo (BlendronicDisplay& o, int bufferSize) : owner (o)
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
    }
    
    void setBufferSize (int newSize)
    {
        levels.removeRange (newSize, levels.size());
        levels.insertMultiple (-1, {}, newSize - levels.size());
        
        if (nextSample >= newSize)
            nextSample = 0;
    }
    
    BlendronicDisplay& owner;
    Array<Range<float>> levels;
    Range<float> value;
    std::atomic<int> nextSample { 0 }, subSample { 0 };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelInfo)
};

//==============================================================================
BlendronicDisplay::BlendronicDisplay ()
: bufferSize(0),
numBlocks (1024),
inputSamplesPerBlock (256),
invInputSamplesPerBlock (1./256.),
lineSpacingInBlocks(256),
pulseOffset(0.0),
verticalZoom(1.0),
horizontalZoom(0.0),
verticalZoomSliderMin(0.01),
verticalZoomSliderMax(2.0),
horizontalZoomSliderMin(0.0),
horizontalZoomSliderMax(0.5),
sliderIncrement(0.0001),
backgroundColour (Colours::black),
waveformColour (Colours::white),
markerColour (Colours::goldenrod.withMultipliedBrightness(0.7)),
playheadColour (Colours::mediumpurple)
{
    setOpaque (true);
    setNumChannels (1);
    setRepaintRate (60);
    
    verticalZoomSlider = std::make_unique<Slider>();
    verticalZoomSlider->addMouseListener(this, true);
    verticalZoomSlider->setRange(verticalZoomSliderMin, verticalZoomSliderMax, sliderIncrement);
    verticalZoomSlider->setSkewFactor(2.0);
    verticalZoomSlider->setValue(verticalZoom);
    verticalZoomSlider->setSliderStyle(Slider::SliderStyle::LinearVertical);
    verticalZoomSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    verticalZoomSlider->setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedBrightness(0.7));
    verticalZoomSlider->setColour(Slider::backgroundColourId, Colours::white.withMultipliedBrightness(0.3));
    verticalZoomSlider->addListener(this);
    addAndMakeVisible(*verticalZoomSlider);
    
    horizontalZoomSlider = std::make_unique<Slider>();
    horizontalZoomSlider->addMouseListener(this, true);
    horizontalZoomSlider->setRange(horizontalZoomSliderMin, horizontalZoomSliderMax, sliderIncrement);
    horizontalZoomSlider->setValue(horizontalZoom);
    horizontalZoomSlider->setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    horizontalZoomSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    horizontalZoomSlider->setColour(Slider::trackColourId, Colours::white.withMultipliedBrightness(0.3));
    horizontalZoomSlider->setColour(Slider::backgroundColourId, Colours::goldenrod.withMultipliedBrightness(0.7));
    horizontalZoomSlider->addListener(this);
    addAndMakeVisible(*horizontalZoomSlider);
}

BlendronicDisplay::BlendronicDisplay (int initialNumChannels)
: bufferSize(0),
numBlocks (1024),
inputSamplesPerBlock (256),
invInputSamplesPerBlock (1./256.),
lineSpacingInBlocks(256),
verticalZoom(0.5),
backgroundColour (Colours::black),
waveformColour (Colours::white),
markerColour (Colours::burlywood),
playheadColour (Colours::mediumpurple)
{
    setOpaque (true);
    setNumChannels (initialNumChannels);
    setRepaintRate (60);
}

BlendronicDisplay::~BlendronicDisplay()
{
}

void BlendronicDisplay::setNumChannels (int numChannels)
{
    channels.clear();
    
    for (int i = 0; i < numChannels; ++i)
        channels.add (new ChannelInfo (*this, numBlocks));
    
    smoothing.clear();
    smoothing.add (new ChannelInfo (*this, numBlocks));
}

void BlendronicDisplay::setNumBlocks (int num)
{
    numBlocks = num;
    
    for (auto* c : channels)
        c->setBufferSize (num);
    for (auto* s : smoothing)
        s->setBufferSize (num);
}

void BlendronicDisplay::clear()
{
    for (auto* c : channels)
        c->clear();
    for (auto* s : smoothing)
        s->clear();
}

void BlendronicDisplay::pushBuffer (const float** d, int numChannels, int num)
{
    numChannels = jmin (numChannels, channels.size());
    setNumBlocks(num*invInputSamplesPerBlock);
    
    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked(i)->pushSamples (d[i], num);
}

void BlendronicDisplay::pushBuffer (const AudioBuffer<float>& buffer)
{
    pushBuffer (buffer.getArrayOfReadPointers(),
                buffer.getNumChannels(),
                buffer.getNumSamples());
}

void BlendronicDisplay::pushBuffer (const AudioSourceChannelInfo& buffer)
{
    auto numChannels = jmin (buffer.buffer->getNumChannels(), channels.size());
    
    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked(i)->pushSamples (buffer.buffer->getReadPointer (i, buffer.startSample),
                                               buffer.numSamples);
}

void BlendronicDisplay::pushSample (const float* d, int numChannels)
{
    numChannels = jmin (numChannels, channels.size());
    
    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked(i)->pushSample (d[i]);
}

void BlendronicDisplay::pushSmoothing (Array<float> sm)
{
    for (auto s : smoothing)
        s->pushSamples (sm.getRawDataPointer(), sm.size());
}

void BlendronicDisplay::setSamplesPerBlock (int newSamplesPerPixel) noexcept
{
    inputSamplesPerBlock = newSamplesPerPixel;
    invInputSamplesPerBlock = 1. / (float) inputSamplesPerBlock;
}


void BlendronicDisplay::setRepaintRate (int frequencyInHz)
{
    startTimerHz (frequencyInHz);
}

void BlendronicDisplay::timerCallback()
{
    repaint();
}

void BlendronicDisplay::setColours (Colour bk, Colour fg) noexcept
{
    backgroundColour = bk;
    waveformColour = fg;
    repaint();
}

void BlendronicDisplay::paint (Graphics& g)
{
    // Get bounds for display components
    auto displayBounds = getLocalBounds();
    auto horizontalZoomSliderBounds = displayBounds.removeFromBottom(getLocalBounds().getHeight()*0.05);
    horizontalZoomSliderBounds.removeFromRight(horizontalZoomSliderBounds.getHeight());
    auto verticalZoomSliderBounds = displayBounds.removeFromRight(horizontalZoomSliderBounds.getHeight()).removeFromTop(getLocalBounds().getHeight()*0.6);
    
    auto bufferBounds = displayBounds.removeFromTop(getLocalBounds().getHeight()*0.6);
    auto smoothingBounds = displayBounds.removeFromTop(getLocalBounds().getHeight()*0.35);
    
    auto channelHeight = bufferBounds.getHeight() / channels.size();
    
    g.fillAll (backgroundColour);
    g.setColour (waveformColour);
    
    auto channelBounds = bufferBounds;
    for (auto* c : channels)
    {
        channelBounds = channelBounds.removeFromTop(channelHeight);
        paintChannel (g, channelBounds.toFloat(),
                      c->levels.begin(), c->levels.size(), c->nextSample);
    }

    g.setColour (backgroundColour);
    g.fillRect(horizontalZoomSliderBounds);
    horizontalZoomSlider->setBounds(horizontalZoomSliderBounds);
    verticalZoomSlider->setBounds(verticalZoomSliderBounds);
    
    g.setColour (waveformColour);
    g.drawHorizontalLine(bufferBounds.getY(), bufferBounds.getX(), bufferBounds.getRight());
    g.drawHorizontalLine(bufferBounds.getBottom(), bufferBounds.getX(), bufferBounds.getRight());
    
    for (auto* s : smoothing)
    {
        paintSmoothing (g, smoothingBounds.toFloat(),
                        s->levels.begin(), s->levels.size(), s->nextSample);
    }
    
}

void BlendronicDisplay::getChannelAsPath (Path& path, const Range<float>* levels,
                                                 int numLevels, int nextSample)
{
    path.preallocateSpace (4 * numLevels + 8);
    
    int offset = playheads[0] * invInputSamplesPerBlock;
    
    for (int i = 4; i < numLevels-2; ++i)
    {
        auto level = -(levels[(nextSample + i + offset) % numLevels].getEnd());
        
        if (i == 4)
            path.startNewSubPath (0.0f, level);
        else
            path.lineTo ((float) i, level);
    }
    
    for (int i = numLevels-2; --i >= 4;)
        path.lineTo ((float) i, -(levels[(nextSample + i + offset) % numLevels].getStart()));
    
    path.closeSubPath();
}

void BlendronicDisplay::paintChannel (Graphics& g, Rectangle<float> area, const Range<float>* levels, int numLevels, int nextSample)
{
    int i = 0;
    g.setColour (waveformColour.withMultipliedBrightness(0.3f));
    g.fillRect(area.getX(), area.getCentreY(), area.getWidth(), 1.0f);

    
    int offset = playheads[0] * invInputSamplesPerBlock;
    
    float leftLevel = numLevels*horizontalZoom;
    
    for (float f = 0; f < numLevels; f += lineSpacingInBlocks * 0.25f)
    {
        float x = (fmod(f + numLevels - offset + pulseOffset, numLevels) - leftLevel) *
                  (area.getRight() - area.getX()) * (1. / (numLevels - leftLevel)) + area.getX();
        
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
    
    g.setColour (waveformColour);
    g.fillPath (p, AffineTransform::fromTargetPoints (numLevels*horizontalZoom, -verticalZoom,  area.getX(), area.getY(),
                                                      numLevels*horizontalZoom, verticalZoom,   area.getX(), area.getBottom(),
                                                      (float) numLevels, -verticalZoom,         area.getRight(), area.getY()));
    
    for (auto m : markers)
    {
        int offset = playheads[0] * invInputSamplesPerBlock;
        float markerLevel = m * invInputSamplesPerBlock;
        float x = (fmod(((markerLevel - offset) + numLevels), numLevels) - leftLevel) *
                  (area.getRight() - area.getX()) * (1. / (numLevels - leftLevel)) + area.getX();
        if (x < 1.0f) continue;
        g.setColour (markerColour);
        g.fillRect(x, area.getY(), 1.0f, area.getHeight());
    }
    
    for (auto p : playheads)
    {
        if (p == playheads[0]) continue;
        int offset = playheads[0] * invInputSamplesPerBlock;
        float playheadLevel = p * invInputSamplesPerBlock;
        float x = (fmod(((playheadLevel - offset) + numLevels), numLevels) - leftLevel) *
                  (area.getRight() - area.getX()) * (1. / (numLevels - leftLevel)) + area.getX();
        g.setColour (playheadColour);
        g.fillRect(x-2.0, area.getY(), 4.0, area.getHeight());
    }
}

void BlendronicDisplay::getSmoothingAsPath (Path& path, const Range<float>* levels,
                                            int numLevels, int nextSample)
{
    path.preallocateSpace (4 * numLevels + 8);
    
    int offset = playheads[0] * invInputSamplesPerBlock;
    
    for (int i = 4; i < numLevels-2; ++i)
    {
        auto level = -(levels[(nextSample + i + offset) % numLevels].getEnd());
        level = fmin(level, maxDelayLength);
        
        if (i == 4)
            path.startNewSubPath (0.0f, level);
        else
            path.lineTo ((float) i, level);
    }
}

void BlendronicDisplay::paintSmoothing (Graphics& g, Rectangle<float> area, const Range<float>* levels, int numLevels, int nextSample)
{
    g.setColour (waveformColour.withMultipliedBrightness(0.4f));
    
    int offset = playheads[0] * invInputSamplesPerBlock;
    
    float leftLevel = numLevels*horizontalZoom;
    
    for (float f = 0; f < numLevels; f += lineSpacingInBlocks)
    {
        float x = (fmod(f + numLevels - offset + pulseOffset, numLevels) - leftLevel) *
        (area.getRight() - area.getX()) * (1. / (numLevels - leftLevel)) + area.getX();
        g.fillRect(x, area.getY(), 1.0f, area.getHeight());
    }
    
    for (int i = 0; (i*area.getHeight() / (maxDelayLength*1.25)) < area.getHeight(); i++)
    {
        g.fillRect(area.getX(), area.getBottom() - (i*area.getHeight() / (maxDelayLength*1.25)), area.getWidth(), 1.0f);
    }
    
    Path p;
    getSmoothingAsPath (p, levels, numLevels, nextSample);
    
    g.setColour (playheadColour);
    g.strokePath (p, PathStrokeType(2.0),
                  AffineTransform::fromTargetPoints (numLevels*horizontalZoom, -maxDelayLength*1.25f,  area.getX(), area.getY(),
                                                     numLevels*horizontalZoom, 0.0f,  area.getX(), area.getBottom(),
                                                     (float) numLevels, -maxDelayLength*1.25f,  area.getRight(), area.getY()));
    
    for (auto m : markers)
    {
        int offset = playheads[0] * invInputSamplesPerBlock;
        float markerLevel = m * invInputSamplesPerBlock;
        float x = (fmod(((markerLevel - offset) + numLevels), numLevels) - leftLevel) *
        (area.getRight() - area.getX()) * (1. / (numLevels - leftLevel)) + area.getX();
        if (x < 1.0f) continue;
        g.setColour (markerColour);
        g.fillRect(x, area.getY(), 1.0f, area.getHeight());
    }
}

void BlendronicDisplay::sliderValueChanged(Slider *slider)
{
    if (slider == verticalZoomSlider.get())
    {
        verticalZoom = (verticalZoomSliderMin + verticalZoomSliderMax) - verticalZoomSlider->getValue();
    }
    else if (slider == horizontalZoomSlider.get())
    {
        horizontalZoom = horizontalZoomSlider->getValue();
    }
}
