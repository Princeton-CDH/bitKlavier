/*
  ==============================================================================

    BKRefCount.h
    Created: 7 Dec 2016 11:27:20am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKREFCOUNT_H_INCLUDED
#define BKREFCOUNT_H_INCLUDED

#include "BKUtilities.h"

class BKReferenceCountedObject : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<BKReferenceCountedObject>   Ptr;
    typedef Array<BKReferenceCountedObject::Ptr>                  Arr;
    typedef Array<BKReferenceCountedObject::Ptr, CriticalSection> CSArr;
    
private:
    
    JUCE_LEAK_DETECTOR(BKReferenceCountedObject);
};


#endif  // BKREFCOUNT_H_INCLUDED
