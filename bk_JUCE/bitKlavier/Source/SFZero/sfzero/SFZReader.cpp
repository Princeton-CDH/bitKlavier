/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#include "SFZReader.h"
#include "SFZRegion.h"
#include "SFZSound.h"
#include "SFZSample.h"

sfzero::Reader::Reader(sfzero::Sound::Ptr soundIn) : sound_(soundIn), line_(1) {}

sfzero::Reader::~Reader() {}

void sfzero::Reader::read(const juce::File &file)
{
    juce::MemoryBlock contents;
    bool ok = file.loadFileAsData(contents);
    
    if (!ok)
    {
        sound_->addError("Couldn't read \"" + file.getFullPathName() + "\"");
        return;
    }
    
    read(static_cast<const char *>(contents.getData()), static_cast<int>(contents.getSize()));
}

void sfzero::Reader::read(const char *text, unsigned int length)
{
    const char *p = text;
    const char *end = text + length;
    char c = 0;
    
    sfzero::Region::Ptr curGroup = new Region();
    sfzero::Region::Ptr curRegion = new Region();
    sfzero::Region::Ptr buildingRegion = nullptr;
    
    int locc64,hicc64;
    int on_locc64,on_hicc64;
    bool inControl = false, inMaster = false;
    bool pedal_up = false,pedal_down = false;
    bool pedal = false;
    sfzero::Region::Trigger masterTrig = sfzero::Region::trignil;
    int masterLoVel = -1, masterHiVel = -1;
    bool regionTrigSpecified = false, regionVelSpecified = false;;
    juce::String defaultPath;
    
    while (p < end)
    {
        // We're at the start of a line; skip any whitespace.
        while (p < end)
        {
            c = *p;
            if ((c != ' ') && (c != '\t'))
            {
                break;
            }
            p += 1;
        }
        if (p >= end)
        {
            break;
        }
        
        // Check if it's a comment line.
        if (c == '/')
        {
            // Skip to end of line.
            while (p < end)
            {
                c = *++p;
                if ((c == '\n') || (c == '\r'))
                {
                    break;
                }
            }
            p = handleLineEnd(p);
            continue;
        }
        
        // Check if it's a blank line.
        if ((c == '\r') || (c == '\n'))
        {
            p = handleLineEnd(p);
            continue;
        }
        
        // Handle elements on the line.
        bool loopModeSet;
        while (p < end)
        {
            c = *p;
            
            // Tag.
            if (c == '<')
            {
                p += 1;
                const char *tagStart = p;
                while (p < end)
                {
                    c = *p++;
                    if ((c == '\n') || (c == '\r'))
                    {
                        error("Unterminated tag");
                        goto fatalError;
                    }
                    else if (c == '>')
                    {
                        break;
                    }
                }
                if (p >= end)
                {
                    error("Unterminated tag");
                    goto fatalError;
                }
                sfzero::StringSlice tag(tagStart, p - 1);
                if (tag == "region")
                {
                    if (buildingRegion && (buildingRegion.get() == curRegion.get()))
                    {
                        if (!regionTrigSpecified && masterTrig != sfzero::Region::trignil)
                        {
                            buildingRegion->trigger = masterTrig;
                        }
                        
                        if (!regionVelSpecified && (masterHiVel >= 0) && (masterLoVel >= 0))
                        {
                            buildingRegion->lovel = masterLoVel;
                            buildingRegion->hivel = masterHiVel;
                        }
                        
                        buildingRegion->pedal = pedal;
                        
                        finishRegion(curRegion);
                    }
                    
                    curRegion = curGroup;
                    buildingRegion = curRegion;
                    
                    regionTrigSpecified = false;
                    
                    
                    inControl = false;
                    inMaster = false;
                }
                else if (tag == "group")
                {
                    if (buildingRegion && (buildingRegion.get() == curRegion.get()))
                    {
                        if (!regionTrigSpecified && masterTrig != sfzero::Region::trignil)
                        {
                            buildingRegion->trigger = masterTrig;
                        }
                        
                        if (!regionVelSpecified && (masterHiVel >= 0) && (masterLoVel >= 0))
                        {
                            buildingRegion->lovel = masterLoVel;
                            buildingRegion->hivel = masterHiVel;
                        }
                        
                        buildingRegion->pedal = pedal;
                        
                        finishRegion(curRegion);
                    }
                    
                    curGroup->clear();
                    buildingRegion = curGroup;
                    
                    inControl = false;
                    inMaster = false;
                }
                else if (tag == "master")
                {
                    inMaster = true;
                    pedal = false;
                    masterTrig = sfzero::Region::trignil;
                }
                else if (tag == "control")
                {
                    if (buildingRegion && (buildingRegion.get() == curRegion.get()))
                    {
                        if (!regionTrigSpecified && masterTrig != sfzero::Region::trignil)
                        {
                            buildingRegion->trigger = masterTrig;
                        }
                        
                        if (!regionVelSpecified && (masterHiVel >= 0) && (masterLoVel >= 0))
                        {
                            buildingRegion->lovel = masterLoVel;
                            buildingRegion->hivel = masterHiVel;
                        }
                        
                        buildingRegion->pedal = pedal;
                        
                        finishRegion(curRegion);
                    }
                    
                    curGroup->clear();
                    buildingRegion = nullptr;
                    inControl = true;
                    inMaster = false;
                }
                else
                {
                    error("Illegal tag");
                }
            }
            // Comment.
            else if (c == '/')
            {
                // Skip to end of line.
                while (p < end)
                {
                    c = *p;
                    if ((c == '\r') || (c == '\n'))
                    {
                        break;
                    }
                    p += 1;
                }
            }
            // Parameter.
            else
            {
                // Get the parameter name.
                const char *parameterStart = p;
                while (p < end)
                {
                    c = *p++;
                    
                    if ((c == '=') || (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
                    {
                        break;
                    }
                }
                if ((p >= end) || (c != '='))
                {
                    error("Malformed parameter");
                    goto nextElement;
                }
                sfzero::StringSlice opcode(parameterStart, p - 1);
                if (inControl)
                {
                    if (opcode == "default_path")
                    {
                        p = readPathInto(&defaultPath, p, end);
                    }
                    else
                    {
                        const char *valueStart = p;
                        while (p < end)
                        {
                            c = *p;
                            if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
                            {
                                break;
                            }
                            p++;
                        }
                        juce::String value(valueStart, p - valueStart);
                        juce::String fauxOpcode = juce::String(opcode.getStart(), opcode.length()) + " (in <control>)";
                        sound_->addUnsupportedOpcode(fauxOpcode);
                    }
                }
                else if (opcode == "sample")
                {
                    juce::String path;
                    p = readPathInto(&path, p, end);
                    if (!path.isEmpty())
                    {
                        if (buildingRegion)
                        {
                            buildingRegion->sample = sound_->addSample(path, defaultPath);
                        }
                        else
                        {
                            error("Adding sample outside a group or region");
                        }
                    }
                    else
                    {
                        error("Empty sample path");
                    }
                }
                else
                {
                    const char *valueStart = p;
                    while (p < end)
                    {
                        c = *p;
                        if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
                        {
                            break;
                        }
                        p++;
                    }
                    juce::String value(valueStart, p - valueStart);
                    if (buildingRegion == nullptr)
                    {
                        error("Setting a parameter outside a region or group");
                    }
                    else if (opcode == "lokey")
                    {
                        buildingRegion->lokey = keyValue(value);
                    }
                    else if (opcode == "hikey")
                    {
                        buildingRegion->hikey = keyValue(value);
                    }
                    else if (opcode == "key")
                    {
                        buildingRegion->hikey = buildingRegion->lokey = buildingRegion->pitch_keycenter = keyValue(value);
                    }
                    else if (opcode == "lovel")
                    {
                        if (inMaster)
                        {
                            masterLoVel = value.getIntValue();
                        }
                        else
                        {
                            regionVelSpecified = true;
                            buildingRegion->lovel = value.getIntValue();
                        }
                    }
                    else if (opcode == "hivel")
                    {
                        if (inMaster)
                        {
                            masterHiVel = value.getIntValue();
                        }
                        else
                        {
                            regionVelSpecified = true;
                            buildingRegion->hivel = value.getIntValue();
                        }
                    }
                    else if (opcode == "trigger")
                    {
                        if (inMaster)
                        {
                            masterTrig = static_cast<sfzero::Region::Trigger>(triggerValue(value));
                        }
                        else
                        {
                            regionTrigSpecified = true;
                            buildingRegion->trigger = static_cast<sfzero::Region::Trigger>(triggerValue(value));
                        }
                        
                    }
                    else if (opcode == "group")
                    {
                        buildingRegion->group = static_cast<int>(value.getLargeIntValue());
                    }
                    else if (opcode == "locc64")
                    {
                        locc64 = static_cast<int>(value.getLargeIntValue());
                        
                        if (locc64 == 127 && hicc64 == 127) pedal = true;
                    }
                    else if (opcode == "hicc64")
                    {
                        hicc64 = static_cast<int>(value.getLargeIntValue());
                        
                        if (locc64 == 127 && hicc64 == 127) pedal = true;
                    }
                    else if (opcode == "on_locc64")
                    {
                        on_locc64 = static_cast<int>(value.getLargeIntValue());
                        
                        if (on_locc64 == 0 && on_hicc64 == 0)     pedal_up = true;
                        if (on_locc64 == 127 && on_hicc64 == 127) pedal_down = true;
                    }
                    else if (opcode == "on_hicc64")
                    {
                        on_hicc64 = static_cast<int>(value.getLargeIntValue());
                        
                        if (on_locc64 == 0 && on_hicc64 == 0)     pedal_up = true;
                        if (on_locc64 == 127 && on_hicc64 == 127) pedal_down = true;
                    }
                    else if (opcode == "off_by")
                    {
                        buildingRegion->off_by = value.getLargeIntValue();
                    }
                    else if (opcode == "offset")
                    {
                        buildingRegion->offset = value.getLargeIntValue();
                    }
                    else if (opcode == "end")
                    {
                        juce::int64 end2 = value.getLargeIntValue();
                        if (end2 < 0)
                        {
                            buildingRegion->negative_end = true;
                        }
                        else
                        {
                            buildingRegion->end = end2;
                        }
                    }
                    else if (opcode == "loop_mode")
                    {
                        loopModeSet = true;
                        bool modeIsSupported = value == "no_loop" || value == "one_shot" || value == "loop_continuous";
                        if (modeIsSupported)
                        {
                            buildingRegion->loop_mode = static_cast<sfzero::Region::LoopMode>(loopModeValue(value));
                        }
                        else
                        {
                            juce::String fauxOpcode = juce::String(opcode.getStart(), opcode.length()) + "=" + value;
                            sound_->addUnsupportedOpcode(fauxOpcode);
                        }
                    }
                    else if (opcode == "loop_start")
                    {
                        buildingRegion->loop_start = value.getLargeIntValue();
                    }
                    else if (opcode == "loop_end")
                    {
                        buildingRegion->loop_end = value.getLargeIntValue();
                    }
                    else if (opcode == "transpose")
                    {
                        buildingRegion->transpose = value.getIntValue();
                    }
                    else if (opcode == "tune")
                    {
                        buildingRegion->tune = value.getIntValue();
                    }
                    else if (opcode == "pitch_keycenter")
                    {
                        buildingRegion->pitch_keycenter = keyValue(value);
                    }
                    else if (opcode == "pitch_keytrack")
                    {
                        buildingRegion->pitch_keytrack = value.getIntValue();
                    }
                    else if (opcode == "bend_up")
                    {
                        buildingRegion->bend_up = value.getIntValue();
                    }
                    else if (opcode == "bend_down")
                    {
                        buildingRegion->bend_down = value.getIntValue();
                    }
                    else if (opcode == "volume")
                    {
                        buildingRegion->volume = value.getFloatValue();
                    }
                    else if (opcode == "pan")
                    {
                        buildingRegion->pan = value.getFloatValue();
                    }
                    else if (opcode == "amp_veltrack")
                    {
                        buildingRegion->amp_veltrack = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_delay")
                    {
                        buildingRegion->ampeg.delay = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_start")
                    {
                        buildingRegion->ampeg.start = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_attack")
                    {
                        buildingRegion->ampeg.attack = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_hold")
                    {
                        buildingRegion->ampeg.hold = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_decay")
                    {
                        buildingRegion->ampeg.decay = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_sustain")
                    {
                        buildingRegion->ampeg.sustain = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_release")
                    {
                        buildingRegion->ampeg.release = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2delay")
                    {
                        buildingRegion->ampeg_veltrack.delay = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2attack")
                    {
                        buildingRegion->ampeg_veltrack.attack = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2hold")
                    {
                        buildingRegion->ampeg_veltrack.hold = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2decay")
                    {
                        buildingRegion->ampeg_veltrack.decay = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2sustain")
                    {
                        buildingRegion->ampeg_veltrack.sustain = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2release")
                    {
                        buildingRegion->ampeg_veltrack.release = value.getFloatValue();
                    }
                    else if (opcode == "default_path")
                    {
                        error("\"default_path\" outside of <control> tag");
                    }
                    else
                    {
                        sound_->addUnsupportedOpcode(juce::String(opcode.getStart(), opcode.length()));
                    }
                }
            }
            
            // Skip to next element.
        nextElement:
            c = 0;
            while (p != nullptr && p < end)
            {
                c = *p;
                if ((c != ' ') && (c != '\t'))
                {
                    break;
                }
                p += 1;
            }
            if ((c == '\r') || (c == '\n'))
            {
                p = handleLineEnd(p);
                break;
            }
        }
        //if (buildingRegion != nullptr && loopModeSet == false) buildingRegion->loop_mode = sfzero::Region::loop_continuous;
    }
    
fatalError:
    if (buildingRegion && (buildingRegion.get() == curRegion.get()))
    {
        if (!regionTrigSpecified && masterTrig != sfzero::Region::trignil)
        {
            buildingRegion->trigger = masterTrig;
        }
        
        if (!regionVelSpecified && (masterHiVel >= 0) && (masterLoVel >= 0))
        {
            buildingRegion->lovel = masterLoVel;
            buildingRegion->hivel = masterHiVel;
        }
        
        buildingRegion->pedal = pedal;
        buildingRegion->pedal_up = pedal_up;
        buildingRegion->pedal_down = pedal_down;
        
        finishRegion(curRegion);
    }
}

const char *sfzero::Reader::handleLineEnd(const char *p)
{
    // Check for DOS-style line ending.
    char lineEndChar = *p++;
    
    if ((lineEndChar == '\r') && (*p == '\n'))
    {
        p += 1;
    }
    line_ += 1;
    return p;
}

const char *sfzero::Reader::readPathInto(juce::String *pathOut, const char *pIn, const char *endIn)
{
    // Paths are kind of funny to parse because they can contain whitespace.
    const char *p = pIn;
    const char *end = endIn;
    const char *pathStart = p;
    const char *potentialEnd = nullptr;
    
    while (p < end)
    {
        char c = *p;
        if (c == ' ')
        {
            // Is this space part of the path?  Or the start of the next opcode?  We
            // don't know yet.
            potentialEnd = p;
            p += 1;
            // Skip any more spaces.
            while (p < end && *p == ' ')
            {
                p += 1;
            }
        }
        else if ((c == '\n') || (c == '\r') || (c == '\t'))
        {
            break;
        }
        else if (c == '=')
        {
            // We've been looking at an opcode; we need to rewind to
            // potentialEnd.
            p = potentialEnd;
            break;
        }
        p += 1;
    }
    if (p > pathStart)
    {
        // Can't do this:
        //      juce::String path(CharPointer_UTF8(pathStart), CharPointer_UTF8(p));
        // It won't compile for some unfathomable reason.
        juce::CharPointer_UTF8 end2(p);
        juce::String path(juce::CharPointer_UTF8(pathStart), end2);
        *pathOut = path;
    }
    else
    {
        *pathOut = juce::String();
    }
    return p;
}

int sfzero::Reader::keyValue(const juce::String &str)
{
    auto chars = str.toRawUTF8();
    
    char c = chars[0];
    
    if ((c >= '0') && (c <= '9'))
    {
        return str.getIntValue();
    }
    
    int note = 0;
    static const int notes[] = {
        12 + 0, 12 + 2, 3, 5, 7, 8, 10,
    };
    if ((c >= 'A') && (c <= 'G'))
    {
        note = notes[c - 'A'];
    }
    else if ((c >= 'a') && (c <= 'g'))
    {
        note = notes[c - 'a'];
    }
    int octaveStart = 1;
    
    c = chars[1];
    if ((c == 'b') || (c == '#'))
    {
        octaveStart += 1;
        if (c == 'b')
        {
            note -= 1;
        }
        else
        {
            note += 1;
        }
    }
    
    int octave = str.substring(octaveStart).getIntValue();
    // A3 == 57.
    int result = octave * 12 + note + (57 - 4 * 12);
    return result;
}

int sfzero::Reader::triggerValue(const juce::String &str)
{
    if (str == "release")
    {
        return sfzero::Region::release;
    }
    else if (str == "release_key")
    {
        return sfzero::Region::release_key;
    }
    else if (str == "first")
    {
        return sfzero::Region::first;
    }
    else if (str == "legato")
    {
        return sfzero::Region::legato;
    }
    return sfzero::Region::attack;
}

int sfzero::Reader::loopModeValue(const juce::String &str)
{
    if (str == "no_loop")
    {
        return sfzero::Region::no_loop;
    }
    else if (str == "one_shot")
    {
        return sfzero::Region::one_shot;
    }
    else if (str == "loop_continuous")
    {
        return sfzero::Region::loop_continuous;
    }
    else if (str == "loop_sustain")
    {
        return sfzero::Region::loop_sustain;
    }
    return sfzero::Region::sample_loop;
}

void sfzero::Reader::finishRegion(sfzero::Region::Ptr region)
{
    sfzero::Region::Ptr newRegion = new sfzero::Region();
    
    *newRegion = *region;
    sound_->addRegion(newRegion);
}

void sfzero::Reader::error(const juce::String &message)
{
    juce::String fullMessage = message;
    
    fullMessage += " (line " + juce::String(line_) + ").";
    sound_->addError(fullMessage);
}

