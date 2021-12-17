#include "bsp_printf.h"
#include "main.h"
#include "usbd_cdc_if.h"
#include "mid_ring_buffer.h"
#include <stdio.h>

static ring_buffer_t p_r_buf;

#pragma import(__use_no_semihosting)

void _sys_exit(int x)
{
    x = x;
}

struct __FILE
{
    int handle;
};
FILE __stdout;

int fputc(int ch, FILE *f)
{
    //HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);
    //while(CDC_Transmit_FS((uint8_t*)&ch, 1) != 0);
    //CDC_Transmit_FS((uint8_t*)&ch, 1);

    // for(uint32_t i = 0; i < 1000; i++) {
    //     if(CDC_Transmit_FS((uint8_t*)&ch, 1) == 0) {
    //         break;
    //     }
    // }

    ring_buffer_queue(&p_r_buf, (char)ch);
    return (ch);
}

//USB2 Full Speed
//12Mb/s => 1.5MB/s => 1500B/ms
//1500B circular buffer maybe ok

void printf_init(void)
{
    ring_buffer_init(&p_r_buf);
}

void printf_process(void)
{
    static uint32_t tickstart = 0;
    static char buffer[RING_BUFFER_SIZE];
    uint16_t num = 0;

    if(ring_buffer_is_empty(&p_r_buf)) {
        tickstart = HAL_GetTick();
    } else {
        if(HAL_GetTick() - tickstart >= 1) {
            tickstart = HAL_GetTick();
            num = ring_buffer_num_items(&p_r_buf);
            ring_buffer_dequeue_arr(&p_r_buf, buffer, num);
            CDC_Transmit_FS((uint8_t*)buffer, num);
        }
    }
}
