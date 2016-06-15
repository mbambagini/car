/*! \file body.cpp
 *  \brief BCM implementation
 *
 * this file implements the BCM component which is in charge of:
 * - lighting system
 * - distance sensors
 * - hit detection
 */

#include "common_types.h"
#include "body.hpp"
#include "net.hpp"
#include "can.hpp"
#include "mbed.h"
#include "rtos.h"

/*** LOCAL DEFINES ***/

//configuration of the GP2D12 device
#define k_5 12466.0
#define k_4 -23216.0
#define k_3 14974.0
#define k_2 -3585.0
#define k_1 19.0
#define k_0 96.0

/*** LOCAL DATA ***/

static AnalogIn back_r(HW_RIGHT_EYE);
static AnalogIn back_l(HW_LEFT_EYE);
static DigitalIn hit_front(HW_HIT_FRONT);
static DigitalIn hit_rear(HW_HIT_REAR);
static DigitalIn hit_left(HW_HIT_LEFT);
static DigitalIn hit_right(HW_HIT_RIGHT);
static DigitalIn photo(HW_PHOTO);
static DigitalOut light_l(HW_LIGHT_L);
static DigitalOut light_c(HW_LIGHT_C);
static DigitalOut light_r(HW_LIGHT_R);

/*** LOCAL FUNCTION PROTOTYPES ***/

/*! \brief read from the analog input GP2D12
 */
uint8 read_distance (AnalogIn* side);

/*! \brief handle of error condition: switch all hardware off
 */
void stop_body() {
  light_l = 0;
  light_c = 0;
  light_r = 0;
}

/*** GLOBAL FUNTIONS ***/

void init_body() {
  // nothing to do
}

void thread_body (void const *args) {
  while(1) {
    if (can_msg_is_missing(CAN_MISSING_CMD_BODY_ID))
      stop_body();
    else {
      //if a message has been received, set lights
      if (can_cmd_body.flag == CAN_FLAG_RECEIVED) {
        light_r = can_cmd_body.payload.msg.light_r;
        light_l = can_cmd_body.payload.msg.light_l;
        light_c = can_cmd_body.payload.msg.light_c;
        can_cmd_body.flag = CAN_FLAG_EMPTY;
      }
    }
    //send sensors' values
    can_sts_body.payload.msg.eye_l = read_distance(&back_l);
    can_sts_body.payload.msg.eye_r = read_distance(&back_r);
    can_sts_body.payload.msg.hit_front = hit_front.read();
    can_sts_body.payload.msg.hit_rear = hit_rear.read();
    can_sts_body.payload.msg.hit_left = hit_left.read();
    can_sts_body.payload.msg.hit_right = hit_right.read();
    can_sts_body.payload.msg.light_sens = photo.read();
#ifdef DEBUG
    printf("HIT: %d %d %d %d\n\r", hit_front.read(), hit_rear.read(), hit_left.read(), hit_right.read());
    printf("EYE: %d %d\n\r", can_sts_body.payload.msg.eye_l,
                             can_sts_body.payload.msg.eye_r);
#endif
//    Thread::wait(BODY_THREAD_PERIOD);
    osThreadEndPeriod();
  }
}

/*** LOCAL FUNCTIONS ***/

uint8 read_distance (AnalogIn* side) {
  float x = side->read();
  float res = 0.0;
  res += k_5 * (x * x * x * x * x);
  res += k_4 * (x * x * x * x);
  res += k_3 * (x * x * x);
  res += k_2 * (x * x);
  res += k_1 *  x;
  res += k_0;
  if (res < 0.0)
    return 0;
  if (res > 255.0)
    return res;
  return (uint8)res;
}

