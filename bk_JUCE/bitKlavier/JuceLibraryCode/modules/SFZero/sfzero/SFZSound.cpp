/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#include "SFZSound.h"
#include "SFZReader.h"
#include "SFZRegion.h"
#include "SFZSample.h"

sfzero::Sound::Sound(const juce::File &fileIn) : file_(fileIn) {}
sfzero::Sound::~Sound()
{
  int numRegions = regions_.size();

  for (int i = 0; i < numRegions; ++i)
  {
    delete regions_[i];
    regions_.set(i, nullptr);
  }

  for (juce::HashMap<juce::String, sfzero::Sample *>::Iterator i(samples_); i.next();)
  {
    delete i.getValue();
  }
    
samples_.clear();
}

bool sfzero::Sound::appliesToNote(int /*midiNoteNumber*/)
{
  // Just say yes; we can't truly know unless we're told the velocity as well.
  return true;
}

bool sfzero::Sound::appliesToChannel(int /*midiChannel*/) { return true; }
void sfzero::Sound::addRegion(sfzero::Region *region) { regions_.add(region); }
sfzero::Sample *sfzero::Sound::addSample(juce::String path, juce::String defaultPath)
{
  path = path.replaceCharacter('\\', '/');
  defaultPath = defaultPath.replaceCharacter('\\', '/');
  juce::File sampleFile;
  if (defaultPath.isEmpty())
  {
    sampleFile = file_.getSiblingFile(path);
  }
  else
  {
    juce::File defaultDir = file_.getSiblingFile(defaultPath);
    sampleFile = defaultDir.getChildFile(path);
  }
  juce::String samplePath = sampleFile.getFullPathName();
  sfzero::Sample *sample = samples_[samplePath];
  if (sample == nullptr)
  {
    sample = new sfzero::Sample(sampleFile);
    samples_.set(samplePath, sample);
  }
  return sample;
}

void sfzero::Sound::addError(const juce::String &message) { errors_.add(message); }


void sfzero::Sound::addUnsupportedOpcode(const juce::String &opcode)
{
  if (!unsupportedOpcodes_.contains(opcode))
  {
    unsupportedOpcodes_.set(opcode, opcode);
    juce::String warning = "unsupported opcode: ";
    warning << opcode;
    warnings_.add(warning);
  }
}

void sfzero::Sound::loadRegions(int subsound)
{
  sfzero::Reader reader(this);

  reader.read(file_);
}

void sfzero::Sound::loadSamples(juce::AudioFormatManager *formatManager, double *progressVar, juce::Thread *thread)
{
  if (progressVar)
  {
    *progressVar = 0.0;
  }

  double numSamplesLoaded = 1.0, numSamples = samples_.size();
  for (juce::HashMap<juce::String, sfzero::Sample *>::Iterator i(samples_); i.next();)
  {
    sfzero::Sample *sample = i.getValue();
    bool ok = sample->load(formatManager);
    if (!ok)
    {
      addError("Couldn't load sample \"" + sample->getShortName() + "\"");
    }

    numSamplesLoaded += 1.0;
    if (progressVar)
    {
      *progressVar = numSamplesLoaded / numSamples;
    }
    if (thread && thread->threadShouldExit())
    {
      return;
    }
  }

  if (progressVar)
  {
    *progressVar = 1.0;
  }
}

sfzero::Region *sfzero::Sound::getRegionFor(int note, int velocity, sfzero::Region::Trigger trigger)
{
  int numRegions = regions_.size();

  for (int i = 0; i < numRegions; ++i)
  {
    sfzero::Region *region = regions_[i];
    if (region->matches(note, velocity, trigger))
    {
      return region;
    }
  }

  return nullptr;
}

int sfzero::Sound::getNumRegions() { return regions_.size(); }

sfzero::Region *sfzero::Sound::regionAt(int index) { return regions_[index]; }

int sfzero::Sound::numSubsounds() { return 1; }

juce::String sfzero::Sound::subsoundName(int /*whichSubsound*/) { return juce::String(); }

void sfzero::Sound::useSubsound(int /*whichSubsound*/) {}

int sfzero::Sound::selectedSubsound() { return 0; }

juce::String sfzero::Sound::dump()
{
  juce::String info;
  auto &errors = getErrors();
  if (errors.size() > 0)
  {
    info << errors.size() << " errors: \n";
    info << errors.joinIntoString("\n");
    info << "\n";
  }
  else
  {
    info << "no errors.\n\n";
  }

  auto &warnings = getWarnings();
  if (warnings.size() > 0)
  {
    info << warnings.size() << " warnings: \n";
    info << warnings.joinIntoString("\n");
  }
  else
  {
    info << "no warnings.\n";
  }

  if (regions_.size() > 0)
  {
    info << regions_.size() << " regions: \n";
    for (int i = 0; i < regions_.size(); ++i)
    {
      info << regions_[i]->dump();
    }
  }
  else
  {
    info << "no regions.\n";
  }

  if (samples_.size() > 0)
  {
    info << samples_.size() << " samples: \n";
    for (juce::HashMap<juce::String, sfzero::Sample *>::Iterator i(samples_); i.next();)
    {
      info << i.getValue()->dump();
    }
  }
  else
  {
    info << "no samples.\n";
  }
  return info;
}
