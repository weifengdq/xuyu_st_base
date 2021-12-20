#include "bsp_test.h"
#include "bsp.h"
#include "bsp_can.h"
#include "bsp_gpio.h"
#include "bsp_eth.h"
#include "bsp_printf.h"
#include "bsp_timer.h"
#include "bsp_test.h"
#include "bsp_usb.h"
#include "main.h"
#include <stdio.h>
#include "app.h"

multi_timer_t test_timer_1s;

void test_timer_1s_callback(multi_timer_t timer, void *userdata) 
{
    multi_timer_start(&test_timer_1s, 1000, (multi_timer_callback_t)test_timer_1s_callback, userdata);

    //USER CODE BELOW

    //printf("%d, %d, %d\r\n", uccs.refresh, uccs.flag, uccs.btime);
    
    //static uint8_t rollingcnt = 0;
    //multicast_send((uint8_t *)"it's multicast", 14);
}

void test_init(void)
{
    multi_timer_start(&test_timer_1s, 1000, (multi_timer_callback_t)test_timer_1s_callback, "soft timer 1s for test");
    app_init();
}

void test_process(void)
{
    app_process();
}
