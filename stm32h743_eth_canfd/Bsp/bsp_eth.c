#include "bsp_eth.h"
#include "main.h"
#include "lwip.h"

void eth_init(void)
{
    MX_LWIP_Init();
}

void eth_process(void)
{
    MX_LWIP_Process();
}
