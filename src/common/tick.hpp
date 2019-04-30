//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

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