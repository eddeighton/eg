

namespace
{

struct HostClock
{
public:
    typedef std::chrono::steady_clock ClockType;
    typedef ClockType::time_point Tick;
    typedef ClockType::duration TickDuration;
    typedef std::chrono::duration< float, std::ratio< 1 > > FloatTickDuration;
    
    HostClock()
    {
        m_lastTick = m_startTick = ClockType::now();
        m_cycle = 1U;
        m_ct = m_dt = 0.0f;
    }
    
    inline void nextCycle()
    {
        const Tick nowTick = ClockType::now();
        m_dt = FloatTickDuration( nowTick - m_lastTick  ).count();
        m_ct = FloatTickDuration( nowTick - m_startTick ).count();
        m_lastTick = nowTick;
        ++m_cycle;
    }
    
    inline Tick actual()           const { return ClockType::now(); }
    inline eg::TimeStamp cycle()   const { return m_cycle; }
    inline float ct()              const { return m_ct; }
    inline float dt()              const { return m_dt; }
    
private:
    Tick m_lastTick, m_startTick;
    eg::TimeStamp m_cycle;
    float m_ct, m_dt;
} 

theClock;

}

eg::TimeStamp clock::cycle()
{
    return theClock.cycle();
}
float clock::ct()
{
    return theClock.ct();
}
float clock::dt()
{
    return theClock.dt();
}
void clock::next()
{
    theClock.nextCycle();
}

