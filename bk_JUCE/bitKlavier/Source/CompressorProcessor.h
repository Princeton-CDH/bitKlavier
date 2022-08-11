/*
  ==============================================================================

    CompressorProcessor.h
    Created: 20 Jul 2022 10:38:53am
    Author:  Davis Polito

  ==============================================================================
*/
#include "compressor-dsp/include/Compressor.h"
#include "compressor-dsp/include/LevelEnvelopeFollower.h"
#include "AudioConstants.h""
#pragma once
class CompressorProcessor
{
public:
    CompressorProcessor();
    ~CompressorProcessor(){};
    //==============================================================================
    Atomic<float> gainReduction;
    Atomic<float> currentInput;
    Atomic<float> currentOutput;
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void update();
    void process(AudioSampleBuffer& buffer);
    Compressor compressor;
    
    inline void setState(XmlElement* e) {
        compressor.setLookahead(e->getBoolAttribute(ptagCompressor_lookAhead));
        compressor.setAutoAttack(e->getBoolAttribute(ptagCompressor_autoAttack));
        compressor.setAutoRelease(e->getBoolAttribute(ptagCompressor_autoRelease));
        compressor.setPower(e->getBoolAttribute(ptagCompressor_power));
        compressor.setAutoMakeup(e->getBoolAttribute(ptagCompressor_autoMakeup));
        
        compressor.setKnee(e->getStringAttribute(ptagCompressor_knee).getFloatValue());
        compressor.setRatio(e->getStringAttribute(ptagCompressor_ratio).getFloatValue());
        compressor.setMix(e->getStringAttribute(ptagCompressor_mix).getFloatValue());
        compressor.setMakeup(e->getStringAttribute(ptagCompressor_makeupGain).getFloatValue());
        compressor.setThreshold(e->getStringAttribute(ptagCompressor_thresh).getFloatValue());
        compressor.setInput(e->getStringAttribute(ptagCompressor_inGain).getFloatValue());
        compressor.setRelease(e->getStringAttribute(ptagCompressor_release).getFloatValue());
        compressor.setAttack(e->getStringAttribute(ptagCompressor_attack).getFloatValue());
    }
    
    inline ValueTree getState(void) {
        ValueTree vt(vtagCompressor);
        
        vt.setProperty(ptagCompressor_lookAhead, compressor.getLookahead(), 0);
        vt.setProperty(ptagCompressor_autoAttack, compressor.getAutoAttack(), 0);
        vt.setProperty(ptagCompressor_autoRelease, compressor.getAutoRelease(), 0);
        vt.setProperty(ptagCompressor_power, compressor.getPower(), 0);
        vt.setProperty(ptagCompressor_autoMakeup, compressor.getAutoMakeup(), 0);
        
        vt.setProperty(ptagCompressor_knee, compressor.getKnee(),0);
        vt.setProperty(ptagCompressor_inGain, compressor.getInput(),0);
        vt.setProperty(ptagCompressor_makeupGain, compressor.getMakeup(),0);
        vt.setProperty(ptagCompressor_attack, compressor.getAttack(),0);
        vt.setProperty(ptagCompressor_release, compressor.getRelease(), 0);
        vt.setProperty(ptagCompressor_thresh, compressor.getThreshold(), 0);
        vt.setProperty(ptagCompressor_ratio, compressor.getRatio(),0);
        vt.setProperty(ptagCompressor_mix, compressor.getMix(),0);
        return vt;
    }
private:
    
    LevelEnvelopeFollower inLevelFollower;
    LevelEnvelopeFollower outLevelFollower;
};
