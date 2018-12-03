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

#define BK_UNIT_TESTS 0

#define NUM_EPOCHS 10

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


#define LOAD_LITE 38
#define LOAD_MEDIUM 39
#define LOAD_HEAVY 40
#define LOAD_LITEST 41

#define SHARE_EMAIL_ID 42
#define SHARE_FACEBOOK_ID 43
#define SHARE_MESSAGE_ID 44

#define RENAME_ID 45
#define COMMENT_ID 46
#define OFF_ID 47
#define KEYBOARD_ID 48

#define ABOUT_ID 49
#define EXPORT_ID 50
#define IMPORT_ID 51

#define SOUNDFONT_ID 1000

#define LOAD_SOUNDFONT 999

inline PopupMenu getNewItemMenu(LookAndFeel* laf)
{
    PopupMenu newMenu;
    newMenu.setLookAndFeel(laf);
    
    newMenu.addItem(KEYMAP_ID, "Keymap (K)");
    newMenu.addItem(DIRECT_ID, "Direct (D)");
    newMenu.addItem(NOSTALGIC_ID, "Nostalgic (N)");
    newMenu.addItem(SYNCHRONIC_ID, "Synchronic (S)");
    newMenu.addItem(TUNING_ID, "Tuning (T)");
    newMenu.addItem(TEMPO_ID, "Tempo (M)");
    newMenu.addSeparator();
    newMenu.addItem(MODIFICATION_ID, "Modification (C)");
    newMenu.addItem(PIANOMAP_ID, "Piano Map (P)");
    newMenu.addItem(RESET_ID, "Reset (R)");
    newMenu.addSeparator();
    newMenu.addItem(COMMENT_ID, "Comment (Q)");

    
    return newMenu;
}

inline PopupMenu getEditItemMenu(LookAndFeel* laf)
{
    PopupMenu menu;
    menu.setLookAndFeel(laf);
    
    menu.addItem(KEYMAP_EDIT_ID, "Keymap");
    menu.addItem(DIRECT_EDIT_ID, "Direct");
    menu.addItem(NOSTALGIC_EDIT_ID, "Nostalgic");
    menu.addItem(SYNCHRONIC_EDIT_ID, "Synchronic");
    menu.addItem(TUNING_EDIT_ID, "Tuning");
    menu.addItem(TEMPO_EDIT_ID, "Tempo");
    menu.addItem(DIRECTMOD_EDIT_ID, "Direct Mod");
    menu.addItem(NOSTALGICMOD_EDIT_ID, "Nostalgic Mod");
    menu.addItem(SYNCHRONICMOD_EDIT_ID, "Synchronic Mod");
    menu.addItem(TUNINGMOD_EDIT_ID, "Tuning Mod");
    menu.addItem(TEMPOMOD_EDIT_ID, "Tempo Mod");
    
    return menu;
}

inline PopupMenu getAlignMenu(LookAndFeel* laf)
{
    PopupMenu menu;
    menu.setLookAndFeel(laf);
    
    menu.addItem(ALIGN_VERTICAL, "Row");
    menu.addItem(ALIGN_HORIZONTAL, "Column");
    
    return menu;
}

inline PopupMenu getEditMenu(LookAndFeel* laf, int numItemsSelected, bool onGraph = false)
{
    PopupMenu menu;
    menu.setLookAndFeel(laf);
    
    if (onGraph)
    {
        menu.addItem(PASTE_ID, "Paste");
        menu.addSeparator();
    }
    else
    {
    
    }
    
    if (numItemsSelected)
    {
        menu.addItem(COPY_ID, "Copy");
        menu.addSeparator();
        menu.addItem(CUT_ID, "Cut");
        menu.addSeparator();
        menu.addItem(DELETE_ID, "Delete");
        menu.addSeparator();
    }
    
    if (numItemsSelected > 1)
    {
        menu.addSubMenu("Align", getAlignMenu(laf));
        menu.addSeparator();
    }
    
    if (numItemsSelected == 0)
    {
#if JUCE_IOS
        if (!onGraph) menu.addSubMenu("Add...", getNewItemMenu(laf));
#else
        menu.addSubMenu("Add...", getNewItemMenu(laf));
#endif
    }

    
    menu.addSeparator();
    menu.addItem(OFF_ID, "All Off");
    
    return menu;
}


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
}DisplayType;

String midiToPitchClass(int midi);

PitchClass      letterNoteToPitchClass(String note);
TuningSystem    tuningStringToTuningSystem(String tuning);

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


//these require inval to be between 0 and 1, and k != 1
double          dt_asymwarp(double inval, double k);
double          dt_asymwarp_inverse(double inval, double k);

String rectangleToString(Rectangle<int> rect);
String rectangleToString(Rectangle<float> rect);



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
