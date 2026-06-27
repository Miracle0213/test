/**
 * @file       param.c
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
/* 头文件包含 *************************************************************************/
#include "bsp.h"
#include <string.h>
#include "user_com.h"
#include "user_config.h"
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/
uint8_t SEQ_SUM;

float REF_CURRENT;  // 充电电流的参考校准
float MAX_CURRENT;  // 充电电流的最大显示值
const float ref_current[3] = {1.5f, 3.0f, 5.0f};
const float max_current[3] = {3.0f, 3.2f, 5.2f};
PARAM_T g_tParam; /* 基本参数 */
CALIB_T g_tCalib; /* 校准参数 */
VAR_T   g_tVar;   /* 全局变量 */
/* 私有函数声明 ***********************************************************************/
void LoadCalibParam(void);
void LoadModeParam(void);

/* 全局变量声明 ***********************************************************************/

/* 全局函数原型 ***********************************************************************/
/*
*********************************************************************************************************
*    函 数 名: LoadParam
*    功能说明: 从eeprom读参数到g_tParam
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/

/**
 * @brief 从eeprom读参数到g_tParam
 *
 */
void LoadParam(void)
{
    /* 读取EEPROM中的参数 */
    bsp_ReadCpuFlash((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
    uint32_t savedParamVer = g_tParam.ParamVer;

    if (sizeof(PARAM_T) > PARAM_SIZE)  //
    {
        /* 基本参数分配空间不足 */
        //		Error_Handler(__FILE__, __LINE__);
        while (1);
    }  

    g_tVar.jumpRst = 0;

    // 设备ID禁止等于0x00
    if (g_tParam.Id == 0x00)
    {
        g_tParam.Id = 1;
    }
    if (g_tParam.DcValueOffset[0] == 0.0f)
    {
        g_tParam.DcValueOffset[0] = 1.0f;
        g_tParam.DcValueOffset[1] = 1.0f;
        g_tParam.DcValueOffset[2] = 1.0f;
    }
    if (g_tParam.AcValueOffset[0] == 0.0f)
    {
        g_tParam.AcValueOffset[0] = 1.0f;
        g_tParam.AcValueOffset[1] = 1.0f;
        g_tParam.AcValueOffset[2] = 1.0f;
    }

    if (savedParamVer != PARAM_VER)  // 版本信息不同
    {
        /** TODO: STM32G0 双字写入FLASH时，如果FLASH中的值为FFFFFFFF，再次写入FFFFFFFF后将无法写入该双字，必须整片擦除后才可以写入  */
        // 第一次写入
        if (savedParamVer == 0xFFFFFFFF)
        {
            memset(&g_tParam, 0x00, sizeof(PARAM_T));

            InitBaseParam();  // 初始化参数
        }
        else
        {
#if 1
            if (savedParamVer >= 0x00000103 && savedParamVer <= 0x00000108)
            {
                g_tParam.ParamVer = PARAM_VER; /* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */
                g_tParam.DcValueOffset[0] = 1.0f;
                g_tParam.DcValueOffset[1] = 1.0f;
                g_tParam.DcValueOffset[2] = 1.0f;
            }
            else
            {
                InitBaseParam();
            }
#endif
        }

        SaveParam();
    }

    bsp_GetCpuID(g_tVar.CPU_Sn); /* 读取CPU ID */
    if (g_tParam.MODE[1] >= CURRENT_SUM)
    {
        g_tParam.MODE[1] = 1;
        SaveParam();
    }
    LoadCalibParam();
    LoadModeParam();
}

/**
 * @brief 保存参数到g_tParam
 *
 */
void SaveParam(void)
{
    /* 将全局的参数变量保存到EEPROM */
    bsp_WriteCpuFlash((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
}

/**
 * @brief 初始化基本参数为缺省值
 *
 */
void InitBaseParam(void)
{
    memset(&g_tParam, 0x00, sizeof(PARAM_T));

    g_tParam.UpgradeFlag = 0;         /*升级标记,0x55AAA55A表示需要更新APP，0xFFFF表示更新完毕*/
    g_tParam.ParamVer    = PARAM_VER; /* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */

    //    memset(&g_tParam.HT7017CalibFlag[0], 0x00, 17);

    g_tParam.HT7017CalibFlag[0] = 1;

    g_tParam.VoltageRange[0]   = 3.7f;
    g_tParam.VoltageRange[1]   = 3.5f;
    g_tParam.CurrentRange[0]   = 0.65f;
    g_tParam.CurrentRange[1]   = 0.45f;
    g_tParam.QCurrentRange[0]  = 5.0f;
    g_tParam.QCurrentRange[1]  = 0.5f;
    g_tParam.CurrentTrigger[0] = 0.05f;
    g_tParam.CurrentTrigger[1] = 1.5f;
    g_tParam.DcValueOffset[0]  = 1.0f;
    g_tParam.DcValueOffset[1]  = 1.0f;
    g_tParam.DcValueOffset[2]  = 1.0f;
    g_tParam.AcValueOffset[0]  = 1.0f;
    g_tParam.AcValueOffset[1]  = 1.0f;
    g_tParam.AcValueOffset[2]  = 1.0f;

    g_tParam.Id          = 1;
    g_tParam.VoltageMode = 0;
    g_tParam.MODE[0]     = 0;
    g_tParam.MODE[1]     = 0;
		
		g_tParam.measure_seq[0]=SEQ_1;
		g_tParam.measure_seq[1]=SEQ_2;
		g_tParam.measure_seq[2]=SEQ_3;
		
		g_tParam.switch_to_next[0]=1;
		g_tParam.switch_to_next[1]=0;
		g_tParam.switch_to_next[2]=0;
		g_tParam.Voltage_auto_trigger=0;
		g_tParam.Voltage_trigger_delay=0.5f;
    SaveParam();
}
/**
 * @brief 从eeprom读参数到g_tCalib
 *
 */

void LoadCalibParam(void)
{
    uint8_t init;
    uint8_t i;

    if (sizeof(g_tCalib) > PARAM_CALIB_SIZE)
    {
        /* 校准参数分配空间不足 */
        //		Error_Handler(__FILE__, __LINE__);
        while (1);
    }

    /* 读取EEPROM中的参数 */
    init = 1;
    for (i = 0; i < 3; i++)
    {
        bsp_ReadCpuFlash((uint8_t *)&g_tCalib, PARAM_CALIB_ADDR, sizeof(g_tCalib));
        if (g_tCalib.CalibVer != 0xFFFFFFFF)  // CALIB_VER
        {
            init = 0;
            break;
        }
        //		bsp_DelayUS(50 * 1000); /* 延迟50ms */
        HAL_Delay(50);
    }

    /* 第一次运行，赋缺省值 */
    if (init == 1)
    {
        //        while (1);
        g_tCalib.CalibVer = CALIB_VER;

        InitCalibParam(); /* 初始化校准参数 */
    }

    if (g_tParam.MODE[1] < CURRENT_SUM)
    {
        g_tCalib.DcCurr[0].y2 = ref_current[g_tParam.MODE[1]];
    }

}
/**
 * @brief 写参数，16bit
 *
 */

void WriteParamUint16(uint16_t _addr, uint16_t _value)
{
    bsp_WriteCpuFlash((uint8_t *)&_value, PARAM_CALIB_ADDR + _addr, 2);
}
/**
 * @brief 将全局变量g_tCalib写入到eeprom
 *
 */

void SaveCalibParam(void)
{
    /* 将全局的参数变量保存到EEPROM */
    bsp_WriteCpuFlash((uint8_t *)&g_tCalib, PARAM_CALIB_ADDR, sizeof(g_tCalib));
}
/**
 * @brief 初始化校准参数为缺省值
 *
 */

void InitCalibParam(void)
{
    memset(&g_tCalib, 0x00, sizeof(CALIB_T));

    g_tCalib.CalibVer = CALIB_VER;

    g_tCalib.DcVolt[0].x1 = -0.0f;
    g_tCalib.DcVolt[0].y1 = 0.0f;
    g_tCalib.DcVolt[0].x2 = -6000000.0f;
    g_tCalib.DcVolt[0].y2 = 5.0f;

    g_tCalib.DcCurr[0].x1 = -0.0f;
    g_tCalib.DcCurr[0].y1 = 0.1f;
    g_tCalib.DcCurr[0].x2 = -6000000.0f;
    g_tCalib.DcCurr[0].y2 = 1.5f;

    g_tCalib.DcCurr[1].x1 = -0.0f;
    g_tCalib.DcCurr[1].y1 = 0.0f;
    g_tCalib.DcCurr[1].x2 = -6000000.0f;
    g_tCalib.DcCurr[1].y2 = 100.0f;

    SaveCalibParam();
}
/**
 * @brief 将g_tCalib 写入到CPU内部Flash
 *
 */
void LoadModeParam(void)
{
    if (g_tParam.MODE[0] == 0)  // SEQ_A 电压-充电-静流
    {
        g_tParam.measure_seq[0]  = SEQ_1;  // 电压的测试顺序
        g_tParam.measure_seq[1]  = SEQ_2;  // 充电的测试顺序
        g_tParam.measure_seq[2] = SEQ_3;  // 静流的测试顺序

        g_tParam.switch_to_next[0]  = 1;  // 测完电压之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[1]  = 0;  // 测完充电之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[2] = 0;  // 测完静流之后跳转 1:允许 0:不允许

        g_tParam.Voltage_auto_trigger = 0;  // 按键自动触发 1:允许 0:不允许
    }
    else if (g_tParam.MODE[0] == 1)  // SEQ_B 静流-电压-充电
    {
        g_tParam.measure_seq[0]  = SEQ_2;  // 电压的测试顺序
        g_tParam.measure_seq[1]  = SEQ_3;  // 充电的测试顺序
        g_tParam.measure_seq[2] = SEQ_1;  // 静流的测试顺序

        g_tParam.switch_to_next[0]  = 1;  // 测完电压之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[1]  = 0;  // 测完充电之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[2] = 0;  // 测完静流之后跳转 1:允许 0:不允许

        g_tParam.Voltage_auto_trigger = 0;  // 按键自动触发 1:允许 0:不允许
    }
    else if (g_tParam.MODE[0] == 2)  // SEQ_C  静流-充电-电压
    {
        g_tParam.measure_seq[0]  = SEQ_3;  // 电压的测试顺序
        g_tParam.measure_seq[1]  = SEQ_2;  // 充电的测试顺序
        g_tParam.measure_seq[2] = SEQ_1;  // 静流的测试顺序

        g_tParam.switch_to_next[0]  = 0;  // 测完电压之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[1]  = 0;  // 测完充电之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[2] = 1;  // 测完静流之后跳转 1:允许 0:不允许

        g_tParam.Voltage_auto_trigger = 0;  // 按键自动触发 1:允许 0:不允许
    }
    else if (g_tParam.MODE[0] == 3)  // SEQ_D  电压(自动触发)-充电-静流
    {
        g_tParam.measure_seq[0]  = SEQ_1;  // 电压的测试顺序
        g_tParam.measure_seq[1]  = SEQ_2;  // 充电的测试顺序
        g_tParam.measure_seq[2] = SEQ_3;  // 静流的测试顺序

        g_tParam.switch_to_next[0]  = 1;  // 测完电压之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[1]  = 0;  // 测完充电之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[2] = 0;  // 测完静流之后跳转 1:允许 0:不允许

        g_tParam.Voltage_auto_trigger = 1;  // 按键自动触发 1:允许 0:不允许
    }
    else if (g_tParam.MODE[0] == 4)  // SEQ_E  静流-电压(自动触发)-充电
    {
        g_tParam.measure_seq[0]  = SEQ_2;  // 电压的测试顺序
        g_tParam.measure_seq[1]  = SEQ_3;  // 充电的测试顺序
        g_tParam.measure_seq[2] = SEQ_1;  // 静流的测试顺序

        g_tParam.switch_to_next[0]  = 1;  // 测完电压之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[1]  = 0;  // 测完充电之后跳转 1:允许 0:不允许
        g_tParam.switch_to_next[2] = 0;  // 测完静流之后跳转 1:允许 0:不允许

        g_tParam.Voltage_auto_trigger = 1;  // 按键自动触发 1:允许 0:不允许
    }

    if (g_tParam.MODE[1] >= CURRENT_SUM)
    {
        g_tParam.MODE[1] = 1;
    }
    REF_CURRENT = ref_current[g_tParam.MODE[1]];  // 充电电流的参考校准
    MAX_CURRENT = max_current[g_tParam.MODE[1]];  // 充电电流的最大显示值
		
		SEQ_SUM=getNeedMeasureSum();      // 测试通道数
}

/* 私有函数原型 ***********************************************************************/

/* ************************************ 文件结束 **************************************/

/*------------------------------*/

/*------------------------------*/
