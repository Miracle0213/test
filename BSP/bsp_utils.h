/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved.
 *
 * @file      bsp_utils.h
 * @brief     none
 * @version   1.0.0
 * @author    Wei-JiaBao
 * @date      2022/9/21
 */

#ifndef BSP_UTILS_H
#define BSP_UTILS_H

uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen);
uint16_t BEBufToUint16(uint8_t *_pBuf);
uint32_t BEBufToUint24(uint8_t *_pBuf);
uint16_t *SwapBEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);
uint16_t GetHigh16OfFloat(float _ff);
uint16_t GetLow16OfFloat(float _ff);
uint32_t Get32BitOfFloat(float _ff);
float BEBufToFloat(uint8_t *_pBuf);
void FloatToBEBuf(float _f, uint8_t *_pBuf);

#endif  //BSP_UTILS_H
