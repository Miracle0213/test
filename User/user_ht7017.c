/**
 * @file       user_com.c
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-14
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
/* 头文件包含 *************************************************************************/
#include "user_ht7017.h"
#include "usart.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "ma_filter.h"

#include "param.h"
#include "user_com.h"
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/
ht7017_handle_t g_tHT7017[HT7017_INDEX_MAX_SIZE] = {0};

/* 私有函数声明 ***********************************************************************/
float CaculTwoPointFloat(float x1, float y1, float x2, float y2, float x);

/* 全局变量声明 ***********************************************************************/

/* 全局函数原型 ***********************************************************************/
uint8_t user_ht7017_uart_init(void)
{
    MX_USART1_UART_Init();
    return 0;
}

uint8_t user_ht7017_uart_deinit(void)
{
    HAL_UART_DeInit(&huart1);
    return 0;
}

uint8_t user_ht7017_uart_read(uint8_t *buf, uint16_t len, uint32_t timeout)
{
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, buf, len, timeout);
    UART_ClearError(&huart1);
    return status;
}

uint8_t user_ht7017_uart_write(uint8_t *buf, uint16_t len, uint32_t timeout)
{
    return HAL_UART_Transmit(&huart1, buf, len, timeout);
}

void debug_printf(char *fmt, ...)
{
#if 0
	volatile char str[256];
	volatile uint8_t len;
	va_list args;

	memset((char *)str, 0, sizeof(char) * 256);
	va_start(args, fmt);
	vsnprintf((char *)str, 256, (char const *)fmt, args);
	va_end(args);

	len = strlen((char *)str);

	HAL_UART_Transmit(&huart1, (uint8_t *)str, len, 100);
#endif
}

uint8_t user_ht7017_calib(uint8_t id, uint8_t channel, uint8_t flag)
{
    uint8_t res;
    if (id >= HT7017_INDEX_MAX_SIZE)
    {
        res = 1;
        return res;
    }

    ht7017_handle_t *pHT7017 = &g_tHT7017[id];

    int32_t  raw   = 0;
    uint32_t raw_1 = 0;

    int32_t  dc = 0;
    uint32_t ac = 0;

    if (flag)
    {
        for (int i = 0; i < 20; ++i)
        {
            HT7017_GetSpl(pHT7017, channel, &raw);

            dc += raw;
            HAL_Delay(5);
        }
        dc /= 20;

        if (channel == HT7017_CH_I1)
        {
            g_tParam.DcCurrOffset[id] = dc;
        }
        else if (channel == HT7017_CH_V)
        {
            g_tParam.DcVoltOffset[id] = dc;
        }

        for (int j = 0; j < 20; ++j)
        {
            HT7017_GetRmsCode(pHT7017, channel, &raw_1);
            ac += raw_1;
            HAL_Delay(5);
        }
        ac /= 20;

        if (channel == HT7017_CH_I1)
        {
            ac = g_tParam.AcCurrOffset[id] = (int32_t)ac;
        }
        else if (channel == HT7017_CH_V)
        {
            ac = g_tParam.AcVoltOffset[id] = (int32_t)ac;
        }
    }
    else  // 上电不需要校准
    {
        if (channel == HT7017_CH_I1)
        {
            dc = g_tParam.DcCurrOffset[id];
            ac = g_tParam.AcCurrOffset[id];
        }
        else if (channel == HT7017_CH_V)
        {
            dc = g_tParam.DcVoltOffset[id];
            ac = g_tParam.AcVoltOffset[id];
        }
    }

    res = HT7017_SetWP(pHT7017, HT7017_WP_50H_7CH);

    res = HT7017_SetDCOffset(pHT7017, channel, dc);
    res = HT7017_SetACOffset(pHT7017, channel, ac);
    res = HT7017_SetWP(pHT7017, HT7017_WP_ENABLE);

    return res;
}

/**
 * @brief 校准HT7017平均值电压
 *
 * @param  id 设备ID
 * @param  channel 通道号
 * @param  index 校准点
 * @param  ref_volt 校准电压
 * @return uint8_t 0:成功 1:失败
 */
uint8_t user_ht7017_calib_dc_voltage(uint8_t id, uint8_t channel, uint8_t index, float ref_volt)
{
    if (id >= HT7017_INDEX_MAX_SIZE)
    {
        return 1;
    }

    ht7017_handle_t *pHT7017 = &g_tHT7017[id];

    uint8_t offset  = id * HT7017_INDEX_MAX_SIZE + index;
    int     raw     = 0;
    float   avg_raw = 0;

    for (int i = 0; i < 20; ++i)
    {
        HT7017_GetDcCode(pHT7017, channel, (int32_t *)&raw);

        avg_raw += (float)raw;
        HAL_Delay(5);
    }
    avg_raw /= 20;

    if (channel == HT7017_CH_I1)
    {
        if (ref_volt <= 0.030f)
        {
            if (fabs(avg_raw - g_tCalib.DcCurr[offset].x2) < 1000)
            {
                return 2;
            }
            else
            {
                g_tCalib.DcCurr[offset].x1 = avg_raw;
                g_tCalib.DcCurr[offset].y1 = ref_volt;
            }
        }
        else
        {
            if (fabs(avg_raw - g_tCalib.DcCurr[offset].x1) < 1000)
            {
                return 3;
            }
            else
            {
                g_tCalib.DcCurr[offset].x2 = avg_raw;
                g_tCalib.DcCurr[offset].y2 = ref_volt;

            }
        }
    }
    else if (channel == HT7017_CH_I2)
    {
        if (ref_volt <= 1.0f)
        {
            if (fabs(avg_raw - g_tCalib.DcCurr[offset].x2) < 1000)
            {
                return 4;
            }
            else
            {
                g_tCalib.DcCurr[offset].x1 = avg_raw;
                g_tCalib.DcCurr[offset].y1 = ref_volt;
            }
        }
        else
        {
            if (fabs(avg_raw - g_tCalib.DcCurr[offset].x1) < 1000)
            {
                return 5;
            }
            else
            {
                g_tCalib.DcCurr[offset].x2 = avg_raw;
                g_tCalib.DcCurr[offset].y2 = ref_volt;
            }
        }
    }
    else if (channel == HT7017_CH_V)
    {
        if (ref_volt <= 0.030f)
        {
            if (fabs(avg_raw - g_tCalib.DcVolt[offset].x2) < 1000)
            {
                return 6;
            }
            else
            {
                g_tCalib.DcVolt[offset].x1 = avg_raw;
                g_tCalib.DcVolt[offset].y1 = ref_volt;
            }
        }
        else
        {
            if (fabs(avg_raw - g_tCalib.DcVolt[offset].x1) < 1000)
            {
                return 7;
            }
            else
            {
                g_tCalib.DcVolt[offset].x2 = avg_raw;
                g_tCalib.DcVolt[offset].y2 = ref_volt;
            }
        }
    }

    // Save calibration parameters to non-volatile memory
    SaveCalibParam();

    return 0;
}


extern uint8_t test_res;
/**
 * @brief 校准HT7017有效值电压
 *
 * @param  id 设备ID
 * @param  channel 通道号
 * @param  index 校准点
 * @param  ref_volt 校准电压
 * @return uint8_t 0:成功 1:失败
 */
uint8_t user_ht7017_calib_ac_voltage(uint8_t id, uint8_t channel, uint8_t index, float ref_volt)
{
    if (id >= HT7017_INDEX_MAX_SIZE)
        return 1;
    ht7017_handle_t *pHT7017 = &g_tHT7017[id];

    uint8_t  offset  = id * HT7017_INDEX_MAX_SIZE + index;
    uint32_t raw     = 0;
    float    avg_raw = 0;

    for (int i = 0; i < 20; ++i)
    {
        HT7017_GetRmsCode(pHT7017, channel, &raw);
        avg_raw += (float)raw;
    }
    avg_raw /= 20;

    if (channel == HT7017_CH_I1)
    {
        if (ref_volt <= 0.020f)
        {
            g_tCalib.AcCurr[offset].x1 = avg_raw;
            g_tCalib.AcCurr[offset].y1 = ref_volt;
        }
        else
        {
            g_tCalib.AcCurr[offset].x2 = avg_raw;
            g_tCalib.AcCurr[offset].y2 = ref_volt;
        }
    }
    else if (channel == HT7017_CH_I2)
    {
        if (ref_volt <= 0.020f)
        {
            g_tCalib.AcCurr[offset].x1 = avg_raw;
            g_tCalib.AcCurr[offset].y1 = ref_volt;
        }
        else
        {
            g_tCalib.AcCurr[offset].x2 = avg_raw;
            g_tCalib.AcCurr[offset].y2 = ref_volt;
        }
    }
    else if (channel == HT7017_CH_V)
    {
        if (ref_volt <= 0.020f)
        {
            g_tCalib.AcVolt[offset].x1 = avg_raw;
            g_tCalib.AcVolt[offset].y1 = ref_volt;
        }
        else
        {
            g_tCalib.AcVolt[offset].x2 = avg_raw;
            g_tCalib.AcVolt[offset].y2 = ref_volt;
        }
    }

    SaveCalibParam();

    return 0;
}

/**
 * @brief 校准HT7017交流有功功率
 *
 * @param  id 设备ID
 * @param  ref_volt 校准功率
 * @return uint8_t 0:成功 1:失败
 */
uint8_t user_ht7017_calib_ac_power(uint8_t id, float ref_volt)
{
    if (id >= HT7017_INDEX_MAX_SIZE)
        return 1;
    ht7017_handle_t *pHT7017 = &g_tHT7017[id];   
    
    uint32_t raw     = 0;
    float    avg_raw = 0;

    for (int i = 0; i < 20; ++i)
    {
        HT7017_GetPower_P(pHT7017,&raw);
        avg_raw += (float)raw;
    }
    avg_raw /= 20;

    if (ref_volt <= 0.020f)
    {
        g_tCalib.AcPower[0].x1 = avg_raw;
        g_tCalib.AcPower[0].y1 = ref_volt;
    }
    else
    {
        g_tCalib.AcPower[0].x2 = avg_raw;
        g_tCalib.AcPower[0].y2 = ref_volt;
    }
    SaveCalibParam();

    return 0;
}


/**
 * @brief 根据ADC、校准参数转换为电压值
 *
 * @param  _index 通道号
 * @param  _adc ADC值
 * @return float 校准后的电压值，单位V
 */
float user_ht7017_AdcToDcVolt(uint8_t _index, float _adc)
{
    float volt;

    volt = CaculTwoPointFloat(g_tCalib.DcVolt[_index].x1, g_tCalib.DcVolt[_index].y1, g_tCalib.DcVolt[_index].x2, g_tCalib.DcVolt[_index].y2, _adc);

    //    if (volt < (float) 0.0005f) /* 0.3V以下认为0 */
    //    {
    //        volt = 0;
    //    }
    return volt;
}

/**
 * @brief 根据ADC、校准参数转换为电流值
 *
 * @param  _index 通道号
 * @param  _adc ADC值
 * @return float 校准后的电流值，单位A
 */
float user_ht7017_AdcToDcCurr(uint8_t _index, float _adc)
{
    float volt;

    volt = CaculTwoPointFloat(g_tCalib.DcCurr[_index].x1, g_tCalib.DcCurr[_index].y1, g_tCalib.DcCurr[_index].x2, g_tCalib.DcCurr[_index].y2, _adc);

    //    if (volt < (float) 0.0005f) /* 0.3V以下认为0 */
    //    {
    //        volt = 0;
    //    }
    return volt;
}


/* * @brief 根据ADC、校准参数转换为电流值
 *
 * @param  _index 通道号
 * @param  _adc ADC值
 * @return float 校准后的电流值,单位A
 */
float user_ht7017_AdcToAcCurr(uint8_t _index, float _adc)
{
    float volt;

    volt = CaculTwoPointFloat(g_tCalib.AcCurr[_index].x1, g_tCalib.AcCurr[_index].y1, g_tCalib.AcCurr[_index].x2, g_tCalib.AcCurr[_index].y2, _adc);

    //    if (volt < (float) 0.0005f) /* 0.3V以下认为0 */
    //    {
    //        volt = 0;
    //    }
    return volt;
}

/**
 * @brief 根据ADC、校准参数转换为有效值电压值
 *
 * @param  _index 通道号
 * @param  _adc ADC值
 * @return float 校准后的电压值，单位V
 */
float user_ht7017_AdcToAcVolt(uint8_t _index, float _adc)
{
    float volt;

    volt = CaculTwoPointFloat(g_tCalib.AcVolt[_index].x1, g_tCalib.AcVolt[_index].y1, g_tCalib.AcVolt[_index].x2, g_tCalib.AcVolt[_index].y2, _adc);

    //    if (volt < (float) 0.0005f) /* 0.3V以下认为0 */
    //    {
    //        volt = 0;
    //    }
    return volt;
}


/**
 * @brief 根据ADC、校准参数转换为有功功率值
 *
 * @param  _index 通道号
 * @param  _adc ADC值
 * @return float 校准后的功率值，单位W
 */
float user_ht7017_AdcToAcPower(uint8_t _index, float _adc)
{
    float power;

    power = CaculTwoPointFloat(g_tCalib.AcPower[_index].x1, g_tCalib.AcPower[_index].y1, g_tCalib.AcPower[_index].x2, g_tCalib.AcPower[_index].y2, _adc);


    return power;
}

/* 私有函数原型 ***********************************************************************/
/**
 * @brief CaculTwoPointFloat
 *
 * @param  x1 坐标点1的x坐标
 * @param  y1 坐标点1的y坐标
 * @param  x2 坐标点2的x坐标
 * @param  y2 坐标点2的y坐标
 * @param  x  输入的x坐标
 * @return float x对应的y值
 */
float CaculTwoPointFloat(float x1, float y1, float x2, float y2, float x)
{
    if (x2 == x1)
    {
        return 0;
    }
    return y1 + ((y2 - y1) * (x - x1)) / (x2 - x1);
}
/* ************************************ 文件结束 **************************************/
