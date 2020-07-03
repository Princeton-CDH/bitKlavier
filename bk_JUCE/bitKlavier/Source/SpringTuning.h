/*
  ==============================================================================

    SpringTuning.h
    Created: 3 Aug 2018 3:43:46pm
    Author:  Theo Trevisan, Mike Mulshine, Dan Trueman
    
    Based on the Verlet mass/spring algorithm:
    Jakobsen, T. (2001). Advanced character physics.
        In IN PROCEEDINGS OF THE GAME DEVELOPERS CONFERENCE 2001, page 19.
 
    in bitKlavier, we use two arrays of springs:
        * the interval springs, which connect all active notes to one another
            at rest lengths given by whatever interval tuning system is being used
        * the tether springs, which connect all active notes to an array of "anchor"
            notes, set by an "anchor" scale (usually ET), to prevent uncontrolled drift
 
    The user sets the strengths of all these strings, either directly, or using some
    automated algorithms, and the system runs, adding and subtracting notes/springs
    from these arrays as they are played.
 
    For more information, see:
        "Playing with Tuning in bitKlavier"
        Trueman, Bhatia, Mulshine, Trevisan
        Computer Music Journal, 2020
    
    The main method is simulate(), which is called repeatedly in a timer()

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
    
    /*
     simulate() first moves through the entire particle array and "integrates" their position,
     moving them based on their "velocities" and the drag values
     
     it then moves through both spring arrays (the tether springs and interval springs) and
     calls satisfyConstraints(), which updates the spring values based on the spring strengths,
     stiffnesses, and offsets from their rest lengths. This in turn updates the target positions
     for the two particles associated with each spring.
     */
	void simulate();
    
    void performModification(SpringTuning::Ptr st, Array<bool> dirty);
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
    inline double getRate(void) { return rate; }
    inline void stop(void) { stopTimer(); }
    
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
    
    inline double getStiffness(void) { return stiffness; }
    inline double getTetherStiffness(void) { return tetherStiffness; }
    inline double getIntervalStiffness(void) { return intervalStiffness; }
    inline void setDrag(double newdrag) { drag = newdrag; }
    inline double getDrag(void) { return drag; }
    
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
    
    bool getFundamentalSetsTether() { return fundamentalSetsTether; }
    void setFundamentalSetsTether(bool s) { fundamentalSetsTether = s; }
    
    double getTetherWeightGlobal() { return tetherWeightGlobal; }
    double getTetherWeightSecondaryGlobal() { return tetherWeightSecondaryGlobal; }
    void setTetherWeightGlobal(double s) { tetherWeightGlobal = s; } // DBG("setTetherWeightGlobal :" + String(tetherWeightGlobal)); }
    void setTetherWeightSecondaryGlobal(double s) { tetherWeightSecondaryGlobal = s; }
    
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
    PitchClass getTetherFundamental(void) {return tetherFundamental;}
    
    void setIntervalTuning(Array<float> tuning);
    Array<float> getIntervalTuning(void){return intervalTuning;}
    
    void setIntervalFundamental(PitchClass  newfundamental)
    {
        intervalFundamental = newfundamental;
        
        if(newfundamental < 12) intervalFundamentalActive = newfundamental;
        
        // DBG("setIntervalFundamental " + String(newfundamental));
        
        if(newfundamental == 12) setUsingFundamentalForIntervalSprings(false);
        else setUsingFundamentalForIntervalSprings(true);
        
        if(newfundamental == 13) useLowestNoteForFundamental = true;
        else useLowestNoteForFundamental = false;
        
        if(newfundamental == 14) useHighestNoteForFundamental = true;
        else useHighestNoteForFundamental = false;
        
        if(newfundamental == 15) useLastNoteForFundamental = true;
        else useLastNoteForFundamental = false;
        
        if(newfundamental == 16) useAutomaticFundamental = true;
        else useAutomaticFundamental = false;
        
        //setTetherFundamental(newfundamental); //when  == fundamentalSetsTether true, tetherFundamental will be used to set tether weights
        setTetherFundamental(intervalFundamentalActive);
    }
    
    PitchClass getIntervalFundamental(void) { return intervalFundamental; }
    PitchClass getIntervalFundamentalActive(void) { return intervalFundamentalActive; }
    
    void findFundamental();
    
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
        prep.setProperty( "fundamentalSetsTether", (int)fundamentalSetsTether, 0);
        prep.setProperty( "tetherWeightGlobal", tetherWeightGlobal, 0);
        prep.setProperty( "tetherWeightSecondaryGlobal", tetherWeightSecondaryGlobal, 0);
 
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
            springMode.setProperty("s"+String(i), (int)getSpringMode(i), 0);
        }
        prep.addChild(tethers, -1, 0);
        prep.addChild(springs, -1, 0);
        prep.addChild(intervalScale, -1, 0);
        prep.addChild(springMode, -1, 0);

        return prep;
    }
    
    // for unit-testing
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
        // DBG("SpringTuning::setState called!!");
        active = (bool) e->getStringAttribute("active").getIntValue();
        
        setRate(e->getStringAttribute("rate").getDoubleValue());
        setDrag(e->getStringAttribute("drag").getDoubleValue());
        
        setStiffness(e->getStringAttribute("stiffness").getDoubleValue());
        setTetherStiffness(e->getStringAttribute("tetherStiffness").getDoubleValue());
        setIntervalStiffness(e->getStringAttribute("intervalStiffness").getDoubleValue());
        
        scaleId = (TuningSystem) e->getStringAttribute("intervalTuningId").getIntValue();
        setIntervalFundamental((PitchClass) e->getStringAttribute("intervalFundamental").getIntValue());

        fundamentalSetsTether = (bool) e->getStringAttribute("fundamentalSetsTether").getIntValue();
        tetherWeightGlobal = e->getStringAttribute("tetherWeightGlobal").getDoubleValue();
        tetherWeightSecondaryGlobal = e->getStringAttribute("tetherWeightSecondaryGlobal").getDoubleValue();

        //usingFundamentalForIntervalSprings = (bool) e->getStringAttribute("usingFundamentalForIntervalSprings").getIntValue();
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName("intervalScale"))
            {
                Array<float> scale;
                for (int i = 0; i < sub->getNumAttributes(); i++)
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
                for (int i = 0; i < sub->getNumAttributes(); i++)
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
                for (int i = 0; i < sub->getNumAttributes(); i++)
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
                for (int i = 0; i < sub->getNumAttributes(); i++)
                {
                    String attr = sub->getStringAttribute("s" + String(i));
                    
                    if (attr == "")
                    {
                        setSpringMode(i, false);
                    }
                    else
                    {
                        setSpringMode(i, (bool)attr.getIntValue());
                        // DBG("setState::setSpringMode " + String(attr.getIntValue()));
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
    double tetherStiffness, intervalStiffness;
    double drag; // actually 1 - drag; drag of 1 => no drag, drag of 0 => infinite drag
    int numNotes; // number of enabled notes
    
    bool active;
    bool usingFundamentalForIntervalSprings; //only false when in "none" mode
    bool useLowestNoteForFundamental;
    bool useHighestNoteForFundamental;
    bool useLastNoteForFundamental;
    bool useAutomaticFundamental; //uses findFundamental() to set fundamental automatically, based on what is played
    
    bool fundamentalSetsTether; //when true, the fundamental will be used to set tether weights
    double tetherWeightGlobal; //sets weight for tethers to fundamental, when in fundamentalSetsTether mode
    double tetherWeightSecondaryGlobal; //sets weights for tethers to non-fundamental notes
    
    TuningSystem scaleId;
    Array<float> intervalTuning;
    PitchClass intervalFundamental; //one stored, including the mode
    PitchClass intervalFundamentalActive; //one actually used in the moment, changed by auto/last/highest/lowest modes
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
    
    void hiResTimerCallback(void) override
    {
        if (active)
        {
            simulate();
        }
    }

};
