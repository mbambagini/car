#ifndef __CLOCK_HPP__
#define __CLOCK_HPP__

/*! \brief initialize the clock by setting it at 00:00
 *
 */
void init_clock();

/*! \brief update clock
 * This thread periodically checks if a CLOCK message has been received and,
 * if so, updates the internal clock
 *
 * \param args not used
 */
void thread_clock (void const *args);

#endif //__CLOCK_HPP__

