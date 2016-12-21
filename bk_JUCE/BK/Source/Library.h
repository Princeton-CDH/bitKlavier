/*
  ==============================================================================

    Library.h
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LIBRARY_H_INCLUDED
#define LIBRARY_H_INCLUDED

#include "BKUtilities.h"

#include "Preset.h"

class Library : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Library>   Ptr;
    typedef Array<Library::Ptr>                  PtrArr;
    typedef Array<Library::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Library>                  Arr;
    typedef OwnedArray<Library, CriticalSection> CSArr;
 
    Library();
    Library(Preset::PtrArr presets);
    ~Library();
    
    inline void setPresets(Preset::PtrArr p)           {   presets = p;      }
    
    inline Preset::PtrArr getPresets(void)             {   return presets;   }
    
    // something like this
    void writeToFile(String filepath);
    Preset::PtrArr readFromFile(String filepath);
    
    
private:
    
    Preset::PtrArr presets;
    
    ValueTree vt;
    
    JUCE_LEAK_DETECTOR(Library);
};



#endif  // LIBRARY_H_INCLUDED
