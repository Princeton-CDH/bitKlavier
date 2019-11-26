/*
  ==============================================================================

    Spring.cpp
    Created: 3 Aug 2018 3:44:01pm
    Author:  Theo

  ==============================================================================
*/

#include "Spring.h"
#include "../JuceLibraryCode/JuceHeader.h"

Spring::Spring(Particle* firstPoint, Particle* secondPoint, double length, double str, int index) :
	a(firstPoint),
	b(secondPoint),
    strength(str),
	restingLength(length),
	intervalIndex(index)
{
    
}

Spring::Ptr Spring::copy(void)
{
    Spring::Ptr copySpring = new Spring(a, b, restingLength, strength, intervalIndex);
	return copySpring;
}

bool Spring::compare(Spring* that)
{
	return (a->compare(that->getA()) &&
		b->compare(that->getB()) &&
		strength == that->getStrength());
}

void Spring::print()
{
	DBG("Point A:");
	a->print();
	DBG("\nPoint B:");
	b->print();
	DBG("Strength: " + String(strength));
}

void Spring::setStrength(double newStrength)
{
	strength = newStrength;
    
    // adjusted strength is scaled non-linearly to make the explosed parameter more intuitive and usable
    double warpCoeff = 100.;
    adjustedStrength = 0.6 * stiffness * (pow(warpCoeff, strength) - 1.) / (warpCoeff - 1.); //replace with dt_asymwarp, for clarity
    // > ~0.6 and the system can become unstable...
}

void Spring::satisfyConstraints(void)
{
    double diff = b->getX() - a->getX();
    
    length = abs(diff);
    
	if (diff == 0.0) return;
    
    if(stiffness != oldStiffness)
    {
        oldStiffness = stiffness;
        setStrength(strength);
    }
    
    diff *= ( (diff - restingLength) / diff ) * adjustedStrength;
    
    if (!a->getLocked())
    {
        a->addX(diff);
    }

    if (!b->getLocked())
    {
        b->subX(diff);
    }

}
