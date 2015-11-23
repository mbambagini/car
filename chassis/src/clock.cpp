#include "common_types.h"
#include "net.hpp"
#include "can.hpp"
#include "mbed.h"
#include "rtos.h"

void init_clock () {
  set_time(0);
}

void thread_clock (void const *args) {
  while(1) {
    //if a time update has been received
    if (can_cmd_time.flag == CAN_FLAG_RECEIVED) {
      //update time
      set_time(can_cmd_time.payload.msg.time);
#ifdef DEBUG
      time_t seconds = time(NULL);
      printf("time: %s\r\n", ctime(&seconds));
#endif
      //set the message as read
      can_cmd_time.flag = CAN_FLAG_EMPTY;
    }
    Thread::wait(CLOCK_THREAD_PERIOD);
  }
}
