#include "mbed.h"
#include "common_types.h"
#include "net.hpp"
#include "can.hpp"
 
Ticker ticker;

int counter = 0;

#define SEND_PERIOD_MS     200 //milliseconds

#define BODY_TX            (1000/SEND_PERIOD_MS)
#define ENGINE_TX          (400/SEND_PERIOD_MS)
#define DIAG_TX            (5000/SEND_PERIOD_MS)
#define TIME_TX            (60000/SEND_PERIOD_MS)

void send() {
  static int body_n_sent = 0;
  static int engine_n_sent = 0;
  static bool engine_direction = false;
  static int diag_n_sent = 0;
  static bool diag_flag = false;

  if ((counter % BODY_TX) == 0) {
    can_cmd_body.payload.msg.light_r = (body_n_sent & 0x0001) == 0x0001;
    can_cmd_body.payload.msg.light_l = (body_n_sent & 0x0002) == 0x0002;
    can_cmd_body.payload.msg.light_c = (body_n_sent & 0x0004) == 0x0004;
    body_n_sent++;
    printf("BODY %d-th (r, l, c) = (%d, %d, %d)\n\r",
           body_n_sent,
           can_cmd_body.payload.msg.light_r,
           can_cmd_body.payload.msg.light_l,
           can_cmd_body.payload.msg.light_c);
  }

  if ((counter % ENGINE_TX) == 0) {
    if ((engine_n_sent % 100) == 0)
      engine_direction = !engine_direction;
    can_cmd_engine.payload.msg.steering = engine_n_sent % 100;
    can_cmd_engine.payload.msg.power = engine_n_sent % 100;
    can_cmd_engine.payload.msg.direction = engine_direction;
    can_cmd_engine.payload.msg.breaking = (engine_n_sent > 200 &&
                                           engine_n_sent < 300) ? 1 : 0;
    printf("ENGINE %d-th (steering, engine, dir, brake) = (%d, %d, %d, %d)\n\r",
           engine_n_sent,
           can_cmd_engine.payload.msg.steering,
           can_cmd_engine.payload.msg.power,
           can_cmd_engine.payload.msg.direction,
           can_cmd_engine.payload.msg.breaking);
    engine_n_sent++;
  }

  if ((counter % DIAG_TX) == 0) {
    if (diag_flag)
      can_cmd_diag.payload.msg.cmd = 0x0A0A;
    else
      can_cmd_diag.payload.msg.cmd = 0x0A0B;
    diag_flag = ~diag_flag;
    can_cmd_diag.payload.msg.data = diag_n_sent;
    printf("DIAG %d-th (data) = (%d)\n\r", diag_n_sent, diag_n_sent);
    can_cmd_diag.flag = CAN_FLAG_SEND;
    diag_n_sent++;
  }
    
  if ((counter % TIME_TX) == 0) {
    can_cmd_time.payload.msg.time = time(NULL);
    can_cmd_time.flag = CAN_FLAG_SEND;
    printf("RESET TIME");
  }

  if (!can_msg_is_missing(CAN_MISSING_STS_BODY_ID) && can_sts_body.flag == CAN_FLAG_RECEIVED) {
    can_sts_body_payload_t x = can_sts_body.payload;
    printf("BODY STS - EYE: %d %d %d\r\n", x.msg.eye_back_l, x.msg.eye_back_r, x.msg.eye_front);
    printf("BODY STS - HIT: %d %d %d %d\r\n", x.msg.hit_front, x.msg.hit_rear, x.msg.hit_left, x.msg.hit_right);
    printf("BODY STS - LIGHT: %d\r\n", x.msg.light_sens);
    can_sts_body.flag = CAN_FLAG_EMPTY;
  }

  if (can_sts_diag.flag == CAN_FLAG_RECEIVED) {
    can_sts_diag_payload_t x = can_sts_diag.payload;
    printf("ECHO RX: %d\n\r", x.msg.data);
    can_sts_diag.flag = CAN_FLAG_EMPTY;
  }

  counter++;
}


int main() {

  printf("INIT\r\n");
  set_time(0);
  ticker.attach(&send, 0.2);

  while(1) {};

}

