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

//#include "Effects.h"
#include "GenericProcessor.h"
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
    
    
    
    BKSampleLoadType sampleType;
    void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages, int numSamples, int midiChannel, BKSampleLoadType type, bool onlyNostalgic = false);
    
    void keyPressed(int noteNumber, float velocity, int channel, int mappedFrom, bool noteDown, bool soundfont = false, String source = String("Default"));
    void keyReleased(int noteNumber, float velocity, int channel, int mappedFrom, bool noteDown, bool soundfont = false, String source = String("Default"));
    //void keyPartReleased(int noteNumber, float velocity, int channel, Keymap::PtrArr affectedKeymaps, bool soundfont = false, String source = String("Default"));
    void postRelease(int noteNumber, float velocity, int channel, int mappedFrom, String source = String("Default"));
    void clearKey(int noteNumber);
    
    template<typename P>
    bool keyReleasedByProcess(P proc, int noteNumber, float velocity, int mappedFrom, String source, bool noteDown, bool soundfont, KeymapTargetType targetTypeStart, KeymapTargetType targetTypeEnd, Array<float>& pressTargetVelocities, Array<float>& releaseTargetVelocities);
    
    void sustainPedalPressed();
    void sustainPedalReleased(bool post);
    void sustainPedalReleased() { sustainPedalReleased(false); };
    void sustainPedalReleased(OwnedArray<HashMap<String, int>>& keysThatArePressed, bool post) { pedalReleaseHandler(keysThatArePressed, post, false); }
    void pedalReleaseHandler(OwnedArray<HashMap<String, int>>& keysThatArePressed, bool post, bool fromSostenutoRelease);
    
    template<typename P>
    void pedalReleaseByProcess(P proc, int noteNumber, float velocity, int mappedFrom, String source, bool keyIsDepressed, bool post, KeymapTargetType targetTypeStart, KeymapTargetType targetTypeEnd);
    
    void attemptReattack(int noteNumber, int mappedFrom, String source = String("Default"));
    void attemptSustain(int noteNumber, float velocity, int channel, int mappedFrom,
                        bool fromPress, bool soundfont, String source = String("Default"));
    
    void sostenutoPedalPressed();
    void sostenutoPedalReleased(OwnedArray<HashMap<String, int>>& keysThatArePressed);
    
    inline bool keymapsContainNote(int noteNumber) {
        bool contains = false;
        for (auto km : keymaps) if (km->containsNote(noteNumber)) contains = true;
        return contains;
    }
    
    inline String getPreparationIds()
    {
        String prep = "";
        for (auto proc : processors)
        {
            switch (proc->getFirst()->getType()) {
                case BKPreparationType::PreparationTypeDirect :
                    for(auto p : *proc)
                    {
                        prep.append("D",1);
                        prep.append(String(p->getId()), 3);
                        prep.append(" ",1);
                    }
                    break;
                
                case BKPreparationType::PreparationTypeSynchronic:
                    for(auto p : *proc)
                    {
                        prep.append("S",1);
                        prep.append(String(p->getId()), 3);
                        prep.append(" ",1);
                    }
                    break;
                case BKPreparationType::PreparationTypeNostalgic:
                    for(auto p : *proc)
                    {
                        prep.append("N",1);
                        prep.append(String(p->getId()), 3);
                        prep.append(" ",1);
                    }
                    break;
                case BKPreparationType::PreparationTypeResonance:
                    for(auto p : *proc)
                    {
                        prep.append("R", 1);
                        prep.append(String(p->getId()), 3);
                        prep.append(" ", 1);
                    }
                    break;
                case BKPreparationType::PreparationTypeTempo:
                    for(auto p : *proc)
                    {
                        prep.append("M",1);
                        prep.append(String(p->getId()), 3);
                        prep.append(" ",1);
                    }
                    break;
                case BKPreparationType::PreparationTypeTuning:
                    for(auto p : *proc)
                    {
                        prep.append("T",1);
                        prep.append(String(p->getId()), 3);
                        prep.append(" ",1);
                    }
                    break;
                case BKPreparationType::PreparationTypeBlendronic:
//                    for(auto p : proc)
//                    {
//                        prep.append("B", 1);
//                        prep.append(String(proc->getId()), 3);
//                        prep.append(" ", 1);
//                    }
                    break;
                default:
                    break;
            }
           
            
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
    
 
    void     getProcessorsOfType    (GenericProcessor::PtrArr, BKPreparationType);
    GenericProcessor::Ptr        getProcessorOfType    (int Id, BKPreparationType);
    bool                        contains                (GenericProcessor::Ptr, BKPreparationType);
    void  clearProcessors() {
        for (auto proc: processors)
        {
            proc->clear();
        }
    }

   

//	void                        addEffectProcessor(EffectProcessor::Ptr);
//	//void                        setEffectProcessors(EffectProcessor::PtrArr);
//	EffectProcessor::PtrArr      getEffectProcessors(void);
//	EffectProcessor::Ptr         getEffectProcessor(int Id, EffectType type);
//	bool                        contains(EffectProcessor::Ptr);


    
    void addProcessor(GenericProcessor::Ptr p)
    {
        processors.getUnchecked(p->getType())->addIfNotAlreadyThere(p);
    }
    void deactivateIfNecessary();
    
    bool isActive;
    
    void print(void)
    {
        Array<int> ps;

//        for (auto p : keymaps) ps.add(p->getId());
//        DBG("Keymap: " + intArrayToString(ps));
//
//        ps.clear();
//        for (auto p : sprocessor) ps.add(p->getId());
//        DBG("Synchronic: " + intArrayToString(ps));
//
//        ps.clear();
//        for (auto p : nprocessor) ps.add(p->getId());
//        DBG("Nostalgic: " + intArrayToString(ps));
//
//        ps.clear();
//        for (auto p : dprocessor) ps.add(p->getId());
//        DBG("Direct: " + intArrayToString(ps));
//
//        ps.clear();
//        for (auto p : tprocessor) ps.add(p->getId());
//        DBG("Tuning: " + intArrayToString(ps));
//
//        ps.clear();
//        for (auto p : mprocessor) ps.add(p->getId());
//        DBG("Tempo: " + intArrayToString(ps));
//
//		ps.clear();
//		for (auto p : eprocessor) ps.add(p->getId());
//		DBG("Effect: " + intArrayToString(ps));
//
//        ps.clear();
//        for (auto p : rprocessor) ps.add(p->getId());
//        DBG("Resonance: " + intArrayToString(ps));
    }
    void handleMidiEvent (const MidiMessage& m)
    {
        for (auto proc : processors)
        {
            for (auto p : *proc)
            {
                p->handleMidiEvent(m);
            }
        }
    }
   void prepareToPlay(GeneralSettings::Ptr g)
   {
       for (auto proc : processors)
       {
           for (auto p : *proc)
           {
               p->prepareToPlay(g);
           }
       }
     
//       for (auto d : eprocessor)
//       {
//           d->handleMidiEvent(m);
//       }
//
//       for (auto s : mprocessor)
//       {
//           s->handleMidiEvent(m);
//       }
//
//       for (auto n : tprocessor)
//       {
//           n->handleMidiEvent(m);
//       }
     
   }
    //NOT FIN
    void allNotesOff()
    {
        for (auto proc : processors)
        {
            if (proc->getFirst() != nullptr)
            {
                if (proc->getFirst()->getType() < PreparationTypeNostalgic || proc->getFirst()->getType() == PreparationTypeResonance)
                    //proc->allNotesOff();    rprocessor.addIfNotAlreadyThere(p);
                    for (auto p : *proc)
                    {
                        int i =0; //HACK ADD NOTES OFF CODE
                    }
                    return;
            }
        }
    }
    
    void fillVelocities(Array<float> pressTargetVelocities, Array<float> releaseTargetVelocities, KeymapTargetType begin, KeymapTargetType end, float velocity, Keymap::Ptr km, int noteNumber, int mappedFrom, String source, bool &ignoreSustain, GenericProcessor::Ptr proc)
    {
        
            if (km->containsNoteMapping(noteNumber, mappedFrom) && (km->getAllMidiInputIdentifiers().contains(source)))
            {
                for (int i = begin; i <= end; i++)
                {
                    if (km->getTargetStates()[i])
                    {
                        if (km->isInverted())
                        {
                            float v = proc->filterVelocity(jmax(releaseTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity)));
                            releaseTargetVelocities.setUnchecked(i, v);
                        }
                        else
                        {
                            float v = proc->filterVelocity(jmax(pressTargetVelocities.getUnchecked(i),
                                                                km->applyVelocityCurve(velocity)));
                            pressTargetVelocities.setUnchecked(i, v);
                        }
                    }
                }
                if (km->getIgnoreSustain()) ignoreSustain = true;
            }
        
    }
private:
    // Keymaps for this PreparationMap
    Keymap::PtrArr                       keymaps;
    
    // Processors
//    DirectProcessor::PtrArr              dprocessor;
//    SynchronicProcessor::PtrArr          sprocessor;
//    NostalgicProcessor::PtrArr           nprocessor;
//    EffectProcessor::PtrArr              eprocessor;
//    TempoProcessor::PtrArr               mprocessor;
//    TuningProcessor::PtrArr              tprocessor;
//    ResonanceProcessor::PtrArr           rprocessor;
   
    OwnedArray<ReferenceCountedArray<GenericProcessor>> processors;
    // Pointers to synths (flown in from BKAudioProcessor)
//    BKSynthesiser*              synth;
//    BKSynthesiser*              resonanceSynth;
//    BKSynthesiser*              hammerSynth;
    
    bool sustainPedalIsDepressed;
    bool sostenutoPedalIsDepressed;

    Array<Note> sustainedNotes;
    
    
    JUCE_LEAK_DETECTOR(PreparationMap)
};


#endif  // PreparationMap_H_INCLUDED
