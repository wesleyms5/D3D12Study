#pragma once

#ifndef DXUT_TIMER_H
#define DXUT_TIMER_H

#include <windows.h>                        
#include "Types.h"                            

class Timer {

private:
    static LARGE_INTEGER freq; // ticks per second
    LARGE_INTEGER start, end; // start and end time
    bool stoped;

public:
    Timer();

    void Start();
    void Stop();
    double Reset();
    double Elapsed();
    bool Elapsed(double secs);

    llong Stamp();
    double Elapsed(llong stamp);
    bool Elapsed(llong stamp, double secs);

};


inline bool Timer::Elapsed(double secs)
{
    return (Elapsed() >= secs ? true : false);
}
inline bool Timer::Elapsed(llong stamp, double secs)
{
    return (Elapsed(stamp) >= secs ? true : false);
}


#endif