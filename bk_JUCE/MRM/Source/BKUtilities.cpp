/*
  ==============================================================================

    BKUtilities.cpp
    Created: 16 Nov 2016 11:12:15am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKUtilities.h"


String intArrayToString(Array<int> arr)
{
    String s = "";
    for (auto key : arr)
    {
        s.append(String(key), 3);
        s.append(" ", 1);
    }
    return s;
}

String floatArrayToString(Array<float> arr)
{
    String s = "";
    for (auto key : arr)
    {
        s.append(String(key), 3);
        s.append(" ", 1);
    }
    return s;
}

Array<int> stringToIntArray(String s)
{
    Array<int> arr = Array<int>();
    
    String temp = "";
    bool inNumber = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    while (*c)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isNumChar = CharacterFunctions::isDigit(c1);
        
        if (!isNumChar)
        {
            if (inNumber)
            {
                arr.add(temp.getIntValue());
            }
            
            inNumber = false;
            continue;
        }
        else
        {
            inNumber = true;
            
            temp += *c;
        }
    }
    
    return arr;
}

Array<float> stringToFloatArray(String s)
{
    Array<float> arr = Array<float>();
    
    String temp = "";
    bool inNumber = false;
    
    String::CharPointerType c = s.getCharPointer();
    String::CharPointerType end = c.findTerminatingNull();
    
    juce_wchar c1 = *c;
    juce_wchar prd = '.';
    
    // DEBUG
    for (c1 = *c ; c != end; c1 = c.getAndAdvance())
    {
        bool isNumChar = CharacterFunctions::isDigit(c1) || CharacterFunctions::compare(c1, prd);
        
        if (!isNumChar)
        {
            if (inNumber)
            {
                arr.add(temp.getFloatValue());
            }
            inNumber = false;
            continue;
        }
        else
        {
            inNumber = true;
            
            temp += c1;
        }
    }
    
    return arr;
}

