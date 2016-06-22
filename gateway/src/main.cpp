#include "mbed.h"
#include "rtos.h"
#include "common_types.h"
#include "net.hpp"
#include "can.hpp"


Serial pc(USBTX, USBRX); // tx, rx


DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);


#define RECEIVER_PERIOD_MS   500 //milliseconds
#define SENDER_PERIOD_MS     800 //milliseconds


//void sender(void const* t);
void receiver(void const* t);


int main() {

  led1 = 0;
  led2 = 0;

  set_time(0);

//  pc.baud(9600);

  uint32 message_mask = NET_TX_CMD_BODY | NET_RX_STS_BODY | 
                        NET_TX_CMD_ENGINE |
                        NET_TX_CMD_DIAG | NET_RX_STS_DIAG | 
                        NET_TX_CMD_TIME;
  init_can(message_mask);

//  Thread* th_txer = new Thread(sender,      100, SENDER_PERIOD_MS,   NULL, osPriorityHigh);
  Thread* th_rxer = new Thread(receiver,    100, RECEIVER_PERIOD_MS, NULL, osPriorityHigh);
  Thread* th_can  = new Thread(thread_can,   12, CAN_THREAD_PERIOD,  NULL, osPriorityRealtime);

  while(1) {};
}

/*
void sender (void const* t) {
  unsigned int counter = 0;

  while(1) {

    char buffer[5] = {0, 0, 0, 0, 0};
    unsigned int len = 1;

    if (can_msg_is_missing(CAN_MISSING_STS_BODY_ID))
      buffer[0] = 0x80;
    else {
      if (can_sts_body.flag == CAN_FLAG_RECEIVED) {
        can_sts_body_payload_t x = can_sts_body.payload;
        buffer[0] = x.msg.hit_front;
        buffer[0] |= x.msg.hit_rear << 1;
        buffer[0] |= x.msg.hit_left << 2;
        buffer[0] |= x.msg.hit_right << 3;
        buffer[0] |= x.msg.light_sens << 4;
        buffer[1] = x.msg.eye_l;
        buffer[2] = x.msg.eye_r;
      }
    }

    if (can_sts_diag.flag == CAN_FLAG_RECEIVED) {
      can_sts_diag_payload_t x = can_sts_diag.payload;
      buffer[3] = x.msg.data;
      len++;
      can_sts_diag.flag = CAN_FLAG_EMPTY;
    }

    buffer[4] = '\n';

    int cnt = 0;
    while (cnt < 5) {
      if (!pc.writeable())
        continue;
      pc.putc(buffer[cnt]);
      cnt++;
    }

    led1 = counter % 2;
    counter++;

    osThreadEndPeriod();
  }
}


void receiver (void const* t) {
  unsigned int counter = 0;
  char c;
  int expected = -1;

  while (1) {
    if (pc.readable() == 0)
      continue;

    c = pc.getc();

    if (c == '\n') {
      expected = 0;
      continue;
    }

    switch (expected) {
      case 0:
        can_cmd_engine.payload.msg.steering = c / 2 + 50;
        break;
      case 1:
        can_cmd_engine.payload.msg.direction = c > 0;
        can_cmd_engine.payload.msg.power = c / 2 + 50;
        break;
      case 2:
        if (c != 0) {
          can_cmd_diag.payload.msg.cmd = 0x0A0A;
          can_cmd_diag.payload.msg.data = c;
          can_cmd_diag.flag = CAN_FLAG_SEND;
        }
        break;
      case 3:
        if (c != 0) {
          can_cmd_diag.payload.msg.cmd = 0x0A0B;
          can_cmd_diag.payload.msg.data = c;
          can_cmd_diag.flag = CAN_FLAG_SEND;
        }
        break;
      case 4:
        can_cmd_body.payload.msg.light_r = (c & 0x01) ? 1 : 0;
        can_cmd_body.payload.msg.light_c = (c & 0x02) ? 1 : 0;
        can_cmd_body.payload.msg.light_l = (c & 0x04) ? 1 : 0;
        can_cmd_engine.payload.msg.breaking = (c & 0x08) ? 1 : 0;
        break;
    }
    expected++;
*/
/*
end_cycle:
    total--;
    if (total == 0) {
      total = -1;
      led2 = counter % 2;
      counter++;
      osThreadEndPeriod();
    }
*/
/*
  }
}
*/

void receiver (void const* t) {
  char c;
  char buffer[2];
  static int count = 0;
  static int count2 = 0;

  while (1) {
    if (pc.readable() == 0)
      continue;

bool toSend = false;

    c = pc.getc();
    count2++;
    led1 = count & 2;
    can_sts_body_payload_t x = can_sts_body.payload;
led3 = x.msg.light_sens;
    switch(c) {
      case 0xBC:
        count++;
        led2 = count & 2;
        buffer[0]  = x.msg.light_sens;
        buffer[0]  = buffer[0] << 1;
        buffer[0] += x.msg.hit_front;
        buffer[0]  = buffer[0] << 1;
        buffer[0] += x.msg.hit_rear;
        buffer[0]  = buffer[0] << 1;
        buffer[0] += x.msg.hit_left;
        buffer[0]  = buffer[0] << 1;
        buffer[0] += x.msg.hit_right;
toSend = true;
        break;
    }
    buffer[1] = '\n';
if (toSend) {
    int cnt = 0;
    while (cnt < 2) {
      if (!pc.writeable())
        continue;
      pc.putc(buffer[cnt]);
      cnt++;
    }
}
  }
}

