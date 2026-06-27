/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved.
 *
 * @file      queue.h
 * @brief     none
 * @version   1.0.0
 * @author    Wei-JiaBao
 * @date      2022/10/9
 */

#ifndef __QUEUE_H
#define __QUEUE_H

//==============================================================================
//  ????
//==============================================================================
#define QUEUE_DATA_T float  //????????

//==============================================================================
//  ????
//==============================================================================
typedef struct QUEUE_HandleTypeDef
{
    unsigned int head;  //?????
    unsigned int tail;  //?????
    unsigned int buffer_length;  //??????(??????)
    QUEUE_DATA_T *buffer;  //??????(??????)
} QUEUE_HandleTypeDef;

typedef enum
{
    QUEUE_OK = 0x00U,  //??OK
    QUEUE_ERROR = 0x01U,  //????
    QUEUE_BUSY = 0x02U,  //???
    QUEUE_TIMEOUT = 0x03U,  //????
    QUEUE_OVERLOAD = 0x04U,  //????
    QUEUE_VOID = 0x05U  //????
} QUEUE_StatusTypeDef;


//==============================================================================
//  ??????
//==============================================================================
extern void Queue_Init(QUEUE_HandleTypeDef *hqueue, QUEUE_DATA_T *buffer, unsigned int len);
extern void Queue_Clear(QUEUE_HandleTypeDef *hqueue);
extern unsigned int Queue_Count(QUEUE_HandleTypeDef *hqueue);
extern QUEUE_StatusTypeDef Queue_Push(QUEUE_HandleTypeDef *hqueue, QUEUE_DATA_T data);
extern unsigned int Queue_Push_Array(QUEUE_HandleTypeDef *hqueue, QUEUE_DATA_T *pdatas, unsigned int len);
extern QUEUE_StatusTypeDef Queue_Pop(QUEUE_HandleTypeDef *hqueue, QUEUE_DATA_T *pdata);
extern unsigned int Queue_Pop_Array(QUEUE_HandleTypeDef *hqueue, QUEUE_DATA_T *pdatas, unsigned int len);
extern QUEUE_StatusTypeDef Queue_Peek(QUEUE_HandleTypeDef *hqueue, QUEUE_DATA_T *pdata);
extern unsigned int Queue_Peek_Array(QUEUE_HandleTypeDef *hqueue, QUEUE_DATA_T *pdatas, unsigned int len);

#endif