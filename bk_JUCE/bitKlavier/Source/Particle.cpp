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
note(n),
enabled(false),
locked(false),
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

Particle* Particle::copy()
{
	Particle* p = new Particle(x, note, name);
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

void Particle::integrate(double drag)
{

    double newX = x - prevX;

    newX *= drag;

    newX += x;

    prevX = x;
    
    x = newX;
    
    if (x < 0.0) x = 0.0;
    else if (x > 10000.0f) x = 10000.0f;
	

	//Aatish's function:
	/*
	var newPos = diff.subVectors( this.position, this.previous );
	newPos.multiplyScalar( DRAG ).add( this.position );
	this.previous = this.position;
	this.position = newPos;
	*/
}
