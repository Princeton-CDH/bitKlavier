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

void SpringProcessor::copy(SpringProcessor::Ptr st)
{
    rate = st->getRate();
    stiffness = st->getStiffness();
    for (int i = 0; i < 12; i++)
    {
        setSpringWeight(i, st->getSpringWeight(i));
        setTetherLock(i, st->getTetherLock(i));
    }
    
    for (int i = 0; i < 128; i++)
    {
        setTetherWeight(i, st->getTetherWeight(i));
    }
}

SpringProcessor::SpringProcessor(SpringProcessor::Ptr st):
scaleId(JustTuning),
tetherStiffness(0.5),
intervalStiffness(0.5),
drag(1.),
rate(100),
active(false)
{
    particleArray.ensureStorageAllocated(128);
    tetherParticleArray.ensureStorageAllocated(128);
    
    tetherTuning = Array<float>({0,0,0,0,0,0,0,0,0,0,0,0});
    intervalTuning = Array<float>({0.0, 0.117313, 0.039101, 0.156414, -0.13686, -0.019547,
        -0.174873, 0.019547, 0.136864, -0.15641, -0.311745, -0.11731});
    
    for (int i = 0; i < 12; i++) tetherLocked[i] = false;
    
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
        
        Spring* s = new Spring(p1, p2, 0.0, 0.5, 0);
        s->setEnabled(false);
        s->setName(intervalLabels[0]);
        tetherSpringArray.add(s);
	}

    springArray.ensureStorageAllocated(1000);
	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < i; j++)
		{
            int diff = i - j;
            
            int interval = diff % 12;
            
            if (diff != 0 && interval == 0)
            {
                interval = 12;
            }

            //DBG("spring: " + String(i) + " " + String(j) + " " + String(diff * 100 + intervalTuning[interval] * 100));

            Spring* spring = new Spring(particleArray[j],
                                        particleArray[i],
                                        diff * 100 + intervalTuning[interval] * 100,
                                        0.5,
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

void SpringProcessor::setTetherTuning(Array<float> tuning)
{
    tetherTuning = tuning;
    
    for (int i = 0; i < 128; i++)
    {
        tetherParticleArray[i]->setX( (i * 100.0) + tetherTuning[i % 12] );
        tetherParticleArray[i]->setRestX( (i * 100.0) + tetherTuning[i % 12] );
        
        //DBG("rest X: " + String((i * 100.0) + tetherTuning[i % 12]));
        particleArray[i]->setRestX( (i * 100.0) + tetherTuning[i % 12] );
    }
}

void SpringProcessor::setIntervalTuning(Array<float> tuning)
{
    intervalTuning = tuning;

    for (auto spring : springArray)
    {
        int interval = spring->getIntervalIndex();
        int diff = spring->getA()->getRestX() - spring->getB()->getRestX();
        //spring->setRestingLength(interval * 100.0 + intervalTuning[interval]);
        spring->setRestingLength(fabs(diff) + intervalTuning[interval]);
        //DBG("setIntervalTuning " + String(interval) + " " + String(spring->getRestingLength()));
    }
}

//#define DRAG 1.0f //expose this!!
void SpringProcessor::simulate()
{
    for (auto particle : particleArray)
    {
		if (particle->getEnabled() && !particle->getLocked())
        {
            particle->integrate(drag);
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

void SpringProcessor::setSpringWeight(int which, double weight)
{
    for (auto spring : springArray)
    {
        int interval = spring->getIntervalIndex();

        if (which == interval)
        {
            spring->setStrength(weight);
        }
    }
}

double SpringProcessor::getSpringWeight(int which)
{
    // find first spring with interval that matches which and return its weight
    for (auto spring : springArray)
    {
        if (spring->getIntervalIndex() == which) return spring->getStrength();
    }
    return 0.0;
}

void SpringProcessor::setTetherWeight(int which, double weight)
{
    int pc = which % 12;
    if (tetherLocked[pc])
    {
        for (int t = pc; t < 128; t += 12)
        {
            Spring* spring = tetherSpringArray[t];
            
            spring->setStrength(weight);
            
            Particle* a = spring->getA();
            Particle* b = spring->getB();
            Particle* particle = nullptr;
            Particle* tethered = tetherParticleArray[t];
            
            if (a != tethered)  particle = a;
            else                particle = b;
            
            if (particle != nullptr)
            {
                if (weight == 1.0)
                {
                    particle->setX(particle->getRestX());
                    particle->setLocked(true);
                }
                else
                {
                    particle->setLocked(false);
                    //if (weight == 0.0) tethered->setEnabled(false);
                }
            }
        }
        
    }
    else
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
    
}

double SpringProcessor::getTetherWeight(int which)
{
    return tetherSpringArray[which]->getStrength();
}

bool SpringProcessor::getTetherSpringEnabled(int which)
{
    return tetherSpringArray[which]->getEnabled();
}

bool SpringProcessor::getSpringEnabled(int which)
{
    for (auto spring : springArray)
    {
        if (spring->getIntervalIndex() == which) return spring->getEnabled();
    }
    return false;
}

String SpringProcessor::getTetherSpringName(int which)
{
    return tetherSpringArray[which]->getName();
}

String SpringProcessor::getSpringName(int which)
{
    for (auto spring : springArray)
    {
        if (spring->getIntervalIndex() == which) return spring->getName();
    }
    return "";
}

void SpringProcessor::toggleSpring()
{
	//tbd
}

void SpringProcessor::addParticle(int note)
{;
    particleArray[note]->setEnabled(true);
    tetherParticleArray[note]->setEnabled(true);
    DBG("addedParticle " + String(note));
}
void SpringProcessor::removeParticle(int note)
{
    Particle* p = particleArray[note];
    p->setEnabled(false);
    tetherParticleArray[note]->setEnabled(false);
}
void SpringProcessor::addNote(int note)
{
    addParticle(note);
    addSpringsByNote(note);
}

void SpringProcessor::removeNote(int note)
{
    removeParticle(note);
    removeSpringsByNote(note);
}

void SpringProcessor::removeAllNotes(void)
{
    for (int i = 0; i < 128; i++) removeNote(i);
}

void SpringProcessor::toggleNote(int noteIndex)
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

void SpringProcessor::addSpring(Spring* s)
{
    s->setEnabled(true);
}

void SpringProcessor::removeSpring(Spring* s)
{
    s->setEnabled(false);
    
}
void SpringProcessor::addSpringsByNote(int note)
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
    
    DBG("addedSpring " + String(note));
}

//DT: wondering if there is a more efficient way to do this, rather than reading throug the whole spring array?
void SpringProcessor::removeSpringsByNote(int note)
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

double SpringProcessor::getFrequency(int note)
{
	return Utilities::centsToFreq((int) particleArray[note]->getX());
}

void SpringProcessor::print()
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

void SpringProcessor::printParticles()
{
	for (int i = 0; i < 128; i++)
	{
		particleArray[i]->print();
	}
}

void SpringProcessor::printActiveParticles()
{
	for (int i = 0; i < 128; i++)
	{
		if (particleArray[i]->getEnabled()) particleArray[i]->print();
	}
}

void SpringProcessor::printActiveSprings()
{
	for (auto spring : springArray)
	{
		if (spring->getEnabled()) spring->print();
	}
}

bool SpringProcessor::checkEnabledParticle(int index)
{
	return particleArray[index]->getEnabled();
}
