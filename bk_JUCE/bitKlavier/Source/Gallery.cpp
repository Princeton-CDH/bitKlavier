/*
  ==============================================================================

    Gallery.cpp
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Gallery.h"

#include "PluginProcessor.h"

Gallery::Gallery(BKAudioProcessor& p):
processor(p)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        used.add(Array<int>({-1}));
        idmap.add(new HashMap<int,int>());
    }
    
    // DBG("idmap size: " + String(idmap.size()));
    
    general = new GeneralSettings();
    
    addDefaultPrepIfNotThere();
    
    isDirty = false;
}

Gallery::Gallery(XmlElement* xml, BKAudioProcessor& p):
processor(p)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        used.add( Array<int>( {-1} ) );
        idmap.add(new HashMap<int,int>());
    }
    
    // DBG("idmap size: " + String(idmap.size()));
    
    general = new GeneralSettings();

    setStateFromXML(xml);
    
    addDefaultPrepIfNotThere();
    
    isDirty = false;
}

Gallery::Gallery(var myJson, BKAudioProcessor& p):
processor(p),
url(String())
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idcounts[i] = 1;
        idmap.add(new HashMap<int,int>());
    }
    
    // DBG("idmap size: " + String(idmap.size()));
    
    general = new GeneralSettings();

    addDefaultPrepIfNotThere();
    
    setStateFromJson(myJson);
    
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        used.add(Array<int>({-1}));
    }
    
    isDirty = false;
}



void Gallery::prepareToPlay (double sampleRate)
{
    for (auto piano : bkPianos) piano->prepareToPlay(sampleRate);
}

Gallery::~Gallery()
{
    bkPianos.clear();
}

void Gallery::resetPreparations(void)
{
    // Optimizations can be made here. Don't need to iterate through EVERY preparation.
    for (int i = direct.size(); --i >= 0; )
        direct[i]->prep->resetModdables();
    
    for (int i = nostalgic.size(); --i >= 0; )
        nostalgic[i]->prep->resetModdables();
    
    for (int i = synchronic.size(); --i >= 0; )
        synchronic[i]->prep->resetModdables();
    
    for (int i = resonance.size(); --i >= 0; )
        resonance[i]->prep->resetModdables();
    
    for (int i = tuning.size(); --i >= 0; )
        tuning[i]->prep->resetModdables();
    
    for (int i = tempo.size(); --i >= 0; )
        tempo[i]->prep->resetModdables();

	for (int i = blendronic.size(); --i >= 0; )
        blendronic[i]->prep->resetModdables();
}

void Gallery::randomize()
{
	Random::getSystemRandom().setSeedRandomly();

    GeneralSettings::Ptr dummyGeneral = new GeneralSettings();
    BKSynthesiser* dummySynth = new BKSynthesiser(processor, dummyGeneral);

    //each piano
    for (int h = 0; h < Random::getSystemRandom().nextInt(Range<int>(1, 5)); h++)
    {
        Piano::Ptr p = new Piano(processor, h);
        addPiano(p);
        //each set of preparations in the piano
        for (int i = 0; i < Random::getSystemRandom().nextInt(Range<int>(1, 10)); i++)
        {
            Keymap::Ptr kp = new Keymap(processor);
            kp->randomize();
            addKeymap(kp);

            Tuning::Ptr t = new Tuning(-1, true);
            addTuning(t);
            int tuningId = t->getId();
            TuningProcessor::Ptr tProc = new TuningProcessor(processor, t);
            p->addTuningProcessor(tuningId);

			Tempo::Ptr m = new Tempo(-1, true);
			addTempo(m);
			int tempoId = m->getId();
			TempoProcessor::Ptr mProc = new TempoProcessor(processor, m);
			p->addTempoProcessor(tempoId);

			Blendronic::Ptr b = new Blendronic(-1, true);
			addBlendronic(b);
			int blendronicId = b->getId();
			BlendronicProcessor::Ptr bProc = new BlendronicProcessor(b, mProc, dummyGeneral, dummySynth);
			p->addBlendronicProcessor(blendronicId);

            Direct::Ptr d = new Direct(-1, true);
            addDirect(d);
            int directId = d->getId();
            DirectProcessor::Ptr dProc = new DirectProcessor(d, tProc, bProc, dummySynth, dummySynth, dummySynth);
            p->addDirectProcessor(directId);

            Synchronic::Ptr s = new Synchronic(-1, true);
            addSynchronic(s);
            int synchronicId = s->getId();
            SynchronicProcessor::Ptr sProc = new SynchronicProcessor(s, tProc, mProc, bProc, dummySynth, dummyGeneral);
            p->addSynchronicProcessor(synchronicId);

            Nostalgic::Ptr n = new Nostalgic(-1, true);
            addNostalgic(n);
            int nostalgicId = n->getId();
            NostalgicProcessor::Ptr nProc = new NostalgicProcessor(n, tProc, sProc, bProc, dummySynth);
            p->addNostalgicProcessor(nostalgicId);

            p->linkPreparationWithKeymap(PreparationTypeDirect, d->getId(), kp->getId());

            p->linkPreparationWithTuning(PreparationTypeDirect, d->getId(), t);

            //randomly chooses whether to link nostalgic with keymap or tuning
            if (Random::getSystemRandom().nextFloat() > 0.5) p->linkPreparationWithKeymap(PreparationTypeNostalgic, nProc->getId(), kp->getId());
            else p->linkPreparationWithTuning(PreparationTypeNostalgic, nProc->getId(), t);

            //randomly chooses whether to link synchronic with keymap, tuning, or nostalgic
            float linkType = Random::getSystemRandom().nextFloat();
            if (linkType > 0.66) p->linkPreparationWithKeymap(PreparationTypeSynchronic, sProc->getId(), kp->getId());
            else if (linkType > 0.33) p->linkPreparationWithTuning(PreparationTypeSynchronic, sProc->getId(), t);
            else p->linkNostalgicWithSynchronic(n, s);

            p->linkPreparationWithTempo(PreparationTypeSynchronic, s->getId(), m);
        }
    }
}
