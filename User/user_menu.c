/**
 * @file       user_menu.c
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-11
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

#include "user_menu.h"
#include <stdint.h>
#include "bsp_key.h"
#include "bsp.h"
#include "lcd_driver.h"
#include "gui_display.h"
#include "interface_com.h"
#include "user_calib.h"
#include "user_mode.h"
#include "user_config.h"
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/
static uint8_t last_idx = 0;
MainMenuCfg_t  tMainMenu;
/* 主菜单 */

/* 私有函数声明 ***********************************************************************/
static void user_menu_set_load(void);
static void user_menu_set_task(void);
static void user_menu_exit(void);
static void user_menu_show(MenuShow_t *ptShowInfo);

/* 全局变量声明 ***********************************************************************/
MenuList_t sg_MainMenuTable[] = {
    {"返回", NULL, NULL, NULL, NULL, NULL},
    {"手动校准", NULL, user_calib_manual_init, user_calib_manual_exit, user_calib_manual_task, NULL},
    {"零点校准", NULL, user_calib_zero, NULL, NULL, NULL},
    {"参考校准", NULL, user_calib_ref, NULL, NULL, NULL},
		{"模式选择", NULL, user_select_mode_init, user_select_mode_exit, user_select_mode_task, NULL},

};
/* 全局函数原型 ***********************************************************************/

void user_menu_param_init(void)
{
    tMainMenu.pszDesc        = "设置";
    tMainMenu.pszEnDesc      = NULL;
    tMainMenu.pfnLoadCallFun = user_menu_set_load;
    tMainMenu.pfnRunCallFun  = user_menu_set_task;
}

void user_menu_init(void)
{
    Menu_Init(&tMainMenu);
}

/* 私有函数原型 ***********************************************************************/
/**
 * @brief 加载菜单设置界面
 *
 */
static void user_menu_set_load(void)
{
    Menu_Bind(sg_MainMenuTable, GET_MENU_NUM(sg_MainMenuTable), user_menu_show);
    LCD_SetPower(0);

    LCD_Fill(MENU_X_START_OFFSET, 0, LCD_WIDTH - 1, MENU_Y_END_OFFSET, C_BLACK);
    Meter_Measure_Show();

    GUI_SetBkColor(C_BLACK);
    GUI_SetColor(C_WHITE);

    GUI_SetFont(&GUI_FontDengXian_22);
    GUI_DispStringAt("设置", MENU_X_START_OFFSET, MENU_Y_START_OFFSET + 5);

    for (int i = 0; i < GET_MENU_NUM(sg_MainMenuTable); ++i)
    {
        GUI_DispStringAt(sg_MainMenuTable[i].pszDesc, MENU_X_START_OFFSET + 20, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX + i * MENU_ITEM_SPACING_PX);
    }
    GUI_DispStringAt(">", MENU_X_START_OFFSET, MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX);
    LCD_SetPower(1);

    last_idx = 0;
}
/**
 * @brief 菜单设置界面任务处理函数
 *
 */
static void user_menu_set_task(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    Meter_Measure_Show();
    ucKeyCode = bsp_GetKey();

    if ((ucKeyCode != KEY_NONE))
    {
        switch (ucKeyCode)
        {
            case KEY_1_UP:
            {
                if (Menu_QuerySelectItem() == 0)
                {
                    user_menu_exit();
                    UI_Init();
                }
                else
                {
                    // 禁用按钮零点校准和参考校准，防止产线使用过程中误触
#if (!ALLOW_CALIB)
                    if (Menu_QuerySelectItem() == 2 || Menu_QuerySelectItem() == 3)
                    {
                        break;
                    }
#endif
#if (!ALLOW_MANNUAL_CALIB)
                    if (Menu_QuerySelectItem() == 1)
                    {
                        break;
                    }
#endif
                    Menu_Enter();
                }
            }
            break;
            case KEY_2_UP:
            {
                Menu_SelectNext(true);
            }
            break;
            case KEY_3_UP:
            {
                Menu_SelectPrevious(true);
            }
            break;
            default:
                break;
        }
    }
}
/**
 * @brief 菜单界面显示
 *
 */
static void user_menu_show(MenuShow_t *ptShowInfo)
{
    uint8_t idx = ptShowInfo->selectItem;

    if (last_idx != idx)
    {
        int16_t y_old = MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX + last_idx * MENU_ITEM_SPACING_PX;
        int16_t y_new = MENU_Y_START_OFFSET + 5 + MENU_ITEM_SPACING_PX + idx * MENU_ITEM_SPACING_PX;

        /* 用背景色擦除旧光标区域 (14px宽 = > 字符的XDist, 22px高 = 字体YDist) */
        /* 避免用空格字符擦除因字体宽度不匹配导致的残留 */
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
 * @brief 退出菜单界面
 *
 */
static void user_menu_exit(void)
{
    Menu_DeInit();
    Meter_Measure_Result_Init();
}

/* ************************************ 文件结束 **************************************/
