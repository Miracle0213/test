/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved
 *
 * @file      ringbuffer.h
 * @brief     none
 * @version   1.0.0
 * @author    Wei-JiaBao
 * @date      2022/3/9
 */

#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include <stdbool.h>
#include <stdint.h>

#define MAX_BUFFER 255  // Maximum size for the communication buffer in bytes.

typedef struct
{
    uint8_t uxBuffer[MAX_BUFFER];
    uint8_t start;
    uint8_t end;
    uint8_t available;
    bool overflow;
} mbRingBuffer_t;

extern mbRingBuffer_t xBufferRX;
//extern mbRingBuffer_t xBufferTX;

// Function prototypes for ModbusRingBuffer
void RingAdd(mbRingBuffer_t *xRingBuffer, uint8_t u8Val);  // adds a byte to the ring buffer
uint16_t RingGetAllBytes(mbRingBuffer_t *xRingBuffer, uint8_t *buffer);  // gets all the available bytes into buffer and return the number of bytes read
uint16_t RingGetNBytes(mbRingBuffer_t *xRingBuffer, uint8_t *buffer, uint16_t uNumber);  // gets uNumber of bytes from ring buffer, returns the actual number of bytes read
uint16_t RingCountBytes(mbRingBuffer_t *xRingBuffer);  // return the number of available bytes
void RingClear(mbRingBuffer_t *xRingBuffer);  // flushes the ring buffer

#endif /* _RINGBUFFER_H_ */
