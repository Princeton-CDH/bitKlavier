/*
  ==============================================================================

    Gallery.h
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine
 
    A "Gallery" is an array of Pianos, along with one General Settings
    (equivalent to a "Library" in the original bitKlavier)

  ==============================================================================
*/

#ifndef Gallery_H_INCLUDED
#define Gallery_H_INCLUDED

#include "BKUtilities.h"

#include "Piano.h"

class Gallery : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Gallery>   Ptr;
    typedef Array<Gallery::Ptr>                  PtrArr;
    typedef Array<Gallery::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Gallery>                  Arr;
    typedef OwnedArray<Gallery, CriticalSection> CSArr;
 
    Gallery(ScopedPointer<XmlElement> xml);
    Gallery(ScopedPointer<XmlElement> xml, BKSynthesiser* main, BKSynthesiser* res, BKSynthesiser* hammer, BKUpdateState::Ptr state);
    Gallery(var json);
    Gallery(var json, BKSynthesiser* main, BKSynthesiser* res, BKSynthesiser* hammer, BKUpdateState::Ptr state);
    ~Gallery();
    
    ValueTree  getState(void);
    void setStateFromXML(ScopedPointer<XmlElement> xml);
    void setStateFromJson(var myJson);
    
    void resetPreparations(void);
    
    inline void printPianoConfigurations(void)
    {
        for (auto piano : bkPianos)
        {
            DBG("PIANO "+ String(piano->getName()));
            piano->configuration->print();
        }
    }
    
    SynchronicProcessor::Ptr getSynchronicProcessor(int Id);
    NostalgicProcessor::Ptr getNostalgicProcessor(int id);
    
    void addSynchronic(void);
    void addSynchronic(SynchronicPreparation::Ptr);
    int addSynchronicIfNotAlreadyThere(SynchronicPreparation::Ptr);
    
    void addNostalgic(void);
    void addNostalgic(NostalgicPreparation::Ptr);
    int addNostalgicIfNotAlreadyThere(NostalgicPreparation::Ptr);
    
    void addTuning(void);
    void addTuning(TuningPreparation::Ptr);
    int addTuningIfNotAlreadyThere(TuningPreparation::Ptr);
    
    void addTempo(void);
    void addTempo(TempoPreparation::Ptr);
    int addTempoIfNotAlreadyThere(TempoPreparation::Ptr);
    
    void addDirect(void);
    void addDirect(DirectPreparation::Ptr);
    int addDirectIfNotAlreadyThere(DirectPreparation::Ptr);
    
    void addPiano(void);
    void removePiano(int Id);
    inline const int getNumPianos(void) const noexcept {return bkPianos.size();}
    
    void addKeymap(void);
    void addKeymap(Keymap::Ptr);
    inline const int getNumKeymaps(void) const noexcept {return bkKeymaps.size();}
    
    void addDirectMod(void);
    void addNostalgicMod(void);
    void addSynchronicMod(void);
    void addTuningMod(void);
    void addTuningMod(TuningModPreparation::Ptr tmod);
    void addTempoMod(void);
    void addTempoMod(TempoModPreparation::Ptr tmod);
    
    void removeDirect(int Id);
    void removeSynchronic(int Id);
    void removeNostalgic(int Id);
    void removeTuning(int Id);
    void removeTempo(int Id);
    void removeDirectModPreparation(int Id);
    void removeNostalgicModPreparation(int Id);
    void removeSynchronicModPreparation(int Id);
    void removeTuningModPreparation(int Id);
    void removeTempoModPreparation(int Id);
    
    inline const int getNumSynchronic(void) const noexcept {return synchronic.size();}
    inline const int getNumNostalgic(void) const noexcept {return nostalgic.size();}
    inline const int getNumDirect(void) const noexcept {return direct.size();}
    inline const int getNumTempo(void) const noexcept {return tempo.size();}
    inline const int getNumTuning(void) const noexcept {return tuning.size();}
    inline const int getNumSynchronicMod(void) const noexcept {return modSynchronic.size();}
    inline const int getNumNostalgicMod(void) const noexcept {return modNostalgic.size();}
    inline const int getNumDirectMod(void) const noexcept {return modDirect.size();}
    inline const int getNumTempoMod(void) const noexcept {return modTempo.size();}
    inline const int getNumTuningMod(void) const noexcept {return modTuning.size();}
    
    inline const void setKeymap(int Id, Array<int> keys) const noexcept
    {
        bkKeymaps[Id]->setKeymap(keys);
    }
    
    inline const Keymap::Ptr getKeymap(int Id) const noexcept
    {
        return bkKeymaps[Id];
    }
    
    inline const Piano::Ptr getPiano(int Id) const noexcept
    {
        return bkPianos[Id];
    }
    
    inline const Piano::PtrArr getPianos(void) const noexcept
    {
        return bkPianos;
    }
    
    inline const StringArray getPianoNames(void) const noexcept
    {
        StringArray names;
        
        for (auto piano : bkPianos)
        {
            names.add(piano->getName());
        }
        
        return names;
    }
    
    
    inline const SynchronicPreparation::Ptr getStaticSynchronicPreparation(int Id) const noexcept
    {
        return synchronic[Id]->sPrep;
    }
    
    inline const SynchronicPreparation::Ptr getActiveSynchronicPreparation(int Id) const noexcept
    {
        return synchronic[Id]->aPrep;
    }
    
    inline const NostalgicPreparation::Ptr getStaticNostalgicPreparation(int Id) const noexcept
    {
        return nostalgic[Id]->sPrep;
    }
    
    inline const NostalgicPreparation::Ptr getActiveNostalgicPreparation(int Id) const noexcept
    {
        return nostalgic[Id]->aPrep;
    }
    
    inline const DirectPreparation::Ptr getStaticDirectPreparation(int Id) const noexcept
    {
        return direct[Id]->sPrep;
    }
    
    inline const DirectPreparation::Ptr getActiveDirectPreparation(int Id) const noexcept
    {
        return direct[Id]->aPrep;
    }
    
    inline const TuningPreparation::Ptr getStaticTuningPreparation(int Id) const noexcept
    {
        return tuning[Id]->sPrep;
    }
    
    inline const TuningPreparation::Ptr getActiveTuningPreparation(int Id) const noexcept
    {
        return tuning[Id]->aPrep;
    }
    
    inline const TempoPreparation::Ptr getStaticTempoPreparation(int Id) const noexcept
    {
        return tempo[Id]->sPrep;
    }
    
    inline const TempoPreparation::Ptr getActiveTempoPreparation(int Id) const noexcept
    {
        return tempo[Id]->aPrep;
    }
    
    inline const Synchronic::Ptr getSynchronic(int Id) const noexcept
    {
        return synchronic[Id];
    }
    
    inline const Nostalgic::Ptr getNostalgic(int Id) const noexcept
    {
        return nostalgic[Id];
    }
    
    inline const Direct::Ptr getDirect(int Id) const noexcept
    {
        return direct[Id];
    }
    
    inline const Direct::PtrArr getAllDirect(void) const noexcept
    {
        return direct;
    }
    
    inline const Tuning::PtrArr getAllTuning(void) const noexcept
    {
        return tuning;
    }
    
    inline const Synchronic::PtrArr getAllSynchronic(void) const noexcept
    {
        return synchronic;
    }
    
    inline const Nostalgic::PtrArr getAllNostalgic(void) const noexcept
    {
        return nostalgic;
    }
    
    inline const Tempo::PtrArr getAllTempo(void) const noexcept
    {
        return tempo;
    }
    
    inline const StringArray getAllKeymapNames(void) const noexcept
    {
        StringArray names;
        
        for (auto keymap : bkKeymaps)
        {
            names.add(keymap->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllDirectNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : direct)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllTuningNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : tuning)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllSynchronicNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : synchronic)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllNostalgicNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : nostalgic)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllTempoNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : tempo)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllDirectModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : modDirect)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllNostalgicModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : modNostalgic)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllSynchronicModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : modSynchronic)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllTempoModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : modTempo)
        {
            names.add(prep->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllTuningModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto prep : modTuning)
        {
            names.add(prep->getName());
        }
        
        return names;
    }

    
    inline const Tuning::Ptr getTuning(int Id) const noexcept
    {
        return tuning[Id];
    }
    
    inline const Tempo::Ptr getTempo(int Id) const noexcept
    {
        return tempo[Id];
    }
    
    inline const SynchronicModPreparation::Ptr getSynchronicModPreparation(int Id) const noexcept
    {
        return modSynchronic[Id];
    }
    
    inline const NostalgicModPreparation::Ptr getNostalgicModPreparation(int Id) const noexcept
    {
        return modNostalgic[Id];
    }
    
    inline const DirectModPreparation::Ptr getDirectModPreparation(int Id) const noexcept
    {
        return modDirect[Id];
    }
    
    inline const TuningModPreparation::Ptr getTuningModPreparation(int Id) const noexcept
    {
        return modTuning[Id];
    }
    
    inline const TempoModPreparation::Ptr getTempoModPreparation(int Id) const noexcept
    {
        return modTempo[Id];
    }
    
    inline const SynchronicModPreparation::PtrArr getSynchronicModPreparations(void) const noexcept
    {
        return modSynchronic;
    }
    
    inline const NostalgicModPreparation::PtrArr getNostalgicModPreparations(void) const noexcept
    {
        return modNostalgic;
    }
    
    inline const DirectModPreparation::PtrArr getDirectModPreparations(void) const noexcept
    {
        return modDirect;
    }
    
    inline const TuningModPreparation::PtrArr getTuningModPreparations(void) const noexcept
    {
        return modTuning;
    }
    
    inline const TempoModPreparation::PtrArr getTempoModPreparations(void) const noexcept
    {
        return modTempo;
    }
    
    inline const GeneralSettings::Ptr getGeneralSettings(void) const noexcept
    {
        return general;
    }
    
    inline const Keymap::PtrArr getKeymaps(void) const noexcept
    {
        return bkKeymaps;
    }
    
    void prepareToPlay (double sampleRate);

    
private:
    double bkSampleRate;
    
    BKUpdateState::Ptr updateState;
    BKSynthesiser* main;
    BKSynthesiser* res;
    BKSynthesiser* hammer;
    
    GeneralSettings::Ptr                general;
    
    Synchronic::PtrArr synchronic;
    Nostalgic::PtrArr nostalgic;
    Direct::PtrArr direct;
    Tuning::PtrArr tuning;
    Tempo::PtrArr tempo;
    
    SynchronicModPreparation::PtrArr    modSynchronic;
    DirectModPreparation::PtrArr        modDirect;
    NostalgicModPreparation::PtrArr     modNostalgic;
    TuningModPreparation::PtrArr        modTuning;
    TempoModPreparation::PtrArr         modTempo;
    
    Keymap::PtrArr                      bkKeymaps;
    Piano::PtrArr                       bkPianos;
    
    JUCE_LEAK_DETECTOR(Gallery);
};



#endif  // Gallery_H_INCLUDED
