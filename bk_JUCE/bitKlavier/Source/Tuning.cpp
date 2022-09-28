/*
 ==============================================================================
 
 Tuning.cpp
 Created: 4 Dec 2016 3:40:27pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#include "Tuning.h"
#include "PluginProcessor.h"
#include "Modification.h"


void TuningPreparation::performModification(TuningModification* p, Array<bool> dirty)
{
    bool reverse = p->altMod && modded;
    
    if (dirty[TuningScale]) tScale.modify(p->tScale, reverse);
    if (dirty[TuningFundamental]) tFundamental.modify(p->tFundamental, reverse);
    if (dirty[TuningOffset]) tFundamentalOffset.modify(p->tFundamentalOffset, reverse);
    if (dirty[TuningA1IntervalScale]) tAdaptiveIntervalScale.modify(p->tAdaptiveIntervalScale, reverse);
    if (dirty[TuningA1Inversional]) tAdaptiveInversional.modify(p->tAdaptiveInversional, reverse);
    if (dirty[TuningA1AnchorScale]) tAdaptiveAnchorScale.modify(p->tAdaptiveAnchorScale, reverse);
    if (dirty[TuningA1AnchorFundamental]) tAdaptiveAnchorFundamental.modify(p->tAdaptiveAnchorFundamental, reverse);
    if (dirty[TuningA1ClusterThresh]) tAdaptiveClusterThresh.modify(p->tAdaptiveClusterThresh, reverse);
    if (dirty[TuningA1History]) tAdaptiveHistory.modify(p->tAdaptiveHistory, reverse);
    if (dirty[TuningCustomScale])
    {
        // Why was this done this way?
        //            // tCustom = p->getCustomScale();
        //            Array<float> temp = p->getCustomScale();
        //            for (int i = 0; i < tCustom.size(); i++) tCustom.set(i, temp[i]);
        //            //tCustom = Array<float>(p->getCustomScale());
        //            tScale = CustomTuning;
        tCustom.modify(p->tCustom, reverse);
    }
    if (dirty[TuningAbsoluteOffsets]) tAbsolute.modify(p->tAbsolute, reverse);
    if (dirty[TuningNToneSemitoneWidth]) nToneSemitoneWidth.modify(p->nToneSemitoneWidth, reverse);
    if (dirty[TuningNToneRootCB]) nToneRoot.modify(p->nToneRoot, reverse);
    if (dirty[TuningAdaptiveSystem]) adaptiveType.modify(p->adaptiveType, reverse);
    
    stuning->performModification(p->getSpringTuning(), dirty, reverse);
    
    if (adaptiveType.value == AdaptiveSpring)
    {
        stuning->setActive(true);
    }
    else
    {
        stuning->setActive(false);
    }
    
    modded = !reverse;
}




TuningProcessor::TuningProcessor(BKAudioProcessor& processor, Tuning::Ptr tuning):
processor(processor),
tuning(tuning),
keymaps(Keymap::PtrArr()),
lastNoteTuning(0),
lastIntervalTuning(0)


{
    for (int j = 0; j < 128; j++)
    {
        velocities.add(Array<float>());
        invertVelocities.add(Array<float>());
        for (int i = 0; i < TargetTypeNil; ++i)
        {
            velocities.getReference(j).add(-1.f);
            invertVelocities.getReference(j).add(-1.f);
        }
    }
}

TuningProcessor::~TuningProcessor()
{
    if(!MTS_CanRegisterMaster()) MTS_DeregisterMaster();
    if(MTS_HasMaster(tuning->prep->client) && tuning->prep->client != nullptr)
        MTS_DeregisterClient(tuning->prep->client);
}

//returns offsets; main callback
float TuningProcessor::getOffset(int midiNoteNumber, bool updateLastInterval)
{
        
    float lastNoteTuningTemp = lastNoteTuning;
    float lastNoteOffset;
 
    //do adaptive tunings if using
    if(tuning->prep->getAdaptiveType() == AdaptiveNormal || tuning->prep->getAdaptiveType() == AdaptiveAnchored)
    {
        lastNoteOffset = adaptiveCalculate(midiNoteNumber) + tuning->prep->getFundamentalOffset(); // added getFundamentalOffset()
        
        if(updateLastInterval)
        {
            lastNoteTuning = midiNoteNumber + lastNoteOffset;
            lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
        }
//        if (tuning->prep->isMTSMaster)
//        {
//            DBG("Master Midinote: "+String(midiNoteNumber) + "New MidiNote: " + String(midiNoteNumber + lastNoteOffset));
//            MTS_SetNoteTuning((double)mtof(midiNoteNumber + lastNoteOffset), (char)midiNoteNumber);
//        }
        return lastNoteOffset;
    }
    
    
    //do nTone tuning if nToneSemitoneWidth != 100cents
    if(tuning->prep->getNToneSemitoneWidth() != 100)
    {
        lastNoteOffset = .01 * (midiNoteNumber - tuning->prep->getNToneRoot()) * (tuning->prep->getNToneSemitoneWidth() - 100);
        int midiNoteNumberTemp = round(midiNoteNumber + lastNoteOffset);
        
        Array<float> currentTuning;
        if(tuning->prep->getScale() == CustomTuning) currentTuning = tuning->prep->getCustomScale();
        else currentTuning = tuning->tuningLibrary.getUnchecked(tuning->prep->getScale());
        
        lastNoteOffset += (currentTuning[(midiNoteNumberTemp - tuning->prep->getFundamental()) % currentTuning.size()]
                          + tuning->prep->getAbsoluteOffsets().getUnchecked(midiNoteNumber)
                          + tuning->prep->getFundamentalOffset());
        
        if(updateLastInterval)
        {
            lastNoteTuning = midiNoteNumber + lastNoteOffset;
            lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
        }
//        if (tuning->prep->isMTSMaster)
//        {
//            DBG("Master Midinote: "+String(midiNoteNumber) + "New MidiNote: " + String(midiNoteNumber + lastNoteOffset));
//            MTS_SetNoteTuning((double)mtof(midiNoteNumber + lastNoteOffset), (char)midiNoteNumber);
//        }
        return lastNoteOffset;
    }
    
    if (tuning->prep->hasMTSMaster() && tuning->prep->client != nullptr)
    {
        float freq = MTS_NoteToFrequency(tuning->prep->client, (char)midiNoteNumber, -1);
        float newmidi = ftom(freq);
        lastNoteOffset = newmidi - midiNoteNumber ;
        if (updateLastInterval)
        {
            lastNoteTuning = midiNoteNumber + lastNoteOffset;
            lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
        }
        DBG("Client Midinote: "+String(midiNoteNumber) + "New MidiNote: " + String(midiNoteNumber + lastNoteOffset));
        
        return lastNoteOffset;
    }
    
    //else do regular tunings
    Array<float> currentTuning;
    if(tuning->prep->getScale() == CustomTuning)
        currentTuning = tuning->prep->getCustomScale();
    else
        currentTuning = tuning->tuningLibrary.getUnchecked(tuning->prep->getScale());

    lastNoteOffset = (currentTuning[(midiNoteNumber - tuning->prep->getFundamental()) % currentTuning.size()]
                      + tuning->prep->getAbsoluteOffsets().getUnchecked(midiNoteNumber)
                      + tuning->prep->getFundamentalOffset());

    if(updateLastInterval)
    {
        lastNoteTuning = midiNoteNumber + lastNoteOffset;
        lastIntervalTuning = lastNoteTuning - lastNoteTuningTemp;
    }
    if (tuning->prep->isMTSMaster && !tuning->prep->getSpringsActive())
    {
        DBG("MAster Midinote: "+String(midiNoteNumber) + "New MidiNote: " + String(midiNoteNumber + lastNoteOffset));
        MTS_SetNoteTuning((double)mtof(midiNoteNumber + lastNoteOffset), (char)midiNoteNumber);
    }
    return lastNoteOffset;
    
}

int TuningProcessor::getAdaptiveClusterTimer()
{
     return clusterTime * (1000.0 / processor.getCurrentSampleRate());
}


//for keeping track of current cluster size
void TuningProcessor::processBlock(int numSamples)
{
    TuningAdaptiveSystemType type = tuning->prep->getAdaptiveType();
    
    if (type == AdaptiveNormal || type == AdaptiveAnchored) {
        
        if(clusterTime <= (tuning->prep->getAdaptiveClusterThresh() * processor.getCurrentSampleRate() * 0.001))
            clusterTime += numSamples;
    }
}

void TuningProcessor::keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an inverted key press, aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &invertVelocities.getReference(noteNumber);
        for (int i = TargetTypeTuning; i < TargetTypeTuning+1; ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i));
        }
    }
    // If this an actual release, aVels will be the incoming velocities,
    // but bVels will use the values from the last press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = &velocities.getReference(noteNumber);
    }
    
    if (bVels->getUnchecked(TargetTypeTuning) < 0.f) return;
    
    tuning->prep->getSpringTuning()->removeNote(noteNumber);
}


//add note to the adaptive tuning history, update adaptive fundamental
void TuningProcessor::keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress)
{
    // aVels will be used for velocity calculations; bVels will be used for conditionals
    Array<float> *aVels, *bVels;
    // If this is an actual key press (not an inverted release) aVels and bVels are the same
    // We'll save and use the incoming velocity values
    if (fromPress)
    {
        aVels = bVels = &velocities.getReference(noteNumber);
        for (int i = TargetTypeTuning; i < TargetTypeTuning+1; ++i)
        {
            aVels->setUnchecked(i, targetVelocities.getUnchecked(i));
        }
    }
    // If this an inverted release, aVels will be the incoming velocities,
    // but bVels will use the values from the last inverted press (keyReleased with fromPress=true)
    else
    {
        aVels = &targetVelocities;
        bVels = &invertVelocities.getReference(noteNumber);
    }
    
    if (bVels->getUnchecked(TargetTypeTuning) < 0.f) return;
    //DBG("TuningProcessor::keyPressed " + String(midiNoteNumber));
    
    adaptiveHistoryCounter++;
    
    TuningAdaptiveSystemType type = tuning->prep->getAdaptiveType();

    if (type == AdaptiveNormal)
    {
        //if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory() - 1)
        if (clusterTime * (1000.0 / processor.getCurrentSampleRate()) > tuning->prep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->prep->getAdaptiveHistory())
        {
            adaptiveHistoryCounter = 0;
            adaptiveFundamentalFreq = adaptiveFundamentalFreq * adaptiveCalculateRatio(noteNumber);
            adaptiveFundamentalNote = noteNumber;
        }
        //else adaptiveHistoryCounter++;
        
    }
    else if (type == AdaptiveAnchored)
    {
        //if(clusterTime * (1000.0 / sampleRate) > tuning->aPrep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->aPrep->getAdaptiveHistory() - 1)
        if (clusterTime * (1000.0 / processor.getCurrentSampleRate()) > tuning->prep->getAdaptiveClusterThresh() || adaptiveHistoryCounter >= tuning->prep->getAdaptiveHistory())
        {
            adaptiveHistoryCounter = 0;
            
            const Array<float> anchorTuning = tuning->tuningLibrary.getUnchecked(tuning->prep->getAdaptiveAnchorScale());
            adaptiveFundamentalFreq = mtof(noteNumber +
                                           anchorTuning[(noteNumber + tuning->prep->getAdaptiveAnchorFundamental()) % anchorTuning.size()],
                                           globalTuningReference
                                           );
            adaptiveFundamentalNote = noteNumber;

        }
        //else adaptiveHistoryCounter++;
    }
    
    tuning->prep->getSpringTuning()->addNote(noteNumber);
    
    clusterTime = 0;
    
}

float TuningProcessor::adaptiveCalculateRatio(const int midiNoteNumber) const
{
    int tempnote = midiNoteNumber;
    float newnote;
    float newratio;
    
    Array<float> intervalScale;
    if(tuning->prep->getAdaptiveIntervalScale() == CustomTuning)
        intervalScale = tuning->prep->getCustomScale();
    else
        intervalScale = tuning->tuningLibrary.getUnchecked(tuning->prep->getAdaptiveIntervalScale());
    
    if(!tuning->prep->getAdaptiveInversional() || tempnote >= adaptiveFundamentalNote)
    {
        
        while((tempnote - adaptiveFundamentalNote) < 0) tempnote += 12;
    
        newnote = midiNoteNumber + intervalScale[(tempnote - adaptiveFundamentalNote) % intervalScale.size()];
        newratio = intervalToRatio(newnote - adaptiveFundamentalNote);
        
        return newratio;
        
    }
    //else
    newnote = midiNoteNumber - intervalScale[(adaptiveFundamentalNote - tempnote) % intervalScale.size()];
    newratio = intervalToRatio(newnote - adaptiveFundamentalNote);
    
    return newratio;
}

float TuningProcessor::adaptiveCalculate(int midiNoteNumber) const
{
    float newnote = adaptiveFundamentalFreq * adaptiveCalculateRatio(midiNoteNumber);
    return ftom(newnote, globalTuningReference) - midiNoteNumber;
}

void TuningProcessor::adaptiveReset()
{
    adaptiveFundamentalNote = tuning->prep->getFundamental();
    adaptiveFundamentalFreq = mtof(adaptiveFundamentalNote, globalTuningReference);
    adaptiveHistoryCounter = 0;
    
}


ValueTree Tuning::getState(bool active)
{
    ValueTree vt(vtagTuning);
    
    vt.setProperty( "Id", Id, 0);
    vt.setProperty( "name", name, 0);
    
    vt.addChild(prep->getState(), -1, 0);
    
    return vt;
}

void Tuning::setState(XmlElement* e)
{
    Id = e->getStringAttribute("Id").getIntValue();
    
    String n = e->getStringAttribute("name");
    
    if (n != String())     name = n;
    else                        name = String(Id);
    
    
    XmlElement* params = e->getChildByName("params");
    
    if (params != nullptr)
    {
        prep->setState(params);
    }
    else
    {
        prep->setState(e);
    }
}

void Tuning::loadScalaFile(std::string fname)
{
    Tunings::Scale s;
    try {
        s = Tunings::readSCLFile(fname);
    } catch (Tunings::TuningError t) {
        AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon, TRANS("Scala Loading Error"), TRANS(t.what()));
        return;
    }
    currentScale = s;
    loadScalaScale(s);
}

void Tuning::loadScalaFile(File file)
{
    Tunings::Scale s;
    String a = file.loadFileAsString();
    DBG(a);
    
    try {
        s = Tunings::parseSCLData(file.loadFileAsString().toStdString());
    } catch (Tunings::TuningError t) {
        AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon, TRANS("Scala Loading Error"), TRANS(t.what()));
        return;
    }
    currentScale = s;
    loadScalaScale(s);
}

void Tuning::loadKBMFile(std::string fname)
{
    Tunings::KeyboardMapping kbm;
    try {
        kbm = Tunings::readKBMFile(fname);
    } catch (Tunings::TuningError t) {
        AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon, TRANS("KBM Loading Error"), TRANS(t.what()));
        return;
    }
    currentKBM = kbm;
    loadKBM(kbm);
}

void Tuning::loadScalaScale(Tunings::Scale& s)
{
    auto scala = Tunings::Tuning(s, currentKBM,true).withSkippedNotesInterpolated();
    currentScalaString = scala.scale.rawText;
    auto offsets = Array<float>(12);
    
    if (s.count != 12)
    {
        isAbsoluteTuning = true;
        prep->tFundamental.setValue(C);
        prep->tFundamentalOffset.setValue(0.);
        prep->setScale(EqualTemperament);
        //Array<float> offsets;
        offsets.ensureStorageAllocated(127);
        for (int i = 0; i <= 127; i++)
        {
            offsets.set(i,(ftom(scala.frequencyForMidiNote(i))- i) * 100.f);
        }
        prep->setAbsoluteOffsetCents(offsets);
    } else
    {
        //subtract from equal temperament to get fractional midi representation
        Tunings::Scale et = Tunings::evenTemperament12NoteScale();
        for (int i = 0; i < 12; i++)
        {
            float micro = s.tones[i].cents;
            float equal = et.tones[i].cents;
            float offset = micro - equal;
            offsets.set((i+1)%12,offset); //.scl format puts first interval as the first line so we shift the representation over
        }
        prep->tFundamental.setValue(C);
        prep->tFundamentalOffset.setValue(0.);
        prep->tCustom.setValue(offsets);
        prep->setScale(CustomTuning);
        offsets.ensureStorageAllocated(127);
        for (int i = 0; i <= 127; i++)
        {
            offsets.set(i,0);
        }
        prep->setAbsoluteOffsetCents(offsets);
        isAbsoluteTuning = false;
    }
    //put tuning name in TuningLibrary
//    for (int i = 0; i <= 127; i++)
//    {
//        DBG(String(i) + " : " + String(scala.frequencyForMidiNote(i)));
//    }
}

void Tuning::loadKBM(Tunings::KeyboardMapping& kbm)
{
    Tunings::Tuning scala;
    currentKBMString = kbm.rawText;
    auto offsets = Array<float>(12);
    try {
        scala = Tunings::Tuning(currentScale, kbm,true).withSkippedNotesInterpolated();
    }  catch (Tunings::TuningError t) {
        AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon, TRANS("KBM Loading Error"), TRANS(t.what()));
        return;
    }
    for (int i = 0; i <= 127; i++)
    {
        offsets.set(i,0);
    }
    currentScalaString = scala.scale.rawText;
    prep->setAbsoluteOffsetCents(offsets);
   // auto interp = scala.withSkippedNotesInterpolated();
    
    if (currentScale.count != 12)
    {
        isAbsoluteTuning = true;
        prep->tFundamental.setValue(C);
        prep->tFundamentalOffset.setValue(0.);
        prep->setScale(EqualTemperament);
        Array<float> offsets;
        offsets.ensureStorageAllocated(127);
        for(int i = 0; i <= 127; i++)
        {
            offsets.set(i,(ftom(scala.frequencyForMidiNote(i))- i) * 100.f);
        }
        prep->setAbsoluteOffsetCents(offsets);
    } else
    {
        int fundamental = scala.keyboardMapping.middleNote % 12;
        //subtract from equal temperament to get fractional midi representation
        Tunings::Scale et = Tunings::evenTemperament12NoteScale();
        for (int i = 0; i < 12; i++)
        {
            float freq = scala.frequencyForMidiNote(scala.keyboardMapping.middleNote + i);
            float a = ftom(freq);
            float micro = (a - (scala.keyboardMapping.middleNote + i)) * 100.f;
//            float equal = et.tones[(i+1)%12].cents;
//            float offset = micro - equal;
            offsets.set((i+1)%12,micro); //.scl format puts first interval as the first line so we shift the representation over
        }
        //std::rotate(offsets.begin(),offsets.begin() + fundamental+ 1, offsets.end());
        prep->tFundamental.setValue((PitchClass)fundamental);
        prep->tFundamentalOffset.setValue(0.);
        prep->tCustom.setValue(offsets);
        prep->setScale(CustomTuning);
        offsets.ensureStorageAllocated(127);
        for (int i = 0; i <= 127; i++)
        {
            offsets.set(i,0);
        }
        prep->setAbsoluteOffsetCents(offsets);
        isAbsoluteTuning = false;
    }
    
//    for (int i = 0; i <= 127; i++)
//    {
//        DBG(String(i) + " : " + String(scala.frequencyForMidiNote(i)));
//    }

}

String Tuning::generateScalaString()
{
    Array<float> currentScale;
    const Array<float> ET     = Array<float>( {100., 200., 300., 400., 500., 600., 700., 800., 900., 1000., 1100.} );
    currentScale = getCurrentScaleCents();
    int current_fundamental = prep->getFundamental();
    std::rotate(currentScale.begin(),currentScale.begin() + current_fundamental + 1, currentScale.end());
    std::ostringstream oss;
    oss.imbue(std::locale("C"));
    oss << "! \nautogenerated scala file\n"
    << currentScale.size()
    << "\n! \n";
    for (int i = 0; i < ET.size(); i++)
    {
        
        oss << " " << std::fixed << std::setprecision(2)<< (currentScale[i] + ET[i]);
        oss << "\n";
    }
    oss << " 2/1";
    
    return String(oss.str());
}


#if BK_UNIT_TESTS

class TuningTests : public UnitTest
{
public:
	TuningTests() : UnitTest("Tunings", "Tuning") {}

	void runTest() override
	{
		beginTest("Tuning");

		for (int i = 0; i < 10; i++)
		{
			// create tuning preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random tuning prep " + String(i);
			DBG("test consistency: " + name);

			TuningPreparation::Ptr tp1 = new TuningPreparation();

			tp1->randomize();

			Tuning t1(tp1, 1);
			t1.setName(name);

			ValueTree vt1 = t1.getState();

			XmlElement* xml = vt1.createXml();

			TuningPreparation::Ptr tp2 = new TuningPreparation();

			Tuning t2(tp2, 1);

			t2.setState(xml);
			t2.setName(name);

			ValueTree vt2 = t2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"tuning prep: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());
		}

		for (int i = 0; i < 10; i++)
		{
			// create tuning and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random tuning " + String(i);
			DBG("test consistency: " + name);

			Tuning t1(-1, true);
			t1.setName(name);

			ValueTree vt1 = t1.getState();

			XmlElement* xml = vt1.createXml();

			Tuning t2(-1, true);

			t2.setState(xml);
			t2.setName(name);

			ValueTree vt2 = t2.getState();

			expect(vt1.isEquivalentTo(vt2),
				"tuning: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());
		}
	}
};

static TuningTests tuningTests;

class TuningModTests : public UnitTest
{
public:
	TuningModTests() : UnitTest("TuningMod", "TuningMod") {}

	void runTest() override
	{
		beginTest("TuningMod");

		for (int i = 0; i < 10; i++)
		{
			// create tuning mod preparation and randomize it
			// call getState() to convert to ValueTree
			// call setState() to convert from ValueTree to preparation
			// compare begin and end states
			String name = "random direct mod " + String(i);
			DBG("test consistency: " + name);

			TuningPreparation::Ptr tp1 = new TuningPreparation();
			TuningModification::Ptr tm1 = new TuningModification(tp1, 1);


			tm1->randomize();
			tm1->setName(name);

			ValueTree vt1 = tm1->getState();

			XmlElement* xml = vt1.createXml();

			TuningPreparation::Ptr tp2 = new TuningPreparation();
			TuningModification::Ptr tm2 = new TuningModification(tp2, 1);

			tm2->setState(xml);

			ValueTree vt2 = tm2->getState();

			expect(vt1.isEquivalentTo(vt2),
				"tuning mod: value trees do not match\n" +
				vt1.toXmlString() +
				"\n=======================\n" +
				vt2.toXmlString());


			//expect(tm1->compare(tm2), "tuning mod: preparations do not match");
		}

	}
};

static TuningModTests tuningModTests;

#endif
