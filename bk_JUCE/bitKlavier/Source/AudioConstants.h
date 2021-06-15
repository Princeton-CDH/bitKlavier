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
const String ptagGeneral_noteOnSetsNoteOffVelocity = "noteOnSetsNoteOffVelocity";
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
const String ptagSynchronic_clusterCap = "clusterCap";
const String ptagSynchronic_clusterThresh = "clusterThresh";
const String ptagSynchronic_transpUsesTuning = "transpUsesTuning";
const String ptagSynchronic_mode = "mode";
const String ptagSynchronic_beatsToSkip = "beatsToSkip";
const String ptagSynchronic_gain = "gain";
const String ptagSynchronic_blendronicGain = "blendronicGain";
const String ptagSynchronic_midiOutput = "midiOutput";

const String ptagSynchronic_targetPatternSync = "targetTypeSynchronicPatternSync";
const String ptagSynchronic_targetBeatSync = "targetTypeSynchronicBeatSync";
const String ptagSynchronic_targetAddNotes = "targetTypeSynchronicAddNotes";
const String ptagSynchronic_targetPausePlay = "targetTypeSynchronicPausePlay";
const String ptagSynchronic_targetClear = "targetTypeSynchronicClear";
const String ptagSynchronic_targetDeleteOldest = "targetTypeSynchronicDeleteOldest";
const String ptagSynchronic_targetDeleteNewest = "targetTypeSynchronicDeleteNewest";
const String ptagSynchronic_targetRotate = "targetTypeSynchronicRotate";

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
const String vtagSynchronic_beatMultsStates = "beatMultipliersStates";
const String vtagSynchronic_lengthMultsStates = "lengthMultipliersStates";
const String vtagSynchronic_accentMultsStates = "accentMultipliersStates";
const String vtagSynchronic_transpOffsetsStates = "transpOffsetsStates";
const String ptagSynchronic_reset = "synchronicReset";
const String vtagSynchronic_ADSRs = "ADSRs";
const String ptagSynchronic_useGlobalSoundSet = "synchronicUseGlobalSoundSet";
const String ptagSynchronic_soundSet = "synchronicSoundSet";


#define ABSOLUTE_OFFSET_SIZE 256
const String vtagNostalgic = "nostalgic";
const String vtagModNostalgic = "modNostalgic";
const String ptagNostalgic_Id = "Id";
const String ptagNostalgic_tuning = "tuning";
const String ptagNostalgic_waveDistance = "waveDistance";
const String ptagNostalgic_undertow = "undertow";
const String vtagNostalgic_transposition = "transposition";
const String ptagNostalgic_transpUsesTuning = "transpUsesTuning";
const String ptagNostalgic_gain = "gain";
const String ptagNostalgic_blendronicGain = "blendronicGain";
const String ptagNostalgic_lengthMultiplier = "lengthMultiplier";
const String ptagNostalgic_mode = "mode";
const String ptagNostalgic_beatsToSkip = "beatsToSkip";
const String ptagNostalgic_syncTarget = "syncTarget";
const String ptagNostalgic_reset = "nostalgicReset";
const String ptagNostalgic_targetClearAll = "targetTypeNostalgicClearAll";
const String vtagNostalgic_reverseADSR = "reverseADSR";
const String vtagNostalgic_undertowADSR = "undertowADSR";
const String ptagNostalgic_useGlobalSoundSet = "nostalgicUseGlobalSoundSet";
const String ptagNostalgic_soundSet = "nostalgicSoundSet";


const String vtagDirect = "direct";
const String vtagModDirect = "modDirect";
const String ptagDirect_id ="Id";
const String ptagDirect_tuning = "tuning";
const String vtagDirect_transposition = "transposition";
const String ptagDirect_transpUsesTuning = "transpUsesTuning";
const String ptagDirect_gain = "gain";
const String ptagDirect_resGain = "resGain";
const String ptagDirect_hammerGain = "hammerGain";
const String ptagDirect_blendronicGain = "blendronicGain";
const String ptagDirect_reset = "directReset";
const String vtagDirect_ADSR = "ADSR";
const String ptagDirect_useGlobalSoundSet = "directUseGlobalSoundSet";
const String ptagDirect_soundSet = "directSoundSet";
const String ptagDirect_velocityMin = "velocityMin";
const String ptagDirect_velocityMax = "velocityMax";


const String vtagResonance = "resonance";
const String vtagModResonance = "modResonance";
const String ptagResonance_id ="Id";
const String ptagResonance_tuning = "tuning";
const String ptagResonance_gain = "gain";
const String ptagResonance_blendronicGain = "blendronicGain";
const String ptagResonance_reset = "resonanceReset";
const String vtagResonance_ADSR = "ADSR";
const String ptagResonance_useGlobalSoundSet = "resonanceUseGlobalSoundSet";
const String ptagResonance_soundSet = "resonanceSoundSet";
const String ptagResonance_starttimeMin = "starttimeMin";
const String ptagResonance_starttimeMax = "starttimeMax";
const String vtagResonance_closestKeys = "closestKeys";
const String vtagResonance_offsets = "offsets";
const String vtagResonance_gains = "gains";


const String vtagTuning = "tuning";
const String vtagModTuning = "modTuning";
const String ptagTuning_Id = "Id";
const String ptagTuning_scale = "scale";
const String ptagTuning_scaleName = "scaleName";
const String ptagTuning_fundamental = "fundamental";
const String ptagTuning_offset = "offset";
const String ptagTuning_adaptiveIntervalScale = "adaptiveIntervalScale";
const String ptagTuning_adaptiveInversional = "adaptiveInversional";
const String ptagTuning_adaptiveAnchorScale = "adaptiveAnchorScale";
const String ptagTuning_adaptiveAnchorFund = "adaptiveAnchorFund";
const String ptagTuning_adaptiveClusterThresh = "adaptiveClusterThresh";
const String ptagTuning_adaptiveHistory = "adaptiveHistory";

const String ptagTuning_nToneRoot = "nToneRoot";
const String ptagTuning_nToneRootCB = "nToneRootCB";
const String ptagTuning_nToneRootOctaveCB = "nToneRootOctaveCB";
const String ptagTuning_nToneSemitoneWidth = "nToneSemitoneWidth";

const String vtagTuning_customScale = "customScale";
const String ptagTuning_customScaleLength = "customScaleLength";
const String ptagTuning_customScaleDegree = "scaleDegree";
const String vTagTuning_absoluteOffsets = "absoluteOffsets";
const String ptagTuning_resetPrep = "tuningReset";

const String ptagTuning_tetherStiffness = "tstiff";
const String ptagTuning_intervalStiffness = "istiff";
const String ptagTuning_rate = "srate";
const String ptagTuning_drag = "drag";
const String ptagTuning_active = "active";
const String ptagTuning_tetherWeights = "tweights";
const String ptagTuning_springWeights = "sweights";
const String ptagTuning_intervalScale = "iscale";
const String ptagTuning_intervalScaleFundamental = "iscalefundamental";


const String vtagBlendronic = "blendronic";
const String vtagModBlendronic = "modBlendronic";
const String ptagBlendronic_id = "Id";
const String vtagBlendronic_beats = "beats";
const String vtagBlendronic_delayLengths = "delayLengths";
const String vtagBlendronic_smoothLengths = "smoothLengths";
const String vtagBlendronic_smoothValues = "smoothValues";
const String vtagBlendronic_feedbackCoefficients = "feedbackCoefficients";
const String vtagBlendronic_beatsStates = "beatsStates";
const String vtagBlendronic_delayLengthsStates  = "delayLengthsStates";
const String vtagBlendronic_smoothLengthsStates  = "smoothLengthsStates";
const String vtagBlendronic_smoothValuesStates  = "smoothValuesStates";
const String vtagBlendronic_feedbackCoefficientsStates  = "feedbackCoefficientsStates";
const String ptagBlendronic_smoothBase = "smoothBase";
const String ptagBlendronic_smoothScale = "smoothScale";
const String ptagBlendronic_outGain = "outGain";
const String ptagBlendronic_delayBufferSize = "delayBufferSize";

const String ptagBlendronic_targetPatternSync = "targetTypeBlendronicPatternSync";
const String ptagBlendronic_targetBeatSync = "targetTypeBlendronicBeatSync";
const String ptagBlendronic_targetClear = "targetTypeBlendronicClear";
const String ptagBlendronic_targetPausePlay = "targetTypeBlendronicPausePlay";
const String ptagBlendronic_targetOpenCloseInput = "targetTypeBlendronicOpenCloseInput";
const String ptagBlendronic_targetOpenCloseOutput = "targetTypeBlendronicOpenCloseOutput";

const String vtagKeymaps = "keymaps";
const String vtagKeymap =  "keymap";
const String vtagKeymap_harmonizer = "harmonizer";
const String vtagKeymap_midiInputNames = "midiInputs";
const String vtagKeymap_midiInputIdentifiers = "midiInputIdentifiers";
const String ptagKeymap_key = "k";
const String ptagKeymap_targetStates = "targetStates";
const String ptagKeymap_inverted = "inverted";
const String ptagKeymap_midiInputName = "midiInput";
const String ptagKeymap_midiInputIdentifier = "midiInputIdentifier";
const String ptagKeymap_defaultSelected = "defSelected";
const String ptagKeymap_onscreenSelected = "osSelected";
const String ptagKeymap_harmonizerPreTranspose = "harmonizerPreTranspose";
const String ptagKeymap_harmonizerPostTranspose = "harmonizerPostTranspose";
const String ptagKeymap_endKeystrokes = "endKeystrokes";
const String ptagKeymap_ignoreSustain = "ignoreSustain";
const String ptagKeymap_sustainPedalKeys = "sustainPedalKeys";

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
    none,
    lowest,
    highest,
    last,
    automatic,
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
    PedalNote,
    BKNoteTypeNil,
} BKNoteType;

static const std::vector<std::string> cNoteTypes = {
    "SynchronicNote",
    "NostalgicNote",
    "DirectNote",
    "MainNote",
    "HammerNote",
    "ResonanceNote",
    "PedalNote",
    "NilNote"
};

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
    PreparationTypeBlendronic,
    PreparationTypeBlendronicMod,
    PreparationTypeResonance,
    BKPreparationTypeNil,
} BKPreparationType;

inline BKPreparationType modToPrepType(BKPreparationType modType)
{
    if (modType == PreparationTypeBlendronicMod) return PreparationTypeBlendronic;
    return ((modType >= PreparationTypeDirectMod && modType <= PreparationTypeTempoMod) ? (BKPreparationType)(modType - 6) : BKPreparationTypeNil);
}

inline BKPreparationType prepToModType(BKPreparationType modType)
{
    if (modType == PreparationTypeBlendronic) return PreparationTypeBlendronicMod;
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
    "Comment",
    "Blendronic",
    "BlendronicMod",
    "Resonance",
    "ResonanceMod"
};

static const std::vector<std::string> cPreparationNames = {
    "Direct",
    "Synchronic",
    "Nostalgic",
    "Tuning",
    "Tempo",
    "Keymap",
    "Direct Mod",
    "Synchronic Mod",
    "Nostalgic Mod",
    "Tuning Mod",
    "Tempo Mod",
    "Generic Mod",
    "Piano Map",
    "Reset",
    "Piano",
    "Comment",
    "Blendronic",
    "Blendronic Mod",
    "Resonance",
    "Resonance Mod"
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
    DisplayAbout,
    DisplayComment,
    DisplayBlendronic,
    DisplayBlendronicMod,
    DisplayResonance,
    DisplayResonanceMod,
    DisplayModdable,
    DisplayNil,
} BKPreparationDisplay;



static const std::vector<std::string> cDisplayNames = {
    "Direct",
    "Synchronic",
    "Nostalgic",
    "Tuning",
    "Tempo",
    "Keymap",
    "Direct Mod",
    "Synchronic Mod",
    "Nostalgic Mod",
    "Tuning Mod",
    "Tempo Mod",
    "General",
    "About",
    "Comment",
	"Blendronic",
    "Blendronic Mod",
    "Resonance",
    "Resonance Mod"
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

typedef enum TargetNoteMode {
    NoteOn = 0,
    NoteOff,
    Both,
    TargetNoteModeNil
} TargetNoteMode;

static const std::vector<std::string> cTargetNoteModes = {
    "Note-On",
    "Note-Off",
    "Both"
};

#pragma mark - Synchronic

typedef enum SynchronicOnOffMode {
    KeyOn,
    KeyOff,
    OnOffModeNil
} SynchronicOnOffMode;

typedef enum SynchronicSyncMode {
    FirstNoteOnSync = 0,
    AnyNoteOnSync,
    FirstNoteOffSync,
    AnyNoteOffSync,
    LastNoteOffSync,
    SynchronicSyncModeNil
} SynchronicSyncMode;

static const std::vector<std::string> cSynchronicSyncModes = {
    "First Note-On",
    "Any Note-On",
    "First Note-Off",
    "Any Note-Off",
    "Last Note-Off"
};

typedef enum SynchronicParameterType {
    SynchronicId = 0,
    SynchronicNumPulses,
    SynchronicClusterMin,
    SynchronicClusterMax,
    SynchronicClusterCap,
    SynchronicClusterThresh,
    SynchronicMode,
    SynchronicBeatsToSkip,
    SynchronicTranspOffsets,
    SynchronicAccentMultipliers,
    SynchronicLengthMultipliers,
    SynchronicBeatMultipliers,
    SynchronicGain,
    SynchronicADSRs,
    SynchronicOnOff,
    SynchronicNumClusters,
    SynchronicHoldMin,
    SynchronicHoldMax,
    SynchronicVelocityMin,
    SynchronicVelocityMax,
    SynchronicMidiOutput,
    SynchronicTranspUsesTuning,
    SynchronicBlendronicGain,
    SynchronicUseGlobalSoundSet,
    SynchronicSoundSet,
    SynchronicParameterTypeNil
} SynchronicParameterType;


static const std::vector<BKParameterDataType> cSynchronicDataTypes = {
    BKInt,
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
    BKArrFloatArr,
    BKInt,
    BKInt,
    BKInt,
    BKInt,
    BKInt,
    BKBool,
    BKInt
};

static const std::vector<std::string> cSynchronicParameterTypes = {
    "Synchronic Id",
    "NumPulses",
    "ClusterMin",
    "ClusterMax",
    "ClusterCap",
    "ClusterThresh",
    "Mode",
    "BeatsToSkip",
    "transpositions",
    "accents",
    "sustain length multipliers",
    "beat length multipliers",
    "gain",
    "ADSRs",
    "SynchronicOnOff",
    "NumClusters",
    "SynchronicHoldMin",
    "SynchronicHoldMax",
    "SynchronicVelocityMin",
    "SynchronicVelocityMax"
};

static const std::vector<std::vector<float>> cSynchronicDefaultRangeValuesAndInc = {
	{ 0.0f, 0.0f, 0.0f, 0.0f }, //min, max, default, increment, skew
	{ 1.0f, 100.0f, 20.0f, 1.0f },
	{ 1.0f, 4.0f, 1.0f, 1.0f },
    { 2.0f, 8.0f, 8.0f, 1.0f },
	{ 1.0f, 20.0f, 8.0f, 1.0f },
	{ 20.0f, 2000.0f, 500.0f, 10.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 4.0f, 0.0f, 1.0f },
	{ -12.0f, 12.0f, 0.0f, 0.01f },
	{ 0.0f, 2.0f, 1.0f, 0.001f },
	{ -2.0f, 2.0f, 1.0f, 0.001f },
	{ 0.0f, 2.0f, 1.0f, 0.001f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f }
};

#pragma mark - Nostalgic
typedef enum NostalgicSyncMode {
    NoteLengthSync = 0, //reverse note length set by played note length
    SynchronicSync,     //reverse note length set by next synchronic pulse, begins with keyOn
    SynchronicSync2,    //reverse note length set by next synchronic pulse, and begins with keyOff
    NostalgicSyncModeNil
} NostalgicSyncMode;

static const std::vector<std::string> cNostalgicSyncModes = {
    "Note Length",
    "Synchronic Sync KeyDown",
    "Synchronic Sync KeyUp"
};

typedef enum NostalgicParameterType {
    NostalgicId = 0,
    NostalgicWaveDistance,
    NostalgicUndertow,
    NostalgicTransposition,
    NostalgicGain,
    NostalgicBlendronicGain, 
    NostalgicLengthMultiplier,
    NostalgicBeatsToSkip,
    NostalgicMode,
    NostalgicSyncTarget,
    NostalgicTuning,
    NostalgicReverseADSR,
    NostalgicUndertowADSR,
    NostalgicHoldMin,
    NostalgicHoldMax,
    NostalgicClusterMin,
    NostalgicClusterThreshold,
    NostalgicKeyOnReset,
    NostalgicVelocityMin,
    NostalgicVelocityMax,
    NostalgicTranspUsesTuning,
    NostalgicUseGlobalSoundSet,
    NostalgicSoundSet,
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
    BKInt,
    BKFloatArr,
    BKFloatArr,
    BKFloat,
    BKFloat,
    BKInt,
    BKInt,
    BKInt,
    BKInt,
    BKBool,
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
    "NostalgicTuning",
    "ReverseADSR",
    "UndertowADSR",
    "HoldMin",
    "HoldMax",
    "ClusterMin",
    "ClusterThreshold",
    "KeyOnReset",
    "VelocityMin",
    "VelocityMax",
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
    DirectBlendronicGain,
    DirectTuning,
    DirectADSR,
    DirectTranspUsesTuning,
    DirectUseGlobalSoundSet,
    DirectSoundSet,
    DirectVelocityMin,
    DirectVelocityMax,
    DirectParameterTypeNil,
    
} DirectParameterType;


static const std::vector<BKParameterDataType> cDirectDataTypes = {
    BKInt,
    BKFloatArr,
    BKFloat,
    BKFloat,
    BKFloat,
    BKInt,
    BKBool,
    BKFloatArr
};

static const std::vector<std::string> cDirectParameterTypes = {
    "Direct Id",
    "Transposition",
    "Gain",
    "ResGain",
    "HammerGain",
    "Tuning Id",
    "ADSR"
};

 #pragma mark - Resonance
 typedef enum ResonanceParameterType
 {
     ResonanceId = 0,
     ResonanceGain,
     ResonanceBlendronicGain,
     ResonanceMinStartTime,
     ResonanceMaxStartTime,
     ResonanceClosestKeys,
     ResonanceOffsets,
     ResonanceGains,
     ResonanceADSR,
     ResonanceUseGlobalSoundSet,
     ResonanceSoundSet,
     ParameterTypeNil
     
 } ResonanceParameterType;

static const std::vector<BKParameterDataType> cResonanceDataTypes = {
    BKInt,
    BKFloat,
    BKFloat,
    BKFloat,
    BKFloat,
    BKIntArr,
    BKFloatArr,
    BKFloatArr,
    BKFloatArr,
    BKBool,
    BKInt
};

static const std::vector<std::string> cResonanceParameterTypes = {
    "Direct Id",
    "Gain",
    "Blendronic Gain",
    "Min Start Time",
    "Max Start Time",
    "Closest Keys",
    "Offsets",
    "Gains",
    "ADSR"
};



typedef enum TuningAdaptiveSystemType
{
    AdaptiveNone = 0,
    AdaptiveNormal,
    AdaptiveAnchored,
    AdaptiveSpring,
    AdaptiveNil
    
} TuningAdaptiveSystemType;

static const std::vector<std::string> cTuningAdaptiveTypes = {
    "Nonadaptive",
    "Adaptive Tuning",
    "Adaptive Anchored",
    "Spring Tuning"
};

#pragma mark - Blendronic

typedef enum BlendronicSmoothBase {
    BlendronicSmoothPulse = 0,
    BlendronicSmoothBeat,
    BlendronicSmoothBaseNil
} BlendronicSmoothBase;

typedef enum BlendronicSmoothScale {
    BlendronicSmoothConstant = 0,
    BlendronicSmoothFull,
    BlendronicSmoothScaleNil
} BlendronicSmoothScale;

typedef enum BlendronicSyncMode {
    BlendronicFirstNoteOnSync = 0,
    BlendronicAnyNoteOnSync,
    BlendronicFirstNoteOffSync,
    BlendronicAnyNoteOffSync,
    BlendronicLastNoteOffSync,
    BlendronicSyncModeNil
} BlendronicSyncMode;

typedef enum BlendronicClearMode {
    BlendronicFirstNoteOnClear = 0,
    BlendronicAnyNoteOnClear,
    BlendronicFirstNoteOffClear,
    BlendronicAnyNoteOffClear,
    BlendronicLastNoteOffClear,
    BlendronicClearModeNil
} BlendronicClearMode;

typedef enum BlendronicOpenMode {
    BlendronicFirstNoteOnOpen = 0,
    BlendronicAnyNoteOnOpen,
    BlendronicFirstNoteOffOpen,
    BlendronicAnyNoteOffOpen,
    BlendronicLastNoteOffOpen,
    BlendronicOpenModeNil
} BlendronicOpenMode;

typedef enum BlendronicCloseMode {
    BlendronicFirstNoteOnClose = 0,
    BlendronicAnyNoteOnClose,
    BlendronicFirstNoteOffClose,
    BlendronicAnyNoteOffClose,
    BlendronicLastNoteOffClose,
    BlendronicCloseModeNil
} BlendronicCloseMode;

typedef enum BlendronicParameterType
{
    BlendronicId = 0,
    BlendronicBeats,
    BlendronicDelayLengths,
    BlendronicSmoothLengths,
    BlendronicSmoothValues,
    BlendronicFeedbackCoeffs,
    BlendronicOutGain,
    BlendronicDelayBufferSize,
    BlendronicParameterTypeNil
} BlendronicParameterType;

static const std::vector<BKParameterDataType> cBlendronicDataTypes = {
    BKInt,
    BKFloatArr,
    BKFloatArr,
    BKFloatArr,
    BKFloatArr,
    BKFloatArr,
    BKFloat
};

static const std::vector<std::string> cBlendronicParameterTypes = {
    "Blendronic Id",
    "beat lengths",
    "delay lengths",
    "smoothing (ms)",
    "smoothing (?)",
    "feedback coefficients",
    "output gain"
};

static const std::vector<std::vector<float>> cBlendronicDefaultRangeValuesAndInc = {
    { 0.0f, 0.0f, 0.0f, 0.0f }, //min, max, default, increment, skew
    { 0.0f, 8.0f, 4.0f, 0.1f },
    { 0.0f, 8.0f, 4.0f, 0.1f },
    { 0.0f, 500.0f, 50.0f, 0.01f },
    { 0.0f, 1.0f, 0.1f, 0.0001f },
    { 0.0f, 1.0f, 0.95f, 0.01f },
    { 0.0f, 1.0f, 1.0f, 0.0001f }
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
    TuningNToneRoot,
    TuningNToneRootCB,
    TuningNToneRootOctaveCB,
    TuningNToneSemitoneWidth,
    TuningCustomScale,
    TuningAbsoluteOffsets,
    TuningSpringStiffness,
    TuningSpringTetherStiffness,
    TuningSpringIntervalStiffness,
    TuningSpringRate,
    TuningSpringDrag,
    TuningSpringActive,
    TuningSpringTetherWeights,
    TuningSpringIntervalWeights,
    TuningSpringIntervalScale,
    TuningSpringIntervalFundamental,
    TuningAdaptiveSystem,
    TuningTetherWeightGlobal,
    TuningTetherWeightGlobal2,
    TuningFundamentalSetsTether,
    TuningParameterTypeNil
    
} TuningParameterType;

// ~~ ~ ! ! ! ! ! ~ ~ ~ 
// HAVE TO INTEGRATE TETHER WEIGHT GLOBAL AND FUND SETS TETHER BUTTON PARAMS AS MODS

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
    BKInt,
    BKInt,
    BKInt,
    BKFloat,
    BKFloatArr,
    BKFloatArr,
    BKFloat,
    BKFloat,
    BKFloat,
    BKFloat,
    BKFloat,
    BKBool,
    BKFloatArr,
    BKFloatArr,
    BKInt,
    BKInt,
    BKInt,
    BKFloat,
    BKFloat,
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
    "NToneRoot",
    "NToneRootCB",
    "NToneRootOctave",
    "NToneSemitoneWidth",
    "CustomScale",
    "AbsoluteOffsets",
    "SpringStiffness",
    "SpringTetherStiffness",
    "SpringIntervalStiffness",
    "SpringRate",
    "SpringDrag",
    "SpringActive",
    "SpringTetherWeights",
    "SpringIntervalWeights",
    "SpringIntervalScale",
    "SpringIntervalFundamental",
    "SpringAdaptiveSystem",
    "SpringGlobalWeight",
    "SpringGlobalWeight2",
    "FundamentalSetsTether"
};


#pragma mark - Tempo

typedef enum TempoType {
    ConstantTempo = 0,
    AdaptiveTempo1,
    HostTempo,
    TempoSystemNil
    
} TempoType;

static const std::vector<std::string> cTempoModeTypes = {
    "Constant Tempo",
    "Adaptive Tempo 1",
    "Host Tempo"
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
    TempoSubdivisions,
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
    BKInt,
    BKFloat
};

static const std::vector<std::string> cTempoParameterTypes = {
    "Tempo Id",
    "Tempo",
    "TempoMode",
    "AT1History",
    "AT1Subdivs",
    "AT1Min",
    "AT1Max",
    "AT1Mode",
    "Subdivisions"
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

// be sure to keep preparation types together sequentially
// maybe only expose Synchronic and Blendronic?
//  none of the others are really necessary, as they either happen
//  automaticaly or can be done with a Reset/Mod
//  better to keep things simpler
//  or, maybe it's confusing NOT to show the ones that are connected automatically?
typedef enum KeymapTargetType
{
    TargetTypeDirect = 0,
    
    TargetTypeSynchronic,
    TargetTypeSynchronicPatternSync,
    TargetTypeSynchronicBeatSync,
    TargetTypeSynchronicAddNotes,
    TargetTypeSynchronicClear,
    TargetTypeSynchronicPausePlay,
    TargetTypeSynchronicDeleteOldest,
    TargetTypeSynchronicDeleteNewest,
    TargetTypeSynchronicRotate,
    
    TargetTypeNostalgic,
    TargetTypeNostalgicClear,
    
    TargetTypeBlendronicPatternSync,
    TargetTypeBlendronicBeatSync,
    TargetTypeBlendronicClear,
    TargetTypeBlendronicPausePlay,
    TargetTypeBlendronicOpenCloseInput,
    TargetTypeBlendronicOpenCloseOutput,
    
    TargetTypeResonance,
    TargetTypeTempo,
    TargetTypeTuning,
    TargetTypeNil
} KeymapTargetType;

static const std::vector<std::string> cKeymapTargetTypes = {
    // Direct
    "Direct",
    // Synchronic
    "Synchronic",
    "Pattern Sync",
    "Beat Sync",
    "Add Notes",
    "Clear",
    "Pause-Play",
    "Delete Oldest Layer",
    "Delete Newest Layer",
    "Rotate Layers",
    // Nostalgic
    "Nostalgic",
    "Clear",
    // Blendronic
    "Pattern Sync",
    "Beat Sync",
    "Clear",
    "Pause-Play",
    "Open-Close Input",
    "Open-Close Output",
    // Tempo
    "Tempo",
    // Tuning
    "Tuning"
};

typedef enum KeymapTargetState
{
    TargetStateDisabled = 0,
    TargetStateEnabled,
    TargetStateNil
} KeymapTargetState;


static const std::vector<std::string> cKeymapParameterTypes = {
    "Keymap Id",
    "Keymap"
};

static const String cMidiInputDefault = "_MidiInputJUCE";
static const String cMidiInputDefaultDisplay = "Default MIDI Input";
static const String cMidiInputDAW = "_MidiInputDAW";
static const String cMidiInputUI = "_MidiInputUI";

typedef enum BKSampleLoadType
{
    BKLoadLitest = 0,
    BKLoadLite,
    BKLoadMedium,
    BKLoadHeavy,
    BKLoadSoundfont,
    BKLoadCustom,
    BKLoadNil
} BKSampleLoadType;

static const std::vector<std::string> cBKSampleLoadTypes = {
    "Piano (litest)",
    "Piano (lite)",
    "Piano (medium)",
    "Piano (heavy)"
};


// Globals
static const int aMaxNumKeymaps = 10;
static const int aMaxNumPreparationsPerPrepMap = 12;
static const int aMaxTotalPreparations = 5; // arbitrary, should be dynamic
static const int aMaxTuningPreparations = 5; // arbitrary, should be dynamic
static const int aMaxTempoPreparations = 5; // arbitrary, should be dynamic

static const float aGlobalGain = 0.5f; //make this user settable

static const float aMaxSampleLengthSec = 90.0f;//30.0f;
static const float aRampOnTimeSec = 0.004f;
static const float aRampOffTimeSec = 0.03f; //was .004. don't actually use these anymore...
static const int aNumScaleDegrees = 12;
static const int aRampUndertowCrossMS = 50;
static const int aRampNostalgicOffMS = 20;

// Sample layers

static const int aVelocityThresh_Sixteen[17] = {
    0,
    8,
    16,
    24,
    32,
    40,
    48,
    56,
    64,
    72,
    80,
    88,
    96,
    104,
    112,
    120,
    128
};

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
    Pythagorean,
    Grammateus,
    KirnbergerII,
    KirnbergerIII,
    WerkmeisterIII,
    QuarterCommaMeantone,
    SplitWolfQCMeantone,
    TransposingQCMeantone,
    Corrette,
    Rameau,
    Marpourg,
    EggarsEnglishOrd,
    ThirdCommaMeantone,
    DAlembertRousseau,
    Kellner,
    Vallotti,
    YoungII,
    SixthCommaMeantone,
    BachBarnes,
    Neidhardt,
    BachLehman,
    BachODonnell,
    BachHill,
    BachSwich,
    Lambert,
    EighthCommaWT,
    PinnockModern,   //35
    CommonJust,
    Symmetric,
    WellTunedPiano,
    HarrisonStrict,
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
    "Custom",
    "Pythagorean",
    "Grammateus",
    "Kirnberger II",
    "Kirnberger III",
    "Werkmeister III",
    "Quarter-Comma Meantone",
    "Split-Wolf QC Meantone",
    "Transposing QC Meantone",
    "Corrette",
    "Rameau",
    "Marpourg",
    "Eggar's English Ord",
    "Third-Comma Meantone",
    "D'Alembert/Rousseau",
    "Kellner",
    "Vallotti",
    "Young II",
    "Sixth-Comma Meantone",
    "Bach/Barnes",
    "Neidhardt Große Stadt",
    "Bach/Lehman",
    "Bach/O'Donnell",
    "Bach/Hill",
    "Bach/Swich",
    "Lambert",
    "Eighth-Comma WT",
    "Pinnock Modern",
    "Common Just",
    "Symmetric Just",
    "Young Well Tuned Piano",
    "Harrison Strict Songs"
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
    "11: B",
    "none"
};

static const String cDirectGain = "Direct_gain";
static const String cDirectResonanceGain = "Direct_resonanceGain";
static const String cDirectHammerGain = "Direct_hammerGain";
static const String cDirectBlendronicGain = "Direct_blendronicGain";

static const String cNostalgicGain = "Nostalgic_gain";
static const String cNostalgicBlendronicGain = "Nostalgic_blendronicGain";
static const String cNostalgicLengthMultiplier = "Nostalgic_lengthMultiplier";
static const String cNostalgicBeatsToSkip = "Nostalgic_beatsToSkip";
static const String cNostalgicClusterMin = "Nostalgic_clusterMin";
static const String cNostalgicClusterThreshold = "Nostalgic_clusterThreshold";

static const String cSynchronicGain = "Synchronic_gain";
static const String cSynchronicBlendronicGain = "Synchronic_blendronicGain";
static const String cSynchronicNumBeats = "Synchronic_numBeats";
static const String cSynchronicClusterThresh = "Synchronic_clusterThresh";
static const String cSynchronicClusterCap = "Synchronic_clusterCap";
static const String cSynchronicNumClusters = "Synchronic_numClusters";

static const String cBlendronicOutGain = "Blendronic_outGain";

static const String cTuningAdaptiveClusterThresh = "Tuning_adaptiveClusterThresh";
static const String cTuningAdaptiveHistory = "Tuning_adaptiveHistory";
static const String cTuningToneSemitoneWidth = "Tuning_toneSemitoneWidth";
static const String cTuningFundamentalOffset = "Tuning_adaptiveClusterThresh";
static const String cSpringTuningRate = "SpringTuning_rate";
static const String cSpringTuningDrag = "SpringTuning_drag";
static const String cSpringTuningStiffness = "SpringTuning_stiffness";
static const String cSpringTuningIntervalStiffness = "SpringTuning_intervalStiffness";
static const String cSpringTuningTetherWeight = "SpringTuning_tetherWeight";
static const String cSpringTuningTetherWeightSecondary = "SpringTuning_tetherWeightSecondary";

static const String cTempoTempo = "Tempo_tempo";
static const String cTempoSubdivisions = "Tempo_subdivisions";
static const String cTempoAT1History = "Tempo_at1History";
static const String cTempoAT1Subdivisions = "Tempo_at1Subdivisions";

static const String cResonanceDefGain = "Resonance_defaultGain";
static const String cResonanceMinStartTime = "Resonance_minstartTime";
static const String cResonanceMaxStartTime = "Resonance_maxstartTime";
static const String cResonanceStartTime = "Resonance_startTime"; // cut
static const String cResonanceLength = "Resonance_length"; // cut
static const String cResonanceExciteThresh = "Resonance_exciteThresh"; // cut
static const String cResonanceAttackThresh = "Resonance_attackThresh"; // cut

#endif  // AUDIOCONSTANTS_H_INCLUDED
