/**
 * @file       menu_interface.c
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
#include "user_calib.h"
#include "user_ht7017.h"
#include "bsp.h"
#include "lcd_driver.h"
#include "menu.h"
#include "param.h"
#include <stdio.h>
#include "user_meter.h"
#include "user_config.h"

#include "gui_display.h"
#include "interface_com.h"
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

static float DcValueDiff[3][4] = {
    0.0001f, 0.0002f, 0.0005f, 0.001f,

    0.001f,  0.002f,  0.005f,  0.01f,

    0.001f,  0.002f,  0.005f,  0.01f,
};

static float AcValueDiff[3][4] = {
    0.0001f, 0.0002f, 0.0005f, 0.001f,

    0.001f,  0.002f,  0.005f,  0.01f,

    0.001f,  0.002f,  0.005f,  0.01f,
};

static char    strValue[32] = {0};
static int32_t iLastTime    = 0;

/* 私有函数声明 ***********************************************************************/
static uint8_t last_idx = 0;
static void    OnAddValue(float diff);
static void    OnSubtractValue(float diff);
static void    OnKeyAutoUp(uint8_t add);
static void    ShowFloatValue(float value);
static void    calib_manual_show(MenuShow_t *ptShowInfo);

/* 全局变量声明 ***********************************************************************/
MenuList_t sg_CalibManualMenuTable[] = {
    {"返回", NULL, NULL, NULL, NULL, NULL},
};

/* 全局函数原型 ***********************************************************************/

/**
 * @brief 零点校准
 *
 */
void user_calib_zero(void)
{
    if (g_tParam.DcValueOffset[User_Meter.MeasureMode] != 1.0f)
    {
        g_tParam.DcValueOffset[User_Meter.MeasureMode] = 1.0f;
        SaveParam();
    }

    LCD_Fill(MENU_X_START_OFFSET, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, C_BLACK);
    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);

    GUI_SetFont(&GUI_FontDengXian_22);
    GUI_DispStringAt("零点校准", MENU_X_START_OFFSET, MENU_Y_START_OFFSET + 5);
    GUI_DispStringAt("校准中...", MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX);

    switch (User_Meter.MeasureMode)
    {
        case VoltageMeter:
        {
            if (g_tParam.VoltageMode == 0)
            {
                user_ht7017_calib_dc_voltage(HT7017_INDEX_01, HT7017_CH_V, MEAS_VOLT_CH1, 0.0f);
            }
            else
            {
                user_ht7017_calib_ac_voltage(HT7017_INDEX_01, HT7017_CH_V, MEAS_VOLT_CH1, 0.0f);
            }
            break;
        }
        case CurrentMeter:
        {
            if (g_tParam.VoltageMode == 0)
            {
                user_ht7017_calib_dc_voltage(HT7017_INDEX_01, HT7017_CH_I1, MEAS_CURR_CH1, 0.0001f);
            }
            else
            {
               user_ht7017_calib_ac_voltage(HT7017_INDEX_01, HT7017_CH_I1, MEAS_CURR_CH1, 0.0001f);
            }
            break;
        }
        case ACPowerMeter:
        {
            user_ht7017_calib_ac_power(HT7017_INDEX_01, 0.0f);
            break;
        }
        default:
            break;
    }

    Menu_Exit(true);
}
 uint8_t test_res = 0;
/**
 * @brief 参考校准
 *
 */
void user_calib_ref(void)
{
    if (g_tParam.DcValueOffset[User_Meter.MeasureMode] != 1.0f)
    {
        g_tParam.DcValueOffset[User_Meter.MeasureMode] = 1.0f;
        SaveParam();
    }

    LCD_Fill(MENU_X_START_OFFSET, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, C_BLACK);
    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);

    GUI_SetFont(&GUI_FontDengXian_22);
    GUI_DispStringAt("参考校准", MENU_X_START_OFFSET, MENU_Y_START_OFFSET + 5);
    GUI_DispStringAt("校准中...", MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX);
    switch (User_Meter.MeasureMode)
    {
        case VoltageMeter:
        {
            if (g_tParam.VoltageMode == 0)
            {
                user_ht7017_calib_dc_voltage(HT7017_INDEX_01, HT7017_CH_V, MEAS_VOLT_CH1, 100.0f);//以100V为参考校准
            }
            else
            {
                 user_ht7017_calib_ac_voltage(HT7017_INDEX_01, HT7017_CH_V, MEAS_VOLT_CH1, 220.0f);//以220V为参考校准
            }
            break;
        }
        case CurrentMeter:
        {

            if (g_tParam.VoltageMode == 0)
            {
                user_ht7017_calib_dc_voltage(HT7017_INDEX_01, HT7017_CH_I1, MEAS_CURR_CH1, 1.5f);//以1.5A为参考校准
            }
            else
            {
                user_ht7017_calib_ac_voltage(HT7017_INDEX_01, HT7017_CH_I1, MEAS_CURR_CH1, 0.45f);//以0.4A为参考校准
            }


            break;
        }
        case ACPowerMeter:
        {
            user_ht7017_calib_ac_power(HT7017_INDEX_01, 29.0f);
            break;
        }
        default:
            break;
    }

    Menu_Exit(true);
}

/**
 * @brief 手动校准初始化
 *
 */
void user_calib_manual_init(void)
{
    bsp_SetKeyParam(KID_K2, KEY_LONG_TIME, 1); /* 600ms ???,??30ms??1?(??20?) */
    bsp_SetKeyParam(KID_K3, KEY_LONG_TIME, 1); /* 600ms ???,??30ms??1?(??20?) */

    Menu_Bind(sg_CalibManualMenuTable, GET_MENU_NUM(sg_CalibManualMenuTable), calib_manual_show);

    /* 绘制前关闭背光,避免绘制过程中旧内容残留可见 */
    LCD_SetPower(0);

    /* 清除范围从 Y=0 开始,与主菜单 user_menu_set_load 保持一致,彻底擦除上一级菜单残留 */
    LCD_Fill(MENU_X_START_OFFSET, 0, LCD_WIDTH - 1, MENU_Y_END_OFFSET, C_BLACK);

    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);

    GUI_SetFont(&GUI_FontDengXian_22);

    // 绘制子菜单
    GUI_DispStringAt("手动校准", MENU_X_START_OFFSET, MENU_Y_START_OFFSET + 5);

    for (int i = 0; i < GET_MENU_NUM(sg_CalibManualMenuTable); ++i)
    {
        GUI_DispStringAt(sg_CalibManualMenuTable[i].pszDesc, MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX + i * MENU_ITEM_SPACING_PX);
    }
    GUI_DispStringAt(">", MENU_X_START_OFFSET, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX);

    if(!g_tParam.VoltageMode)
    {
        ShowFloatValue(g_tParam.DcValueOffset[User_Meter.MeasureMode]);
    }
    else 
    {
        ShowFloatValue(g_tParam.AcValueOffset[User_Meter.MeasureMode]);
    }

    last_idx = 0;

    LCD_SetPower(1);   // 绘制完成后打开背光
}

/**
 * @brief 手动校准任务处理函数
 *
 */
void user_calib_manual_task(void)
{
    uint8_t ucKeyCode; /* ???? */
    ucKeyCode = bsp_GetKey();

    if ((ucKeyCode != KEY_NONE))
    {
        switch (ucKeyCode)
        {
            case KEY_1_UP:
            {
                Menu_Exit(true);
            }
            break;
            case KEY_2_UP:
            {
                if(!g_tParam.VoltageMode)
                {
                    OnAddValue(DcValueDiff[User_Meter.MeasureMode][0]);
                }
                else
                {
                    OnAddValue(AcValueDiff[User_Meter.MeasureMode][0]);
                }
            }
            break;
            case KEY_3_UP:
            {
                if(!g_tParam.VoltageMode)
                {
                    OnAddValue(-DcValueDiff[User_Meter.MeasureMode][0]);
                }
                else
                {
                    OnAddValue(-AcValueDiff[User_Meter.MeasureMode][0]);
                }
            }
            break;
            case KEY_2_LONG_DOWN:
            case KEY_3_LONG_DOWN:
            {
                iLastTime = bsp_GetRunTime();
            }
            break;

            case KEY_2_AUTO_UP:
            {
                OnKeyAutoUp(0);
            }
            break;
            case KEY_3_AUTO_UP:
            {
                OnKeyAutoUp(1);
            }
            break;

            default:
                break;
        }
    }
}

/**
 * @brief 返回主菜单
 *
 */
void user_calib_manual_exit(void)
{
    bsp_SetKeyParam(KID_K2, KEY_LONG_TIME, 0);  // ????
    bsp_SetKeyParam(KID_K3, KEY_LONG_TIME, 0);  // ????

    SaveParam();
}
/* 私有函数原型 ***********************************************************************/
/**
 * @brief 显示手动校准菜单
 *
 * @param  ptShowInfo
 */
static void calib_manual_show(MenuShow_t *ptShowInfo)
{
    uint8_t idx = ptShowInfo->selectItem;

    if (last_idx != idx)
    {
        int16_t y_old = MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX + last_idx * MENU_ITEM_SPACING_PX;
        int16_t y_new = MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX + idx * MENU_ITEM_SPACING_PX;

        /* 用背景色擦除旧光标区域,避免单空格宽度不足导致残留 */
        LCD_Fill(MENU_X_START_OFFSET, y_old, MENU_X_START_OFFSET + 13, y_old + 21, C_BLACK);

        /* 在新位置绘制光标 */
        GUI_SetBkColor(C_BLACK);
        GUI_SetColor(C_WHITE);
        GUI_SetFont(&GUI_FontDengXian_22);
        GUI_DispStringAt(">", MENU_X_START_OFFSET, y_new);

        last_idx = idx;
    }
}

/**
 * @brief 显示浮点数
 *
 * @param  value 手动校准偏移量
 */
static void ShowFloatValue(float value)
{
    if (value >= 0.0f)
    {
        sprintf(strValue, "×%05.4f", value);
    }
    else
    {
        sprintf(strValue, "%05.4f", value);
    }
    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);

    GUI_SetFont(&GUI_FontDengXian_22);

    GUI_DispStringAt(strValue, MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX + 3 * MENU_ITEM_SPACING_PX);
}

/**
 * @brief 加值
 *
 * @param  diff
 */
static void OnAddValue(float diff)
{
    if(!g_tParam.VoltageMode)
    {
        g_tParam.DcValueOffset[User_Meter.MeasureMode] += diff;
        ShowFloatValue(g_tParam.DcValueOffset[User_Meter.MeasureMode]);
    }
    else 
    {
        g_tParam.AcValueOffset[User_Meter.MeasureMode] += diff;
        ShowFloatValue(g_tParam.AcValueOffset[User_Meter.MeasureMode]);
    }
    
}
/**
 * @brief 减值
 *
 * @param  diff
 */
static void OnSubtractValue(float diff)
{
    if(!g_tParam.VoltageMode)
    {
        g_tParam.DcValueOffset[User_Meter.MeasureMode] -= diff;
        ShowFloatValue(g_tParam.DcValueOffset[User_Meter.MeasureMode]);
    }
    else 
    {
        g_tParam.AcValueOffset[User_Meter.MeasureMode] -= diff;
        ShowFloatValue(g_tParam.AcValueOffset[User_Meter.MeasureMode]);
    }
    
}
/**
 * @brief 按键控制数值加或减以及加减的速度
 *
 * @param  add
 */
static void OnKeyAutoUp(uint8_t add)
{
    float step = 0.0f;
    if (bsp_CheckRunTime(iLastTime) < 2000)
    {
        step = DcValueDiff[User_Meter.MeasureMode][1];
    }
    else if (bsp_CheckRunTime(iLastTime) < 5000)
    {
        step = DcValueDiff[User_Meter.MeasureMode][2];
    }
    else
    {
        step = DcValueDiff[User_Meter.MeasureMode][3];
    }

    OnAddValue((add == 0) ? step : -step);
}

/* ************************************ 文件结束 **************************************/
