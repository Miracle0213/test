/**
 * @file       gui_draw.c
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
 * 版权所有  深圳思拓微电子有限公司。保留所有权利。
 * ************************************************************************
 */
/* 头文件包含 *************************************************************************/

#include "gui_draw.h"
#include "lcd_driver.h"

/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

static uint16_t LCD_ConvTable[126];

__ALIGNED(4) uint8_t LCD_Buffer[2][LCD_WIDTH];

static uint16_t        R_Front_Color;  // 前景色
static uint16_t        R_Back_Color;   // 背景色
static uint8_t         R_Front_Alpha;  // 前景色混色值
static uint8_t         R_Back_Alpha;   // 背景色混色值
static uint8_t         R_PenSize;      // 画笔宽度
static const GUI_FONT* R_pNewFont;     // 设置文本输出使用的字体

/* 私有函数声明 ***********************************************************************/

static void      DrawBitLine1BPP(int16_t x, int16_t y, uint8_t const* p, int16_t Diff, int16_t xsize, const uint16_t* pTrans);
static void      DrawBitLine2BPP(int16_t x, int16_t y, uint8_t const* p, int16_t Diff, int16_t xsize, const uint16_t* pTrans);
static void      DrawBitLine4BPP(int16_t x, int16_t y, uint8_t const* p, int16_t Diff, int16_t xsize, const uint16_t* pTrans);
static void      DrawBitLine8BPP(int16_t x, int16_t y, uint8_t const* p, int16_t xsize, const uint16_t* pTrans);
static void      DrawBitLine16BPP(uint16_t x, uint16_t y, uint16_t const* p, uint16_t xsize);
static void      LCD_L0_DrawBitmap(int16_t x0, int16_t y0, int16_t xsize, int16_t ysize, int16_t BitsPerPixel, int16_t BytesPerLine, const uint8_t* pData, int16_t Diff,
                                   const uint16_t* pTrans);
static void      LCD_DrawBitmap(int16_t x0, int16_t y0, int16_t xsize, int16_t ysize, int16_t BitsPerPixel, int16_t BytesPerLine, const uint8_t* pPixel, const uint16_t* pTrans);
static uint16_t* LCD_GetpPalConvTable(const GUI_LOGPALETTE* pLogPal);

/* 全局变量声明 ***********************************************************************/

uint16_t LCD_BKCOLORINDEX;

/* 全局函数原型 ***********************************************************************/

// 设置当前前景色。
void GUI_SetColor(uint32_t Color)
{
    R_Front_Color = CZH(Color);
//    R_Front_Color = Color;
    R_Front_Alpha = (uint8_t)(Color >> 24);
}

// 设置当前背景色。
void GUI_SetBkColor(uint32_t Color)
{
    R_Back_Color = CZH(Color);
//    R_Back_Color = CZH(Color);
    R_Back_Alpha = (uint8_t)(Color >> 24);
}

// 设置画笔宽度
void GUI_SetPenSize(uint8_t PenSize)
{
    R_PenSize = PenSize;
}

// 设置字体
void GUI_SetFont(const GUI_FONT* pNewFont)
{
    R_pNewFont = pNewFont;
}

// 公用一个图片数据，不同颜色组合
// 比如 选中和未选中  多套UI，图标一样，颜色不一样
void GUI_DrawBitmap_P(const GUI_BITMAP_P* pBitmap, uint16_t x0, uint16_t y0)
{
    const uint16_t* pTrans;

    if (x0 > LCD_WIDTH || y0 > LCD_HEIGHT)
        return;

#ifdef BITMAP_NOPAL6Bit
    if (pBitmap->pfDraw)
    {
        pBitmap->pfDraw(x0, y0, pBitmap->pBitMap_NoPal->XSize, pBitmap->pBitMap_NoPal->YSize, (uint8_t const*)pBitmap->pBitMap_NoPal->pData);
    }
    else
#endif
    {
        pTrans = LCD_GetpPalConvTable(pBitmap->pPal);

        if (!pTrans)
            pTrans = (pBitmap->pBitMap_NoPal->BitsPerPixel != 1) ? NULL : &LCD_BKCOLORINDEX;
        LCD_DrawBitmap(x0, y0, pBitmap->pBitMap_NoPal->XSize, pBitmap->pBitMap_NoPal->YSize, pBitmap->pBitMap_NoPal->BitsPerPixel, pBitmap->pBitMap_NoPal->BytesPerLine,
                       pBitmap->pBitMap_NoPal->pData, pTrans);
    }
}

// 直接显示位图
// 用BmpCvtST.exe
// Image -> Best palette
void GUI_DrawBitmap(const GUI_BITMAP* pBitmap, uint16_t x0, uint16_t y0)
{
    const uint16_t* pTrans;

    pTrans = LCD_GetpPalConvTable(pBitmap->pPal);

    if (!pTrans)
        pTrans = (pBitmap->BitsPerPixel != 1) ? NULL : &LCD_BKCOLORINDEX;
    LCD_DrawBitmap(x0, y0, pBitmap->XSize, pBitmap->YSize, pBitmap->BitsPerPixel, pBitmap->BytesPerLine, pBitmap->pData, pTrans);
}

int data1,data2,data3,data4;
/**
 * @brief 在指定位置显示单个字符
 *
 * @param c 要显示的字符
 * @param x 字符的起始 x 坐标
 * @param y 字符的起始 y 坐标
 * @return 下一个字符的 x 坐标位置
 */
int16_t GUI_DispCharAt(uint16_t c, int16_t x, int16_t y)
{
    GUI_CONST_STORAGE GUI_FONT_PROP_EXT* currentFontProp;
    const GUI_CHARINFO_EXT*             characterInfo;

    int16_t  i, j;
    uint16_t byteLength;
    int16_t  nextXPosition = x;

    currentFontProp = R_pNewFont->prop;

    while (currentFontProp != 0)
    {
        if (c >= currentFontProp->First && c <= currentFontProp->Last)
        {
            characterInfo = currentFontProp->paCharInfo + c - currentFontProp->First;
            nextXPosition = x + characterInfo->XDist;
            if (x >= LCD_WIDTH || y >= LCD_HEIGHT || (y + R_pNewFont->YDist) < 0)
            {
                return nextXPosition;
            }

            if (R_Back_Alpha != 0xFF)  // 背景色混色值
            {
                LCD_Fill(x, y, characterInfo->XDist + x - 1, y + characterInfo->YPos - 1, R_Back_Color);

                LCD_Fill(x, y + characterInfo->YPos + characterInfo->YSize, characterInfo->XDist + x - 1, y + R_pNewFont->YDist - 1, R_Back_Color);
							
							  LCD_Fill(x, y + characterInfo->YPos, x + characterInfo->XPos - 1, y + characterInfo->YPos + characterInfo->YSize - 1, R_Back_Color);
                
							  LCD_Fill(x + characterInfo->XPos + characterInfo->XSize, y + characterInfo->YPos, x + characterInfo->XDist - 1, y + characterInfo->YPos + characterInfo->YSize - 1,
                         R_Back_Color);
            }

            if (x + characterInfo->XPos >= LCD_WIDTH || y + characterInfo->YPos >= LCD_HEIGHT)
            {
                return nextXPosition;
            }

            uint8_t   bufferIndex = 0;
            uint16_t* pixelBuffer;

            pixelBuffer = (uint16_t*)LCD_Buffer[bufferIndex];

            if (R_pNewFont->FONT_TYPE == GUI_FONTTYPE_PROP_EXT)
            {
                byteLength = (characterInfo->XSize + 7) >> 3;
                for (i = 0; i < characterInfo->YSize; i++)
                {
                    for (j = 0; j < characterInfo->XSize; j++)
                    {
                        pixelBuffer[j] = (characterInfo->pData[i * byteLength + (j >> 3)] >> (7 - j % 8)) & 0x01 ? R_Front_Color : (R_Back_Color << 8) | (R_Back_Color >> 8);
                    }
                    LCD_DrawLineByColor(x + characterInfo->XPos, y + characterInfo->YPos + i, pixelBuffer, characterInfo->XSize);
                }
            }
            else if (R_pNewFont->FONT_TYPE == GUI_FONTTYPE_PROP_AA2_EXT)
            {
                uint16_t intermediateColor1, intermediateColor2;

                intermediateColor1 = CZH(((((R_Front_Color & 0xF800) >> 8) + ((R_Back_Color & 0xF800) >> 8) * 2) / 3) |
                                         (((((R_Front_Color & 0x07E0) >> 3) + ((R_Back_Color & 0x07E0) >> 3) * 2) / 3) << 8) |
                                         (((((R_Front_Color & 0x001F) << 3) + ((R_Back_Color & 0x001F) << 3) * 2) / 3) << 16));
                intermediateColor2 = CZH(((((R_Front_Color & 0xF800) >> 8) * 2 + ((R_Back_Color & 0xF800) >> 8)) / 3) |
                                         (((((R_Front_Color & 0x07E0) >> 3) * 2 + ((R_Back_Color & 0x07E0) >> 3)) / 3) << 8) |
                                         (((((R_Front_Color & 0x001F) << 3) * 2 + ((R_Back_Color & 0x001F) << 3)) / 3) << 16));

                byteLength = (characterInfo->XSize + 3) >> 2;

                for (i = 0; i < characterInfo->YSize; i++)
                {
                    for (j = 0; j < characterInfo->XSize; j++)
                    {
                        uint8_t pixelData = (characterInfo->pData[i * byteLength + (j >> 2)] >> (2 * (3 - j % 4))) & 0x03;
                        switch (pixelData)
                        {
                            case 0:
                                pixelBuffer[j] = R_Back_Color;
                                break;
                            case 1:
                                pixelBuffer[j] = intermediateColor1;
                                break;
                            case 2:
                                pixelBuffer[j] = intermediateColor2;
                                break;
                            default:
                                pixelBuffer[j] = R_Front_Color;
                                break;
                        }
                    }
                    LCD_DrawLineByColor(x + characterInfo->XPos, y + characterInfo->YPos + i, pixelBuffer, characterInfo->XSize);
                }
            }
            return nextXPosition;
        }
        else
        {
            currentFontProp = currentFontProp->pNext;
        }
    }
    return nextXPosition;
}

// 定义一个辅助函数来处理不同长度的UTF-8字符
/**
 * @brief 获取UTF-8编码的字符并将指针前移到下一个字符
 *
 * 该函数从输入字符串指针s中读取一个UTF-8编码的字符，并将指针前移到下一个字符的位置。
 * 根据UTF-8编码的字节数（1到4字节），函数会解析并返回对应的Unicode码点。
 * 如果遇到非法的UTF-8编码，函数会返回U+FFFD（替换字符）。
 *
 * @param s 指向UTF-8字符串指针的指针
 * @return 解析得到的Unicode码点，如果遇到非法字符则返回U+FFFD
 */
static uint32_t get_utf8_char(const char** s)
{
    uint8_t byte1 = (uint8_t) * *s;
    if ((byte1 & 0x80) == 0)
    {  // ASCII
        (*s)++;
        return byte1;
    }
    else if ((byte1 & 0xE0) == 0xC0)
    {  // 2-byte UTF-8
        uint8_t byte2 = (uint8_t)(*((*s) + 1));
        (*s) += 2;
        return ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
    }
    else if ((byte1 & 0xF0) == 0xE0)
    {  // 3-byte UTF-8
        uint8_t byte2 = (uint8_t)(*((*s) + 1));
        uint8_t byte3 = (uint8_t)(*((*s) + 2));
        (*s) += 3;
        return ((byte1 & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
    }
    else if ((byte1 & 0xF8) == 0xF0)
    {  // 4-byte UTF-8
        uint8_t byte2 = (uint8_t)(*((*s) + 1));
        uint8_t byte3 = (uint8_t)(*((*s) + 2));
        uint8_t byte4 = (uint8_t)(*((*s) + 3));
        (*s) += 4;
        return ((byte1 & 0x07) << 18) | ((byte2 & 0x3F) << 12) | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
    }
    else
    {
        (*s)++;
        return 0xFFFD;  // 替换非法字符为U+FFFD
    }
}

/**
 * @brief 在指定位置显示字符串
 *
 * @param s 要显示的字符串
 * @param x 字符串的起始 x 坐标
 * @param y 字符串的起始 y 坐标
 */
void GUI_DispStringAt(const char* s, int x, int y)
{
    if (s == NULL)
    {
        return;  // 处理空指针
    }

    int16_t next_x = x;

    while (*s)
    {
        uint32_t utf8_code = get_utf8_char(&s);

        next_x = GUI_DispCharAt(utf8_code, next_x, y);
        if (next_x < 0)  // 检查GUI_DispCharAt的返回值
        {
            break;  // 如果返回值异常，停止处理
        }
    }
}

// 在指定位置显示指定字符数的十进制数值
void GUI_DispDecAt(uint32_t v, int16_t x, int16_t y, uint8_t Len)
{
    if (Len > 10)
        return;

    int8_t i;
    char   S[Len + 1];
    S[Len] = 0;

    for (i = Len - 1; i >= 0; i--)
    {
        S[i] = (v % 10) + '0';
        v /= 10;
    }
    GUI_DispStringAt(S, x, y);
}
/**
 * @brief 在指定位置显示带小数点的十进制数值
 * @param value 要显示的数值
 * @param x 显示位置的x坐标
 * @param y 显示位置的y坐标
 * @param len 显示的总长度（包括小数点）
 * @param shift 小数点后的位数
 * @note 最大支持显示10位数字（包括小数点）
 */
void GUI_DispDecShift(uint32_t value, int16_t x, int16_t y, uint8_t len, uint8_t shift)
{
    // 参数校验
    if (len == 0 || len > 10)
    {
        return;  // 长度无效
    }
    if (shift >= len || len - shift < 2)
    {
        return;  // 小数点位置无效
    }

    // 使用固定长度数组，避免VLA
    char s[11] = {0};  // 最大10位数字 + 结束符

    int8_t i = len - 1;
    // 处理小数部分
    for (; i >= (int8_t)(len - shift); i--)
    {
        s[i] = (value % 10) + '0';
        value /= 10;
    }

    // 添加小数点
    s[i--] = '.';

    // 处理整数部分
    for (; i >= 0; i--)
    {
        s[i] = (value % 10) + '0';
        value /= 10;
    }

    // 显示字符串
    GUI_DispStringAt(s, x, y);
}

/* 私有函数原型 ***********************************************************************/

static void DrawBitLine1BPP(int16_t x, int16_t y, uint8_t const* p, int16_t Diff, int16_t xsize, const uint16_t* pTrans)
{
    uint16_t pixels;
    uint16_t Index0 = *(pTrans + 0);
    uint16_t Index1 = *(pTrans + 1);

    pixels = *p;

    switch (Diff & 7)
    {
        case 0:
            goto WriteBit0;
        case 1:
            goto WriteBit1;
        case 2:
            goto WriteBit2;
        case 3:
            goto WriteBit3;
        case 4:
            goto WriteBit4;
        case 5:
            goto WriteBit5;
        case 6:
            goto WriteBit6;
        case 7:
            goto WriteBit7;
    }

WriteBit0:
    LCD_DrawPixel(x + 0, y, (pixels & (1 << 7)) ? Index1 : Index0);
    if (!--xsize)
        return;
WriteBit1:
    LCD_DrawPixel(x + 1, y, (pixels & (1 << 6)) ? Index1 : Index0);
    if (!--xsize)
        return;
WriteBit2:
    LCD_DrawPixel(x + 2, y, (pixels & (1 << 5)) ? Index1 : Index0);
    if (!--xsize)
        return;
WriteBit3:
    LCD_DrawPixel(x + 3, y, (pixels & (1 << 4)) ? Index1 : Index0);
    if (!--xsize)
        return;
WriteBit4:
    LCD_DrawPixel(x + 4, y, (pixels & (1 << 3)) ? Index1 : Index0);
    if (!--xsize)
        return;
WriteBit5:
    LCD_DrawPixel(x + 5, y, (pixels & (1 << 2)) ? Index1 : Index0);
    if (!--xsize)
        return;
WriteBit6:
    LCD_DrawPixel(x + 6, y, (pixels & (1 << 1)) ? Index1 : Index0);
    if (!--xsize)
        return;
WriteBit7:
    LCD_DrawPixel(x + 7, y, (pixels & (1 << 0)) ? Index1 : Index0);
    if (!--xsize)
        return;
    x += 8;
    pixels = *(++p);
    goto WriteBit0;
}

static void DrawBitLine2BPP(int16_t x, int16_t y, uint8_t const* p, int16_t Diff, int16_t xsize, const uint16_t* pTrans)
{
    uint16_t pixels = *p;

    if (pTrans)
    {
        switch (Diff & 3)
        {
            case 0:
                goto WriteBit0;
            case 1:
                goto WriteBit1;
            case 2:
                goto WriteBit2;
            default:
                goto WriteBit3;
        }

    WriteBit0:
        LCD_DrawPixel(x + 0, y, *(pTrans + (pixels >> 6)));
        if (!--xsize)
            return;
    WriteBit1:
        LCD_DrawPixel(x + 1, y, *(pTrans + (3 & (pixels >> 4))));
        if (!--xsize)
            return;
    WriteBit2:
        LCD_DrawPixel(x + 2, y, *(pTrans + (3 & (pixels >> 2))));
        if (!--xsize)
            return;
    WriteBit3:
        LCD_DrawPixel(x + 3, y, *(pTrans + (3 & (pixels))));
        if (!--xsize)
            return;
        pixels = *(++p);
        x += 4;
        goto WriteBit0;
    }
    else
    {
        switch (Diff & 3)
        {
            case 0:
                goto WriteDDPBit0;
            case 1:
                goto WriteDDPBit1;
            case 2:
                goto WriteDDPBit2;
            default:
                goto WriteDDPBit3;
        }

    WriteDDPBit0:
        LCD_DrawPixel(x + 0, y, (pixels >> 6));
        if (!--xsize)
            return;
    WriteDDPBit1:
        LCD_DrawPixel(x + 1, y, (3 & (pixels >> 4)));
        if (!--xsize)
            return;
    WriteDDPBit2:
        LCD_DrawPixel(x + 2, y, (3 & (pixels >> 2)));
        if (!--xsize)
            return;
    WriteDDPBit3:
        LCD_DrawPixel(x + 3, y, (3 & (pixels)));
        if (!--xsize)
            return;
        pixels = *(++p);
        x += 4;
        goto WriteDDPBit0;
    }
}

static void DrawBitLine4BPP(int16_t x, int16_t y, uint8_t const* p, int16_t Diff, int16_t xsize, const uint16_t* pTrans)
{
    uint16_t pixels = *p;

    if (pTrans)
    {
        if ((Diff & 1) == 0)
            goto WriteBit0;
        goto WriteBit1;

    WriteBit0:
        LCD_DrawPixel(x + 0, y, *(pTrans + (pixels >> 4)));
        if (!--xsize)
            return;
    WriteBit1:
        LCD_DrawPixel(x + 1, y, *(pTrans + (pixels & 0xf)));
        if (!--xsize)
            return;
        x += 2;
        pixels = *(++p);
        goto WriteBit0;
    }
    else
    {
        if ((Diff & 1) == 0)
            goto WriteDDPBit0;
        goto WriteDDPBit1;

    WriteDDPBit0:
        LCD_DrawPixel(x + 0, y, (pixels >> 4));
        if (!--xsize)
            return;
    WriteDDPBit1:
        LCD_DrawPixel(x + 1, y, (pixels & 0xf));
        if (!--xsize)
            return;
        x += 2;
        pixels = *(++p);
        goto WriteDDPBit0;
    }
}

static void DrawBitLine8BPP(int16_t x, int16_t y, uint8_t const* p, int16_t xsize, const uint16_t* pTrans)
{
    uint16_t pixel;
    if (pTrans)
    {
        for (; xsize > 0; xsize--, x++, p++)
        {
            pixel = *p;
            if (pixel)
            {
                LCD_DrawPixel(x + 0, y, *(pTrans + pixel));
            }
        }
    }
    else
    {
        for (; xsize > 0; xsize--, x++, p++)
        {
            pixel = *p;
            if (pixel)
            {
                LCD_DrawPixel(x + 0, y, pixel);
            }
        }
    }
}

static void DrawBitLine16BPP(uint16_t x, uint16_t y, uint16_t const* p, uint16_t xsize)
{
    uint16_t Index;
    for (; xsize > 0; xsize--, x++, p++)
    {
        Index = *p;
        if (Index)
        {
            LCD_DrawPixel(x + 0, y, Index);
        }
    }
}

static void LCD_L0_DrawBitmap(int16_t x0, int16_t y0, int16_t xsize, int16_t ysize, int16_t BitsPerPixel, int16_t BytesPerLine, const uint8_t* pData, int16_t Diff,
                              const uint16_t* pTrans)
{
    int16_t i;  // 溢出 i原本是int8_t类型，到125会溢出变成负的，因此多刷了一遍屏幕
    switch (BitsPerPixel)
    {
        case 1:
            for (i = 0; i < ysize; i++)
            {
                DrawBitLine1BPP(x0, i + y0, pData, Diff, xsize, pTrans);
                pData += BytesPerLine;
            }
            break;
        case 2:
            for (i = 0; i < ysize; i++)
            {
                DrawBitLine2BPP(x0, i + y0, pData, Diff, xsize, pTrans);
                pData += BytesPerLine;
            }
            break;
        case 4:
            for (i = 0; i < ysize; i++)
            {
                DrawBitLine4BPP(x0, i + y0, pData, Diff, xsize, pTrans);
                pData += BytesPerLine;
            }
            break;
        case 8:
            for (i = 0; i < ysize; i++)
            {
                DrawBitLine8BPP(x0, i + y0, pData, xsize, pTrans);
                pData += BytesPerLine;
            }
            break;
        case 16:
            for (i = 0; i < ysize; i++)
            {
                DrawBitLine16BPP(x0, i + y0, (uint16_t*)pData, xsize);
                pData += BytesPerLine;
            }
            break;
    }
}

static void LCD_DrawBitmap(int16_t x0, int16_t y0, int16_t xsize, int16_t ysize, int16_t BitsPerPixel, int16_t BytesPerLine, const uint8_t* pPixel, const uint16_t* pTrans)
{
    int16_t x1, y1;
    y1 = y0 + ysize - 1;
    x1 = x0 + xsize - 1;

    int16_t Diff;

    Diff = 0 - y0;
    if (Diff > 0)
    {
        ysize -= Diff;
        if (ysize <= 0)
            return;

        y0 = 0;

        pPixel += (unsigned)Diff * (unsigned)BytesPerLine;
    }

    Diff = y1 - LCD_HEIGHT;
    if (Diff > 0)
    {
        ysize -= Diff;
        if (ysize <= 0)
            return;
    }

    Diff = x1 - LCD_WIDTH;
    if (Diff > 0)
        xsize -= Diff;

    Diff = 0;
    if (x0 < 0)
    {
        Diff = 0 - x0;
        xsize -= Diff;
        switch (BitsPerPixel)
        {
            case 1:
                pPixel += (Diff >> 3);
                x0 += (Diff >> 3) << 3;
                Diff &= 7;
                break;
            case 2:
                pPixel += (Diff >> 2);
                x0 += (Diff >> 2) << 2;
                Diff &= 3;
                break;
            case 4:
                pPixel += (Diff >> 1);
                x0 += (Diff >> 1) << 1;
                Diff &= 1;
                break;
            case 8:
                pPixel += Diff;
                x0 += Diff;
                Diff = 0;
                break;
            case 16:
                pPixel += (Diff << 1);
                x0 += Diff;
                Diff = 0;
                break;
        }
    }
    if (xsize <= 0)
        return;

    LCD_L0_DrawBitmap(x0, y0, xsize, ysize, BitsPerPixel, BytesPerLine, pPixel, Diff, pTrans);
}

static uint16_t* LCD_GetpPalConvTable(const GUI_LOGPALETTE* pLogPal)
{
    if (pLogPal == NULL)
    {
        return NULL;
    }
    if (!pLogPal->pPalEntries)
    {
        return NULL;
    }

    if (pLogPal->NumEntries > 125)
    {
        return NULL;
    }

    uint8_t i;
    uint8_t NumEntries = pLogPal->NumEntries;

    const uint32_t* pPalEntry = &pLogPal->pPalEntries[0];
    for (i = 0; i < NumEntries; i++)
    {
        LCD_ConvTable[i] = CZH(*(pPalEntry + i));
    }

    return &LCD_ConvTable[0];
}

/* ************************************ 文件结束 **************************************/
