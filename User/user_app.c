/**
 * @file       user_app.c
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-14
 * @brief      简单描述改文件的功能
 * @version    v1.0
 *
 * 本文件进行按键的检测和处理，对相应事件触发相应应用
 * ************************************************************************
 * @attention
 * 变更记录     版本号       作者       变更日期       变更内容
 *
 * ************************************************************************
 * 版权所有 © 深圳思拓微电子有限公司。保留所有权利。
 * ************************************************************************
 */
/* 头文件包含 *************************************************************************/

#include "user_app.h"
#include "user_com.h"
#include "user_meter.h"
#include "user_menu.h"
#include "user_test.h"
#include "user_config.h"

#include "bsp.h"
#include "menu.h"
#include "interface_com.h"
#include <stdio.h>
#include "ma_filter.h"

/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

/* 私有函数声明 ***********************************************************************/

/* 全局变量声明 ***********************************************************************/

Base_Var_T Base_Var;

/* 全局函数原型 ***********************************************************************/

/**
 * @brief 测试步骤复位
 *
 */
void user_app_test_step_reset(void)
{
#if (DELAY_VALUE_RESULT)
    user_meter_queue_clear();
#endif
//    Meter_Measure_Result_Init(); // 确保测试完成后数据保持显示
//    LED1_ON(false);
//    LED2_ON(false);
}

/**
 * @brief 应用层初始化函数
 *
 */
void user_app_init(void)
{
    user_meter_init();
    user_menu_param_init();
    user_test_init();

    UI_Init();
}
/**
 * @brief 应用层任务处理函数
 *
 */
void user_app_task(void)
{
    Base_Var.Key_Code = bsp_GetKey();
    Base_Var.Is_Menu  = Menu_IsRun();

    if (Base_Var.Key_Code != KEY_NONE)
    {
        switch (Base_Var.Key_Code)
        {
            case KEY_1_UP:
            {
            }
            break;
            case KEY_1_LONG_DOWN:
            {
                uSET_EVENT(Base_Var.Is_Menu ? EVENT_NONE : EVENT_MENU_ENTER);
            }
            break;
            case KEY_2_LONG_DOWN:
            {
            }
            break;
            case KEY_2_UP:
            {
                uSET_EVENT(Base_Var.Is_Menu ? EVENT_MENU_KEY_UP : EVENT_METER_KEY_UP);
            }
            break;
            case KEY_3_UP:
            {
                uSET_EVENT(Base_Var.Is_Menu ? EVENT_MENU_KEY_DOWN : EVENT_METER_KEY_DOWN);
            }
            break;
            case KEY_4_DOWN:  // 电压测试
            {
							if (isChNeedMeasure(VoltageMeter))
							{						
                Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_INITIALIZE;  // 开始电压测试
							}
            }
            break;
            case KEY_4_UP:
            case KEY_4_LONG_UP:
            {
                if (g_tParam.Voltage_auto_trigger==0)
                {
                    if (isChNeedMeasure(VoltageMeter))
                    {
                        if (!Base_Var.Is_Menu)
                        {
                            CheckVoltage(g_tParam.switch_to_next[0]);
                        }
                    }
                    Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_MEASURE_END;
                }
            }
            break;
            case KEY_5_DOWN:  // 静流测试
            {
								if (isChNeedMeasure(QCurrentMeter))
                {
									Base_Var.Trig_Event = TEST_STATE_QCURRENT_METER_INITIALIZE;  // 开始静流测试
									bool isFirst = (g_tParam.measure_seq[QCurrentMeter] == 0);    // 当第一通道为QCurrentMeter时，并且开始测试时清除显示数据
									if(isFirst)
									{
									   Meter_Measure_Result_Init();
									   LED1_ON(false);
									   LED2_ON(false);
									}
								}
            }
            break;
            case KEY_5_UP:
            case KEY_5_LONG_UP:
            {
                if (isChNeedMeasure(QCurrentMeter))
                {
                    if (!Base_Var.Is_Menu)
                    {
                        CheckQCurrent();
                    }
                }
                Base_Var.Trig_Event = TEST_STATE_QCURRENT_METER_MEASURE_END;
            }
            break;
            default:
                break;
        }
    }
    user_meter_task();
    user_test_task();
    UI_Run();
}

/* 私有函数原型 ***********************************************************************/

/* ************************************ 文件结束 **************************************/
