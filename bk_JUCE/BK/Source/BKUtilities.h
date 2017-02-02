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


typedef enum BKModificationType
{
    ModSynchronicTempo = 0,
    ModSynchronicNumPulses,
    ModSynchronicClusterMin,
    ModSynchronicClusterMax,
    ModSynchronicClusterThresh,
    ModSynchronicMode,
    ModSynchronicBeatsToSkip,
    ModSynchronicBeatMultipliers,
    ModSynchronicLengthMultipliers,
    ModSynchronicAccentMultipliers,
    ModSynchronicTranspOffsets,
    ModAT1Mode,
    ModAT1History,
    ModAT1Subdivisions,
    ModAT1Min,
    ModAT1Max,
    
    ModNostalgicWaveDistance,
    ModNostalgicUndertow,
    ModNostalgicTransposition,
    ModNostalgicGain,
    ModNostalgicLengthMultiplier,
    ModNostalgicBeatsToSkip,
    ModNostalgicMode,
    ModNostalgicSyncTarget,

    ModDirectTransposition,
    ModDirectGain,
    ModDirectResGain,
    ModDirectHammerGain,
    
    ModTuningScale,
    ModTuningFundamental,
    ModTuningOffset,
    ModTuningA1IntervalScale,
    ModTuningA1Inversional,
    ModTuningA1AnchorScale,
    ModTuningA1AnchorFundamental,
    ModTuningA1ClusterThresh,
    ModTuningA1History,
    ModTuningCustomScale,
} BKModificationType;


static const std::vector<int> bkModificationDataTypes = {

    BKFloat,    // ModSynchronicTempo
    BKInt,      // ModSynchronicNumPulses
    BKInt,      // ModSynchronicClusterMin
    BKInt,      // ModSynchronicClusterMax
    BKFloat,    // ModSynchronicClusterThresh
    BKInt,      // ModSynchronicMode
    BKFloat,    // ModSynchronicBeatsToSkip
    BKFloatArr, // ModSynchronicBeatMultipliers
    BKFloatArr, // ModSynchronicLengthMultipliers
    BKFloatArr, // ModSynchronicAccentMultipliers
    BKFloatArr, // ModSynchronicTranspOffsets
    BKInt,      // ModAT1Mode
    BKInt,      // ModAT1History
    BKInt,      // ModAT1Subdivisions
    BKFloat,    // ModAT1Min
    BKFloat,    // ModAT1Max
    
    BKFloat,    // ModNostalgicWaveDistance
    BKInt,      // ModNostalgicUndertow
    BKFloat,    // ModNostalgicTransposition
    BKFloat,    // ModNostalgicGain
    BKFloat,    // ModNostalgicLengthMultiplier
    BKFloat,    // ModNostalgicBeatsToSkip
    BKInt,      // ModNostalgicMode
    BKInt,      // ModNostalgicSyncTarget
    
    BKFloat,    // ModDirectTransposition
    BKFloat,    // ModDirectGain
    BKFloat,    // ModDirectResGain
    BKFloat,    // ModDirectHammerGain
    
    BKInt,      // ModTuningScale
    BKInt,      // ModTuningFundamental
    BKFloat,    // ModTuningOffset
    BKInt,      // ModTuningA1IntervalScale
    BKBool,     // ModTuningA1Inversional
    BKInt,      // ModTuningA1AnchorScale
    BKInt,      // ModTuningA1AnchorFundamental
    BKInt,      // ModTuningA1ClusterThresh
    BKInt,      // ModTuningA1History
    BKFloatArr  // ModTuningCustomScale
    
};



#endif  // BKUTILITIES_H_INCLUDED
