//this project implements the body/engine devices

#include "mbed.h"
#include "common_types.h"
#include "rtos.h"

#include "can.hpp"
#include "led.hpp"
#include "body.hpp"
#include "engine.hpp"
#include "diag.hpp"
#include "clock.hpp"

//initialize the system:
//- single components: body, clock, diagnosis, engine
//- hardware: leds, can
//- threads
int init();

int main()
{
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

void init_threads ()
{
  th_body = new Thread(thread_body);
  th_engine = new Thread(thread_engine);
  th_can = new Thread(thread_can);
  th_diag = new Thread(thread_diag);
  th_clock = new Thread(thread_clock);
}

int init ()
{
  init_body();
  init_clock();
  init_diag();
  init_engine();

  //printf("INIT LED\r\n");
  init_led();

  //printf("INIT CAN\r\n");
  init_can();

  //printf("INIT THREAD\r\n");
  init_threads();

  return true;
}
