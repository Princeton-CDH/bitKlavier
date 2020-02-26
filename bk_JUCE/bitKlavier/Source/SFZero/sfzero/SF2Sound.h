/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#ifndef SF2SOUND_H_INCLUDED
#define SF2SOUND_H_INCLUDED

#include "SFZSound.h"

namespace sfzero
{
    
    class SF2Sound : public Sound
    {
    public:
        SF2Sound(const juce::File &file);
        virtual ~SF2Sound();
        
        typedef juce::ReferenceCountedObjectPtr<SF2Sound> Ptr;
        typedef juce::Array<SF2Sound::Ptr> PtrArr;
        
        void loadRegions(int subsound) override;
        void loadSamples(juce::AudioFormatManager *formatManager, double *progressVar = nullptr, juce::Thread *thread = nullptr) override;
        
        class Preset : public juce::ReferenceCountedObject
        {
        public: 
            typedef juce::ReferenceCountedObjectPtr<Preset> Ptr;
            typedef juce::Array<Preset::Ptr> PtrArr;
            
            juce::String name;
            int bank;
            int preset;
            Region::PtrArr regions;
            
            Preset(juce::String nameIn, int bankIn, int presetIn) : name(nameIn), bank(bankIn), preset(presetIn) {}
            ~Preset() {}
            void addRegion(Region::Ptr region) { regions.add(region); }
        };
        void addPreset(Preset::Ptr preset);
        
        int numSubsounds() override;
        juce::String subsoundName(int whichSubsound) override;
        void useSubsound(int whichSubsound) override;
        int selectedSubsound() override;
        
        Sample::Ptr sampleFor(double sampleRate);
        void setSamplesBuffer(juce::AudioSampleBuffer *buffer);
        
    private:
        Preset::PtrArr presets_;
        juce::HashMap<int, Sample::Ptr> samplesByRate_;
        int selectedPreset_;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SF2Sound)
    };
}

#endif // SF2SOUND_H_INCLUDED

