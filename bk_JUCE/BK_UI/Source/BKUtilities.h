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
