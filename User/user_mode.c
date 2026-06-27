/**
 * @file       user_mode.c
 * @author     huxiaodan (huxiaodan@stw-ecig.com)
 * @date       2025-08-21
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
#include "user_mode.h"
#include "menu.h"
#include "bsp.h"
#include "lcd_driver.h"
#include "gui_display.h"
#include "param.h"
#include <stdio.h>
#include "interface_com.h"
#include "user_config.h"
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

/* 私有函数声明 ***********************************************************************/
static void mode_select_show(MenuShow_t *ptShowInfo);


/* 全局变量声明 ***********************************************************************/
MenuList_t sg_modeSelectTable[] = {
    {"返回", NULL, NULL, NULL, NULL, NULL},
};
const char *seq_text[8] = {
    "SEQ_A", "SEQ_B", "SEQ_C", "SEQ_D", "SEQ_E",  "NULL",
};


/* 全局函数原型 ***********************************************************************/

/**
 * @brief 模式选择菜单显示函数(静态光标,仅占位防止父菜单show函数残留)
 *
 * @param ptShowInfo 菜单显示信息
 */
static void mode_select_show(MenuShow_t *ptShowInfo)
{
    (void)ptShowInfo;
    /* 模式选择界面光标是静态的,不需要动画。
       必须提供此函数而非 NULL,否则 Menu_Bind 不会清除从父菜单继承的 user_menu_show,
       导致主菜单的 show 函数继续在此子菜单中运行,用残留的 last_idx 造成显示错乱。 */
}

/**
 * @brief 模式选择初始化
 *
 */
void user_select_mode_init(void)
{
    uint8_t index;
    char    buf[30];

    /* 必须传入合法的 show 函数而非 NULL,否则父菜单的 user_menu_show 会继续运行,
       用其残留的 last_idx 造成光标/显示错乱 */
    Menu_Bind(sg_modeSelectTable, GET_MENU_NUM(sg_modeSelectTable), mode_select_show);

    /* 绘制前关闭背光,避免绘制过程中旧内容残留可见 */
    LCD_SetPower(0);

    /* 清除范围从 Y=0 开始,与主菜单保持一致,彻底擦除上一级菜单残留 */
    LCD_Fill(MENU_X_START_OFFSET, 0, LCD_WIDTH - 1, MENU_Y_END_OFFSET, C_BLACK);
    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);
    GUI_SetFont(&GUI_FontDengXian_22);

    for (int i = 0; i < GET_MENU_NUM(sg_modeSelectTable); ++i)
    {
        GUI_DispStringAt(sg_modeSelectTable[i].pszDesc, MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX + i * MENU_ITEM_SPACING_PX);
    }
    GUI_DispStringAt(">", MENU_X_START_OFFSET, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX);

    GUI_DispStringAt("模式选择", MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX * 2);
    if (g_tParam.MODE[0] >= MODE_SUM)
    {
        g_tParam.MODE[0] = MODE_NULL;
    }
    index = g_tParam.MODE[0];
    sprintf(buf, "--%s  ", seq_text[index]);
    GUI_DispStringAt(buf, MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX * 3);

    /* 绘制完成后打开背光 */
    LCD_SetPower(1);
}
/**
 * @brief 模式选择任务处理函数
 * 
 */
void user_select_mode_task(void)
{
    uint8_t index;
    char    buf[30];
    uint8_t ucKeyCode; /* ???? */

    static uint8_t last_mode[2] = {0, 0};

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
                g_tParam.MODE[0]++;
                g_tParam.MODE[0] %= MODE_SUM;
            }
            break;
            case KEY_3_UP:
            {
                g_tParam.MODE[0] = (g_tParam.MODE[0] == 0) ? (MODE_SUM - 1) : (g_tParam.MODE[0] - 1);
            }
            break;

            default:
                break;
        }
        GUI_SetBkColor(C_BLACK);
        GUI_SetColor(C_WHITE);

        GUI_SetFont(&GUI_FontDengXian_22);
    }
    if (last_mode[0] != g_tParam.MODE[0])
    {
        last_mode[0] = g_tParam.MODE[0];
        if (g_tParam.MODE[0] >= MODE_SUM)
        {
            g_tParam.MODE[0] = MODE_NULL;
        }
        index        = g_tParam.MODE[0];
        sprintf(buf, "--%s  ", seq_text[index]);
        GUI_DispStringAt(buf, MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX * 3);
    }
}
/**
 * @brief 退出模式选择界面
 * 
 */
void user_select_mode_exit(void)
{   
		LoadModeParam();
	  SaveParam();
}

/* 私有函数原型 ***********************************************************************/

/* ************************************ 文件结束 **************************************/
