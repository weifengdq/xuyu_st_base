#include "bsp.h"
#include "bsp_can.h"
#include "bsp_gpio.h"
#include "bsp_eth.h"
#include "bsp_printf.h"
#include "bsp_usb.h"
#include "main.h"
#include <stdio.h>

//#define LED_TEST

void bsp_init(void)
{
    gpio_init();
    usb_init();
    printf_init();
    eth_init();
    can_init(); //can place in front of eth_init()

    printf("\r\napp build time: %s, %s\r\n", __DATE__, __TIME__);
    printf("elapsed time  : %.3fs\n", HAL_GetTick()/1000.0);
    uint32_t ip_addr = eth_get_ip_addr();
    printf("ip: %d.%d.%d.%d\r\n", (ip_addr&0xFF), (ip_addr>>8&0xFF), (ip_addr>>16&0xFF), (ip_addr>>24&0xFF));
    printf("multicast addr@port: %d.%d.%d.%d@%d\r\n", MIP1, MIP2, MIP3, MIP4, MPORT);
    printf("stm32h7 init success\r\n");
    multicast_send((uint8_t *)"it's multicast", 14);
    led5_red_on();
}

void bsp_loop(void)
{
#ifdef LED_TEST
    gpio_test();
#else
    //user code below
#endif
    printf_process();
    eth_process();
}
