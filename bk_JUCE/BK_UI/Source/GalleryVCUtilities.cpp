/*
  ==============================================================================

    TextProcessingUtilities.cpp
    Created: 22 Mar 2017 12:23:48pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "GalleryViewController.h"

String GalleryViewController::processPianoMapString(const String& s)
{
    String temp = "";
    String out = "";
    
    bool isNumber = false;
    bool isKeymap = false;
    bool isColon  = false;
    bool isSpace = false;
    bool isEndOfString = false;
    
    bool itsAKeymap = false;
    bool itsAColon = false;
    bool itsASpace = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar colon = ':';
    juce_wchar keymap = 'k';
    
    Array<int> keys;
    
    for (int i = processor.currentPiano->pianoMap.size(); --i >= 0;)
        processor.currentPiano->pianoMap.setUnchecked(i, 0);
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        isColon     = !CharacterFunctions::compare(c1, colon);
        isKeymap    = !CharacterFunctions::compare(c1, keymap);
        isNumber    = CharacterFunctions::isDigit(c1);
        isSpace     = CharacterFunctions::isWhitespace(c1);
        if (i==s.length()) isEndOfString = true;
        
        
        
        if (!isNumber)
        {
            if (!itsAColon && isColon)
            {
                itsAColon = true;
                if (itsAKeymap)
                    keys = processor.gallery->getKeymap(temp.getIntValue())->keys();
                else
                    keys.add(temp.getIntValue());
                
                temp = "";
            }
            else if (!itsASpace && (isEndOfString || isSpace))
            {
                itsASpace = true;
                
                int whichPiano = temp.getIntValue();
                
                if (whichPiano > 0)
                {
                    // Set piano map parameters.
                    for (auto key : keys)
                    {
                        out += (String(key) + ":" +String(whichPiano) + " ");
                        processor.currentPiano->pianoMap.set(key, whichPiano);
                    }
                }
                
                itsAKeymap = false;
                itsAColon = false;
                
                temp = "";
                
                keys.clearQuick();
            }
            else if (!itsAKeymap && isKeymap)
            {
                itsAKeymap = true;
            }
            else
            {
                itsASpace = false;
                continue;
            }
        }
        else
        {
            itsASpace = false;
            temp += c1;
        }
        
    }
    
    return out;
}

String GalleryViewController::processResetMapString(const String& s)
{
    String out = "";
    String rest = s;
    
    Array<int> keys;
    
    for (int i = 0; i < 128; i++)
    {
        processor.currentPiano->modMap[i]->synchronicReset.clearQuick();
        processor.currentPiano->modMap[i]->tuningReset.clearQuick();
        processor.currentPiano->modMap[i]->tempoReset.clearQuick();
        processor.currentPiano->modMap[i]->nostalgicReset.clearQuick();
        processor.currentPiano->modMap[i]->directReset.clearQuick();
    }
    
    while (rest != "")
    {
        keys.clearQuick();
        
        String rmap = rest.upToFirstOccurrenceOf(" ", false, true);
        if (rmap.contains("{"))
        {
            rmap =  rest.upToFirstOccurrenceOf("}", true, false);
        }
        
        DBG("rmap: " + rmap);
        
        String keyPart = rmap.upToFirstOccurrenceOf(":", false, true);
        DBG("keyPart: " + keyPart);
        
        if (keyPart.contains("k"))
        {
            int whichKeymap = keyPart.fromLastOccurrenceOf("k", false, true).getIntValue();
            keys = processor.gallery->getKeymap(whichKeymap)->keys();
            
        }
        else
        {
            keys.add( keyPart.getIntValue());
        }
        
        String temp = rmap.substring(keyPart.length()+1);
        
        String prepPart = temp.fromFirstOccurrenceOf("{", false, true).upToLastOccurrenceOf("}", false, true);
        
        if (prepPart == "") prepPart = temp;
        
        DBG("prepPart: " + prepPart);
        
        
        
        juce_wchar synchronicLC = 's';
        juce_wchar synchronicUC = 'S';
        juce_wchar nostalgicLC = 'n';
        juce_wchar nostalgicUC = 'N';
        juce_wchar tuningLC = 't';
        juce_wchar tuningUC = 'T';
        juce_wchar directLC = 'd';
        juce_wchar directUC = 'D';
        juce_wchar tempoLC = 'm';
        juce_wchar tempoUC = 'M';
        
        bool isSynchronic = false, inSynchronic = false;
        bool isNostalgic = false, inNostalgic = false;
        bool isDirect = false, inDirect = false;
        bool isTuning = false, inTuning = false;
        bool isTempo = false, inTempo = false;
        bool isNumber = false;
        
        
        String num = "";
        
        String::CharPointerType c = prepPart.getCharPointer();
        
        for (int i = 0; i < (prepPart.length()+1); i++)
        {
            juce_wchar c1 = c.getAndAdvance();
            
            isSynchronic   = !CharacterFunctions::compare(c1, synchronicLC) ||  !CharacterFunctions::compare(c1, synchronicUC);
            isNostalgic    = !CharacterFunctions::compare(c1, nostalgicLC)  ||  !CharacterFunctions::compare(c1, nostalgicUC);
            isDirect       = !CharacterFunctions::compare(c1, directLC)     ||  !CharacterFunctions::compare(c1, directUC);
            isTuning       = !CharacterFunctions::compare(c1, tuningLC)     ||  !CharacterFunctions::compare(c1, tuningUC);
            isTempo        = !CharacterFunctions::compare(c1, tempoLC)     ||  !CharacterFunctions::compare(c1, tempoUC);
            isNumber       = CharacterFunctions::isDigit(c1);
            
            if (isNumber) num += c1;
            else
            {
                if (isSynchronic)        inSynchronic    = true;
                else if (isNostalgic)    inNostalgic     = true;
                else if (isDirect)       inDirect        = true;
                else if (isTuning)       inTuning        = true;
                else if (isTempo)        inTempo        = true;
                else if (inSynchronic)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        out += String(k) + ":" + "s" + String(whichPrep) + " ";
                        processor.currentPiano->modMap[k]->synchronicReset.add(whichPrep);
                    }
                    
                    num = "";
                    inSynchronic = false;
                    
                }
                else if (inNostalgic)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        out += String(k) + ":" + "n" + String(whichPrep) + " ";
                        processor.currentPiano->modMap[k]->nostalgicReset.add(whichPrep);
                    }
                    
                    num = "";
                    inNostalgic = false;
                }
                else if (inDirect)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        out += String(k) + ":" + "d" + String(whichPrep) + " ";
                        processor.currentPiano->modMap[k]->directReset.add(whichPrep);
                    }
                    
                    num = "";
                    inDirect = false;
                }
                else if (inTuning)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        out += String(k) + ":" + "t" + String(whichPrep) + " ";
                        processor.currentPiano->modMap[k]->tuningReset.add(whichPrep);
                    }
                    
                    num = "";
                    inTuning = false;
                }
                else if (inTempo)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        out += String(k) + ":" + "m" + String(whichPrep) + " ";
                        processor.currentPiano->modMap[k]->tempoReset.add(whichPrep);
                    }
                    
                    num = "";
                    inTempo = false;
                }
                
                num = "";
            }
            
        }
        
        rest = rest.substring(rmap.length()+1);
        
    }
    
    
    return out;
}

String GalleryViewController::processModMapString(const String& s)
{
    String temp = "";
    String out = "";
    
    bool isNumber = false;
    bool isKeymap = false;
    bool isColon  = false;
    bool isSpace = false;
    bool isEndOfString = false;
    
    bool isSynchronic = false;
    bool isNostalgic = false;
    bool isTuning = false;
    bool isDirect= false;
    bool isTempo = false;
    
    bool isBracket;
    
    bool itsAKeymap = false;
    bool lastColon = false;
    bool lastBracket = false;
    bool itsASpace = false;
    bool itsADirect = false;
    bool itsASynchronic = false;
    bool itsATuning = false;
    bool itsANostalgic = false;
    bool itsATempo = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar keymap = 'k';
    juce_wchar colon = ':';
    
    
    juce_wchar synchronicLC = 's';
    juce_wchar synchronicUC = 'S';
    juce_wchar nostalgicLC = 'n';
    juce_wchar nostalgicUC = 'N';
    juce_wchar tuningLC = 't';
    juce_wchar tuningUC = 'T';
    juce_wchar tempoLC = 'm';
    juce_wchar tempoUC = 'M';
    juce_wchar directLC = 'd';
    juce_wchar directUC = 'D';
    juce_wchar lBracket = '{';
    juce_wchar rBracket = '}';
    juce_wchar lBracket2 = '[';
    juce_wchar rBracket2 = ']';
    juce_wchar lBracket3 = '(';
    juce_wchar rBracket3 = ')';
    
    Array<int> keys;
    
    DBG("clearing mod map");
    processor.currentPiano->clearModMap();
    
    for (int i = 0; i < 128; i++) processor.currentPiano->modificationMaps[i]->clear();
    
    if (s.length() == 0) return out;
    
    for (auto map : processor.currentPiano->pianoMap)    map = 0;
    
    int numColons, numBrackets;
    numColons = 0; numBrackets = 0;
    int whichMod = -1;
    
    Array<int> whichPreps;
    
    Array<Array<int>> theModMap;
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        
        isSynchronic   = !CharacterFunctions::compare(c1, synchronicLC) || !CharacterFunctions::compare(c1, synchronicUC);
        isNostalgic    = !CharacterFunctions::compare(c1, nostalgicLC) || !CharacterFunctions::compare(c1, nostalgicUC);
        isDirect       = !CharacterFunctions::compare(c1, directLC) || !CharacterFunctions::compare(c1, directUC);
        isTuning       = !CharacterFunctions::compare(c1, tuningLC) || !CharacterFunctions::compare(c1, tuningUC);
        isTempo      = !CharacterFunctions::compare(c1, tempoLC) || !CharacterFunctions::compare(c1, tempoUC);
        
        isBracket   = !CharacterFunctions::compare(c1, lBracket) || !CharacterFunctions::compare(c1, rBracket) ||
        !CharacterFunctions::compare(c1, lBracket2) || !CharacterFunctions::compare(c1, rBracket2) ||
        !CharacterFunctions::compare(c1, lBracket3) || !CharacterFunctions::compare(c1, rBracket3);
        isKeymap    = !CharacterFunctions::compare(c1, keymap);
        isColon     = !CharacterFunctions::compare(c1, colon);
        isNumber    = CharacterFunctions::isDigit(c1);
        isSpace     = CharacterFunctions::isWhitespace(c1);
        
        if (i==s.length()) isEndOfString = true;
        
        if (!isNumber)
        {
            if (isColon)
            {
                if (numColons == 0)
                {
                    if (itsAKeymap)
                        keys = processor.gallery->getKeymap(temp.getIntValue())->keys();
                    else
                        keys.add(temp.getIntValue());
                }
                else if (numColons == 1)
                {
                    whichMod = temp.getIntValue();
                }
                
                temp = "";
                
                if (++numColons == 2) lastColon = true;
            }
            else if ((numBrackets == 1) && isSpace)
            {
                whichPreps.add(temp.getIntValue());
                temp = "";
            }
            else if (isBracket)
            {
                if (numBrackets == 1 && temp != "") whichPreps.add(temp.getIntValue());
                
                if (++numBrackets == 2) lastBracket = true;
            }
            else if (isSpace || isEndOfString)
            {
                itsASpace = true;
                
                // Set piano map parameters.
                // keys | mod id | prepIds
                for (auto key : keys)
                {
                    
                    if (itsADirect)
                    {
                        DirectModPreparation::Ptr dmod = processor.gallery->getDirectModPreparation(whichMod);
                        
                        processor.currentPiano->configureDirectModification(key, dmod, whichPreps);
                    }
                    else if (itsANostalgic)
                    {
                        NostalgicModPreparation::Ptr nmod = processor.gallery->getNostalgicModPreparation(whichMod);
                        
                        processor.currentPiano->configureNostalgicModification(key, nmod, whichPreps);
                    }
                    else if (itsASynchronic)
                    {
                        SynchronicModPreparation::Ptr smod = processor.gallery->getSynchronicModPreparation(whichMod);
                        
                        processor.currentPiano->configureSynchronicModification(key, smod, whichPreps);
                    }
                    else if (itsATuning)
                    {
                        TuningModPreparation::Ptr tmod = processor.gallery->getTuningModPreparation(whichMod);
                        
                        processor.currentPiano->configureTuningModification(key, tmod, whichPreps);
                        
                    }
                    else if (itsATempo)
                    {
                        TempoModPreparation::Ptr mmod = processor.gallery->getTempoModPreparation(whichMod);
                        
                        processor.currentPiano->configureTempoModification(key, mmod, whichPreps);
                    }
                    
                }
                
                itsAKeymap = false;
                itsADirect = false;
                itsASynchronic = false;
                itsATuning = false;
                itsANostalgic = false;
                itsATempo = false;
                
                numColons = 0;
                numBrackets = 0;
                whichMod = -1;
                whichPreps.clearQuick();
                
                temp = "";
                
                keys.clearQuick();
            }
            else if (!itsAKeymap && isKeymap)
            {
                itsAKeymap = true;
            }
            else if (isDirect)
            {
                itsADirect = true;
            }
            else if (isTempo)
            {
                itsATempo = true;
            }
            else if (isSynchronic)
            {
                itsASynchronic = true;
            }
            else if (isTuning)
            {
                itsATuning = true;
            }
            else if (isNostalgic)
            {
                itsANostalgic = true;
            }
            else
            {
                itsASpace = false;
                continue;
            }
        }
        else
        {
            itsASpace = false;
            temp += c1;
        }
        
    }
    
    return processor.currentPiano->modificationMapsToString();
}
