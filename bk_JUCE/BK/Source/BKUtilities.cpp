/*
  ==============================================================================

    BKUtilities.cpp
    Created: 16 Nov 2016 11:12:15am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKUtilities.h"

BKParameterDataType getBKDataType ( SynchronicParameterType type)
{
    if ((type == SynchronicTuning) ||
        (type == SynchronicNumPulses) ||
        (type == SynchronicClusterMin) ||
        (type == SynchronicClusterMax) ||
        (type == SynchronicClusterThresh) ||
        (type == SynchronicMode) ||
        (type == SynchronicBeatsToSkip) ||
        (type == AT1Mode) ||
        (type == AT1History))
        return BKInt;
        
    
    if ((type == SynchronicTempo) ||
        (type == AT1Subdivisions) ||
        (type == AT1Min) ||
        (type == AT1Max))
        return BKFloat;
    
    if((type == SynchronicBeatMultipliers) ||
       (type == SynchronicLengthMultipliers) ||
       (type == SynchronicAccentMultipliers) ||
       (type == SynchronicTranspOffsets))
        return BKFloatArr;
}
/*
TuningId = 0,
TuningScale,
TuningFundamental,
TuningOffset,
TuningA1IntervalScale,
TuningA1Inversional,
TuningA1AnchorScale,
TuningA1AnchorFundamental,
TuningA1ClusterThresh,
TuningA1History,
TuningCustomScale
*/
BKParameterDataType getBKDataType ( TuningParameterType type)
{
    if ((type == TuningId) ||
        (type == TuningScale) ||
        (type == TuningFundamental) ||
        (type == TuningA1IntervalScale) ||
        (type == TuningA1AnchorScale) ||
        (type == TuningA1AnchorFundamental) ||
        (type == TuningA1ClusterThresh) ||
        (type == TuningA1History))
        return BKInt;
    
    
    if (type == TuningOffset)
        return BKFloat;
    
    if (type == TuningCustomScale)
        return BKFloatArr;
    
    if (type == TuningA1Inversional)
        return BKBool;
}

/*
BKParameterDataType getBKDataType ( DirectParameterType param)
{
    if ((type == SynchronicTuning) ||
        (type == SynchronicNumPulses) ||
        (type == SynchronicClusterMin) ||
        (type == SynchronicClusterMax) ||
        (type == SynchronicClusterThresh) ||
        (type == SynchronicMode) ||
        (type == SynchronicBeatsToSkip) ||
        (type == AT1Mode) ||
        (type == AT1History))
        return BKInt;
    
    
    if ((type == SynchronicTempo) ||
        (type == AT1Subdivisions) ||
        (type == AT1Min) ||
        (type == AT1Max))
        return BKFloat;
    
    if((type == SynchronicBeatMultipliers) ||
       (type == SynchronicLengthMultipliers) ||
       (type == SynchronicAccentMultipliers) ||
       (type == SynchronicTranspOffsets))
        return BKFloatArr;
}
 */

/*
BKParameterDataType getBKDataType ( NostalgicParameterType param)
{
    if ((type == SynchronicTuning) ||
        (type == SynchronicNumPulses) ||
        (type == SynchronicClusterMin) ||
        (type == SynchronicClusterMax) ||
        (type == SynchronicClusterThresh) ||
        (type == SynchronicMode) ||
        (type == SynchronicBeatsToSkip) ||
        (type == AT1Mode) ||
        (type == AT1History))
        return BKInt;
    
    
    if ((type == SynchronicTempo) ||
        (type == AT1Subdivisions) ||
        (type == AT1Min) ||
        (type == AT1Max))
        return BKFloat;
}
*/

String boolArrayToString(Array<bool> arr)
{
    String s = "";
    for (auto key : arr)
    {
        if (key)
        {
            s.append(String(1), 1);
            s.append(" ", 1);
        }
        else
        {
            s.append(String(0), 1);
            s.append(" ", 1);
        }
    }
    return s;
}

String intArrayToString(Array<int> arr)
{
    String s = "";
    for (auto key : arr)
    {
        s.append(String(key), 6);
        s.append(" ", 1);
    }
    return s;
}

String floatArrayToString(Array<float> arr)
{
    String s = "";
    for (auto key : arr)
    {
        s.append(String(key), 6);
        s.append(" ", 1);
    }
    return s;
}

Array<int> keymapStringToIntArray(String s)
{
    Array<int> arr = Array<int>();
    
    String temp = "";
    
    bool inNumber = false;
    bool inVector = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar dash = '-';
    juce_wchar arrow = '>';
    
    int last;
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isArrow = !CharacterFunctions::compare(c1, arrow);
        bool isDash = !CharacterFunctions::compare(c1, dash);
        
        bool isNumChar = CharacterFunctions::isDigit(c1) || isDash;
        
        if (!isNumChar)
        {
            if (inNumber)
            {
                if (isArrow)
                {
                    inVector = true;
                    last  = temp.getIntValue();
                }
                else if (inVector)
                {
                    inVector = false;
                    int curr = temp.getIntValue();
                    if ((last <= curr) && (curr < 128))
                    {
                        for (int n = last; n <= temp.getIntValue(); n++) arr.add(n);
                    }
                }
                else
                {
                    inVector = false;
                    arr.add(temp.getIntValue());
                    
                }
                
                temp = "";
            }
            
            inNumber = false;
            continue;
        }
        else
        {
            inNumber = true;
            
            temp += c1;
        }
        
    }
    
    return arr;
}




Array<int> stringToIntArray(String s)
{
    Array<int> arr = Array<int>();
    
    String temp = "";
    
    bool inNumber = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar dash = '-';
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isDash = !CharacterFunctions::compare(c1, dash);
        
        bool isNumChar = CharacterFunctions::isDigit(c1) || isDash;
        
        if (!isNumChar)
        {
            if (inNumber)
            {
                arr.add(temp.getIntValue());
                temp = "";
            }
            
            inNumber = false;
            continue;
        }
        else
        {
            inNumber = true;
            
            temp += c1;
        }
        
    }
    
    return arr;
}

Array<float> stringToFloatArray(String s)
{
    Array<float> arr = Array<float>();
    
    String temp = "";
    bool inNumber = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar prd = '.';
    juce_wchar dash = '-';
    
    int prdCnt = 0;
    
    // DEBUG
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isPrd = !CharacterFunctions::compare(c1, prd);
        bool isDash = !CharacterFunctions::compare(c1, dash);
        
        if (isPrd) prdCnt += 1;
        
        bool isNumChar = CharacterFunctions::isDigit(c1) || isPrd || isDash;
        
        if (!isNumChar)
        {
            if (inNumber)
            {
                arr.add(temp.getFloatValue());
                temp = "";
            }
            
            inNumber = false;
            continue;
        }
        else
        {
            inNumber = true;
            
            temp += c1;
        }
    }
    
    return arr;
}


//takes string of ordered pairs in the form x1:y1 x2:y2
//and converts into Array of floats of y, with indices x
Array<float> stringOrderedPairsToFloatArray(String s, int size)
{
    
    String tempInt = "";
    String tempFloat = "";
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar colon = ':';
    juce_wchar dash = '-';
    juce_wchar prd = '.';

    bool isNumber = false;
    bool isColon  = false;
    bool isSpace = false;
    bool isDash = false;
    bool isPeriod = false;

    bool inInt = false;
    bool inFloat = false;
    
    bool previousColon = false;
    bool previousSpace = true;
    
    bool isEndOfString = false;
    
    int newindex = 0;
    float newval = 0.;
    
    Array<float> newarray;
    newarray.ensureStorageAllocated(size);
    for(int i=0;i<size;i++) newarray.set(i, 0.);
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        isColon     = !CharacterFunctions::compare(c1, colon);
        isNumber    = CharacterFunctions::isDigit(c1);
        isSpace     = CharacterFunctions::isWhitespace(c1);
        isDash      = !CharacterFunctions::compare(c1, dash);
        isPeriod    = !CharacterFunctions::compare(c1, prd);
        if (i==s.length()) isEndOfString = true;
        
        //numbers
        if(isNumber && previousSpace) //beginning index read
        {
            inInt = true;
            tempInt += c1;
        }
        else if(isNumber && inInt) //still reading index
        {
            tempInt += c1;
        }
        else if( (isNumber && previousColon) || isDash || isPeriod ) //beginning val read
        {
            inFloat = true;
            tempFloat += c1;
        }
        else if(isNumber && inFloat) //still reading float val
        {
            tempFloat += c1;
        }
        
        //colons and spaces
        if(isColon)
        {
            previousColon = true;
            inInt = false;
            inFloat = false;
            
            newindex = tempInt.getIntValue();
            tempInt = "";
        }
        else previousColon = false;
        
        if(isSpace || isEndOfString)
        {
            previousSpace = true;
            inInt = false;
            inFloat = false;
            
            newval = tempFloat.getFloatValue();
            tempFloat = "";
            
            //DBG("new array index and value " + String(newindex) + " " + String(newval));
            
            newarray.insert(newindex, newval);
        }
        else previousSpace = false;
        
    }
    
    return newarray;
}

// the following 2 functions are
// lifted from  PD source
// specifically x_acoustics.c
// http://puredata.info/downloads
#define LOGTWO 0.69314718055994528623
#define LOGTEN 2.302585092994

//-----------------------------------------------------------------------------
// name: mtof()
// desc: midi to freq
//-----------------------------------------------------------------------------
double mtof( double f )
{
    if( f <= -1500 ) return (0);
    else if( f > 1499 ) return (mtof(1499));
    // else return (8.17579891564 * exp(.0577622650 * f));
    // TODO: optimize
    else return ( pow(2, (f - 69) / 12.0) * 440.0 );
}


//-----------------------------------------------------------------------------
// name: ftom()
// desc: freq to midi
//-----------------------------------------------------------------------------
double ftom( double f )
{
    // return (f > 0 ? 17.3123405046 * log(.12231220585 * f) : -1500);
    // TODO: optimize
    return (f > 0 ? (log(f / 440.0) / LOGTWO) * 12.0 + 69 : -1500);
}

