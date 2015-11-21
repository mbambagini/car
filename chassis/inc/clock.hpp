#ifndef __CLOCK_HPP__
#define __CLOCK_HPP__

//initialize the clock by setting it at 00:00
void init_clock();

//handle clock messages to update internal clock
void thread_clock (void const *args);

#endif //__CLOCK_HPP__

