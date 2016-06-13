/*! \file main.cpp
 * this file contains the main loop which initializes the system and then
 * lets it run.
 * This project is the BCM/ECM SW of the remote-controlled car
 */

/*! \mainpage CAR Chassis project
*
* This project is the software which implements the BCM and ECM components
* of the remote-controlled car
*
* \section pinout
*               _______________________
*              |                       |
*          gnd | p1                p40 | X
*          +5V | p2                p39 | X
*            X | p3                p38 | X
*            X | p4                p37 | X
*   e2prom si  | p5                p36 | X
*   e2prom so  | p6                p35 | X
*   e2prom sck | p7                p34 | X
*   e2prom en  | p8                p33 | X
*       can tx | p9                p32 | X
*       can rx | p10               p31 | X
*  touch back  | p11               p30 |
*  touch front | p12               p29 |
*              | p13               p28 |
*              | p14               p27 | light l
*  touch right | p15               p26 | light c
*  touch left  | p16               p25 | light r
*        photo | p17               p24 | engine direction 1
*              | p18               p23 | engine direction 2
*       eye_r  | p19               p22 | engine power
*       eye_l  | p20               p21 | steering
*              |_______________________|
*
* \section install_sec Compile
* 
* To compile the module just type:
* \code{.sh}
* make
* \endcode
* The elf file is produced in the same directory.
* 
* To update documentation, type:
* \code{.sh}
* make doc
* \endcode
*/

#include "mbed.h"
#include "rtos.h"

#include "common_types.h"

#include "net.hpp"
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
  th_body   = new Thread(thread_body,   20, 100, NULL, osPriorityHigh);
  th_engine = new Thread(thread_engine, 20, 100, NULL, osPriorityHigh);
  th_can    = new Thread(thread_can,    15,  25, NULL, osPriorityRealtime);
  th_diag   = new Thread(thread_diag,   20, 200, NULL, osPriorityHigh);
  th_clock  = new Thread(thread_clock,  20, 500, NULL, osPriorityHigh);
}

void init () {
  //setup network
  uint32 message_mask = NET_RX_CMD_BODY & NET_TX_STS_BODY & 
                        NET_RX_CMD_ENGINE &
                        NET_RX_CMD_DIAG & NET_TX_STS_DIAG &
                        NET_RX_CMD_TIME;
  init_can(message_mask);

  //setup components
  init_body();
  init_clock();
  init_diag();
  init_engine();
  init_led();

  //run threads
  init_threads();
}

