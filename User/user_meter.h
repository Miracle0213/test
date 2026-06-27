/**
 * @file       user_meter.h
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-09
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

#ifndef __USER_METER_H
#define __USER_METER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* 头文件包含 *************************************************************************/
#include "queue.h"
#include "ma_filter.h"
#include <stdint.h>
#include "user_com.h"
#include "user_config.h"

/* 全局宏定义 *************************************************************************/

/* 全局宏函数定义 *********************************************************************/

/* 全局类型定义 ***********************************************************************/

typedef struct
{
    int32_t  DC_V;
    int32_t  DC_I1;
    int32_t  DC_I2;
    uint32_t RMS_V;
    uint32_t RMS_I1;
    uint32_t RMS_I2;
    uint32_t RMS_P;

    float ADC_VoltCh;
    float ADC_CurrCh;
    float ADC_QCurrCh;

    float Volt;
    float Curr;
    float QCurr;

    float MAF_Volt;
    float MAF_Curr;
    float MAF_QCurr;

    float MeterVolt;
    float MeterCurr;
    float MeterQCurr;
    float MeterACPower_P;//交流的有功功率

    uint8_t MeasureMode;  // 测试功能
    uint8_t VoltMode;
    uint8_t CurrentMode;

    uint8_t ChipSelect;     // 0=芯片1(输出直流端), 1=芯片2(输入交流端)

    // 输出直流端 (芯片1)
    float MeterVoltOut;
    float MeterCurrOut;

    // 输入交流端 (芯片2)
    float MeterVoltIn;
    float MeterCurrIn;
    float MeterPowerIn;

    uint8_t SwitchMode_Flag;
} User_Meter_T;

/* 全局变量声明 ***********************************************************************/

extern User_Meter_T User_Meter;
extern MA_FILTER_T  MAF_DCVolt[HT7017_INDEX_MAX_SIZE];
extern MA_FILTER_T  MAF_DCCurr[HT7017_INDEX_MAX_SIZE];
#if (DELAY_VALUE_RESULT)
extern QUEUE_HandleTypeDef qValue;
extern QUEUE_DATA_T        BufferValue[Q_VALUE_BUFFER_SIZE];
#endif

/* 全局函数声明 ***********************************************************************/

extern void user_meter_measure_switch(uint8_t func);
extern void user_meter_switch(uint8_t func);
extern void user_meter_queue_clear(void);
extern void user_meter_current_filter_reset(void);
extern void user_meter_voltage_filter_reset(void);

extern void user_meter_init(void);
extern void user_meter_task(void);
extern void user_meter_uart_task(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_METER_H */

#endif
/* ************************************* 文件结束 *************************************/
