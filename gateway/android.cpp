#include "android.hpp"
#include "AndroidGateway.hpp"
#include "common_types.h"
#include "net.hpp"
#include "can.hpp"
#include "rtos.h"

static AndroidGateway gateway;

void android_init() {
    gateway.setupDevice();
}

#define MAX_TX_BUFFER (2 + CAN_STS_PAYLOAD_BODY + CAN_STS_PAYLOAD_DIAG)
#define MAX_RX_BUFFER (1 + CAN_CMD_PAYLOAD_BODY + CAN_CMD_PAYLOAD_ENGINE + CAN_CMD_PAYLOAD_TIME + CAN_CMD_PAYLOAD_CAMERA + CAN_CMD_PAYLOAD_DIAG)

void android_thread() {
  while (1) {
    //send data to android device
    if (!gateway.isWritePending()) {
      unsigned char buffer_tx[MAX_TX_BUFFER];
      memset(buffer_tx, 0, MAX_RX_BUFFER);
      int size = 2;
      //present
      if (can_sts_body.flag == CAN_FLAG_RECEIVED) {
        buffer_tx[0] = buffer_tx[0] | 0x01;
        memcpy(&buffer_tx[size], &can_sts_body.payload.msg, CAN_STS_PAYLOAD_BODY);
        size += CAN_STS_PAYLOAD_BODY;
      }
      if (can_sts_diag.flag == CAN_FLAG_RECEIVED) {
        buffer_tx[0] = buffer_tx[0] | 0x02;
        memcpy(&buffer_tx[size], &can_sts_diag.payload.msg, CAN_STS_PAYLOAD_DIAG);
        size += CAN_STS_PAYLOAD_DIAG;
      }
      //missing
      if (can_msg_is_missing(CAN_MISSING_STS_BODY_ID))
        buffer_tx[1] = buffer_tx[1] | 0x01;
      if (buffer_tx[0] != 0 || buffer_tx[1] != 0)
        gateway.send(buffer_tx, size);
    }

    //read data from android device
    if (gateway.isReadAvailable()) {
      unsigned char buffer_rx[MAX_RX_BUFFER];
      int num = gateway.receive(buffer_rx, MAX_RX_BUFFER);
      int size = 1;
      if (num > 2) {
        if (buffer_rx[0] & 0x01) { //CMD_BODY - periodic
          memcpy(&can_cmd_body.payload.msg, &buffer_rx[size], CAN_CMD_PAYLOAD_BODY);
          size += CAN_CMD_PAYLOAD_BODY;
        }
        if (buffer_rx[0] & 0x02) { //CMD_ENGINE - periodic
          memcpy(&can_cmd_engine.payload.msg, &buffer_rx[size], CAN_CMD_PAYLOAD_ENGINE);
          size += CAN_CMD_PAYLOAD_ENGINE;
        }
        if (buffer_rx[0] & 0x04) { //CMD_CAMERA - periodic
          memcpy(&can_cmd_camera.payload.msg, &buffer_rx[size], CAN_CMD_PAYLOAD_CAMERA);
          size += CAN_CMD_PAYLOAD_CAMERA;
        }
        if (buffer_rx[0] & 0x08) { //CMD_TIME - aperiodic
          memcpy(&can_cmd_time.payload.msg, &buffer_rx[size], CAN_CMD_PAYLOAD_TIME);
          can_cmd_time.flag = CAN_FLAG_SEND;
          size += CAN_CMD_PAYLOAD_TIME;
        }
        if (buffer_rx[0] & 0x10) { //CMD_DIAG - aperiodic
          memcpy(&can_cmd_diag.payload.msg, &buffer_rx[size], CAN_CMD_PAYLOAD_DIAG);
          can_cmd_diag.flag = CAN_FLAG_SEND;
          size += CAN_CMD_PAYLOAD_DIAG;
        }
      }
    }
    Thread::wait(ANDROID_THREAD_PERIOD);
  }
}

