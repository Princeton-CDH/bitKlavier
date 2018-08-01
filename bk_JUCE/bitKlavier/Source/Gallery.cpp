/*
  ==============================================================================

    Gallery.cpp
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Gallery.h"

#include "PluginProcessor.h"

Gallery::Gallery(ScopedPointer<XmlElement> xml, BKAudioProcessor& p, bool firstTime):
processor(p)
{    
    general = new GeneralSettings();

    setStateFromXML(xml, firstTime);
    
    bool add = true;
    for (auto p : tempo) { if (p->getId() == -1) { add = false; break;} }
    if (add) addTempoWithId(-1);
    
    add = true;
    for (auto p : tuning) { if (p->getId() == -1) { add = false; break;} }
    if (add) addTuningWithId(-1);
    
    add = true;
    for (auto p : synchronic) { if (p->getId() == -1) { add = false; break;} }
    if (add) addSynchronicWithId(-1);
    
    add = true;
    for (auto p : nostalgic) { if (p->getId() == -1) { add = false; break;} }
    if (add) addNostalgicWithId(-1);
    
    add = true;
    for (auto p : direct) { if (p->getId() == -1) { add = false; break;} }
    if (add) addDirectWithId(-1);
    
    add = true;
    for (auto p : bkKeymaps) { if (p->getId() == -1) { add = false; break;} }
    if (add) addKeymapWithId(-1);
    
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        used.add(Array<int>({-1}));
    }
    
    isDirty = false;
}

Gallery::Gallery(var myJson, BKAudioProcessor& p):
processor(p),
url(String::empty)
{
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idCount.add(10);
    }
    
    general = new GeneralSettings();

    setStateFromJson(myJson);
    
    bool add = true;
    for (auto p : tempo) { if (p->getId() == -1) { add = false; break;} }
    if (add) addTempoWithId(-1);
    
    add = true;
    for (auto p : tuning) { if (p->getId() == -1) { add = false; break;} }
    if (add) addTuningWithId(-1);
    
    add = true;
    for (auto p : synchronic) { if (p->getId() == -1) { add = false; break;} }
    if (add) addSynchronicWithId(-1);
    
    add = true;
    for (auto p : nostalgic) { if (p->getId() == -1) { add = false; break;} }
    if (add) addNostalgicWithId(-1);
    
    add = true;
    for (auto p : direct) { if (p->getId() == -1) { add = false; break;} }
    if (add) addDirectWithId(-1);
    
    add = true;
    for (auto p : bkKeymaps) { if (p->getId() == -1) { add = false; break;} }
    if (add) addKeymapWithId(-1);
    
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        used.add(Array<int>({-1}));
    }
    
    isDirty = false;
}



void Gallery::prepareToPlay (double sampleRate)
{
    
    for (auto piano : bkPianos)     piano->prepareToPlay(sampleRate);
}

Gallery::~Gallery()
{
    bkPianos.clear();
}

void Gallery::resetPreparations(void)
{
    // Optimizations can be made here. Don't need to iterate through EVERY preparation.
    for (int i = direct.size(); --i >= 0; )
        direct[i]->aPrep->copy(direct[i]->sPrep);
    
    for (int i = nostalgic.size(); --i >= 0; )
        nostalgic[i]->aPrep->copy(nostalgic[i]->sPrep);
    
    for (int i = synchronic.size(); --i >= 0; )
        synchronic[i]->aPrep->copy(synchronic[i]->sPrep);
    
    for (int i = tuning.size(); --i >= 0; )
        tuning[i]->aPrep->copy(tuning[i]->sPrep);
    
    for (int i = tempo.size(); --i >= 0; )
        tempo[i]->aPrep->copy(tempo[i]->sPrep);
}

void Gallery::randomize()
	{
		BKAudioProcessor dummyAudio;
		BKSynthesiser* dummySynth;
		GeneralSettings::Ptr dummyGeneral = new GeneralSettings();

		//each piano
		for (int h = 0; h < Random::getSystemRandom().nextInt(Range<int>(1, 5)); h++)
		{
			Piano::Ptr p = new Piano(dummyAudio, h);
			addPiano(p);
			//each set of preparations in the piano
			for (int i = 0; i < Random::getSystemRandom().nextInt(Range<int>(1, 10)); i++)
			{
				Keymap::Ptr kp = new Keymap();
				kp->randomize();
				addKeymap(kp);
				p->addPreparationMap(kp);

				Tuning::Ptr t = new Tuning(-1, true);
				addTuning(t);
				TuningProcessor::Ptr tProc = new TuningProcessor(t);
				p->addTuningProcessor(tProc->getId());

				Direct::Ptr d = new Direct(-1, true);
				addDirect(d);
				DirectProcessor::Ptr dProc = new DirectProcessor(d, tProc, dummySynth, dummySynth, dummySynth);
				p->addDirectProcessor(dProc->getId());

				Tempo::Ptr m = new Tempo(-1, true);
				addTempo(m);
				TempoProcessor::Ptr mProc = new TempoProcessor(m);
				p->addTempoProcessor(mProc->getId());

				Synchronic::Ptr s = new Synchronic(-1, true);
				addSynchronic(s);
				SynchronicProcessor::Ptr sProc = new SynchronicProcessor(s, tProc, mProc, dummySynth, dummyGeneral);
				p->addSynchronicProcessor(sProc->getId());

				Nostalgic::Ptr n = new Nostalgic(-1, true);
				addNostalgic(n);
				NostalgicProcessor::Ptr nProc = new NostalgicProcessor(n, tProc, sProc, dummySynth);
				p->addNostalgicProcessor(nProc->getId());

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

				p->linkSynchronicWithTempo(s, m);
			}
		}
	}
/* ///unit test currently commented out because it creates infinite loop
#if BK_UNIT_TESTS

class GalleryTests : public UnitTest
{
public:
	GalleryTests() : UnitTest("Galleries", "Gallery") {}

	void runTest() override
	{
		beginTest("GalleryXML");

		for (int i = 0; i < 10; i++)
		{
			// create gallery and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random gallery " + String(i);
			DBG("test consistency: " + name);

			ScopedPointer<XmlElement> dummyXml;
			BKAudioProcessor dummyAudio;

			Gallery g1(dummyXml, dummyAudio, true);
			DBG("gallery created");
			g1.randomize();
			DBG("randomize done");
			g1.setName(name);

			ValueTree vt1 = g1.getState();

			ScopedPointer<XmlElement> xml = vt1.createXml();

			Gallery g2(xml, dummyAudio, true);
			g2.setStateFromXML(xml);
			g2.setName(name);

			ValueTree vt2 = g2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"gallery: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());

			//expect(tp2->compare(tp1), tp1->getName() + " and " + tp2->getName() + " did not match.");
		}
	}
};

static GalleryTests galleryTest;

#endif
*/