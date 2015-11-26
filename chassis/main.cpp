/*! \file main.cpp
 * this file contains the main loop which initializes the system and then
 * lets it run.
 * This project is the BCM/ECM SW of the remote-controlled car
 */

#include "mbed.h"
#include "rtos.h"

#include "common_types.h"

#include "can.hpp"
#include "led.hpp"
#include "body.hpp"
#include "engine.hpp"
#include "diag.hpp"
#include "clock.hpp"

/*! \brief system initialization
 * the initialization components of the system are:
 * - hardware: can
 * - single components: body, clock, diagnosis, engine, leds
 * - threads
 */
void init();

int main() {
  //system setup
  init();

  //main loop
  while(1) {};
}

Thread *th_body;
Thread *th_can;
Thread *th_engine;
Thread *th_diag;
Thread *th_clock;

/*! \brief start all threads
 *
 */
void init_threads () {
  th_body = new Thread(thread_body);
  th_engine = new Thread(thread_engine);
  th_can = new Thread(thread_can);
  th_diag = new Thread(thread_diag);
  th_clock = new Thread(thread_clock);
}

void init () {
  //setup network
  init_can();

  //setup components
  init_body();
  init_clock();
  init_diag();
  init_engine();
  init_led();

  //run threads
  init_threads();
}

