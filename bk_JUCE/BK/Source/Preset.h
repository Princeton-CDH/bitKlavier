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
    typedef Array<Preset::Ptr>                  PtrArr;
    typedef Array<Preset::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Preset>                  Arr;
    typedef OwnedArray<Preset, CriticalSection> CSArr;
    
    Preset();
    Preset(BKAudioProcessor&);
    ~Preset();
    
    inline GeneralSettings::Ptr             getGeneralSettings(void)            {   return general;         }
    
    inline SynchronicPreparation::PtrArr       getSynchronicPreparations(void)     {   return synchronic;      }
    inline NostalgicPreparation::PtrArr        getNostalgicPreparations(void)      {   return nostalgic;       }
    inline DirectPreparation::PtrArr           getDirectPreparations(void)         {   return direct;          }
    
    inline void setGeneralSettings(GeneralSettings::Ptr g)                  {   general = g;            }
    
    inline void setSynchronicPreparations(SynchronicPreparation::PtrArr s)     {   synchronic = s;         }
    inline void setNostalgicPreparations(NostalgicPreparation::PtrArr n)       {   nostalgic=  n;          }
    inline void setDirectPreparations(DirectPreparation::PtrArr d)             {   direct = d;             }
    
    
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
    
    SynchronicPreparation::PtrArr  synchronic;
    NostalgicPreparation::PtrArr   nostalgic;
    DirectPreparation::PtrArr      direct;
    
    ValueTree vt;
    
    
    JUCE_LEAK_DETECTOR(Preset)
};


#endif  // PRESET_H_INCLUDED
