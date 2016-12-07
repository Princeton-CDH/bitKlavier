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
        tempoMultiplier(1.0),
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
    
    
    const float getTuningFundamental(void)  const noexcept  { return tuningFundamental;     };
    const float getTempoMultiplier(void)    const noexcept  { return tempoMultiplier;       };
    const float getSynchronicGain(void)     const noexcept  { return synchronicGain;        };
    const float getNostalgicGain(void)      const noexcept  { return nostalgicGain;         };
    const float getDirectGain(void)         const noexcept  { return directGain;            };
    const float getResonanceGain(void)      const noexcept  { return resonanceGain;         };
    const float getHammerGain(void)         const noexcept  { return hammerGain;            };
    const float getGlobalGain(void)         const noexcept  { return globalGain;            };
    const bool getResonanceAndHammer(void)  const noexcept  { return resonanceAndHammer;    };
    const bool getInvertSustain(void)       const noexcept  { return invertSustain;         };
    
    void setTuningFundamental(float val)    { tuningFundamental = val;      };
    void setTempoMultiplier(float val)      { tempoMultiplier = val;       };
    void setSynchronicGain(float val)       { synchronicGain = val;        };
    void setNostalgicGain(float val)        { nostalgicGain = val;         };
    void setDirectGain(float val)           { directGain = val;            };
    void setResonanceGain(float val)        { resonanceGain = val;         };
    void setHammerGain(float val)           { hammerGain = val;            };
    void setGlobalGain(float val)           { globalGain = val;            };
    void setResonanceAndHammer(bool val)    { resonanceAndHammer = val;    };
    void setInvertSustain(bool val)         { invertSustain = val;         };
    
private:
    
    float tuningFundamental;
    
    float tempoMultiplier;
    
    float synchronicGain, nostalgicGain, directGain;
    float resonanceGain, hammerGain;
    float globalGain;
    
    bool resonanceAndHammer;
    
    bool invertSustain;
    
    JUCE_LEAK_DETECTOR(GeneralSettings);
};


#endif  // GENERAL_H_INCLUDED
