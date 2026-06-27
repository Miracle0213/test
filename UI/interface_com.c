/**
 * @file       interface_com.c
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

#include "interface_com.h"
#include "bsp_ht7017.h"
#include "bsp.h"
#include "user_meter.h"
#include "user_ht7017.h"
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

/* 私有函数声明 ***********************************************************************/

/* 全局变量声明 ***********************************************************************/

Interface_Var_T If_Var = {0};
extern User_Meter_T User_Meter;

/* 全局函数原型 ***********************************************************************/

void Interface_Var_Init(void)
{
    If_Var.MeasureMode_Set   = 0;
    If_Var.MeasureMode       = 0xff;
    If_Var.VoltMode_Set      = 0;
    If_Var.VoltMode          = 0xff;
    If_Var.MeasureValue1_Set = 0;
    If_Var.MeasureValue1     = 0xffff;
    If_Var.MeasureValue2_Set = 0;
    If_Var.MeasureValue2     = 0xffff;
    If_Var.MeasureValue3_Set = 0;
    If_Var.MeasureValue3     = 0xffff;
    If_Var.MeasureValue4_Set = 0;
    If_Var.MeasureValue4     = 0xffff;
    If_Var.MeasureValue5_Set = 0;
    If_Var.MeasureValue5     = 0xffff;
}


extern uint8_t test_res;
/**
 * @brief UI界面显示表头测量值及单位
 * @note 暂时只支持单表头
 */
void Meter_Measure_Show(void)
{

    //设置校准模式
    if (If_Var.VoltMode_Set != If_Var.VoltMode||If_Var.MeasureMode_Set != If_Var.MeasureMode)
    {
        If_Var.VoltMode = If_Var.VoltMode_Set;
        If_Var.MeasureMode = If_Var.MeasureMode_Set;

        GUI_SetBkColor(C_BLACK); 
        GUI_SetColor(C_WHITE);
        GUI_SetFont(&GUI_FontDengXian_24);
        if (If_Var.VoltMode == 0)
        {	
            GUI_DispStringAt("(D)", UNIT_X_OFFSET + 25, UNIT_Y_OFFSET + 14);		
        }
        else
        {
            GUI_DispStringAt("(C)", UNIT_X_OFFSET + 25, UNIT_Y_OFFSET + 14);
        }
        
        
        GUI_SetFont(&GUI_FontDengXian_24);

        switch (If_Var.MeasureMode)
        {
        case VoltageMeter:
            GUI_DispStringAt("(V)", UNIT_X_OFFSET + 25, UNIT_Y_OFFSET + 14 + 83);
            break;
        case CurrentMeter:
            GUI_DispStringAt("(A)", UNIT_X_OFFSET + 25, UNIT_Y_OFFSET + 14 + 83);
            break;
        case ACPowerMeter:
            GUI_DispStringAt("(P)", UNIT_X_OFFSET + 25, UNIT_Y_OFFSET + 14 + 83);
            break;
        default:
            break;
        }
    }

    if (If_Var.MeasureValue1_Set != If_Var.MeasureValue1||If_Var.MeasureValue2_Set != If_Var.MeasureValue2)
    {
        If_Var.MeasureValue1 = If_Var.MeasureValue1_Set;
        If_Var.MeasureValue2 = If_Var.MeasureValue2_Set;
        GUI_SetBkColor(C_BLACK);
        GUI_SetColor(C_WHITE);

        // 显示表头测量值
        GUI_SetFont(&GUI_FontDengXian_24);

        GUI_DispDecShift(If_Var.MeasureValue1, VALUE_X_OFFSET, VALUE_Y_OFFSET, 5, 1);
        GUI_DispDecShift(If_Var.MeasureValue2, VALUE_X_OFFSET, VALUE_Y_OFFSET+30, 5, 3);

            /********************************************************测试代码*******************************************/
            // int32_t DC_V = 0;
            // int32_t DC_I1 = 0;
            //   uint8_t res = HT7017_GetPower_P(&g_tHT7017[HT7017_INDEX_01], &DC_V);
            // uint8_t res = HT7017_GetDcCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_V, &DC_V);
            // // //HT7017_GetDcCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_V, &User_Meter.DC_V);
            //  uint8_t res = HT7017_GetRmsCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_V, &DC_V);
            // uint8_t res = HT7017_GetRmsCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_I1, &DC_I1);
            // // // HAL_Delay(5);
            // // //uint8_t res = HT7017_GetDcCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_I1, &DC_I1);
        // GUI_DispDecShift(test_res, VALUE_X_OFFSET, VALUE_Y_OFFSET+30, 3, 1);
            // GUI_DispDecShift(g_tCalib.AcPower[0].x2, VALUE_X_OFFSET, VALUE_Y_OFFSET, 10, 1);
            // GUI_DispDecShift(g_tCalib.AcPower[0].y2, VALUE_X_OFFSET, VALUE_Y_OFFSET+30, 10, 1);
            //   GUI_DispDecShift(DC_V, VALUE_X_OFFSET, VALUE_Y_OFFSET+83, 10, 1);
               //GUI_DispDecShift(DC_I1, VALUE_X_OFFSET, VALUE_Y_OFFSET+30, 10, 1);
            //    GUI_DispDecShift(res, VALUE_X_OFFSET+40, VALUE_Y_OFFSET+60, 3, 1);

            //  GUI_DispDecShift(g_tCalib.AcVolt[User_Meter.MeasureMode].x2, VALUE_X_OFFSET, VALUE_Y_OFFSET, 10, 1);
            //  GUI_DispDecShift(g_tCalib.AcVolt[User_Meter.MeasureMode].y2, VALUE_X_OFFSET, VALUE_Y_OFFSET+60, 10, 1);
            /***********************************************************************************************************/
        char buf[5] = {0};
        GUI_SetFont(&GUI_FontDengXian_14);
        //GUI_DispStringAt("P:",60,10);
        sprintf(buf, "P:%d.%dW", If_Var.MeasureValue1*If_Var.MeasureValue2/10000,If_Var.MeasureValue1*If_Var.MeasureValue2/1000%10);
        LCD_Fill(90,10,170,30, C_BLACK);
        GUI_DispStringAt(buf,90,10);
        

        

    }
    if(If_Var.MeasureValue3_Set != If_Var.MeasureValue3||If_Var.MeasureValue4_Set != If_Var.MeasureValue4\
        ||If_Var.MeasureValue5_Set != If_Var.MeasureValue5)
    {
        If_Var.MeasureValue3 = If_Var.MeasureValue3_Set;
        If_Var.MeasureValue4 = If_Var.MeasureValue4_Set;
        If_Var.MeasureValue5 = If_Var.MeasureValue5_Set;
        //显示AC测量值
        GUI_SetBkColor(C_BLACK);
        GUI_SetColor(C_WHITE);
        GUI_SetFont(&GUI_FontDengXian_24);
        GUI_DispDecShift(If_Var.MeasureValue3, VALUE_X_OFFSET, VALUE_Y_OFFSET+83, 5, 1);
        GUI_DispDecShift(If_Var.MeasureValue4, VALUE_X_OFFSET, VALUE_Y_OFFSET+32+80, 5, 3);

        int AC_Power = If_Var.MeasureValue3*If_Var.MeasureValue4;
        int PF = 0;
        if(AC_Power != 0)
        {
            PF = If_Var.MeasureValue5*250000/AC_Power;
        }
        else
        {
            PF = 0;
        }
        if(PF > 100)
        {
            PF = 0;
        }
        char buf[5] = {0};
        GUI_SetFont(&GUI_FontDengXian_14);
        //GUI_DispStringAt("PF:",60,94);
        sprintf(buf, "PF:%d%%", PF);
        LCD_Fill(90,94,170,114, C_BLACK);
        GUI_DispStringAt(buf,90,94);

    }
}

/* 私有函数原型 ***********************************************************************/

/* ************************************ 文件结束 **************************************/
