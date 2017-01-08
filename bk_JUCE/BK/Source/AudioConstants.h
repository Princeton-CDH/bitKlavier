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

#define USE_SYNTH_INTERNAL 0
#define CRAY_COOL_MUSIC_MAKER 0
#define CRAY_COOL_MUSIC_MAKER_2 0

typedef enum PitchClass {
    C = 0,
    CSharp,
    D,
    DSharp,
    E,
    F,
    FSharp,
    G,
    GSharp,
    A,
    ASharp,
    B,
    DFlat = CSharp,
    EFlat = DSharp,
    GFlat = FSharp,
    AFlat = GSharp,
    BFlat = ASharp,
} PitchClass;

#pragma mark - Synthesiser/Sampler
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
    Main,
    Hammer,
    Resonance,
    BKNoteTypeNil,
} BKNoteType;

typedef enum BKPreparationType {
    PreparationTypeDirect = 0,
    PreparationTypeSynchronic,
    PreparationTypeNostalgic,
    PreparationTypeTuning,
    BKPreparationTypeNil,
} BKPreparationType;

static const std::vector<std::string> cPreparationTypes = {
    "Main/Direct",
    "Synchronic",
    "Nostalgic",
    "Tuning"
};

typedef enum BKPianoCBType {
    PianoCBNumber,
    BKPianoCBTypeNil,
} BKPianoCBType;

static const std::vector<std::string> cPianoCBType = {
    "PianoNumber"
};

static const std::vector<std::string> cPianoNumberName= {
    "One",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eigth",
    "Nine",
    "Ten",
    "Eleven",
    "Twelve"
};





#pragma mark - General
typedef enum GeneralParameterType
{
    GeneralTuningFundamental,
    GeneralGlobalGain,
    GeneralSynchronicGain,
    GeneralNostalgicGain,
    GeneralDirectGain,
    GeneralResonanceGain,
    GeneralHammerGain,
    GeneralTempoMultiplier,
    GeneralResAndHammer,
    GeneralInvertSustain,
    GeneralParameterTypeNil
    
} GeneralParameterType;

static const std::vector<std::string> cGeneralParameterTypes = {
    "GeneralTuningFundamental",
    "GeneralGlobalGain",
    "GeneralSynchronicGain",
    "GeneralNostalgicGain",
    "GeneralDirectGain",
    "GeneralResonanceGain",
    "GeneralHammerGain",
    "GeneralTempoMultiplier",
    "GeneralResAndHammer",
    "GeneralInvertSustain"
    
};

#pragma mark - Synchronic
typedef enum AdaptiveTempo1Mode {
    TimeBetweenNotes,
    NoteLength,
    AdaptiveTempo1ModeNil
}AdaptiveTempo1Mode;

typedef enum SynchronicSyncMode {
    FirstNoteOnSync = 0,
    LastNoteOnSync,
    LastNoteOffSync,
    SynchronicSyncModeNil
} SynchronicSyncMode;

typedef enum SynchronicParameterType {
    SynchronicId = 0,
    SynchronicTuning,
    SynchronicTempo,
    SynchronicNumPulses,
    SynchronicClusterMin,
    SynchronicClusterMax,
    SynchronicClusterThresh,
    SynchronicMode,
    SynchronicBeatsToSkip,
    SynchronicBeatMultipliers,
    SynchronicLengthMultipliers,
    SynchronicAccentMultipliers,
    SynchronicTranspOffsets,
    AT1Mode,
    AT1History,
    AT1Subdivisions,
    AT1Min,
    AT1Max,
    SynchronicParameterTypeNil
} SynchronicParameterType;

static const std::vector<std::string> cSynchronicParameterTypes = {
    "Synchronic Id",
    "SynchronicTuning Id",
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
    "SynchronicTranspOffsets",
    "AdaptiveTempo1Mode",
    "AdaptiveTempo1History",
    "AdaptiveTempo1Subdivisions",
    "AdaptiveTempo1Min",
    "AdaptiveTempo1Max"
};

#pragma mark - Nostalgic
typedef enum NostalgicSyncMode {
    NoteLengthSync = 0, //reverse note length set by played note length
    SynchronicSync,     //reverse note length set by next synchronic pulse
    NostalgicSyncModeNil
} NostalgicSyncMode;

typedef enum NostalgicParameterType {
    NostalgicId = 0,
    NostalgicTuning,
    NostalgicWaveDistance,
    NostalgicUndertow,
    NostalgicTransposition,
    NostalgicGain,
    NostalgicLengthMultiplier,
    NostalgicBeatsToSkip,
    NostalgicMode,
    NostalgicSyncTarget,
    NostalgicParameterTypeNil
    
} NostalgicParameterType;

static const std::vector<std::string> cNostalgicParameterTypes = {
    "Nostalgic Id",
    "NostalgicTuning Id",
    "NostalgicWaveDistance",
    "NostalgicUndertow",
    "NostalgicTransposition",
    "NostalgicGain",
    "NostalgicLengthMultiplier",
    "NostalgicBeatsToSkip",
    "NostalgicMode",
    "NostalgicSyncTarget"
};

#pragma mark - Direct
typedef enum DirectParameterType
{
    DirectId = 0,
    DirectTuning,
    DirectTransposition,
    DirectGain,
    DirectResGain,
    DirectHammerGain,
    DirectParameterTypeNil,
    
} DirectParameterType;

static const std::vector<std::string> cDirectParameterTypes = {
    "Direct Id",
    "DirectTuning Id",
    "DirectTransposition",
    "DirectGain",
    "DirectResGain",
    "DirectHammerGain"
};

#pragma mark - Tuning
typedef enum TuningParameterType
{
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
    TuningCustomScale,
    TuningParameterTypeNil
    
} TuningParameterType;

static const std::vector<std::string> cTuningParameterTypes = {
    "Tuning Id",
    "TuningScale",
    "TuningFundamental",
    "TuningOffset",
    "TuningA1IntervalScale",
    "TuningA1Inversional",
    "TuningA1AnchorScale",
    "TuningA1AnchorFundamental",
    "TuningA1ClusterThresh",
    "TuningA1History",
    "TuningCustomScale"
};


#pragma mark - Piano
typedef enum PianoTFIndex
{
    PianoKeymapId = 0,
    PianoPreparationId,
    PianoParameterTypeNil
    
} PianoTFIndex;

static const std::vector<std::string> cPianoParameterTypes = {
    "PianoNumber",
    "PianoKeymap Id",
    "PianoPreparation Id"
};

#pragma mark - Keymap
typedef enum KeymapParameterType
{
    KeymapId = 0,
    KeymapField,
    KeymapParameterTypeNil
    
} KeymapParameterType;

static const std::vector<std::string> cKeymapParameterTypes = {
    "Keymap Id",
    "Keymap"
};


// Globals
static const int aMaxNumPianos = 12;
static const int aMaxNumPreparationsPerPiano = 12;
static const int aMaxTotalPreparations = aMaxNumPianos * aMaxNumPreparationsPerPiano;
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
    30,
    50,
    68,
    84,
    98,
    110,
    120,
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
