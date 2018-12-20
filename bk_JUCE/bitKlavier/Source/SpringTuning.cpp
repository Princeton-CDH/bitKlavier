/*
  ==============================================================================

    SpringTuning.cpp
    Created: 3 Aug 2018 3:43:46pm
    Author:  Theo, Mike, Dan

  ==============================================================================
*/

#include "SpringTuning.h"
#include "SpringTuningUtilities.h"

using namespace std;

void SpringTuning::copy(SpringTuning::Ptr st)
{
    rate = st->getRate();
    stiffness = st->getStiffness();
    active = st->getActive();
    drag = st->getDrag();
    
    intervalStiffness = st->getIntervalStiffness();
    tetherStiffness = st->getTetherStiffness();
    
    for (int i = 0; i < 13; i++)
    {
        springWeights[i] = st->springWeights[i];
    }
    
    scaleId = st->getScaleId();
    
    setIntervalTuning(st->getIntervalTuning());
    setTetherTuning(st->getTetherTuning());
    
    setSpringWeights(st->getSpringWeights());
    setTetherWeights(st->getTetherWeights());
}

SpringTuning::SpringTuning(SpringTuning::Ptr st):
rate(100),
tetherStiffness(0.5),
intervalStiffness(0.5),
drag(0.1),
active(false),
usingFundamentalForIntervalSprings(false), //should be false by default
scaleId(JustTuning)
{
    particleArray.ensureStorageAllocated(128);
    tetherParticleArray.ensureStorageAllocated(128);
    
    enabledSpringArray.clear();
    
    tetherTuning = Array<float>({0,0,0,0,0,0,0,0,0,0,0,0});
    intervalTuning = Array<float>({0.0, 0.117313, 0.039101, 0.156414, -0.13686, -0.019547, -0.174873, 0.019547, 0.136864, -0.15641, -0.311745, -0.11731});
    
    for (int i = 0; i < 12; i++) tetherLocked[i] = false;
    
    for (int i = 0; i < 13; i++) springWeights[i] = 0.5;
    
	for (int i = 0; i < 128; i++)
	{
        // Active particle
        int pc = (i % 12);
        int octave = (int)(i / 12);
        
        Particle* p1 = new Particle(i * 100, i, notesInAnOctave[pc]);
        p1->setOctave(octave);
        p1->setEnabled(false);
		p1->setLocked(false);
        p1->setNote(i);
        particleArray.add(p1);
        
        // Tether particle
        Particle* p2 = new Particle(i * 100, i, notesInAnOctave[pc]);
        p2->setOctave(octave);
        p2->setEnabled(false);
        p2->setLocked(true);
        p2->setNote(i);
        tetherParticleArray.add(p2);
        
        Spring* s = new Spring(p1, p2, 0.0, 0.5, 0);
        s->setEnabled(false);
        s->setName(intervalLabels[0]);
        tetherSpringArray.add(s);
	}

    springArray.ensureStorageAllocated(1000);
    
    if(!usingFundamentalForIntervalSprings)
    {
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
                                            diff * 100 + intervalTuning[interval] * 100, //rest length in cents
                                            0.5,
                                            interval);
                
                spring->setEnabled(false);
                spring->setName(intervalLabels[interval]);
                springArray.add(spring);
            }
        }
    }
    else
    {
        //usingFundamentalForIntervalSprings

        for (int i = 0; i < 128; i++)
        {
            for (int j = 0; j < i; j++)
            {
                float diff = i - j;
                int interval = (int)diff % 12;
                if (diff != 0 && interval == 0)
                {
                    interval = 12;
                }
                
                int scaleDegree1 = particleArray[i]->getNote();
                int scaleDegree2 = particleArray[j]->getNote();;
                int intervalFundamental = 0; //temporary, will set in preparation
                
                diff =  100. * ((scaleDegree1 + intervalTuning[(scaleDegree1 - intervalFundamental) % 12]) -
                                (scaleDegree2 + intervalTuning[(scaleDegree2 - intervalFundamental) % 12]));
                
                //DBG("setting new spring " + String(scaleDegree1) + " " + String(scaleDegree2) + " length = " + String(fabs(diff)));
                Spring* spring = new Spring(particleArray[j],
                                            particleArray[i],
                                            fabs(diff), //rest length in cents
                                            0.5,
                                            interval);
                
                spring->setEnabled(false);
                spring->setName(intervalLabels[interval]);
                springArray.add(spring);
            }
            
            /*
             to make this dependent on scale degree (so the interval springs have a fundamental), we need something like
             
             diff = (scaledegree1 + intervalTuning[(scaledegree1 - fundamental) % 12]) -
             (scaledegree2 + intervalTuning[(scaledegree2 - fundamental) % 12]])
             
             where  scaledegree1 = particleArray[i]->getNote() = i
             and    scaledegree2 = particleArray[j]->getNote() = j
             
             so, if i = 64, and j = 62, and fundamental is C (0)
             diff = (64 + intervalTuning[64 % 12 = 4]) -
             (62 + intervalTuning[62 % 12 = 2]);
             
             or, if i = 66, and j = 64, and fundamental is D (2)
             diff = (66 + intervalTuning[64 % 12 = 4]) -
             (64 + intervalTuning[62 % 12 = 2])
             */
        }
    }
	
    
    setStiffness(1.0);
	numNotes = 0;
    if (st != nullptr) copy(st);
    setDrag(drag);
    setRate(rate);
}

void SpringTuning::setTetherTuning(Array<float> tuning)
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

void SpringTuning::setIntervalTuning(Array<float> tuning)
{
    intervalTuning = tuning;
    
    for (auto spring : enabledSpringArray)
    {
        int interval = spring->getIntervalIndex();
        int diff = spring->getA()->getRestX() - spring->getB()->getRestX();
        
        spring->setRestingLength(fabs(diff) + intervalTuning[interval]);
    }

#if 0
    if(!usingFundamentalForIntervalSprings)
    {
        for (auto spring : springArray)
        {
            int interval = spring->getIntervalIndex();
            int diff = spring->getA()->getRestX() - spring->getB()->getRestX();

            spring->setRestingLength(fabs(diff) + intervalTuning[interval]);
        }
    }
    else
    {
        for (auto spring : springArray)
        {
            /*
            int scaleDegree1 = spring->getA()->getNote();
            int scaleDegree2 = spring->getB()->getNote();
            int intervalFundamental = 0; //temporary, will set in preparation
            
            float diff = (100. * scaleDegree2 + intervalTuning[(scaleDegree2 - intervalFundamental) % 12]) -
                         (100. * scaleDegree1 + intervalTuning[(scaleDegree1 - intervalFundamental) % 12]);

            spring->setRestingLength(fabs(diff));
            */
            
            retuneIndividualSpring(spring);
        }
    }
#endif
}

void SpringTuning::simulate()
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

	for (auto spring : enabledSpringArray)
	{
        spring->satisfyConstraints();
	}
}

void SpringTuning::setSpringWeight(int which, double weight)
{
    which += 1;
    
    springWeights[which] = weight;
    
    for (auto spring : enabledSpringArray)
    {
        if (spring->getIntervalIndex() == which)
        {
            spring->setStrength(weight);
        }
    }
}

double SpringTuning::getSpringWeight(int which)
{
    which += 1;
    
    return springWeights[which];
}

void SpringTuning::setTetherWeight(int which, double weight)
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

double SpringTuning::getTetherWeight(int which)
{
    return tetherSpringArray[which]->getStrength();
}

String SpringTuning::getTetherSpringName(int which)
{
    return tetherSpringArray[which]->getName();
}

String SpringTuning::getSpringName(int which)
{
    for (auto spring : springArray)
    {
        if (spring->getIntervalIndex() == which) return spring->getName();
    }
    return "";
}

void SpringTuning::toggleSpring()
{
	//tbd
}

void SpringTuning::addParticle(int note)
{
    particleArray[note]->setEnabled(true);
    tetherParticleArray[note]->setEnabled(true);
}
void SpringTuning::removeParticle(int note)
{
    Particle* p = particleArray[note];
    p->setEnabled(false);
    tetherParticleArray[note]->setEnabled(false);
}
void SpringTuning::addNote(int note)
{
    //change interval fundamental here, depending on mode?
    addParticle(note);
    addSpringsByNote(note);
}

void SpringTuning::removeNote(int note)
{
    removeParticle(note);
    removeSpringsByNote(note);
}

void SpringTuning::removeAllNotes(void)
{
    for (int i = 0; i < 128; i++) removeNote(i);
}

void SpringTuning::toggleNote(int noteIndex)
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

void SpringTuning::addSpring(Spring::Ptr spring)
{
    int interval = spring->getIntervalIndex();
    
    spring->setEnabled(true);
    spring->setStiffness(intervalStiffness);
    spring->setStrength(springWeights[interval]);
    enabledSpringArray.add(spring);
    
    //if(usingFundamentalForIntervalSprings) retuneIndividualSpring(spring);
    
    int diff = spring->getA()->getRestX() - spring->getB()->getRestX();
    
    spring->setRestingLength(fabs(diff) + intervalTuning[interval]);
}

void SpringTuning::addSpringsByNote(int note)
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
				if (b->getEnabled())
                {
                    addSpring(spring);
                }
			}
			else if (b == p)
			{
				if (a->getEnabled())
                {
                    addSpring(spring);
                }
			}
        }
	}

    tetherSpringArray[note]->setEnabled(true);
    
}

void SpringTuning::retuneIndividualSpring(Spring::Ptr spring)
{
    int scaleDegree1 = spring->getA()->getNote();
    int scaleDegree2 = spring->getB()->getNote();
    int intervalFundamental = 0; //temporary, will set in preparation
    
    float diff = (100. * scaleDegree2 + intervalTuning[(scaleDegree2 - intervalFundamental) % 12]) -
    (100. * scaleDegree1 + intervalTuning[(scaleDegree1 - intervalFundamental) % 12]);
    
    spring->setRestingLength(fabs(diff));
}

//DT: wondering if there is a more efficient way to do this, rather than reading throug the whole spring array?
void SpringTuning::removeSpringsByNote(int note)
{
	Particle* p = particleArray[note];
    
    int size = enabledSpringArray.size();
    for (int i = (size-1); i >= 0; i--)
	{
        Spring* spring = enabledSpringArray[i];
        Particle* a = spring->getA();
        Particle* b = spring->getB();
        
        DBG("spring: " + spring->getName());
        
		if (spring->getEnabled() && ((a == p) || (b == p)))
        {
            spring->setEnabled(false);
            enabledSpringArray.remove(i);
        }
	}
    
    tetherSpringArray[note]->setEnabled(false);
}

double SpringTuning::getFrequency(int note)
{
	return Utilities::centsToFreq((int) particleArray[note]->getX());
}

void SpringTuning::print()
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

void SpringTuning::printParticles()
{
	for (int i = 0; i < 128; i++)
	{
		particleArray[i]->print();
	}
}

void SpringTuning::printActiveParticles()
{
	for (int i = 0; i < 128; i++)
	{
		if (particleArray[i]->getEnabled()) particleArray[i]->print();
	}
}

void SpringTuning::printActiveSprings()
{
	for (auto spring : springArray)
	{
		if (spring->getEnabled()) spring->print();
	}
}

bool SpringTuning::checkEnabledParticle(int index)
{
	return particleArray[index]->getEnabled();
}
