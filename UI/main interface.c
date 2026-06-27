/**
 * @file       main interface.c
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-15
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
#include "user_config.h"
#include "interface_com.h"
#include "param.h"
#include "user_meter.h"
#include <stdio.h>
#include "user_menu.h"
#include "user_app.h"
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

/* 私有函数声明 ***********************************************************************/

/* 全局变量声明 ***********************************************************************/

UI_WINDOW_T Main_Window = {
    .Init = Main_Interface_Init,
    .Run  = Main_Interface,
    .Exit = NULL,
};

UI_WINDOW_T Menu_Window = {
    .Init = user_menu_init,
    .Run  = Menu_Task,
    .Exit = NULL,
};

/* 全局函数原型 ***********************************************************************/

/**
 * @brief 清除电压测试结果
 *
 */
void Clear_Voltage_Measure_Result(void)
{
    // GUI_SetBkColor(C_BLACK);
    // GUI_SetColor(C_WHITE);

    // GUI_SetFont(&GUI_FontDengXian_32);

    // if (g_tParam.measure_seq[0] != SEQ_IDLE)
    // {
    //     GUI_DispStringAt("0.000V", MENU_X_START_OFFSET + 15, MENU_Y_START_OFFSET + 15 + 38 * INDEX_VOLTAGE);
    // }
}


/**
 * @brief 清除电流测试结果
 *
 */

void Clear_Current_Measure_Result(void)
{
    // GUI_SetBkColor(C_BLACK);
    // GUI_SetColor(C_WHITE);

    // GUI_SetFont(&GUI_FontDengXian_32);

    // if (g_tParam.measure_seq[1] != SEQ_IDLE)
    // {
    //     GUI_DispStringAt("0.000A", MENU_X_START_OFFSET + 15, MENU_Y_START_OFFSET + 15 + 38 * INDEX_CURRENT);
    // }
}

/**
 * @brief 清除小电流测试结果
 *
 */
void Clear_QCurrent_Measure_Result(void)
{
    // GUI_SetBkColor(C_BLACK);
    // GUI_SetColor(C_WHITE);

    // GUI_SetFont(&GUI_FontDengXian_32);

    // if (g_tParam.measure_seq[2] != SEQ_IDLE)
    // {
    //     GUI_DispStringAt("000.0μA", MENU_X_START_OFFSET + 15, MENU_Y_START_OFFSET + 15 + 38 * INDEX_QCURRENT);
    // }
}

/**
 * @brief 清除直流电压测试结果
 *
 */
void Clear_DC_Voltage_Measure_Result(void)
{
    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);

    GUI_SetFont(&GUI_FontDengXian_24);

    GUI_DispStringAt("000.0V", VALUE_X_OFFSET+185, VALUE_Y_OFFSET);
    GUI_DispStringAt("0.000A", VALUE_X_OFFSET+185, VALUE_Y_OFFSET+30);
}


/**
 * @brief 清除交流电压测试结果
 *
 */
void Clear_AC_Voltage_Measure_Result(void)
{
    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);

    GUI_SetFont(&GUI_FontDengXian_24);
    GUI_DispStringAt("000.0V", VALUE_X_OFFSET+185, VALUE_Y_OFFSET+83);
    GUI_DispStringAt("0.000A", VALUE_X_OFFSET+185, VALUE_Y_OFFSET+83+29);

}

/**
 * @brief 电压跳变 > 50V 3s 后锁存 DC/AC 电压电流值
 *        覆盖右侧 DC 和 AC 清除显示区
 */
void Show_Snap_DC_Result(uint32_t dcVolt, uint32_t dcCurr)
{
    char buf[16];

    GUI_SetBkColor(C_BLACK);
    if(abs(dcVolt-1600) > 80 && abs(dcCurr-300) > 15)//正常电压范围152-168V，电流范围285-315mA
    {
        GUI_SetColor(C_GREEN);//实际显示红色       
    }
    else
    {
        GUI_SetColor(C_BLUE);//实际显示绿色 
    }
    GUI_SetFont(&GUI_FontDengXian_24);

    // 电压: xxx.x V
    sprintf(buf, "%03d.%1dV", (int)(dcVolt / 10), (int)(dcVolt % 10));
    GUI_DispStringAt(buf, VALUE_X_OFFSET + 185, VALUE_Y_OFFSET);
    // 电流: x.xxx A
    sprintf(buf, "%1d.%03dA", (int)(dcCurr / 1000), (int)(dcCurr % 1000));
    GUI_DispStringAt(buf, VALUE_X_OFFSET + 185, VALUE_Y_OFFSET + 30);
}

void Show_Snap_AC_Result(uint32_t acVolt, uint32_t acCurr)
{
    char buf[16];

    GUI_SetBkColor(C_BLACK);
    if(abs(acVolt-2200) >= 100)
    {
        GUI_SetColor(C_GREEN);//实际显示红色       
    }
    else
    {
        GUI_SetColor(C_BLUE);//实际显示绿色 
    }
    GUI_SetFont(&GUI_FontDengXian_24);

    // 电压: xxx.x V
    sprintf(buf, "%03d.%1dV", (int)(acVolt / 10), (int)(acVolt % 10));
    GUI_DispStringAt(buf, VALUE_X_OFFSET + 185, VALUE_Y_OFFSET + 83);
    // 电流: x.xxx A
    sprintf(buf, "%1d.%03dA", (int)(acCurr / 1000), (int)(acCurr % 1000));
    GUI_DispStringAt(buf, VALUE_X_OFFSET + 185, VALUE_Y_OFFSET + 83 + 29);
}

/**
 * @brief 表头测试模式显示
 *
 */
#include "bsp.h"


void Meter_Measure_VoltMode_Show(void)
{
    // char buf[12] = {0};
    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);

    // 显示表头单位
    GUI_SetFont(&GUI_FontDengXian_24);
    GUI_DispStringAt("V", UNIT_X_OFFSET, UNIT_Y_OFFSET);
    GUI_DispStringAt("A", UNIT_X_OFFSET, UNIT_Y_OFFSET+30);

    GUI_DispStringAt("V", UNIT_X_OFFSET, UNIT_Y_OFFSET+83);
    GUI_DispStringAt("A", UNIT_X_OFFSET, UNIT_Y_OFFSET+30+83);

    // GUI_DispStringAt("PF:", UNIT_X_OFFSET+25, UNIT_Y_OFFSET+18);
    // GUI_DispStringAt("PF:", UNIT_X_OFFSET+25, UNIT_Y_OFFSET+18+83);


    GUI_SetFont(&GUI_FontDengXian_22);
    GUI_DispStringAt("DC", 8, 10);
    GUI_DispStringAt("AC", 8, 94);

}

/**
 * @brief 表头测试结果初始化
 *
 */
void Meter_Measure_Result_Init(void)
{
    LCD_Fill(MENU_X_START_OFFSET, MENU_Y_START_OFFSET + 10, MENU_X_END_OFFSET, MENU_Y_END_OFFSET, C_BLACK);
    // 显示测试结果

    Clear_DC_Voltage_Measure_Result();
    Clear_AC_Voltage_Measure_Result();
}
/**
 * @brief 主界面初始化
 *
 */

void Main_Interface_Init(void)
{
    LCD_SetPower(1);

    LCD_FillScreen(C_BLACK);
    GUI_SetColor(C_WHITE);
    // 显示测试值
    Meter_Measure_Show();
    // 显示测试结果
    Meter_Measure_Result_Init();
    // 显示测试电压类型 DC/AC
    Meter_Measure_VoltMode_Show();
#if (VOLTAGE_CYCLE_TEST == 1)
    //	显示测试次数
    Meter_Measure_sum_Init();
#endif
    LCD_SetPower(1);
}

/**
 * @brief 主界面显示
 *
 */
void Main_Interface(void)
{
    Meter_Measure_Show();
}

/* 私有函数原型 ***********************************************************************/

/* ************************************ 文件结束 **************************************/
