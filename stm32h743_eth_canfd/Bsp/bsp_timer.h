#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include "mid_multi_timer.h"

typedef MultiTimer multi_timer_t;
typedef MultiTimerCallback_t multi_timer_callback_t;

extern void timer_init(void);
extern void timer_process(void);

extern void multi_timer_start(multi_timer_t* timer, uint32_t timing, multi_timer_callback_t callback, void* userData);

#endif
