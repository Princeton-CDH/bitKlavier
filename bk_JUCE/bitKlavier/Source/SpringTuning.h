/*
  ==============================================================================

    SpringTuning.h
    Created: 3 Aug 2018 3:43:46pm
    Author:  Theo

  ==============================================================================
*/

#pragma once
#include "SpringTuningUtilities.h"

#include "Particle.h"
#include "Spring.h"

class SpringTuningModel : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<SpringTuningModel> Ptr;
    
    SpringTuningModel(SpringTuningModel::Ptr st = nullptr);
    ~SpringTuningModel(){};
	void simulate();
    
    void copy(SpringTuningModel::Ptr st);
    
    float tetherWeight, springWeight;

	void toggleSpring();

	void addParticle(int noteIndex);
	void removeParticle(int removeIndex);
    
    Particle* getParticle(int note) { return particleArray[note];}
    
	void addNote(int noteIndex);
	void removeNote(int noteIndex);
    void removeAllNotes(void);
	void toggleNote(int noteIndex);
    
	void updateNotes();
	void updateFreq();

	void addSpring(Spring* s);
	void removeSpring(Spring* s);
    
	void addSpringsByNote(int addIndex);
	void removeSpringsByNote(int removeIndex);
	void addSpringsByInterval(double interval);
	void removeSpringsByInterval(double interval);
	void adjustSpringsByInterval(double interval, double stiffness);
    
    void setSpringWeight(int which, double weight);
    double getSpringWeight(int which);
    
    void setTetherWeight(int which, double weight);
    double getTetherWeight(int which);

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
    
    void setTetherTuning(int tuning);
    int getTetherTuning(void){return tetherTuning;}
    
    void setIntervalTuning(int tuning);
    int getIntervalTuning(void){return intervalTuning;}

private:
	const int octaves[1] = { 4 }; //will return to when adding more octaves
    
    int tetherTuning;
    int intervalTuning;

    Particle::PtrArr    particleArray;
    Spring::PtrArr      springArray; // efficiency fix: make this ordered by spring interval 
    
    Particle::PtrArr    tetherParticleArray;
    Spring::PtrArr      tetherSpringArray;
    
    
    
	int numNotes; // number of enabled notes
};
