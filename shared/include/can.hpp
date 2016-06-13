#ifndef __CAN_H__
#define __CAN_H__

#include "common_types.h"

#define HW_CAN_TX                    p9
#define HW_CAN_RX                    p10

//CAN driver initialization
//p contains the mask of supported messages
void init_can(uint32 p);

//thread of the CAN driver which receives and transmits messages periodically
void thread_can(void const *args);

//return if a message hasn't been received for longer than
//CAN_MISSING_DETECTION
bool can_msg_is_missing(uint8 msg_id);

//return the total number of errors happened while transmitting messages
//on the CAN bus
uint32 get_can_errors ();

#endif //__CAN_H__
