#include "mbed.h"
#include "rtos.h"
#include "common_types.h"
#include "net.hpp"
#include "can.hpp"

Serial pc(USBTX, USBRX); // tx, rx

void receiver();

int main() {
  set_time(0);
  uint32 message_mask = NET_TX_CMD_BODY | NET_RX_STS_BODY | 
                        NET_TX_CMD_ENGINE |
                        NET_TX_CMD_DIAG | NET_RX_STS_DIAG | 
                        NET_TX_CMD_TIME;
  init_can(message_mask);

  Thread* th_can = new Thread(thread_can, 12, CAN_THREAD_PERIOD, NULL,
                                                           osPriorityRealtime);

  receiver();

  while(1);
  return 0;
}


void receiver () {
  char c;
  char buffer[10];
  unsigned int ui, cnt;
  char r;
  unsigned int nToSend;

  while (1) {
    if (pc.readable() == 0)
      continue;

    // read and parse command 
    c = pc.getc();

    nToSend = 0;
    switch(c) {
      case 0x10:
        goto ACTION_STATUS_BCM;
      case 0x11:
        goto ACTION_STATUS_BCM_EYE_R;
      case 0x12:
        goto ACTION_STATUS_BCM_EYE_L;
      case 0x13:
        goto ACTION_COMMAND_BCM;

      case 0x20:
        goto ACTION_ENGINE_STEERING;
      case 0x21:
        goto ACTION_ENGINE_POWER;

      case 0x30:
        goto ACTION_COMMAND_ECHO_BCM;
      case 0x31:
        goto ACTION_COMMAND_ECHO_ECM;
      case 0x32:
        goto ACTION_STATUS_DIAG;
    }
    goto ACTION_EXIT;

ACTION_STATUS_BCM:
    buffer[0] = can_sts_body.payload.msg.light_sens ? 1 : 0;
    buffer[0]  = buffer[0] << 1;
    buffer[0] += can_sts_body.payload.msg.hit_front ? 1 : 0;
    buffer[0]  = buffer[0] << 1;
    buffer[0] += can_sts_body.payload.msg.hit_rear ? 1 : 0;
    buffer[0]  = buffer[0] << 1;
    buffer[0] += can_sts_body.payload.msg.hit_left ? 1 : 0;
    buffer[0]  = buffer[0] << 1;
    buffer[0] += can_sts_body.payload.msg.hit_right ? 1 : 0;
    buffer[0] |= 0x80;
    nToSend = 1;
    goto ACTION_EXIT;

ACTION_COMMAND_BCM:
    while (pc.readable() == 0);
    r = pc.getc();
    can_cmd_body.payload.msg.light_r = (r & 0x02) ? 1 : 0;
    can_cmd_body.payload.msg.light_c = (r & 0x04) ? 1 : 0;
    can_cmd_body.payload.msg.light_l = (r & 0x08) ? 1 : 0;
    can_cmd_engine.payload.msg.breaking = (r & 0x01) ? 1 : 0;    
    goto ACTION_EXIT;

ACTION_STATUS_BCM_EYE_R:
    buffer[0]  = can_sts_body.payload.msg.eye_r;
    buffer[0] |= 0x80;
    nToSend = 1;
    goto ACTION_EXIT;

ACTION_STATUS_BCM_EYE_L:
    buffer[0]  = can_sts_body.payload.msg.eye_l;
    buffer[0] |= 0x80;
    nToSend = 1;
    goto ACTION_EXIT;

ACTION_ENGINE_STEERING:
    while (pc.readable() == 0);
    r = pc.getc();
    can_cmd_engine.payload.msg.steering = (r / 2) + 50;
    goto ACTION_EXIT;

ACTION_ENGINE_POWER:
    while (pc.readable() == 0);
    r = pc.getc();
    can_cmd_engine.payload.msg.direction = r > 0;
    can_cmd_engine.payload.msg.power = abs(r);
    goto ACTION_EXIT;

ACTION_COMMAND_ECHO_BCM:
    while (pc.readable() == 0);
    r = pc.getc();
    can_cmd_diag.payload.msg.cmd = 0x0A0A;
    can_cmd_diag.payload.msg.data = r;
    can_cmd_diag.flag = CAN_FLAG_SEND;
    goto ACTION_EXIT;

ACTION_COMMAND_ECHO_ECM:
    while (pc.readable() == 0);
    r = pc.getc();
    can_cmd_diag.payload.msg.cmd = 0x0A0B;
    can_cmd_diag.payload.msg.data = r;
    can_cmd_diag.flag = CAN_FLAG_SEND;
    goto ACTION_EXIT;

ACTION_STATUS_DIAG:
    ui = 0;
    if (can_sts_diag.flag == CAN_FLAG_RECEIVED) {
      ui = can_sts_diag.payload.msg.data;
      can_sts_diag.flag = CAN_FLAG_EMPTY;
    }
    buffer[0] = ui & 0x000000ff;
    ui = ui >> 8;
    buffer[1] = ui & 0x000000ff;
    ui = ui >> 8;
    buffer[2] = ui & 0x000000ff;
    ui = ui >> 8;
    buffer[3] = ui & 0x000000ff;
    nToSend = 4;
    goto ACTION_EXIT;

ACTION_EXIT:
    if (nToSend > 0) {
      buffer[nToSend++] = '\n';
      cnt = 0;
      while (cnt < nToSend) {
        if (!pc.writeable())
          continue;
        pc.putc(buffer[cnt++]);
      }
    }
  }
}

