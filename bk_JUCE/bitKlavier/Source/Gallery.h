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
    

 
    Gallery(XmlElement* xml, BKAudioProcessor&);
    Gallery(var json, BKAudioProcessor&);
    Gallery(BKAudioProcessor& p);
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

		s += "\nblendronic";
		for (auto item : blendronic) s += (" " + String(item->getId()));

        s += "\nresonance";
        for (auto item : resonance) s += (" " + String(item->getId()));
        
        s += "\nkeymap";
        for (auto item : bkKeymaps) s += (" " + String(item->getId()));
        
        s += "\ndirectmod";
        for (auto item : modDirect) s += (" " + String(item->getId()));
        
        s += "\nnostalgicmod";
        for (auto item : modNostalgic) s += (" " + String(item->getId()));
        
        s += "\nresonancemod";
        for (auto item : modResonance) s += (" " + String(item->getId()));
        
        s += "\nsynchronicmod";
        for (auto item : modSynchronic) s += (" " + String(item->getId()));
        
        s += "\ntuningmod";
        for (auto item : modTuning) s += (" " + String(item->getId()));
        
        s += "\ntempomod";
        for (auto item : modTempo) s += (" " + String(item->getId()));

		s += "\nblendronicmod";
		for (auto item : modBlendronic) s += (" " + String(item->getId()));
        
        DBG("\n~ ~ ~ ~ ~ GALLERY ~ ~ ~ ~ ~ ~");
        DBG(s);
        DBG("\n");
    }
    
    ValueTree  getState(void);
    void setStateFromXML(XmlElement* xml);
    void setStateFromJson(var myJson);
    
    void resetPreparations(void);

	//void randomize();
	
    inline void addDefaultPrepIfNotThere(void)
    {
        bool add = true;
        for (auto p : tempo) { if (p->getId() == -1) { add = false; break;} }
        if (add) addTempoWithId(-1);
        
        add = true;
        for (auto p : tuning) { if (p->getId() == -1) { add = false; break;} }
        if (add) addTuningWithId(-1);
        
        add = true;
        for (auto p : synchronic) { if (p->getId() == -1) { add = false; break;} }
        if (add) addSynchronicWithId(-1);
        
        add = true;
        for (auto p : nostalgic) { if (p->getId() == -1) { add = false; break;} }
        if (add) addNostalgicWithId(-1);
        
        add = true;
        for (auto p : direct) { if (p->getId() == -1) { add = false; break;} }
        if (add) addDirectWithId(-1);
        
        add = true;
        for (auto p : bkKeymaps) { if (p->getId() == -1) { add = false; break;} }
        if (add) addKeymapWithId(-1);

		add = true;
		for (auto p : blendronic) { if (p->getId() == -1) { add = false; break; } }
		if (add) addBlendronicWithId(-1);

        add = true;
        for (auto p : resonance) { if (p->getId() == -1) { add = false; break; } }
        if (add) addResonanceWithId(-1);
    }
    
    inline const int getNumPianos(void) const noexcept {return bkPianos.size();}
    
    int add     (BKPreparationType type);
    int addCopy (BKPreparationType type, XmlElement* xml, int oldId=-1);
    
    int numWithSameNameAs(BKPreparationType type, int Id);
    String iterateName(BKPreparationType type, String name);
    
    int duplicate(BKPreparationType type, int Id);
    void addTypeWithId(BKPreparationType type, int Id);
    void remove(BKPreparationType type, int Id);
    int  getNum(BKPreparationType type);
    
    
    
    
    
    inline const bool isGalleryDirty(void) const noexcept {return isDirty; }
    void setGalleryDirty(bool dirt) {isDirty = dirt;}
    
    inline const int getNumSynchronic(void) const noexcept {return synchronic.size();}
    inline const int getNumNostalgic(void) const noexcept {return nostalgic.size();}
    inline const int getNumDirect(void) const noexcept {return direct.size();}
    inline const int getNumTempo(void) const noexcept {return tempo.size();}
    inline const int getNumTuning(void) const noexcept {return tuning.size();}
	inline const int getNumBlendronic(void) const noexcept { return blendronic.size(); }
    inline const int getNumResonance(void) const noexcept { return resonance.size(); }
    inline const int getNumSynchronicMod(void) const noexcept {return modSynchronic.size();}
    inline const int getNumNostalgicMod(void) const noexcept {return modNostalgic.size();}
    inline const int getNumResonanceMod(void) const noexcept {return modResonance.size();}
    inline const int getNumDirectMod(void) const noexcept {return modDirect.size();}
    inline const int getNumTempoMod(void) const noexcept {return modTempo.size();}
    inline const int getNumTuningMod(void) const noexcept {return modTuning.size();}
	inline const int getNumBlendronicMod(void) const noexcept { return modBlendronic.size(); }
    
    inline void deregisterMTS()
    {
        for (auto t: tuning)
        {
            t->prep->deregisterMTS();
        }
    }
    
    inline const void setKeymap(int Id, Array<int> keys) const noexcept
    {
        getKeymap(Id)->setKeymap(keys);
    }

    inline const void setKeymapHarmonization(int Id, int harKey, Array<int> harArray) const noexcept
    {
        getKeymap(Id)->setHarmonizerList(harKey, harArray);
    }
    
    inline const void setKeymapHarmonizersFromString(int Id, String useToSet) const noexcept
    {
        getKeymap(Id)->setHarmonizerFromDisplayText(useToSet);
    }
    
    inline const Piano::PtrArr getPianos(void) const noexcept
    {
		return bkPianos;

		/*PianoComparator pianoComp = PianoComparator();
		Piano::PtrArr copySortPianos = Piano::PtrArr(bkPianos);
		copySortPianos.Array::sort(pianoComp, true);
        return copySortPianos;*/
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
    
    inline const Blendronic::PtrArr getAllBlendronic(void) const noexcept
    {
        return blendronic;
    }

    inline const Resonance::PtrArr getAllResonance(void) const noexcept
    {
        return resonance;
    }
    
    inline const Tempo::PtrArr getAllTempo(void) const noexcept
    {
        return tempo;
    }
    
    inline TuningModification::Ptr matches(TuningModification::Ptr mod)
    {
        for (auto p : modTuning)
        {
            if (p->compare(mod)) return p;
        }
        
        return nullptr;
    }
    
    inline Keymap::Ptr matches(Keymap::Ptr keymap)
    {
        for (auto p : bkKeymaps)
        {
            if (p->compare(keymap)) return p;
        }
        
        return nullptr;
    }
    
    inline Tuning::Ptr matches(TuningPreparation::Ptr prep)
    {
        for (auto p : tuning)
        {
            if (p->prep->compare(prep)) return p;
        }
        return nullptr;
    }
    
    inline Direct::Ptr matches(DirectPreparation::Ptr prep)
    {
        for (auto p : direct)
        {
            if (p->prep->compare(prep)) return p;
        }
        return nullptr;
    }
    
    inline Synchronic::Ptr matches(SynchronicPreparation::Ptr prep)
    {
        for (auto p : synchronic)
        {
            if (p->prep->compare(prep)) return p;
        }
        return nullptr;
    }
    
    inline Nostalgic::Ptr matches(NostalgicPreparation::Ptr prep)
    {
        for (auto p : nostalgic)
        {
            if (p->prep->compare(prep)) return p;
        }
        return nullptr;
    }
    
    inline Tempo::Ptr matches(TempoPreparation::Ptr prep)
    {
        for (auto p : tempo)
        {
            if (p->prep->compare(prep)) return p;
        }
        return nullptr;
    }

	inline Blendronic::Ptr matches(BlendronicPreparation::Ptr prep)
	{
		for (auto p : blendronic)
		{
			if (p->prep->compare(prep)) return p;
		}
		return nullptr;
	}

    inline Resonance::Ptr matches(ResonancePreparation::Ptr prep)
    {
        for (auto p : resonance)
        {
            if (p->prep->compare(prep)) return p;
        }
        return nullptr;
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

	inline const StringArray getAllBlendronicNames(void) const noexcept
	{
		StringArray names;

		for (auto prep : blendronic)
		{
			names.add(prep->getName());
		}

		return names;
	}

    inline const StringArray getAllResonanceNames(void) const noexcept
    {
        StringArray names;

        for (auto prep : resonance)
        {
            names.add(prep->getName());
        }

        return names;
    }
    
    inline const StringArray getAllDirectModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto mod : modDirect)
        {
            names.add(mod->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllNostalgicModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto mod : modNostalgic)
        {
            names.add(mod->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllResonanceModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto mod : modResonance)
        {
            names.add(mod->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllSynchronicModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto mod : modSynchronic)
        {
            names.add(mod->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllTempoModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto mod : modTempo)
        {
            names.add(mod->getName());
        }
        
        return names;
    }
    
    inline const StringArray getAllTuningModNames(void) const noexcept
    {
        StringArray names;
        
        for (auto mod : modTuning)
        {
            names.add(mod->getName());
        }
        
        return names;
    }

	inline const StringArray getAllBlendronicModNames(void) const noexcept
	{
		StringArray names;

		for (auto mod : modBlendronic)
		{
			names.add(mod->getName());
		}

		return names;
	}
    
    inline const DirectPreparation::Ptr getDirectPreparation(int Id) const noexcept
    {
        
        for (auto p : direct)
        {
            if (p->getId() == Id)   return p->prep;
        }
        return nullptr;
    }
    
    
    inline const SynchronicPreparation::Ptr getSynchronicPreparation(int Id) const noexcept
    {
        for (auto p : synchronic)
        {
            if (p->getId() == Id)   return p->prep;
        }
        return nullptr;
    }
    
    
    inline const NostalgicPreparation::Ptr getNostalgicPreparation(int Id) const noexcept
    {
 
        for (auto p : nostalgic)
        {
            if (p->getId() == Id)   return p->prep;
        }
        return nullptr;
    }
    
    inline const TuningPreparation::Ptr getTuningPreparation(int Id) const noexcept
    {

        for (auto p : tuning)
        {
            if (p->getId() == Id)   return p->prep;
        }
        return nullptr;
    }
    
    inline const TempoPreparation::Ptr getTempoPreparation(int Id) const noexcept
    {

        for (auto p : tempo)
        {
            if (p->getId() == Id)   return p->prep;
        }
        return nullptr;
    }

	inline const BlendronicPreparation::Ptr getBlendronicPreparation(int Id) const noexcept
	{

		for (auto p : blendronic)
		{
			if (p->getId() == Id)   return p->prep;
		}
		return nullptr;
	}

    inline const ResonancePreparation::Ptr getResonancePreparation(int Id) const noexcept
    {
        for (auto p : resonance)
        {
            if (p->getId() == Id)   return p->prep;
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

	inline const Blendronic::Ptr getBlendronic(int Id) const noexcept
	{

		for (auto p : blendronic)
		{
			if (p->getId() == Id)   return p;
		}
		return nullptr;
	}

    inline const Resonance::Ptr getResonance(int Id) const noexcept
    {
        for (auto p : resonance)
        {
            DBG(String(p->getId()));
        }
        for (auto p : resonance)
        {
            DBG(String(p->getId()));
            if (p->getId() == Id) return p;
            DBG("nope");
        }
        return nullptr;
    }
    
    inline const SynchronicModification::Ptr getSynchronicModification(int Id) const noexcept
    {

        for (auto p : modSynchronic)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const NostalgicModification::Ptr getNostalgicModification(int Id) const noexcept
    {

        for (auto p : modNostalgic)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const ResonanceModification::Ptr getResonanceModification(int Id) const noexcept
    {
        for (auto p : modResonance)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const DirectModification::Ptr getDirectModification(int Id) const noexcept
    {

        for (auto p : modDirect)
        {
            if (p->getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const TuningModification::Ptr getTuningModification(int Id) const noexcept
    {

        for (auto p : modTuning)
        {
            if (p->TuningModification::getId() == Id)   return p;
        }
        return nullptr;
    }
    
    inline const TempoModification::Ptr getTempoModification(int Id) const noexcept
    {

        for (auto p : modTempo)
        {
            if (p->TempoModification::getId() == Id)   return p;
        }
        return nullptr;
    }

	inline const BlendronicModification::Ptr getBlendronicModification(int Id) const noexcept
	{

		for (auto p : modBlendronic)
		{
			if (p->BlendronicModification::getId() == Id)   return p;
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
    
    inline const SynchronicModification::PtrArr getSynchronicModifications(void) const noexcept
    {
        return modSynchronic;
    }
    
    inline const NostalgicModification::PtrArr getNostalgicModifications(void) const noexcept
    {
        return modNostalgic;
    }
    
    inline const ResonanceModification::PtrArr getResonanceModifications(void) const noexcept
    {
        return modResonance;
    }
    
    inline const DirectModification::PtrArr getDirectModifications(void) const noexcept
    {
        return modDirect;
    }
    
    inline const TuningModification::PtrArr getTuningModifications(void) const noexcept
    {
        return modTuning;
    }
    
    inline const TempoModification::PtrArr getTempoModifications(void) const noexcept
    {
        return modTempo;
    }

	inline const BlendronicModification::PtrArr getBlendronicModifications(void) const noexcept
	{
		return modBlendronic;
	}
    
    inline const GeneralSettings::Ptr getGeneralSettings(void) const noexcept
    {
        return general;
    }
    
    inline const Keymap::PtrArr getKeymaps(void) const noexcept
    {
        return bkKeymaps;
    }
    
    void prepareToPlay ();

    inline int getNewId(BKPreparationType type)
    {
        int newId = idcounts[type];
        
        //DBG("OLD: " + String(oldId));
        
        idcounts[type]++;;
        
        //DBG("NEW: " + String(newId));
        
        return newId;
    }
    
    inline void setIdCount(BKPreparationType type, int count)
    {
        idcounts[type] = count;
        isDirty = true;
    }
    
    inline int getIdCount(BKPreparationType type)
    {
        return idcounts[type];
    }
    
    inline void setDefaultPiano(int Id)
    {
        defaultPianoId = Id;
        isDirty = true;
    }
    
    inline int getDefaultPiano(void)
    {
        return defaultPianoId;
    }
    
    void clean(void);
    
    void addSynchronicWithId(int Id);
    void addNostalgicWithId(int Id);
    void addTuningWithId(int Id);
    void addTempoWithId(int Id);
    void addDirectWithId(int Id);
    void addKeymapWithId(int Id);
	void addBlendronicWithId(int Id);
    void addResonanceWithId(int Id);
    
    inline void setURL(String newURL) { url = newURL; }
    
    inline String getURL(void) const noexcept {return url;}
    
    inline String getName(void) { return name;}
    
    inline void setName(String n) { name = n;}
    
    
    Array<Array<int>> used;
    
    OwnedArray<HashMap<int,int>> idmap;
    
    int idcounts[BKPreparationTypeNil];
    
    void addPiano(XmlElement* xml, OwnedArray<HashMap<int,int>>* map);
    
    GeneralSettings::Ptr                general; //HACK FOR INITIALIZATINONN
private:
    BKAudioProcessor& processor;
    
    String url;
    String name;
    
    
    
    Synchronic::PtrArr                  synchronic;
    Nostalgic::PtrArr                   nostalgic;
    Direct::PtrArr                      direct;
    Tuning::PtrArr                      tuning;
    Tempo::PtrArr                       tempo;
	Blendronic::PtrArr				    blendronic;
    Resonance::PtrArr                   resonance;
    
    SynchronicModification::PtrArr      modSynchronic;
    DirectModification::PtrArr          modDirect;
    NostalgicModification::PtrArr       modNostalgic;
    ResonanceModification::PtrArr       modResonance;
    TuningModification::PtrArr          modTuning;
    TempoModification::PtrArr           modTempo;
	BlendronicModification::PtrArr	    modBlendronic;
    
    Keymap::PtrArr                      bkKeymaps;
    Piano::PtrArr                       bkPianos;

    int defaultPianoId;
    bool isDirty;
    
    
    void addSynchronic(void);
    void addSynchronic(Synchronic::Ptr);
    void addSynchronic(SynchronicPreparation::Ptr);
    
    void addNostalgic(void);
    void addNostalgic(Nostalgic::Ptr);
    void addNostalgic(NostalgicPreparation::Ptr);
    
    void addTuning(void);
    void addTuning(Tuning::Ptr);
    void addTuning(TuningPreparation::Ptr);
    
    void addTempo(void);
    void addTempo(Tempo::Ptr);
    void addTempo(TempoPreparation::Ptr);
    
	void addBlendronic(void);
	void addBlendronic(Blendronic::Ptr);
	void addBlendronic(BlendronicPreparation::Ptr);

    void addResonance(void);
    void addResonance(Resonance::Ptr);
    void addResonance(ResonancePreparation::Ptr);

    void addDirect(void);
    void addDirect(Direct::Ptr);
    void addDirect(DirectPreparation::Ptr);
    
    void addKeymap(void);
    void addKeymap(Keymap::Ptr);
    inline const int getNumKeymaps(void) const noexcept {return bkKeymaps.size();}
    
    
    void addPiano(void);
    void addPiano(Piano::Ptr);
    void addPianoWithId(int Id);
    void removePiano(int Id);
    
    void addDirectMod(void);
    void addNostalgicMod(void);
    void addResonanceMod(void);
    void addSynchronicMod(void);
    void addTuningMod(void);
    void addTempoMod(void);
	void addBlendronicMod(void);
    
    void addDirectModWithId(int Id);
    void addNostalgicModWithId(int Id);
    void addResonanceModWithId(int Id);
    void addSynchronicModWithId(int Id);
    void addTuningModWithId(int Id);
    void addTempoModWithId(int Id);
	void addBlendronicModWithId(int Id);
    
    void addTuningMod(TuningModification::Ptr);
    void addTempoMod(TempoModification::Ptr);
    void addDirectMod(DirectModification::Ptr);
    void addSynchronicMod(SynchronicModification::Ptr);
    void addNostalgicMod(NostalgicModification::Ptr);
    void addResonanceMod(ResonanceModification::Ptr);
	void addBlendronicMod(BlendronicModification::Ptr);
    
    void removeDirect(int Id);
    void removeSynchronic(int Id);
    void removeNostalgic(int Id);
    void removeTuning(int Id);
    void removeTempo(int Id);
	void removeBlendronic(int Id);
    void removeResonance(int Id);
    void removeKeymap(int Id);
    void removeDirectModification(int Id);
    void removeNostalgicModification(int Id);
    void removeSynchronicModification(int Id);
    void removeTuningModification(int Id);
    void removeTempoModification(int Id);
	void removeBlendronicModification(int Id);
    void removeResonanceModification(int Id);
    
    int transformId(BKPreparationType type, int oldId);
    
    void addFromXML(XmlElement* xml);
    void addFromValueTree(ValueTree vt);
    
    JUCE_LEAK_DETECTOR(Gallery);
};



#endif  // Gallery_H_INCLUDED
