/**
 * @file       user_com.h
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
#if 1  // 全局宏开关

#ifndef __USER_COM_H
#define __USER_COM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* 头文件包含 *************************************************************************/

#include <stdint.h>
#include <stdbool.h>
/* 全局宏定义 *************************************************************************/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#define EVENT_NONE           0
#define EVENT_METER_KEY_UP   (1 << 1)
#define EVENT_METER_KEY_DOWN (1 << 2)
#define EVENT_MENU_ENTER     (1 << 3)
#define EVENT_MENU_KEY_UP    (1 << 4)
#define EVENT_MENU_KEY_DOWN  (1 << 5)
#define EVENT_MENU_KEY_OK    (1 << 6)

enum
{
    HT7017_INDEX_01 = 0,
    HT7017_INDEX_02 = 1,
    HT7017_INDEX_MAX_SIZE,
};

enum
{
    VoltageMeter,
    CurrentMeter,
    ACPowerMeter,
    QCurrentMeter,
    VoltAndCurrMeter,
    VoltAndQCurrMeter,   
    MaxFunc,
};

/* 全局宏函数定义 *********************************************************************/

#define uSET_BIT(value, mask)   (value |= (mask))
#define uCLEAR_BIT(value, mask) (value &= ~(mask))

#define uSET_EVENT(mask)   (uSET_BIT(Base_Var.Key_Trig_Event, mask))
#define uCLEAR_EVENT(mask) (uCLEAR_BIT(Base_Var.Key_Trig_Event, mask))

/* 全局类型定义 ***********************************************************************/

typedef struct
{
    uint8_t Key_Code;
    uint8_t Is_Menu;
    uint8_t Trig_Event;
    uint8_t Key_Trig_Event;
} Base_Var_T;

extern Base_Var_T Base_Var;

/* 全局变量声明 ***********************************************************************/

/* 全局函数声明 ***********************************************************************/
int8_t   get_InitMeasureChannel(void);
int8_t   getNextMeasureCh(uint8_t ch);
uint8_t* FindNeedMeasureChannel(void);
bool     isChNeedMeasure(uint8_t ch);
int8_t getNeedMeasureSum(void);

void CheckVoltage(bool isAllowSwitch);
void CheckCurrent(void);
void CheckQCurrent(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_COM_H */

#endif
/* ************************************* 文件结束 *************************************/
