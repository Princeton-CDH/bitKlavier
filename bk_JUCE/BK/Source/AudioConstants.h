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

const String jsonSynchronicX = "synchronic::";
const String jsonDirectX = "direct::";
const String jsonNostalgicX = "nostalgic::";

const String jsonSynchronicLayerX = "synchronic::synchronicSub.";
const String jsonDirectLayerX = "direct::directSub.";
const String jsonNostalgicLayerX = "nostalgic::nostalgicSub.";

const String vtagGallery = "gallery";
const String vtagGeneral = "general";
const String ptagGeneral_globalGain = "globalGain";
const String ptagGeneral_directGain = "directGain";
const String ptagGeneral_synchronicGain = "synchronicGain";
const String ptagGeneral_nostalgicGain = "nostalgicGain";
const String ptagGeneral_resonanceGain = "resonanceGain";
const String ptagGeneral_hammerGain = "hammerGain";
const String ptagGeneral_tempoMultiplier = "tempoMultiplier";
const String ptagGeneral_resAndHammer = "resAndHammer";
const String ptagGeneral_invertSustain = "invertSustain";
const String ptagGeneral_tuningFund = "tuningFund";

const String vtagPiano = "piano";
const String vtagPrepMap = "prepMap";
const String ptagPrepMap_keymapId = "keymapId";

const String vtagPrepMap_directPreps = "directPrepIds";
const String vtagPrepMap_synchronicPreps = "synchronicPrepIds";
const String vtagPrepMap_nostalgicPreps = "nostalgicPrepIds";
const String ptagPrepMap_directPrepId = "d";
const String ptagPrepMap_synchronicPrepId = "s";
const String ptagPrepMap_nostalgicPrepId = "n";

const String vtagPianoMap = "pianoMap";
const String ptagPianoMap_key = "key";
const String ptagPianoMap_piano = "piano";

const String vtagModDirect = "directMod";
const String vtagModSynchronic = "synchronicMod";
const String vtagModNostalgic = "nostalgicMod";
const String vtagModTuning = "tuningMod";
const String ptagModX_key = "key";
const String ptagModX_modPrep = "modPrep";
const String ptagModX_type = "type";
const String ptagModX_prep = "prep";

const String ptagFloat = "f";
const String ptagInt = "i";
const String ptagBool = "b";
const String ptagFloatArr = "fa";
const String ptagIntArr = "ia";


const String vtagSynchronicPrep = "synchronicPrep";
const String vtagSynchronicModPrep = "synchronicModPrep";
const String ptagSynchronic_Id = "Id";
const String ptagSynchronic_tuning = "tuning";
const String ptagSynchronic_tempo = "tempo";
const String ptagSynchronic_numBeats = "numBeats";
const String ptagSynchronic_clusterMin = "clusterMin";
const String ptagSynchronic_clusterMax = "clusterMax";
const String ptagSynchronic_clusterThresh = "clusterThresh";
const String ptagSynchronic_mode = "mode";
const String ptagSynchronic_beatsToSkip = "beatsToSkip";
const String ptagSynchronic_at1Mode = "AT1Mode";
const String ptagSynchronic_at1History = "AT1History";
const String ptagSynchronic_at1Subdivisions = "AT1Subdivisions";
const String ptagSynchronic_AT1Min = "AT1Min";
const String ptagSynchronic_AT1Max = "AT1Max";
const String vtagSynchronic_beatMults = "beatMultipliers";
const String vtagSynchronic_lengthMults = "lengthMultipliers";
const String vtagSynchronic_accentMults = "accentMultipliers";
const String vtagSynchronic_transpOffsets = "transpOffsets";
const String ptagSynchronic_reset = "synchronicReset";


const String vtagNostalgicPrep = "nostalgicPrep";
const String vtagNostalgicModPrep = "nostalgicModPrep";
const String ptagNostalgic_Id = "Id";
const String ptagNostalgic_tuning = "tuning";
const String ptagNostalgic_waveDistance = "waveDistance";
const String ptagNostalgic_undertow = "undertow";
const String vtagNostalgic_transposition = "transposition";
const String ptagNostalgic_gain = "gain";
const String ptagNostalgic_lengthMultiplier = "lengthMultiplier";
const String ptagNostalgic_mode = "mode";
const String ptagNostalgic_beatsToSkip = "beatsToSkip";
const String ptagNostalgic_syncTarget = "syncTarget";
const String ptagNostalgic_reset = "nostalgicReset";



const String vtagDirectPrep = "directPrep";
const String vtagDirectModPrep = "directModPrep";
const String ptagDirect_id ="Id";
const String ptagDirect_tuning = "tuning";
const String vtagDirect_transposition = "transposition";
const String ptagDirect_gain = "gain";
const String ptagDirect_resGain = "resGain";
const String ptagDirect_hammerGain = "hammerGain";
const String ptagDirect_reset = "directReset";


const String vtagTuningModPrep = "tuningModPrep";
const String vtagTuningPrep = "tuningPrep";
const String ptagTuning_Id = "Id";
const String ptagTuning_scale = "scale";
const String ptagTuning_fundamental = "fundamental";
const String ptagTuning_offset = "offset";
const String ptagTuning_adaptiveIntervalScale = "adaptiveIntervalScale";
const String ptagTuning_adaptiveInversional = "adaptiveInversional";
const String ptagTuning_adaptiveAnchorScale = "adaptiveAnchorScale";
const String ptagTuning_adaptiveAnchorFund = "adaptiveAnchorFund";
const String ptagTuning_adaptiveClusterThresh = "adaptiveClusterThresh";
const String ptagTuning_adaptiveHistory = "adaptiveHistory";

const String vtagTuning_customScale = "customScale";
const String ptagTuning_customScaleLength = "customScaleLength";
const String ptagTuning_customScaleDegree = "scaleDegree";
const String vTagTuning_absoluteOffsets = "absoluteOffsets";
const String ptagTuning_resetPrep = "tuningReset";


const String vtagKeymaps = "keymaps";
const String vtagKeymap =  "keymap";
const String ptagKeymap_key = "k";

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
    SynchronicNote = 0,
    NostalgicNote,
    DirectNote,
    MainNote,
    HammerNote,
    ResonanceNote,
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






static const std::vector<std::string> cPianoName = {
    "Piano1",
    "Piano2",
    "Piano3",
    "Piano4",
    "Piano5",
    "Piano6",
    "Piano7",
    "Piano8",
    "Piano9",
    "Piano10",
    "Piano11",
    "Piano12",
    "Piano13",
    "Piano14",
    "Piano15",
    "Piano16",
    "Piano17",
    "Piano18",
    "Piano19",
    "Piano20",
    "Piano21",
    "Piano22",
    "Piano23",
    "Piano24",
    "Piano25",
    "Piano26",
    "Piano27",
    "Piano28",
    "Piano29",
    "Piano30",
    "Piano31",
    "Piano32",
    "Piano33",
    "Piano34",
    "Piano35"
    
};

static const int aMaxNumPianos = cPianoName.size();


static const std::vector<std::string> cPrepMapName= {
    "PrepMap1",
    "PrepMap2",
    "PrepMap3",
    "PrepMap4",
    "PrepMap5",
    "PrepMap6",
    "PrepMap7",
    "PrepMap8",
    "PrepMap9",
    "PrepMap10",
    "PrepMap11",
    "PrepMap12"
};

static const int aMaxNumPreparationKeymaps = cPrepMapName.size();

typedef enum BKParameterDataType
{
    BKVoid = 0,
    BKBool,
    BKInt,
    BKFloat,
    BKIntArr,
    BKFloatArr,
    BKArrFloatArr,
    BKParameterTypeNil
} BKParameterDataType;

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
    "GeneralTuningFund",
    "GeneralGlobalGain",
    "GeneralSyncGain",
    "GeneralNostGain",
    "GeneralDrctGain",
    "GeneralResGain",
    "GeneralHamGain",
    "GeneralTempoMult",
    "GeneralResAndHam",
    "GeneralInvertSus"
    
};

#pragma mark - Synchronic
typedef enum AdaptiveTempo1Mode {
    TimeBetweenNotes,
    NoteLength,
    AdaptiveTempo1ModeNil
}AdaptiveTempo1Mode;

typedef enum SynchronicSyncMode {
    FirstNoteOnSync = 0,
    AnyNoteOnSync,
    LastNoteOffSync,
    AnyNoteOffSync,
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
    SynchronicReset,
    SynchronicParameterTypeNil
} SynchronicParameterType;

static const std::vector<BKParameterDataType> cSynchronicDataTypes = {
    BKInt,
    BKInt,
    BKFloat,
    BKInt,
    BKInt,
    BKInt,
    BKFloat,
    BKInt,
    BKInt,
    BKFloatArr,
    BKFloatArr,
    BKFloatArr,
    BKArrFloatArr,
    BKFloat,
    BKInt,
    BKFloat,
    BKFloat,
    BKFloat,
    BKInt
};

static const std::vector<std::string> cSynchronicParameterTypes = {
    "Synchronic Id",
    "Tuning Id",
    "Tempo",
    "NumPulses",
    "ClusterMin",
    "ClusterMax",
    "ClusterThresh",
    "Mode",
    "BeatsToSkip",
    "BeatMults",
    "LengthMults",
    "AccentMults",
    "TranspOffsets",
    "AT1Mode",
    "AT1History",
    "AT1Subdivs",
    "AT1Min",
    "AT1Max",
    "ResetPrep"
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
    NostalgicReset,
    NostalgicParameterTypeNil
    
} NostalgicParameterType;

static const std::vector<BKParameterDataType> cNostalgicDataTypes =
{
    BKInt,
    BKInt,
    BKInt,
    BKInt,
    BKFloatArr,
    BKFloat,
    BKFloat,
    BKInt,
    BKInt,
    BKInt,
    BKInt
};

static const std::vector<std::string> cNostalgicParameterTypes = {
    "Nostalgic Id",
    "Tuning Id",
    "WaveDistance",
    "Undertow",
    "Transposition",
    "Gain",
    "LengthMult",
    "BeatsToSkip",
    "Length Mode",
    "SyncTarget",
    "ResetPrep"
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
    DirectReset,
    DirectParameterTypeNil,
    
} DirectParameterType;


static const std::vector<BKParameterDataType> cDirectDataTypes = {
    BKInt,
    BKInt,
    BKFloatArr,
    BKFloat,
    BKFloat,
    BKFloat,
    BKInt
};

static const std::vector<std::string> cDirectParameterTypes = {
    "Direct Id",
    "Tuning Id",
    "Transposition",
    "Gain",
    "ResGain",
    "HammerGain",
    "ResetPrep"
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
    TuningAbsoluteOffsets,
    TuningReset,
    TuningParameterTypeNil
    
} TuningParameterType;

static const std::vector<BKParameterDataType> cTuningDataTypes = {
    BKInt,
    BKInt,
    BKInt,
    BKFloat,
    BKInt,
    BKBool,
    BKInt,
    BKInt,
    BKInt,
    BKInt,
    BKFloatArr,
    BKFloatArr,
    BKInt
};

static const std::vector<std::string> cTuningParameterTypes = {
    "Tuning Id",
    "Scale",
    "Fundamental",
    "Offset",
    "A1IntervalScale",
    "A1Inversional",
    "A1AnchorScale",
    "A1AnchorFund",
    "A1ClusterThresh",
    "A1History",
    "CustomScale",
    "AbsoluteOffsets",
    "ResetPrep"
};


#pragma mark - PrepMap
typedef enum PrepMapTFIndex {
    PrepMapKeymapId = 0,
    PrepMapPreparationId,
    PrepMapParameterTypeNil
    
} PrepMapTFIndex;

static const std::vector<std::string> cPrepMapParameterTypes = {
    "PrepMapKeymap Id",
    "PrepMapPreparation Id"
};

#pragma mark - Piano
static const std::vector<std::string> cPianoParameterTypes = {
    "Piano"
};

typedef enum PianoCBIndex {
    PianoCBPiano = 0,
    PianoCBIndexNil
} PianoCBIndex;

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

typedef enum BKSampleLoadType
{
    BKLoadLite,
    BKLoadMedium,
    BKLoadHeavy,
    BKLoadNil
    
}BKSampleLoadType;

static const std::vector<std::string> cBKSampleLoadTypes = {
    "Load Light",
    "Load Medium",
    "Load Heavy"
};


// Globals
static const int aMaxNumKeymaps = 10;
static const int aMaxNumPreparationsPerPrepMap = 12;
static const int aMaxTotalPreparations = 5; // arbitrary, should be dynamic
static const int aMaxTuningPreparations = 5; // arbitrary, should be dynamic

static const float aGlobalGain = 0.5; //make this user settable

static const float aMaxSampleLengthSec = 30.0f;
static const float aRampOnTimeSec = 0.004f;
static const float aRampOffTimeSec = 0.03f; //was .004. don't actually use these anymore...
static const int aNumScaleDegrees = 12;
static const int aRampUndertowCrossMS = 50;
static const int aRampNostalgicOffMS = 20;

// Sample layers

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
    CustomTuning,
    TuningSystemNil

} TuningSystem;

#endif  // AUDIOCONSTANTS_H_INCLUDED
