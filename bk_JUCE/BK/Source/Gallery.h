/*
  ==============================================================================

    Gallery.h
    Created: 7 Dec 2016 11:03:57am
    Author:  Michael R Mulshine
 
    A "Gallery" is an array of Pianos, along with one General Settings
    (equivalent to a "Library" in the original bitKlavier)

  ==============================================================================
*/

#ifndef Gallery_H_INCLUDED
#define Gallery_H_INCLUDED

#include "BKUtilities.h"

#include "Piano.h"

class Gallery : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Gallery>   Ptr;
    typedef Array<Gallery::Ptr>                  PtrArr;
    typedef Array<Gallery::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Gallery>                  Arr;
    typedef OwnedArray<Gallery, CriticalSection> CSArr;
 
    Gallery();
    Gallery(Piano::PtrArr pianos);
    ~Gallery();
    
    inline GeneralSettings::Ptr                getGeneralSettings(void)            {   return general;         }
    
    inline void setGeneralSettings(GeneralSettings::Ptr g)                          {   general = g;            }

    /*
    inline void setPianos(Piano::PtrArr p)           {   pianos = p;      }
    
    inline Piano::PtrArr getPianos(void)             {   return pianos;   }
    
    // something like this
    void writeToFile(String filepath);
    Piano::PtrArr readFromFile(String filepath);
     */
    
    
private:
    
    Piano::PtrArr pianos;
    GeneralSettings::Ptr general;
    ValueTree vt;
    
    JUCE_LEAK_DETECTOR(Gallery);
};



#endif  // Gallery_H_INCLUDED
