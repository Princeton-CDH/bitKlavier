/*
  ==============================================================================

    CompressorProcessor.cpp
    Created: 20 Jul 2022 10:38:53am
    Author:  Davis Polito

  ==============================================================================
*/

#include "CompressorProcessor.h"
CompressorProcessor::CompressorProcessor(){
    gainReduction.set(0.0f);
    currentInput.set(-std::numeric_limits<float>::infinity());
    currentOutput.set(-std::numeric_limits<float>::infinity());
}
