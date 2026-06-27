/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved
 *
 * @file      bsp_ht7017.h
 * @brief     none
 * @version   1.0.0
 * @author    Wei-JiaBao
 * @date      2022/3/9
 */

#ifndef _BSP_HT7017_H_
#define _BSP_HT7017_H_

#include <stdint.h>

enum
{
    HT7017_WP_50H_7CH = 0xA6,
    HT7017_WP_40H_45H = 0xBC,
    HT7017_WP_ENABLE  = 0xFF,
};

enum
{
    HT7017_SPL_976HZ   = 0x00,
    HT7017_SPL_1953HZ  = 0x01,
    HT7017_SPL_3906HZ  = 0x02,
    HT7017_SPL_7812HZ  = 0x03,
    HT7017_SPL_15620HZ = 0x07,
};

enum
{
    HT7017_EMU_2MHZ = 0x00,
    HT7017_EMU_1MHZ = 0x01,
};

enum
{
    HT7017_PRFCFG_3_125HZ = 0x00,
    HT7017_PRFCFG_12_5HZ  = 0x02,
};

enum
{
    HT7017_CH_I1,  // 电流通道1
    HT7017_CH_I2,  // 电流通道2
    HT7017_CH_V,   // 电压通道1
    HT7017_CH_V2,  // 电压通道2
};

enum
{
    MEAS_CURR_CH1 = 0,
    MEAS_CURR_CH2 = 1,
};

enum
{
    MEAS_VOLT_CH1 = 0,
};

enum
{
    MEAS_POWERP_CH1 = 0,
};

typedef struct
{
    uint8_t (*uart_init)(void);
    uint8_t (*uart_deinit)(void);
    uint8_t (*uart_read)(uint8_t *buf, uint16_t len, uint32_t timeout);
    uint8_t (*uart_write)(uint8_t *buf, uint16_t len, uint32_t timeout);
    void (*delay_us)(uint32_t ms);
    void (*debug_print)(char *fmt, ...);
    uint8_t inited;

} ht7017_handle_t;  // 句柄指针

#define DRIVER_HT7017_LINK_INIT(HANDLE, STRUCTURE)  memset(HANDLE, 0, sizeof(STRUCTURE))
#define DRIVER_HT7017_LINK_UART_INIT(HANDLE, FUC)   (HANDLE)->uart_init = FUC
#define DRIVER_HT7017_LINK_UART_DEINIT(HANDLE, FUC) (HANDLE)->uart_deinit = FUC
#define DRIVER_HT7017_LINK_UART_READ(HANDLE, FUC)   (HANDLE)->uart_read = FUC
#define DRIVER_HT7017_LINK_UART_WRITE(HANDLE, FUC)  (HANDLE)->uart_write = FUC
#define DRIVER_HT7017_LINK_DELAY_US(HANDLE, FUC)    (HANDLE)->delay_us = FUC
#define DRIVER_HT7017_LINK_DEBUG_PRINT(HANDLE, FUC) (HANDLE)->debug_print = FUC
uint8_t HT7017_Init(ht7017_handle_t *handle);
uint8_t HT7017_DeInit(ht7017_handle_t *handle);
uint8_t HT7017_Write(ht7017_handle_t *handle, uint8_t reg, const uint8_t *data, uint16_t len);
uint8_t HT7017_Read(ht7017_handle_t *handle, uint8_t reg, uint8_t *data, uint16_t len);
uint8_t HT7017_MultipleRead(ht7017_handle_t *handle, uint8_t regStart, uint8_t regEnd, uint8_t *data, uint16_t len);
uint8_t HT7017_Get_ID(ht7017_handle_t *handle);  // ID获取
uint8_t HT7017_SWREST(ht7017_handle_t *handle);
uint8_t HT7017_SetWP(ht7017_handle_t *handle, uint8_t wp);
uint8_t HT7017_SetMode(ht7017_handle_t *handle, uint8_t mode);
uint8_t HT7017_SetFreq(ht7017_handle_t *handle, uint16_t freq);
uint8_t HT7017_SetAdcCon(ht7017_handle_t *handle, uint16_t con);
uint8_t HT7017_SetDCOffset(ht7017_handle_t *handle, uint8_t ch, int32_t offset);
uint8_t HT7017_SetACOffset(ht7017_handle_t *handle, uint8_t ch, uint32_t offset);
uint8_t HT7017_GetSpl(ht7017_handle_t *handle, uint8_t ch, int32_t *spl);
uint8_t HT7017_GetDcCode(ht7017_handle_t *handle, uint8_t ch, int32_t *dc);
uint8_t HT7017_GetRmsCode(ht7017_handle_t *handle, uint8_t ch, uint32_t *rms);
uint8_t HT7017_GetValue(ht7017_handle_t *handle, int32_t *idc, int32_t *udc);
uint8_t HT7017_GetAllValue(ht7017_handle_t *handle, int32_t *idc, int32_t *udc, uint32_t *i1rms, uint32_t *i2rms, uint32_t *urms);
uint8_t HT7017_GetPower_P(ht7017_handle_t *handle, int32_t *dc);
uint8_t HT7017_GetPower_Q(ht7017_handle_t *handle, int32_t *dc);



#endif /* _BSP_HT7017_H_ */
