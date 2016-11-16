/*
  ==============================================================================

    UIState.h
    Created: 15 Nov 2016 6:36:33pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef UISTATE_H_INCLUDED
#define UISTATE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConstants.h"

class UIState
{
public:
    UIState()
    {
        numSynchronicLayers = 0;
        currentSynchronicLayer = 0;
        sTempo = 120;
        sNumPulses = 2;
        sClusterMin = 2;
        sClusterMax = 8;
        sClusterThresh = 1.0;
        sMode = FirstNoteSync;
        sBeatsToSkip = 0;
        sBeatMultipliers = Array<float>({1.0});
        sLengthMultipliers = Array<float>({1.0});
        sAccentMultipliers = Array<float>({1.0});
        sTuningOffsets = Array<float>(aEqualTuning);
        sBasePitch = 0;
    }
    
    ~UIState()
    {
        clearSingletonInstance();
    }
    
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
    int sBasePitch; // float?
    
    juce_DeclareSingleton (UIState, false);
    
};

juce_ImplementSingleton(UIState);


#endif  // UISTATE_H_INCLUDED
