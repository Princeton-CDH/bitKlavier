/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#ifndef SF2READER_H_INCLUDED
#define SF2READER_H_INCLUDED

#include "SF2.h"
#include "SFZRegion.h"
#include "SF2Sound.h"

namespace sfzero
{
    class SF2Reader : public juce::ReferenceCountedObject
    {
    public:
        typedef juce::ReferenceCountedObjectPtr<SF2Reader> Ptr;
        typedef juce::Array<SF2Reader::Ptr> PtrArr;
        
        SF2Reader(SF2Sound::Ptr sound, const juce::File &file);
        virtual ~SF2Reader();
        
        void read();
        juce::AudioSampleBuffer *readSamples(double *progressVar = nullptr, juce::Thread *thread = nullptr);
        
    private:
        SF2Sound::Ptr sound_;
        juce::FileInputStream *file_;
        
        void addGeneratorToRegion(word genOper, SF2::genAmountType *amount, Region::Ptr region);
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SF2Reader)
    };
}

#endif // SF2READER_H_INCLUDED

