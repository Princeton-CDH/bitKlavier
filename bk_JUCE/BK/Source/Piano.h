/*
  ==============================================================================

    Piano.h
    Created: 8 Dec 2016 12:59:46am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PIANO_H_INCLUDED
#define PIANO_H_INCLUDED

#include "BKUtilities.h"

#include "BKSynthesiser.h"

#include "Keymap.h"

#include "Tuning.h"

#include "Synchronic.h"

#include "Nostalgic.h"

#include "Direct.h"

class Piano : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Piano>    Ptr;
    typedef Array<Piano::Ptr>                   Arr;
    typedef Array<Piano::Ptr, CriticalSection>  CSArr;
    
    Piano(int pianoNum,
          BKSynthesiser *s,
          BKSynthesiser *res,
          BKSynthesiser *ham);
    
    ~Piano();
    
    inline void setPianoNumber(int val)         {   pianoNumber = val; print();     }
    
    inline int getPianoNumber(void)             {   return pianoNumber;     }
    
    void processBlock(int numSamples, int midiChannel);
    void setCurrentPlaybackSampleRate(double sr);
    void keyPressed(int noteNumber, float velocity, int channel);
    void keyReleased(int noteNumber, float velocity, int channel);
    
    
    void print(void)
    {
        DBG("pianoNum: " + String(pianoNumber));
    }
    
private:
    int pianoNumber;
    
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    
    // Keymap for this Piano (one per piano)
    Keymap::Ptr                     pKeymap;
    
    // Processors.
    SynchronicProcessor::CSArr      sProcessor;
    NostalgicProcessor::Arr         nProcessor;
    DirectProcessor::Arr            dProcessor;
        
    // Preparations.
    SynchronicPreparation::CSArr    sPreparation;
    NostalgicPreparation::CSArr     nPreparation;
    DirectPreparation::CSArr        dPreparation;
    TuningPreparation::CSArr        tPreparation;
    
    double sampleRate;
    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // PIANO_H_INCLUDED
