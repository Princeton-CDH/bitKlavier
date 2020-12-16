/*
 ==============================================================================
 
 Moddable.h
 Created: 2 Dec 2020 11:56:22am
 Author:  Matthew Wang
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ModdableBase
{
public:
    ModdableBase() {}
    virtual ~ModdableBase() {}
    
    virtual void setTime(int ms) {}
    virtual int getTime() { return 0; }
    
    virtual void setInc(double v) {}
    virtual double getInc() { return 0.0; }
    
    virtual int getNumberOfInc() { return 0; }
    
    virtual void setMaxNumberOfInc(int mn) {}
    virtual int getMaxNumberOfInc() { return 0; }
    
    // tag dispatch pattern
    template <class T>
    struct tag {};
};

template <typename ValueType>
class Moddable : public ModdableBase
{
public:
    Moddable () = default;
    
    Moddable (ValueType v, int t, bool dB = false):
    value(v),
    base(v),
    mod(v),
    active(false),
    time(t),
    dv(v), // Make sure to properly calculate dv before (and if) it is needed
    n(0),
    maxN(0),
    dB(dB)
    {
        initInc(tag<ValueType>{});
    };
    
    Moddable (ValueType v, bool dB = false):
    Moddable (v, 0, dB) {}
    
    Moddable (const Moddable& m):
    Moddable (m.base, m.time)
    {
        inc = m.inc;
        maxN = m.maxN;
    }
    
    bool operator== (const Moddable& m) const noexcept
    {
        return (base == m.base &&
                time == m.time);
    }
    
    bool operator!= (const Moddable& m) const noexcept
    {
        return !(base == m.base &&
                 time == m.time);
    }
    
    Moddable& operator= (const Moddable&) = default;
    
    Moddable& operator= (ValueType v)
    {
        value = v;
        base = v;
        mod = v;
        active = false;
        return *this;
    }
    
    ~Moddable() {};
    
    //==============================================================================
    
    void modify(Moddable& m, bool reverse)
    {
        if (reverse)
        {
            mod = base;
            time = m.time;
            modTo(tag<ValueType>{}, m, false);
            return;
        }
        mod = m.base;
        time = m.time;
        modTo(tag<ValueType>{}, m, true);
    }
    
    // The minimal behavior for modTo
    // For special behaviors overload this below with a specific tag
    // as has been done for double, int, and float
    template<class T = ValueType>
    void modTo(tag<T>, Moddable& m, bool shouldInc)
    {
        if (time > 0)
        {
            active = true;
            timeElapsed = 0;
            return;
        }
        value = mod;
        active = false;
    }
    
    void modTo(tag<double>, Moddable& m, bool shouldInc)
    {
        if (shouldInc)
        {
            mod += (m.inc * m.n);
            if (m.n < m.maxN || m.maxN == 0) m.n++;
        }
        
        if (time > 0 && (mod - value) != 0)
        {
            calcDV(tag<ValueType>{});
            active = true; //active = m.active;
            timeElapsed = 0;
            return;
        }
        value = mod;
        active = false;
    }
    void modTo(tag<float>, Moddable& m, bool shouldInc) { modTo(tag<double>{}, m, shouldInc); }
    void modTo(tag<int>, Moddable& m, bool shouldInc) { modTo(tag<double>{}, m, shouldInc); }
    
    //==============================================================================
    
    void reset()
    {
        value = base;
        n = 0;
        active = false;
    }
    
    // Setters
    void set(ValueType v)
    {
        value = v;
        base = v;
        mod = v;
        active = false;
    }
    void setValue(ValueType v) { value = v; }
    void setBase(ValueType v) { base = v; }
    void setMod(ValueType v)
    {
        mod = v;
        n = 0;
    }
    
    // This use a double arg instead of ValueType so it's usable from the ModdableBase class
    void setInc(double v) override { setInc(tag<ValueType>{}, v); }
    
    template<class T = ValueType>
    void setInc(tag<T>, double v) { ; }
    
    // Be careful to make sure these overload the less specific function above.
    void setInc(tag<int>, double v) { inc = v; n = 0; }
    void setInc(tag<float>, double v) { inc = v; n = 0; }
    void setInc(tag<double>, double v) { inc = v; n = 0; }
    
    void setTime(int ms) override { time = ms; }
    void setActive(bool a) { active = a; }
    void setMaxNumberOfInc(int mn) override
    {
        maxN = mn;
        n = 0;
    }
    
    // Getters
    double getInc() override { return getInc(tag<ValueType>{}); }
    
    template<class T = ValueType>
    double getInc(tag<T>) { return 0.0; }
    
    double getInc(tag<int>) { return inc; }
    double getInc(tag<float>) { return inc; }
    double getInc(tag<double>) { return inc; }
    
    int getTime() override { return time; }
    int getNumberOfInc() override { return n; }
    int getMaxNumberOfInc() override { return maxN; }
    
    //==============================================================================
    // Step
    void step()
    {
        if (!active) return;
        step(tag<ValueType>{});
        timeElapsed++;
    }
    
    template<class T = ValueType>
    void step(tag<T>)
    {
        if (time - timeElapsed <= 0)
        {
            value = mod;
            active = false;
        }
    }
    
    void step(tag<double>)
    {
        if (dv > 0)
        {
            if (value < mod) value += dv;
            else
            {
                value = mod;
                active = false;
            }
        }
        else if (dv < 0)
        {
            if (value > mod) value += dv;
            else
            {
                value = mod;
                active = false;
            }
        }
    }
    void step(tag<float>) { step(tag<double>{}); }
    void step(tag<int>)
    {
        float p = 1.0f - (float(timeElapsed) / float(time));
        if (dv > 0)
        {
            if (value < mod) value = mod - int(dv * p);
            else
            {
                value = mod;
                active = false;
            }
        }
        else if (dv < 0)
        {
            if (value > mod) value += dv;
            else
            {
                value = mod;
                active = false;
            }
        }
    }
    
    //==============================================================================
    // Doing getState and setState a bit different than elsewhere
    // (passing in reference of tree instead of returning a sub tree)
    // because it works out better for backwards compatibility while
    // avoiding a lot of extra code in preparations'
    // getState and setState functions
    void getState(ValueTree& vt, StringArray s) { getState(tag<ValueType>{}, vt, s); }
    void getState(ValueTree& vt, String s) { getState(tag<ValueType>{}, vt, StringArray(s)); }
    
    template<class T = ValueType>
    void getState(tag<T>, ValueTree& vt, StringArray s)
    {
        vt.setProperty(s[0], base, 0);
        vt.setProperty(s[0] + "_time", time, 0);
        vt.setProperty(s[0] + "_maxN", maxN, 0);
    }
    
    void getState(tag<double>, ValueTree& vt, StringArray s)
    {
        if (dB) vt.setProperty(s[0], Decibels::decibelsToGain(base), 0);
        else vt.setProperty(s[0], base, 0);
        vt.setProperty(s[0] + "_inc", inc, 0);
        vt.setProperty(s[0] + "_time", time, 0);
        vt.setProperty(s[0] + "_maxN", maxN, 0);
    }
    void getState(tag<int>, ValueTree& vt, StringArray s) { getState(tag<double>{}, vt, s); }
    void getState(tag<float>, ValueTree& vt, StringArray s) { getState(tag<double>{}, vt, s); }
    
    template <class T>
    void getState(tag<Array<T>>, ValueTree& vt, StringArray s)
    {
        ValueTree bt(s[0]);
        ValueTree it(s[0] + "_inc");
        int count = 0;
        for (auto v : base) bt.setProperty(s[1] + String(count++), v, 0);
        count = 0;
        for (auto v : inc) it.setProperty(s[1] + String(count++), v, 0);
        vt.addChild(bt, -1, 0);
        vt.addChild(it, -1, 0);
        vt.setProperty(s[0] + "_time", time, 0);
        vt.setProperty(s[0] + "_maxN", maxN, 0);
    }
    
    template <class T>
    void getState(tag<Array<Array<T>>>, ValueTree& vt, StringArray s)
    {
        ValueTree bt(s[0]);
        ValueTree it(s[0] + "_inc");
        int count = 0;
        for (auto a : base)
        {
            ValueTree sst(s[1] + String(count++));
            int scount = 0;
            for (auto v : a) sst.setProperty(s[2] + String(scount++), v, 0);
            bt.addChild(sst, -1, 0);
        }
        count = 0;
        for (auto a : inc)
        {
            ValueTree sst(s[1] + String(count++));
            int scount = 0;
            for (auto v : a) sst.setProperty(s[2] + String(scount++), v, 0);
            it.addChild(sst, -1, 0);
        }
        vt.addChild(bt, -1, 0);
        vt.addChild(it, -1, 0);
        vt.setProperty(s[0] + "_time", time, 0);
        vt.setProperty(s[0] + "_maxN", maxN, 0);
    }
    
    //==============================================================================
    
    void setState(XmlElement* e, StringArray s, ValueType defaultValue)
    { setState(tag<ValueType>{}, e, s, defaultValue); }
    void setState(XmlElement* e, String s, ValueType defaultValue)
    { setState(tag<ValueType>{}, e, StringArray(s), defaultValue); }
    
    // This will catch enum types
    template<class T = ValueType>
    void setState(tag<T>, XmlElement* e, StringArray s, ValueType defaultValue)
    {
        base = T(e->getIntAttribute(s[0], int(defaultValue)));
        time = e->getIntAttribute(s[0] + "_time", 0);
        maxN = e->getIntAttribute(s[0] + "_maxN", 0);
        value = base;
    }
    
    void setState(tag<double>, XmlElement* e, StringArray s, ValueType defaultValue)
    {
        if (dB) base = Decibels::gainToDecibels(e->getDoubleAttribute(s[0], defaultValue));
        else base = e->getDoubleAttribute(s[0], defaultValue);
        inc = e->getDoubleAttribute(s[0] + "_inc", 0.0);
        time = e->getIntAttribute(s[0] + "_time", 0);
        maxN = e->getIntAttribute(s[0] + "_maxN", 0);
        value = base;
    }
    void setState(tag<int>, XmlElement* e, StringArray s, ValueType defaultValue)
    { setState(tag<double>{}, e, s, defaultValue); }
    void setState(tag<float>, XmlElement* e, StringArray s, ValueType defaultValue)
    { setState(tag<double>{}, e, s, defaultValue); }
    
    void setState(tag<bool>, XmlElement* e, StringArray s, ValueType defaultValue)
    {
        base = e->getBoolAttribute(s[0], defaultValue);
        time = e->getIntAttribute(s[0] + "_time", 0);
        maxN = e->getIntAttribute(s[0] + "_maxN", 0);
        value = base;
    }
    void setState(tag<String>, XmlElement* e, StringArray s, ValueType defaultValue)
    {
        base = e->getStringAttribute(s[0], defaultValue);
        time = e->getIntAttribute(s[0] + "_time", 0);
        maxN = e->getIntAttribute(s[0] + "_maxN", 0);
        value = base;
    }
    
    template <class T>
    void setState(tag<Array<T>>, XmlElement* e, StringArray s, ValueType defaultValue)
    {
        base = defaultValue;
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(s[0]))
            {
                base = Array<T>();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    if (sub->hasAttribute(s[1] + String(k)))
                        base.add(getAttribute(tag<T>{}, sub, s[1] + String(k)));
                }
            }
            else if (sub->hasTagName(s[0] + "_inc"))
            {
                inc = Array<T>();
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    if (sub->hasAttribute(s[1] + String(k)))
                        inc.add(getAttribute(tag<T>{}, sub, s[1] + String(k)));
                }
            }
        }
        time = e->getIntAttribute(s[0] + "_time", 0);
        maxN = e->getIntAttribute(s[0] + "_maxN", 0);
        value = base;
    }
    
    template <class T>
    void setState(tag<Array<Array<T>>>, XmlElement* e, StringArray s, ValueType defaultValue)
    {
        base = defaultValue;
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(s[0]))
            {
                base = Array<Array<T>>();
                int count = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName(s[1] + String(count++)))
                    {
                        Array<T> ba;
                        for (int k = 0; k < asub->getNumAttributes(); k++)
                            ba.add(getAttribute(tag<T>{}, asub, s[2] + String(k)));
                        base.add(ba);
                    }
                }
            }
            else if (sub->hasTagName(s[0] + "_inc"))
            {
                inc = Array<Array<T>>();
                int count = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName(s[1] + String(count++)))
                    {
                        Array<T> ia;
                        for (int k = 0; k < asub->getNumAttributes(); k++)
                            ia.add(getAttribute(tag<T>{}, asub, s[2] + String(k)));
                        inc.add(ia);
                    }
                }
            }
        }
        time = e->getIntAttribute(s[0] + "_time", 0);
        maxN = e->getIntAttribute(s[0] + "_maxN", 0);
        value = base;
    }
    //==============================================================================
    
    // Moddables are exist in pairs, with one in a preparation that is being modded
    // and one in a modification that contains info for performing modifications.
    // For a Moddable in a mod, base, value, and mod are generally the same. For
    // moddable in a preparation, value is the current value to be used in any
    // processing, base is the value set in the preparation editor which the preparation
    // can be reset to, and mod is the target value set when modify is called()
    
    ValueType value;
    ValueType base;
    ValueType mod;
    
    // Only used by mod Moddables - the amount by which the mod value should be adjusted
    // on each activation
    ValueType inc;
    
    // Whether a prep Moddable is in the process of modding, irrelevant in mod Moddable
    bool active;
    
private:
    template <class T = ValueType>
    void initInc(tag<T>) { ; }
    void initInc(tag<double>) { inc = 0.0; }
    void initInc(tag<int>) { inc = 0; }
    void initInc(tag<float>) { inc = 0.0f; }
    void initInc(tag<Array<double>>) { inc = Array<double>(0.0); }
    void initInc(tag<Array<int>>) { inc = Array<int>(0); }
    void initInc(tag<Array<float>>) { inc = Array<float>(0.0f); }
    
    void calcDV(tag<int>)
    {
        dv = mod - value;
    }
    void calcDV(tag<double>)
    {
        if (time == 0) dv = mod - value;
        else dv = (mod - value) / time;
    }
    void calcDV(tag<float>) { calcDV(tag<double>{}); }
    
    float getAttribute(tag<double>, XmlElement* e, String tagName, double v = 0.0)
    {
        return e->getDoubleAttribute(tagName, v);
    }
    float getAttribute(tag<float>, XmlElement* e, String tagName, float v = 0.0f)
    {
        return e->getDoubleAttribute(tagName, v);
    }
    int getAttribute(tag<int>, XmlElement* e, String tagName, int v = 0)
    {
        return e->getIntAttribute(tagName, v);
    }
    bool getAttribute(tag<bool>, XmlElement* e, String tagName, bool v = false)
    {
        return e->getBoolAttribute(tagName, v);
    }
    String getAttribute(tag<String>, XmlElement* e, String tagName, String v = String())
    {
        return e->getStringAttribute(tagName, v);
    }
    
    // Time to mod  of a prep Moddable which changes to match the time of a mod Moddable
    int time;
    
    // Amount of change to <value> per step() to reach <mod> in <time>. Calculated when Moddable is activated
    ValueType dv;
    
    int timeElapsed;
    
    // Irrelevant in prep Moddable, increment count of mod Moddable
    int n;
    
    // Max number of times mod can be incremented
    int maxN;
    
    bool dB;
};

