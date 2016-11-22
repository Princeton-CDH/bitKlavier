/*
  ==============================================================================

    SynchronicView.h
    Created: 15 Nov 2016 4:02:15pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef SYNCHRONICVIEWCONTROLLER_H_INCLUDED
#define SYNCHRONICVIEWCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"

#include "GraphicsConstants.h"

#include "AudioConstants.h"

#include "PreparationView.h"

#include "BKTextField.h"

#include "BKLabel.h"

//==============================================================================
/*
*/
class SynchronicViewController    : public PreparationView,
                                    private TextEditor::Listener
{
public:
    SynchronicViewController(MrmAudioProcessor&);
    ~SynchronicViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    int numSynchronicLayers;
    int currentSynchronicLayer;
    float sTempo;
    int sNumPulses,sClusterMin,sClusterMax;
    float sClusterThresh;
    SynchronicSyncMode sMode;
    int sBeatsToSkip; // float?
    Array<float> sBeatMultipliers;
    Array<float> sLengthMultipliers;
    Array<float> sAccentMultipliers;
    Array<float> sTuningOffsets;
    Array<int> sKeymap;
    int sBasePitch; // float?
    
    // Text Fields
    BKTextField sNumLayersTF;
    BKTextField sCurrentLayerTF;
    
    BKTextField sTempoTF;
    BKTextField sNumPulsesTF;
    BKTextField sClusterMinTF;
    BKTextField sClusterMaxTF;
    BKTextField sClusterThreshTF;
    BKTextField sModeTF;
    BKTextField sBeatsToSkipTF;
    BKTextField sBeatMultipliersTF;
    BKTextField sLengthMultipliersTF;
    BKTextField sAccentMultipliersTF;
    BKTextField sTuningOffsetsTF;
    BKTextField sBasePitchTF;
    
    BKTextField sKeymapTF;
    
    // BKLabels
    BKLabel sNumLayersL;
    BKLabel sCurrentLayerL;
    
    BKLabel sTempoL;
    BKLabel sNumPulsesL;
    BKLabel sClusterMinL;
    BKLabel sClusterMaxL;
    BKLabel sClusterThreshL;
    BKLabel sModeL;
    BKLabel sBeatsToSkipL;
    BKLabel sBeatMultipliersL;
    BKLabel sLengthMultipliersL;
    BKLabel sAccentMultipliersL;
    BKLabel sTuningOffsetsL;
    BKLabel sBasePitchL;
    
    BKLabel sKeymapL;
    
    bool shouldChange;

private:
    MrmAudioProcessor& processor;
    
    // TextEditor input parsing
    void textFieldDidChange(TextEditor&);
    
    void textEditorTextChanged(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchronicViewController)
};


#endif  // SYNCHRONICVIEWCONTROLLER_H_INCLUDED
