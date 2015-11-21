#ifndef __CAN_H__
#define __CAN_H__

#include "common_types.h"

#define HW_CAN_TX                    p9
#define HW_CAN_RX                    p10

//CAN driver initialization
void init_can();

//thread of the CAN driver which receives and transmits messages periodically
void thread_can(void const *args);

//return if a message hasn't been received for longer than
//CAN_MISSING_DETECTION
bool can_msg_is_missing(uint8 msg_id);

#endif //__CAN_H__
