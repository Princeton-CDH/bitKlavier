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

class SpringTuningModel : public ReferenceCountedObject, private HighResolutionTimer
{
public:
    typedef ReferenceCountedObjectPtr<SpringTuningModel> Ptr;
    
    SpringTuningModel(SpringTuningModel::Ptr st = nullptr);
    ~SpringTuningModel(){stopTimer();};
	void simulate();
    
    void copy(SpringTuningModel::Ptr st);

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

	void addSpring(Spring* s);
	void removeSpring(Spring* s);
    
	void addSpringsByNote(int pc);
	void removeSpringsByNote(int removeIndex);
	void addSpringsByInterval(double interval);
	void removeSpringsByInterval(double interval);
	void adjustSpringsByInterval(double interval, double stiffness);

    inline void setRate(double r)
    {
        rate = r;
        startTimer(1000 / rate);
    }
    
    inline double getRate(void)
    {
        return rate;
    }
    
    inline void setStiffness(double stiff)
    {
        stiffness = stiff;
        
        for (auto spring : springArray)
        {
            spring->setStiffness(stiffness);
        }
        
        for (auto spring : tetherSpringArray)
        {
            spring->setStiffness(stiffness);
        }
    }
    
    inline double getStiffness(void)
    {
        return stiffness;
    }

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
    
    bool getTetherSpringEnabled(int which);
    bool getSpringEnabled(int which);
    
    String getTetherSpringName(int which);
    
    String getSpringName(int which);
    
    void setTetherTuning(Array<float> tuning);
    Array<float> getTetherTuning(void){return tetherTuning;}
    
    void setIntervalTuning(Array<float> tuning);
    Array<float> getIntervalTuning(void){return intervalTuning;}
    
    void setSpringWeight(int which, double weight);
    double getSpringWeight(int which);
    
    void setTetherWeight(int which, double weight);
    double getTetherWeight(int which);
    
    ValueTree getState(void)
    {
        ValueTree prep("springtuning");
        
        prep.setProperty( "rate", rate, 0);
        prep.setProperty( "stiffness", stiffness, 0);
        prep.setProperty( "active", active ? 1 : 0, 0);
        prep.setProperty( "scaleId", scaleId, 0);

        ValueTree tethers( "tethers");
        ValueTree springs( "springs");
        ValueTree tetherLocks( "locks");
        
        for (int i = 0; i < 128; i++)
        {
            tethers.setProperty( "t"+String(i), getTetherWeight(i), 0 );
        }
        
        for (int i = 0; i < 12; i++)
        {
            springs.setProperty( "s"+String(i), getSpringWeight(i), 0 );
            tetherLocks.setProperty("tl"+String(i), getTetherLock(i) ? 1 : 0, 0);
            
        }
        prep.addChild(tethers, -1, 0);
        prep.addChild(springs, -1, 0);
        prep.addChild(tetherLocks, -1, 0);

        return prep;
    }
    
    void setState(XmlElement* e)
    {
        setRate(e->getStringAttribute("rate").getDoubleValue());
        
        setStiffness(e->getStringAttribute("stiffness").getDoubleValue());
        
        active = (bool) e->getStringAttribute("active").getIntValue();
        
        scaleId = (TuningSystem) e->getStringAttribute("scaleId").getIntValue();
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName("tethers"))
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
                Array<float> scale;
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
            else if (sub->hasTagName("locks"))
            {
                Array<float> scale;
                for (int i = 0; i < 12; i++)
                {
                    String attr = sub->getStringAttribute("tl" + String(i));
                    
                    if (attr == "")
                    {
                        setTetherLock(i, false);
                    }
                    else
                    {
                        setTetherLock(i, (bool)attr.getIntValue());
                    }
                }
            }
        }
    }
    
    
    inline void setTetherLock(int pc, bool tl) { tetherLocked[pc] = tl;}
    inline bool getTetherLock(int pc) { return tetherLocked[pc];}
    
    inline void setActive(bool status) { active = status; }
    inline bool getActive(void) { return active; }
    
    inline void setScaleId(TuningSystem which) { scaleId = which; }
    inline TuningSystem getScaleId(void) { return scaleId; }

private:
    double rate, stiffness;
    
    bool active;
    
    TuningSystem scaleId;

    Particle::PtrArr    particleArray;
    Spring::PtrArr      springArray; // efficiency fix: make this ordered by spring interval 
    
    Particle::PtrArr    tetherParticleArray;
    Spring::PtrArr      tetherSpringArray;
    
    bool tetherLocked[12];
    
    Array<float> tetherTuning;
    Array<float> intervalTuning;
    
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
