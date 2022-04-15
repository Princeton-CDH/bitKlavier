/*
  ==============================================================================

    PreparationMap.h
    Created: 8 Dec 2016 12:59:46am
    Author:  Michael R Mulshine
 
    A "PreparationsMap" is a Keymap connected to one or more Preparations of any type

  ==============================================================================
*/

#ifndef PreparationMap_H_INCLUDED
#define PreparationMap_H_INCLUDED

#include "BKUtilities.h"
#include "Keymap.h"

#include "Synchronic.h"
#include "Direct.h"
#include "Nostalgic.h"
#include "Tempo.h"
#include "Tuning.h"
#include "Blendronic.h"
#include "Resonance.h"

class PreparationMap : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<PreparationMap>    Ptr;
    typedef Array<PreparationMap::Ptr>                   PtrArr;
    typedef Array<PreparationMap::Ptr, CriticalSection>  CSPtrArr;
    typedef OwnedArray<PreparationMap>                   Arr;
    typedef OwnedArray<PreparationMap, CriticalSection>  CSArr;
    
    PreparationMap();
    ~PreparationMap();
    
    void prepareToPlay (double sampleRate);
    
    BKSampleLoadType sampleType;
    void processBlock(AudioSampleBuffer& buffer, int numSamples, int midiChannel, BKSampleLoadType type, bool onlyNostalgic = false);
    
    void keyPressed(int noteNumber, float velocity, int channel, int mappedFrom, bool noteDown, bool soundfont = false, String source = String("Default"));
    void keyReleased(int noteNumber, float velocity, int channel, int mappedFrom, bool noteDown, bool soundfont = false, String source = String("Default"));
    //void keyPartReleased(int noteNumber, float velocity, int channel, Keymap::PtrArr affectedKeymaps, bool soundfont = false, String source = String("Default"));
    void postRelease(int noteNumber, float velocity, int channel, int mappedFrom, String source = String("Default"));
    void clearKey(int noteNumber);
    
    void sustainPedalPressed();
    void sustainPedalReleased(bool post);
    void sustainPedalReleased(OwnedArray<HashMap<String, int>>& keysThatArePressed, bool post);
    void sustainPedalReleased() { sustainPedalReleased(false); };
    
    void attemptReattack(int noteNumber, int mappedFrom, String source = String("Default"));
    void attemptSustain(int noteNumber, float velocity, int channel, int mappedFrom,
                        bool fromPress, bool soundfont, String source = String("Default"));
    
    inline bool keymapsContainNote(int noteNumber) {
        bool contains = false;
        for (auto km : keymaps) if (km->containsNote(noteNumber)) contains = true;
        return contains;
    }
    
    inline String getPreparationIds()
    {
        String prep = "";
        for (auto p : sprocessor)
        {
            prep.append("S",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : nprocessor)
        {
            prep.append("N",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : dprocessor)
        {
            prep.append("D",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : mprocessor)
        {
            prep.append("M",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : tprocessor)
        {
            prep.append("T",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }

		for (auto p : bprocessor)
		{
			prep.append("B", 1);
			prep.append(String(p->getId()), 3);
			prep.append(" ", 1);
		}

        for (auto p : rprocessor)
        {
            prep.append("R", 1);
            prep.append(String(p->getId()), 3);
            prep.append(" ", 1);
        }
        
        if (prep == "")
            return " ";
        
        return prep;
        
    }
    
    void linkKeymapToPreparation(int keymapId, BKPreparationType thisType, int thisId);
    
    void                        addKeymap               (Keymap::Ptr );
    void                        setKeymaps              (Keymap::PtrArr);
    Keymap::PtrArr              getKeymaps              (void);
    Keymap::Ptr                 getKeymap               (int Id);
    bool                        contains                (Keymap::Ptr);
    
    void                        addDirectProcessor      (DirectProcessor::Ptr );
    void                        setDirectProcessors     (DirectProcessor::PtrArr);
    DirectProcessor::PtrArr     getDirectProcessors     (void);
    DirectProcessor::Ptr        getDirectProcessor      (int Id);
    bool                        contains                (DirectProcessor::Ptr);
    
    void                        addNostalgicProcessor   (NostalgicProcessor::Ptr );
    void                        setNostalgicProcessors  (NostalgicProcessor::PtrArr);
    NostalgicProcessor::PtrArr  getNostalgicProcessors  (void);
    NostalgicProcessor::Ptr     getNostalgicProcessor   (int Id);
    bool                        contains                (NostalgicProcessor::Ptr);
    
    void                        addSynchronicProcessor  (SynchronicProcessor::Ptr );
    void                        setSynchronicProcessors (SynchronicProcessor::PtrArr);
    SynchronicProcessor::PtrArr getSynchronicProcessors (void);
    SynchronicProcessor::Ptr    getSynchronicProcessor  (int Id);
    bool                        contains                (SynchronicProcessor::Ptr);
    
    void                        addTuningProcessor      (TuningProcessor::Ptr );
    void                        setTuningProcessors     (TuningProcessor::PtrArr);
    TuningProcessor::PtrArr     getTuningProcessors     (void);
    TuningProcessor::Ptr        getTuningProcessor      (int Id);
    bool                        contains                (TuningProcessor::Ptr);
    
    void                        addTempoProcessor       (TempoProcessor::Ptr );
    void                        setTempoProcessors      (TempoProcessor::PtrArr);
    TempoProcessor::PtrArr      getTempoProcessors      (void);
    TempoProcessor::Ptr         getTempoProcessor       (int Id);
    bool                        contains                (TempoProcessor::Ptr);

	void                        addBlendronicProcessor(BlendronicProcessor::Ptr);
	void                        setBlendronicProcessors(BlendronicProcessor::PtrArr);
	BlendronicProcessor::PtrArr      getBlendronicProcessors(void);
	BlendronicProcessor::Ptr         getBlendronicProcessor(int Id);
	bool                        contains(BlendronicProcessor::Ptr);

    void                        addResonanceProcessor(ResonanceProcessor::Ptr);
    void                        setResonanceProcessors(ResonanceProcessor::PtrArr);
    ResonanceProcessor::PtrArr      getResonanceProcessors(void);
    ResonanceProcessor::Ptr         getResonanceProcessor(int Id);
    bool                        contains(ResonanceProcessor::Ptr);
    

    void deactivateIfNecessary();
    
    bool isActive;
    
    void print(void)
    {
        Array<int> ps;

        for (auto p : keymaps) ps.add(p->getId());
        DBG("Keymap: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : sprocessor) ps.add(p->getId());
        DBG("Synchronic: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : nprocessor) ps.add(p->getId());
        DBG("Nostalgic: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : dprocessor) ps.add(p->getId());
        DBG("Direct: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : tprocessor) ps.add(p->getId());
        DBG("Tuning: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : mprocessor) ps.add(p->getId());
        DBG("Tempo: " + intArrayToString(ps));

		ps.clear();
		for (auto p : bprocessor) ps.add(p->getId());
		DBG("Blendronic: " + intArrayToString(ps));

        ps.clear();
        for (auto p : rprocessor) ps.add(p->getId());
        DBG("Resonance: " + intArrayToString(ps));
    }
    
private:
    // Keymaps for this PreparationMap
    Keymap::PtrArr                       keymaps;
    
    // Processors
    DirectProcessor::PtrArr              dprocessor;
    SynchronicProcessor::PtrArr          sprocessor;
    NostalgicProcessor::PtrArr           nprocessor;
    BlendronicProcessor::PtrArr          bprocessor;
    TempoProcessor::PtrArr               mprocessor;
    TuningProcessor::PtrArr              tprocessor;
    ResonanceProcessor::PtrArr           rprocessor;
    
    // Pointers to synths (flown in from BKAudioProcessor)
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    
    bool sustainPedalIsDepressed;

    struct Note
    {
        int noteNumber;
        float velocity;
        int channel;
        int mappedFrom; // tracks what key was played that triggered this note, for harmonizer purposes
        String source;
    };
    Array<Note> sustainedNotes;
    
    
    JUCE_LEAK_DETECTOR(PreparationMap)
};


#endif  // PreparationMap_H_INCLUDED
