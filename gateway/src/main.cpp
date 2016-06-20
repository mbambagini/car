#include "mbed.h"
#include "rtos.h"
#include "common_types.h"
#include "net.hpp"
#include "can.hpp"


Serial pc(USBTX, USBRX); // tx, rx


DigitalOut led1(LED1);
DigitalOut led2(LED2);


#define RECEIVER_PERIOD_MS   500 //milliseconds

#define SENDER_PERIOD_MS     800 //milliseconds


void sender(void const* t);
void receiver(void const* t);

unsigned long int COUNTER = 0;

int main() {

  led1 = 0;
  set_time(0);
  uint32 message_mask = NET_TX_CMD_BODY | NET_RX_STS_BODY | 
                        NET_TX_CMD_ENGINE |
                        NET_TX_CMD_DIAG | NET_RX_STS_DIAG | 
                        NET_TX_CMD_TIME;
  init_can(message_mask);
  Thread* th_txer = new Thread(sender,      100, SENDER_PERIOD_MS,   NULL, osPriorityHigh);
  Thread* th_rxer = new Thread(receiver,    100, RECEIVER_PERIOD_MS, NULL, osPriorityHigh);
  Thread* th_can  = new Thread(thread_can,  12, CAN_THREAD_PERIOD,  NULL, osPriorityRealtime);

  pc.baud(115200);

  while(1) {};

}

void sender (void const* t) {
  static int counter = 0;

  while(1) {

    char buffer[4] = {0, 0, 0, 0};
    unsigned int len = 1;

    if (!can_msg_is_missing(CAN_MISSING_STS_BODY_ID) && can_sts_body.flag == CAN_FLAG_RECEIVED) {
      can_sts_body_payload_t x = can_sts_body.payload;
      buffer[1] = x.msg.hit_front;
      buffer[1] = x.msg.hit_rear << 1;
      buffer[1] = x.msg.hit_left << 2;
      buffer[1] = x.msg.hit_right << 3;
      buffer[1] = x.msg.light_sens << 4;

      buffer[2] = x.msg.eye_l;
      buffer[3] = x.msg.eye_r;
      can_sts_body.flag = CAN_FLAG_EMPTY;

      buffer[0] = 0x03;
      len += 3;
    }

    if (can_sts_diag.flag == CAN_FLAG_RECEIVED) {
      can_sts_diag_payload_t x = can_sts_diag.payload;
      buffer[len] = x.msg.data;
      buffer[0] |= 0x04;
      len++;
      can_sts_diag.flag = CAN_FLAG_EMPTY;
    }

    int cnt = 0;
    while (cnt < len) {
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
  int total = -1;
  unsigned int counter = 0;
  char cmd = 0;
  char c;

  while (1) {
    if (pc.readable() == 0)
      continue;

    c = pc.getc();
    if (total == -1) { //first byte
      total = 1 + (c&0x01) + (c&0x02)?1:0 + (c&0x04)?1:0 + (c&0x08)?1:0 + (c&0x10)?1:0;
      cmd = c;
      goto end_cycle;
    }
    if (cmd & 0x01) {
      can_cmd_engine.payload.msg.steering = c / 2 + 50;
      cmd &= 0xFE; //clear stearing byte presence
      goto end_cycle;
    }
    if (cmd & 0x02) {
      can_cmd_engine.payload.msg.direction = c > 0;
      can_cmd_engine.payload.msg.power = c / 2 + 50;
      cmd &= 0xFD; //clear engine power byte presence
      goto end_cycle;
    }
    if (cmd & 0x04) { // skip echo bytes
      can_cmd_diag.payload.msg.cmd = 0x0A0A;
      can_cmd_diag.payload.msg.data = c;
      can_cmd_diag.flag = CAN_FLAG_SEND;
      cmd &= 0xFB; //clear echo bytes presence
      goto end_cycle;
    }
    if (cmd & 0x08) { // skip echo bytes
      can_cmd_diag.payload.msg.cmd = 0x0A0B;
      can_cmd_diag.payload.msg.data = c;
      can_cmd_diag.flag = CAN_FLAG_SEND;
      cmd &= 0xF7; //clear echo bytes presence
      goto end_cycle;
    }
    if (cmd & 0x10) { // flags
      can_cmd_body.payload.msg.light_r = (c & 0x01) ? 1 : 0;
      can_cmd_body.payload.msg.light_c = (c & 0x02) ? 1 : 0;
      can_cmd_body.payload.msg.light_l = (c & 0x04) ? 1 : 0;
      can_cmd_engine.payload.msg.breaking = (c & 0x08) ? 1 : 0;
      cmd &= 0xF7; //clear echo bytes presence
      goto end_cycle;
    }

end_cycle:
    total--;
    if (total == 0) {
      total = -1;
      osThreadEndPeriod();
      led2 = counter % 2;
      counter++;
    }
  }
}

