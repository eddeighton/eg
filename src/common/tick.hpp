/*
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef TICK_17_01_2013
#define TICK_17_01_2013

#include <stdint.h>

#include <chrono>
#include <ratio>

namespace Timing
{

typedef std::chrono::steady_clock ClockType;
typedef ClockType::time_point Tick;
typedef ClockType::duration TickDuration;
typedef std::chrono::duration< float, std::ratio< 1 > > FloatTickDuration;

class Clock
{
public:
    Clock()
    {
        m_lastTick = m_startTick = ClockType::now();
    }
    
    float now() const 
    {
        return FloatTickDuration( ClockType::now() - m_startTick ).count();
    }
    
    inline void operator()( float& ct, float& dt )
    {
        const Tick nowTick = ClockType::now();
        dt = FloatTickDuration( nowTick - m_lastTick  ).count();
        ct = FloatTickDuration( nowTick - m_startTick ).count();
        
        m_lastTick = nowTick;
    }
    
private:
    Tick m_lastTick, m_startTick;
};


class UpdateTick
{
    Tick m_tick;
    UpdateTick& operator=( const UpdateTick& );
public:
    UpdateTick() : m_tick( ClockType::now() ) {}

    inline bool elapsed( float fTime ) const
    {
        const float fElapsedTime = FloatTickDuration(ClockType::now() - m_tick).count();
        return fElapsedTime >= fTime;
    }
    inline bool operator<( const UpdateTick& cmp ) const
    {
        return m_tick < cmp.m_tick;
    }
    inline bool operator>( const UpdateTick& cmp ) const
    {
        return cmp < *this;
    }

    void update() { m_tick = ClockType::now(); }
};

}

#endif //TICK_17_01_2013