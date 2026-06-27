/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved.
 *
 * @file      bsp.h
 * @brief     none
 * @version   1.0.0
 * @author    Wei-JiaBao
 * @date      2022/9/21
 */

#ifndef BSP_H
#define BSP_H

#define ENABLE_INT()  __set_PRIMASK(0)
#define DISABLE_INT() __set_PRIMASK(1)

#include "main.h"

#include "bsp_key.h"
#include "bsp_utils.h"

#include "param.h"

// #include "bsp_tft_lcd.h"
// #include "bsp_tft_port.h"
//  #include "lcd.h"

#include "bsp_cpu_flash.h"
#include "bsp_ht7017.h"

#define BSP_SET_GPIO_1(gpio, pin) gpio->BSRR = pin
#define BSP_SET_GPIO_0(gpio, pin) gpio->BSRR = (uint32_t)(pin) << 16U

#define LED1_ON(BOOL) BOOL ? HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED2_ON(BOOL)                                                    \
    do                                                                   \
    {                                                                    \
        if (BOOL)                                                        \
        {                                                                \
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);          \
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); \
        }                                                                \
        else                                                             \
        {                                                                \
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);        \
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);   \
        }                                                                \
    } while (0)

void    bsp_DelayMS(uint32_t _ulDelayTime);
int32_t bsp_GetRunTime(void);
int32_t bsp_CheckRunTime(int32_t _lastTime);

void bsp_Init(void);
void SysTick_ISR(void);
void bsp_GetCpuID(uint32_t *_id);
void bsp_Idle(void);

#endif  // BSP_H
