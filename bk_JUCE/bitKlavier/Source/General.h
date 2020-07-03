/*
  ==============================================================================

    General.h
    Created: 6 Dec 2016 12:46:48pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef GENERAL_H_INCLUDED
#define GENERAL_H_INCLUDED

#include "BKUtilities.h"

class GeneralSettings : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<GeneralSettings>   Ptr;
    typedef Array<GeneralSettings::Ptr>                  Arr;
    typedef Array<GeneralSettings::Ptr, CriticalSection> CSArr;
    
    GeneralSettings():
        tuningFundamental(440.0),
        tuningRatio(1.),
        tempoMultiplier(1.0),
        periodMultiplier(1.0),
        invertSustain(false),
        noteOnSetsNoteOffVelocity(true)
    {

#if JUCE_IOS
        globalGain = 0.75;
#else
        globalGain = 1.0;
#endif
    }
    
    ~GeneralSettings()
    {
    }
    
    inline void setState(XmlElement* e)
    {
        float f; bool b;
        
        f = e->getStringAttribute( ptagGeneral_globalGain ).getFloatValue();
        setGlobalGain(f);
        
        f = e->getStringAttribute( ptagGeneral_tempoMultiplier ).getFloatValue();
        setTempoMultiplier(f);
        
        f = e->getStringAttribute( ptagGeneral_tuningFund ).getFloatValue();
        setTuningFundamental(f);
        
        b = (bool) e->getStringAttribute( ptagGeneral_invertSustain ).getIntValue();
        setInvertSustain(b);
        
        b = (bool) e->getStringAttribute( ptagGeneral_noteOnSetsNoteOffVelocity ).getIntValue();
        setNoteOnSetsNoteOffVelocity(b);
        
    }
    
    inline ValueTree getState(void)
    {
        ValueTree generalVT( vtagGeneral);
        
        generalVT.setProperty( ptagGeneral_globalGain,       getGlobalGain(), 0);
        generalVT.setProperty( ptagGeneral_tempoMultiplier,  getTempoMultiplier(), 0);
        generalVT.setProperty( ptagGeneral_tuningFund,       getTuningFundamental(), 0);
        generalVT.setProperty( ptagGeneral_invertSustain,    getInvertSustain(), 0);
        generalVT.setProperty( ptagGeneral_noteOnSetsNoteOffVelocity,    getNoteOnSetsNoteOffVelocity(), 0);
        
        return generalVT;
        
    }
    
    const float getTuningFundamental(void)  const noexcept  { return tuningFundamental;     };
    const float getTuningRatio(void)        const noexcept  { return tuningRatio;           };
    const float getTempoMultiplier(void)    const noexcept  { return tempoMultiplier;       };
    const float getPeriodMultiplier(void)   const noexcept  { return periodMultiplier;      };
    const float getGlobalGain(void)         const noexcept  { return globalGain;            };
    const bool  getInvertSustain(void)      const noexcept  { return invertSustain;         };
    const bool  getNoteOnSetsNoteOffVelocity(void)      const noexcept  { return noteOnSetsNoteOffVelocity; };
    
    void setTuningFundamental(float val)    {
        tuningFundamental = val;
        tuningRatio = tuningFundamental / 440.;
        // DBG("new tuning ratio " + String(tuningRatio));
    };
    void setTempoMultiplier(float val)      { tempoMultiplier = val;
                                              periodMultiplier = 1./tempoMultiplier;};
    void setGlobalGain(float val)           { globalGain = val;            };
    void setInvertSustain(bool inv)         { invertSustain = inv;}
    void setNoteOnSetsNoteOffVelocity(bool inv)         { noteOnSetsNoteOffVelocity = inv;}
    
private:
    
    float tuningFundamental; //Hz
    float tuningRatio;
    
    float tempoMultiplier;
    float periodMultiplier;
    
    float globalGain;
    
    bool resonanceAndHammer;
    bool invertSustain;
    bool noteOnSetsNoteOffVelocity;
    
    JUCE_LEAK_DETECTOR(GeneralSettings);
};


#endif  // GENERAL_H_INCLUDED
