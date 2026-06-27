/**
 * @file       ma_filter.h
 * @author     ??? (weijiabao@stw-ecig.cn)
 * @date       2025-01-11
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

#ifndef __MA_FILTER_H
#define __MA_FILTER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ????? *************************************************************************/

#include <stdint.h>

/* ????? *************************************************************************/
#define MVF_LENGTH 10

typedef float EM_SAMPLE;

/*?????????????*/
typedef struct
{
    EM_SAMPLE buffer[MVF_LENGTH];
    EM_SAMPLE sum;
    int       index;
} MA_FILTER_T;

/* ??????? *********************************************************************/

/* ?????? ***********************************************************************/



/* ?????? ***********************************************************************/

extern void      MoveAverageFilter_Init(MA_FILTER_T *pMaf);
extern EM_SAMPLE MoveAverageFilter_Insert(MA_FILTER_T *pMaf, EM_SAMPLE xn);
extern void      MoveAverageFilter_ClearBuffer(MA_FILTER_T *pMaf);

/* ?????? ***********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __MA_FILTER_H */

#endif
/* ************************************* ???? *************************************/
