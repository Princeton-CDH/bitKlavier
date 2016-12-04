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
    SynchronicTuningOffsets,
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
    "SynchronicTuningOffsets",
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




// Tunings
// Just: 1/1, 16/15, 9/8, 6/5, 5/4, 4/3,  7/5, 3/2, 8/5, 5/3, 7/4,  15/8
static const float aJustTuning[aNumScaleDegrees] = {0., .117313, .039101,  .156414, -.13686, -.019547, -.174873, .019547, .136864, -.15641, -.311745, -.11731};


// Partial: 1/1, 16/15, 9/8, 7/6, 5/4, 4/3, 11/8, 3/2, 13/8, 5/3, 7/4, 11/6
/*
 Partial tuning has: flat m3rd, flat TT, sharp m6th, flat M7th; relative to just
m6th is tuned like 13th partial, and TT is tuned like 11th partial
inspired by tuning of the 6ths in the Bruremarsj frå Engerdal, Sven Nyhus
the TT tuning is the symmetrical pair around the perfect 5th (11/8 <=> 13/8)
*/
static const float aPartialTuning[aNumScaleDegrees] = {0., .117313, .039101, -.331291, -.13686, -.019547, -.486824, .019547, .405273, -.15641, -.311745, -.506371};

// Equal temperament:
static const float aEqualTuning[aNumScaleDegrees] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

// Other tunings, from Partch and Jeff Snyder

// Duodene: 1/1, 16/15, 9/8, 6/5, 5/4, 4/3, 45/32, 3/2, 8/5, 5/3, 16/9, 15/8
static const float aDuodeneTuning[aNumScaleDegrees] = {0.0,  .117313,  .039101,  .156414, -.13686, -.019547, -.097763,  .019547,  .136864, -.15641, -.039101,  -.11731};

// Otonal: 1/1, 17/16, 9/8, 19/16, 5/4, 21/16, 11/8, 3/2, 13/8, 27/16, 7/4, 15/8
static const float aOtonalTuning[aNumScaleDegrees] = {0.0,  .049553,  .039101, -.02872,  -.13686,  -.292191, -.486824,  .019547,  .405273,  .058647, -.311745,  -.11731};

// Utonal: 1/1, 16/15, 8/7, 32/27, 16/13, 4/3, 16/11, 32/21, 8/5, 32/19, 16/9, 32/17
static const float aUtonalTuning[aNumScaleDegrees] = {0.0,  .117313,  .311745,  .156414, -.405273, -.019547,  .486824,  .292191,  .136864,  .024847, -.039101,  -.049553};

typedef enum Tuning {
    PartialTuning = 0,
    JustTuning,
    EqualTemperament,
    AdaptiveTuning,
    AdaptiveAnchoredTuning,
    DuodeneTuning,
    OtonalTuning,
    UtonalTuning,
    CustomTuning,
    TuningNil
} Tuning;

#endif  // AUDIOCONSTANTS_H_INCLUDED
