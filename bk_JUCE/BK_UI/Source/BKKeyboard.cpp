/*
 ==============================================================================
 
 BKKeymapKeyboard.cpp
 Created: 27 Mar 2017 12:06:24pm
 Author:  Michael R Mulshine (modified from JUCE BKKeymapKeyboardComponent)
 
 ==============================================================================
 */

#include "BKKeyboard.h"

class BKKeymapKeyboardUpDownButton  : public Button
{
public:
    BKKeymapKeyboardUpDownButton (BKKeymapKeyboardComponent& comp, const int d)
    : Button (String()), owner (comp), delta (d)
    {
    }
    
    void clicked() override
    {
        int note = owner.getLowestVisibleKey();
        
        if (delta < 0)
            note = (note - 1) / 12;
        else
            note = note / 12 + 1;
        
        owner.setLowestVisibleKey (note * 12);
    }
    
    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        owner.drawUpDownButton (g, getWidth(), getHeight(),
                                isMouseOverButton, isButtonDown,
                                delta > 0);
    }
    
private:
    BKKeymapKeyboardComponent& owner;
    const int delta;
    
    JUCE_DECLARE_NON_COPYABLE (BKKeymapKeyboardUpDownButton)
};

//==============================================================================
BKKeymapKeyboardComponent::BKKeymapKeyboardComponent (BKKeymapKeyboardState& s, Orientation o)
: state (s),
blackNoteLengthRatio (0.7f),
xOffset (0),
keyWidth (16.0f),
orientation (o),
midiChannel (1),
midiInChannelMask (0xffff),
velocity (1.0f),
shouldCheckState (false),
rangeStart (0),
rangeEnd (127),
firstKey (12 * 4.0f),
canScroll (true),
useMousePositionForVelocity (true),
shouldCheckMousePos (false),
keyMappingOctave (6),
octaveNumForMiddleC (3)
{
    firstKeyDown = -1; lastKeyDown = -1; lastKeySelected = -1;
    
    addChildComponent (scrollDown = new BKKeymapKeyboardUpDownButton (*this, -1));
    addChildComponent (scrollUp   = new BKKeymapKeyboardUpDownButton (*this, 1));
    
    // initialise with a default set of qwerty key-mappings..
    const char* const keymap = "awsedftgyhujkolp;";
    
    for (int i = 0; keymap[i] != 0; ++i)
        setKeyPressForNote (KeyPress (keymap[i], 0, 0), i);
    
    mouseOverNotes.insertMultiple (0, -1, 32);
    mouseDownNotes.insertMultiple (0, -1, 32);
    
    colourChanged();
    setWantsKeyboardFocus (true);
    
    state.addListener (this);
    
    startTimerHz (50);
}

BKKeymapKeyboardComponent::~BKKeymapKeyboardComponent()
{
    state.removeListener (this);
}

//==============================================================================
void BKKeymapKeyboardComponent::setKeyWidth (const float widthInPixels)
{
    jassert (widthInPixels > 0);
    
    if (keyWidth != widthInPixels) // Prevent infinite recursion if the width is being computed in a 'resized()' call-back
    {
        keyWidth = widthInPixels;
        resized();
    }
}

void BKKeymapKeyboardComponent::setOrientation (const Orientation newOrientation)
{
    if (orientation != newOrientation)
    {
        orientation = newOrientation;
        resized();
    }
}

void BKKeymapKeyboardComponent::setAvailableRange (const int lowestNote,
                                               const int highestNote)
{
    jassert (lowestNote >= 0 && lowestNote <= 127);
    jassert (highestNote >= 0 && highestNote <= 127);
    jassert (lowestNote <= highestNote);
    
    if (rangeStart != lowestNote || rangeEnd != highestNote)
    {
        rangeStart = jlimit (0, 127, lowestNote);
        rangeEnd = jlimit (0, 127, highestNote);
        firstKey = jlimit ((float) rangeStart, (float) rangeEnd, firstKey);
        resized();
    }
}

void BKKeymapKeyboardComponent::setLowestVisibleKey (int noteNumber)
{
    setLowestVisibleKeyFloat ((float) noteNumber);
}

void BKKeymapKeyboardComponent::setLowestVisibleKeyFloat (float noteNumber)
{
    noteNumber = jlimit ((float) rangeStart, (float) rangeEnd, noteNumber);
    
    if (noteNumber != firstKey)
    {
        const bool hasMoved = (((int) firstKey) != (int) noteNumber);
        firstKey = noteNumber;
        
        if (hasMoved)
            sendChangeMessage();
        
        resized();
    }
}

void BKKeymapKeyboardComponent::setScrollButtonsVisible (const bool newCanScroll)
{
    if (canScroll != newCanScroll)
    {
        canScroll = newCanScroll;
        resized();
    }
}

void BKKeymapKeyboardComponent::colourChanged()
{
    setOpaque (findColour (whiteNoteColourId).isOpaque());
    repaint();
}

//==============================================================================
void BKKeymapKeyboardComponent::setMidiChannel (const int midiChannelNumber)
{
    jassert (midiChannelNumber > 0 && midiChannelNumber <= 16);
    
    if (midiChannel != midiChannelNumber)
    {
        resetAnyKeysInUse();
        midiChannel = jlimit (1, 16, midiChannelNumber);
    }
}

void BKKeymapKeyboardComponent::setMidiChannelsToDisplay (const int midiChannelMask)
{
    midiInChannelMask = midiChannelMask;
    shouldCheckState = true;
}

void BKKeymapKeyboardComponent::setVelocity (const float v, const bool useMousePosition)
{
    velocity = jlimit (0.0f, 1.0f, v);
    useMousePositionForVelocity = useMousePosition;
}

//==============================================================================
void BKKeymapKeyboardComponent::getKeyPosition (int midiNoteNumber, const float keyWidth_, int& x, int& w) const
{
    jassert (midiNoteNumber >= 0 && midiNoteNumber < 128);
    
    static const float blackNoteWidth = 0.7f;
    
    static const float notePos[] = { 0.0f, 1 - blackNoteWidth * 0.6f,
        1.0f, 2 - blackNoteWidth * 0.4f,
        2.0f,
        3.0f, 4 - blackNoteWidth * 0.7f,
        4.0f, 5 - blackNoteWidth * 0.5f,
        5.0f, 6 - blackNoteWidth * 0.3f,
        6.0f };
    
    const int octave = midiNoteNumber / 12;
    const int note   = midiNoteNumber % 12;
    
    x = roundToInt (octave * 7.0f * keyWidth_ + notePos [note] * keyWidth_);
    w = roundToInt (MidiMessage::isMidiNoteBlack (note) ? blackNoteWidth * keyWidth_ : keyWidth_);
}

void BKKeymapKeyboardComponent::getKeyPos (int midiNoteNumber, int& x, int& w) const
{
    getKeyPosition (midiNoteNumber, keyWidth, x, w);
    
    int rx, rw;
    getKeyPosition (rangeStart, keyWidth, rx, rw);
    
    x -= xOffset + rx;
}

Rectangle<int> BKKeymapKeyboardComponent::getRectangleForKey (const int note) const
{
    jassert (note >= rangeStart && note <= rangeEnd);
    
    int x, w;
    getKeyPos (note, x, w);
    
    if (MidiMessage::isMidiNoteBlack (note))
    {
        const int blackNoteLength = getBlackNoteLength();
        
        switch (orientation)
        {
            case horizontalKeyboard:            return Rectangle<int> (x, 0, w, blackNoteLength);
            case verticalKeyboardFacingLeft:    return Rectangle<int> (getWidth() - blackNoteLength, x, blackNoteLength, w);
            case verticalKeyboardFacingRight:   return Rectangle<int> (0, getHeight() - x - w, blackNoteLength, w);
            default:                            jassertfalse; break;
        }
    }
    else
    {
        switch (orientation)
        {
            case horizontalKeyboard:            return Rectangle<int> (x, 0, w, getHeight());
            case verticalKeyboardFacingLeft:    return Rectangle<int> (0, x, getWidth(), w);
            case verticalKeyboardFacingRight:   return Rectangle<int> (0, getHeight() - x - w, getWidth(), w);
            default:                            jassertfalse; break;
        }
    }
    
    return Rectangle<int>();
}

int BKKeymapKeyboardComponent::getKeyStartPosition (const int midiNoteNumber) const
{
    int x, w;
    getKeyPos (midiNoteNumber, x, w);
    return x;
}

int BKKeymapKeyboardComponent::getTotalKeyboardWidth() const noexcept
{
    int x, w;
    getKeyPos (rangeEnd, x, w);
    return x + w;
}

int BKKeymapKeyboardComponent::getNoteAtPosition (Point<int> p)
{
    float v;
    return xyToNote (p, v);
}

const uint8 BKKeymapKeyboardComponent::whiteNotes[] = { 0, 2, 4, 5, 7, 9, 11 };
const uint8 BKKeymapKeyboardComponent::blackNotes[] = { 1, 3, 6, 8, 10 };

int BKKeymapKeyboardComponent::xyToNote (Point<int> pos, float& mousePositionVelocity)
{
    if (! reallyContains (pos, false))
        return -1;
    
    Point<int> p (pos);
    
    if (orientation != horizontalKeyboard)
    {
        p = Point<int> (p.y, p.x);
        
        if (orientation == verticalKeyboardFacingLeft)
            p = Point<int> (p.x, getWidth() - p.y);
        else
            p = Point<int> (getHeight() - p.x, p.y);
    }
    
    return remappedXYToNote (p + Point<int> (xOffset, 0), mousePositionVelocity);
}

int BKKeymapKeyboardComponent::remappedXYToNote (Point<int> pos, float& mousePositionVelocity) const
{
    const int blackNoteLength = getBlackNoteLength();
    
    if (pos.getY() < blackNoteLength)
    {
        for (int octaveStart = 12 * (rangeStart / 12); octaveStart <= rangeEnd; octaveStart += 12)
        {
            for (int i = 0; i < 5; ++i)
            {
                const int note = octaveStart + blackNotes [i];
                
                if (note >= rangeStart && note <= rangeEnd)
                {
                    int kx, kw;
                    getKeyPos (note, kx, kw);
                    kx += xOffset;
                    
                    if (pos.x >= kx && pos.x < kx + kw)
                    {
                        mousePositionVelocity = pos.y / (float) blackNoteLength;
                        return note;
                    }
                }
            }
        }
    }
    
    for (int octaveStart = 12 * (rangeStart / 12); octaveStart <= rangeEnd; octaveStart += 12)
    {
        for (int i = 0; i < 7; ++i)
        {
            const int note = octaveStart + whiteNotes [i];
            
            if (note >= rangeStart && note <= rangeEnd)
            {
                int kx, kw;
                getKeyPos (note, kx, kw);
                kx += xOffset;
                
                if (pos.x >= kx && pos.x < kx + kw)
                {
                    const int whiteNoteLength = (orientation == horizontalKeyboard) ? getHeight() : getWidth();
                    mousePositionVelocity = pos.y / (float) whiteNoteLength;
                    return note;
                }
            }
        }
    }
    
    mousePositionVelocity = 0;
    return -1;
}

//==============================================================================
void BKKeymapKeyboardComponent::repaintNote (const int noteNum)
{
    if (noteNum >= rangeStart && noteNum <= rangeEnd)
        repaint (getRectangleForKey (noteNum));
}

void BKKeymapKeyboardComponent::paint (Graphics& g)
{
    g.fillAll (findColour (whiteNoteColourId));
    
    Colour keyColour;
    const Colour lineColour (findColour (keySeparatorLineColourId));
    const Colour textColour (findColour (textLabelColourId));
    
    for (int octave = 0; octave < 128; octave += 12)
    {
        for (int white = 0; white < 7; ++white)
        {
            
            const int noteNum = octave + whiteNotes [white];
            
            if (state.isInKeymap(noteNum))
                keyColour = Colour((uint8)210, (uint8)210, (uint8)64, 0.62f); // KEYMAP COLOUR
            else
                keyColour = Colour(Colours::transparentWhite);
            
            if (noteNum >= rangeStart && noteNum <= rangeEnd)
            {
                Rectangle<int> pos = getRectangleForKey (noteNum);
                
                drawWhiteNote (noteNum, g, pos.getX(), pos.getY(), pos.getWidth(), pos.getHeight(),
                               state.isNoteOnForChannels (midiInChannelMask, noteNum),
                               mouseOverNotes.contains (noteNum), keyColour, lineColour, textColour);
            }
        }
    }
    
    float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
    const int width = getWidth();
    const int height = getHeight();
    
    if (orientation == verticalKeyboardFacingLeft)
    {
        x1 = width - 1.0f;
        x2 = width - 5.0f;
    }
    else if (orientation == verticalKeyboardFacingRight)
        x2 = 5.0f;
    else
        y2 = 5.0f;
    
    int x, w;
    getKeyPos (rangeEnd, x, w);
    x += w;
    
    const Colour shadowCol (findColour (shadowColourId));
    
    if (! shadowCol.isTransparent())
    {
        g.setGradientFill (ColourGradient (shadowCol, x1, y1, shadowCol.withAlpha (0.0f), x2, y2, false));
        
        switch (orientation)
        {
            case horizontalKeyboard:            g.fillRect (0, 0, x, 5); break;
            case verticalKeyboardFacingLeft:    g.fillRect (width - 5, 0, 5, x); break;
            case verticalKeyboardFacingRight:   g.fillRect (0, 0, 5, x); break;
            default: break;
        }
    }
    
    if (! lineColour.isTransparent())
    {
        g.setColour (lineColour);
        
        switch (orientation)
        {
            case horizontalKeyboard:            g.fillRect (0, height - 1, x, 1); break;
            case verticalKeyboardFacingLeft:    g.fillRect (0, 0, 1, x); break;
            case verticalKeyboardFacingRight:   g.fillRect (width - 1, 0, 1, x); break;
            default: break;
        }
    }
    
    const Colour blackNoteColour (findColour (blackNoteColourId));
    
    for (int octave = 0; octave < 128; octave += 12)
    {
        for (int black = 0; black < 5; ++black)
        {
            const int noteNum = octave + blackNotes [black];
            
            if (state.isInKeymap(noteNum))
                keyColour = findColour(keyDownOverlayColourId);
            else
                keyColour = blackNoteColour;
            
            if (noteNum >= rangeStart && noteNum <= rangeEnd)
            {
                Rectangle<int> pos = getRectangleForKey (noteNum);
                
                drawBlackNote (noteNum, g, pos.getX(), pos.getY(), pos.getWidth(), pos.getHeight(),
                               state.isNoteOnForChannels (midiInChannelMask, noteNum),
                               mouseOverNotes.contains (noteNum), keyColour);
            }
        }
    }
}

void BKKeymapKeyboardComponent::drawWhiteNote (int midiNoteNumber,
                                           Graphics& g, int x, int y, int w, int h,
                                           bool isDown, bool isOver,
                                           const Colour& keyColour,
                                           const Colour& lineColour,
                                           const Colour& textColour)
{
    Colour c (keyColour);
    
    if (isDown)  c = findColour (keyDownOverlayColourId);
    if (isOver)  c = c.overlaidWith (findColour (mouseOverKeyOverlayColourId));
    
    g.setColour (c);
    g.fillRect (x, y, w, h);
    
    const String text (getWhiteNoteText (midiNoteNumber));
    
    if (text.isNotEmpty())
    {
        const float fontHeight = jmin (12.0f, keyWidth * 0.9f);
        
        g.setColour (textColour);
        g.setFont (Font (fontHeight).withHorizontalScale (0.8f));
        
        switch (orientation)
        {
            case horizontalKeyboard:            g.drawText (text, x + 1, y,     w - 1, h - 2, Justification::centredBottom, false); break;
            case verticalKeyboardFacingLeft:    g.drawText (text, x + 2, y + 2, w - 4, h - 4, Justification::centredLeft,   false); break;
            case verticalKeyboardFacingRight:   g.drawText (text, x + 2, y + 2, w - 4, h - 4, Justification::centredRight,  false); break;
            default: break;
        }
    }
    
    if (! lineColour.isTransparent())
    {
        g.setColour (lineColour);
        
        switch (orientation)
        {
            case horizontalKeyboard:            g.fillRect (x, y, 1, h); break;
            case verticalKeyboardFacingLeft:    g.fillRect (x, y, w, 1); break;
            case verticalKeyboardFacingRight:   g.fillRect (x, y + h - 1, w, 1); break;
            default: break;
        }
        
        if (midiNoteNumber == rangeEnd)
        {
            switch (orientation)
            {
                case horizontalKeyboard:            g.fillRect (x + w, y, 1, h); break;
                case verticalKeyboardFacingLeft:    g.fillRect (x, y + h, w, 1); break;
                case verticalKeyboardFacingRight:   g.fillRect (x, y - 1, w, 1); break;
                default: break;
            }
        }
    }
}

void BKKeymapKeyboardComponent::drawBlackNote (int /*midiNoteNumber*/,
                                           Graphics& g, int x, int y, int w, int h,
                                           bool isDown, bool isOver,
                                           const Colour& noteFillColour)
{
    Colour c (noteFillColour);
    
    if (isDown)  c = c.overlaidWith (findColour (keyDownOverlayColourId));
    if (isOver)  c = c.overlaidWith (findColour (mouseOverKeyOverlayColourId));
    
    g.setColour (c);
    g.fillRect (x, y, w, h);
    
    if (isDown)
    {
        g.setColour (noteFillColour);
        g.drawRect (x, y, w, h);
    }
    else
    {
        g.setColour (c.brighter());
        const int xIndent = jmax (1, jmin (w, h) / 8);
        
        switch (orientation)
        {
            case horizontalKeyboard:            g.fillRect (x + xIndent, y, w - xIndent * 2, 7 * h / 8); break;
            case verticalKeyboardFacingLeft:    g.fillRect (x + w / 8, y + xIndent, w - w / 8, h - xIndent * 2); break;
            case verticalKeyboardFacingRight:   g.fillRect (x, y + xIndent, 7 * w / 8, h - xIndent * 2); break;
            default: break;
        }
    }
}

void BKKeymapKeyboardComponent::setOctaveForMiddleC (const int octaveNum)
{
    octaveNumForMiddleC = octaveNum;
    repaint();
}

String BKKeymapKeyboardComponent::getWhiteNoteText (const int midiNoteNumber)
{
    if (midiNoteNumber % 12 == 0)
        return MidiMessage::getMidiNoteName (midiNoteNumber, true, true, octaveNumForMiddleC);
    
    return String();
}

void BKKeymapKeyboardComponent::drawUpDownButton (Graphics& g, int w, int h,
                                              const bool mouseOver,
                                              const bool buttonDown,
                                              const bool movesOctavesUp)
{
    g.fillAll (findColour (upDownButtonBackgroundColourId));
    
    float angle;
    
    switch (orientation)
    {
        case horizontalKeyboard:            angle = movesOctavesUp ? 0.0f  : 0.5f;  break;
        case verticalKeyboardFacingLeft:    angle = movesOctavesUp ? 0.25f : 0.75f; break;
        case verticalKeyboardFacingRight:   angle = movesOctavesUp ? 0.75f : 0.25f; break;
        default:                            jassertfalse; angle = 0; break;
    }
    
    Path path;
    path.addTriangle (0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
    path.applyTransform (AffineTransform::rotation (float_Pi * 2.0f * angle, 0.5f, 0.5f));
    
    g.setColour (findColour (upDownButtonArrowColourId)
                 .withAlpha (buttonDown ? 1.0f : (mouseOver ? 0.6f : 0.4f)));
    
    g.fillPath (path, path.getTransformToScaleToFit (1.0f, 1.0f, w - 2.0f, h - 2.0f, true));
}

void BKKeymapKeyboardComponent::setBlackNoteLengthProportion (float ratio) noexcept
{
    jassert (ratio >= 0.0f && ratio <= 1.0f);
    if (blackNoteLengthRatio != ratio)
    {
        blackNoteLengthRatio = ratio;
        resized();
    }
}

int BKKeymapKeyboardComponent::getBlackNoteLength() const noexcept
{
    const int whiteNoteLength = orientation == horizontalKeyboard ? getHeight() : getWidth();
    
    return roundToInt (whiteNoteLength * blackNoteLengthRatio);
}

void BKKeymapKeyboardComponent::resized()
{
    int w = getWidth();
    int h = getHeight();
    
    if (w > 0 && h > 0)
    {
        if (orientation != horizontalKeyboard)
            std::swap (w, h);
        
        int kx2, kw2;
        getKeyPos (rangeEnd, kx2, kw2);
        
        kx2 += kw2;
        
        if ((int) firstKey != rangeStart)
        {
            int kx1, kw1;
            getKeyPos (rangeStart, kx1, kw1);
            
            if (kx2 - kx1 <= w)
            {
                firstKey = (float) rangeStart;
                sendChangeMessage();
                repaint();
            }
        }
        
        scrollDown->setVisible (canScroll && firstKey > (float) rangeStart);
        
        xOffset = 0;
        
        if (canScroll)
        {
            const int scrollButtonW = jmin (12, w / 2);
            Rectangle<int> r (getLocalBounds());
            
            if (orientation == horizontalKeyboard)
            {
                scrollDown->setBounds (r.removeFromLeft  (scrollButtonW));
                scrollUp  ->setBounds (r.removeFromRight (scrollButtonW));
            }
            else if (orientation == verticalKeyboardFacingLeft)
            {
                scrollDown->setBounds (r.removeFromTop    (scrollButtonW));
                scrollUp  ->setBounds (r.removeFromBottom (scrollButtonW));
            }
            else
            {
                scrollDown->setBounds (r.removeFromBottom (scrollButtonW));
                scrollUp  ->setBounds (r.removeFromTop    (scrollButtonW));
            }
            
            int endOfLastKey, kw;
            getKeyPos (rangeEnd, endOfLastKey, kw);
            endOfLastKey += kw;
            
            float mousePositionVelocity;
            const int spaceAvailable = w;
            const int lastStartKey = remappedXYToNote (Point<int> (endOfLastKey - spaceAvailable, 0), mousePositionVelocity) + 1;
            
            if (lastStartKey >= 0 && ((int) firstKey) > lastStartKey)
            {
                firstKey = (float) jlimit (rangeStart, rangeEnd, lastStartKey);
                sendChangeMessage();
            }
            
            int newOffset = 0;
            getKeyPos ((int) firstKey, newOffset, kw);
            xOffset = newOffset;
        }
        else
        {
            firstKey = (float) rangeStart;
        }
        
        getKeyPos (rangeEnd, kx2, kw2);
        scrollUp->setVisible (canScroll && kx2 > w);
        repaint();
    }
}

void BKKeymapKeyboardComponent::handleKeymapNoteOn (BKKeymapKeyboardState*, int /*midiNoteNumber*/)
{
    shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
}

void BKKeymapKeyboardComponent::handleKeymapNoteOff (BKKeymapKeyboardState*, int /*midiNoteNumber*/)
{
    shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
}

void BKKeymapKeyboardComponent::handleKeymapNoteToggled (BKKeymapKeyboardState*, int /*midiNoteNumber*/)
{
    shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
}



//==============================================================================
void BKKeymapKeyboardComponent::resetAnyKeysInUse()
{
    if (! keysPressed.isZero())
    {
        for (int i = 128; --i >= 0;)
            if (keysPressed[i])
                state.noteOff (midiChannel, i, 0.0f);
        
        keysPressed.clear();
    }
    
    for (int i = mouseDownNotes.size(); --i >= 0;)
    {
        const int noteDown = mouseDownNotes.getUnchecked(i);
        
        if (noteDown >= 0)
        {
            state.noteOff (midiChannel, noteDown, 0.0f);
            mouseDownNotes.set (i, -1);
        }
        
        mouseOverNotes.set (i, -1);
    }
}

void BKKeymapKeyboardComponent::updateNoteUnderMouse (const MouseEvent& e, bool isDown)
{
    updateNoteUnderMouse (e.getEventRelativeTo (this).getPosition(), isDown, e.source.getIndex());
}

void BKKeymapKeyboardComponent::updateNoteUnderMouse (Point<int> pos, bool isDown, int fingerNum)
{
    float mousePositionVelocity = 0.0f;
    const int newNote = xyToNote (pos, mousePositionVelocity);
    const int oldNote = mouseOverNotes.getUnchecked (fingerNum);
    const int oldNoteDown = mouseDownNotes.getUnchecked (fingerNum);
    const float eventVelocity = useMousePositionForVelocity ? mousePositionVelocity * velocity : 1.0f;
    
    lastNoteOver = newNote;
    
    if (oldNote != newNote)
    {
        repaintNote (oldNote);
        repaintNote (newNote);
        mouseOverNotes.set (fingerNum, newNote);
    }
    
    if (isDown)
    {
        if (newNote != oldNoteDown)
        {
            if (oldNoteDown >= 0)
            {
                mouseDownNotes.set (fingerNum, -1);
                
                if (! mouseDownNotes.contains (oldNoteDown))
                    state.noteOff (midiChannel, oldNoteDown, eventVelocity);
            }
            
            if (newNote >= 0 && ! mouseDownNotes.contains (newNote))
            {
                state.noteOn (midiChannel, newNote, eventVelocity);
                mouseDownNotes.set (fingerNum, newNote);
            }
        }
    }
    else if (oldNoteDown >= 0)
    {
        mouseDownNotes.set (fingerNum, -1);
        
        if (! mouseDownNotes.contains (oldNoteDown))
            state.noteOff (midiChannel, oldNoteDown, eventVelocity);
    }
}

void BKKeymapKeyboardComponent::mouseMove (const MouseEvent& e)
{
    updateNoteUnderMouse (e, false);
    shouldCheckMousePos = false;
}

void BKKeymapKeyboardComponent::mouseDrag (const MouseEvent& e)
{
    float mousePositionVelocity;
    const int newNote = xyToNote (e.getPosition(), mousePositionVelocity);
    
    if (newNote >= 0)
        mouseDraggedToKey (newNote, e);
    
    updateNoteUnderMouse (e, true);
}

void BKKeymapKeyboardComponent::setKeysInKeymap(Array<int> keys)
{
    Array<bool> keymap;

    for (int i = 0; i < 128; i++) keymap.add(false);
    
    for (auto key : keys) keymap.set(key, true);
    
    state.setKeymap(keymap);
    
    repaint();
}

// FLESH THESE OUT TO ALLOW FOR DRAGGING
bool BKKeymapKeyboardComponent::mouseDownOnKey    (int midiNoteNumber, const MouseEvent&)
{
    if (lastKeySelected == -1)
    {
        lastKeySelected = midiNoteNumber;
    }
    
    return true;
}

void BKKeymapKeyboardComponent::mouseDraggedToKey (int midiNoteNumber, const MouseEvent&)
{
    if (midiNoteNumber != lastKeySelected)
    {
        state.toggle(lastKeySelected);
        repaint(getRectangleForKey(lastKeySelected));
        
        lastKeySelected = midiNoteNumber;
    }
}

void BKKeymapKeyboardComponent::mouseUpOnKey      (int midiNoteNumber, const MouseEvent&)
{
    state.toggle(midiNoteNumber);
    repaint(getRectangleForKey(midiNoteNumber));
    
    lastKeySelected = -1;
    
}

void BKKeymapKeyboardComponent::mouseDown (const MouseEvent& e)
{
    float mousePositionVelocity;
    const int newNote = xyToNote (e.getPosition(), mousePositionVelocity);
    
    if (newNote >= 0 && mouseDownOnKey (newNote, e))
    {
        updateNoteUnderMouse (e, true);
        shouldCheckMousePos = true;
    }
}

void BKKeymapKeyboardComponent::mouseUp (const MouseEvent& e)
{
    updateNoteUnderMouse (e, false);
    shouldCheckMousePos = false;
    
    float mousePositionVelocity;
    const int note = xyToNote (e.getPosition(), mousePositionVelocity);
    if (note >= 0)
    {
        mouseUpOnKey (note, e);
    }
}

void BKKeymapKeyboardComponent::mouseEnter (const MouseEvent& e)
{
    updateNoteUnderMouse (e, false);
}

void BKKeymapKeyboardComponent::mouseExit (const MouseEvent& e)
{
    updateNoteUnderMouse (e, false);
}

void BKKeymapKeyboardComponent::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel)
{
    const float amount = (orientation == horizontalKeyboard && wheel.deltaX != 0)
    ? wheel.deltaX : (orientation == verticalKeyboardFacingLeft ? wheel.deltaY
                      : -wheel.deltaY);
    
    setLowestVisibleKeyFloat (firstKey - amount * keyWidth);
}

void BKKeymapKeyboardComponent::timerCallback()
{
    if (shouldCheckState)
    {
        shouldCheckState = false;
        
        for (int i = rangeStart; i <= rangeEnd; ++i)
        {
            if (keysCurrentlyDrawnDown[i] != state.isNoteOnForChannels (midiInChannelMask, i))
            {
                keysCurrentlyDrawnDown.setBit (i, state.isNoteOnForChannels (midiInChannelMask, i));
                repaintNote (i);
            }
        }
    }
    
    if (shouldCheckMousePos)
    {
        const Array<MouseInputSource>& mouseSources = Desktop::getInstance().getMouseSources();
        
        for (MouseInputSource* mi = mouseSources.begin(), * const e = mouseSources.end(); mi != e; ++mi)
            if (mi->getComponentUnderMouse() == this || isParentOf (mi->getComponentUnderMouse()))
                updateNoteUnderMouse (getLocalPoint (nullptr, mi->getScreenPosition()).roundToInt(), mi->isDragging(), mi->getIndex());
    }
}

//==============================================================================
void BKKeymapKeyboardComponent::clearKeyMappings()
{
    resetAnyKeysInUse();
    keyPressNotes.clear();
    keyPresses.clear();
}

void BKKeymapKeyboardComponent::setKeyPressForNote (const KeyPress& key, int midiNoteOffsetFromC)
{
    removeKeyPressForNote (midiNoteOffsetFromC);
    
    keyPressNotes.add (midiNoteOffsetFromC);
    keyPresses.add (key);
}

void BKKeymapKeyboardComponent::removeKeyPressForNote (const int midiNoteOffsetFromC)
{
    for (int i = keyPressNotes.size(); --i >= 0;)
    {
        if (keyPressNotes.getUnchecked (i) == midiNoteOffsetFromC)
        {
            keyPressNotes.remove (i);
            keyPresses.remove (i);
        }
    }
}

void BKKeymapKeyboardComponent::setKeyPressBaseOctave (const int newOctaveNumber)
{
    jassert (newOctaveNumber >= 0 && newOctaveNumber <= 10);
    
    keyMappingOctave = newOctaveNumber;
}

bool BKKeymapKeyboardComponent::keyStateChanged (const bool /*isKeyDown*/)
{
    bool keyPressUsed = false;
    
    for (int i = keyPresses.size(); --i >= 0;)
    {
        const int note = 12 * keyMappingOctave + keyPressNotes.getUnchecked (i);
        
        if (keyPresses.getReference(i).isCurrentlyDown())
        {
            if (! keysPressed [note])
            {
                keysPressed.setBit (note);
                state.noteOn (midiChannel, note, velocity);
                keyPressUsed = true;
            }
        }
        else
        {
            if (keysPressed [note])
            {
                keysPressed.clearBit (note);
                state.noteOff (midiChannel, note, 0.0f);
                keyPressUsed = true;
            }
        }
    }
    
    return keyPressUsed;
}

bool BKKeymapKeyboardComponent::keyPressed (const KeyPress& key)
{
    return keyPresses.contains (key);
}

void BKKeymapKeyboardComponent::focusLost (FocusChangeType)
{
    resetAnyKeysInUse();
}
