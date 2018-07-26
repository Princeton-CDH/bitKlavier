/*
  ==============================================================================

    Synchronic.h
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef SYNCHRONIC_H_INCLUDED
#define SYNCHRONIC_H_INCLUDED

#include "BKUtilities.h"
#include "BKSynthesiser.h"
#include "Tuning.h"
#include "Tempo.h"
#include "General.h"
#include "Keymap.h"

class SynchronicPreparation : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicPreparation>   Ptr;
    typedef Array<SynchronicPreparation::Ptr>                  PtrArr;
    typedef Array<SynchronicPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicPreparation>                  Arr;
    typedef OwnedArray<SynchronicPreparation, CriticalSection> CSArr;
    
    // Copy Constructor
    SynchronicPreparation(SynchronicPreparation::Ptr p):
    sNumBeats(p->getNumBeats()),
    sClusterMin(p->getClusterMin()),
    sClusterMax(p->getClusterMax()),
    sClusterCap(p->getClusterCap()),
    sMode(p->getMode()),
    sBeatsToSkip(p->getBeatsToSkip()),
    sBeatMultipliers(p->getBeatMultipliers()),
    sAccentMultipliers(p->getAccentMultipliers()),
    sLengthMultipliers(p->getLengthMultipliers()),
    sTransposition(p->getTransposition()),
    sAttacks(p->getAttacks()),
    sDecays(p->getDecays()),
    sSustains(p->getSustains()),
    sReleases(p->getReleases()),
    envelopeOn(p->getEnvelopesOn()),
    sGain(p->getGain()),
    sClusterThresh(p->getClusterThreshMS()),
    sClusterThreshSec(p->getClusterThreshSEC()),
    sReleaseVelocitySetsSynchronic(p->getReleaseVelocitySetsSynchronic())
    {
        
    }
    
                        
    SynchronicPreparation(int numBeats,
                          int clusterMin,
                          int clusterMax,
                          float clusterThresh,
                          SynchronicSyncMode mode,
                          bool velocityMode,
                          int beatsToSkip,
                          Array<float> beatMultipliers,
                          Array<float> accentMultipliers,
                          Array<float> lengthMultipliers,
                          Array<Array<float>> transp):
    sNumBeats(numBeats),
    sClusterMin(clusterMin),
    sClusterMax(clusterMax),
    sMode(mode),
    sBeatsToSkip(beatsToSkip),
    sBeatMultipliers(beatMultipliers),
    sAccentMultipliers(accentMultipliers),
    sLengthMultipliers(lengthMultipliers),
    sTransposition(transp),
    sClusterThresh(clusterThresh),
    sClusterThreshSec(.001 * sClusterThresh),
    sReleaseVelocitySetsSynchronic(velocityMode)
    {
    }

    
    SynchronicPreparation(void):
    sNumBeats(20),
    sClusterMin(1),
    sClusterMax(12),
    sClusterCap(8), //8 in original bK, but behavior is different here?
    sMode(FirstNoteOnSync),
    sBeatsToSkip(0),
    sBeatMultipliers(Array<float>({1.0})),
    sAccentMultipliers(Array<float>({1.0})),
    sLengthMultipliers(Array<float>({1.0})),
    sAttacks(Array<int>({3,3,3,3,3,3,3,3,3,3,3,3})),
    sDecays(Array<int>({3,3,3,3,3,3,3,3,3,3,3,3})),
    sSustains(Array<float>({1.,1,1,1,1,1,1,1,1,1,1,1})),
    sReleases(Array<int>({30,30,30,30,30,30,30,30,30,30,30,30})),
    envelopeOn(Array<bool>({true,false,false,false,false,false,false,false,false,false,false,false})),
    sGain(1.0),
    sClusterThresh(500),
    sClusterThreshSec(.001 * sClusterThresh)
    {
        sTransposition.ensureStorageAllocated(1);
        sTransposition.add(Array<float>({0.0}));
    }
    
    inline void copy(SynchronicPreparation::Ptr s)
    {
        sNumBeats = s->getNumBeats();
        sClusterMin = s->getClusterMin();
        sClusterMax = s->getClusterMax();
        sClusterCap = s->getClusterCap();
        sMode = s->getMode();
        sBeatsToSkip = s->getBeatsToSkip();
        sBeatMultipliers = s->getBeatMultipliers();
        sAccentMultipliers = s->getAccentMultipliers();
        sLengthMultipliers = s->getLengthMultipliers();
        sGain = s->getGain();
        sTransposition = s->getTransposition();
        sClusterThresh = s->getClusterThreshMS();
        sClusterThreshSec = s->getClusterThreshSEC();
        sReleaseVelocitySetsSynchronic = s->getReleaseVelocitySetsSynchronic();
        
        sAttacks = s->getAttacks();
        sDecays = s->getDecays();
        sSustains = s->getSustains();
        sReleases = s->getReleases();
        
        envelopeOn = s->getEnvelopesOn();
    }
    
    bool compare(SynchronicPreparation::Ptr s)
    {
        bool lens = true;
        bool accents = true;
        bool beats = true;
        bool transp = true;
        bool attack = true;
        bool decay = true;
        bool sustain = true;
        bool release = true;
        bool envelope = true;
        
        for (int i = s->getLengthMultipliers().size(); --i>=0;)
        {
            if (s->getLengthMultipliers()[i] != sLengthMultipliers[i])
            {
                lens = false;
                break;
                
            }
        }
        
        for (int i = s->getAccentMultipliers().size(); --i>=0;)
        {
            if (s->getAccentMultipliers()[i] != sAccentMultipliers[i])
            {
                accents = false;
                break;
                
            }
        }
        
        for (int i = s->getBeatMultipliers().size(); --i>=0;)
        {
            if (s->getBeatMultipliers()[i] != sBeatMultipliers[i])
            {
                beats = false;
                break;
                
            }
        }
        
        for (int i  = s->getTransposition().size(); --i >= 0;)
        {
            Array<float> transposition = s->getTransposition()[i];
            for (int j = transposition.size(); --j >= 0;)
            {
                if (transposition[j] != sTransposition[i][j])
                {
                    transp = false;
                    break;
                    
                }
            }
        }
        
        for (int i = s->getAttacks().size(); --i>=0;)
        {
            if (s->getAttacks()[i] != sAttacks[i])
            {
                attack = false;
                break;
            }
        }
        
        for (int i = s->getDecays().size(); --i>=0;)
        {
            if (s->getDecays()[i] != sDecays[i])
            {
                decay = false;
                break;
            }
        }
        
        for (int i = s->getSustains().size(); --i>=0;)
        {
            if (s->getSustains()[i] != sSustains[i])
            {
                sustain = false;
                break;
            }
        }
        
        for (int i = s->getReleases().size(); --i>=0;)
        {
            if (s->getReleases()[i] != sReleases[i])
            {
                release = false;
                break;
            }
        }
        
        for (int i = s->getEnvelopesOn().size(); --i>=0;)
        {
            if (s->getEnvelopesOn()[i] != envelopeOn[i])
            {
                envelope = false;
                break;
            }
        }
        
        return (sNumBeats == s->getNumBeats() &&
                sClusterMin == s->getClusterMin() &&
                sClusterMax == s->getClusterMax() &&
                sClusterCap == s->getClusterCap() &&
                (sMode == s->getMode()) &&
                transp && lens && accents && beats && attack && decay && sustain && release &&
                sGain == s->getGain() &&
                sClusterThresh == s->getClusterThreshMS() &&
                sClusterThreshSec == s->getClusterThreshSEC() &&
                sReleaseVelocitySetsSynchronic == s->getReleaseVelocitySetsSynchronic());
    }
    
    //inline const float getTempo() const noexcept                       {return sTempo;               }
    inline const int getNumBeats() const noexcept                      {return sNumBeats;              }
    inline const int getClusterMin() const noexcept                    {return sClusterMin;            }
    inline const int getClusterMax() const noexcept                    {return sClusterMax;            }
    inline const int getClusterCap() const noexcept                    {return sClusterCap;            }
    inline const float getClusterThreshSEC() const noexcept            {return sClusterThreshSec;      }
    inline const float getClusterThreshMS() const noexcept             {return sClusterThresh;         }
    inline const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    inline const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    inline const int getBeatsToSkip() const noexcept                   {return sBeatsToSkip;           }
    inline const int getOffsetParamToggle() const noexcept
    {
        if(getMode() == FirstNoteOnSync || getMode() == AnyNoteOnSync) return getBeatsToSkip() + 1;
        else return getBeatsToSkip();
    }
    
    inline const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    inline const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    inline const Array<Array<float>> getTransposition() const noexcept {return sTransposition;         }
    inline const bool getReleaseVelocitySetsSynchronic() const noexcept{return sReleaseVelocitySetsSynchronic; }
    inline const float getGain() const noexcept                        {return sGain;                   }
    
    inline const Array<int> getAttacks() const noexcept         {return sAttacks;   }
    inline const Array<int> getDecays() const noexcept          {return sDecays;    }
    inline const Array<float> getSustains() const noexcept      {return sSustains;  }
    inline const Array<int> getReleases() const noexcept        {return sReleases;  }
    inline const Array<bool> getEnvelopesOn() const noexcept    {return envelopeOn; }
    
    inline const int getAttack(int which) const noexcept    {return sAttacks[which];}
    inline const int getDecay(int which) const noexcept     {return sDecays[which];}
    inline const float getSustain(int which) const noexcept {return sSustains[which];}
    inline const int getRelease(int which) const noexcept   {return sReleases[which];}
    
    inline const Array<Array<float>> getADSRs() const noexcept
    {
        Array<Array<float>> allADSRs;
        for(int i=0; i<sAttacks.size(); i++)
        {
            Array<float> singleADSR;
            singleADSR.insert(0, sAttacks[i]);
            singleADSR.insert(1, sDecays[i]);
            singleADSR.insert(2, sSustains[i]);
            singleADSR.insert(3, sReleases[i]);
            if(envelopeOn[i])singleADSR.insert(4, 1);
            else singleADSR.insert(4, 0);
            
            allADSRs.insert(i, singleADSR);
        }
        
        return allADSRs;
    }
    
    inline const bool getEnvelopeOn(int which) const noexcept   {return envelopeOn[which];}
    
    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = sClusterThresh * .001;
    }
    
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline void setNumBeats(int numBeats)                              {sNumBeats = numBeats;                              }
    inline void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    inline void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    inline void setClusterCap(int clusterCap)                          {sClusterCap = clusterCap;                          }
    inline void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    inline void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    inline void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    inline void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    inline void setTransposition(Array<Array<float>> transp)           {sTransposition.swapWith(transp);                   }
    inline void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    
    inline void setBeatMultiplier(int whichSlider, float value)        {sBeatMultipliers.set(whichSlider, value);           }
    inline void setAccentMultiplier(int whichSlider, float value)      {sAccentMultipliers.set(whichSlider, value);         }
    inline void setLengthMultiplier(int whichSlider, float value)      {sLengthMultipliers.set(whichSlider, value);         }
    inline void setSingleTransposition(int whichSlider, Array<float> values) {sTransposition.set(whichSlider, values); }
    inline void setReleaseVelocitySetsSynchronic(bool rvss)            {sReleaseVelocitySetsSynchronic = rvss;          }
    inline void setGain(float gain)                                    {sGain = gain;                          }
    
    inline void setAttacks(Array<int> attacks)      {sAttacks.swapWith(attacks);    }
    inline void setDecays(Array<int> decays)        {sDecays.swapWith(decays);      }
    inline void setSustains(Array<float> sustains)  {sSustains.swapWith(sustains);  }
    inline void setReleases(Array<int> releases)    {sReleases.swapWith(releases);  }
    
    inline void setAttack(int which, int val)       {sAttacks.set(which, val);}
    inline void setDecay(int which, int val)        {sDecays.set(which, val);}
    inline void setSustain(int which, float val)    {sSustains.set(which, val);}
    inline void setRelease(int which, int val)      {sReleases.set(which, val);}
    
    inline void setADSRs(Array<Array<float>> allADSRs)
    {
        for(int i=0; i<allADSRs.size(); i++)
        {
            setAttack(i, allADSRs[i][0]);
            setDecay(i, allADSRs[i][1]);
            setSustain(i, allADSRs[i][2]);
            setRelease(i, allADSRs[i][3]);
            if(allADSRs[i][4] > 0 || i==0) setEnvelopeOn(i, true);
            else setEnvelopeOn(i, false);
            DBG("ADSR envelopeOn = " + String(i) + " " + String((int)getEnvelopeOn(i)));
        }
    }
    
    inline void setADSR(int which, Array<float> oneADSR)
    {
        setAttack(which, oneADSR[0]);
        setDecay(which, oneADSR[1]);
        setSustain(which, oneADSR[2]);
        setRelease(which, oneADSR[3]);
        if(oneADSR[4] > 0 || which==0) setEnvelopeOn(which, true);
        else setEnvelopeOn(which, false);
        DBG("ADSR envelopeOn = " + String(which) + " " + String((int)getEnvelopeOn(which)));
        
    }
    
    inline void setEnvelopeOn(int which, bool val)  {envelopeOn.set(which, val);}
    
    void print(void)
    {
        DBG("| - - - Synchronic Preparation - - - |");
        DBG("sTempo: " + String(sTempo));
        DBG("sNumBeats: " + String(sNumBeats));
        DBG("sClusterMin: " + String(sClusterMin));
        DBG("sClusterMax: " + String(sClusterMax));
        DBG("sClusterCap: " + String(sClusterCap));
        DBG("sClusterThresh: " + String(sClusterThresh));
        DBG("sReleaseVelocitySetsSynchronic: " + String((int)sReleaseVelocitySetsSynchronic));
        DBG("sMode: " + String(sMode));
        DBG("sBeatsToSkip: " + String(sBeatsToSkip));
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers));
        
        String s = "";
        for (auto arr : sTransposition) s += "{ " + floatArrayToString(arr) + " },\n";
        DBG("sTransposition: " + s);
        
        DBG("sClusterThreshSec: " + String(sClusterThreshSec));
        //DBG("resetKeymap: " + intArrayToString(getResetMap()->keys()));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
private:
    String name;
    float sTempo;
    int sNumBeats,sClusterMin,sClusterMax;
    int sClusterCap = 8; //max in cluster; 8 in original bK. pulseDepth?
    
    SynchronicSyncMode sMode;
    int sBeatsToSkip;
    
    Array<float> sBeatMultipliers;      //multiply pulse lengths by these
    Array<float> sAccentMultipliers;    //multiply velocities by these
    Array<float> sLengthMultipliers;    //multiply note duration by these
    Array<Array<float>> sTransposition; //transpose by these
    
    Array<int> sAttacks;
    Array<int> sDecays;
    Array<float> sSustains;
    Array<int> sReleases;
    Array<bool> envelopeOn;
    

    float sGain;               //gain multiplier
    float sClusterThresh;      //max time between played notes before new cluster is started, in MS
    float sClusterThreshSec;
    
    bool sReleaseVelocitySetsSynchronic;

    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SYNCHRONIC ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

class Synchronic : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Synchronic>   Ptr;
    typedef Array<Synchronic::Ptr>                  PtrArr;
    typedef Array<Synchronic::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Synchronic>                  Arr;
    typedef OwnedArray<Synchronic, CriticalSection> CSArr;
    
    Synchronic(SynchronicPreparation::Ptr prep,
               int Id):
    sPrep(new SynchronicPreparation(prep)),
    aPrep(new SynchronicPreparation(sPrep)),
    Id(Id),
    name(String(Id))
    {
        
    }
    
    Synchronic(int Id):
    Id(Id),
    name(String(Id))
    {
        sPrep       = new SynchronicPreparation();
        aPrep       = new SynchronicPreparation(sPrep);
    }
    
    inline Synchronic::Ptr duplicate()
    {
        SynchronicPreparation::Ptr copyPrep = new SynchronicPreparation(sPrep);
        
        Synchronic::Ptr copy = new Synchronic(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    inline void clear(void)
    {
        sPrep       = new SynchronicPreparation();
        aPrep       = new SynchronicPreparation(sPrep);
    }

    inline void copy(Synchronic::Ptr from)
    {
        sPrep->copy(from->sPrep);
        aPrep->copy(sPrep);
    }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagSynchronic);
        
        prep.setProperty( "Id",Id, 0);
        prep.setProperty( "name", name, 0);
        prep.setProperty( "gain", sPrep->getGain(), 0);
        prep.setProperty( ptagSynchronic_numBeats,            sPrep->getNumBeats(), 0);
        prep.setProperty( ptagSynchronic_clusterMin,          sPrep->getClusterMin(), 0);
        prep.setProperty( ptagSynchronic_clusterMax,          sPrep->getClusterMax(), 0);
        prep.setProperty( ptagSynchronic_clusterThresh,       sPrep->getClusterThreshMS(), 0);
        prep.setProperty( ptagSynchronic_mode,                sPrep->getMode(), 0);
        prep.setProperty( ptagSynchronic_beatsToSkip,         sPrep->getBeatsToSkip(), 0);
        
        ValueTree beatMults( vtagSynchronic_beatMults);
        int count = 0;
        for (auto f : sPrep->getBeatMultipliers())
        {
            beatMults.      setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(beatMults, -1, 0);
        
        
        ValueTree lenMults( vtagSynchronic_lengthMults);
        count = 0;
        for (auto f : sPrep->getLengthMultipliers())
        {
            lenMults.       setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(lenMults, -1, 0);
        
        
        ValueTree accentMults( vtagSynchronic_accentMults);
        count = 0;
        for (auto f : sPrep->getAccentMultipliers())
        {
            accentMults.    setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(accentMults, -1, 0);
        
        
        ValueTree transposition( vtagSynchronic_transpOffsets);
        
        int tcount = 0;
        for (auto arr : sPrep->getTransposition())
        {
            ValueTree t("t"+String(tcount++));
            count = 0;
            for (auto f : arr)  t.setProperty( ptagFloat + String(count++), f, 0);
            transposition.addChild(t,-1,0);
        }
        prep.addChild(transposition, -1, 0);
        
        ValueTree ADSRs( vtagSynchronic_ADSRs);
        
        tcount = 0;
        for (auto arr : sPrep->getADSRs())
        {
            ValueTree e("e"+String(tcount++));
            count = 0;
            for (auto f : arr)  e.setProperty( ptagFloat + String(count++), f, 0);
            ADSRs.addChild(e,-1,0);
        }
        prep.addChild(ADSRs, -1, 0);
        
        return prep;
        
    }
    
    inline void setState(XmlElement* e, Tuning::PtrArr tuning, Tempo::PtrArr tempo)
    {
        int i; float f;
        
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);
        
        n = e->getStringAttribute("gain");
        if (n != "") sPrep->setGain(n.getFloatValue());
        else         sPrep->setGain(1.0);
        
        i = e->getStringAttribute(ptagSynchronic_numBeats).getIntValue();
        sPrep->setNumBeats(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterMin).getIntValue();
        sPrep->setClusterMin(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterMax).getIntValue();
        sPrep->setClusterMax(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterThresh).getIntValue();
        sPrep->setClusterThresh(i);
        
        i = e->getStringAttribute(ptagSynchronic_mode).getIntValue();
        sPrep->setMode((SynchronicSyncMode) i);
        
        i = e->getStringAttribute(ptagSynchronic_beatsToSkip).getIntValue();
        sPrep->setBeatsToSkip(i);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagSynchronic_beatMults))
            {
                Array<float> beats;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        beats.add(f);
                    }
                }
                
                sPrep->setBeatMultipliers(beats);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_accentMults))
            {
                Array<float> accents;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        accents.add(f);
                    }
                }
                
                sPrep->setAccentMultipliers(accents);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_lengthMults))
            {
                Array<float> lens;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        lens.add(f);
                    }
                }
                
                sPrep->setLengthMultipliers(lens);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
            {
                Array<Array<float>> atransp;
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("t"+String(tcount++)))
                    {
                        Array<float> transp;
                        for (int k = 0; k < 128; k++)
                        {
                            String attr = asub->getStringAttribute(ptagFloat + String(k));
                            
                            if (attr == String::empty) break;
                            else
                            {
                                f = attr.getFloatValue();
                                transp.add(f);
                            }
                        }
                        atransp.set(tcount-1, transp);
                    }
                }
                
                sPrep->setTransposition(atransp);
            }
            else  if (sub->hasTagName(vtagSynchronic_ADSRs))
            {
                Array<Array<float>> aADSRs;
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("e"+String(tcount++)))
                    {
                        Array<float> singleADSR;
                        for (int k = 0; k < 5; k++)
                        {
                            String attr = asub->getStringAttribute(ptagFloat + String(k));
                            
                            if (attr == String::empty) break;
                            else
                            {
                                f = attr.getFloatValue();
                                singleADSR.add(f);
                            }
                        }
                        aADSRs.set(tcount-1, singleADSR);
                    }
                }
                
                sPrep->setADSRs(aADSRs);
            }
        }
        
        aPrep->copy(sPrep);
    }
    
    ~Synchronic() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    SynchronicPreparation::Ptr      sPrep;
    SynchronicPreparation::Ptr      aPrep;

    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
    }
    
private:
    int Id;
    String name;
    
    
    JUCE_LEAK_DETECTOR(Synchronic)
};

class SynchronicModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<SynchronicModPreparation>   Ptr;
    typedef Array<SynchronicModPreparation::Ptr>                  PtrArr;
    typedef Array<SynchronicModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicModPreparation>                  Arr;
    typedef OwnedArray<SynchronicModPreparation, CriticalSection> CSArr;
    
    SynchronicModPreparation(SynchronicPreparation::Ptr p, int Id):
    Id(Id)
    {
        param.ensureStorageAllocated((int)cSynchronicParameterTypes.size());
        
        param.set(SynchronicNumPulses, String(p->getNumBeats()));
        param.set(SynchronicClusterMin, String(p->getClusterMin()));
        param.set(SynchronicClusterMax, String(p->getClusterMax()));
        param.set(SynchronicClusterThresh, String(p->getClusterThreshMS()));
        param.set(SynchronicMode, String(p->getMode()));
        param.set(SynchronicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(SynchronicTranspOffsets, arrayFloatArrayToString(p->getTransposition()));
        param.set(SynchronicAccentMultipliers, floatArrayToString(p->getAccentMultipliers()));
        param.set(SynchronicLengthMultipliers, floatArrayToString(p->getLengthMultipliers()));
        param.set(SynchronicBeatMultipliers, floatArrayToString(p->getBeatMultipliers()));
        param.set(SynchronicGain, String(p->getGain()));
        param.set(SynchronicADSRs, arrayFloatArrayToString(p->getADSRs()));
        
    }
    
    
    SynchronicModPreparation(int Id):
    Id(Id)
    {
        param.set(SynchronicNumPulses, "");
        param.set(SynchronicClusterMin, "");
        param.set(SynchronicClusterMax, "");
        param.set(SynchronicClusterThresh, "");
        param.set(SynchronicMode, "");
        param.set(SynchronicBeatsToSkip, "");
        param.set(SynchronicTranspOffsets, "");
        param.set(SynchronicAccentMultipliers, "");
        param.set(SynchronicLengthMultipliers, "");
        param.set(SynchronicBeatMultipliers, "");
        param.set(SynchronicGain, "");
        param.set(SynchronicADSRs, "");

    }
    
    inline SynchronicModPreparation::Ptr duplicate(void)
    {
        SynchronicModPreparation::Ptr copyPrep = new SynchronicModPreparation(-1);
        
        copyPrep->copy(this);
        
        copyPrep->setName(this->getName());
        
        return copyPrep;
    }
    
    inline void setId(int newId) { Id = newId; }
    inline int getId(void) const noexcept { return Id; }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagModSynchronic );
        
        prep.setProperty( "Id",Id, 0);
        
        String p = "";
        
        p = getParam(SynchronicNumPulses);
        if (p != String::empty) prep.setProperty( ptagSynchronic_numBeats,            p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterMin);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterMin,          p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterMax);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterMax,          p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterThresh);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterThresh,       p.getIntValue(), 0);
        
        p = getParam(SynchronicMode);
        if (p != String::empty) prep.setProperty( ptagSynchronic_mode,                p.getIntValue(), 0);
        
        p = getParam(SynchronicBeatsToSkip);
        if (p != String::empty) prep.setProperty( ptagSynchronic_beatsToSkip,         p.getIntValue(), 0);
        
        p = getParam(SynchronicGain);
        if (p != String::empty) prep.setProperty( ptagSynchronic_gain,                p.getFloatValue(), 0);
        
        ValueTree beatMults( vtagSynchronic_beatMults);
        int count = 0;
        p = getParam(SynchronicBeatMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                beatMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(beatMults, -1, 0);
        
        
        ValueTree lengthMults( vtagSynchronic_lengthMults);
        count = 0;
        p = getParam(SynchronicLengthMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                lengthMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(lengthMults, -1, 0);
        
        
        ValueTree accentMults( vtagSynchronic_accentMults);
        count = 0;
        p = getParam(SynchronicAccentMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                accentMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(accentMults, -1, 0);
 
        
        ValueTree transpOffsets( vtagSynchronic_transpOffsets);
        int tcount = 0;
        p = getParam(SynchronicTranspOffsets);
        if (p != String::empty)
        {
            Array<Array<float>> m = stringToArrayFloatArray(p);
            for (auto f : m)
            {
                ValueTree t("t"+String(tcount++));
                count = 0;
                for (auto nf : f) t.setProperty( ptagFloat + String(count++), nf, 0);
                transpOffsets.addChild(t,-1,0);
            }
        }
        prep.addChild(transpOffsets, -1, 0);
        
        
        ValueTree envelopes( vtagSynchronic_ADSRs);
        tcount = 0;
        p = getParam(SynchronicADSRs);
        if (p != String::empty)
        {
            Array<Array<float>> m = stringToArrayFloatArray(p);
            for (auto f : m)
            {
                ValueTree e("e"+String(tcount++));
                count = 0;
                for (auto nf : f) e.setProperty( ptagFloat + String(count++), nf, 0);
                envelopes.addChild(e,-1,0);
            }
        }
        prep.addChild(envelopes, -1, 0);
        
        
        return prep;
        
    }
    
    inline void setState(XmlElement* e)
    {
        float f;
        
        Id = e->getStringAttribute("Id").getIntValue();
        
        String p = e->getStringAttribute(ptagSynchronic_numBeats);
        setParam(SynchronicNumPulses, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterMin);
        setParam(SynchronicClusterMin, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterMax);
        setParam(SynchronicClusterMax, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterThresh);
        setParam(SynchronicClusterThresh, p);
        
        p = e->getStringAttribute(ptagSynchronic_mode);
        setParam(SynchronicMode, p);
        
        p = e->getStringAttribute(ptagSynchronic_gain);
        setParam(SynchronicGain, p);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagSynchronic_beatMults))
            {
                Array<float> beats;
                
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        beats.add(f);
                    }
                }
                
                setParam(SynchronicBeatMultipliers, floatArrayToString(beats));
                
            }
            else  if (sub->hasTagName(vtagSynchronic_accentMults))
            {
                Array<float> accents;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        accents.add(f);
                    }
                }
                
                setParam(SynchronicAccentMultipliers, floatArrayToString(accents));
                
            }
            else  if (sub->hasTagName(vtagSynchronic_lengthMults))
            {
                Array<float> lens;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        lens.add(f);
                    }
                }
                
                setParam(SynchronicLengthMultipliers, floatArrayToString(lens));
                
            }
            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
            {
                Array<Array<float>> atransp;
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("t"+String(tcount++)))
                    {
                        Array<float> transp;
                        for (int k = 0; k < 128; k++)
                        {
                            String attr = asub->getStringAttribute(ptagFloat + String(k));
                            
                            if (attr == String::empty) break;
                            else
                            {
                                f = attr.getFloatValue();
                                transp.add(f);
                            }
                        }
                        atransp.set(tcount-1, transp);
                    }
                }
                
                setParam(SynchronicTranspOffsets, arrayFloatArrayToString(atransp));
                
            }
            else  if (sub->hasTagName(vtagSynchronic_ADSRs))
            {
                Array<Array<float>> aenvs;
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("e"+String(tcount++)))
                    {
                        Array<float> envs;
                        for (int k = 0; k < 128; k++)
                        {
                            String attr = asub->getStringAttribute(ptagFloat + String(k));
                            
                            if (attr == String::empty) break;
                            else
                            {
                                f = attr.getFloatValue();
                                envs.add(f);
                            }
                        }
                        aenvs.set(tcount-1, envs);
                    }
                }
                
                setParam(SynchronicADSRs, arrayFloatArrayToString(aenvs));
            }
        }
        
    }
    
    
    ~SynchronicModPreparation(void)
    {
        
    }
    
    inline void copy(SynchronicPreparation::Ptr p)
    {
        param.set(SynchronicNumPulses, String(p->getNumBeats()));
        param.set(SynchronicClusterMin, String(p->getClusterMin()));
        param.set(SynchronicClusterMax, String(p->getClusterMax()));
        param.set(SynchronicClusterThresh, String(p->getClusterThreshMS()));
        param.set(SynchronicGain, String(p->getGain()));
        param.set(SynchronicMode, String(p->getMode()));
        param.set(SynchronicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(SynchronicBeatMultipliers, floatArrayToString(p->getBeatMultipliers()));
        param.set(SynchronicLengthMultipliers, floatArrayToString(p->getLengthMultipliers()));
        param.set(SynchronicAccentMultipliers, floatArrayToString(p->getAccentMultipliers()));
        param.set(SynchronicTranspOffsets, arrayFloatArrayToString(p->getTransposition()));
        param.set(SynchronicADSRs, arrayFloatArrayToString(p->getADSRs()));
    }
    
    inline void copy(SynchronicModPreparation::Ptr p)
    {
        for (int i = SynchronicId+1; i < SynchronicParameterTypeNil; i++)
        {
            param.set(i, p->getParam((SynchronicParameterType)i));
        }
    }
    
    void clearAll()
    {
        for (int i = SynchronicId+1; i < SynchronicParameterTypeNil; i++)
        {
            param.set(i, "");
        }
    }
    
    
    inline const StringArray getStringArray(void) { return param; }
    
    inline const String getParam(SynchronicParameterType type)
    {
        if (type != SynchronicId)
            return param[type];
        else
            return "";
    }
    
    inline void setParam(SynchronicParameterType type, String val)
    {
        param.set(type, val);
        
        
    }
    
    void print(void)
    {
        
    }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}
    
private:
    int Id;
    String name;
    StringArray          param;
    
    JUCE_LEAK_DETECTOR(SynchronicModPreparation);
};

class SynchronicProcessor  : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicProcessor>   Ptr;
    typedef Array<SynchronicProcessor::Ptr>                  PtrArr;
    typedef Array<SynchronicProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicProcessor>                  Arr;
    typedef OwnedArray<SynchronicProcessor,CriticalSection>  CSArr;
    
    
    SynchronicProcessor(Synchronic::Ptr synchronic,
                        TuningProcessor::Ptr tuning,
                        TempoProcessor::Ptr tempo,
                        BKSynthesiser* main,
                        GeneralSettings::Ptr general);
    
    ~SynchronicProcessor();
    
    void         setCurrentPlaybackSampleRate(double sr);
    inline const uint64 getCurrentNumSamplesBeat(void) const noexcept   { return numSamplesBeat;    }
    inline const uint64 getCurrentPhasor(void) const noexcept           { return phasor;            }
    
    BKSampleLoadType sampleType;
    void processBlock(int numSamples, int midiChannel, BKSampleLoadType type);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, float velocity, int channel);
    float getTimeToBeatMS(float beatsToSkip);
    
    inline const int getBeatMultiplierCounter() const noexcept { return beatMultiplierCounter; }
    inline const int getAccentMultiplierCounter() const noexcept { return accentMultiplierCounter; }
    inline const int getLengthMultiplierCounter() const noexcept { return lengthMultiplierCounter; }
    inline const int getTranspCounter() const noexcept { return transpCounter; }
    inline const int getEnvelopeCounter() const noexcept { return envelopeCounter; }
    inline const int getBeatCounter() const noexcept { return beatCounter; }
    inline const float getClusterThresholdTimer() const noexcept { return 1000. * clusterThresholdTimer / sampleRate ;}
    inline const float getClusterThreshold() const noexcept { return 1000. * clusterThresholdSamples / sampleRate ;}
    inline const int getClusterSize() const noexcept {return cluster.size(); }
    inline const int getNumKeysDepressed() const noexcept {return keysDepressed.size(); }
    inline const bool getPlayCluster() const noexcept { return playCluster; }
    
    inline const SynchronicSyncMode getMode() const noexcept {return synchronic->aPrep->getMode(); }

    inline int getId(void) const noexcept { return synchronic->getId(); }
    
    inline void setSynchronic(Synchronic::Ptr newSynchronic)
    {
        synchronic = newSynchronic;
    }
    
    inline void setTuning(TuningProcessor::Ptr tuning)
    {
        tuner = tuning;
    }
    
    inline void setTempo(TempoProcessor::Ptr newTempo)
    {
        tempo = newTempo;
    }
    
    inline Synchronic::Ptr getSynchronic(void) const noexcept
    {
        return synchronic;
    }
    
    inline TuningProcessor::Ptr getTuning(void) const noexcept
    {
        return tuner;
    }
    
    inline TempoProcessor::Ptr getTempo(void) const noexcept
    {
        return tempo;
    }
    
    inline int getTuningId(void) const noexcept
    {
        return tuner->getId();
    }
    
    inline int getTempoId(void) const noexcept
    {
        return tempo->getId();
    }

    inline void prepareToPlay(float sr, BKSynthesiser* main)
    {
        synth = main;
        sampleRate = sr;
    }
    //void  atReset();
    
    inline void reset(void)
    {
        synchronic->aPrep->copy(synchronic->sPrep);
    }
    
    void clearOldNotes()
    {
        keysDepressed.clearQuick();
        cluster.clearQuick();
    }
    
private:
    BKSynthesiser* synth;
    GeneralSettings::Ptr general;
    
    Synchronic::Ptr synchronic;
    TuningProcessor::Ptr tuner;
    TempoProcessor::Ptr tempo;
    
    double sampleRate;

    
    Array<float> tuningOffsets;
    PitchClass tuningBasePitch;
    
    int beatCounter;  //beat (or pulse) counter; max set by users -- sNumBeats
    
    //parameter field counters
    int beatMultiplierCounter;   //beat length (time between beats) multipliers
    int accentMultiplierCounter; //accent multipliers
    int lengthMultiplierCounter; //note length (sounding length) multipliers (multiples of 50ms, at least for now)
    int transpCounter;     //transposition offsets
    int envelopeCounter;
    
    //reset the phase, including of all the parameter fields
    void resetPhase(int skipBeats);
    
    
    
    void playNote(int channel, int note, float velocity);
    Array<float> velocities;    //record of velocities
    Array<int> keysDepressed;   //current keys that are depressed
    bool playCluster;
    
    bool inCluster;
    uint64 clusterThresholdSamples;
    uint64 clusterThresholdTimer;
    uint64 clusterTimer;
    Array<int> cluster;         //cluster of notes played, with repetitions, limited to totalClusters (8?)
    Array<int> slimCluster;     //cluster without repetitions
    
    uint64 phasor;
    uint64 numSamplesBeat;          // = beatThresholdSamples * beatMultiplier
    uint64 beatThresholdSamples;    // # samples in a beat, as set by tempo
    
    //adaptive tempo stuff
    /*
     uint64 atTimer, atLastTime; //in samples
     int atDelta;                //in ms
     Array<int> atDeltaHistory;  //in ms
     void atNewNote();
     void atNewNoteOff();
     void atCalculatePeriodMultiplier();
     float adaptiveTempoPeriodMultiplier;
     */
    
    bool shouldPlay;
    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};


#endif  // SYNCHRONIC_H_INCLUDED
