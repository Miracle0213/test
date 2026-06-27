/**
 * @file       param.h
 * @author     huxiaodan (huxiaodan@stw-ecig.com)
 * @date       2025-08-21
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
#if 1 // 全局宏开关

#ifndef __PARAM_H
#define __PARAM_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件包含 *************************************************************************/
#include <stdint.h>

/* 全局宏定义 *************************************************************************/


#define FLASH_USER_START_ADDR (uint32_t)(0x0800F000) /* 384K 最后128K作为数据存储 */

#define PARAM_ADDR FLASH_USER_START_ADDR + 0 /* 基本参数区地址 */
#define PARAM_SIZE 512 /* 最大空间，用于编译查错 */

#define PARAM_CALIB_ADDR (uint32_t)(0x0800F800) /* 校准参数区地址 */
#define PARAM_CALIB_SIZE 512 /* 最大空间，用于编译查错 */

#define PARAM_BASE_VER 0x00000100 /* 基本参数版本 100 */
#define PARAM_VER 0x00000202 /* 基本参数版本 100 */

#define CALIB_VER 0x00000201 /* 校准参数版本 200 513*/
#define SEQ_1 0x00 
#define SEQ_2 0x01
#define SEQ_3 0x02
/* 全局宏函数定义 *********************************************************************/


/* 全局类型定义 ***********************************************************************/
/* 校准参数结构，两点校准， 通用校准参数，ADC */

typedef struct
{
    float x1;
    float y1;
    float x2;
    float y2;
} AN_CALIB_T;
/* 校准参数结构，多点校准，解决DAC电路非线性不好的问题 */
typedef struct
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
    int16_t x3;
    int16_t y3;
    int16_t x4;
    int16_t y4;
} AN_CALIB_DAC_T;

/* 校准参数结构，多点校准，解决电流非线性不好的问题 */
typedef struct
{
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float x4;
    float y4;
} AN_CALIB_ADC_T;

/* 全局参数 */
typedef struct
{
    uint32_t UpgradeFlag; /*升级标记,0x55AAA55A表示需要更新APP，0xFFFF表示更新完毕*/
    uint32_t ParamVer; /* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */

    // HT7017 Param
    uint8_t HT7017CalibFlag[1]; /* HT7017初始化标志物 */
    int32_t DcCurrOffset[1]; /* HT7017 DC OFFSET 寄存器值 */
    int32_t AcCurrOffset[1]; /* HT7017 RMS OFFSET 寄存器值 */
    int32_t DcVoltOffset[1]; /* HT7017 DC OFFSET 寄存器值 */
    int32_t AcVoltOffset[1]; /* HT7017 RMS OFFSET 寄存器值 */

    float VoltageRange[2];
    float CurrentRange[2];
    float QCurrentRange[2];
    float CurrentTrigger[2];

    uint16_t Id;
    uint8_t VoltageMode;
    float DcValueOffset[3];
    float AcValueOffset[3];
	  
		uint8_t MODE[2]; //MODE[0]: 0xFF:上位机控制测试顺序 0-4：按键设置SEQ_A-SEQ_E   MODE[1]: 表示参考电流索引；0:1.5A; 1：3A; 2：5A
		uint8_t measure_seq[3];
		uint8_t switch_to_next[3];
		uint8_t Voltage_auto_trigger;
		float Voltage_trigger_delay;
} PARAM_T;

/* 模拟量校准参数 */
typedef struct
{
    uint32_t CalibVer; /* 校准参数版本. 用于升级 */

    AN_CALIB_T DcVolt[1]; /* 电池电压校准参数 */
    AN_CALIB_T AcVolt[1]; /* 电池电流校准参数，2个量程  */
    AN_CALIB_T DcCurr[2 * 1]; /* 电池电压校准参数 */
    AN_CALIB_T AcCurr[2 * 1]; /* 电池电流校准参数，2个量程  */
    AN_CALIB_T AcPower[1]; /* 电池有功功率校准参数 */

} CALIB_T;

/* 全局变量 */
typedef struct
{
    /* MCU ID */
    uint32_t CPU_Sn[3];
    uint8_t jumpRst;

    uint8_t SystemState;
    uint8_t Measure_Mode;
    uint8_t NeedSaveParam;

    uint8_t MeasureMode[1];  // 通道1和通道2的测量模式
    uint8_t Volt_On[1];  // 电压通道打开或关闭
    uint8_t Curr_On[1];  // 电流通道打开或关闭

    float MAF_DcValue[3];
    float DcValue[3];
    float ADC_DcValue[3];

    float ValueResult[3];
    uint8_t ValueResultPass[3];
	
		
    uint8_t CalibFlag;

    uint8_t NeedMeasureFunc;
    uint32_t MenuJump;
} VAR_T;

/* 全局变量声明 ***********************************************************************/

extern uint8_t SEQ_SUM;

extern float REF_CURRENT ;  // 充电电流的参考校准
extern float MAX_CURRENT ;  // 充电电流的最大显示值
extern const float ref_current[3];
extern const float max_current[3];
/*------------------------------*/
extern PARAM_T g_tParam;  // 参数
extern CALIB_T g_tCalib;  // 校准
extern VAR_T g_tVar;  // 电压
extern uint16_t g_LcdHeight;
extern uint16_t g_LcdWidth;
/* 全局函数声明 ***********************************************************************/
void LoadParam(void);
void SaveParam(void);
void InitBaseParam(void);

void InitCalibParam(void);
void SaveCalibParam(void);
void LoadModeParam(void);

#ifdef __cplusplus
}
#endif

#endif /* __PARAM_H */

#endif
/* ************************************* 文件结束 *************************************/