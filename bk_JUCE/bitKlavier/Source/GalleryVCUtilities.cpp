/*
  ==============================================================================

    TextProcessingUtilities.cpp
    Created: 22 Mar 2017 12:23:48pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "HeaderViewController.h"

#define TEXT_INTERFACE 0

#if TEXT_INTERFACE
String HeaderViewController::processPianoMapString(const String& s)
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

String HeaderViewController::processResetMapString(const String& s)
{
    String out = "";
    String rest = s;
    
    Array<int> keys;
    Array<String> sources;
    
    for (int i = 0; i < 128; i++)
    {
        processor.currentPiano->modificationMap[i]->blendronicResets.clearQuick();
        processor.currentPiano->modificationMap[i]->synchronicResets.clearQuick();
        processor.currentPiano->modificationMap[i]->tuningResets.clearQuick();
        processor.currentPiano->modificationMap[i]->tempoResets.clearQuick();
        processor.currentPiano->modificationMap[i]->nostalgicResets.clearQuick();
        processor.currentPiano->modificationMap[i]->directResets.clearQuick();
        processor.currentPiano->modificationMap[i]->blendronicModResets.clearQuick();
        processor.currentPiano->modificationMap[i]->synchronicModResets.clearQuick();
        processor.currentPiano->modificationMap[i]->tuningModResets.clearQuick();
        processor.currentPiano->modificationMap[i]->tempoModResets.clearQuick();
        processor.currentPiano->modificationMap[i]->nostalgicModResets.clearQuick();
        processor.currentPiano->modificationMap[i]->directModResets.clearQuick();
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
        
        Modifications::Reset reset;
        
        if (keyPart.contains("k"))
        {
            int whichKeymap = keyPart.fromLastOccurrenceOf("k", false, true).getIntValue();
            keys = processor.gallery->getKeymap(whichKeymap)->keys();
            reset.keymapIds.add(whichKeymap);
        }
        else
        {
            keys.add( keyPart.getIntValue());
        }
        
        String temp = rmap.substring(keyPart.length()+1);
        
        String prepPart = temp.fromFirstOccurrenceOf("{", false, true).upToLastOccurrenceOf("}", false, true);
        
        if (prepPart == "") prepPart = temp;
        
        DBG("prepPart: " + prepPart);
        
        
        juce_wchar blendronicLC = 'b';
        juce_wchar blendronicUC = 'B';
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
        
        bool isBlendronic = false, inBlendronic = false;
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
            
            isBlendronic   = !CharacterFunctions::compare(c1, blendronicLC) ||  !CharacterFunctions::compare(c1, blendronicUC);
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
                else if (isBlendronic)   inBlendronic = true;
                else if (inSynchronic)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        reset.prepId = whichPrep;
                        out += String(k) + ":" + "s" + String(whichPrep) + " ";
                        processor.currentPiano->modificationMap[k]->synchronicResets.add(reset);
                    }
                    
                    num = "";
                    inSynchronic = false;
                    
                }
                else if (inNostalgic)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        reset.prepId = whichPrep;
                        out += String(k) + ":" + "n" + String(whichPrep) + " ";
                        processor.currentPiano->modificationMap[k]->nostalgicResets.add(reset);
                    }
                    
                    num = "";
                    inNostalgic = false;
                }
                else if (inDirect)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        reset.prepId = whichPrep;
                        out += String(k) + ":" + "d" + String(whichPrep) + " ";
                        processor.currentPiano->modificationMap[k]->directResets.add(reset);
                    }
                    
                    num = "";
                    inDirect = false;
                }
                else if (inTuning)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        reset.prepId = whichPrep;
                        out += String(k) + ":" + "t" + String(whichPrep) + " ";
                        processor.currentPiano->modificationMap[k]->tuningResets.add(reset);
                    }
                    
                    num = "";
                    inTuning = false;
                }
                else if (inTempo)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        reset.prepId = whichPrep;
                        out += String(k) + ":" + "m" + String(whichPrep) + " ";
                        processor.currentPiano->modificationMap[k]->tempoResets.add(reset);
                    }
                    
                    num = "";
                    inTempo = false;
                }
                else if (inBlendronic)
                {
                    int whichPrep = num.getIntValue();
                    for (auto k : keys)
                    {
                        reset.prepId = whichPrep;
                        out += String(k) + ":" + "m" + String(whichPrep) + " ";
                        processor.currentPiano->modificationMap[k]->blendronicResets.add(reset);
                    }
                    
                    num = "";
                    inBlendronic = false;
                }
                
                num = "";
            }
            
        }
        
        rest = rest.substring(rmap.length()+1);
        
    }
    
    
    return out;
}

String HeaderViewController::processmodificationMapString(const String& s)
{
    String temp = "";
    String out = "";
    
    bool isNumber = false;
    bool isKeymap = false;
    bool isColon  = false;
    bool isSpace = false;
    bool isEndOfString = false;
    
    bool isBlendronic = false;
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
    bool itsABlendronic = false;
    bool itsATuning = false;
    bool itsANostalgic = false;
    bool itsATempo = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar keymap = 'k';
    juce_wchar colon = ':';
    
    juce_wchar blendronicLC = 'b';
    juce_wchar blendronicUC = 'B';
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
    processor.currentPiano->clearmodificationMap();
    
    processor.currentPiano->clearMapper();
    
    if (s.length() == 0) return out;
    
    for (auto map : processor.currentPiano->pianoMap)    map = 0;
    
    int numColons, numBrackets;
    numColons = 0; numBrackets = 0;
    int whichMod = -1;
    
    Array<int> whichPreps;
    
    BKPreparationType type;
    
    Array<Array<int>> themodificationMap;
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        isBlendronic   = !CharacterFunctions::compare(c1, blendronicLC) || !CharacterFunctions::compare(c1, synchronicUC);
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
                
                ItemMapper::Ptr thisMapper = new ItemMapper(type, whichMod, keys, whichPreps);
                processor.currentPiano->addMapper(thisMapper);
                processor.currentPiano->configureModification(thisMapper);

                itsAKeymap = false;
                itsADirect = false;
                itsASynchronic = false;
                itsABlendronic = false;
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
                
                type = PreparationTypeDirectMod;
            }
            else if (isTempo)
            {
                itsATempo = true;
                
                type = PreparationTypeTempoMod;
            }
            else if (isSynchronic)
            {
                itsASynchronic = true;
                
                type = PreparationTypeSynchronicMod;
            }
            else if (isTuning)
            {
                itsATuning = true;
                
                type = PreparationTypeTuningMod;
            }
            else if (isNostalgic)
            {
                itsANostalgic = true;
                
                type = PreparationTypeNostalgicMod;
            }
            else if (isBlendronic)
            {
                itsABlendronic = true;
                
                type = PreparationTypeBlendronicMod;
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

#endif
