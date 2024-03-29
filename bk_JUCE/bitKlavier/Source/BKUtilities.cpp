/*
  ==============================================================================

    BKUtilities.cpp
    Created: 16 Nov 2016 11:12:15am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKUtilities.h"


String rectangleToString(Rectangle<int> rect)
{
    return ("X: " + String(rect.getX()) +
            " Y: " + String(rect.getY()) +
            " W: " + String(rect.getWidth()) +
            " H: " + String(rect.getHeight()));
}
String rectangleToString(Rectangle<float> rect)
{
    return ("X: " + String(rect.getX()) +
            " Y: " + String(rect.getY()) +
            " W: " + String(rect.getWidth()) +
            " H: " + String(rect.getHeight()));
}

BKParameterDataType getBKDataType ( SynchronicParameterType type)
{
    if ((type == SynchronicNumPulses) ||
        (type == SynchronicClusterMin) ||
        (type == SynchronicClusterMax) ||
        (type == SynchronicClusterThresh) ||
        (type == SynchronicMode) ||
        (type == SynchronicBeatsToSkip)) /*||
        (type == AT1Mode) ||
        (type == AT1History)) */
        return BKInt;
    
    if((type == SynchronicBeatMultipliers) ||
       (type == SynchronicLengthMultipliers) ||
       (type == SynchronicAccentMultipliers))
        return BKFloatArr;
    
    if (type == SynchronicTranspOffsets)
        return BKArrFloatArr;

    return BKParameterTypeNil;
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
    
    return BKParameterTypeNil;
}

/*
typedef enum TuningSystem {
    PartialTuning = 0,
    JustTuning,
    EqualTemperament,
    DuodeneTuning,
    OtonalTuning,
    UtonalTuning,
    AdaptiveTuning,
    AdaptiveAnchoredTuning,
    CustomTuning
} TuningSystem;
*/

TuningSystem tuningStringToTuningSystem(String tuning)
{
    if (tuning == "partial")                return PartialTuning;
    else if (tuning == "just")              return JustTuning;
    else if (tuning == "equal")             return EqualTemperament;
    else if (tuning == "duodene")           return DuodeneTuning;
    else if (tuning == "otonal")            return OtonalTuning;
    else if (tuning == "utonal")            return UtonalTuning;
    else if (tuning == "adaptive")          return AdaptiveTuning;
    else if (tuning == "adaptiveanchor")    return AdaptiveAnchoredTuning;
    else if (tuning == "custom")            return CustomTuning;
    else                                    return TuningSystemNil;

}

String pitch_classes[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B",};

String midiToPitchClass(int midi)
{
    int pc = midi % 12;
    int octave = (midi / 12) - 1;
    
    return String(pitch_classes[pc]) + String(octave);
}

PitchClass      letterNoteToPitchClass(String note)
{
    if (note == "C" || note == "c") return C;
    
    else if (note == "C#" || note == "c#" || note == "Db" || note == "db")   return CSharp;
    
    else if (note == "D" || note == "d") return D;
    
    else if (note == "D#" || note == "d#" || note == "Eb" || note == "eb")   return DSharp;
    
    else if (note == "E" || note == "e") return E;
    
    else if (note == "F" || note == "f") return F;
    
    else if (note == "F#" || note == "f#" || note == "Gb" || note == "gb")   return FSharp;
    
    else if (note == "G" || note == "g") return G;
    
    else if (note == "G#" || note == "g#" || note == "Ab" || note == "ab")   return GSharp;
    
    else if (note == "A" || note == "a" )    return A;
    
    else if (note == "A#" || note == "a#" || note == "Bb" || note == "bb")   return ASharp;
    
    else if (note == "B" || note == "b")     return B;
    
    
    return PitchClassNil;
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

String offsetArrayToString(Array<float> arr)
{
    String s = "";
    int key = 0;
    for (auto offset : arr)
    {
        //if (offset != 0.0)  s += String(key) + ":" + String((int)(offset*100.0f)) + " ";
        if (offset != 0.0)  s += String(key) + ":" + String((offset*100.0f)) + " ";
        
        ++key;
    }
    
    return s;
}

String offsetArrayToString2(Array<float> arr)
{
    String s = "";
    int key = 0;
    for (auto offset : arr)
    {
        //if (offset != 0.0)  s += String(key) + ":" + String((int)(offset*100.0f)) + " ";
        if (offset != 0.0)  s += String(key) + ":" + String((offset)) + " ";
        
        ++key;
    }
    
    return s;
}

String offsetArrayToString3(Array<float> arr, float mid)
{
    String s = "";
    int key = 0;
    for (auto offset : arr)
    {
        //if (offset != 0.0)  s += String(key) + ":" + String((int)(offset*100.0f)) + " ";
        DBG("offsetArrayToString3 val = " + String(offset));
        if (offset != mid)  s += String(key) + ":" + String((offset)) + " ";
        
        ++key;
    }
    
    return s;
}

String arrayIntArrayToString(Array<Array<int>> afarr)
{
    String s = "";
    for (auto arr : afarr)
    {
        if (arr.size()>1)
        {
            s += "[";
            for (auto i : arr)
            {
                s += String(i)+" ";
            }
            s += "] ";
        }
        else
        {
            s +=String(arr[0])+" ";
        }
    }
    return s;
}

String arrayFloatArrayToString(Array<Array<float>> afarr)
{
    String s = "";
    for (auto arr : afarr)
    {
        if (arr.size()>1)
        {
            s += "[";
            for (auto f : arr)
            {
                s += String(f)+" ";
            }
            s += "] ";
        }
        else
        {
            s +=String(arr[0])+" ";
        }
    }
    return s;
}

String arrayActiveFloatArrayToString(Array<Array<float>> afarr, Array<bool> act)
{
    String s = "";
    
    int boolCtr = 0;
    
    for (auto arr : afarr)
    {
        if (arr.size() > 1)
        {
            s += "[";
            for (auto f : arr)
            {
                s += String(f) + " ";
            }
            s += "] ";
        }
        else
        {
            if (act[boolCtr]) {
                s += String(arr[0]) + " ";
            }
            else
            {
                s +=  "/ ";
            }
        }
        
        boolCtr++;
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

Array<Array<float>> stringToArrayFloatArray(String s)
{
    Array<Array<float>> afarr;

    String rest = s;
    
    // "4"
    while (rest.length())
    {
        String sub = rest.upToFirstOccurrenceOf("[", false, true);
        
        Array<float> ind = stringToFloatArray(sub);
        
        for (auto f : ind)
        {
            Array<float> arr; arr.add(f);
            afarr.add(arr);
        }
        
        if (sub == rest) break; // no [ in s
        
        rest = rest.substring(sub.length()+1);
        
        sub = rest.upToFirstOccurrenceOf("]", false, true);
        
        if (sub != rest)
        {
            Array<float> group = stringToFloatArray(sub);
            
            afarr.add(group);
        }
        
        rest = rest.substring(sub.length()+1);
    }
 
    return afarr;
}

// reads through string: single whitespaces = true, slashes = false
// assume whitespaces follow values that we want, so all single whitespaces
// set to true, and otherwise set slashes to false
// need to ignore []
Array<bool> slashToFalse(String s)
{
    Array<bool> arr = Array<bool>();
    s.append(" ", 1); // to get the last value
    
    String::CharPointerType c = s.getCharPointer();
    juce_wchar slash = '/'; // blank: put a zero in
    juce_wchar leftbracket = '[';
    juce_wchar rightbracket = ']';
    
    bool precedingIsSpace = true;
    bool precedingIsSlash = false;
    bool inBracket = false;

    for (int i = 0; i < (s.length() + 1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        if (!CharacterFunctions::compare(c1, leftbracket))
            inBracket = true;
        
        if (!inBracket) {
            if (!CharacterFunctions::compare(c1, slash)) {
                arr.add(false);
                precedingIsSlash = true;
            }
            else if(CharacterFunctions::isWhitespace(c1)) {
                if (!precedingIsSlash && !precedingIsSpace) arr.add(true);
                precedingIsSpace = true;
            }
            else {
                precedingIsSpace = false;
                precedingIsSlash = false;
            }
        }
        else if (!CharacterFunctions::compare(c1, rightbracket))
        {
            inBracket = false;
            precedingIsSpace = false;
            precedingIsSlash = false;
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
    juce_wchar slash = '/'; // blank: put a zero in
    
    int prdCnt = 0;
    
    // DEBUG
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isPrd = !CharacterFunctions::compare(c1, prd);
        bool isDash = !CharacterFunctions::compare(c1, dash);
        bool isSlash = !CharacterFunctions::compare(c1, slash);
        
        if (isPrd) prdCnt += 1;
        
        bool isNumChar = CharacterFunctions::isDigit(c1) || isPrd || isDash;
        
        if (!isNumChar)
        {
            if (inNumber)
            {
                arr.add(temp.getFloatValue());
                temp = "";
            }
            
            // slash indicates a zero slot
            if (isSlash) {
                arr.add(0.);
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
Array<float> stringOrderedPairsToFloatArray(String s, int size, float init)
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
    // for(int i=0;i<size;i++) newarray.set(i, 0.);
    for(int i=0;i<size;i++) newarray.set(i, init);
    
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
        
        if(isSpace && previousSpace) //skip repeated spaces
        {
            previousSpace = true;
            inInt = false;
            inFloat = false;
        }
        else if(previousSpace && isEndOfString)
        {
            //do nothing; previousSpace already finalized array
        }
        else if(isSpace || isEndOfString)
        {
            previousSpace = true;
            inInt = false;
            inFloat = false;
            
            newval = tempFloat.getFloatValue();
            tempFloat = "";

            newarray.set(newindex, newval);
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

double mtof( double f, double sr )
{
    if( f <= -1500 ) return (0);
    else if( f > 1499 ) return (mtof(1499));
    // else return (8.17579891564 * exp(.0577622650 * f));
    // TODO: optimize
    else return ( pow(2, (f - 69) / 12.0) * sr );
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


double ftom( double f, double sr )
{
    // return (f > 0 ? 17.3123405046 * log(.12231220585 * f) : -1500);
    // TODO: optimize
    return (f > 0 ? (log(f / sr) / LOGTWO) * 12.0 + 69 : -1500);
}

// warping functions, for velocity curves and other parameters that
// might benefit from some signal conditioning

// these require inval to be between 0 and 1, and k != 1
// they warp the input asymmetrically, mostly to
// help focus one extreme or another of a slider range

// https://www.desmos.com/calculator/kfajuawsfs, to play with params/graphing


double dt_asymwarp(double inval, double k)
{
    if(k == 1) return inval;
    return (pow(k, inval) - 1.) / (k - 1.);
}

double dt_asymwarp_inverse(double inval, double k)
{
    if(k == 1) return inval;
    return log(inval * (k-1) + 1) / log(k);
}

// this one warps symmetrically around 0.5, keeping 0 and 1 fixed
double dt_symwarp(double inval, double k)
{
    if(k == 1) return inval;
    
    if (inval < 0.5)    return 1. - 0.5 * (pow(1. - 2. * inval, k) + 1);
    else                return      0.5 * (pow(2. * inval - 1., k) + 1);
}

// this one combines asymwarp, symwarp, and generally scaling and offset
// with both k values == 1, this is linear
// with scale = 1 and offset = 0, and k's == 1, this returns the input
double dt_warpscale(double inval, double asym_k, double sym_k, double scale, double offset)
{
    return offset + scale * dt_asymwarp(dt_symwarp(inval, sym_k), asym_k);
}


int mod(int a, int b) { return (a % b + b) % b; }

#if BK_UNIT_TESTS

#define	MAX_FREQ 20000.f

class UtilitiesTests : public UnitTest
{
public:
	UtilitiesTests() : UnitTest("Utilities", "Utilities") {}

	void runTest() override
	{
		beginTest("Utilities");
		
		DBG("====== testing freq to midi ======");

		//test ftom
		for (int i = 0; i < 10; i++)
		{
			double r = Random::getSystemRandom().nextDouble();
			double freq1 = r * MAX_FREQ;
			double freq2 = mtof(ftom(freq1));
			expect(abs(freq1 - freq2) < 0.005, String(freq1) + " and " + String(freq2) + " do not match");
		}

		DBG("====== testing midi to freq ======");

		//test mtof
		for (int i = 0; i < 10; i++)
		{
			double r = Random::getSystemRandom().nextDouble();
			double midi1 = r * 128.0;
			double midi2 = ftom(mtof(midi1));
			expect(abs(midi1 - midi2) < 0.005, String(midi1) + " and " + String(midi2) + " do not match");
		}
	}
};

static UtilitiesTests utilitiesTests;

#endif
