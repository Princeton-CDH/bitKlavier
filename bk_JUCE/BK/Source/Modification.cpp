/*
  ==============================================================================

    Modification.cpp
    Created: 1 Feb 2017 5:32:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Modification.h"

Modification::Modification(BKModificationType whichMod, int whichPrep,  int val):
prepId(whichPrep),
modInt(val),
type(whichMod)
{
    
}

Modification::Modification(BKModificationType whichMod, int whichPrep, float val):
prepId(whichPrep),
modFloat(val),
type(whichMod)
{
    
}

Modification::Modification(BKModificationType whichMod, int whichPrep, bool val):
prepId(whichPrep),
modBool(val),
type(whichMod)
{
    
}

Modification::Modification(BKModificationType whichMod, int whichPrep, Array<int> val):
prepId(whichPrep),
modIntArr(val),
type(whichMod)
{
    
}

Modification::Modification(BKModificationType whichMod, int whichPrep, Array<float> val):
prepId(whichPrep),
modFloatArr(val),
type(whichMod)
{
    
}

Modification::~Modification()
{
    
}
