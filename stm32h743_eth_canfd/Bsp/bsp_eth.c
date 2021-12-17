#include "bsp_eth.h"
#include "main.h"
#include "lwip.h"

void eth_init(void)
{
    HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
    MX_LWIP_Init();
}

void eth_process(void)
{
    MX_LWIP_Process();
}
