/*
  ==============================================================================

    Spring.cpp
    Created: 3 Aug 2018 3:44:01pm
    Author:  Theo

  ==============================================================================
*/

#include "Spring.h"
#include "../JuceLibraryCode/JuceHeader.h"

Spring::Spring(Particle* firstPoint, Particle* secondPoint, double length, double str, double interval, int index) :
	a(firstPoint),
	b(secondPoint),
	restingLength(length),
	strength(str),
	baseInterval(interval),
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

double Spring::getLength()
{
	return restingLength;
}

double Spring::getStrength()
{
	return strength;
}

double Spring::getBaseInterval()
{
	return baseInterval;
}

void Spring::setBaseInterval(double interval)
{
    baseInterval = interval;
}

int Spring::getIntervalIndex()
{
	return intervalIndex;
}

Spring Spring::copy()
{
	Spring copySpring(a, b, restingLength, strength, baseInterval, intervalIndex);
	return copySpring;
}

bool Spring::compare(Spring* that)
{
	return (a->compare(that->getA()) &&
		b->compare(that->getB()) &&
		strength == that->getStrength() &&
		baseInterval == that->getBaseInterval());
}

void Spring::print()
{
	DBG("Point A:");
	a->print();
	DBG("\nPoint B:");
	b->print();
	DBG("Strength: " + String(strength));
	DBG("Base Interval: " + String(baseInterval));
}

//we probably don't want to be messing around with this but it can't hurt to have the method
void Spring::setLength(double newLength)
{
	restingLength = newLength;
}

void Spring::setStrength(double newStrength)
{
	strength = newStrength;
}

/*
String Spring::getStringBaseInterval()
{
	String result = "";
	switch (baseInterval) 
	{
	case (double)(25.0 / 24.0) :
		result = "Minor 2nd";
		break;

	case 9.0/8.0:
		result = "Major 2nd";
		break;

	case 6.0/5.0:
		result = "Minor 3rd";
		break;

	case 5.0/4.0:
		result = "Major 3rd";
		break;

	case 4.0/3.0:
		result = "Perfect 4th";
		break;

	case 45.0/32.0:
		result = "Diminished 5th";
		break;

	case 3.0/2.0:
		result = "Perfect 5th";
		break;
	
	case 8.0/5.0:
		result = "Minor 6th";
		break;

	case 5.0/3.0:
		result = "Major 6th";
		break;

	case 9.0/5.0:
		result = "Minor 7th";
		break;

	case 15.0/8.0:
		result = "Major 7th";
		break;

	case 2.0:
		result = "Octave";
		break;
	}
	return result;
}
*/

void Spring::satisfyConstraints(void)
{
	double diff = b->getX() - a->getX();
	if (diff == 0.0) return;
    
	//DBG("Satisfying constraints for " + getA()->getName() + " to " + getB()->getName());
	//DBG("Initial diff: " + String(diff));

    const double maxStiffness = 0.5;
    const double meanStiffness = 0.05;

	double actualStrength = Utilities::clip(0.0, (meanStiffness * strength) / (1.0 - strength), maxStiffness);

	/*
	DBG("Variables: ");
	DBG("Weight = " + String(strength));
	DBG("Strength = " + String(actualStrength));
	DBG("Diff - RestingLength = " + String(diff - restingLength));
	DBG("/Diff = " + String((diff - restingLength) / diff));
	DBG("* strength = " + String((diff - restingLength) / diff * actualStrength));
	*/

    diff *= ((diff - restingLength) / diff) * actualStrength;
	//DBG("Final diff: " + String(diff));

    if (!a->getLocked())
    {
        a->addX(diff);
		//DBG("New A = " + String(a->getX()));
    }

    if (!b->getLocked())
    {
        b->subX(diff);
		//DBG("New B = " + String(b->getX()));
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
