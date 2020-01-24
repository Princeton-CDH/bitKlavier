/*
  ==============================================================================

    Particle.cpp
    Created: 3 Aug 2018 3:44:24pm
    Author:  Theo

  ==============================================================================
*/

#include "Particle.h"
#include "../JuceLibraryCode/JuceHeader.h"

Particle::Particle(double xVal, int n, String s):
x(xVal),
restX(xVal),
prevX(xVal),
enabled(false),
locked(false),
note(n),
name(s)
{

}

void Particle::setRestX(double xp)
{
    restX = xp;
}

double Particle::getRestX()
{
    return restX;
}

void Particle::setX(double xp)
{
    x = xp;
}

double Particle::getX()
{
	return x;
}

Particle::Ptr Particle::copy()
{
	Particle* p = new Particle(x, note * 100, name);
	p->setEnabled(enabled);
	p->setLocked(locked);

	return p;
}

bool Particle::compare(Particle* that)
{
	return (x == that->getX());
}

void Particle::print()
{
	DBG("Position: " + String(x));
	if (enabled) DBG("Currently enabled");
	else DBG("Currently disabled");
}

void Particle::confirmEnabled()
{
	if (enabled) DBG("Currently enabled");
	else DBG("Currently disabled");
}

void Particle::addX(double that)
{
	x += that;
}

void Particle::subX(double that)
{
	x -= that;
}


/*
 integrate() updates the position of the particle based on its velocity
 and attenuated by a drag/friction factor
 
 newX = x + (velocity * deltatime) * dragFactor
 newX = x + (x - prevX) * deltatime * dragFactor; // velocity ~ deltaPosition
 
 deltatime = 1; our time unit is simply the iteration time
 
 drag is actually the inverse of drag; no drag if == 1, infinite drag if == 0
 */

void Particle::integrate(double drag)
{

    // newPosition = oldPosition + velocity * deltatime * inverseOfDragFactor
    //                             velocity = newPosition - oldPosition
    //                                        deltatime = 1 (iteration)
    double newX = x + (x - prevX) * drag;

    // save this as old position, for the next iteration
    prevX = x;
    
    x = newX;
    
    // boundary cases
    if (x < 0.0) x = 0.0;
    else if (x > 13000.0f) x = 13000.0f;
	
}
