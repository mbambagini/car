#include "mbed.h"
#include "rtos.h"
#include "common_types.h"
#include "net.hpp"
#include "can.hpp"

DigitalOut led1(LED1);
DigitalOut led2(LED2);


#define RECEIVER_PERIOD_MS   100 //milliseconds

#define SENDER_PERIOD_MS     50 //milliseconds

#define BODY_PERIOD_MS       100 //milliseconds
#define BODY_COUNTER_TICK    (BODY_PERIOD_MS/SENDER_PERIOD_MS)

#define ENGINE_PERIOD_MS     50 //milliseconds
#define ENGINE_COUNTER_TICK  (ENGINE_PERIOD_MS/SENDER_PERIOD_MS)

#define DIAG_PERIOD_MS       10000 //milliseconds
#define DIAG_COUNTER_TICK    (DIAG_PERIOD_MS/SENDER_PERIOD_MS)

#define TIME_PERIOD_MS       60000 //milliseconds
#define TIME_COUNTER_TICK    (TIME_PERIOD_MS/SENDER_PERIOD_MS)


void sender(void const* t);
void receiver(void const* t);

unsigned long int COUNTER = 0;

int main() {

  printf("INIT %d %d\r\n", CAN_THREAD_PERIOD, PERIOD_100ms);

  led1 = 0;
  set_time(0);
  uint32 message_mask = NET_TX_CMD_BODY | NET_RX_STS_BODY | 
                        NET_TX_CMD_ENGINE |
                        NET_TX_CMD_DIAG | NET_RX_STS_DIAG | 
                        NET_TX_CMD_TIME;
  init_can(message_mask);
  Thread* th_txer = new Thread(sender,      40, SENDER_PERIOD_MS,   NULL, osPriorityHigh);
  Thread* th_rxer = new Thread(receiver,    25, RECEIVER_PERIOD_MS, NULL, osPriorityHigh);
  Thread* th_can  = new Thread(thread_can,  12, CAN_THREAD_PERIOD,  NULL, osPriorityRealtime);

  printf("RUN\r\n");

  while(1) {};

}

void sender(void const* t) {
  static int body_n_sent = 0;
  static int engine_n_sent = 0;
  static bool engine_direction = false;
  static int diag_n_sent = 0;
  static bool diag_flag = false;
  static int counter = 0;

  while(1) {

    led1 = counter % 2;

    if ((counter % BODY_COUNTER_TICK) == 0) {
      can_cmd_body.payload.msg.light_r = (body_n_sent & 0x0001) == 0x0001;
      can_cmd_body.payload.msg.light_c = (body_n_sent & 0x0002) == 0x0002;
      can_cmd_body.payload.msg.light_l = (body_n_sent & 0x0004) == 0x0004;
      body_n_sent++;
/*
    printf("BODY %d-th (r, l, c) = (%d, %d, %d)\n\r",
           body_n_sent,
           can_cmd_body.payload.msg.light_r,
           can_cmd_body.payload.msg.light_l,
           can_cmd_body.payload.msg.light_c);
*/
    }

    if ((counter % ENGINE_COUNTER_TICK) == 0) {
      if ((engine_n_sent % 100) == 0)
        engine_direction = !engine_direction;
      can_cmd_engine.payload.msg.direction = engine_direction;
      can_cmd_engine.payload.msg.power = engine_n_sent % 100;
      can_cmd_engine.payload.msg.steering = engine_n_sent % 100;
      can_cmd_engine.payload.msg.breaking = (engine_n_sent%1000) >= 900 ? 1 : 0;
/*
    printf("ENGINE %d-th (steering, engine, dir, brake) = (%d, %d, %d, %d)\n\r",
           engine_n_sent,
           can_cmd_engine.payload.msg.steering,
           can_cmd_engine.payload.msg.power,
           can_cmd_engine.payload.msg.direction,
           can_cmd_engine.payload.msg.breaking);
*/
      engine_n_sent++;
    }

    if ((counter % DIAG_COUNTER_TICK) == 0) {
      if (diag_flag)
        can_cmd_diag.payload.msg.cmd = 0x0A0A;
      else
        can_cmd_diag.payload.msg.cmd = 0x0A0B;
      diag_flag = !diag_flag;
      can_cmd_diag.payload.msg.data = diag_n_sent;
      can_cmd_diag.flag = CAN_FLAG_SEND;
      printf("DIAG %d-th (data) = (%d)\n\r", diag_n_sent, diag_n_sent);
      diag_n_sent++;
    }

    if ((counter % TIME_COUNTER_TICK) == 0) {
      can_cmd_time.payload.msg.time = time(NULL);
      can_cmd_time.flag = CAN_FLAG_SEND;
      printf("RESET TIME\n\r");
    }

    counter++;

    osThreadEndPeriod();
  }
}

void receiver (void const* t) {
  static int counter = 0;
  static int hit_left_old = 0;
  static int hit_right_old = 0;
  static int hit_front_old = 0;
  static int hit_rear_old = 0;
  static int light_old = 0;
  static double eye_l = 0.0;
  static double eye_r = 0.0;
  static int eye_counter = 0;

  while(1) {
    led2 = counter % 2;
    counter++;

    if (can_msg_is_missing(CAN_MISSING_STS_BODY_ID)) {
      printf("MISSING BODY\r\n");
    } else {
      if (can_sts_body.flag == CAN_FLAG_RECEIVED) {
        can_sts_body_payload_t x = can_sts_body.payload;
	if (x.msg.hit_front != hit_front_old || x.msg.hit_rear != hit_rear_old || x.msg.hit_left != hit_left_old || x.msg.hit_right != hit_right_old)
           printf("%d %d %d %d\n\r", x.msg.hit_front, x.msg.hit_rear, x.msg.hit_left, x.msg.hit_right);
        hit_front_old = x.msg.hit_front;
        hit_rear_old = x.msg.hit_rear;
        hit_left_old = x.msg.hit_left;
        hit_right_old = x.msg.hit_right;

        if (light_old != x.msg.light_sens)
           printf("light %d\n\r", x.msg.light_sens);
        light_old = x.msg.light_sens;

        eye_l += x.msg.eye_l;
        eye_r += x.msg.eye_r;
        if (++eye_counter == 15) {
          printf("EYEs: %d %d\r\n", eye_l/eye_counter, eye_r/eye_counter);
          eye_l = 0.0;
          eye_r = 0.0;
          eye_counter = 0;
        }

/*
        printf("BODY STS - EYE: %d %d\r\n", x.msg.eye_l, x.msg.eye_r);
        printf("BODY STS - HIT: %d %d %d %d\r\n", x.msg.hit_front, x.msg.hit_rear, x.msg.hit_left, x.msg.hit_right);
        printf("BODY STS - LIGHT: %d\r\n", x.msg.light_sens);
*/
        can_sts_body.flag = CAN_FLAG_EMPTY;
      }
    }

    if (can_sts_diag.flag == CAN_FLAG_RECEIVED) {
      can_sts_diag_payload_t x = can_sts_diag.payload;
      printf("ECHO RX: %d\n\r", x.msg.data);
      can_sts_diag.flag = CAN_FLAG_EMPTY;
    }

    osThreadEndPeriod();
  }
}

