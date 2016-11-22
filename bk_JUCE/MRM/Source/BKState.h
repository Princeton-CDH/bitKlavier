/*
  ==============================================================================

    BKState.h
    Created: 15 Nov 2016 6:36:33pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKSTATE_H_INCLUDED
#define BKSTATE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConstants.h"

class BKState
{
public:
    BKState()
    {
        numSynchronicLayers = 2;
        currentSynchronicLayer = 0;
        sTempo = 60;
        sNumPulses = 2;
        sClusterMin = 2;
        sClusterMax = 8;
        sClusterThresh = 1.0;
        sMode = FirstNoteSync;
        sBeatsToSkip = 0;
        sKeymap = Array<int>({60});
        sBeatMultipliers = Array<float>({1.0});
        sLengthMultipliers = Array<float>({1.0});
        sAccentMultipliers = Array<float>({1.0});
        sTuningOffsets = Array<float>(aEqualTuning,12);
        sBasePitch = 0;
    }
    
    ~BKState()
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
    Array<int> sKeymap;
    int sBasePitch; // float?
    
    /*
    Array<int> stringToIntArray(String s)
    {
        Array<int> arr = Array<int>({0});
        return arr;
    }
    */
    juce_DeclareSingleton (BKState, false);
    
};

juce_ImplementSingleton(BKState);


#endif  // BKSTATE_H_INCLUDED
