/*
  ==============================================================================

    SpringTuning.cpp
    Created: 3 Aug 2018 3:43:46pm
    Author:  Theo Trevisan, Mike Mulshine, Dan Trueman
 
    Based on the Verlet mass/spring algorithm:
    Jakobsen, T. (2001). Advanced character physics.
     In IN PROCEEDINGS OF THE GAME DEVELOPERS CONFERENCE 2001, page 19.

  ==============================================================================
*/

#include "SpringTuning.h"
#include "SpringTuningUtilities.h"

using namespace std;

void SpringTuning::copy(SpringTuning::Ptr st)
{
    // DBG("SpringTuning::copy called!!");
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
    
    for (int i=0; i<12; i++)
    {
        setSpringMode(i, st->getSpringMode(i));
    }
    
    scaleId = st->getScaleId();
    
    setIntervalTuning(st->getIntervalTuning());
    setTetherTuning(st->getTetherTuning());
    
    setSpringWeights(st->getSpringWeights());
    setTetherWeights(st->getTetherWeights());
    
    setIntervalFundamental(st->getIntervalFundamental());
    //setUsingFundamentalForIntervalSprings(st->getUsingFundamentalForIntervalSprings());
    
    setFundamentalSetsTether(st->getFundamentalSetsTether());
    setTetherWeightGlobal(st->getTetherWeightGlobal());
    setTetherWeightSecondaryGlobal(st->getTetherWeightSecondaryGlobal());
}

void SpringTuning::performModification(SpringTuning::Ptr st, Array<bool> dirty, bool reverse)
{
    // DBG("SpringTuning::performModification called!!");
    if (dirty[TuningSpringRate]) rate.modify(st->rate, reverse);
    if (dirty[TuningSpringStiffness]) stiffness.modify(st->stiffness, reverse);
    if (dirty[TuningSpringActive]) active.modify(st->active, reverse);
    if (dirty[TuningSpringDrag]) drag.modify(st->drag, reverse);
    
    if (dirty[TuningSpringIntervalStiffness]) intervalStiffness.modify(st->intervalStiffness, reverse);
    if (dirty[TuningSpringTetherStiffness]) tetherStiffness.modify(st->tetherStiffness, reverse);
    
    if (dirty[TuningSpringIntervalScale]) scaleId.modify(st->scaleId, reverse);
    
    if (dirty[TuningSpringIntervalFundamental])
    {
        intervalFundamental.modify(st->intervalFundamental, reverse);
        intervalFundamentalChanged();
    }
    //setUsingFundamentalForIntervalSprings(st->getUsingFundamentalForIntervalSprings());
    
    if (dirty[TuningFundamentalSetsTether]) fundamentalSetsTether.modify(st->fundamentalSetsTether, reverse);
    if (dirty[TuningTetherWeightGlobal]) tetherWeightGlobal.modify(st->tetherWeightGlobal, reverse);
    if (dirty[TuningTetherWeightGlobal2]) tetherWeightSecondaryGlobal.modify(st->tetherWeightSecondaryGlobal, reverse);
}

void SpringTuning::stepModdables()
{
    rate.step();
    stiffness.step();
    active.step();
    drag.step();
    intervalStiffness.step();
    tetherStiffness.step();
    scaleId.step();
    intervalFundamental.step();
    fundamentalSetsTether.step();
    tetherWeightGlobal.step();
    tetherWeightSecondaryGlobal.step();
}

void SpringTuning::resetModdables()
{
    rate.reset();
    stiffness.reset();
    active.reset();
    drag.reset();
    intervalStiffness.reset();
    tetherStiffness.reset();
    scaleId.reset();
    intervalFundamental.reset();
    fundamentalSetsTether.reset();
    tetherWeightGlobal.reset();
    tetherWeightSecondaryGlobal.reset();
}

SpringTuning::SpringTuning(SpringTuning::Ptr st):
rate(100),
stiffness(1.0),
tetherStiffness(0.5),
intervalStiffness(0.5),
drag(.15),
active(false),
fundamentalSetsTether(true),
tetherWeightGlobal(0.5),
tetherWeightSecondaryGlobal(0.1),
scaleId(JustTuning),
intervalFundamental(PitchClass(12)),
usingFundamentalForIntervalSprings(false), //should be false by default
intervalFundamentalActive(PitchClass(12))
{
    particleArray.ensureStorageAllocated(128);
    tetherParticleArray.ensureStorageAllocated(128);
    
    enabledSpringArray.ensureStorageAllocated(128);
    enabledSpringArray.clear();
    
    tetherTuning = Array<float>({0,0,0,0,0,0,0,0,0,0,0,0});
    
    springMode.ensureStorageAllocated(12);
    for(int i=0; i<12; i++) springMode.insert(i, true);
    
    intervalTuning = Array<float>({0.0, 0.117313, 0.039101, 0.156414, -0.13686, -0.019547, -0.174873, 0.019547, 0.136864, -0.15641, -0.311745, -0.11731});
    
    useLowestNoteForFundamental = false;
    useHighestNoteForFundamental = false;
    useLastNoteForFundamental = false;

    setFundamentalSetsTether(true);        //needs UI toggle; when on, don't show tether sliders for current notes, only the two tether weight sliders
    tetherFundamental = C;

    //setTetherWeightGlobal(0.5);             //needs UI slider; only show when fundamentalSetsTether == true
    //setTetherWeightSecondaryGlobal(0.1);    //needs UI slider; only show when fundamentalSetsTether == true
    
    for (int i = 0; i < 13; i++) springWeights[i] = 0.5;
    springWeights[7] = 0.75;
    springWeights[9] = 0.25;
    setSpringMode(6, false);
    
    // Converting std::string to juce::String takes time so
    // convert before the look
    // (was happening implicitly at spring->setName() and costing a lot of time
    Array<String> labels;
    for (auto label : intervalLabels)
        labels.add(String(label));
    
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
        
        Spring* s = new Spring(p1, p2, 0.0, 0.5, 0, labels.getUnchecked(0), false);
        tetherSpringArray.add(s);
	}
    
	numNotes = 0;
    if (st != nullptr) copy(st);
    setDrag(drag.value);
    setRate(rate.value);
    setIntervalFundamental((PitchClass)(automatic + 1));
}

void SpringTuning::setTetherTuning(Array<float> tuning)
{
    tetherTuning = tuning;
    
    for (int i = 0; i < 128; i++)
    {
        tetherParticleArray[i]->setX( (i * 100.0) + tetherTuning[(i-tetherFundamental) % 12] );
        tetherParticleArray[i]->setRestX( (i * 100.0) + tetherTuning[(i-tetherFundamental) % 12] );
        
        //DBG("rest X: " + String((i * 100.0) + tetherTuning[i % 12]));
        particleArray[i]->setRestX( (i * 100.0) + tetherTuning[(i-tetherFundamental) % 12] );
    }
}

void SpringTuning::setTetherFundamental(PitchClass newfundamental)
{
    tetherFundamental = newfundamental;
    setTetherTuning(getTetherTuning());
}

void SpringTuning::setIntervalTuning(Array<float> tuning)
{
    intervalTuning = tuning;
}

/*
simulate() first moves through the entire particle array and "integrates" their position,
moving them based on their "velocities" and the drag values

it then moves through both spring arrays (the tether springs and interval springs) and
calls satisfyConstraints(), which updates the spring values based on the spring strengths,
stiffnesses, and offsets from their rest lengths. This in turn updates the target positions
for the two particles associated with each spring.
*/
void SpringTuning::simulate()
{
    // update particle positions based on current velocities
    for (auto particle : particleArray)
    {
		if (particle->getEnabled() && !particle->getLocked())
        {
            particle->integrate(drag.value);
        }
	}
    
    // apply tether spring forces to all particles
    for (auto spring : tetherSpringArray)
    {
        if (spring->getEnabled())
        {
            spring->satisfyConstraints();
        }
    }

    // apply interval spring forces to all particless
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
            
            // DBG("reweighting interval " + String(which) +  " to " + String(weight));
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

    // DBG("SpringTuning::setTetherWeight " + String(which) + " " + String(weight));
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
    addParticle(note);
    // DBG("SpringTuning::addNote, useAutomaticFundamental " + String((int)useAutomaticFundamental));
    
    if(useLowestNoteForFundamental)
    {
        // DBG("lowest current note = " + String(getLowestActiveParticle()));
        intervalFundamentalActive = (PitchClass)(getLowestActiveParticle() % 12);
        
        if(fundamentalSetsTether.value) setTetherFundamental(intervalFundamentalActive);
    }
    else if(useHighestNoteForFundamental)
    {
        // DBG("highest current note = " + String(getHighestActiveParticle()));
        intervalFundamentalActive = (PitchClass)(getHighestActiveParticle() % 12);
        
        if(fundamentalSetsTether.value) setTetherFundamental(intervalFundamentalActive);
    }
    else if(useLastNoteForFundamental)
    {
        // DBG("last note = " + String(note));
        intervalFundamentalActive = (PitchClass)(note % 12);
        
        if(fundamentalSetsTether.value) setTetherFundamental(intervalFundamentalActive);
    }
    else if(useAutomaticFundamental)
    {
        findFundamental(); //sets intervalFundamental internally
        if(fundamentalSetsTether.value) setTetherFundamental(intervalFundamentalActive);
    }
    
    addSpringsByNote(note);
    
    if(useLowestNoteForFundamental || useHighestNoteForFundamental || useLastNoteForFundamental || useAutomaticFundamental) retuneAllActiveSprings();
}

void SpringTuning::removeNote(int note)
{
    removeParticle(note);
    
    if(useLowestNoteForFundamental)
    {
        // DBG("lowest current note = " + String(getLowestActiveParticle()));
        intervalFundamentalActive = (PitchClass)(getLowestActiveParticle() % 12);
    }
    else if(useHighestNoteForFundamental)
    {
        // DBG("highest current note = " + String(getHighestActiveParticle()));
        intervalFundamentalActive = (PitchClass)(getHighestActiveParticle() % 12);
    }
    else if(useAutomaticFundamental)
    {
        findFundamental();
        if(fundamentalSetsTether.value) setTetherFundamental(intervalFundamentalActive);
    }
    
    removeSpringsByNote(note);
    
    if(useLowestNoteForFundamental || useHighestNoteForFundamental || useAutomaticFundamental) retuneAllActiveSprings();
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

void SpringTuning::findFundamental()
{
    //create sorted array of notes
    Array<int> enabledNotes;
    for (int i=127; i>=0; i--)
    {
        if(particleArray[i]->getEnabled())
        {
            enabledNotes.insert(0, i);
             //DBG("enabledNotes = " + String(i));
        }
    }
    
    //if(enabledNotes.size() > 1)
    {
        int fundamental_57 = -1;
        int fundamental_48 = -1;
        int fundamental_39 = -1;
        
        for(int i=enabledNotes.size() - 1; i>0; i--)
        {
            for(int j=i-1; j>=0; j--)
            {
                int interval = (enabledNotes[i] - enabledNotes[j]) % 12;
                
                if(interval == 7)
                {
                    fundamental_57 = enabledNotes[j] % 12;
                    //DBG("Fifth fundamental 5 = " + String(fundamental_57));
                }
                else if(interval == 5)
                {
                    fundamental_57 = enabledNotes[i] % 12;
                    //DBG("Fifth fundamental 4 = " + String(fundamental_57));
                }
                else if(interval == 4)
                {
                    fundamental_48 = enabledNotes[j] % 12;
                    //DBG("Third fundamental 3 = " + String(fundamental_48));
                }
                else if(interval == 8)
                {
                    fundamental_48 = enabledNotes[i] % 12;
                    //DBG("Third fundamental 6 = " + String(fundamental_48));
                }
                else if(interval == 3)
                {
                    fundamental_39 = (enabledNotes[j] - 4) % 12;
                    //DBG("MinorThird fundamental 3 = " + String(fundamental_39));
                }
                else if(interval == 9)
                {
                    fundamental_39 = (enabledNotes[i] - 4) % 12;
                    //DBG("MinorThird fundamental 6 = " + String(fundamental_39));
                }
            }
        }
        
        if(fundamental_57 > -1)
        {
            // DBG("CHOICE = " + String(fundamental_57));
            intervalFundamentalActive = (PitchClass(fundamental_57));
        }
        else if(fundamental_48 > -1)
        {
            // DBG("CHOICE = " + String(fundamental_48));
            intervalFundamentalActive = (PitchClass(fundamental_48));
            
        }
        else if(fundamental_39 > -1)
        {
            // DBG("CHOICE = " + String(fundamental_39));
            intervalFundamentalActive = (PitchClass(fundamental_39));
            
        }
    }
}

void SpringTuning::addSpring(Spring::Ptr spring)
{
    if (enabledSpringArray.contains(spring)) return;
    int interval = spring->getIntervalIndex();
    
    spring->setEnabled(true);
    spring->setStiffness(intervalStiffness.value);
    spring->setStrength(springWeights[interval]);
    enabledSpringArray.add(spring);

    retuneIndividualSpring(spring);
}

void SpringTuning::addSpringsByNote(int note)
{
    for (auto p : particleArray)
    {
        int otherNote = p->getNote();
        if (otherNote == note) continue;
        if (p->getEnabled())
        {
            int upperNote = note > otherNote ? note : otherNote;
            int lowerNote = note < otherNote ? note : otherNote;
            int hash = (upperNote << 16 | lowerNote);
            if (!springArray.contains(hash))
            {
                float diff = upperNote - lowerNote;
                
                int interval = (int)diff % 12;
                int octInterval = interval;
                
                if (diff != 0 && interval == 0)
                {
                    octInterval = 12;
                }
                
                if (usingFundamentalForIntervalSprings)
                {
                    int scaleDegree1 = particleArray.getUnchecked(upperNote)->getNote();
                    int scaleDegree2 = particleArray.getUnchecked(lowerNote)->getNote();;
                    //int intervalFundamental = 0; //temporary, will set in preparation
                    
                    diff = fabs(100. *
                    ((scaleDegree1 +
                      intervalTuning.getUnchecked((scaleDegree1 - (int)intervalFundamentalActive) % 12)) -
                    (scaleDegree2 +
                     intervalTuning.getUnchecked((scaleDegree2 - (int)intervalFundamentalActive) % 12))));
                }
                else diff = diff * 100 + intervalTuning.getUnchecked(interval) * 100;

                springArray.set(hash, new Spring(particleArray.getUnchecked(lowerNote),
                                                 particleArray.getUnchecked(upperNote),
                                                 diff, //rest length in cents
                                                 0.5,
                                                 octInterval,
                                                 intervalLabels[octInterval],
                                                 false));
            }
            addSpring(springArray[hash]);
        }
    }

    tetherSpringArray[note]->setEnabled(true);
    
    if(getFundamentalSetsTether())
    {
        for (auto tether : tetherSpringArray)
        {
            if(tether->getEnabled())
            {
                int tnoteA = tether->getA()->getNote();
                int tnoteB = tether->getB()->getNote();

                if(tnoteA % 12 == getTetherFundamental() || tnoteB % 12 == getTetherFundamental())
                {
                    // DBG("SpringTuning::addSpringsByNote getTetherWeightGlobal = " + String((int)getTetherWeightGlobal()));
                    setTetherWeight(tnoteA, getTetherWeightGlobal());
                }
                else{
                    setTetherWeight(tnoteA, getTetherWeightSecondaryGlobal());
                }
            }
        }
    }
    
    
}

void SpringTuning::retuneIndividualSpring(Spring::Ptr spring)
{
    int interval = spring->getIntervalIndex();
    
    //set spring length locally, for all if !usingFundamentalForIntervalSprings, or for individual springs as set by L/F
    if(!usingFundamentalForIntervalSprings ||
       !getSpringMode(interval - 1))
    {
        int diff = spring->getA()->getRestX() - spring->getB()->getRestX();
        spring->setRestingLength(fabs(diff) + intervalTuning[interval]);
    }
    
    //otherwise, set resting length to interval scale relative to intervalFundamental (F)
    else
    {
        int scaleDegree1 = spring->getA()->getNote();
        int scaleDegree2 = spring->getB()->getNote();
        //int intervalFundamental = 0; //temporary, will set in preparation
        
        float diff =    (100. * scaleDegree2 + intervalTuning[(scaleDegree2 - (int)intervalFundamentalActive) % 12]) -
        (100. * scaleDegree1 + intervalTuning[(scaleDegree1 - (int)intervalFundamentalActive) % 12]);
        
        spring->setRestingLength(fabs(diff));
    }
}

void SpringTuning::retuneAllActiveSprings(void)
{
    for (auto spring : enabledSpringArray)
    {
        retuneIndividualSpring(spring);
    }
}

void SpringTuning::removeSpringsByNote(int note)
{
	Particle* p = particleArray[note];
    
    int size = enabledSpringArray.size();
    for (int i = (size-1); i >= 0; i--)
	{
        Spring* spring = enabledSpringArray[i];
        Particle* a = spring->getA();
        Particle* b = spring->getB();
    
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
		if(particleArray[i]->getEnabled()) particleArray[i]->print();
	}
}

int SpringTuning::getLowestActiveParticle()
{
    int lowest = 0;
    
    while(lowest < particleArray.size())
    {
        if(particleArray[lowest]->getEnabled()) return lowest;
        
        lowest++;
    }
    
    return lowest;
}

int SpringTuning::getHighestActiveParticle()
{
    int highest = particleArray.size() - 1;
    
    while(highest >= 0)
    {
        if(particleArray[highest]->getEnabled()) return highest;
        
        highest--;
    }
    
    return highest;
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
