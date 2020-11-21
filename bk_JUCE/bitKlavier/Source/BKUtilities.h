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

#include "BKMenu.h"

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

#define BLENDRONIC_ID 52 //should these defines be replaced with a typedef enum for menu items?
#define BLENDRONIC_EDIT_ID 53
#define BLENDRONICMOD_EDIT_ID 54

#define CONNECTION_ID 55
#define CONNECT_ALL_ID 56
#define DISCONNECT_FROM_ID 57
#define DISCONNECT_BETWEEN_ID 58

#define SOUNDFONT_ID 1000

#define MIDIOUT_ID 2000

#define SF_DEFAULT_0 800
#define SF_DEFAULT_1 801
#define SF_DEFAULT_2 802
#define SF_DEFAULT_3 803
#define SF_DEFAULT_4 804
#define SF_DEFAULT_5 805
#define SF_DEFAULT_6 806



inline PopupMenu getNewItemMenu(LookAndFeel* laf)
{
    BKPopupMenu newMenu;
    
    newMenu.addItem(KEYMAP_ID, "Keymap" + gKeymapShortcut);
    newMenu.addItem(DIRECT_ID, "Direct" + gDirectShortcut);
    newMenu.addItem(NOSTALGIC_ID, "Nostalgic" + gNostalgicShortcut);
    newMenu.addItem(SYNCHRONIC_ID, "Synchronic" + gSynchronicShortcut);
    newMenu.addItem(BLENDRONIC_ID, "Blendronic" + gBlendronicShortcut);
    newMenu.addItem(TUNING_ID, "Tuning" + gTuningShortcut);
    newMenu.addItem(TEMPO_ID, "Tempo" + gTempoShortcut);
    newMenu.addSeparator();
    newMenu.addItem(MODIFICATION_ID, "Modification" + gModificationShortcut);
    newMenu.addItem(PIANOMAP_ID, "Piano Map" + gPianoMapShortcut);
    newMenu.addItem(RESET_ID, "Reset" + gResetShortcut);
    newMenu.addSeparator();
    newMenu.addItem(COMMENT_ID, "Comment" + gCommentShortcut);
    
    return std::move(newMenu);
}

// What is this for?
inline PopupMenu getEditItemMenu(LookAndFeel* laf)
{
    BKPopupMenu menu;
    
    menu.addItem(KEYMAP_EDIT_ID, "Keymap");
    menu.addItem(DIRECT_EDIT_ID, "Direct");
    menu.addItem(NOSTALGIC_EDIT_ID, "Nostalgic");
    menu.addItem(SYNCHRONIC_EDIT_ID, "Synchronic");
    menu.addItem(BLENDRONIC_EDIT_ID, "Blendronic");
    menu.addItem(TUNING_EDIT_ID, "Tuning");
    menu.addItem(TEMPO_EDIT_ID, "Tempo");
    menu.addItem(DIRECTMOD_EDIT_ID, "Direct Mod");
    menu.addItem(NOSTALGICMOD_EDIT_ID, "Nostalgic Mod");
    menu.addItem(SYNCHRONICMOD_EDIT_ID, "Synchronic Mod");
    menu.addItem(TUNINGMOD_EDIT_ID, "Tuning Mod");
    menu.addItem(TEMPOMOD_EDIT_ID, "Tempo Mod");
    
    return std::move(menu);
}

inline PopupMenu getAlignMenu(LookAndFeel* laf)
{
    BKPopupMenu menu;
    
    menu.addItem(ALIGN_VERTICAL, "Row");
    menu.addItem(ALIGN_HORIZONTAL, "Column");
    
    return std::move(menu);
}

inline PopupMenu getEditMenuStandalone(LookAndFeel* laf, int numItemsSelected, bool onGraph = false, bool rightClick = false)
{
    BKPopupMenu menu;
    
#if JUCE_IOS
    if (!onGraph) menu.addSubMenu("Add...", getNewItemMenu(laf));
#else
    menu.addSubMenu("Add...", getNewItemMenu(laf));
#endif
    
    if (numItemsSelected)
    {
        menu.addSeparator();
        menu.addItem(COPY_ID, "Copy" + gCopyShortcut);
        menu.addItem(CUT_ID, "Cut" + gCutShortcut);
        menu.addItem(PASTE_ID, "Paste" + gPasteShortcut);
        menu.addItem(DELETE_ID, "Delete");
        
        menu.addSeparator();
        menu.addItem(UNDO_ID, "Undo" + gUndoShortcut);
        menu.addItem(REDO_ID, "Redo" + gRedoShortcut);
        
        if (numItemsSelected == 1)
        {
            menu.addSeparator();
            menu.addItem(EDIT_ID, "Edit" + gEditShortcut);
            menu.addSeparator();
            menu.addItem(CONNECTION_ID, "Make Connection" + gConnectionShortcut);
            menu.addItem(DISCONNECT_FROM_ID, "Remove Connections To Selected" + gDisconnectFromShortcut);
        }
        
        if (numItemsSelected > 1)
        {
            menu.addSeparator();
            menu.addItem(CONNECT_ALL_ID, "Connect Selected" + gConnectAllShortcut);
            menu.addItem(DISCONNECT_FROM_ID, "Remove Connections To Selected" + gDisconnectFromShortcut);
            menu.addItem(DISCONNECT_BETWEEN_ID, "Remove Connections Between Selected" + gDisconnectBetweenShortcut);
            menu.addSeparator();
            menu.addSubMenu("Align" + gAlignShortcut, getAlignMenu(laf));
        }
    }
    else if (numItemsSelected == 0)
    {
        menu.addSeparator();
        menu.addItem(PASTE_ID, "Paste" + gPasteShortcut);
        
        menu.addSeparator();
        menu.addItem(UNDO_ID, "Undo" + gUndoShortcut);
        menu.addItem(REDO_ID, "Redo" + gRedoShortcut);
    }
    
    if (!rightClick)
    {
        menu.addSeparator();
        menu.addItem(OFF_ID, "All Off" + gAllOffShortcut);
    }
    
    return std::move(menu);
}

inline PopupMenu getEditMenu(LookAndFeel* laf, int numItemsSelected, bool onGraph = false, bool rightClick = false)
{
    BKPopupMenu menu;
    
#if JUCE_IOS
    if (!onGraph) menu.addSubMenu("Add...", getNewItemMenu(laf));
#else
    menu.addSubMenu("Add...", getNewItemMenu(laf));
#endif
    
    if (numItemsSelected)
    {
        menu.addSeparator();
        menu.addItem(COPY_ID, "Copy");
        menu.addItem(CUT_ID, "Cut");
        menu.addItem(PASTE_ID, "Paste");
        menu.addItem(DELETE_ID, "Delete");
        
        menu.addSeparator();
        menu.addItem(UNDO_ID, "Undo");
        menu.addItem(REDO_ID, "Redo");
        
        if (numItemsSelected == 1)
        {
            menu.addSeparator();
            menu.addItem(EDIT_ID, "Edit" + gEditShortcut);
            menu.addSeparator();
            menu.addItem(CONNECTION_ID, "Make Connection" + gConnectionShortcut);
            menu.addItem(DISCONNECT_FROM_ID, "Remove Connections To Selected" + gDisconnectFromShortcut);
        }
        
        if (numItemsSelected > 1)
        {
            menu.addSeparator();
            menu.addItem(CONNECT_ALL_ID, "Connect Selected" + gConnectAllShortcut);
            menu.addItem(DISCONNECT_FROM_ID, "Remove Connections To Selected" + gDisconnectFromShortcut);
            menu.addItem(DISCONNECT_BETWEEN_ID, "Remove Connections Between Selected" + gDisconnectBetweenShortcut);
            menu.addSeparator();
            menu.addSubMenu("Align" + gAlignShortcut, getAlignMenu(laf));
        }
    }
    else if (numItemsSelected == 0)
    {
        menu.addSeparator();
        menu.addItem(PASTE_ID, "Paste");
        
        menu.addSeparator();
        menu.addItem(UNDO_ID, "Undo");
        menu.addItem(REDO_ID, "Redo");
    }
    if (!rightClick)
    {
        menu.addSeparator();
        menu.addItem(OFF_ID, "All Off" + gAllOffShortcut);
    }
    
    return std::move(menu);
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
String          arrayActiveFloatArrayToString(Array<Array<float>> afarr, Array<bool> act);

String          arrayIntArrayToString(Array<Array<int>> arr);
Array<float>    stringToFloatArray(String s);
Array<Array<float>> stringToArrayFloatArray(String s);
Array<bool>     slashToFalse(String s);

String          offsetArrayToString(Array<float> arr);
String          offsetArrayToString2(Array<float> arr);
String          boolArrayToString(Array<bool> arr);
Array<int>      stringToIntArray(String s);
Array<int>      keymapStringToIntArray(String s);

Array<float>    stringOrderedPairsToFloatArray(String s, int size);

double          mtof(double f);
double          ftom(double f);

double          mtof(double f, double sr);
double          ftom(double f, double sr);


//these require inval to be between 0 and 1, and k != 1
double          dt_asymwarp(double inval, double k);
double          dt_asymwarp_inverse(double inval, double k);

int mod(int a, int b);

String rectangleToString(Rectangle<int> rect);
String rectangleToString(Rectangle<float> rect);



BKParameterDataType getBKDataType ( SynchronicParameterType param);
BKParameterDataType getBKDataType ( TuningParameterType param);
BKParameterDataType getBKDataType ( DirectParameterType param);
BKParameterDataType getBKDataType ( NostalgicParameterType param);

inline int layerToLayerId(BKNoteType type, int layer) { return (50*type)+layer;}

inline int gainToMidiVelocity(float gain) { return sqrt(127*127*gain); }

typedef enum BKTextFieldType
{
    BKParameter = 0,
    BKModification,
    BKTFNil
    
} BKTextFieldType;

typedef enum BKEditorType
{
    BKPreparationEditor,
    BKModificationEditor,
    BKEditorTypeNil
} BKEditorType;


class ModdableBase
{
public:
    ModdableBase() {}
    virtual ~ModdableBase() {}
    
    virtual void setTime(int ms) {}
    virtual int getTime() { return 0; }
    
    virtual void setInc(double v) {}
    virtual double getInc() { return 0.0; }
    
    virtual int getNumberOfInc() { return 0; }
    
    virtual void setMaxNumberOfInc(int mn) {}
    virtual int getMaxNumberOfInc() { return 0; }
    
    // tag dispatch pattern
    template <class T>
    struct tag {};
};

template <typename ValueType>
class Moddable : public ModdableBase
{
public:
    Moddable () = default;
    
    Moddable (ValueType v, ValueType m, int t, bool a):
    value(v),
    base(v),
    mod(m),
    time(t),
    dv(v), // Make sure to properly calculate dv before (and if) it is needed
    active(a),
    n(0),
    maxN(0) {};
    
    Moddable (ValueType v, int t):
    Moddable (v, v, t, false) {}
    
    Moddable (ValueType v):
    Moddable (v, v, 0, false) {}
    
    Moddable (const Moddable& m):
    Moddable (m.base, m.mod, m.time, m.active) {}
    
    bool operator== (const Moddable& m) const noexcept
    {
        return (base == m.base &&
                time == m.time);
    }
    
    Moddable& operator= (const Moddable&) = default;
    
    Moddable& operator= (ValueType v)
    {
        value = v;
        base = v;
        if (!active) mod = v;
        return *this;
    }
    
    ~Moddable() {};
    
    //==============================================================================
    
    void modTo(Moddable& m)
    {
        mod = m.mod;
        time = m.time;
        modTo(tag<ValueType>{}, m);
    }
    
    void modTo(tag<double>, Moddable& m)
    {
        mod += (m.inc * m.n);
        if (m.n < m.maxN) m.n++;
        
        if (time > 0 && (mod - value) != 0)
        {
            calcDV(tag<ValueType>{});
            active = true; //active = m.active;
            timeElapsed = 0;
            return;
        }
        value = mod;
        active = false;
    }
    void modTo(tag<int>, Moddable& m) { modTo(tag<double>{}, m); }
    void modTo(tag<float>, Moddable& m) { modTo(tag<double>{}, m); }
    void modTo(tag<bool>, Moddable& m)
    {
        if (time > 0)
        {
            active = true;
            timeElapsed = 0;
            return;
        }
        value = mod;
        active = false;
    }
    void modTo(tag<String>, Moddable& m) { modTo(tag<bool>{}, m); }
    void modTo(tag<Array<float>>, Moddable& m) { modTo(tag<bool>{}, m); }
    
    //==============================================================================
    
    void unmodFrom(Moddable& m)
    {
        mod = base;
        time = m.time;
        unmodFrom(tag<ValueType>{}, m);
    }
    
    void unmodFrom(tag<double>, Moddable& m)
    {
        if (time > 0 && (mod - value) != 0)
        {
            calcDV(tag<ValueType>{});
            active = true; //active = m.active;
            timeElapsed = 0;
            return;
        }
        value = mod;
        active = false;
    }
    void unmodFrom(tag<int>, Moddable& m) { unmodFrom(tag<double>{}, m); }
    void unmodFrom(tag<float>, Moddable& m) { unmodFrom(tag<double>{}, m); }
    void unmodFrom(tag<bool>, Moddable& m)
    {
        if (time > 0)
        {
            active = true;
            timeElapsed = 0;
            return;
        }
        value = mod;
        active = false;
    }
    void unmodFrom(tag<String>, Moddable& m) { unmodFrom(tag<bool>{}, m); }
    void unmodFrom(tag<Array<float>>, Moddable& m) { unmodFrom(tag<bool>{}, m); }
    
    //==============================================================================
    
    void reset()
    {
        value = base;
        n = 0;
        active = false;
    }
    
    // Setters
    void set(ValueType v)
    {
        value = v;
        base = v;
        mod = v;
    }
    void setValue(ValueType v) { value = v; }
    void setBase(ValueType v) { base = v; }
    void setMod(ValueType v)
    {
        mod = v;
        n = 0;
    }
    void setInc(double v) override { setInc(tag<ValueType>{}, v); }
    void setInc(tag<int>, int v) { inc = v; n = 0; }
    void setInc(tag<float>, float v) { inc = v; n = 0; }
    void setInc(tag<double>, double v) { inc = v; n = 0; }
    void setInc(tag<bool>, double v) { ; }
    void setInc(tag<String>, double v) { ; }
    void setInc(tag<Array<float>>, double v) { ; }
    void setTime(int ms) override { time = ms; }
    void setActive(bool a) { active = a; }
    void setMaxNumberOfInc(int mn) override
    {
        maxN = mn;
        n = 0;
    }
    
    // Getters
    double getInc() override { return getInc(tag<ValueType>{}); }
    double getInc(tag<int>) { return inc; }
    double getInc(tag<float>) { return inc; }
    double getInc(tag<double>) { return inc; }
    double getInc(tag<bool>) { return 0.0; }
    double getInc(tag<String>) { return 0.0; }
    double getInc(tag<Array<float>>) { return 0.0f; }
    int getTime() override { return time; }
    int getNumberOfInc() override { return n; }
    int getMaxNumberOfInc() override { return maxN; }
    
    // Step
    void step()
    {
        if (!active) return;
        if(std::is_same<ValueType,int>::value ||
           std::is_same<ValueType,double>::value ||
           std::is_same<ValueType,float>::value)
        {
            step(tag<ValueType>{});
        }
        else
        {
            if (time - timeElapsed <= 0)
            {
                value = mod;
                active = false;
            }
        }
        timeElapsed++;
    }
    void step(tag<int>)
    {
        float p = 1.0f - (float(timeElapsed) / float(time));
        if (dv > 0)
        {
            if (value < mod) value = mod - int(dv * p);
            else
            {
                value = mod;
                active = false;
            }
        }
        else if (dv < 0)
        {
            if (value > mod) value += dv;
            else
            {
                value = mod;
                active = false;
            }
        }
    }
    void step(tag<double>)
    {
        if (dv > 0)
        {
            if (value < mod) value += dv;
            else
            {
                value = mod;
                active = false;
            }
        }
        else if (dv < 0)
        {
            if (value > mod) value += dv;
            else
            {
                value = mod;
                active = false;
            }
        }
    }
    void step(tag<float>) { step(tag<double>{}); }
    
    // Doing getState and setState a bit different than elsewhere
    // (passing in reference of tree instead of returning a sub tree)
    // because it works out better for backwards compatibility while
    // avoids a lot of extra code in preparations
    // getState and setState functions
    void getState(ValueTree& vt, String s)
    { getState(tag<ValueType>{}, vt, s); }
    void getState(tag<int>, ValueTree& vt, String s)
    { getState(tag<double>{}, vt, s); }
    void getState(tag<float>, ValueTree& vt, String s)
    { getState(tag<double>{}, vt, s); }
    void getState(tag<double>, ValueTree& vt, String s)
    {
        vt.setProperty(s, base, 0);
        vt.setProperty(s + "_mod", mod, 0);
        vt.setProperty(s + "_inc", inc, 0);
        vt.setProperty(s + "_time", time, 0);
        vt.setProperty(s + "_maxN", maxN, 0);
    }
    void getState(tag<bool>, ValueTree& vt, String s)
    {
        vt.setProperty(s, base, 0);
        vt.setProperty(s + "_mod", mod, 0);
        vt.setProperty(s + "_time", time, 0);
        vt.setProperty(s + "_maxN", maxN, 0);
    }
    void getState(tag<String>, ValueTree& vt, String s)
    {
        vt.setProperty(s, base, 0);
        vt.setProperty(s + "_mod", mod, 0);
        vt.setProperty(s + "_time", time, 0);
        vt.setProperty(s + "_maxN", maxN, 0);
    }
    void getState(tag<Array<float>>, ValueTree& vt, String s)
    {
        int count = 0;
        for (auto v : base) vt.setProperty(s + String(count++), v, 0);
        count = 0;
        for (auto v : mod) vt.setProperty(s + "_mod" + String(count++), v, 0);
        count = 0;
        for (auto v : inc) vt.setProperty(s + "_inc" + String(count++), v, 0);
        vt.setProperty(s + "_time", time, 0);
        vt.setProperty(s + "_maxN", maxN, 0);
    }
    
    void setState(XmlElement* e, String s, ValueType defaultValue)
    { setState(tag<ValueType>{}, e, s, defaultValue); }
    void setState(tag<int>, XmlElement* e, String s, ValueType defaultValue)
    { setState(tag<double>{}, e, s, defaultValue); }
    void setState(tag<float>, XmlElement* e, String s, ValueType defaultValue)
    { setState(tag<double>{}, e, s, defaultValue); }
    void setState(tag<double>, XmlElement* e, String s, ValueType defaultValue)
    {
        base = e->getDoubleAttribute(s, defaultValue);
        mod = e->getDoubleAttribute(s + "_mod", base);
        inc = e->getDoubleAttribute(s + "_inc", 0.0);
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    void setState(tag<bool>, XmlElement* e, String s, ValueType defaultValue)
    {
        base = e->getBoolAttribute(s, defaultValue);
        mod = e->getBoolAttribute(s + "_mod", base);
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    void setState(tag<String>, XmlElement* e, String s, ValueType defaultValue)
    {
        base = e->getStringAttribute(s, defaultValue);
        mod = e->getStringAttribute(s + "_mod", base);
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    void setState(tag<Array<float>>, XmlElement* e, String s, ValueType defaultValue)
    {
        base = Array<float>();
        mod = Array<float>();
        inc = Array<float>();
        int count = 0;
        for (int k = 0; k < e->getNumAttributes(); k++)
        {
            if (e->hasAttribute(s + String(count)))
            {
                float b = e->getDoubleAttribute(s + String(count), 0.0f);
                base.add(b);
                mod.add(e->getDoubleAttribute(s + "_mod" + String(count), b));
                inc.add(e->getDoubleAttribute(s + "_inc" + String(count), 0.0f));
            }
            count++;
        }
        
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    
    // Moddables are used in pairs, with one existing in a preparation that is being modded
    // and one in a modification that contains mod info. Some of these members function differently
    // depending on which of the pair the Moddable is.
    
    // Current value being used by a prep Moddable, irrelevant in mod Moddable
    ValueType value;
    
    // Initial value of a prep Moddable, irrelevant in mod Moddable
    ValueType base;
    
    // Mod value of a prep Moddable which changes to match the mod value of a mod Moddable
    ValueType mod;
    
    // Irrelevant in prep Moddable, increment amount of mod Moddable
    ValueType inc;
    
private:
    void calcDV(tag<int>)
    {
        dv = mod - value;
    }
    void calcDV(tag<double>)
    {
        if (time == 0) dv = mod - value;
        else dv = (mod - value) / time;
    }
    void calcDV(tag<float>)
    {
        if (time == 0) dv = mod - value;
        else dv = (mod - value) / time;
    }
    
    // Time to mod  of a prep Moddable which changes to match the time of a mod Moddable
    int time;
    
    // Amount of change to <value> per step() to reach <mod> in <time>. Calculated when Moddable is activated
    ValueType dv;
    
    int timeElapsed;
    
    // Whether a prep Moddable is in the process of modding, irrelevant in mod Moddable
    bool active;
    
    // Irrelevant in prep Moddable, increment count of mod Moddable
    int n;
    
    // Max number of times mod can be incremented
    int maxN;
};

#endif  // BKUTILITIES_H_INCLUDED

