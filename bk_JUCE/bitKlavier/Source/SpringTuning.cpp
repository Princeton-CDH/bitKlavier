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
        setTetherWeight(i, st->getTetherWeight(i));
    }
}

SpringTuningModel::SpringTuningModel(SpringTuningModel::Ptr st):
tetherTuning(1),
intervalTuning(0),
rate(100)
{
    particleArray.ensureStorageAllocated(12);

	//double xValue = cFreq;

	for (int i = 0; i < 12; i++)
	{
        // Active particle
        Particle* p1 = new Particle(i * 100, i, notesInAnOctave[i]);
        p1->setEnabled(false);
		p1->setLocked(false);
        particleArray.add(p1);
        
        // Tether particle
        Particle* p2 = new Particle(i * 100, i, notesInAnOctave[i]);
        p2->setEnabled(false);
        p2->setLocked(true);
        tetherParticleArray.add(p2);
        
        Spring* s = new Spring(p1, p2, 0.0, 0.2, 1.0, 0);
        s->setEnabled(false);
        s->setName(intervalLabels[i]);
        tetherSpringArray.add(s);
        
        notes[i] = 0;
	}

    springArray.ensureStorageAllocated(100);
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < i; j++)
		{
			//will add in a better length calculation method once mapping is figured out
            Spring* spring = new Spring(particleArray[j],
                                        particleArray[i],
				centLengths[0][(i - j)], 0.5 , tunings[intervalTuning][(i - j)], (i - j));
            
            //DBG("spring: " + String(i) + " interval: " + String(i-j));
            
            spring->setEnabled(false);
            spring->setName(intervalLabels[(i - j)]);
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
    
    for (int i = 0; i < 12; i++)
    {
        tetherParticleArray[i]->setX(Utilities::cFreq * tunings[tetherTuning][i]);
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

void SpringTuningModel::addParticle(int index)
{;
    particleArray[index]->setEnabled(true);
    tetherParticleArray[index]->setEnabled(true);
}
void SpringTuningModel::removeParticle(int index)
{
    Particle* p = particleArray[index];
    p->setEnabled(false);
    p->setX(index * 100); // DO WE WANT THIS?
    tetherParticleArray[index]->setEnabled(false);
}
void SpringTuningModel::addNote(int note)
{
    int pc = (note % 12);
    
    notes[pc]++;
    
    if (!particleArray[pc]->getEnabled() && !tetherParticleArray[pc]->getEnabled())
    {
        addParticle(pc);
        addSpringsByNote(pc);
    }
}

void SpringTuningModel::removeNote(int noteIndex)
{
    int pc = noteIndex % 12;
    
    notes[pc]--;
    notes[pc] = (notes[pc] < 0) ? 0 : notes[pc];
    
    if (notes[pc] == 0)
    {
        removeParticle(pc);
        removeSpringsByNote(pc);
    }
}

void SpringTuningModel::removeAllNotes(void)
{
    for (int i = 0; i < 12; i++) removeNote(i);
}

void SpringTuningModel::toggleNote(int noteIndex)
{
	int convertedIndex = noteIndex % 12; // just in case a midi value is passed accidentally

	if (particleArray[convertedIndex]->getEnabled())
	{
		removeNote(convertedIndex);
	}
	else
	{
		addNote(convertedIndex);
	}
}



//probably not necessary until UI?
void SpringTuningModel::updateNotes()
{
	//tbd
}

//probably not necessary until UI?
void SpringTuningModel::updateFreq()
{
	//tbd
}

void SpringTuningModel::addSpring(Spring* s)
{
    s->setEnabled(true);
}

void SpringTuningModel::removeSpring(Spring* s)
{
    s->setEnabled(false);
    
}
void SpringTuningModel::addSpringsByNote(int addIndex)
{
    Particle* p = particleArray[addIndex];
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
    
    tetherSpringArray[addIndex]->setEnabled(true);
}
void SpringTuningModel::removeSpringsByNote(int removeIndex)
{
	Particle* p = particleArray[removeIndex];
	for (auto spring : springArray)
	{
        Particle* particleA = spring->getA();
        Particle* particleB = spring->getB();
        
		if (spring->getEnabled() && (particleA->compare(p) || particleB->compare(p)))
        {
            spring->setEnabled(false);
        }
	}
    
    tetherSpringArray[removeIndex]->setEnabled(false);
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

double SpringTuningModel::getFrequency(int index)
{
	return Utilities::centsToFreq((int) particleArray[index]->getX());
}

bool SpringTuningModel::pitchEnabled(int index)
{
	return particleArray[index]->getEnabled();
}

void SpringTuningModel::print()
{
    DBG("~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
	for (int i = 0; i < 12; i++)
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
	for (int i = 0; i < 12; i++)
	{
		particleArray[i]->print();
	}
}

void SpringTuningModel::printActiveParticles()
{
	for (int i = 0; i < 12; i++)
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
