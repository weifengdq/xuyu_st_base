#include "bsp_timer.h"
#include "main.h"
#include "tim.h"
#include "mid_multi_timer.h"

uint64_t g_1ms_tick_count = 0;

uint32_t multi_timer_platform_ticks_get_func(void)
{
    return (uint32_t)g_1ms_tick_count&0xFFFFFFFF;
}

void multi_timer_init(void)
{
    MultiTimerInstall(multi_timer_platform_ticks_get_func);
}

void multi_timer_process(void)
{
    MultiTimerYield();
}

void timer_init(void)
{
    MX_TIM6_Init();
    HAL_TIM_Base_Start_IT(&htim6);

    multi_timer_init();
}

void timer_process(void)
{
    multi_timer_process();
}

void multi_timer_start(multi_timer_t* timer, uint32_t timing, multi_timer_callback_t callback, void* userData)
{
    MultiTimerStart(timer, timing, callback, userData);
}

void timer6_1ms_callback(void)
{
    g_1ms_tick_count++;
}

//timer callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6) {
        timer6_1ms_callback();
    }
}
