#ifndef __ANDROID_GATEWAY_HPP__
#define __ANDROID_GATEWAY_HPP__

#include "AndroidAccessory.h"

#define RX_BUFFER_SIZE          100
#define TX_BUFFER_SIZE          100

#define ANDROID_MANUFACTOR      "myself"
#define ANDROID_MODEL           "gateway"
#define ANDROID_DESCRIPTION     "gateway device"
#define ANDROID_VERSION         "1.0"
#define ANDROID_URI             "http://www.mbed.org"
#define ANDROID_SERIAL          "0000000011111111"

#define ANDROID_STATE_IDLE      0
#define ANDROID_STATE_WAITING   1

class AndroidGateway : public AndroidAccessory {
 public:
  AndroidGateway() : AndroidAccessory(RX_BUFFER_SIZE, //rx buffer size
                                      TX_BUFFER_SIZE, //tx buffer size
                                      ANDROID_MANUFACTOR,
                                      ANDROID_MODEL,
                                      ANDROID_DESCRIPTION,
                                      ANDROID_VERSION,
                                      ANDROID_URI,
                                      ANDROID_SERIAL) {};

  virtual void setupDevice();
  virtual void resetDevice();
  virtual int callbackRead(u8 *buff, int len);
  virtual int callbackWrite();

  int isReadAvailable () {
      return (rx_index > 0);
  }

  int isWritePending () {
      return (internal_state == ANDROID_STATE_WAITING);
  }

  int receive(u8 *buff, int len);
  int send(u8 *buff, int len);

 private:
  int internal_state;
  char buffer[RX_BUFFER_SIZE];
  int rx_index;
};

#endif //__ANDROID_GATEWAY_HPP__
