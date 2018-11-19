/*
  ==============================================================================

    Spring.h
    Created: 3 Aug 2018 3:44:01pm
    Author:  Theo

  ==============================================================================
*/

#include "SpringTuningUtilities.h"

#include "Particle.h"

class Spring : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Spring> Ptr;
    typedef Array<Spring::Ptr> PtrArr;
    
	Spring(Particle* firstPoint, Particle* secondPoint, double length, double str, int index);
	Particle* getA(void);
	Particle* getB(void);
    
	int getIntervalIndex();
    Spring::Ptr copy();
	bool compare(Spring* that);
	void print();
    
    void setStrength(double newStrength);
    double getStrength();
    
    inline void setAdjustedStrength(double strength) { adjustedStrength = strength; }
    inline double getAdjustedStrength() { return adjustedStrength; }
    
    void setRestingLength(double len) { restingLength = len; }
    double getRestingLength(void) { return restingLength; }
    
    void setLength(double len) { length = len; }
    double getLength(void) { return length; }
    
    inline void setStiffness(double stiff)
    {
        stiffness = stiff;
    }
    
    inline double getStiffness(void)
    {
        return stiffness;
    }

	void satisfyConstraints(void);

	void update();
    
    bool getEnabled(void) { return enabled; }
    void setEnabled(bool e) { enabled = e; }

    void setName(String s) { name = s;}
    String getName(void) { return name;}

private:
	Particle* a;
	Particle* b;
    
	double strength;
    double adjustedStrength;
    double stiffness;
    
	double restingLength, length;
    bool enabled;
	int intervalIndex; //will probably replace base interval
    String name;
};

#pragma once
