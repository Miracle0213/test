/**
 * @file       gui_display.h
 * @author     ??? (weijiabao@stw-ecig.cn)
 * @date       2025-01-03
 * @brief      ??????????
 * @version    v1.0
 *
 * ************************************************************************
 * @attention
 * ????     ???       ??       ????       ????
 *
 * ************************************************************************
 * ???? © ???????????????????
 * ************************************************************************
 */
#if 1  // ?????

#ifndef __GUI_DISPLAY_H
#define __GUI_DISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ????? *************************************************************************/

#include "gui.h"

/* ????? *************************************************************************/

/* ??????? *********************************************************************/

/* ?????? ***********************************************************************/

// ??????
typedef enum
{
    UI_STATE_NULL,
    UI_STATE_INIT,
    UI_STATE_ANIMATION_ENTER,
    UI_STATE_RUN,
    UI_STATE_ANIMATION_EXIT,
    UI_STATE_EXIT,
} UI_STATE_E;

// ?????
typedef struct
{
    void (*Init)(void);  // ?????
    void (*Run)(void);   // ????
    void (*Exit)(void);  // ????
} UI_WINDOW_T;

// ?????
typedef struct
{
    UI_WINDOW_T* CurrentWindow;  // ????
    UI_WINDOW_T* NextWindow;     // ?????
    UI_STATE_E   State;          // ????
} UI_MANAGER_T;

/* ?????? ***********************************************************************/

extern UI_MANAGER_T UIManager;

/* ?????? ***********************************************************************/

extern void UI_Init(void);
extern void UI_Run(void);
extern void UI_SwitchWindow(UI_WINDOW_T* newWindow);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_DISPLAY_H */

#endif
/* ************************************* ???? *************************************/
