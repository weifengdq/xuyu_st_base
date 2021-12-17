#ifndef BSP_CAN_H
#define BSP_CAN_H

#include "main.h"
#include "fdcan.h"

extern void can_init(void);
extern void can_send(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *txheader, uint8_t tx_data[], uint8_t len);
extern void can_rx_callback(FDCAN_HandleTypeDef *hfdcan);

extern uint8_t can_dlc2len(uint32_t data_length_code);
extern uint32_t can_len2dlc(uint8_t len);

#endif
