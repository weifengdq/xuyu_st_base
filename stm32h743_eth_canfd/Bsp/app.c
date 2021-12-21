#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "app.h"
#include "bsp_can.h"
#include "bsp_eth.h"

udp_can_control_struct uccs;

void can_rx_callback(FDCAN_HandleTypeDef *hfdcan)
{
    FDCAN_RxHeaderTypeDef RxHeader;

    if(uccs.index > UDP_CAN_PAYLOAD - 72) {
        uccs.over = 1;
        uccs.index_old = uccs.index;
        uccs.index = 0;
        uccs.flag = uccs.flag ? 0 : 1;
    }

    uccs.refresh = 1;

    //valid data
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, &(uccs.uc[uccs.flag].body[uccs.index+8]));

    //std or ext, id
    memcpy(&(uccs.uc[uccs.flag].body[uccs.index]), &(RxHeader.Identifier), sizeof(RxHeader.Identifier));
    if(RxHeader.IdType == FDCAN_EXTENDED_ID) {  //extended frame
        uccs.uc[uccs.flag].body[uccs.index+3] |= 0x80;
    }

    //len
    uccs.uc[uccs.flag].body[uccs.index+4] = can_dlc2len(RxHeader.DataLength);

    //brs
    if(RxHeader.BitRateSwitch == FDCAN_BRS_ON) {
        uccs.uc[uccs.flag].body[uccs.index+5] |= 0x01;
    }

    //esi
    if(RxHeader.ErrorStateIndicator == FDCAN_ESI_PASSIVE) {
        uccs.uc[uccs.flag].body[uccs.index+5] |= 0x02;
    }

    //fd
    if(RxHeader.FDFormat == FDCAN_FD_CAN) {
        uccs.uc[uccs.flag].body[uccs.index+5] |= 0x04;
        //uccs.uc[uccs.flag].head[15] = 0x0d; //canfd
    } else {
        //uccs.uc[uccs.flag].head[15] = 0x0c; //can
    }



    //uccs.index+6 as from which can
    if (hfdcan->Instance == FDCAN1) {
        uccs.uc[uccs.flag].body[uccs.index+6] = 0x0;
    } else if (hfdcan->Instance == FDCAN2) {
        uccs.uc[uccs.flag].body[uccs.index+6] = 0x1;
    }

    uccs.uc[uccs.flag].body[uccs.index+7] = uccs.rollingcnt & 0xFF; //rollingcnt
    uccs.rollingcnt += 1;

    uccs.index = uccs.index + 8 + uccs.uc[uccs.flag].body[uccs.index+4];

}


void multicast_rx_callback(void *arg, struct udp_pcb *upcb,struct pbuf *p, ip_addr_t *addr, u16_t port)
{
    if(p!= NULL) {
        uint8_t *ptr;
        uint16_t len;
        uint16_t i;
        FDCAN_TxHeaderTypeDef TxHeader;

        ptr = p->payload;
        len = p->len;

        for(i=48; i<len;) {
            TxHeader.Identifier = (*(uint32_t *)&ptr[i]) & 0x1FFFFFFF;
            TxHeader.IdType = ((*(uint32_t *)&ptr[i])>>31) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
            TxHeader.TxFrameType = FDCAN_DATA_FRAME;
            TxHeader.DataLength =  can_len2dlc(ptr[i+4]); //ptr[i+4] << 16;   // data_length_code
            TxHeader.BitRateSwitch = (ptr[i+5] & 0x01) ? FDCAN_BRS_ON : FDCAN_BRS_OFF;
            TxHeader.ErrorStateIndicator = ((ptr[i+5]>>1) & 0x01) ? FDCAN_ESI_PASSIVE : FDCAN_ESI_ACTIVE;
            TxHeader.FDFormat = ((ptr[i+5]>>2) & 0x01) ? FDCAN_FD_CAN : FDCAN_CLASSIC_CAN;
            TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
            TxHeader.MessageMarker = 0;
            switch (ptr[i+6]) {
                case 0:
                    if(__HAL_FDCAN_GET_FLAG(&hfdcan1, FDCAN_FLAG_BUS_OFF) != SET) {
                        HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, &ptr[i+8]);
                    }
                    break;
                case 1:
                    if(__HAL_FDCAN_GET_FLAG(&hfdcan2, FDCAN_FLAG_BUS_OFF) != SET) {
                        HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, &ptr[i+8]);
                    }
                    break;
                //default:
            }

            i = i + 8 + ptr[i+4];
        }
    }
}

void app_init(void)
{
    uccs.btime = HAL_GetTick();
}

void app_process(void)
{
    static uint32_t cnt = 0;
    static uint32_t max_cnt = 0xFFFFFFFF;

    cnt++;

    //overflow send
    if( (cnt % (max_cnt/50) == 0) &&
        (uccs.over) ) {   //200us

        uint8_t i = uccs.flag ? 0 : 1;
        uccs.uc[i].head[8] = 0x00;  //device identifier, 00-mcu, 01-pc
        multicast_send((uint8_t *)(&(uccs.uc[i])), (32 + 16 + uccs.index_old));
        memset(&(uccs.uc[i]), 0, sizeof(udp_can_struct));
        uccs.over = 0;

    }

    // static uint8_t send_flag = 0;

    //__disable_irq(); 

    // if(uccs.refresh && (send_flag==0)) {
    //     uccs.btime = HAL_GetTick();
    //     send_flag = 1;
    // }

    // //overtime send
    // if( (HAL_GetTick() - uccs.btime >= 10) &&
    //     (uccs.refresh) && send_flag ) {  //10ms

    if( (HAL_GetTick() - uccs.btime >= 10) &&
        (uccs.refresh) ) {  //10ms
        //send_flag = 0;
        uccs.refresh = 0;
        uccs.index_old = uccs.index;
        uccs.index = 0;
        uccs.flag = uccs.flag ? 0 : 1;
        uint8_t i = uccs.flag ? 0 : 1;
        uccs.btime = HAL_GetTick();
        max_cnt = cnt;
        cnt=0;
        uccs.uc[i].head[8] = 0x00;  //device identifier, 00-mcu, 01-pc
        multicast_send((uint8_t *)(&(uccs.uc[i])), (32 + 16 + uccs.index_old));
        memset(&(uccs.uc[i]), 0, sizeof(udp_can_struct));
        //printf("alive\r\n");

    }

    //__enable_irq();
}
