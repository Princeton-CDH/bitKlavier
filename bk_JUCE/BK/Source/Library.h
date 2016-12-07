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
    typedef Array<Library::Ptr>                  Arr;
    typedef Array<Library::Ptr, CriticalSection> CSArr;
 
    Library();
    Library(Preset::Arr presets);
    ~Library();
    
    inline void setPresets(Preset::Arr p)           {   presets = p;      }
    
    inline Preset::Arr getPresets(void)             {   return presets;   }
    
    // something like this
    void writeToFile(String filepath);
    Preset::Arr readFromFile(String filepath);
    
    
private:
    
    Preset::Arr presets;
    
    ValueTree vt;
    
    JUCE_LEAK_DETECTOR(Library);
};



#endif  // LIBRARY_H_INCLUDED
