/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved.
 *
 * @file      bsp.c
 * @brief     none
 * @version   1.0.0
 * @author    Wei-JiaBao
 * @date      2022/9/21
 */

#include "bsp.h"
#include "user_meter.h"
#include "main.h"

/* 这2个全局变量转用于 bsp_DelayMS() 函数 */
static volatile uint32_t s_uiDelayCount       = 0;
static volatile uint8_t  s_ucTimeOutFlag      = 0;
__IO int32_t             g_iRunTime           = 0;
static __IO uint8_t      g_ucEnableSystickISR = 0; /* 等待变量初始化 */

void bsp_DelayMS(uint32_t _ulDelayTime)
{
    HAL_Delay(_ulDelayTime);
}

int32_t bsp_GetRunTime(void)
{
    int32_t runtime;

    DISABLE_INT(); /* 关中断 */

    runtime = g_iRunTime; /* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

    ENABLE_INT(); /* 开中断 */

    return runtime;
}

int32_t bsp_CheckRunTime(int32_t _lastTime)
{
    int32_t now_time;
    int32_t time_diff;

    DISABLE_INT(); /* 关中断 */

    now_time = g_iRunTime; /* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

    ENABLE_INT(); /* 开中断 */

    if (now_time >= _lastTime)
    {
        time_diff = now_time - _lastTime;
    }
    else
    {
        time_diff = 0x7FFFFFFF - _lastTime + now_time;
    }

    return time_diff;
}

void bsp_Init(void)
{
    bsp_InitKey(); /* 按键初始化，要放在滴答定时器之前，因为按钮检测是通过滴答定时器扫描 */

    ENABLE_INT();
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RunPer10ms
*	功能说明: 该函数每隔10ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些处理时间要求不严格的
*			任务可以放在此函数。比如：按键扫描、蜂鸣器鸣叫控制等。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
    bsp_KeyScan10ms();

    // BEEP_Pro(); /* 蜂鸣器任务 */

    // PERIOD_Scan(); /* 控制LED闪烁 */

    // bsp_AdcTask10ms(); /* ADC后台任务 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RunPer1ms
*	功能说明: 该函数每隔1ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理的事务
*			 可以放在此函数。比如：触摸坐标扫描。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{
}

void SysTick_ISR(void)
{
    static uint8_t s_count = 0;

    static uint16_t Tim10ms = 0;
    Tim10ms++;
    if (Tim10ms >= 10)
    {
        Tim10ms      = 0;
    }

    /* 每隔1ms进来1次 （仅用于 bsp_DelayMS） */
    if (s_uiDelayCount > 0)
    {
        if (--s_uiDelayCount == 0)
        {
            s_ucTimeOutFlag = 1;
        }
    }

    /* 全局运行时间每1ms增1 */
    g_iRunTime++;
    if (g_iRunTime == 0x7FFFFFFF) /* 这个变量是 int32_t 类型，最大数为 0x7FFFFFFF */
    {
        g_iRunTime = 0;
    }

    bsp_RunPer1ms(); /* 每隔1ms调用一次此函数，此函数在 bsp.c */

    if (++s_count >= 10)
    {
        s_count = 0;
        bsp_RunPer10ms(); /* 每隔10ms调用一次此函数，此函数在 bsp.c */
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_GetCpuID
*    功能说明: 该CPU UID
*    形    参: _id : 返回ID
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_GetCpuID(uint32_t *_id)
{
    _id[0] = *(__IO uint32_t *)(0x1FFF7A10);
    _id[1] = *(__IO uint32_t *)(0x1FFF7A10 + 4);
    _id[2] = *(__IO uint32_t *)(0x1FFF7A10 + 8);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Idle
*	功能说明: 空闲时执行的函数。一般主程序在for和while循环程序体中需要插入 CPU_IDLE() 宏来调用本函数。
*			 本函数缺省为空操作。用户可以添加喂狗、设置CPU进入休眠模式的功能。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Idle(void)
{
    /* --- 喂狗 */

    /* --- 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */

    /* 例如 emWin 图形库，可以插入图形库需要的轮询函数 */
    // GUI_Exec();

    /* 例如 uIP 协议，可以插入uip轮询函数 */
    // TOUCH_CapScan();
}

/*
*********************************************************************************************************
*	函 数 名: HAL_Delay
*	功能说明: 重定向毫秒延迟函数。替换HAL中的函数。因为HAL中的缺省函数依赖于Systick中断，如果在USB、SD卡
*             中断中有延迟函数，则会锁死。也可以通过函数HAL_NVIC_SetPriority提升Systick中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
/* 当前例子使用stm32f4xx_hal.c默认方式实现，未使用下面重定向的函数 */
#if 0
void HAL_Delay(uint32_t Delay)
{
	bsp_DelayUS(Delay * 1000);
}
#endif
