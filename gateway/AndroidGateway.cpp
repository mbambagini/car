#include "AndroidGateway.hpp"

void AndroidGateway::setupDevice() {
  resetDevice();
}

void AndroidGateway::resetDevice() {
  memset(buffer, 0, RX_BUFFER_SIZE);
  rx_index = 0;
  internal_state = ANDROID_STATE_IDLE;
}

int AndroidGateway::callbackRead(u8 *buf, int len) {
  if (len > RX_BUFFER_SIZE) {
    rx_index = 0;
    return 0;
  }
  memcpy(buffer, buf, len);
  rx_index = len;
  return 0;
}
 
int AndroidGateway::callbackWrite() {
  internal_state = ANDROID_STATE_IDLE;
  return 0;
}

int AndroidGateway::send(u8 *buff, int len) {
  internal_state = ANDROID_STATE_WAITING;
  return write(buff, len);
}

int AndroidGateway::receive(u8 *buff, int len) {
  int size = rx_index;
  if (rx_index == 0 || len < rx_index)
    return 0;
  memcpy(buff, buffer, size);
  rx_index = 0;
  return size;
}
