/*
  ==============================================================================

    SpringTuning.h
    Created: 3 Aug 2018 3:43:46pm
    Author:  Theo

  ==============================================================================
*/

#pragma once
#include "SpringTuningUtilities.h"

#include "AudioConstants.h"

#include "Particle.h"
#include "Spring.h"

class SpringTuning : public ReferenceCountedObject, private HighResolutionTimer
{
public:
    typedef ReferenceCountedObjectPtr<SpringTuning> Ptr;
    
    SpringTuning(SpringTuning::Ptr st = nullptr);
    ~SpringTuning(){stopTimer();};
	void simulate();
    
    void copy(SpringTuning::Ptr st);

	void toggleSpring();

	void addParticle(int pc);
	void removeParticle(int pc);
    
    Particle* getParticle(int note) { return particleArray[note];}
    
	void addNote(int noteIndex);
	void removeNote(int noteIndex);
    void removeAllNotes(void);
	void toggleNote(int noteIndex);
    
	void updateNotes();
    void updateFreq();
    
   
	void addSpringsByNote(int pc);
	void removeSpringsByNote(int removeIndex);
	void addSpringsByInterval(double interval);
	void removeSpringsByInterval(double interval);
	void adjustSpringsByInterval(double interval, double stiffness);
    
    inline void setRate(double r, bool start = true)
    {
        rate = r;
        if (start)  startTimer(1000 / rate);
        else        stopTimer();
    }
    
    inline double getRate(void)
    {
        return rate;
    }
    
    inline void stop(void)
    {
        stopTimer();
    }
    
    inline void setStiffness(double stiff)
    {
        stiffness = stiff;
        
        for (auto spring : enabledSpringArray)
        {
            spring->setStiffness(stiffness);
        }
        
        for (auto spring : tetherSpringArray)
        {
            spring->setStiffness(stiffness);
        }
    }
    
    inline void setTetherStiffness(double stiff)
    {
        tetherStiffness = stiff;
        for (auto spring : tetherSpringArray)
        {
            spring->setStiffness(tetherStiffness);
        }
    }
    
    inline void setIntervalStiffness(double stiff)
    {
        intervalStiffness = stiff;
        
        for (auto spring : enabledSpringArray)
        {
            spring->setStiffness(intervalStiffness);
        }
    }
    
    inline double getStiffness(void)
    {
        return stiffness;
    }
    
    inline double getTetherStiffness(void)
    {
        return tetherStiffness;
    }
    
    inline double getIntervalStiffness(void)
    {
        return intervalStiffness;
    }
    
    inline void setDrag(double newdrag)
    {
        drag = newdrag;
    }

    inline double getDrag(void)
    {
        return drag;
    }
    
    inline Array<float> getTetherWeights(void)
    {
        Array<float> weights;
        for (auto spring : getTetherSprings())
        {
            weights.add(spring->getStrength());
        }
        
        return weights;
    }
    
    inline void setTetherWeights(Array<float> weights)
    {
        for (int i = 0; i < 128; i++)
        {
            tetherSpringArray[i]->setStrength(weights[i]);
        }
    }
    
    inline Array<float> getSpringWeights(void)
    {
        Array<float> weights;
        
        for (int i = 0; i < 12; i++)
        {
            weights.add(getSpringWeight(i));
        }
        
        return weights;
    }
    
    inline void setSpringWeights(Array<float> weights)
    {
        for (int i = 0; i < 12; i++)
        {
            setSpringWeight(i, weights[i]);
        }
    }
    
    inline void setSpringMode(int which, bool on)
    {
        springMode.set(which, on);
    }
    
    bool getSpringMode(int which) {return springMode.getUnchecked(which);}
    bool getSpringModeButtonState(int which) {return springMode.getUnchecked(which);}
    
	double getFrequency(int index);
	bool pitchEnabled(int index);

	void print();
	void printParticles();
	void printActiveParticles();
	void printActiveSprings();

	bool checkEnabledParticle(int index);
    
    Particle::PtrArr& getTetherParticles(void) { return tetherParticleArray;}
    Spring::PtrArr& getTetherSprings(void) { return tetherSpringArray;}
    
    Particle::PtrArr& getParticles(void) { return particleArray;}
    Spring::PtrArr& getSprings(void) { return springArray;}
    
    Spring::PtrArr& getEnabledSprings(void) { return enabledSpringArray;}
    
    String getTetherSpringName(int which);
    
    String getSpringName(int which);
    
    void setTetherTuning(Array<float> tuning);
    Array<float> getTetherTuning(void) {return tetherTuning;}
    
    void setTetherFundamental(PitchClass  newfundamental);
    int getTetherFundamental(void) {return tetherFundamental;}
    
    void setIntervalTuning(Array<float> tuning);
    Array<float> getIntervalTuning(void){return intervalTuning;}
    
    void setIntervalFundamental(PitchClass  newfundamental)
    {
        intervalFundamental = newfundamental;
        
        if(newfundamental == 12) setUsingFundamentalForIntervalSprings(false);
        else setUsingFundamentalForIntervalSprings(true);
        
        if(newfundamental == 13) useLowestNoteForFundamental = true;
        else useLowestNoteForFundamental = false;
        
        if(newfundamental == 14) useHighestNoteForFundamental = true;
        else useHighestNoteForFundamental = false;
        
        if(newfundamental == 15) useLastNoteForFundamental = true;
        else useLastNoteForFundamental = false;
    }
    
    PitchClass getIntervalFundamental(void) {DBG("getIntervalFundamental " + String(intervalFundamental)); return intervalFundamental;}
    
    void retuneIndividualSpring(Spring::Ptr spring);
    void retuneAllActiveSprings(void);
    
    void setSpringWeight(int which, double weight);
    double getSpringWeight(int which);
    
    void setTetherWeight(int which, double weight);
    double getTetherWeight(int which);
    
    void setUsingFundamentalForIntervalSprings(bool use) { usingFundamentalForIntervalSprings = use; }
    bool getUsingFundamentalForIntervalSprings(void) { return usingFundamentalForIntervalSprings; }
    
    int getLowestActiveParticle();
    int getHighestActiveParticle();
    
    ValueTree getState(void)
    {
        ValueTree prep("springtuning");
        
        prep.setProperty( "rate", rate, 0);
        prep.setProperty( "drag", drag, 0);
        prep.setProperty( "tetherStiffness", tetherStiffness, 0);
        prep.setProperty( "intervalStiffness", intervalStiffness, 0);
        prep.setProperty( "stiffness", stiffness, 0);
        prep.setProperty( "active", active ? 1 : 0, 0);
        prep.setProperty( "intervalTuningId", scaleId, 0);
        prep.setProperty( "intervalFundamental", (int)intervalFundamental, 0);
        //prep.setProperty( "usingFundamentalForIntervalSprings", (int)usingFundamentalForIntervalSprings, 0);
    
        ValueTree tethers( "tethers");
        ValueTree springs( "springs");
        ValueTree intervalScale("intervalScale");
        ValueTree springMode("springMode");
        
        for (int i = 0; i < 128; i++)
        {
            tethers.setProperty( "t"+String(i), getTetherWeight(i), 0 );
        }

        for (int i = 0; i < 12; i++)
        {
            springs.setProperty( "s"+String(i), getSpringWeight(i), 0 );
            intervalScale.setProperty("s"+String(i), intervalTuning[i], 0);
            springMode.setProperty("s"+String(i), getSpringMode(i), 0);
        }
        prep.addChild(tethers, -1, 0);
        prep.addChild(springs, -1, 0);
        prep.addChild(intervalScale, -1, 0);
        prep.addChild(springMode, -1, 0);

        return prep;
    }
    
    inline void randomize()
    {
        Random::getSystemRandom().setSeedRandomly();
        
        double r[200];
        
        for (int i = 0; i < 200; i++)  r[i] = (Random::getSystemRandom().nextDouble());
        int idx = 0;
        
        rate = r[idx++];
        drag = r[idx++];
        tetherStiffness = r[idx++];
        intervalStiffness = r[idx++];
        stiffness = r[idx++];
        active = r[idx++];
        scaleId = (TuningSystem) (int) (r[idx++] * TuningSystemNil);
        intervalFundamental = (PitchClass) (int) (r[idx++] * PitchClassNil);
        
        for (int i = 0; i < 128; i++)
        {
            setTetherWeight(i, r[idx++]);
        }
        
        for (int i = 0; i < 12; i++)
        {
            setSpringWeight(i, r[idx++]);
            intervalTuning.setUnchecked(i, r[idx++] - 0.5);
            setSpringMode(i, r[idx++]);
        }
    }
    
    void setState(XmlElement* e)
    {
        active = (bool) e->getStringAttribute("active").getIntValue();
        
        setRate(e->getStringAttribute("rate").getDoubleValue());
        setDrag(e->getStringAttribute("drag").getDoubleValue());
        
        setStiffness(e->getStringAttribute("stiffness").getDoubleValue());
        setTetherStiffness(e->getStringAttribute("tetherStiffness").getDoubleValue());
        setIntervalStiffness(e->getStringAttribute("intervalStiffness").getDoubleValue());
        
        scaleId = (TuningSystem) e->getStringAttribute("intervalTuningId").getIntValue();
        setIntervalFundamental((PitchClass) e->getStringAttribute("intervalFundamental").getIntValue());
        DBG("getIntervalFundamental = " + String(getIntervalFundamental()));
        //usingFundamentalForIntervalSprings = (bool) e->getStringAttribute("usingFundamentalForIntervalSprings").getIntValue();
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName("intervalScale"))
            {
                Array<float> scale;
                for (int i = 0; i < 12; i++)
                {
                    String attr = sub->getStringAttribute("s" + String(i));
                    
                    if (attr == "") scale.add(0.0);
                    else            scale.add(attr.getFloatValue());
                }
                
                setIntervalTuning(scale);
            }
            else if (sub->hasTagName("tethers"))
            {
                Array<float> scale;
                for (int i = 0; i < 128; i++)
                {
                    String attr = sub->getStringAttribute("t" + String(i));
                    
                    if (attr == "")
                    {
                        setTetherWeight(i, 0.2);
                    }
                    else
                    {
                        setTetherWeight(i, attr.getDoubleValue());
                    }
                }
            }
            else if (sub->hasTagName("springs"))
            {
                for (int i = 0; i < 12; i++)
                {
                    String attr = sub->getStringAttribute("s" + String(i));
                    
                    if (attr == "")
                    {
                        setSpringWeight(i, 0.5);
                    }
                    else
                    {
                        setSpringWeight(i, attr.getDoubleValue());
                    }
                }
            }
            else if (sub->hasTagName("springMode"))
            {
                for (int i = 0; i < 12; i++)
                {
                    String attr = sub->getStringAttribute("s" + String(i));
                    
                    if (attr == "")
                    {
                        setSpringMode(i, false);
                    }
                    else
                    {
                        setSpringMode(i, (bool)attr.getIntValue());
                    }
                }
            }
        }
    }
    

    
    inline void setActive(bool status) { active = status; }
    inline bool getActive(void) { return active; }
    
    inline void setScaleId(TuningSystem which) { scaleId = which; }
    inline TuningSystem getScaleId(void) { return scaleId; }
    
    //void setTetherStrength(double strength);
    //double getTetherStrength(void);
    
    //void setIntervalStrength(double strength);
    //double getIntervalStrength(void);
    
private:
    double rate, stiffness;
    
    //double tetherStrength, intervalStrength;
    double tetherStiffness, intervalStiffness;
    double drag;
    
    bool active;
    bool usingFundamentalForIntervalSprings;
    bool useLowestNoteForFundamental;
    bool useHighestNoteForFundamental;
    bool useLastNoteForFundamental;
    //int intervalSpringsFundamental;
    
    TuningSystem scaleId;
    Array<float> intervalTuning;
    PitchClass intervalFundamental;
    Array<bool> springMode;
    
    Array<float> tetherTuning;
    TuningSystem tetherTuningId;
    PitchClass tetherFundamental;

    Particle::PtrArr    particleArray;
    Spring::PtrArr      springArray; // efficiency fix: make this ordered by spring interval
    
    Particle::PtrArr    tetherParticleArray;
    Spring::PtrArr      tetherSpringArray;
    
    Spring::PtrArr      enabledSpringArray;
    Spring::PtrArr      enabledParticleArray;
    
    float springWeights[13];
    
    void addSpring(Spring::Ptr spring);
    
    /*
    Spring::PtrArr activeTetherSprings;
    Spring::PtrArr activeSprings;
    Particle::PtrArr activeParticles;
    */
    void hiResTimerCallback(void) override
    {
        if (active)
        {
            simulate();
        }
    }
    
    
    
	int numNotes; // number of enabled notes
};
