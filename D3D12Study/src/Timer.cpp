

#include "Timer.h"



LARGE_INTEGER Timer::freq = { };   



Timer::Timer()
{
  
    if (!freq.QuadPart)
    {
       
        QueryPerformanceFrequency(&freq);
    }

   
    ZeroMemory(&start, sizeof(start));
    ZeroMemory(&end, sizeof(end));

  
    stoped = false;
}

void Timer::Start()
{
    if (stoped)
    {
       

        
        llong elapsed = end.QuadPart - start.QuadPart;

        
        QueryPerformanceCounter(&start);
        start.QuadPart -= elapsed;

        
        stoped = false;
    }
    else
    {
        
        QueryPerformanceCounter(&start);
    }
}



void Timer::Stop()
{
    if (!stoped)
    {
       
        QueryPerformanceCounter(&end);
        stoped = true;
    }
}



double Timer::Reset()
{
    llong elapsed;

    if (stoped)
    {
       
        elapsed = end.QuadPart - start.QuadPart;

       
        QueryPerformanceCounter(&start);

       
        stoped = false;
    }
    else
    {
        
        QueryPerformanceCounter(&end);

       
        elapsed = end.QuadPart - start.QuadPart;

        
        start = end;
    }

    
    return elapsed / double(freq.QuadPart);
}



llong Timer::Stamp()
{
    QueryPerformanceCounter(&end);
    return end.QuadPart;
}



double Timer::Elapsed()
{
    llong elapsed;

    if (stoped)
    {
       
        elapsed = end.QuadPart - start.QuadPart;
    }
    else
    {
     
        QueryPerformanceCounter(&end);

    
        elapsed = end.QuadPart - start.QuadPart;
    }

   
    return elapsed / double(freq.QuadPart);
}



double Timer::Elapsed(llong stamp)
{
    llong elapsed;

    if (stoped)
    {
        
        elapsed = end.QuadPart - stamp;

    }
    else
    {
    
        QueryPerformanceCounter(&end);

     
        elapsed = end.QuadPart - stamp;
    }


    return elapsed / double(freq.QuadPart);
}
