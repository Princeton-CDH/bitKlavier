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

#include "BKGraph.h"

class BKAudioProcessor;

class Gallery : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Gallery>   Ptr;
    typedef Array<Gallery::Ptr>                  PtrArr;
    typedef Array<Gallery::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Gallery>                  Arr;
    typedef OwnedArray<Gallery, CriticalSection> CSArr;
 
    Gallery(ScopedPointer<XmlElement> xml, BKAudioProcessor&);
    Gallery(var json, BKAudioProcessor&);
    ~Gallery();
    
    inline void print(void)
    {
        String s = "direct";
        for (auto item : direct) s += (" " + String(item->getId()));
        
        s += "\nnostalgic";
        for (auto item : nostalgic) s += (" " + String(item->getId()));
        
        s += "\nsynchronic";
        for (auto item : synchronic) s += (" " + String(item->getId()));
        
        s += "\ntuning";
        for (auto item : tuning) s += (" " + String(item->getId()));
        
        s += "\ntempo";
        for (auto item : tempo) s += (" " + String(item->getId()));
        
        s += "\nkeymap";
        for (auto item : bkKeymaps) s += (" " + String(item->getId()));
        
        s += "\ndirectmod";
        for (auto item : modDirect) s += (" " + String(item->getId()));
        
        s += "\nnostalgicmod";
        for (auto item : modNostalgic) s += (" " + String(item->getId()));
        
        s += "\nsynchronicmod";
        for (auto item : modSynchronic) s += (" " + String(item->getId()));
        
        s += "\ntuningmod";
        for (auto item : modTuning) s += (" " + String(item->getId()));
        
        s += "\ntempomod";
        for (auto item : modTempo) s += (" " + String(item->getId()));
        
        DBG("\n~ ~ ~ ~ ~ GALLERY ~ ~ ~ ~ ~ ~");
        DBG(s);
        DBG("\n");
    }
    
    
    ValueTree  getState(void);
    void setStateFromXML(ScopedPointer<XmlElement> xml);
    void setStateFromJson(var myJson);
    
    void resetPreparations(void);
    
    inline const int getNumPianos(void) const noexcept {return bkPianos.size();}
    
    void add(BKPreparationType type);
    void addTypeWithId(BKPreparationType type, int Id);
    void remove(BKPreparationType type, int Id);
    int  getNum(BKPreparationType type);
    
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
    
    inline const SynchronicPreparation::Ptr getStaticSynchronicPreparation(int Id) const noexcept
    {
        for (auto p : synchronic)
        {
            if (p->getId() == Id)   return p->sPrep;
        }
        return nullptr;
    }
    
    inline const SynchronicPreparation::Ptr getActiveSynchronicPreparation(int Id) const noexcept
    {

        for (auto p : synchronic)
        {
            if (p->getId() == Id)   return p->aPrep;
        }
        return nullptr;
    }
    
    inline const NostalgicPreparation::Ptr getStaticNostalgicPreparation(int Id) const noexcept
    {

        for (auto p : nostalgic)
        {
            if (p->getId() == Id)   return p->sPrep;
        }
        return nullptr;
    }
    
    inline const NostalgicPreparation::Ptr getActiveNostalgicPreparation(int Id) const noexcept
    {
 
        for (auto p : nostalgic)
        {
            if (p->getId() == Id)   return p->aPrep;
        }
        return nullptr;
    }
    
    inline const DirectPreparation::Ptr getStaticDirectPreparation(int Id) const noexcept
    {

        for (auto p : direct)
        {
            if (p->getId() == Id)   return p->sPrep;
        }
        return nullptr;
    }
    
    inline const DirectPreparation::Ptr getActiveDirectPreparation(int Id) const noexcept
    {
 
        for (auto p : direct)
        {
            if (p->getId() == Id)   return p->aPrep;
        }
        return nullptr;
    }
    
    inline const TuningPreparation::Ptr getStaticTuningPreparation(int Id) const noexcept
    {

        for (auto p : tuning)
        {
            if (p->getId() == Id)   return p->sPrep;
        }
        return nullptr;
    }
    
    inline const TuningPreparation::Ptr getActiveTuningPreparation(int Id) const noexcept
    {

        for (auto p : tuning)
        {
            if (p->getId() == Id)   return p->aPrep;
        }
        return nullptr;
    }
    
    inline const TempoPreparation::Ptr getStaticTempoPreparation(int Id) const noexcept
    {

        for (auto p : tempo)
        {
            if (p->getId() == Id)   return p->sPrep;
        }
        return nullptr;
    }
    
    inline const TempoPreparation::Ptr getActiveTempoPreparation(int Id) const noexcept
    {

        for (auto p : tempo)
        {
            if (p->getId() == Id)   return p->aPrep;
        }
        return nullptr;
    }
    
    inline const Synchronic::Ptr getSynchronic(int Id) const noexcept
    {

        for (auto p : synchronic)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const Nostalgic::Ptr getNostalgic(int Id) const noexcept
    {

        for (auto p : nostalgic)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const Direct::Ptr getDirect(int Id) const noexcept
    {

        for (auto p : direct)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const Tuning::Ptr getTuning(int Id) const noexcept
    {

        for (auto p : tuning)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const Tempo::Ptr getTempo(int Id) const noexcept
    {

        for (auto p : tempo)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const SynchronicModPreparation::Ptr getSynchronicModPreparation(int Id) const noexcept
    {

        for (auto p : modSynchronic)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const NostalgicModPreparation::Ptr getNostalgicModPreparation(int Id) const noexcept
    {

        for (auto p : modNostalgic)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const DirectModPreparation::Ptr getDirectModPreparation(int Id) const noexcept
    {

        for (auto p : modDirect)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const TuningModPreparation::Ptr getTuningModPreparation(int Id) const noexcept
    {

        for (auto p : modTuning)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const TempoModPreparation::Ptr getTempoModPreparation(int Id) const noexcept
    {

        for (auto p : modTempo)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const Keymap::Ptr getKeymap(int Id) const noexcept
    {

        for (auto p : bkKeymaps)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const Piano::Ptr getPiano(int Id) const noexcept
    {

        for (auto p : bkPianos)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }

    void copy(BKPreparationType type, int from, int to);
    
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

    inline int getNewId(BKPreparationType type)
    {
        int oldId = idCount[type];
        
        DBG("OLD: " + String(oldId));
        
        int newId = oldId + 1;
        
        idCount.set(type, newId);
        
        DBG("NEW: " + String(newId));
        
        return newId;
    }
    
    inline void setDefaultPiano(int Id)
    {
        defaultPianoId = Id;
    }
    
    inline int getDefaultPiano(void)
    {
        return defaultPianoId;
    }
    
    
    void addSynchronicWithId(int Id);
    void addNostalgicWithId(int Id);
    void addTuningWithId(int Id);
    void addTempoWithId(int Id);
    void addDirectWithId(int Id);
    void addKeymapWithId(int Id);
    
    inline void setURL(String newURL) { url = newURL; }
    
    inline String getURL(void) const noexcept {return url;}
    
    
private:
    double bkSampleRate;
    BKAudioProcessor& processor;
    
    Array< int> idCount;
    
    String url;
    
    GeneralSettings::Ptr                general;
    
    Synchronic::PtrArr                  synchronic;
    Nostalgic::PtrArr                   nostalgic;
    Direct::PtrArr                      direct;
    Tuning::PtrArr                      tuning;
    Tempo::PtrArr                       tempo;
    
    SynchronicModPreparation::PtrArr    modSynchronic;
    DirectModPreparation::PtrArr        modDirect;
    NostalgicModPreparation::PtrArr     modNostalgic;
    TuningModPreparation::PtrArr        modTuning;
    TempoModPreparation::PtrArr         modTempo;
    
    Keymap::PtrArr                      bkKeymaps;
    Piano::PtrArr                       bkPianos;

    int defaultPianoId;
    
    
    
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
    
    void addSynchronic(int Id, int tuningId, int tempoId);
    void addNostalgic(int Id, int tuningId, int synchronicId);
    void addDirect(int Id, int tuningId);
    
    void addKeymap(void);
    void addKeymap(Keymap::Ptr);
    inline const int getNumKeymaps(void) const noexcept {return bkKeymaps.size();}
    
    void addPiano(void);
    void addPianoWithId(int Id);
    void removePiano(int Id);
    
    void addDirectMod(void);
    void addNostalgicMod(void);
    void addSynchronicMod(void);
    void addTuningMod(void);
    void addTempoMod(void);
    
    void addDirectModWithId(int Id);
    void addNostalgicModWithId(int Id);
    void addSynchronicModWithId(int Id);
    void addTuningModWithId(int Id);
    void addTempoModWithId(int Id);
    
    void addTuningMod(TuningModPreparation::Ptr tmod);
    void addTempoMod(TempoModPreparation::Ptr tmod);
    
    void removeDirect(int Id);
    void removeSynchronic(int Id);
    void removeNostalgic(int Id);
    void removeTuning(int Id);
    void removeTempo(int Id);
    void removeKeymap(int Id);
    void removeDirectModPreparation(int Id);
    void removeNostalgicModPreparation(int Id);
    void removeSynchronicModPreparation(int Id);
    void removeTuningModPreparation(int Id);
    void removeTempoModPreparation(int Id);
    
    JUCE_LEAK_DETECTOR(Gallery);
};



#endif  // Gallery_H_INCLUDED
