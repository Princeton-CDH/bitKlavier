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
        s.append(String(key), 6);
        s.append(" ", 1);
    }
    return s;
}

String floatArrayToString(Array<float> arr)
{
    String s = "";
    for (auto key : arr)
    {
        s.append(String(key), 6);
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
    
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isNumChar = CharacterFunctions::isDigit(c1);
        
        if (!isNumChar)
        {
            if (inNumber)
            {
                arr.add(temp.getIntValue());
                temp = "";
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

Array<float> stringToFloatArray(String s)
{
    Array<float> arr = Array<float>();
    
    String temp = "";
    bool inNumber = false;
    
    String::CharPointerType c = s.getCharPointer();
    
    juce_wchar prd = '.';
    
    int prdCnt = 0;
    
    // DEBUG
    for (int i = 0; i < (s.length()+1); i++)
    {
        juce_wchar c1 = c.getAndAdvance();
        
        bool isPrd = !CharacterFunctions::compare(c1, prd);
        
        if (isPrd) prdCnt += 1;
        
        bool isNumChar = CharacterFunctions::isDigit(c1) || isPrd;
        
        if (!isNumChar)
        {
            if (inNumber)
            {
                arr.add(temp.getFloatValue());
                temp = "";
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

