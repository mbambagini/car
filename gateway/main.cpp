#include "mbed.h"
#include "common_types.h"
#include "rtos.h"
#include "can.hpp"
#include "USBHost.h"

DigitalOut myled(LED1);

void init_threads();

int main() {
    init_can();
    //init_android();
    init_threads();
    USBInit();

    while(1) {
        USBLoop();
    }
}

Thread *th_can;
Thread *th_android;

void init_threads() {
    th_can = new Thread(thread_can);
    //th_android = new Thread(thread_android);
}
