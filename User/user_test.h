/**
 * @file       user_test.h
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

#ifndef __USER_TEST_H
#define __USER_TEST_H

#ifdef __cplusplus
extern "C"
{
#endif

/* 头文件包含 *************************************************************************/

/* 全局宏定义 *************************************************************************/

#define TEST_STATE_IDEL                          0
#define TEST_STATE_VOLTAGE_METER_INITIALIZE      1
#define TEST_STATE_VOLTAGE_METER_WAIT_TRIGGER    2
#define TEST_STATE_VOLTAGE_METER_TRIGGER_MEASURE 3
#define TEST_STATE_VOLTAGE_METER_MEASURE         4
#define TEST_STATE_VOLTAGE_METER_MEASURE_END     5
#define TEST_STATE_CURRENT_METER_INITIALIZE      6
#define TEST_STATE_CURRENT_METER_MEASURE         7
#define TEST_STATE_CURRENT_METER_WAIT_TRIGGER    8
#define TEST_STATE_CURRENT_METER_MEASURE_END     9
#define TEST_STATE_QCURRENT_METER_INITIALIZE     10
#define TEST_STATE_QCURRENT_METER_MEASURE        11
#define TEST_STATE_QCURRENT_METER_MEASURE_END    12
#define TEST_STATE_QCURRENT_METER_MEASURE_END    12
#define TEST_STATE_QCURRENT_METER_MEASURE_END    12

/* 全局宏函数定义 *********************************************************************/

/* 全局类型定义 ***********************************************************************/

/* 全局变量声明 ***********************************************************************/

void user_test_init(void);
void user_test_task(void);

/* 全局函数声明 ***********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __USER_TEST_H */

#endif
/* ************************************* 文件结束 *************************************/
