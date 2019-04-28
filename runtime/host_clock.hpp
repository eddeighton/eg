
#ifndef EG_CLOCK_IMPL_24_04_2019
#define EG_CLOCK_IMPL_24_04_2019

struct HostClock : public eg::_clock
{
public:
    typedef std::chrono::steady_clock ClockType;
    typedef ClockType::time_point Tick;
    typedef ClockType::duration TickDuration;
    typedef std::chrono::duration< float, std::ratio< 1 > > FloatTickDuration;
    
    HostClock()
    {
        m_lastTick = m_startTick = ClockType::now();
        m_cycle = m_subcycle = m_cycleSubCycle = 0;
        m_ct = m_dt = 0.0f;
    }
    
    inline Tick nextCycle()
    {
        const Tick nowTick = ClockType::now();
        m_dt = FloatTickDuration( nowTick - m_lastTick  ).count();
        m_ct = FloatTickDuration( nowTick - m_startTick ).count();
        
        m_lastTick = nowTick;
        
        nextSubCycle();
        m_cycleSubCycle = m_subcycle;
        
        ++m_cycle;
        
        return nowTick;
    }
    
    inline void nextSubCycle()
    {
        ++m_subcycle;
    }
    
    inline std::size_t cycleSubCycles() const { return m_cycleSubCycle; }
    
    Tick actual() const { return ClockType::now(); }
    
    //interface
    virtual eg::TimeStamp cycle()    const { return m_cycle; }
    virtual eg::TimeStamp subcycle() const { return m_subcycle; }
    virtual float ct()     const { return m_ct; }
    virtual float dt()     const { return m_dt; }
    
private:
    Tick m_lastTick, m_startTick;
    eg::TimeStamp m_cycle, m_subcycle, m_cycleSubCycle;
    float m_ct, m_dt;
};

#endif //EG_CLOCK_IMPL_24_04_2019