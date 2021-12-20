#ifndef APP_H
#define APP_H

#include "main.h"

#define UDP_CAN_PAYLOAD 1152

typedef struct
{
    uint8_t someip_header[32];
    uint8_t head[16];
    uint8_t body[UDP_CAN_PAYLOAD];
} udp_can_struct;   //1200 bytes max

typedef struct
{
    udp_can_struct uc[2];

    //rx control
    uint8_t refresh;    //indicate for new frame
    uint8_t flag;   //ping0-pong1
    uint8_t over;   //overflow
    uint16_t index; //uc.body index
    uint16_t index_old;
    uint32_t btime; //begin time
    uint32_t rollingcnt;

    //tx control
} udp_can_control_struct;

extern udp_can_control_struct uccs;

extern void app_init(void);
extern void app_process(void);

#endif // !APP_H
