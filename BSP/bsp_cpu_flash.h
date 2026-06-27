/**
* Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved.
*
* @file      bsp_cpu_flash.h
* @brief     none
* @version   1.0.0
* @author    Wei-JiaBao
* @date      2021/11/22
*/


#ifndef BSP_CPU_FLASH_H
#define BSP_CPU_FLASH_H

#include <stdint.h>
//#include "stm32g0xx_hal_flash.h"


#define CPU_FLASH_SIZE       	(64 * 1024)	/* FLASH总容量 */
#define CPU_FLASH_SECTOR_SIZE	(2 * 1024)		/* 扇区大小，字节 */

#define FLASH_IS_EQU		0   /* Flash内容和待写入的数据相等，不需要擦除和写操作 */
#define FLASH_REQ_WRITE		1	/* Flash不需要擦除，直接写 */
#define FLASH_REQ_ERASE		2	/* Flash需要先擦除,再写 */
#define FLASH_PARAM_ERR		3	/* 函数参数错误 */

uint8_t bsp_ReadCpuFlash(uint8_t *_ucpDst, uint32_t _ulFlashAddr, uint32_t _ulSize);
uint8_t bsp_WriteCpuFlash(uint8_t *_ucpSrc, uint32_t _ulFlashAddr, uint32_t _ulSize);
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize);

uint8_t bsp_EraseCpuFlash(uint32_t _ulFlashAddr);


#endif //BSP_CPU_FLASH_H

