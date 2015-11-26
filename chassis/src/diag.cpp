#include "common_types.h"
#include "can.hpp"
#include "net.hpp"
#include "mbed.h"
#include "rtos.h"

void init_diag () {
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
        default:
          //ignore it
          break;
      }
      can_cmd_diag.flag = CAN_FLAG_EMPTY;
    }
    Thread::wait(DIAG_THREAD_PERIOD);
  }
}
