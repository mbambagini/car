#ifndef __NET_H__
#define __NET_H__

#include "common_types.h"

#define CAN_FLAG_EMPTY          0x00
#define CAN_FLAG_RECEIVED       0x01
#define CAN_FLAG_SEND           0x02

/**************************************************
 * DIAG CODEs
 **************************************************/
#define CMD_ECHO_BCM            0x0A0A
#define CMD_ECHO_ECM            0x0A0B
#define CMD_TIME_ALL            0x0B0B

/**************************************************
 * MESSAGE IDs
 **************************************************/

#define CAN_CMD_BODY_ID         0xAA
#define CAN_STS_BODY_ID         0xAB
#define CAN_CMD_ENGINE_ID       0x0A
#define CAN_CMD_TIME_ID         0xBB
#define CAN_CMD_DIAG_ID         0x11
#define CAN_STS_DIAG_ID         0x10

/**************************************************
 * COMMANDS
 **************************************************/

#define NET_TX_CMD_BODY         0x0001
#define NET_RX_CMD_BODY         0x0002
#define NET_TX_STS_BODY         0x0004
#define NET_RX_STS_BODY         0x0008
#define NET_TX_CMD_ENGINE       0x0010
#define NET_RX_CMD_ENGINE       0x0020
#define NET_TX_CMD_DIAG         0x0040
#define NET_RX_CMD_DIAG         0x0080
#define NET_TX_STS_DIAG         0x0100
#define NET_RX_STS_DIAG         0x0200
#define NET_TX_CMD_TIME         0x0400
#define NET_RX_CMD_TIME         0x0800


/**************************************************
 * MESSAGE PERIODs
 **************************************************/

#define PERIOD_60s              (60000/CAN_THREAD_PERIOD)
#define PERIOD_3s               (3000/CAN_THREAD_PERIOD)
#define PERIOD_1s               (1000/CAN_THREAD_PERIOD)
#define PERIOD_500ms            (500/CAN_THREAD_PERIOD)
#define PERIOD_400ms            (400/CAN_THREAD_PERIOD)
#define PERIOD_300ms            (300/CAN_THREAD_PERIOD)
#define PERIOD_200ms            (200/CAN_THREAD_PERIOD)
#define PERIOD_100ms            (100/CAN_THREAD_PERIOD)

#define CAN_CMD_BODY_PERIOD     PERIOD_100ms
#define CAN_STS_BODY_PERIOD     PERIOD_100ms
#define CAN_CMD_ENGINE_PERIOD   PERIOD_100ms

#define CAN_MISSING_DETECTION   PERIOD_3s

#define CAN_MISSING_CMD_BODY_ID     0
#define CAN_MISSING_CMD_ENGINE_ID   1
#define CAN_MISSING_STS_BODY_ID     2

#define CAN_RX_PERIODIC_MSG         3

/**************************************************
 * MESSAGE SIZEs
 **************************************************/

#define CAN_CMD_PAYLOAD_BODY    1
#define CAN_STS_PAYLOAD_BODY    4
#define CAN_CMD_PAYLOAD_ENGINE  4
#define CAN_CMD_PAYLOAD_TIME    4
#define CAN_CMD_PAYLOAD_DIAG    8
#define CAN_STS_PAYLOAD_DIAG    4

/**************************************************
 * MESSAGE TYPEs
 **************************************************/
typedef union can_cmd_body_payload_s {
    uint8 buf[CAN_CMD_PAYLOAD_BODY];
    struct {
        uint32 light_r:1;
        uint32 light_c:1;
        uint32 light_l:1;
        uint32 unused:5;
    } msg;
} can_cmd_body_payload_t;
typedef struct can_cmd_body_s {
    can_cmd_body_payload_t payload;
    uint8 flag;
} can_cmd_body_t;

typedef union can_sts_body_payload_s {
    uint8 buf[CAN_STS_PAYLOAD_BODY];
    struct {
        uint32 hit_front:1;
        uint32 hit_rear:1;
        uint32 hit_left:1;
        uint32 hit_right:1;
        uint32 light_sens:1;
        uint32 unused:3;
        uint32 eye_l:8;
        uint32 eye_r:8;
        uint32 unused_:8;
    } msg;
} can_sts_body_payload_t;
typedef struct can_sts_body_s {
    can_sts_body_payload_t payload;
    uint8 flag;
} can_sts_body_t;

typedef union can_cmd_engine_payload_s {
    uint8 buf[CAN_CMD_PAYLOAD_ENGINE];
    struct {
        uint32 steering:8;
        uint32 power:8;
        uint32 direction:1;
        uint32 breaking:1;
        uint32 unused:14;
    } msg;
} can_cmd_engine_payload_t;
typedef struct can_cmd_engine_s {
    can_cmd_engine_payload_t payload;
    uint8 flag;
} can_cmd_engine_t;
typedef union can_cmd_time_payload_s {
    uint8 buf[CAN_CMD_PAYLOAD_TIME];
    struct {
        uint32 time;
    } msg;
} can_cmd_time_payload_t;
typedef struct can_cmd_time_s {
    can_cmd_time_payload_t payload;
    uint8 flag;
} can_cmd_time_t;
typedef union can_cmd_diag_payload_s {
    uint8 buf[CAN_CMD_PAYLOAD_DIAG];
    struct {
        uint32 cmd:16;
        uint32 opt:16;
        uint32 data;
    } msg;
} can_cmd_diag_payload_t;
typedef struct can_cmd_diag_s {
    can_cmd_diag_payload_t payload;
    uint8 flag;
} can_cmd_diag_t;

typedef union can_sts_diag_payload_s {
    uint8 buf[CAN_STS_PAYLOAD_DIAG];
    struct {
        uint32 data;
    } msg;
} can_sts_diag_payload_t;
typedef struct can_sts_diag_time_s {
    can_sts_diag_payload_t payload;
    uint8 flag;
} can_sts_diag_t;

/*
typedef union can_cmd_camera_payload_s {
    uint8 buf[CAN_CMD_PAYLOAD_CAMERA];
    struct {
        uint16 x:7;
        uint16 y:7;
        uint16 mode:2;
    } msg;
} can_cmd_camera_payload_t;
typedef struct can_cmd_camera_s {
    can_cmd_camera_payload_t payload;
    uint8 flag;
} can_cmd_camera_t;
*/

/**************************************************
 * MESSAGE BUFFERs
 **************************************************/

extern can_cmd_body_t can_cmd_body;
extern can_sts_body_t can_sts_body;
extern can_cmd_engine_t can_cmd_engine;
extern can_cmd_time_t can_cmd_time;
extern can_cmd_diag_t can_cmd_diag;
extern can_sts_diag_t can_sts_diag;
//extern can_cmd_camera_t can_cmd_camera;

#endif //__NET_H__

