#include "mbed.h"
#include "led.hpp"
#include "common_types.h"

static Ticker alive_ticker;
static DigitalOut alive_led(HW_ALIVE_LED);

void blink_led()
{
  alive_led = !alive_led;
}

void init_led ()
{
  //alive blinking led - 1Hz
  alive_led = 0;
  alive_ticker.attach(blink_led, 1.0);
}
