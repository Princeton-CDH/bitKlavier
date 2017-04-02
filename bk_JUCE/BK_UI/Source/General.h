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
        synchronicGain(1.0),
        nostalgicGain(1.0),
        directGain(1.0),
        resonanceGain(1.0),
        hammerGain(0.01),
        globalGain(1.0),
        resonanceAndHammer(true),
        invertSustain(false)
    {
    }
    
    ~GeneralSettings()
    {
    }
    
    inline void setState(XmlElement* e)
    {
        float f; bool b;
        
        f = e->getStringAttribute( ptagGeneral_globalGain ).getFloatValue();
        setGlobalGain(f);
        
        f = e->getStringAttribute( ptagGeneral_hammerGain ).getFloatValue();
        setHammerGain(f);
        
        f = e->getStringAttribute( ptagGeneral_resonanceGain ).getFloatValue();
        setResonanceGain(f);
        
        f = e->getStringAttribute( ptagGeneral_directGain ).getFloatValue();
        setDirectGain(f);
        
        f = e->getStringAttribute( ptagGeneral_nostalgicGain ).getFloatValue();
        setNostalgicGain(f);
        
        f = e->getStringAttribute( ptagGeneral_tempoMultiplier ).getFloatValue();
        setTempoMultiplier(f);
        
        f = e->getStringAttribute( ptagGeneral_tuningFund ).getFloatValue();
        setTuningFundamental(f);
        
        b = (bool) e->getStringAttribute( ptagGeneral_resAndHammer ).getIntValue();
        setResonanceAndHammer(b);
        
        b = (bool) e->getStringAttribute( ptagGeneral_invertSustain ).getIntValue();
        setInvertSustain(b);
    }
    
    inline ValueTree getState(void)
    {
        ValueTree generalVT( vtagGeneral);
        
        generalVT.setProperty( ptagGeneral_globalGain,       getGlobalGain(), 0);
        generalVT.setProperty( ptagGeneral_directGain,       getDirectGain(), 0);
        generalVT.setProperty( ptagGeneral_synchronicGain,   getSynchronicGain(), 0);
        generalVT.setProperty( ptagGeneral_nostalgicGain,    getNostalgicGain(), 0);
        generalVT.setProperty( ptagGeneral_resonanceGain,    getDirectGain(), 0);
        generalVT.setProperty( ptagGeneral_resonanceGain,    getResonanceGain(), 0);
        generalVT.setProperty( ptagGeneral_hammerGain,       getHammerGain(), 0);
        generalVT.setProperty( ptagGeneral_tempoMultiplier,  getTempoMultiplier(), 0);
        generalVT.setProperty( ptagGeneral_resAndHammer,     getResonanceAndHammer(), 0);
        generalVT.setProperty( ptagGeneral_invertSustain,    getInvertSustain(), 0);
        generalVT.setProperty( ptagGeneral_tuningFund,       getTuningFundamental(), 0);
        
        return generalVT;
        
    }
    
    
    
    const float getTuningFundamental(void)  const noexcept  { return tuningFundamental;     };
    const float getTuningRatio(void)        const noexcept  { return tuningRatio;           };
    const float getTempoMultiplier(void)    const noexcept  { return tempoMultiplier;       };
    const float getPeriodMultiplier(void)   const noexcept  { return periodMultiplier;      };
    const float getSynchronicGain(void)     const noexcept  { return synchronicGain;        };
    const float getNostalgicGain(void)      const noexcept  { return nostalgicGain;         };
    const float getDirectGain(void)         const noexcept  { return directGain;            };
    const float getResonanceGain(void)      const noexcept  { return resonanceGain;         };
    const float getHammerGain(void)         const noexcept  { return hammerGain;            };
    const float getGlobalGain(void)         const noexcept  { return globalGain;            };
    const bool getResonanceAndHammer(void)  const noexcept  { return resonanceAndHammer;    };
    const bool getInvertSustain(void)       const noexcept  { return invertSustain;         };
    
    void setTuningFundamental(float val)    {
        tuningFundamental = val;
        tuningRatio = tuningFundamental / 440.;
    };
    void setTempoMultiplier(float val)      { tempoMultiplier = val;
                                              periodMultiplier = 1./tempoMultiplier;};
    void setSynchronicGain(float val)       { synchronicGain = val;        };
    void setNostalgicGain(float val)        { nostalgicGain = val;         };
    void setDirectGain(float val)           { directGain = val;            };
    void setResonanceGain(float val)        { resonanceGain = val;         };
    void setHammerGain(float val)           { hammerGain = val;            };
    void setGlobalGain(float val)           { globalGain = val;            };
    void setResonanceAndHammer(bool val)    { resonanceAndHammer = val;    };
    void setInvertSustain(bool val)         { invertSustain = val;         };
    
private:
    
    float tuningFundamental; //Hz
    float tuningRatio;
    
    float tempoMultiplier;
    float periodMultiplier;
    
    float synchronicGain, nostalgicGain, directGain;
    float resonanceGain, hammerGain;
    float globalGain;
    
    bool resonanceAndHammer;
    
    bool invertSustain;
    
    JUCE_LEAK_DETECTOR(GeneralSettings);
};


#endif  // GENERAL_H_INCLUDED
