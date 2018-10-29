/*
  ==============================================================================

    SpringTuning.cpp
    Created: 3 Aug 2018 3:43:46pm
    Author:  Theo

  ==============================================================================
*/

#include "SpringTuning.h"
#include "SpringTuningUtilities.h"

using namespace std;

void SpringTuningModel::copy(SpringTuningModel::Ptr st)
{
    rate = st->getRate();
    stiffness = st->getStiffness();
    for (int i = 0; i < 12; i++)
    {
        setSpringWeight(i, st->getSpringWeight(i));
    }
    
    for (int i = 0; i < 128; i++)
    {
        setTetherWeight(i, st->getTetherWeight(i));
    }
}

SpringTuningModel::SpringTuningModel(SpringTuningModel::Ptr st):
tetherTuning(1),
intervalTuning(0),
rate(100)
{
    particleArray.ensureStorageAllocated(128);
    tetherParticleArray.ensureStorageAllocated(128);

	//double xValue = cFreq;
    
	for (int i = 0; i < 128; i++)
	{
        // Active particle
        int pc = (i % 12);
        int octave = (int)(i / 12);
        
        Particle* p1 = new Particle(i * 100, i, notesInAnOctave[pc]);
        p1->setOctave(octave);
        p1->setEnabled(false);
		p1->setLocked(false);
        particleArray.add(p1);
        
        // Tether particle
        Particle* p2 = new Particle(i * 100, i, notesInAnOctave[pc]);
        p2->setOctave(octave);
        p2->setEnabled(false);
        p2->setLocked(true);
        tetherParticleArray.add(p2);
        
        Spring* s = new Spring(p1, p2, 0.0, 0.5, 1.0, 0);
        s->setEnabled(false);
        s->setName(intervalLabels[0]);
        tetherSpringArray.add(s);
	}

    springArray.ensureStorageAllocated(1000);
	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < i; j++)
		{
			//will add in a better length calculation method once mapping is figured out
            
            int interval = ((i-j) % 12);
            
            Spring* spring = new Spring(particleArray[j],
                                        particleArray[i],
                                        centLengths[0][interval],
                                        0.5,
                                        tunings[intervalTuning][interval],
                                        interval);
            
            spring->setEnabled(false);
            spring->setName(intervalLabels[interval]);
            springArray.add(spring);
		}
	}
    
    setStiffness(1.0);

	numNotes = 0;
    
    if (st != nullptr) copy(st);
    
    setRate(rate);
}

void SpringTuningModel::setTetherTuning(int tuning)
{
    tetherTuning = tuning;
    
    for (int i = 0; i < 128; i++)
    {
        tetherParticleArray[i]->setX(Utilities::cFreq * tunings[tetherTuning][(i%12)]);
    }
}

void SpringTuningModel::setIntervalTuning(int tuning)
{
    intervalTuning = tuning;
    int which = 0;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < i; j++)
        {
            springArray[which++]->setBaseInterval(Utilities::cFreq * tunings[intervalTuning][i-j]);
        }
    }
}

#define DRAG 1.0f
void SpringTuningModel::simulate()
{

    for (auto particle : particleArray)
    {
		if (particle->getEnabled() && !particle->getLocked())
        {
            particle->integrate(DRAG);
        }
	}
    
    for (auto spring : tetherSpringArray)
    {
        if (spring->getEnabled())
        {
            spring->satisfyConstraints();
        }
    }

	for (auto spring : springArray)
	{
		if (spring->getEnabled())
		{
            spring->satisfyConstraints();
		}
	}
}

void SpringTuningModel::setSpringWeight(int which, double weight)
{
    for (auto spring : springArray)
    {
        if (spring->getIntervalIndex() == which)
        {
            spring->setStrength(weight);
        }
    }
}

double SpringTuningModel::getSpringWeight(int which)
{
    // find first spring with interval that matches which and return its weight
    for (auto spring : springArray)
    {
        if (spring->getIntervalIndex() == which) return spring->getStrength();
    }
    return 0.0;
}

void SpringTuningModel::setTetherWeight(int which, double weight)
{
    Spring* spring = tetherSpringArray[which];
    
    spring->setStrength(weight);
    
    Particle* a = spring->getA();
    Particle* b = spring->getB();
    Particle* use = nullptr;
    Particle* tethered = tetherParticleArray[which];
    
    if (a != tethered)  use = a;
    else                use = b;

    if (use != nullptr)
    {
        if (weight == 1.0)
        {
            use->setX(use->getRestX());
            use->setLocked(true);
        }
        else
        {
            use->setLocked(false);
			if (weight == 0.0) tethered->setEnabled(false);
        }
    }
}

double SpringTuningModel::getTetherWeight(int which)
{
    return tetherSpringArray[which]->getStrength();
}

bool SpringTuningModel::getTetherSpringEnabled(int which)
{
    return tetherSpringArray[which]->getEnabled();
}

bool SpringTuningModel::getSpringEnabled(int which)
{
    for (auto spring : springArray)
    {
        if (spring->getIntervalIndex() == which) return spring->getEnabled();
    }
    return false;
}

String SpringTuningModel::getTetherSpringName(int which)
{
    return tetherSpringArray[which]->getName();
}

String SpringTuningModel::getSpringName(int which)
{
    for (auto spring : springArray)
    {
        if (spring->getIntervalIndex() == which) return spring->getName();
    }
    return "";
}

void SpringTuningModel::toggleSpring()
{
	//tbd
}

void SpringTuningModel::addParticle(int note)
{;
    particleArray[note]->setEnabled(true);
    tetherParticleArray[note]->setEnabled(true);
}
void SpringTuningModel::removeParticle(int note)
{
    Particle* p = particleArray[note];
    p->setEnabled(false);
    //p->setX(note * 100); // DO WE WANT THIS?
    tetherParticleArray[note]->setEnabled(false);
}
void SpringTuningModel::addNote(int note)
{
    addParticle(note);
    addSpringsByNote(note);
}

void SpringTuningModel::removeNote(int note)
{
    removeParticle(note);
    removeSpringsByNote(note);
}

void SpringTuningModel::removeAllNotes(void)
{
    for (int i = 0; i < 128; i++) removeNote(i);
}

void SpringTuningModel::toggleNote(int noteIndex)
{
	int convertedIndex = noteIndex; // just in case a midi value is passed accidentally

	if (particleArray[convertedIndex]->getEnabled())
	{
		removeNote(convertedIndex);
	}
	else
	{
		addNote(convertedIndex);
	}
}

void SpringTuningModel::addSpring(Spring* s)
{
    s->setEnabled(true);
}

void SpringTuningModel::removeSpring(Spring* s)
{
    s->setEnabled(false);
    
}
void SpringTuningModel::addSpringsByNote(int note)
{
    Particle* p = particleArray[note];
    for (auto spring : springArray)
    {
        Particle* a = spring->getA();
        Particle* b = spring->getB();
        
		if (!spring->getEnabled())
        {
			// sets the spring to enabled if one spring matches the index and the other is enabled
			if (a == p)
			{
				if (b->getEnabled()) spring->setEnabled(true);
			}
			else if (b == p)
			{
				if (a->getEnabled()) spring->setEnabled(true);
			}
        }
	}
    
    tetherSpringArray[note]->setEnabled(true);
}
void SpringTuningModel::removeSpringsByNote(int note)
{
	Particle* p = particleArray[note];
	for (auto spring : springArray)
	{
        Particle* a = spring->getA();
        Particle* b = spring->getB();
        
		if (spring->getEnabled() && ((a == p) || (b == p)))
        {
            spring->setEnabled(false);
        }
	}
    
    tetherSpringArray[note]->setEnabled(false);
}
void SpringTuningModel::addSpringsByInterval(double interval)
{
	for (auto spring : springArray)
	{
        if (!spring->getEnabled() && (abs(spring->getBaseInterval() - interval) <= 0.001))
        {
            spring->setEnabled(true);
        }
	}
}
void SpringTuningModel::removeSpringsByInterval(double interval)
{
    for (auto spring : springArray)
	{
        if (spring->getEnabled() && (abs(spring->getBaseInterval() - interval) <= 0.001))
        {
            spring->setEnabled(false);
        }
	}
}
void SpringTuningModel::adjustSpringsByInterval(double interval, double weight)
{
	for (auto spring : springArray)
	{
		if ((abs(spring->getBaseInterval() - interval) <= 0.001))
        {
            spring->setStrength(weight);
        }
	}
}

double SpringTuningModel::getFrequency(int note)
{
	return Utilities::centsToFreq((int) particleArray[note]->getX());
}

bool SpringTuningModel::pitchEnabled(int index)
{
	return particleArray[index]->getEnabled();
}

void SpringTuningModel::print()
{
    DBG("~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
	for (int i = 0; i < 128; i++)
	{
		String printStatus = "";
		if (particleArray[i]->getEnabled())
        {
            DBG(String(i));
        }
	}
}

void SpringTuningModel::printParticles()
{
	for (int i = 0; i < 128; i++)
	{
		particleArray[i]->print();
	}
}

void SpringTuningModel::printActiveParticles()
{
	for (int i = 0; i < 128; i++)
	{
		if (particleArray[i]->getEnabled()) particleArray[i]->print();
	}
}

void SpringTuningModel::printActiveSprings()
{
	for (auto spring : springArray)
	{
		if (spring->getEnabled()) spring->print();
	}
}

bool SpringTuningModel::checkEnabledParticle(int index)
{
	return particleArray[index]->getEnabled();
}
