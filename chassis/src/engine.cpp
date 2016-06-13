#include "Servo.h"
#include "common_types.h"
#include "engine.hpp"
#include "net.hpp"
#include "can.hpp"
#include "mbed.h"
#include "rtos.h"

/*** DEFINES ***/
//maximum time interval (in milliseconds) during which the
//engine can be blocked
#define MAX_BREAKING_INTERVAL (1000/ENGINE_THREAD_PERIOD) 

/*** LOCAL DATA ***/
//steering servo motor
static Servo steering_servo(HW_STEERING_SERVO);
//engine
static PwmOut engine_power(HW_ENGINE_ENABLER);
static DigitalOut dir1(HW_ENGINE_DIR_1);
static DigitalOut dir2(HW_ENGINE_DIR_2);

/*** LOCAL FUNCTION PROTOTYPES ***/
//stop the engine (without breaking) and set the steering
//servo at 0°
void stop_engine();

/*** GLOBAL FUNCTIONS ***/
void init_engine() {
  steering_servo.calibrate(HW_SERVO_RANGE_INIT, HW_SERVO_ANGLE_INIT);
  engine_power.period(HW_ENGINE_PERIOD); //100ms
  engine_power = 0; //off    
}

void thread_engine (void const *args) {
  (void*)args;

  static uint8 prev_breaking = 0; //if it was breaking the previous step
  static uint8 breaking_interval = 0; //how long it has been breaking

  while(1) {
    if (can_msg_is_missing(CAN_MISSING_CMD_ENGINE_ID)) {
      // no message received for a while: stop everything
      stop_engine();
    } else {
      //if a valid message has been received
      if (can_cmd_engine.flag == CAN_FLAG_RECEIVED) {
#ifdef DEBUG
        printf("ENGINE: %d %d %d %d\r\n", can_cmd_engine.payload.msg.steering, 
               can_cmd_engine.payload.msg.power, can_cmd_engine.payload.msg.breaking,
               can_cmd_engine.payload.msg.direction);
#endif
        //set steering angle
        uint8 tmp = can_cmd_engine.payload.msg.steering;
        steering_servo =  tmp >= 100 ? 1.0 : (float)tmp/100.0;

        //if it is breaking
        if (can_cmd_engine.payload.msg.breaking) {
          //it was not breaking during the previous step, so start breaking
          if (prev_breaking == 0) {
            breaking_interval = 0;
            prev_breaking = 1;
          } else if (breaking_interval < MAX_BREAKING_INTERVAL) {
            //still breaking
            dir1 = 1;
            dir2 = 1;
            breaking_interval++;
          } else {
            //time expired. stop breaking (too much current wasted)
            dir1 = 1;
            dir2 = 0;
            engine_power = 0;
            // do not set prev_breaking==0, otherwise it starts breaking again
          }
        } else {
          //normal execution (not breaking)
          //deactivate breaking
          prev_breaking = 0;
          //set direction
          if (can_cmd_engine.payload.msg.direction) {
            dir1 = 1;
            dir2 = 0;
          } else {
            dir1 = 0;
            dir2 = 1;
          }
          //set engine power
          tmp = can_cmd_engine.payload.msg.power;
          engine_power = tmp >= 100 ? 1.0 : (float)tmp/100.0;
        }
        //set the message as read
        can_cmd_engine.flag = CAN_FLAG_EMPTY;
      }
    }
//    Thread::wait(ENGINE_THREAD_PERIOD);
    osThreadEndPeriod();
  }
}

/*** LOCAL FUNCTIONS ***/

void stop_engine ()
{
  engine_power = 0; //off
  steering_servo = 0; //center
  dir1 = 1;
  dir2 = 0;
}

