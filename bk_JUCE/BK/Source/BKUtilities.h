/*
  ==============================================================================

    BKUtilities.h
    Created: 16 Nov 2016 11:12:15am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKUTILITIES_H_INCLUDED
#define BKUTILITIES_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "BKReferenceCountedBuffer.h"

#include "GraphicsConstants.h"

#include "AudioConstants.h"

String          intArrayToString(Array<int> arr);

String          floatArrayToString(Array<float> arr);

Array<int>      stringToIntArray(String s);

Array<int>      keymapStringToIntArray(String s);

Array<float>    stringToFloatArray(String s);

double          mtof(double f);

double          ftom(double f);

inline int layerToLayerId(BKNoteType type, int layer) { return (50*type)+layer;}

typedef enum BKParameterType
{
    BKVoid = 0,
    BKBool,
    BKInt,
    BKFloat,
    BKIntArr,
    BKFloatArr,
    BKParameterTypeNil
} BKParameterType;

typedef enum BKTextFieldType
{
    BKParameter = 0,
    BKModification,
    BKTFNil,
};



static const std::vector<int> bkificationDataTypes = {

    BKFloat,    // SynchronicTempo
    BKInt,      // SynchronicNumPulses
    BKInt,      // SynchronicClusterMin
    BKInt,      // SynchronicClusterMax
    BKFloat,    // SynchronicClusterThresh
    BKInt,      // Synchronice
    BKFloat,    // SynchronicBeatsToSkip
    BKFloatArr, // SynchronicBeatMultipliers
    BKFloatArr, // SynchronicLengthMultipliers
    BKFloatArr, // SynchronicAccentMultipliers
    BKFloatArr, // SynchronicTranspOffsets
    BKInt,      // AT1e
    BKInt,      // AT1History
    BKInt,      // AT1Subdivisions
    BKFloat,    // AT1Min
    BKFloat,    // AT1Max
    
    BKFloat,    // NostalgicWaveDistance
    BKInt,      // NostalgicUndertow
    BKFloat,    // NostalgicTransposition
    BKFloat,    // NostalgicGain
    BKFloat,    // NostalgicLengthMultiplier
    BKFloat,    // NostalgicBeatsToSkip
    BKInt,      // Nostalgice
    BKInt,      // NostalgicSyncTarget
    
    BKFloat,    // DirectTransposition
    BKFloat,    // DirectGain
    BKFloat,    // DirectResGain
    BKFloat,    // DirectHammerGain
    
    BKInt,      // TuningScale
    BKInt,      // TuningFundamental
    BKFloat,    // TuningOffset
    BKInt,      // TuningA1IntervalScale
    BKBool,     // TuningA1Inversional
    BKInt,      // TuningA1AnchorScale
    BKInt,      // TuningA1AnchorFundamental
    BKInt,      // TuningA1ClusterThresh
    BKInt,      // TuningA1History
    BKFloatArr  // TuningCustomScale
    
};



#endif  // BKUTILITIES_H_INCLUDED
