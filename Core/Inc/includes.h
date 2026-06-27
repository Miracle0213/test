/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved
 *
 * @file      includes.h
 * @brief     头文件汇总
 * @version   1.0.0
 * @author    WeiJiaBao
 * @date      2022/1/6
 *
 * @note		1.	[2022/1/6][v1.0.0][WeiJiaBao]	first commit.
 *
 */

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
//#include  <stdint.h>
#include <math.h>

/*
*********************************************************************************************************
*                                           OS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        APP / BSP
*********************************************************************************************************
*/

#include "bsp.h"
#include "ui_def.h"

#include "Status_SystemSet.h"

/*
*********************************************************************************************************
*                                            其它
*********************************************************************************************************
*/

#include "disp_lib.h"
#include "modify_param.h"
#include "param.h"
#include "lcd_menu.h"

/*
*********************************************************************************************************
*                                          变量和函数
*********************************************************************************************************
*/


/* 主程序状态字定义, MS = Main Status */
enum
{
    MS_LINK_MODE = 0, /* 联机状态 */
    MS_POWER_ON, /* 联机状态 */
    MS_PCB_TEST, /* 关于-硬件信息 */
    MS_SYSTEM_SET, /* 系统设置 */

    MS_HARD_INFO, /* 关于-硬件信息 */
    MS_USB_EMMC, /* 虚拟串口状态。RS232 RS485 TTL-UART */
    MS_MODIFY_PARAM, /* 参数设置 */
    MS_FILE_MANAGE, /* 文件管理 */

    MS_EXTEND_MENU_CALIB, /* 第1级菜单 */
    MS_EXTEND_MENU_LUA, /* 第2级菜单-LUA程序 */
    MS_EXTEND_MENU_REC, /* 第2级菜单-数据记录仪 */
    MS_STEP_SELECT_FILE,

    MS_USB_UART, /* USB虚拟串口 */
};

uint16_t NextStatus(uint16_t _NowStatus);
uint16_t LastStatus(uint16_t _NowStatus);

extern uint16_t g_MainStatus;

// /* 方便RTOS里面使用 */
extern void SysTick_ISR(void);

#define bsp_ProPer1ms SysTick_ISR

#endif


