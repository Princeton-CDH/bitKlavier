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

#define SAVE_ID 1
#define SAVEAS_ID 2
#define OPEN_ID 3
#define NEW_ID 4
#define CLEAN_ID 5
#define SETTINGS_ID 6
#define OPENOLD_ID 7
#define DIRECT_ID 8
#define NOSTALGIC_ID 9
#define SYNCHRONIC_ID 10
#define TUNING_ID 11
#define TEMPO_ID 12
#define MODIFICATION_ID 13
#define PIANOMAP_ID 14
#define RESET_ID 15
#define NEWGALLERY_ID 16
#define DELETE_ID 17
#define KEYMAP_ID 18
#define COPY_ID 19
#define PASTE_ID 20
#define ALIGN_VERTICAL 21
#define ALIGN_HORIZONTAL 22
#define UNDO_ID 23
#define REDO_ID 24
#define CUT_ID 25
#define EDIT_ID 26


#define KEYMAP_EDIT_ID 27
#define DIRECT_EDIT_ID 28
#define NOSTALGIC_EDIT_ID 29
#define SYNCHRONIC_EDIT_ID 30
#define TUNING_EDIT_ID 31
#define TEMPO_EDIT_ID 32
#define DIRECTMOD_EDIT_ID 33
#define NOSTALGICMOD_EDIT_ID 34
#define SYNCHRONICMOD_EDIT_ID 35
#define TUNINGMOD_EDIT_ID 36
#define TEMPOMOD_EDIT_ID 37



#define IOS_SCALE 0.5


typedef enum BKPlatform
{
    BKOSX,
    BKIOS,
    BKWindows,
    BKAndroid,
    BKLinux,
    BKPlatformNil
} BKPlatform;


typedef enum DisplayType
{
    DisplayDefault = 0,
    DisplayConstruction,
    DisplayKeyboard,
    DisplayTypeNil
};

BKPlatform platform;

PitchClass      letterNoteToPitchClass(String note);
TuningSystem tuningStringToTuningSystem(String tuning);

String          intArrayToString(Array<int> arr);

String          floatArrayToString(Array<float> arr);
String          arrayFloatArrayToString(Array<Array<float>> arr);

String          arrayIntArrayToString(Array<Array<int>> arr);
Array<float>    stringToFloatArray(String s);
Array<Array<float>> stringToArrayFloatArray(String s);

String          offsetArrayToString(Array<float> arr);
String          offsetArrayToString2(Array<float> arr);
String          boolArrayToString(Array<bool> arr);
Array<int>      stringToIntArray(String s);
Array<int>      keymapStringToIntArray(String s);

Array<float>    stringOrderedPairsToFloatArray(String s, int size);

double          mtof(double f);
double          ftom(double f);

BKParameterDataType getBKDataType ( SynchronicParameterType param);
BKParameterDataType getBKDataType ( TuningParameterType param);
BKParameterDataType getBKDataType ( DirectParameterType param);
BKParameterDataType getBKDataType ( NostalgicParameterType param);

inline int layerToLayerId(BKNoteType type, int layer) { return (50*type)+layer;}

typedef enum BKTextFieldType
{
    BKParameter = 0,
    BKModification,
    BKTFNil
    
}BKTextFieldType;

typedef enum BKEditorType
{
    BKPreparationEditor,
    BKModificationEditor,
    BKEditorTypeNil
}BKEditorType;

#endif  // BKUTILITIES_H_INCLUDED
