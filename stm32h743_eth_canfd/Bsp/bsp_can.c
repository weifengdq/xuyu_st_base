#include "bsp_can.h"
#include "main.h"
#include "fdcan.h"

void can1_config(void)
{
    FDCAN_FilterTypeDef sFilterConfig1;

    sFilterConfig1.IdType = FDCAN_STANDARD_ID;
    sFilterConfig1.FilterIndex = 0;
    sFilterConfig1.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig1.FilterID1 = 0;
    sFilterConfig1.FilterID2 = 0;
    HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig1);

    sFilterConfig1.IdType = FDCAN_EXTENDED_ID;
    sFilterConfig1.FilterIndex = 0;
    sFilterConfig1.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig1.FilterID1 = 0;
    sFilterConfig1.FilterID2 = 0;
    HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig1);

    /* Configure global filter to reject all non-matching frames */
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    /* Configure Rx FIFO 0 watermark to 8 */
    //HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 8);
    //HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_WATERMARK, 0);

    /* Activate Rx FIFO 0 new message notification on both FDCAN instances */
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_BUS_OFF, 0);

    /* Configure and enable Tx Delay Compensation, required for BRS mode.
          TdcOffset default recommended value: DataTimeSeg1 * DataPrescaler
          TdcFilter default recommended value: 0 */
    HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan1, hfdcan1.Init.DataPrescaler * hfdcan1.Init.DataTimeSeg1, 0);
    HAL_FDCAN_EnableTxDelayCompensation(&hfdcan1);

    HAL_FDCAN_Start(&hfdcan1);
}

void can2_config(void)
{
    FDCAN_FilterTypeDef sFilterConfig1;

    sFilterConfig1.IdType = FDCAN_STANDARD_ID;
    sFilterConfig1.FilterIndex = 0;
    sFilterConfig1.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig1.FilterID1 = 0;
    sFilterConfig1.FilterID2 = 0;
    HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig1);

    sFilterConfig1.IdType = FDCAN_EXTENDED_ID;
    sFilterConfig1.FilterIndex = 0;
    sFilterConfig1.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig1.FilterID1 = 0;
    sFilterConfig1.FilterID2 = 0;
    HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig1);

    /* Configure global filter to reject all non-matching frames */
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    /* Configure Rx FIFO 0 watermark to 8 */
    //HAL_FDCAN_ConfigFifoWatermark(&hfdcan2, FDCAN_CFG_RX_FIFO0, 8);
    //HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_WATERMARK, 0);

    /* Activate Rx FIFO 0 new message notification on both FDCAN instances */
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_BUS_OFF, 0);

    /* Configure and enable Tx Delay Compensation, required for BRS mode.
          TdcOffset default recommended value: DataTimeSeg1 * DataPrescaler
          TdcFilter default recommended value: 0 */
    HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan2, hfdcan2.Init.DataPrescaler * hfdcan2.Init.DataTimeSeg1, 0);
    HAL_FDCAN_EnableTxDelayCompensation(&hfdcan2);

    HAL_FDCAN_Start(&hfdcan2);
}

void can1_init(void)
{
    MX_FDCAN1_Init();
    can1_config();
}

void can2_init(void)
{
    MX_FDCAN2_Init();
    can2_config();
} 

void can_init(void)
{
    can1_init();
    can2_init();
}

//busoff handler
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
    //__HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_BUS_OFF);
    if(hfdcan->Instance == FDCAN1) {
        //fdcan1_busoff_flag = 1;
        can1_init();
    } else if(hfdcan->Instance == FDCAN2) {
        //fdcan2_busoff_flag = 2;
        can2_init();
    } else {

    }
}

void can_send(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *txheader, uint8_t tx_data[], uint8_t len)
{
    //busoff not send
    if(__HAL_FDCAN_GET_FLAG(hfdcan, FDCAN_FLAG_BUS_OFF) == SET) {
        return;
    }

    txheader->DataLength =  can_len2dlc(len);

    // txheader->Identifier = can_id;
    // txheader->IdType = std_or_ext ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    // txheader->TxFrameType = FDCAN_DATA_FRAME;
    // txheader->DataLength =  len<<16;   // data_length_code;
    // txheader->ErrorStateIndicator = FDCAN_ESI_PASSIVE;
    // txheader->BitRateSwitch = FDCAN_BRS_OFF;
    // txheader->FDFormat = FDCAN_CLASSIC_CAN;
    // txheader->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    // txheader->MessageMarker = 0;
    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, txheader, tx_data);
}

//ref: stm32h7xx_hal_fdcan.h
//@defgroup FDCAN_data_length_code FDCAN Data Length Code
uint8_t can_dlc2len(uint32_t data_length_code)
{
  uint8_t dlc2len[]={0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64};
  return dlc2len[data_length_code>>16];
}

uint32_t can_len2dlc(uint8_t len)
{
    if (len <= 8) {
        return ((uint32_t)len<<16);
    }
    switch(len) {
        case 12: return (uint32_t)0x9<<16;
        case 16: return (uint32_t)0xA<<16;
        case 20: return (uint32_t)0xB<<16;
        case 24: return (uint32_t)0xC<<16;
        case 32: return (uint32_t)0xD<<16;
        case 48: return (uint32_t)0xE<<16;
        case 64: return (uint32_t)0xF<<16;
    }
    return 0;
}

__weak void can_rx_callback(FDCAN_HandleTypeDef *hfdcan)
{
    UNUSED(hfdcan);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    // if( ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0) ||
    //     ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_WATERMARK) != 0) ||
    //     ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_FULL) != 0) ) {

    //     do {
    //         canfd_rx_callback(hfdcan);
    //     } while((hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0FL) != 0U);
    // }

    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0) {
        can_rx_callback(hfdcan);
    }

    // //FDCAN_RxHeaderTypeDef RxHeader;
    // //uint8_t rx_data[8];
    // //uint8_t cnt = 0;

    // if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0) {
    //     canfd_rx_callback(hfdcan);
    //     //if (hfdcan->Instance == FDCAN1) {
    //     //   printf("can1, ");
    //     // } else if (hfdcan->Instance == FDCAN2) {
    //     //   printf("can2, ");
    //     // } else {
    //     // }
    //     // printf("0x%8X, %2d:", RxHeader.Identifier, can_dlc2len(RxHeader.DataLength));
    //     // for(cnt = 0; cnt < can_dlc2len(RxHeader.DataLength); cnt++) {
    //     //   printf(" %02X", rx_data[cnt]);
    //     // }
    //     // printf("\n\r");
    // }

    // if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_WATERMARK) != 0) {

    // }
}
