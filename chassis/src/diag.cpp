/*! \file diag.cpp
 * \brief Implementation of diag task
 *
 * The diag task is in charge of checking periodically if there are any
 * diagnosis request from the driver. The ones, which are implemeted so
 * far, are:
 * - CMD_ECHO_BCM: periodically sent to check if the BCM is alive
 * - CMD_ECHO_ECM: periodically sent to check if the ECM is alive
 * - CMD_TIME_ALL: broadcast msg which updates the clock
 */

#include "common_types.h"
#include "can.hpp"
#include "net.hpp"
#include "mbed.h"
#include "rtos.h"

void init_diag () {
  set_time(0);
}

void thread_diag (void const *args) {
  while(1) {
    if (can_cmd_diag.flag == CAN_FLAG_RECEIVED) {
      uint16 cmd = can_cmd_diag.payload.msg.cmd;
      uint32 data = can_cmd_diag.payload.msg.data;
      switch(cmd) {
        case CMD_ECHO_ECM:
        case CMD_ECHO_BCM:
          can_sts_diag.payload.msg.data = data;
          can_sts_diag.flag = CAN_FLAG_SEND;
          break;
        case CMD_TIME_ALL:
          can_sts_diag.payload.msg.data = data;
          can_sts_diag.flag = CAN_FLAG_SEND;
          set_time(data); //+ offset 1/1/2000
#ifdef DEBUG
          time_t seconds = time(NULL);
          printf("time: %s\r\n", ctime(&seconds));
#endif
          break;
        default:
          //ignore it
          break;
      }
      can_cmd_diag.flag = CAN_FLAG_EMPTY;
    }
    osThreadEndPeriod();
//    Thread::wait(DIAG_THREAD_PERIOD);
  }
}

