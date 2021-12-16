#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_eth.h"
#include "bsp_usb.h"
#include "main.h"
#include <stdio.h>

//#define LED_TEST

void bsp_init(void)
{
    gpio_init();
    usb_init();
    eth_init();

    led1_red_on();
    printf("\r\napp build time: %s, %s\r\n", __DATE__, __TIME__);
    printf("elapsed time  : %.3fs\n", HAL_GetTick()/1000.0);
    printf("stm32h7 init success\r\n");
}

void bsp_loop(void)
{
#ifdef LED_TEST
    gpio_test();
#else
    //user code below
#endif

    eth_process();
}
