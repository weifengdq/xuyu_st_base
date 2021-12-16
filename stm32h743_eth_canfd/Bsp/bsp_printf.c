#include "bsp_printf.h"
#include "main.h"
#include "usbd_cdc_if.h"
#include <stdio.h>

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
    while(CDC_Transmit_FS((uint8_t*)&ch, 1) != 0);
    return (ch);
}
