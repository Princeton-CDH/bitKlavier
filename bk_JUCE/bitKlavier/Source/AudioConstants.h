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

const String posX = "X";
const String posY = "Y";

const String jsonSynchronicX = "synchronic::";
const String jsonDirectX = "direct::";
const String jsonNostalgicX = "nostalgic::";

const String jsonSynchronicLayerX = "synchronic::synchronicSub.";
const String jsonDirectLayerX = "direct::directSub.";
const String jsonNostalgicLayerX = "nostalgic::nostalgicSub.";

const String vtagGallery = "gallery";
const String vtagGalleryPath = "galleryPath";
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
const String ptagPrepMap_tempoPrepId = "m";
const String ptagPrepMap_tuningPrepId = "t";

const String vtagPianoMap = "pianoMap";
const String ptagPianoMap_key = "key";
const String ptagPianoMap_piano = "piano";

const String vtagReset = "reset";
const String vtagResetSynchronic = "synchronicReset";
const String vtagResetNostalgic = "nostalgicReset";
const String vtagResetTuning = "tuningReset";

const String ptagModX_key = "key";
const String ptagModX_modPrep = "modPrep";
const String ptagModX_type = "type";
const String ptagModX_prep = "prep";

const String ptagFloat = "f";
const String ptagInt = "i";
const String ptagBool = "b";
const String ptagFloatArr = "fa";
const String ptagIntArr = "ia";

const String vtagSynchronic= "synchronic";
const String vtagModSynchronic = "modSynchronic";
const String ptagSynchronic_Id = "Id";
const String ptagSynchronic_tuning = "tuningId";
const String ptagSynchronic_tempo = "tempoId";
const String ptagSynchronic_numBeats = "numBeats";
const String ptagSynchronic_clusterMin = "clusterMin";
const String ptagSynchronic_clusterMax = "clusterMax";
const String ptagSynchronic_clusterThresh = "clusterThresh";
const String ptagSynchronic_mode = "mode";
const String ptagSynchronic_beatsToSkip = "beatsToSkip";

const String vtagTempo= "tempo";
const String vtagModTempo = "modTempo";
const String ptagTempo_Id = "Id";
const String ptagTempo_system = "system";
const String ptagTempo_tempo = "tempo";
const String ptagTempo_at1Mode = "at1Mode";
const String ptagTempo_at1History = "at1History";
const String ptagTempo_at1Subdivisions = "at1Subdivisions";
const String ptagTempo_at1Min = "at1Min";
const String ptagTempo_at1Max = "at1Max";

const String vtagSynchronic_beatMults = "beatMultipliers";
const String vtagSynchronic_lengthMults = "lengthMultipliers";
const String vtagSynchronic_accentMults = "accentMultipliers";
const String vtagSynchronic_transpOffsets = "transpOffsets";
const String ptagSynchronic_reset = "synchronicReset";


const String vtagNostalgic = "nostalgic";
const String vtagModNostalgic = "modNostalgic";
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


const String vtagDirect = "direct";
const String vtagModDirect = "modDirect";
const String ptagDirect_id ="Id";
const String ptagDirect_tuning = "tuning";
const String vtagDirect_transposition = "transposition";
const String ptagDirect_gain = "gain";
const String ptagDirect_resGain = "resGain";
const String ptagDirect_hammerGain = "hammerGain";
const String ptagDirect_reset = "directReset";


const String vtagTuning = "tuning";
const String vtagModTuning = "modTuning";
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
    PitchClassNil
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
    PreparationTypeTempo,
    PreparationTypeKeymap,
    PreparationTypeDirectMod,
    PreparationTypeSynchronicMod,
    PreparationTypeNostalgicMod,
    PreparationTypeTuningMod,
    PreparationTypeTempoMod,
    PreparationTypeGenericMod,
    PreparationTypePianoMap,
    PreparationTypeReset,
    PreparationTypePiano,
    PreparationTypeComment,
    BKPreparationTypeNil
} BKPreparationType;

inline BKPreparationType modToPrepType(BKPreparationType modType)
{
    return ((modType >= PreparationTypeDirectMod && modType <= PreparationTypeTempoMod) ? (BKPreparationType)(modType - 6) : BKPreparationTypeNil);
}

inline BKPreparationType prepToModType(BKPreparationType modType)
{
    return ((modType >= PreparationTypeDirect && modType <= PreparationTypeTempo) ? (BKPreparationType)(modType + 6) : BKPreparationTypeNil);
}


static const std::vector<std::string> cPreparationTypes = {
    "Direct",
    "Synchronic",
    "Nostalgic",
    "Tuning",
    "Tempo",
    "Keymap",
    "DirectMod",
    "SynchronicMod",
    "NostalgicMod",
    "TuningMod",
    "TempoMod",
    "GenericMod",
    "PianoMap",
    "Reset",
    "Piano",
    "Comment"
};

typedef enum BKPreparationDisplay {
    DisplayDirect = 0,
    DisplaySynchronic,
    DisplayNostalgic,
    DisplayTuning,
    DisplayTempo,
    DisplayKeymap,
    DisplayDirectMod,
    DisplaySynchronicMod,
    DisplayNostalgicMod,
    DisplayTuningMod,
    DisplayTempoMod,
    DisplayGeneral,
    DisplayNil,
} BKPreparationDisplay;



static const std::vector<std::string> cDisplayNames = {
    "Direct",
    "Synchronic",
    "Nostalgic",
    "Tuning",
    "Tempo",
    "Keymap",
    "General"

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

static const int aMaxNumPianos = (int)cPianoName.size();


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

static const int aMaxNumPreparationKeymaps = (int)cPrepMapName.size();

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

typedef enum SynchronicSyncMode {
    FirstNoteOnSync = 0,
    AnyNoteOnSync,
    LastNoteOffSync,
    AnyNoteOffSync,
    SynchronicSyncModeNil
} SynchronicSyncMode;

static const std::vector<std::string> cSynchronicSyncModes = {
    "First Note-On",
    "Any Note-On",
    "Last Note-Off",
    "Any Note-Off"
};

typedef enum SynchronicParameterType {
    SynchronicId = 0,
    SynchronicNumPulses,
    SynchronicClusterMin,
    SynchronicClusterMax,
    SynchronicClusterThresh,
    SynchronicMode,
    SynchronicBeatsToSkip,
    SynchronicTranspOffsets,
    SynchronicAccentMultipliers,
    SynchronicLengthMultipliers,
    SynchronicBeatMultipliers,
    SynchronicGain,
    SynchronicTuning,
    SynchronicTempo,
    SynchronicParameterTypeNil
} SynchronicParameterType;


static const std::vector<BKParameterDataType> cSynchronicDataTypes = {
    BKInt,
    BKInt,
    BKInt,
    BKInt,
    BKFloat,
    BKInt,
    BKInt,
    BKArrFloatArr,
    BKFloatArr,
    BKFloatArr,
    BKFloatArr,
    BKFloat,
    BKInt,
    BKInt,
};

static const std::vector<std::string> cSynchronicParameterTypes = {
    "Synchronic Id",
    "NumPulses",
    "ClusterMin",
    "ClusterMax",
    "ClusterThresh",
    "Mode",
    "BeatsToSkip",
    "transpositions",
    "accents",
    "sustain length multipliers",
    "beat length multipliers",
    "gain",
    "Tuning Id",
    "Tempo Id"
};

static const std::vector<std::vector<float>> cSynchronicDefaultRangeValuesAndInc = {
	{ 0.0f, 0.0f, 0.0f, 0.0f }, //min, max, default, increment, skew
	{ 1.0f, 100.0f, 20.0f, 1.0f },
	{ 1.0f, 4.0f, 1.0f, 1.0f },
	{ 2.0f, 8.0f, 8.0f, 1.0f },
	{ 20.0f, 2000.0f, 500.0f, 10.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 4.0f, 0.0f, 1.0f },
	{ -12.0f, 12.0f, 0.0f, 0.01f },
	{ 0.0f, 2.0f, 1.0f, 0.001f },
	{ -2.0f, 2.0f, 1.0f, 0.001f },
	{ 0.0f, 2.0f, 1.0f, 0.001f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
};

#pragma mark - Nostalgic
typedef enum NostalgicSyncMode {
    NoteLengthSync = 0, //reverse note length set by played note length
    SynchronicSync,     //reverse note length set by next synchronic pulse
    NostalgicSyncModeNil
} NostalgicSyncMode;

static const std::vector<std::string> cNostalgicSyncModes = {
    "Note Length",
    "Synchronic Sync"
};

typedef enum NostalgicParameterType {
    NostalgicId = 0,
    NostalgicWaveDistance,
    NostalgicUndertow,
    NostalgicTransposition,
    NostalgicGain,
    NostalgicLengthMultiplier,
    NostalgicBeatsToSkip,
    NostalgicMode,
    NostalgicSyncTarget,
    NostalgicTuning,
    NostalgicParameterTypeNil
    
} NostalgicParameterType;

static const std::vector<BKParameterDataType> cNostalgicDataTypes =
{
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
    "WaveDistance",
    "Undertow",
    "Transposition",
    "Gain",
    "LengthMult",
    "BeatsToSkip",
    "Length Mode",
    "SyncTarget",
    "Tuning Id"
};

#pragma mark - Direct
typedef enum DirectParameterType
{
    DirectId = 0,
    DirectTransposition,
    DirectGain,
    DirectResGain,
    DirectHammerGain,
    DirectTuning,
    DirectParameterTypeNil,
    
} DirectParameterType;


static const std::vector<BKParameterDataType> cDirectDataTypes = {
    BKInt,
    BKFloatArr,
    BKFloat,
    BKFloat,
    BKFloat,
    BKInt
};

static const std::vector<std::string> cDirectParameterTypes = {
    "Direct Id",
    "Transposition",
    "Gain",
    "ResGain",
    "HammerGain",
    "Tuning Id"
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
    BKFloatArr
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
    "AbsoluteOffsets"
};


#pragma mark - Tempo

static const std::vector<std::string> cTempoModeTypes = {
    "Constant Tempo",
    "Adaptive Tempo 1"
};

typedef enum AdaptiveTempo1Mode {
    TimeBetweenNotes = 0,
    NoteLength,
    AdaptiveTempo1ModeNil
} AdaptiveTempo1Mode;

static const std::vector<std::string> cAdaptiveTempoModeTypes = {
    "Time Between Notes",
    "Note Sustain Length"
};

typedef enum TempoParameterType
{
    TempoId = 0,
    TempoBPM,
    TempoSystem,
    AT1History,
    AT1Subdivisions,
    AT1Min,
    AT1Max,
    AT1Mode,
    TempoParameterTypeNil
    
} TempoParameterType;

static const std::vector<BKParameterDataType> cTempoDataTypes = {
    BKInt,
    BKFloat,
    BKInt,
    BKInt,
    BKFloat,
    BKFloat,
    BKFloat,
    BKInt
};

static const std::vector<std::string> cTempoParameterTypes = {
    "Tempo Id",
    "Tempo",
    "TempoMode",
    "AT1History",
    "AT1Subdivs",
    "AT1Min",
    "AT1Max",
    "AT1Mode"
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
    BKLoadLitest,
    BKLoadLite,
    BKLoadMedium,
    BKLoadHeavy,
    BKLoadNil
    
}BKSampleLoadType;

static const std::vector<std::string> cBKSampleLoadTypes = {
    "Load Very Light",
    "Load Light",
    "Load Medium",
    "Load Heavy"
};


// Globals
static const int aMaxNumKeymaps = 10;
static const int aMaxNumPreparationsPerPrepMap = 12;
static const int aMaxTotalPreparations = 5; // arbitrary, should be dynamic
static const int aMaxTuningPreparations = 5; // arbitrary, should be dynamic
static const int aMaxTempoPreparations = 5; // arbitrary, should be dynamic

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

static const int aVelocityThresh_One[3] = {
    0,
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
    AdaptiveTuning,
    AdaptiveAnchoredTuning,
    DuodeneTuning,
    OtonalTuning,
    UtonalTuning,
    CustomTuning,
    TuningSystemNil

} TuningSystem;

static const std::vector<std::string> cTuningSystemNames = {
    "Partial",
    "Just",
    "Equal Temperament",
    "Adaptive Tuning 1",
    "Adaptive Anchored Tuning 1",
    "Duodene",
    "Otonal",
    "Utonal",
    "Custom"
};

static const std::vector<std::string> cFundamentalNames = {
    "0: C",
    "1: C#/Db",
    "2: D",
    "3: D#/Eb",
    "4: E",
    "5: F",
    "6: F#/Gb",
    "7: G",
    "8: G#/Ab",
    "9: A",
    "10: A#/Bb",
    "11: B"
};

typedef enum TempoType {
    ConstantTempo = 0,
    AdaptiveTempo1,
    TempoSystemNil
    
} TempoType;

#endif  // AUDIOCONSTANTS_H_INCLUDED
