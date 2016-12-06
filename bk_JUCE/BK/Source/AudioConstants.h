/*
  ==============================================================================

    AudioConstants.h
    Created: 13 Oct 2016 12:16:10am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef AUDIOCONSTANTS_H_INCLUDED
#define AUDIOCONSTANTS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#define TEXT_CHANGE_INTERNAL 0

#define OFFSETS 1

#define USE_SYNTH_INTERNAL 0
#define CRAY_COOL_MUSIC_MAKER 0
#define CRAY_COOL_MUSIC_MAKER_2 0

typedef enum PianoSamplerNoteType {
    Normal = 0,
    NormalFixedStart,
    FixedLength,
    FixedLengthFixedStart,
    PianoSamplerNoteTypeNil
} PianoSamplerNoteType;

typedef enum PianoSamplerNoteDirection {
    Forward = 0,
    Reverse,
    PianoSamplerPlaybackDirectionNil
} PianoSamplerNoteDirection;

typedef enum BKNoteType {
    Synchronic = 0,
    Nostalgic,
    Direct,
    BKNoteTypeNil,
} BKNoteType;

// Synchronic
typedef enum SynchronicSyncMode {
    FirstNoteSync = 0,
    LastNoteSync,
    SynchronicSyncModeNil
} SynchronicSyncMode;

// Adding text field to SynchronicViewController is as easy as adding typedef and string to these two data types.
typedef enum SynchronicParameterType {
    SynchronicTempo = 0,
    SynchronicNumPulses,
    SynchronicClusterMin,
    SynchronicClusterMax,
    SynchronicClusterThresh,
    SynchronicMode,
    SynchronicBeatsToSkip,
    SynchronicBeatMultipliers,
    SynchronicLengthMultipliers,
    SynchronicAccentMultipliers,
#if OFFSETS
    SynchronicTranspOffsets,
#endif
    SynchronicTuning,
    SynchronicBasePitch,
    SynchronicParameterTypeNil
} SynchronicParameterType;

static const std::vector<std::string> cSynchronicParameterTypes = {
    "SynchronicTempo",
    "SynchronicNumPulses",
    "SynchronicClusterMin",
    "SynchronicClusterMax",
    "SynchronicClusterThresh",
    "SynchronicMode",
    "SynchronicBeatsToSkip",
    "SynchronicBeatMultipliers",
    "SynchronicLengthMultipliers",
    "SynchronicAccentMultipliers",
#if OFFSETS
    "SynchronicTranspOffsets",
#endif
    "SynchronicTuning",
    "SynchronicBasePitch"
};

// Nostalgic
typedef enum NostalgicSyncMode {
    NoteLengthSync = 0, //reverse note length set by played note length
    SynchronicSync,     //reverse note length set by next synchronic pulse
    NostalgicSyncModeNil
} NostalgicSyncMode;

// Adding text field to NostalgicViewController is as easy as adding typedef and string to these two data types.
typedef enum NostalgicParameterType {
    NostalgicWaveDistance = 0,
    NostalgicUndertow,
    NostalgicTransposition,
    NostalgicGain,
    NostalgicLengthMultiplier,
    NostalgicBeatsToSkip,
    NostalgicMode,
    NostalgicSyncTarget,
    NostalgicTuning,
    NostalgicBasePitch,
    NostalgicParameterTypeNil
} NostalgicParameterType;

static const std::vector<std::string> cNostalgicParameterTypes = {
    "NostalgicWaveDistance",
    "NostalgicUndertow",
    "NostalgicTransposition",
    "NostalgicGain",
    "NostalgicLengthMultiplier",
    "NostalgicBeatsToSkip",
    "NostalgicMode",
    "NostalgicSyncTarget",
    "NostalgicTuning",
    "NostalgicBasePitch",
};

// Globals
static const int aMaxNumLayers = 12;
static const float aGlobalGain = 0.5; //make this user settable

static const float aMaxSampleLengthSec = 30.0f;
static const float aRampOnTimeSec = 0.004f;
static const float aRampOffTimeSec = 0.004f;
static const int aNumScaleDegrees = 12;
static const int aRampUndertowCrossMS = 50;

// Sample layers
static const int aNumSampleLayers = 2;
static const int aVelocityThresh_Eight[9] = {
    0,
    25,
    42,
    60,
    76,
    90,
    104,
    116,
    128
};

static const int aVelocityThresh_Four[5] = {
    0,
    42,
    76,
    104,
    128
};

static const int aVelocityThresh_Two[3] = {
    0,
    76,
    128
};

static const int aResonanceVelocityThresh[4] = {
    0,
    40,
    85,
    128
};


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
} Tuning;

#endif  // AUDIOCONSTANTS_H_INCLUDED
