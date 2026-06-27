/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved
 *
 * @file      ringbuffer.c
 * @brief     none
 * @version   1.0.0
 * @author    Wei-JiaBao
 * @date      2022/3/9
 */

#include "ringbuffer.h"

mbRingBuffer_t xBufferRX = { 0 };
//mbRingBuffer_t xBufferTX = { 0 };

/* Ring Buffer functions */
// This function must be called only after disabling USART RX interrupt or inside of the RX interrupt
void RingAdd(mbRingBuffer_t *xRingBuffer, uint8_t u8Val)
{

    xRingBuffer->uxBuffer[xRingBuffer->end] = u8Val;
    xRingBuffer->end = (xRingBuffer->end + 1) % MAX_BUFFER;
    if (xRingBuffer->available >= MAX_BUFFER)
    {
        xRingBuffer->overflow = true;
        xRingBuffer->start = xRingBuffer->end;
    }
    else
    {
        xRingBuffer->overflow = false;
        xRingBuffer->available++;
    }
}

// This function must be called only after disabling USART RX interrupt
uint16_t RingGetAllBytes(mbRingBuffer_t *xRingBuffer, uint8_t *buffer)
{
    return RingGetNBytes(xRingBuffer, buffer, xRingBuffer->available);
}

// This function must be called only after disabling USART RX interrupt
uint16_t RingGetNBytes(mbRingBuffer_t *xRingBuffer, uint8_t *buffer, uint16_t uNumber)
{
    uint32_t uCounter;
    if (xRingBuffer->available == 0 || uNumber == 0)
        return 0;
    if (uNumber > MAX_BUFFER)
        return 0;

    for (uCounter = 0; uCounter < uNumber && uCounter < xRingBuffer->available; uCounter++)
    {
        buffer[uCounter] = xRingBuffer->uxBuffer[xRingBuffer->start];
        xRingBuffer->start = (xRingBuffer->start + 1) % MAX_BUFFER;
    }
    xRingBuffer->available = xRingBuffer->available - uCounter;
    xRingBuffer->overflow = false;
    RingClear(xRingBuffer);

    return uCounter;
}

uint16_t RingCountBytes(mbRingBuffer_t *xRingBuffer)
{
    return xRingBuffer->available;
}

void RingClear(mbRingBuffer_t *xRingBuffer)
{
    xRingBuffer->start = 0;
    xRingBuffer->end = 0;
    xRingBuffer->available = 0;
    xRingBuffer->overflow = false;
}

/* End of Ring Buffer functions */
