/**
 * @file       gui_draw_bitmap.h
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
#if 1  // 全局宏开关

#ifndef __GUI_DRAW_H
#define __GUI_DRAW_H

#ifdef __cplusplus
extern "C"
{
#endif

/* 头文件包含 *************************************************************************/

// #include "lcd_driver.h"
#include "GUI_XBF_Font.h"

/* 全局宏定义 *************************************************************************/

typedef uint32_t GUI_COLOR;
/* 全局宏函数定义 *********************************************************************/

// #define BITMAP_16Bit

/* 全局类型定义 ***********************************************************************/

/**
 * @brief GUI调色板结构体
 *
 * 该结构体定义了一个GUI调色板，包括调色板条目数量、是否有透明色以及调色板条目的指针。
 */
typedef struct
{
    uint8_t         NumEntries;  /**< 调色板条目数量 */
    int8_t          HasTrans;    /**< 是否有透明色 */
    const uint32_t* pPalEntries; /**< 调色板条目指针 */
} GUI_LOGPALETTE;

/**
 * @brief GUI位图结构体
 *
 * 该结构体定义了一个GUI位图，包括位图的宽度、高度、每行字节数、每像素位数、位图数据指针以及调色板指针。
 */
typedef struct
{
    uint16_t              XSize;        /**< 位图宽度 */
    uint16_t              YSize;        /**< 位图高度 */
    uint16_t              BytesPerLine; /**< 每行字节数 */
    uint16_t              BitsPerPixel; /**< 每像素位数 */
    const uint8_t*        pData;        /**< 位图数据指针 */
    const GUI_LOGPALETTE* pPal;         /**< 调色板指针 */
} GUI_BITMAP;

/**
 * @brief 无调色板的位图结构体
 */
typedef struct
{
    uint8_t        XSize;        /**< 位图的宽度 */
    uint8_t        YSize;        /**< 位图的高度 */
    uint8_t        BytesPerLine; /**< 每行的字节数 */
    uint8_t        BitsPerPixel; /**< 每像素的位数 */
    const uint8_t* pData;        /**< 指向位图数据的指针 */
} GUI_BITMAP_NOPAL;

/**
 * @brief 带调色板的位图结构体
 */
typedef struct
{
    const GUI_BITMAP_NOPAL* pBitMap_NoPal; /**< 指向无调色板位图的指针 */
    const GUI_LOGPALETTE*   pPal;          /**< 指向调色板的指针 */
#ifdef BITMAP_16Bit
    /**
     * @brief 绘制函数指针
     *
     * @param x0 起始x坐标
     * @param y0 起始y坐标
     * @param xsize 位图宽度
     * @param ysize 位图高度
     * @param pPixel 指向像素数据的指针
     */
    void (*pfDraw)(int16_t x0, int16_t y0, int16_t xsize, int16_t ysize, const uint8_t* pPixel);
#endif
} GUI_BITMAP_P;

/* 全局变量声明 ***********************************************************************/

/* 全局函数声明 ***********************************************************************/

extern void GUI_SetColor(uint32_t Color);
extern void GUI_SetBkColor(uint32_t Color);
extern void GUI_SetPenSize(uint8_t PenSize);
extern void GUI_SetFont(const GUI_FONT* pNewFont);

extern int16_t GUI_DispCharAt(uint16_t c, int16_t x, int16_t y);
extern void    GUI_DispStringAt(const char* s, int x, int y);
extern void    GUI_DispDecAt(uint32_t v, int16_t x, int16_t y, uint8_t Len);
extern void    GUI_DispDecShift(uint32_t value, int16_t x, int16_t y, uint8_t len, uint8_t shift);

extern void GUI_DrawBitmap(const GUI_BITMAP* pBitmap, uint16_t x0, uint16_t y0);
extern void GUI_DrawBitmap_P(const GUI_BITMAP_P* pBitmap, uint16_t x0, uint16_t y0);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_DRAW_BITMAP_H */

#endif

/* ************************************* 文件结束 *************************************/
