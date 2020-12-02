/*
  ==============================================================================

    Moddable.h
    Created: 2 Dec 2020 11:56:22am
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

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
    
    Moddable (ValueType v, ValueType m, int t):
    value(v),
    base(v),
    mod(m),
    time(t),
    dv(v), // Make sure to properly calculate dv before (and if) it is needed
    active(false),
    n(0),
    maxN(0) {};
    
    Moddable (ValueType v, int t):
    Moddable (v, v, t) {}
    
    Moddable (ValueType v):
    Moddable (v, v, 0) {}
    
    Moddable (const Moddable& m):
    Moddable (m.base, m.mod, m.time)
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
        if (!active) mod = v;
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
        mod = m.mod;
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
            if (m.n < m.maxN) m.n++;
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
    
    void setInc(tag<int>, int v) { inc = v; n = 0; }
    void setInc(tag<float>, float v) { inc = v; n = 0; }
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
    void getState(ValueTree& vt, String s) { getState(tag<ValueType>{}, vt, s); }
    
    template<class T = ValueType>
    void getState(tag<T>, ValueTree& vt, String s)
    {
        vt.setProperty(s, base, 0);
        vt.setProperty(s + "_mod", mod, 0);
        vt.setProperty(s + "_time", time, 0);
        vt.setProperty(s + "_maxN", maxN, 0);
    }
    
    void getState(tag<int>, ValueTree& vt, String s) { getState(tag<double>{}, vt, s); }
    void getState(tag<float>, ValueTree& vt, String s) { getState(tag<double>{}, vt, s); }
    void getState(tag<double>, ValueTree& vt, String s)
    {
        vt.setProperty(s, base, 0);
        vt.setProperty(s + "_mod", mod, 0);
        vt.setProperty(s + "_inc", inc, 0);
        vt.setProperty(s + "_time", time, 0);
        vt.setProperty(s + "_maxN", maxN, 0);
    }
    void getState(tag<Array<float>>, ValueTree& vt, String s)
    {
        int count = 0;
        for (auto v : base) vt.setProperty(s + String(count++), v, 0);
        count = 0;
        for (auto v : mod) vt.setProperty(s + "_mod" + String(count++), v, 0);
        count = 0;
        for (auto v : inc) vt.setProperty(s + "_inc" + String(count++), v, 0);
        vt.setProperty(s + "_time", time, 0);
        vt.setProperty(s + "_maxN", maxN, 0);
    }
    
    //==============================================================================
    
    void setState(XmlElement* e, String s, ValueType defaultValue)
    { setState(tag<ValueType>{}, e, s, defaultValue); }
    
    template<class T = ValueType>
    void setState(tag<T>, XmlElement* e, String s, T defaultValue)
    {
        base = T(e->getIntAttribute(s, defaultValue));
        mod = T(e->getIntAttribute(s + "_mod", base));
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    
    void setState(tag<int>, XmlElement* e, String s, ValueType defaultValue)
    { setState(tag<double>{}, e, s, defaultValue); }
    void setState(tag<float>, XmlElement* e, String s, ValueType defaultValue)
    { setState(tag<double>{}, e, s, defaultValue); }
    void setState(tag<double>, XmlElement* e, String s, ValueType defaultValue)
    {
        base = e->getDoubleAttribute(s, defaultValue);
        mod = e->getDoubleAttribute(s + "_mod", base);
        inc = e->getDoubleAttribute(s + "_inc", 0.0);
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    void setState(tag<bool>, XmlElement* e, String s, ValueType defaultValue)
    {
        base = e->getBoolAttribute(s, defaultValue);
        mod = e->getBoolAttribute(s + "_mod", base);
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    void setState(tag<String>, XmlElement* e, String s, ValueType defaultValue)
    {
        base = e->getStringAttribute(s, defaultValue);
        mod = e->getStringAttribute(s + "_mod", base);
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    void setState(tag<Array<float>>, XmlElement* e, String s, ValueType defaultValue)
    {
        base = Array<float>();
        mod = Array<float>();
        inc = Array<float>();
        int count = 0;
        for (int k = 0; k < e->getNumAttributes(); k++)
        {
            if (e->hasAttribute(s + String(count)))
            {
                float b = e->getDoubleAttribute(s + String(count), 0.0f);
                base.add(b);
                mod.add(e->getDoubleAttribute(s + "_mod" + String(count), b));
                inc.add(e->getDoubleAttribute(s + "_inc" + String(count), 0.0f));
            }
            count++;
        }
        
        time = e->getIntAttribute(s + "_time", 0);
        maxN = e->getIntAttribute(s + "_maxN", 0);
        value = base;
    }
    //==============================================================================
    
    // Moddables are used in pairs, with one existing in a preparation that is being modded
    // and one in a modification that contains mod info. Some of these members function differently
    // depending on which of the pair the Moddable is.
    
    // Current value being used by a prep Moddable, irrelevant in mod Moddable
    ValueType value;
    
    // Initial value of a prep Moddable, irrelevant in mod Moddable
    ValueType base;
    
    // Mod value of a prep Moddable which changes to match the mod value of a mod Moddable
    ValueType mod;
    
    // Irrelevant in prep Moddable, increment amount of mod Moddable
    ValueType inc;
    
private:
    void calcDV(tag<int>)
    {
        dv = mod - value;
    }
    void calcDV(tag<double>)
    {
        if (time == 0) dv = mod - value;
        else dv = (mod - value) / time;
    }
    void calcDV(tag<float>)
    {
        if (time == 0) dv = mod - value;
        else dv = (mod - value) / time;
    }
    
    // Time to mod  of a prep Moddable which changes to match the time of a mod Moddable
    int time;
    
    // Amount of change to <value> per step() to reach <mod> in <time>. Calculated when Moddable is activated
    ValueType dv;
    
    int timeElapsed;
    
    // Whether a prep Moddable is in the process of modding, irrelevant in mod Moddable
    bool active;
    
    // Irrelevant in prep Moddable, increment count of mod Moddable
    int n;
    
    // Max number of times mod can be incremented
    int maxN;
};
