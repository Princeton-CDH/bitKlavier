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

String          intArrayToString(Array<int> arr);

String          floatArrayToString(Array<float> arr);

Array<int>      stringToIntArray(String s);

Array<float>    stringToFloatArray(String s);





#endif  // BKUTILITIES_H_INCLUDED
