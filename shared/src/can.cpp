#include "mbed.h"
#include "net.hpp"
#include "can.hpp"
#include "rtos.h"

/********************** GLOBAL DATA **********************/
//message buffers
can_cmd_body_t can_cmd_body;
can_sts_body_t can_sts_body;
can_cmd_engine_t can_cmd_engine;
can_cmd_time_t can_cmd_time;
can_cmd_diag_t can_cmd_diag;
can_sts_diag_t can_sts_diag;
//can_cmd_camera_t can_cmd_camera;

/********************** LOCAL DATA ***********************/

/*! \brief CAN transceiver
 */
static CAN can(HW_CAN_TX, HW_CAN_RX);

/*! \brief state the messages received and sent
 */
static uint32 can_message_list;

/*! \brief internal counter for consecutive missing messages
 */
static uint8 can_missing[CAN_RX_PERIODIC_MSG];

/*! \brief counter for overall tx errors
 */
static uint32 can_tx_error_counter = 0;

/*** LOCAL FUNCTION PROTOTYPES ***/
/*! \brief read if there are messages in the buffer
 * Read messages are stored within the global buffers
 */
void can_rx();
/*! \brief send pending messages
 */
void can_tx();

/*** GLOBAL FUNCTIONS ***/
uint32 get_can_errors () {
  return can_tx_error_counter;
}

void init_can(uint32 p) {

  can_message_list = p;

  for (int i = 0; i < CAN_RX_PERIODIC_MSG; i++)
    can_missing[i] = CAN_MISSING_DETECTION;
}

void thread_can (void const *args) {
  (void)args;

  while(1) {
    can_rx();
    can_tx();
    osThreadEndPeriod();
//    Thread::wait(CAN_THREAD_PERIOD);
  }
}

bool can_msg_is_missing (uint8 msg_id) {
  if (msg_id < CAN_RX_PERIODIC_MSG)
    return (can_missing[msg_id] == 0);
  return true;
}

/*** LOCAL FUNCTIONS ***/
void can_rx () {
  CANMessage message;

  do {
    if (can.read(message) == 0)
      break;
    switch(message.id) {
      case CAN_CMD_BODY_ID:
        if (can_message_list & NET_RX_CMD_BODY) {
          can_cmd_body.payload.buf[0] = message.data[0];
          can_cmd_body.flag = CAN_FLAG_RECEIVED;
          can_missing[CAN_MISSING_CMD_BODY_ID] = CAN_MISSING_DETECTION+1;
        }
        break;
      case CAN_STS_BODY_ID:
        if (can_message_list & NET_RX_STS_BODY) {
          can_sts_body.payload.buf[0] = message.data[0];
          can_sts_body.payload.buf[1] = message.data[1];
          can_sts_body.payload.buf[2] = message.data[2];
          can_sts_body.payload.buf[3] = message.data[3];
          can_sts_body.flag = CAN_FLAG_RECEIVED;
          can_missing[CAN_MISSING_STS_BODY_ID] = CAN_MISSING_DETECTION+1;
        }
        break;
      case CAN_CMD_ENGINE_ID:
        if (can_message_list & NET_RX_CMD_ENGINE) {
          can_cmd_engine.payload.buf[0] = message.data[0];
          can_cmd_engine.payload.buf[1] = message.data[1];
          can_cmd_engine.payload.buf[2] = message.data[2];
          can_cmd_engine.payload.buf[3] = message.data[3];
          can_cmd_engine.flag = CAN_FLAG_RECEIVED;
          can_missing[CAN_MISSING_CMD_ENGINE_ID] = CAN_MISSING_DETECTION+1;
        }
        break;
      case CAN_CMD_TIME_ID:
        if (can_message_list & NET_RX_CMD_TIME) {
          can_cmd_time.payload.buf[0] = message.data[0];
          can_cmd_time.payload.buf[1] = message.data[1];
          can_cmd_time.payload.buf[2] = message.data[2];
          can_cmd_time.payload.buf[3] = message.data[3];
          can_cmd_time.flag = CAN_FLAG_RECEIVED;
        }
        break;
      case CAN_CMD_DIAG_ID:
        if (can_message_list & NET_RX_CMD_DIAG) {
          can_cmd_diag.payload.buf[0] = message.data[0];
          can_cmd_diag.payload.buf[1] = message.data[1];
          can_cmd_diag.payload.buf[2] = message.data[2];
          can_cmd_diag.payload.buf[3] = message.data[3];
          can_cmd_diag.payload.buf[4] = message.data[4];
          can_cmd_diag.payload.buf[5] = message.data[5];
          can_cmd_diag.payload.buf[6] = message.data[6];
          can_cmd_diag.payload.buf[7] = message.data[7];
          can_cmd_diag.flag = CAN_FLAG_RECEIVED;
        }
        break;
      case CAN_STS_DIAG_ID:
        if (can_message_list & NET_RX_STS_DIAG) {
          can_sts_diag.payload.buf[0] = message.data[0];
          can_sts_diag.payload.buf[1] = message.data[1];
          can_sts_diag.payload.buf[2] = message.data[2];
          can_sts_diag.payload.buf[3] = message.data[3];
          can_sts_diag.flag = CAN_FLAG_RECEIVED;
        }
        break;
/*
#ifdef NET_RX_CMD_CAMERA
      case CAN_CMD_CAMERA_ID:
        can_cmd_camera.payload.buf[0] = message.data[0];
        can_cmd_camera.payload.buf[1] = message.data[1];
        can_cmd_camera.flag = CAN_FLAG_RECEIVED;
        can_missing[CAN_MISSING_CMD_CAMERA_ID] = CAN_MISSING_DETECTION+1;
        break;
#endif
*/
      default:
        break;
    };
  } while(1);

  for (int i = 0; i < CAN_RX_PERIODIC_MSG; i++)
    if (can_missing[i] > 0)
      can_missing[i]--;
}

void can_tx () {
  /** periodic messages */
  if ((can_message_list & NET_TX_CMD_BODY) && (can_cmd_body.flag == 0)) {
    wait(0.002); //wait 2 milliseconds between 2 consecutive transmissions
    if (!can.write(CANMessage(CAN_CMD_BODY_ID, (char*)(&(can_cmd_body.payload.buf)), CAN_CMD_PAYLOAD_BODY)))
      can_tx_error_counter++;
    can_cmd_body.flag = CAN_CMD_BODY_PERIOD;
  }
  can_cmd_body.flag--;

  if ((can_message_list & NET_TX_STS_BODY) && (can_sts_body.flag == 0)) {
    wait(0.002); //wait 2 milliseconds between 2 consecutive transmissions
    if (!can.write(CANMessage(CAN_STS_BODY_ID, (char*)(&(can_sts_body.payload.buf)), CAN_STS_PAYLOAD_BODY)))
      can_tx_error_counter++;
    can_sts_body.flag = CAN_STS_BODY_PERIOD;
  }
  can_sts_body.flag--;

  if ((can_message_list & NET_TX_CMD_ENGINE) && (can_cmd_engine.flag == 0)) {
    wait(0.001); //wait 1 millisecond between 2 consecutive transmissions
    if (!can.write(CANMessage(CAN_CMD_ENGINE_ID, (char*)(&(can_cmd_engine.payload.buf)), CAN_CMD_PAYLOAD_ENGINE)))
      can_tx_error_counter++;
    can_cmd_engine.flag = CAN_CMD_ENGINE_PERIOD;
  }
  can_cmd_engine.flag--;

/*
  if ((can_message_list & NET_TX_CMD_CAMERA) && (can_cmd_camera.flag == 0)) {
    wait(1); //wait 1 millisecond between 2 consecutive transmissions
    if (!can.write(CANMessage(CAN_CMD_CAMERA_ID, (char*)(&(can_cmd_camera.payload.buf)), CAN_CMD_PAYLOAD_CAMERA)))
      can_tx_error_counter++;
    can_cmd_camera.flag = CAN_CMD_CAMERA_PERIOD;
  }
  can_cmd_camera.flag--;
*/

  /** event messages */
  if ((can_message_list & NET_TX_CMD_DIAG) && (can_cmd_diag.flag == CAN_FLAG_SEND)) {
    wait(0.001); //wait 1 millisecond between 2 consecutive transmissions
    if (!can.write(CANMessage(CAN_CMD_DIAG_ID, (char*)(&(can_cmd_diag.payload.buf)), CAN_CMD_PAYLOAD_DIAG)))
      can_tx_error_counter++;
    can_cmd_diag.flag = CAN_FLAG_EMPTY;
  }

  if ((can_message_list & NET_TX_STS_DIAG) && (can_sts_diag.flag == CAN_FLAG_SEND)) {
    wait(0.001); //wait 1 millisecond between 2 consecutive transmissions
    if (!can.write(CANMessage(CAN_STS_DIAG_ID, (char*)(&(can_sts_diag.payload.buf)), CAN_STS_PAYLOAD_DIAG)))
      can_tx_error_counter++;
    can_sts_diag.flag = CAN_FLAG_EMPTY;
  }

  if ((can_message_list & NET_TX_CMD_TIME) && (can_cmd_time.flag == CAN_FLAG_SEND)) {
    wait(1); //wait 1 millisecond between 2 consecutive transmissions
    if (!can.write(CANMessage(CAN_CMD_TIME_ID, (char*)(&(can_cmd_time.payload.buf)), CAN_CMD_PAYLOAD_TIME)))
      can_tx_error_counter++;
    can_cmd_time.flag = CAN_FLAG_EMPTY;
  }
}

