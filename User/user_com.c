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
#include "user_com.h"
#include "user_app.h"
#include "user_com.h"
#include "user_meter.h"
#include "user_menu.h"
#include "user_test.h"
#include "user_config.h"

#include "bsp.h"
#include <stdio.h>
#include "interface_com.h"
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

/* 私有函数声明 ***********************************************************************/

/* 全局变量声明 ***********************************************************************/

/* 全局函数原型 ***********************************************************************/

/**
 * @brief 查找第一个测试的通道
 *
 * @return uint8_t 第一个测试的通道
 */
int8_t get_InitMeasureChannel(void)
{
    for (int i = 0; i < CH_SUM; i++)
    {
        if (g_tParam.measure_seq[i] == 0)
        {
            return i;
        }
    }
    return -1;
}
/**
 * @brief 查找下一个测试的通道
 *
 * @param  ch  当前测试通道
 * @return uint8_t 下一个测试通道
 */
int8_t getNextMeasureCh(uint8_t ch)  // 2:静流（0：电压 1：充电 2：静流）
{
    uint8_t i;

    for (i = 0; i < CH_SUM; i++)
    {
        if ((g_tParam.measure_seq[ch] + 1) % SEQ_SUM == g_tParam.measure_seq[i] && g_tParam.measure_seq[i] != SEQ_IDLE)
        {
            break;
        }
    }
    return i;
}

/**
 * @brief 判断当前通道是否需要测量
 *
 * @param  ch  当前测试通道
 * @return bool 是否需要测量 
 */
bool isChNeedMeasure(uint8_t ch)
{
    if (g_tParam.measure_seq[ch] != SEQ_IDLE)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief 获取当前需要测量的通道总数
 * 
 * @return int8_t 是否需要测量的通道总数
 */
int8_t getNeedMeasureSum(void)
{
	int8_t chsum=0;
	for(int8_t i=0;i<3;i++)
	{
		if(g_tParam.measure_seq[i] != SEQ_IDLE)
		{
			chsum++;
		}
	}
	return chsum;
}
/**
 * @brief 检查电压
 *
 */
void CheckVoltage(bool isAllowSwitch)
{
//     char buf[32] = {0};
//     if (g_tVar.ValueResult[0] < 0.0f)
//     {
//         sprintf(buf, "0.000V ");
//     }
//     else if (g_tVar.ValueResult[0] < 110.0f)
//     {
//         sprintf(buf, "%06.3fV ", g_tVar.ValueResult[0]);
//     }
//     else
//     {
//         sprintf(buf, "9.999V ");
//     }

//     GUI_SetFont(&GUI_FontDengXian_32);
//     GUI_SetColor(C_WHITE);
//     if (g_tVar.ValueResult[0] >= g_tParam.VoltageRange[1] && g_tVar.ValueResult[0] <= g_tParam.VoltageRange[0])
//     {
//         GUI_SetBkColor(C_BLUE);
//         g_tVar.ValueResultPass[0] = true;
//     }
//     else
//     {
//         GUI_SetBkColor(C_RED);
//     }
//     GUI_DispStringAt(buf, MENU_X_START_OFFSET + 15, MENU_Y_START_OFFSET + 15 + 38 * INDEX_VOLTAGE);

//     if (g_tParam.measure_seq[VoltageMeter] == SEQ_SUM - 1)
//     {
//         if ((!g_tVar.ValueResultPass[0] && g_tParam.measure_seq[0] != SEQ_IDLE) || (!g_tVar.ValueResultPass[1] && g_tParam.measure_seq[1] != SEQ_IDLE) ||
//             (!g_tVar.ValueResultPass[2] && g_tParam.measure_seq[2] != SEQ_IDLE))
//         {
//             LED1_ON(false);
//             LED2_ON(true);
//         }

//         else
//         {
//             LED1_ON(true);
//             LED2_ON(false);
//         }
//     }
//     else
//     {
//         if (!g_tVar.ValueResultPass[0])
//         {
//             LED1_ON(false);
//             LED2_ON(true);
//         }
//         else
//         {
//             LED1_ON(false);
//             LED2_ON(false);
//         }
//     }

//     if (isAllowSwitch)
//     {
//         User_Meter.MeasureMode = getNextMeasureCh(VoltageMeter);
// #if (DELAY_VALUE_RESULT)
//         user_meter_queue_clear();
// #endif
//         user_meter_voltage_filter_reset();
//         user_meter_measure_switch(User_Meter.MeasureMode);
//     }
}

/**
 * @brief 检查充电电流
 *
 */
void CheckCurrent(void)
{
//     char buf[32] = {0};
//     if (g_tVar.ValueResult[1] < 0.0f)
//     {
//         sprintf(buf, "0.000A ");
//     }
//     else if (g_tVar.ValueResult[1] < 10.0f)
//     {
//         sprintf(buf, "%05.3fA ", g_tVar.ValueResult[1]);
//     }
//     else
//     {
//         sprintf(buf, "9.999A ");
//     }

//     GUI_SetFont(&GUI_FontDengXian_32);
//     GUI_SetColor(C_WHITE);

//     if (g_tVar.ValueResult[1] >= g_tParam.CurrentRange[1] && g_tVar.ValueResult[1] <= g_tParam.CurrentRange[0])
//     {
//         GUI_SetBkColor(C_BLUE);
//         g_tVar.ValueResultPass[1] = true;
//     }
//     else
//     {
//         GUI_SetBkColor(C_RED);
//     }
//     GUI_DispStringAt(buf, MENU_X_START_OFFSET + 15, MENU_Y_START_OFFSET + 15 + 38 * INDEX_CURRENT);

//     if (g_tParam.measure_seq[CurrentMeter] == SEQ_SUM - 1)
//     {
//         if ((!g_tVar.ValueResultPass[0] && g_tParam.measure_seq[0] != SEQ_IDLE) || (!g_tVar.ValueResultPass[1] && g_tParam.measure_seq[1] != SEQ_IDLE) ||
//             (!g_tVar.ValueResultPass[2] && g_tParam.measure_seq[2] != SEQ_IDLE))
//         {
//             LED1_ON(false);
//             LED2_ON(true);
//         }
//         else
//         {
//             LED1_ON(true);
//             LED2_ON(false);
//         }
//     }
//     else
//     {
//         if (!g_tVar.ValueResultPass[1])
//         {
//             LED1_ON(false);
//             LED2_ON(true);
//         }
//         else
//         {
//             LED1_ON(false);
//             LED2_ON(false);
//         }
//     }

//     if (g_tParam.switch_to_next[1])
//     {
//         User_Meter.MeasureMode = getNextMeasureCh(CurrentMeter);
// #if (DELAY_VALUE_RESULT)
//         user_meter_queue_clear();
// #endif
//         user_meter_current_filter_reset();
//         user_meter_measure_switch(User_Meter.MeasureMode);
//     }
}

/**
 * @brief 检查静流
 *
 */
void CheckQCurrent(void)
{
//     char buf[32] = {0};
//     if (g_tVar.ValueResult[2] < 0.0f)
//     {
//         sprintf(buf, "000.0μA");
//     }
//     else if (g_tVar.ValueResult[2] < 400.0f)
//     {
//         sprintf(buf, "%05.1fμA", g_tVar.ValueResult[2]);
//     }
//     else
//     {
//         sprintf(buf, "999.9μA");
//     }

//     GUI_SetFont(&GUI_FontDengXian_32);
//     GUI_SetColor(C_WHITE);

//     if (g_tVar.ValueResult[2] >= g_tParam.QCurrentRange[1] && g_tVar.ValueResult[2] <= g_tParam.QCurrentRange[0])
//     {
//         GUI_SetBkColor(C_BLUE);
//         g_tVar.ValueResultPass[2] = true;
//     }
//     else
//     {
//         GUI_SetBkColor(C_RED);
//     }
//     GUI_DispStringAt(buf, MENU_X_START_OFFSET + 15, MENU_Y_START_OFFSET + 15 + 38 * INDEX_QCURRENT);

//     if (g_tParam.measure_seq[QCurrentMeter] == SEQ_SUM - 1)
//     {
//         if ((!g_tVar.ValueResultPass[0] && g_tParam.measure_seq[0] != SEQ_IDLE) || (!g_tVar.ValueResultPass[1] && g_tParam.measure_seq[1] != SEQ_IDLE) ||
//             (!g_tVar.ValueResultPass[2] && g_tParam.measure_seq[2] != SEQ_IDLE))
//         {
//             LED1_ON(false);
//             LED2_ON(true);
//         }
//         else
//         {
//             LED1_ON(true);
//             LED2_ON(false);
//         }
//     }
//     else
//     {
//         if (!g_tVar.ValueResultPass[2])
//         {
//             LED1_ON(false);
//             LED2_ON(true);
//         }
//         else
//         {
//             LED1_ON(false);
//             LED2_ON(false);
//         }
//     }
//     if (g_tParam.switch_to_next[2])
//     {
//         User_Meter.MeasureMode = getNextMeasureCh(QCurrentMeter);
// #if (DELAY_VALUE_RESULT)
//         user_meter_queue_clear();
// #endif
//         user_meter_current_filter_reset();
//         user_meter_measure_switch(User_Meter.MeasureMode);
//     }
}

/* 私有函数原型 ***********************************************************************/

/* ************************************ 文件结束 **************************************/
