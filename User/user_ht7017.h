/**
 * @file       gui.h
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-08
 * @brief      简单描述改文件的功能
 * @version    v1.0
 *
 * ************************************************************************
 * @attention
 * 变更记录     版本号       作者       变更日期       变更内容
 *
 * ************************************************************************
 * 版权所有 © 深圳思拓微电子有限公司。保留所有权利。
 * ************************************************************************
 */
#if 1  // 全局宏开关

#ifndef __USER_HT7017_H
#define __USER_HT7017_H

#ifdef __cplusplus
extern "C"
{
#endif

/* 头文件包含 *************************************************************************/
#include "ma_filter.h"
#include "user_com.h"
#include "bsp_ht7017.h"

/* 全局宏定义 *************************************************************************/

/* 全局宏函数定义 *********************************************************************/

/* 全局类型定义 ***********************************************************************/

/* 全局变量声明 ***********************************************************************/
extern ht7017_handle_t g_tHT7017[HT7017_INDEX_MAX_SIZE];

/* 全局函数声明 ***********************************************************************/
uint8_t user_ht7017_uart_init(void);
uint8_t user_ht7017_uart_deinit(void);
uint8_t user_ht7017_uart_read(uint8_t *buf, uint16_t len, uint32_t timeout);
uint8_t user_ht7017_uart_write(uint8_t *buf, uint16_t len, uint32_t timeout);
void    debug_printf(char *fmt, ...);

uint8_t user_ht7017_calib(uint8_t id, uint8_t channel, uint8_t flag);
uint8_t user_ht7017_calib_dc_voltage(uint8_t id, uint8_t channel, uint8_t index, float ref_volt);
uint8_t user_ht7017_calib_ac_voltage(uint8_t id, uint8_t channel, uint8_t index, float ref_volt);
uint8_t user_ht7017_calib_ac_power(uint8_t id, float ref_power);


float user_ht7017_AdcToDcVolt(uint8_t _index, float _adc);
float user_ht7017_AdcToDcCurr(uint8_t _index, float _adc);
float user_ht7017_AdcToAcVolt(uint8_t _index, float _adc);
float user_ht7017_AdcToAcCurr(uint8_t _index, float _adc);
float user_ht7017_AdcToAcPower(uint8_t _index, float _adc);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_H */

#endif
/* ************************************* 文件结束 *************************************/
