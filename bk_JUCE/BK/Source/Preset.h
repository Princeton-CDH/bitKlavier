/*
  ==============================================================================

    Preset.h
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PRESET_H_INCLUDED
#define PRESET_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "Synchronic.h"
#include "Nostalgic.h"
#include "Direct.h"
#include "Tuning.h"
#include "General.h"

class Preset : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Preset>   Ptr;
    typedef Array<Preset::Ptr>                  Arr;
    typedef Array<Preset::Ptr, CriticalSection> CSArr;
    
    Preset();
    Preset(BKAudioProcessor&);
    ~Preset();
    
    inline GeneralSettings::Ptr             getGeneralSettings(void)            {   return general;         }
    
    inline SynchronicPreparation::Arr       getSynchronicPreparations(void)     {   return synchronic;      }
    inline NostalgicPreparation::Arr        getNostalgicPreparations(void)      {   return nostalgic;       }
    inline DirectPreparation::Arr           getDirectPreparations(void)         {   return direct;          }
    
    inline void setGeneralSettings(GeneralSettings::Ptr g)                  {   general = g;            }
    
    inline void setSynchronicPreparations(SynchronicPreparation::Arr s)     {   synchronic = s;         }
    inline void setNostalgicPreparations(NostalgicPreparation::Arr n)       {   nostalgic=  n;          }
    inline void setDirectPreparations(DirectPreparation::Arr d)             {   direct = d;             }
    
    
    inline SynchronicPreparation::Ptr       getSynchronicPreparationForLayer(int layer)     {   return synchronic[layer];      }
    inline NostalgicPreparation::Ptr        getNostalgicPreparationForLayer(int layer)      {   return nostalgic[layer];       }
    inline DirectPreparation::Ptr           getDirectPreparationForLayer(int layer)         {   return direct[layer];          }
    
    inline void setSynchronicPreparationForLayer(SynchronicPreparation::Ptr s, int layer)   {   synchronic[layer]   = s;    }
    inline void setNostalgicPreparationForLayer(NostalgicPreparation::Ptr n, int layer)     {   nostalgic[layer]    = n;    }
    inline void setDirectPreparationForLayer(DirectPreparation::Ptr d, int layer)           {   direct[layer]       = d;    }
    
    ValueTree*  getPresetValueTree(void);
    
private:
    
    BKAudioProcessor& processor;
    
    GeneralSettings::Ptr        general;
    
    SynchronicPreparation::Arr  synchronic;
    NostalgicPreparation::Arr   nostalgic;
    DirectPreparation::Arr      direct;
    // TuningPreparation::Ptr      tuning;
    // TempoPreparation::Ptr       tempo;
    
    ValueTree vt;
    
    
    JUCE_LEAK_DETECTOR(Preset)
};


#endif  // PRESET_H_INCLUDED
