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
	restingLength(length),
	strength(str),
	intervalIndex(index)
{
    
}

Particle* Spring::getA()
{
	return a;
}

Particle* Spring::getB()
{
	return b;
}

double Spring::getStrength()
{
	return strength;
}

int Spring::getIntervalIndex()
{
	return intervalIndex;
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
    double warpCoeff = 100.;
    adjustedStrength = 0.6 * stiffness * (pow(warpCoeff, strength) - 1.) / (warpCoeff - 1.);
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

void Spring::update()
{
	/*
	Verlet function
	protected void update(boolean applyConstraints) {
        Vec2D delta = b.sub(a);
        // add minute offset to avoid div-by-zero errors
        float dist = delta.magnitude() + EPS;
        float normDistStrength = (dist - restLength)
                / (dist * (a.invWeight + b.invWeight)) * strength;
        if (!a.isLocked && !isALocked) {
            a.addSelf(delta.scale(normDistStrength * a.invWeight));
            if (applyConstraints) {
                a.applyConstraints();
            }
        }
        if (!b.isLocked && !isBLocked) {
            b.addSelf(delta.scale(-normDistStrength * b.invWeight));
            if (applyConstraints) {
                b.applyConstraints();
            }
        }
    }

	*/
}
