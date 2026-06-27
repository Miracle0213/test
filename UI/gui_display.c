/**
 * @file       gui_display.c
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-03
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

#include "gui_display.h"
#include "interface_com.h"

/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

/* 私有函数声明 ***********************************************************************/

/* 全局变量声明 ***********************************************************************/

UI_MANAGER_T UIManager;

/* 全局函数原型 ***********************************************************************/

void UI_Init(void)
{
    Interface_Var_Init();

    UIManager.CurrentWindow = &Main_Window;
    UIManager.NextWindow    = NULL;
    UIManager.State         = UI_STATE_INIT;
}

// 运行UI管理器
void UI_Run(void)
{
    switch (UIManager.State)
    {
        case UI_STATE_INIT:
            if (UIManager.CurrentWindow && UIManager.CurrentWindow->Init)
            {
                UIManager.CurrentWindow->Init();
            }
            UIManager.State = UI_STATE_ANIMATION_ENTER;
            break;

        case UI_STATE_ANIMATION_ENTER:
            UIManager.State = UI_STATE_RUN;
            break;

        case UI_STATE_RUN:
            if (UIManager.CurrentWindow && UIManager.CurrentWindow->Run)
            {
                UIManager.CurrentWindow->Run();
            }
            if (UIManager.NextWindow)
            {
                UIManager.State = UI_STATE_ANIMATION_EXIT;
            }
            break;

        case UI_STATE_ANIMATION_EXIT:
            if (UIManager.CurrentWindow && UIManager.CurrentWindow->Exit)
            {
                UIManager.CurrentWindow->Exit();
            }
            UIManager.CurrentWindow = UIManager.NextWindow;
            UIManager.NextWindow    = NULL;
            UIManager.State         = UI_STATE_INIT;
            break;

        case UI_STATE_EXIT:
            // 处理退出逻辑
            break;

        default:
            break;
    }
}

// 切换窗口
void UI_SwitchWindow(UI_WINDOW_T* newWindow)
{
    UIManager.NextWindow = newWindow;
}

/* 私有函数原型 ***********************************************************************/

/* ************************************ 文件结束 **************************************/
