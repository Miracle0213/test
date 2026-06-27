/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "user_meter.h"
#include "ringbuffer.h"

uint8_t dat = 0;
volatile uint8_t g_uart2_idle_pending = 0;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

void MX_USART1_UART_Init(void)
{
    huart1.Instance                    = USART1;
    huart1.Init.BaudRate               = 4800;
    huart1.Init.WordLength             = UART_WORDLENGTH_9B;
    huart1.Init.StopBits               = UART_STOPBITS_1;
    huart1.Init.Parity                 = UART_PARITY_EVEN;
    huart1.Init.Mode                   = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_SWAP_INIT;
    huart1.AdvancedInit.Swap           = UART_ADVFEATURE_SWAP_ENABLE;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
    {
        Error_Handler();
    }

    // __HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);  // 使能 ERR中断
}

void MX_USART2_UART_Init(void)
{
    huart2.Instance                    = USART2;
    huart2.Init.BaudRate               = 115200;
    huart2.Init.WordLength             = UART_WORDLENGTH_8B;
    huart2.Init.StopBits               = UART_STOPBITS_1;
    huart2.Init.Parity                 = UART_PARITY_NONE;
    huart2.Init.Mode                   = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_UART_Receive_IT(&huart2, &dat, 1);        // hal库用于开启串口接收的函数
    __HAL_UART_CLEAR_IDLEFLAG(&huart2);           // 清除IDLE标志
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);  // 使能 IDLE中断
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
    GPIO_InitTypeDef         GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit   = {0};
    if (uartHandle->Instance == USART1)
    {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
        PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();

        // PB6     ------> USART1_TX
        // PB7     ------> USART1_RX
        GPIO_InitStruct.Pin       = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        //        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        //        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    else if (uartHandle->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();

        // PA2     ------> USART2_TX
        // PA3     ------> USART2_RX
        GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
}

void UART2_TX_MODE(uint8_t mode)
{
    static uint8_t _mode = 255;

    if (_mode != mode)
    {
        _mode                            = mode;
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin   = GPIO_PIN_2;
        GPIO_InitStruct.Pull  = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        if (mode == 0)
        {
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        }
        else
        {
            GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
        }
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{
    if (uartHandle->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_DISABLE();

        // PA2     ------> USART2_TX
        // PA3     ------> USART2_RX
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);
    }
    else if (uartHandle->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_DISABLE();

        // PA2     ------> USART2_TX
        // PA3     ------> USART2_RX
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
    }
}

// 串口接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        RingAdd(&xBufferRX, dat);
        HAL_UART_Receive_IT(&huart2, &dat, 1);  // 继续使能RX中断
    }
}

extern void user_meter_measure_switch(uint8_t func);

// 串口接收空闲中断回调函数，这个是从hal里面新加的
void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        g_uart2_idle_pending = 1;
    }
}

void UART_ClearError(UART_HandleTypeDef *huart)
{
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_PE))
    {
        /*!< Parity error            */
        // 奇偶校验错误
        __HAL_UART_CLEAR_PEFLAG(huart);
    }
    else if (__HAL_UART_GET_FLAG(huart, UART_FLAG_NE))
    {
        /*!< Noise error             */
        // 噪声错误
        __HAL_UART_CLEAR_NEFLAG(huart);
    }
    else if (__HAL_UART_GET_FLAG(huart, UART_FLAG_FE))
    {
        /*!< Frame error             */
        // 帧格式错误
        __HAL_UART_CLEAR_FEFLAG(huart);
    }
    else if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE))
    {
        /*!< Overrun error           */
        // 数据太多串口来不及接收错误
        __HAL_UART_CLEAR_OREFLAG(huart);
    }
}

// 串口接收错误中断，下一节会说明为啥要有这个
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    UART_ClearError(huart);
    // 当这个串口发生了错误，一定要在重新使能接收中断
    if (huart->Instance == USART2)
    {
        HAL_UART_Receive_IT(&huart2, &dat, 1);
    }
    // 其他串口......
}
