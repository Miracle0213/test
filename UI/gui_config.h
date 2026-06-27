/**
 * @file       gui_config.h
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
 * ???? � ???????????????????
 * ************************************************************************
 */
#if 1  // ?????

#ifndef __GUI_CONFIG_H
#define __GUI_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ????? *************************************************************************/

#include "GUI_XBF_Font.h"
//#include "user_config.h"

/* ????? *************************************************************************/

#ifndef GUI_CONST_STORAGE
#define GUI_CONST_STORAGE const
#endif
#define s64 int64_t
#define s32 int32_t
#define s16 int16_t
#define s8  int8_t

#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8  uint8_t

#define GUI_RESULT_FAIL -1
#define GUI_RESULT_OK   0

/* ??????? *********************************************************************/

/* ?????? ***********************************************************************/

/* ?????? ***********************************************************************/

extern GUI_CONST_STORAGE GUI_FONT GUI_FontDengXian_24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontDengXian_48;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontDengXian_32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontDengXian_22;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontDengXian_14;
/* ?????? ***********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __GUI_CONFIG_H */

#endif
/* ************************************* ???? *************************************/
