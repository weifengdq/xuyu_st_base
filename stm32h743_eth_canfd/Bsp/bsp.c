#include "bsp.h"
#include "bsp_gpio.h"

#define TEST

void bsp_init(void)
{
    gpio_init();
}

void bsp_loop(void)
{
#ifdef TEST
    //gpio_test();
#else
    //user code below
#endif
}
