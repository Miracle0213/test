/**
 * @file       interface_com.h
 * @author     ??? (weijiabao@stw-ecig.cn)
 * @date       2025-01-15
 * @brief      ??????????
 * @version    v1.0
 *
 * ************************************************************************
 * @attention
 * ????     ???       ??       ????       ????
 *
 * ************************************************************************
 * ???? � ???????????????????
 * ************************************************************************
 */
#if 1  // ?????

#ifndef __INTERFACE_COM_H
#define __INTERFACE_COM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ????? *************************************************************************/

#include "gui_display.h"
#include "user_com.h"
#include "menu.h"
#include "param.h"
/* ????? *************************************************************************/
#define INDEX_VOLTAGE  g_tParam.measure_seq[0]
#define INDEX_CURRENT  g_tParam.measure_seq[1]
#define INDEX_QCURRENT g_tParam.measure_seq[2]

#define VALUE_X_OFFSET (10)
#define VALUE_Y_OFFSET (30)

#define UNIT_X_OFFSET (100)
#define UNIT_Y_OFFSET (30)

#define RESULT_X_OFFSET (150)
#define RESULT_Y_OFFSET (5 + 25)

#define MENU_X_START_OFFSET (180)
#define MENU_Y_START_OFFSET (3)
#define MENU_ITEM_SPACING_PX (26)

#define MENU_X_END_OFFSET (320)
#define MENU_Y_END_OFFSET (180)

#define TESTCNT_X_OFFSET (192)
#define TESTCNT_Y_OFFSET (4)

#define DEFAULT_TEXT_COLOR C_WHITE

/* ??????? *********************************************************************/

/* ?????? ***********************************************************************/

typedef struct
{
    uint8_t MeasureMode_Set;
    uint8_t MeasureMode;

    uint8_t VoltMode_Set;
    uint8_t VoltMode;

    uint32_t MeasureValue1_Set;
    uint32_t MeasureValue2_Set;

    uint32_t MeasureValue3_Set;
    uint32_t MeasureValue4_Set;
    uint32_t MeasureValue5_Set;

    uint32_t MeasureValue1;
    uint32_t MeasureValue2;

    uint32_t MeasureValue3;
    uint32_t MeasureValue4;
    uint32_t MeasureValue5;

    uint8_t Is_Menu;
} Interface_Var_T;

/* ?????? ***********************************************************************/

extern Interface_Var_T If_Var;

extern UI_WINDOW_T Main_Window;
extern UI_WINDOW_T Menu_Window;

/* ?????? ***********************************************************************/

extern void Interface_Var_Init(void);

extern void Meter_Measure_Show(void);

extern void Main_Interface_Init(void);
extern void Main_Interface(void);
extern void Meter_Measure_Result_Init(void);

extern void Clear_Voltage_Measure_Result();
extern void Clear_Current_Measure_Result();
extern void Clear_QCurrent_Measure_Result();

extern void Clear_DC_Voltage_Measure_Result();
extern void Clear_AC_Voltage_Measure_Result();

extern void Meter_Measure_sum_Init(void);
extern void Meter_Measure_sum_Show(void);

extern void Show_Snap_DC_Result(uint32_t dcVolt, uint32_t dcCurr);
extern void Show_Snap_AC_Result(uint32_t acVolt, uint32_t acCurr);

#ifdef __cplusplus
}
#endif

#endif /* __INTERFACE_COM_H */

#endif
/* ************************************* ???? *************************************/
