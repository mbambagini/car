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

static AnalogIn back_r(HW_REAR_RIGHT_EYE);
static AnalogIn back_l(HW_REAR_LEFT_EYE);
static I2C i2c(HW_FRONT_EYE_RX, HW_FRONT_EYE_TX);
static DigitalIn hit_front(HW_HIT_FRONT);
static DigitalIn hit_rear(HW_HIT_REAR);
static DigitalIn hit_left(HW_HIT_LEFT);
static DigitalIn hit_right(HW_HIT_RIGHT);

/*** LOCAL FUNCTION PROTOTYPES ***/

//read from the analog input GP2D12
uint8 read_back_distance (AnalogIn* side);

//initialize the SRF10
void srf10_start (char addr);

//change the address of the SRF10
void srf10_change_address (char actual_addr, char new_addr);

//read from the SRF10
uint16 srf10_read (char addr);

//handle of error condition: switch all hardware off
void stop_body() {
}

/*** GLOBAL FUNTIONS ***/

void init_body() {
  srf10_start(HW_FRONT_EYE_DEFAULT_ADDR);
  srf10_change_address(HW_FRONT_EYE_DEFAULT_ADDR, HW_FRONT_EYE_ADDR);
}

void thread_body (void const *args) {
  while(1) {
    if (can_msg_is_missing(CAN_MISSING_CMD_BODY_ID))
      stop_body();
    else {
      //if a message has been received, set lights
      if (can_cmd_body.flag == CAN_FLAG_RECEIVED) {
        //int r = can_cmd_body.payload.msg.light_r;
        //int l = can_cmd_body.payload.msg.light_l;
        //int c = can_cmd_body.payload.msg.light_c;
        can_cmd_body.flag = CAN_FLAG_EMPTY;
      }
    }
    //send sensors' values
    can_sts_body.payload.msg.eye_back_l = read_back_distance(&back_l);
    can_sts_body.payload.msg.eye_back_r = read_back_distance(&back_r);
    can_sts_body.payload.msg.eye_front = srf10_read(HW_FRONT_EYE_ADDR);
    can_sts_body.payload.msg.hit_front = hit_front.read();
    can_sts_body.payload.msg.hit_rear = hit_rear.read();
    can_sts_body.payload.msg.hit_left = hit_left.read();
    can_sts_body.payload.msg.hit_right = hit_right.read();
/*
    printf("HIT: %d %d %d %d\n\r", hit_front.read(),
                                   hit_rear.read(),
                                   hit_left.read(),
                                   hit_right.read());
    printf("EYE: %d %d %d\n\r", can_sts_body.payload.msg.eye_back_l,
                                can_sts_body.payload.msg.eye_back_r, 
                                can_sts_body.payload.msg.eye_front);
*/
    Thread::wait(BODY_THREAD_PERIOD);
  }
}

/*** LOCAL FUNCTIONS ***/

uint8 read_back_distance (AnalogIn* side) {
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

void srf10_start (char addr) {
  char cmd[2];

  //set range: register x 43mm + 43mm
  cmd[0] = 0x02;
  cmd[1] = 0x30; //register = 48*43mm+43mm=2107mm
  i2c.write(addr, cmd, 2);

  cmd[0] = 0x01; //receiver gain register
  cmd[1] = 0x10; //maximum gain
  i2c.write(addr, cmd, 2);
}

void srf10_change_address (char addr, char new_addr) {
  char cmd[2];

  cmd[0] = 0x00;
  cmd[1] = 0xA0;
  i2c.write(addr, cmd, 2);
  cmd[0] = 0x00;
  cmd[1] = 0xAA;
  i2c.write(addr, cmd, 2);
  cmd[0] = 0x00;
  cmd[1] = 0xA5;
  i2c.write(addr, cmd, 2);
  cmd[0] = 0x00;
  cmd[1] = new_addr;
  i2c.write(addr, cmd, 2);
}

uint16 srf10_read (char addr) {
  char cmd[2];
  char echo[2];

  cmd[0] = 0x00; //Command register
  cmd[1] = 0x51; //Ranging results in cm
  i2c.write(addr, cmd, 2); // Send ranging burst

  wait(0.07); // Wait for return echo

  cmd[0] = 0x02; //Address of first echo
  i2c.write(addr, cmd, 1, 1); //Send address of first echo
  i2c.read(addr, echo, 2); //Read two-byte echo result

  return (echo[0]<<8)+echo[1];
}
