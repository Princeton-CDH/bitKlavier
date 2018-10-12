/*
  ==============================================================================

    SpringTuningUtilities.h
    Created: 10 Aug 2018 12:26:55pm
    Author:  airship

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#define INV_440 0.00227272727f

static const std::vector<std::string> notesInAnOctave = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    
};

static const std::vector<std::string> intervalLabels = {
    "unison",
    "minor second",
    "major second",
    "minor third",
    "major third",
    "perfect fourth",
    "dimished fifth",
    "perfect fifth",
    "minor sixth",
    "major sixth",
    "minor seventh",
    "major seventh",
    "octave"
};

constexpr static const double tunings[6][13] = {
    { 1, 16 / 15, 9 / 8, 6 / 5, 5 / 4, 4 / 3, 45 / 32, 3 / 2, 8 / 5, 5 / 3, 9 / 5, 15 / 8, 2 / 1}, // common just
    { 1, 1.0595, 1.1125, 1.1893, 1.2601, 1.3351, 1.4144, 1.4986, 1.5877, 1.6822, 1.7823, 1.8883, 2.0 }, // equal tempered
    { 1, 16 / 15, 9 / 8, 6 / 5, 5 / 4, 4 / 3, 1.41421356237, 3 / 2, 8 / 5, 5 / 3, 16 / 9, 15 / 8, 2 / 1}, // symmetric
    { 1, 17 / 16, 9 / 8, 19 / 16, 5 / 4, 21 / 16, 11 / 8, 3 / 2, 13 / 8, 27 / 16, 7 / 4, 15 / 8, 2 / 1}, // overtone
    { 1, 567 / 512, 9 / 8, 147 / 128, 21 / 16, 1323 / 1024, 189 / 128, 3 / 2, 49 / 32, 7 / 4, 441 / 256, 63 / 32, 2 / 1}, // well tuned piano
    { 1, 256 / 243, 9 / 8, 32 / 27, 81 / 64, 4 / 3, 729 / 512, 3 / 2, 128 / 81, 27 / 16, 16 / 9, 243 / 128, 2 / 1} }; //pythagorean

//interval lengths (not ratios) in cents, will need to add more tuning systems later
constexpr static const double centLengths[1][13] = {
	{ 0, 111.731, 203.910, 315.641, 386.314, 498.045, 590.224 , 701.995, 813.687, 884.359, 1017.597, 1088.269, 1200 } // common just
};

class Utilities
{
public:

    static double   softClip(double val, double thresh)
    {
        double x;
        
        if(val > thresh)
        {
            x = thresh / val;
            return (1.0 - x) * (1.0 - thresh) + thresh;
        }
        else if(val < -thresh)
        {
            x = -thresh / val;
            return -((1.0 - x) * (1.0 - thresh) + thresh);
        }
        else
        {
            return val;
        }
    }
    
    static double ftom(double f)
    {
        if (f < 0.0) f = 0.0;
        return (69.0f + 12.0f * log2(f * INV_440));
    }
    
    static float ftom(float f)
    {
        if (f < 0.0) f = 0.0;
        return (69.0f + 12.0f * log2(f * INV_440));
    }

	static double map(double value, double valueMin, double valueMax, double mapMin, double mapMax)
	{
		return ((value - valueMin) / (valueMax - valueMin)) * (mapMax - mapMin) + mapMin;
	}
    
	static double noteToFreq(int noteIndex)
	{
		//int midiIndex = 60 + noteIndex; may need to do with multiple octaves
		return (double)(cFreq * pow(2.0, noteIndex / 12.0)); // will need to change when multiple octaves are added
	}

	//need to changec
	static double posToFreq(double position)
	{
		double noteValue = 60.0 + position;
		return (double)(cFreq * pow(2.0, noteValue / 12.0)) / 32; // will need to change when multiple octaves are added
	}

	static int noteToCents(int noteIndex)
	{
		return (int)(freqToCents(noteToFreq(noteIndex)));
	}
	static int freqToCents(double whichFreq)
	{
		return (int)(ratioToCents(whichFreq / cFreq));
	}
	static double centsToFreq(int centsFromC)
	{
		return cFreq * pow(2, centsFromC / 1200.0);
	}

	//will eventually need to do centsToPos
	
	//same with noteToPos
	
	//same with posToCents

	static int ratioToCents(double ratio)
	{
		return 1200 * log(ratio) / log(2);
	}
	static double halfStepUp(double freq)
	{
		return freq * halfStepRatio;
	}
	static double halfStepDown(double freq)
	{
		return freq / halfStepRatio;
	}

    static double   clip(double min, double val, double max)
    {
        
        if (val < min) {
            return min;
        } else if (val > max) {
            return max;
        } else {
            return val;
        }
    }

    
    static float   clip(float min, float val, float max)
    {
        
        if (val < min) {
            return min;
        } else if (val > max) {
            return max;
        } else {
            return val;
        }
    }
    
    constexpr static const float pi = 3.14159265359f;
    constexpr static const float twopi = 6.28318530718f;
	constexpr static const double cFreq = 261.6255653;
	constexpr static const double halfStepRatio = 1.0595;
	constexpr static const int octaves[1] = { 4 }; //will return to when adding more octaves
    
private:
    Utilities () {};
};

