/**
 * @file       app_meter.c
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-09
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

#include "user_meter.h"
#include "user_config.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "param.h"
#include "ma_filter.h"
#include "queue.h"
#include "usart.h"
#include "ringbuffer.h"
#include "bsp.h"
#include "menu.h"
#include "user_config.h"
#include "user_com.h"
#include "user_app.h"
#include "interface_com.h"
#include "user_menu.h"
#include "user_test.h"
#include "user_ht7017.h"
/* 私有类型定义 ***********************************************************************/

#pragma pack(1)
typedef struct
{
    uint8_t  head;
    uint16_t deviceType;
    uint16_t deviceId;
    uint8_t  funcType;
    uint16_t dataLength;
    uint8_t *data;
    uint16_t crc;
} SProtocol;
#pragma pack()  // 取消自定义字节对齐方式

enum
{
    NO_ACK = 0x00,
    ACK    = 0x01,
};

enum
{
    RES_OK      = 0x00,
    RES_ERROR   = 0x01,
    RES_TIMEOUT = 0x02,
};

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

#define BUILD_RESPONSE_PACKET(PTR, RES) *(PTR + 8) = (uint8_t)RES

/* 私有变量声明 ***********************************************************************/

// ht7017_handle_t g_tHT7017[HT7017_INDEX_MAX_SIZE] = {0};
MA_FILTER_T MAF_DCVolt[HT7017_INDEX_MAX_SIZE];
MA_FILTER_T MAF_DCCurr[HT7017_INDEX_MAX_SIZE];

uint8_t timeCnt            = 0;
uint8_t currentTriggerFlag = 0;
// uint8_t  testStep           = TEST_STATE_IDEL;
uint8_t cntFilter = 0;

char strValue[32]     = {0};
char voltStrValue[32] = {0};
char currStrValue[32] = {0};

#if (DELAY_VALUE_RESULT)
QUEUE_HandleTypeDef qValue;
QUEUE_DATA_T        BufferValue[Q_VALUE_BUFFER_SIZE];
#endif

/* 私有函数声明 ***********************************************************************/

uint8_t meter_ht7017_init(void);

/* 全局变量声明 ***********************************************************************/

User_Meter_T User_Meter = {0};

/* 全局函数原型 ***********************************************************************/

void user_meter_measure_switch(uint8_t func)
{
    // //    baseTime = bsp_GetRunTime();
    // //    testTime = 0;
    // static uint8_t lastFunc = 0;

    // if (func <= QCurrentMeter)
    // {
    //     if (func == VoltageMeter)
    //     {
    //         HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_RESET);
    //         HAL_GPIO_WritePin(VSAMPLE_EN_GPIO_Port, VSAMPLE_EN_Pin, GPIO_PIN_SET);

    //         g_tVar.Volt_On[0] = 1;
    //         g_tVar.Curr_On[0] = 0;
    //     }
    //     else if (func == CurrentMeter)
    //     {
    //         HAL_GPIO_WritePin(VSAMPLE_EN_GPIO_Port, VSAMPLE_EN_Pin, GPIO_PIN_RESET);
    //         HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_RESET);

    //         g_tVar.Volt_On[0] = 0;
    //         g_tVar.Curr_On[0] = 1;
    //     }
    //     else if (func == QCurrentMeter)
    //     {
    //         HAL_GPIO_WritePin(VSAMPLE_EN_GPIO_Port, VSAMPLE_EN_Pin, GPIO_PIN_RESET);
    //         HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_SET);

    //         g_tVar.Volt_On[0] = 0;
    //         g_tVar.Curr_On[0] = 1;
    //     }
    // }
    // else if (func <= VoltAndQCurrMeter)
    // {
    //     LED1_ON(false);
    //     LED2_ON(false);

    //     if (func == VoltAndCurrMeter)
    //     {
    //         HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_RESET);
    //         HAL_GPIO_WritePin(VSAMPLE_EN_GPIO_Port, VSAMPLE_EN_Pin, GPIO_PIN_SET);
    //     }
    //     else if (func == VoltAndQCurrMeter)
    //     {
    //         HAL_GPIO_WritePin(VSAMPLE_EN_GPIO_Port, VSAMPLE_EN_Pin, GPIO_PIN_SET);
    //         HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_SET);
    //     }

    //     g_tVar.Volt_On[0] = 1;
    //     g_tVar.Curr_On[0] = 1;
    // }

    // lastFunc = func;
}

/**
 * @brief 切换电压测量模式
 * @param  func 功能类型
 */
void user_meter_switch(uint8_t chipSelect)
{
    User_Meter.ChipSelect = chipSelect;
    if (chipSelect == 0)   // 芯片1 输出直流端
    {
        HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_SET);
        HAL_Delay(5);
    }
    else                   // 芯片2 输入交流端
    {
        HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_RESET);
        HAL_Delay(5);
    }
}

void user_meter_queue_clear(void)
{
#if (DELAY_VALUE_RESULT)
    Queue_Clear(&qValue);
#endif
}

void user_meter_current_filter_reset(void)
{
    MoveAverageFilter_ClearBuffer(&MAF_DCCurr[HT7017_INDEX_01]);
}

void user_meter_voltage_filter_reset(void)
{
    MoveAverageFilter_ClearBuffer(&MAF_DCVolt[HT7017_INDEX_01]);
}
/**
 * @brief  表头初始化函数
 *
 */
void user_meter_init(void)
{
    meter_ht7017_init();

    g_tVar.Volt_On[0]      = 1;
    g_tVar.Curr_On[0]      = 1;
    User_Meter.MeasureMode = get_InitMeasureChannel();
		if(User_Meter.MeasureMode==CurrentMeter)
		{
			Base_Var.Trig_Event = TEST_STATE_CURRENT_METER_INITIALIZE;
		}
		if(User_Meter.MeasureMode==VoltageMeter&&g_tParam.Voltage_auto_trigger== 1)
		{
			Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_INITIALIZE;
		}
    user_meter_measure_switch(User_Meter.MeasureMode);

#if (DELAY_VALUE_RESULT)
    Queue_Init(&qValue, BufferValue, Q_VALUE_BUFFER_SIZE);
#endif
}

/**
 * @brief 表头任务处理函数
 *
 */
void user_meter_task(void)
{
    if (Base_Var.Key_Trig_Event & EVENT_METER_KEY_UP)
    {
        uCLEAR_EVENT(EVENT_METER_KEY_UP | EVENT_METER_KEY_DOWN);
        g_tParam.VoltageMode = !g_tParam.VoltageMode;
        //(User_Meter.MeasureMode >= QCurrentMeter) ? User_Meter.MeasureMode = VoltageMeter : User_Meter.MeasureMode++;
    }
    else if (Base_Var.Key_Trig_Event & EVENT_METER_KEY_DOWN)
    {
        uCLEAR_EVENT(EVENT_METER_KEY_UP | EVENT_METER_KEY_DOWN);
        //User_Meter.VoltMode = !User_Meter.VoltMode;
        if(!g_tParam.VoltageMode)User_Meter.MeasureMode = !User_Meter.MeasureMode;
        else 
        {
            if(User_Meter.MeasureMode == CurrentMeter) User_Meter.MeasureMode = ACPowerMeter;
            else User_Meter.MeasureMode = !User_Meter.MeasureMode;
        }
        //(User_Meter.MeasureMode == VoltageMeter) ? User_Meter.MeasureMode = QCurrentMeter : User_Meter.MeasureMode++;
        user_meter_measure_switch(User_Meter.MeasureMode);
    }
    else if (Base_Var.Key_Trig_Event & EVENT_MENU_KEY_OK)
    {
        uCLEAR_EVENT(EVENT_MENU_KEY_OK);
    }
    else if (Base_Var.Key_Trig_Event & EVENT_MENU_ENTER)
    {
        uCLEAR_EVENT(EVENT_MENU_ENTER);
        UI_SwitchWindow(&Menu_Window);
        user_menu_init();
    }
    else if (Base_Var.Key_Trig_Event & EVENT_MENU_KEY_UP)
    {
        uCLEAR_EVENT(EVENT_MENU_KEY_DOWN | EVENT_MENU_KEY_UP);
    }
    else if (Base_Var.Key_Trig_Event & EVENT_MENU_KEY_DOWN)
    {
        uCLEAR_EVENT(EVENT_MENU_KEY_DOWN | EVENT_MENU_KEY_UP);
    }

    // 更新变量
    If_Var.MeasureMode_Set = User_Meter.MeasureMode;
    If_Var.VoltMode_Set = g_tParam.VoltageMode;



}

SProtocol sProtocol;

uint8_t buffer[256]   = {0};
uint8_t txBuffer[256] = {0};

static bool protocol_payload_available(uint16_t frameLength, uint16_t requiredLength)
{
    if (frameLength < 10)
    {
        return false;
    }

    if (sProtocol.dataLength != (uint16_t)(frameLength - 10))
    {
        return false;
    }

    return sProtocol.dataLength >= requiredLength;
}

static bool is_valid_measure_mode(uint8_t mode)
{
    return mode < MaxFunc;
}

static bool is_valid_measure_sequence(const uint8_t *sequence)
{
    uint8_t activeCount = 0;
    bool    seen[CH_SUM] = {false};

    for (uint8_t i = 0; i < CH_SUM; i++)
    {
        if (sequence[i] == SEQ_IDLE)
        {
            continue;
        }
        if (sequence[i] >= CH_SUM)
        {
            return false;
        }
        if (seen[sequence[i]])
        {
            return false;
        }
        seen[sequence[i]] = true;
        activeCount++;
    }

    if (activeCount == 0)
    {
        return false;
    }

    for (uint8_t i = 0; i < activeCount; i++)
    {
        if (!seen[i])
        {
            return false;
        }
    }

    return true;
}

void user_meter_uart_task()
{
    // 一般要在这里判断一下，是否已经接收到数据，防止误触发
    // 经测试发现在开机刚开始使能idle中断的时候，会触发一次idle中断。
    uint8_t ack       = 0;
    int     resLength = 0;

    HAL_NVIC_DisableIRQ(USART2_IRQn);
    uint16_t len = 0;
    if (RingCountBytes(&xBufferRX) >= 1)
    {
        len = RingGetAllBytes(&xBufferRX, buffer);
    }
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    if (len >= 1)
    {
        // if ((len >= 10) && (len <= 246))
        if (len >= 10)
        {
            sProtocol.head       = buffer[0];
            sProtocol.deviceType = BEBufToUint16(&buffer[1]);
            sProtocol.deviceId   = BEBufToUint16(&buffer[3]);
            sProtocol.funcType   = buffer[5];
            sProtocol.dataLength = BEBufToUint16(&buffer[6]);
            sProtocol.data       = &buffer[8];
            sProtocol.crc        = buffer[len - 2] << 8 | buffer[len - 1];

            //                __CHECK_PROTOCOL_HEAD()
            if (sProtocol.head == 0xAA)
            {
                uint16_t crc = CRC16_Modbus(buffer, len - 2);
                if (crc == sProtocol.crc)
                {
                    if ((sProtocol.deviceType == 0x0001) && sProtocol.deviceId == 0x0000)
                    {
                        if (sProtocol.funcType == 0x02)  // 设置测试通道 - 广播模式不应答
                        {
                        }
                    }
                    if ((sProtocol.deviceType == 0x0001) && sProtocol.deviceId == g_tParam.Id)
                    {
                        if (sProtocol.funcType == 0x01)  // 查询设备在线
                        {
                            uint8_t *pData         = &txBuffer[8];
                            *(pData + resLength++) = 0x01;
                            ack                    = ACK;
                        }
                        else if (sProtocol.funcType == 0x02)  // 设置测试通道
                        {
                            if (protocol_payload_available(len, 1) && is_valid_measure_mode(sProtocol.data[0]))
                            {
                                User_Meter.MeasureMode = sProtocol.data[0];
                                g_tVar.NeedMeasureFunc = true;

                                uint8_t *pData         = &txBuffer[8];
                                *(pData + resLength++) = 0x01;
                                ack                    = ACK;
                            }
                        }
                        else if (sProtocol.funcType == 0x03)  // 查询表头值
                        {
                            uint8_t *pData   = &txBuffer[8];
                            uint8_t  channel = User_Meter.MeasureMode;

                            if (protocol_payload_available(len, 0) && channel <= QCurrentMeter)
                            {
                                *(pData + resLength++) = channel;

                                uint32_t tmp           = Get32BitOfFloat(g_tVar.MAF_DcValue[channel]);
                                *(pData + resLength++) = (uint8_t)(tmp >> 24 & 0xFF);
                                *(pData + resLength++) = (uint8_t)(tmp >> 16 & 0xFF);
                                *(pData + resLength++) = (uint8_t)(tmp >> 8 & 0xFF);
                                *(pData + resLength++) = (uint8_t)(tmp & 0xFF);

                                ack = ACK;
                            }
                        }
                    }
                    else if ((sProtocol.deviceType == 0x0001) && sProtocol.deviceId == 0xFFFF)  // 上位机专用
                    {
                        if (sProtocol.funcType == 0x01)
                        {
                            resLength = sprintf((char *)&txBuffer[8], "EgoMeter - %03d - %s %s\r\n", g_tParam.Id, __DATE__, __TIME__);
                            ack       = ACK;
                        }
                        else if (sProtocol.funcType == 0x02)  // 版本号查询
                        {
                            uint8_t *pData         = &txBuffer[8];
                            *(pData + resLength++) = (SOFTWARE_VERSION >> 16) & 0xff;
                            *(pData + resLength++) = (SOFTWARE_VERSION >> 8) & 0xff;
                            *(pData + resLength++) = SOFTWARE_VERSION & 0xff;
                            *(pData + resLength++) = (HARDWARE_VERSION >> 16) & 0xff;
                            *(pData + resLength++) = (HARDWARE_VERSION >> 8) & 0xff;
                            *(pData + resLength++) = HARDWARE_VERSION & 0xff;
                            ack                    = ACK;
                        }
                        else if ((sProtocol.funcType == 0x03) || (sProtocol.funcType == 0x04))  // 查询当前测试值
                        {
                            uint8_t *pData   = &txBuffer[8];
                            uint8_t  channel = User_Meter.MeasureMode;
                            float   *value   = g_tVar.MAF_DcValue;

                            if (sProtocol.funcType == 0x04)  // 查询快速电压电流值，不经过平均滤波的值
                            {
                                value = g_tVar.DcValue;
                            }

                            *(pData + resLength++) = channel;

                            if (channel <= QCurrentMeter)
                            {
                                uint32_t tmp           = Get32BitOfFloat(value[channel]);
                                *(pData + resLength++) = (uint8_t)(tmp >> 24 & 0xFF);
                                *(pData + resLength++) = (uint8_t)(tmp >> 16 & 0xFF);
                                *(pData + resLength++) = (uint8_t)(tmp >> 8 & 0xFF);
                                *(pData + resLength++) = (uint8_t)(tmp & 0xFF);

                                ack = ACK;
                            }
                            else if (channel < MaxFunc)
                            {
                                uint16_t tmp = 0;
                                if (value[VoltageMeter] >= 0.0f)
                                {
                                    tmp = (uint16_t)((value[VoltageMeter] + 0.0005) * 1000);
                                }
                                *(pData + resLength++) = (uint8_t)(tmp >> 8 & 0xFF);
                                *(pData + resLength++) = (uint8_t)(tmp & 0xFF);

                                uint8_t currentCh = CurrentMeter;
                                if (channel == VoltAndQCurrMeter)
                                {
                                    currentCh = QCurrentMeter;
                                }
                                tmp = 0;
                                if (value[currentCh] >= 0.0f)
                                {
                                    tmp = (uint16_t)((value[currentCh] + 0.0005) * 1000);
                                }
                                *(pData + resLength++) = (uint8_t)(tmp >> 8 & 0xFF);
                                *(pData + resLength++) = (uint8_t)(tmp & 0xFF);

                                ack = ACK;
                            }
                            else
                            {
                                ack = NO_ACK;
                            }
                        }
                        else if (sProtocol.funcType == 0x10)  // 测试范围查询
                        {
                            uint8_t *pData         = &txBuffer[8];
                            uint16_t tmp           = (uint16_t)(g_tParam.VoltageRange[0] * 1000.0f);
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            tmp                    = (uint16_t)(g_tParam.VoltageRange[1] * 1000);
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            tmp                    = (uint16_t)(g_tParam.CurrentRange[0] * 1000);
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            tmp                    = (uint16_t)(g_tParam.CurrentRange[1] * 1000);
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            tmp                    = (uint16_t)(g_tParam.QCurrentRange[0] * 10);
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            tmp                    = (uint16_t)(g_tParam.QCurrentRange[1] * 10);
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            tmp                    = (uint16_t)(g_tParam.CurrentTrigger[0] * 1000);
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            tmp                    = (uint16_t)(g_tParam.CurrentTrigger[1] * 1000);
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            tmp                    = (uint16_t)g_tParam.Id;
                            *(pData + resLength++) = tmp >> 8;
                            *(pData + resLength++) = tmp & 0xFF;

                            ack = ACK;
                        }
                        else if (sProtocol.funcType == 0x11)  // 测试范围设置
                        {
                            if (protocol_payload_available(len, 18))
                            {
                                g_tParam.VoltageRange[0]   = ((float)BEBufToUint16(&sProtocol.data[0]) / 1000);
                                g_tParam.VoltageRange[1]   = ((float)BEBufToUint16(&sProtocol.data[2]) / 1000);
                                g_tParam.CurrentRange[0]   = ((float)BEBufToUint16(&sProtocol.data[4]) / 1000);
                                g_tParam.CurrentRange[1]   = ((float)BEBufToUint16(&sProtocol.data[6]) / 1000);
                                g_tParam.QCurrentRange[0]  = ((float)BEBufToUint16(&sProtocol.data[8]) / 10);
                                g_tParam.QCurrentRange[1]  = ((float)BEBufToUint16(&sProtocol.data[10]) / 10);
                                g_tParam.CurrentTrigger[0] = ((float)BEBufToUint16(&sProtocol.data[12]) / 1000);
                                g_tParam.CurrentTrigger[1] = ((float)BEBufToUint16(&sProtocol.data[14]) / 1000);
                                g_tParam.Id                = BEBufToUint16(&sProtocol.data[16]);
                                g_tVar.NeedSaveParam       = 1;

                                BUILD_RESPONSE_PACKET(&txBuffer[0], RES_OK);
                                resLength = 1;
                                ack       = ACK;
                            }
                        }
                        else if (sProtocol.funcType == 0x12)  // 测量功能切换
                        {
                            if (protocol_payload_available(len, 1) && is_valid_measure_mode(sProtocol.data[0]))
                            {
                                User_Meter.MeasureMode = sProtocol.data[0];
                                g_tVar.NeedMeasureFunc = true;

                                BUILD_RESPONSE_PACKET(&txBuffer[0], RES_OK);
                                resLength = 1;
                                ack       = ACK;
                            }
                        }
                        else if (sProtocol.funcType == 0x13)  // 菜单跳转
                        {
                            if (protocol_payload_available(len, 3))
                            {
                                g_tVar.MenuJump = BEBufToUint24(&sProtocol.data[0]);

                                BUILD_RESPONSE_PACKET(&txBuffer[0], RES_OK);
                                resLength = 1;
                                ack       = ACK;
                            }
                        }
                        else if (sProtocol.funcType == 0x14)  // 复位校准值
                        {
                            if (User_Meter.MeasureMode == VoltageMeter)
                            {
                                g_tCalib.DcVolt[0].x1 = -0.0f;
                                g_tCalib.DcVolt[0].y1 = 0.0f;
                                g_tCalib.DcVolt[0].x2 = -6000000.0f;
                                g_tCalib.DcVolt[0].y2 = 5.0f;
                            }
                            else if (User_Meter.MeasureMode == CurrentMeter)
                            {
                                g_tCalib.DcCurr[0].x1 = -0.0f;
                                g_tCalib.DcCurr[0].y1 = 0.1f;
                                g_tCalib.DcCurr[0].x2 = -6000000.0f;
                                g_tCalib.DcCurr[0].y2 = ref_current[g_tParam.MODE[1]];
                            }
                            else if (User_Meter.MeasureMode == QCurrentMeter)
                            {
                                g_tCalib.DcCurr[1].x1 = -0.0f;
                                g_tCalib.DcCurr[1].y1 = 0.0f;
                                g_tCalib.DcCurr[1].x2 = -6000000.0f;
                                g_tCalib.DcCurr[1].y2 = 100.0f;
                            }
                            BUILD_RESPONSE_PACKET(&txBuffer[0], RES_OK);
                            ack       = ACK;
                        }
                        else if (sProtocol.funcType == 0x15)
                        {
                            uint8_t *pData         = &txBuffer[8];
                            *(pData + resLength++) = g_tParam.measure_seq[0] & 0xFF;
                            *(pData + resLength++) = g_tParam.measure_seq[1] & 0xFF;
                            *(pData + resLength++) = g_tParam.measure_seq[2] & 0xFF;
                            *(pData + resLength++) = g_tParam.switch_to_next[0] & 0xFF;
                            *(pData + resLength++) = g_tParam.switch_to_next[1] & 0xFF;
                            *(pData + resLength++) = g_tParam.switch_to_next[2] & 0xFF;
                            *(pData + resLength++) = g_tParam.Voltage_auto_trigger & 0xFF;
                            *(pData + resLength++) = (uint16_t)(g_tParam.Voltage_trigger_delay * 1000) >> 8;
                            *(pData + resLength++) = (uint16_t)(g_tParam.Voltage_trigger_delay * 1000) & 0xFF;
                            ack       = ACK;
                        }
                        else if (sProtocol.funcType == 0x16)
                        {
                            if (protocol_payload_available(len, 9))
                            {
                                uint8_t sequence[CH_SUM] = {
                                    (uint8_t)sProtocol.data[0],
                                    (uint8_t)sProtocol.data[1],
                                    (uint8_t)sProtocol.data[2],
                                };
                                if (is_valid_measure_sequence(sequence) &&
                                    sProtocol.data[3] <= 1 &&
                                    sProtocol.data[4] <= 1 &&
                                    sProtocol.data[5] <= 1 &&
                                    sProtocol.data[6] <= 1)
                                {
                                    g_tParam.measure_seq[0]          = sequence[0];
                                    g_tParam.measure_seq[1]          = sequence[1];
                                    g_tParam.measure_seq[2]          = sequence[2];
                                    g_tParam.switch_to_next[0]       = (uint8_t)sProtocol.data[3];
                                    g_tParam.switch_to_next[1]       = (uint8_t)sProtocol.data[4];
                                    g_tParam.switch_to_next[2]       = (uint8_t)sProtocol.data[5];
                                    g_tParam.Voltage_auto_trigger    = (uint8_t)sProtocol.data[6];
                                    g_tParam.Voltage_trigger_delay   = ((float)BEBufToUint16(&sProtocol.data[7]) / 1000);

                                    SEQ_SUM          = getNeedMeasureSum();
                                    g_tParam.MODE[0] = MODE_NULL;
                                    BUILD_RESPONSE_PACKET(&txBuffer[0], RES_OK);
                                    resLength            = 1;
                                    ack                  = ACK;
                                    g_tVar.NeedSaveParam = 1;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (ack && resLength >= 0)
        {
            memcpy(&txBuffer, &buffer, 6);

            txBuffer[6] = ((uint16_t)resLength) >> 8;
            txBuffer[7] = resLength & 0xFF;

            uint16_t totalLength    = 8 + resLength;
            uint16_t crc            = CRC16_Modbus(txBuffer, totalLength);
            txBuffer[totalLength++] = crc >> 8;
            txBuffer[totalLength++] = crc & 0xFF;
            HAL_UART_Transmit(&huart2, txBuffer, totalLength, 200);
        }
    }
}

/* 私有函数原型 ***********************************************************************/
uint8_t meter_ht7017_init(void)
{
    uint8_t          res     = 0;
    ht7017_handle_t *pHT7017 = NULL;

    pHT7017 = &g_tHT7017[HT7017_INDEX_01];

    DRIVER_HT7017_LINK_INIT(pHT7017, ht7017_handle_t);
    DRIVER_HT7017_LINK_UART_INIT(pHT7017, user_ht7017_uart_init);
    DRIVER_HT7017_LINK_UART_DEINIT(pHT7017, user_ht7017_uart_deinit);
    DRIVER_HT7017_LINK_UART_READ(pHT7017, user_ht7017_uart_read);
    DRIVER_HT7017_LINK_UART_WRITE(pHT7017, user_ht7017_uart_write);
    DRIVER_HT7017_LINK_DELAY_US(pHT7017, HAL_Delay);
    DRIVER_HT7017_LINK_DEBUG_PRINT(pHT7017, debug_printf);

    res = HT7017_Init(pHT7017);

    HT7017_SetWP(pHT7017, HT7017_WP_40H_45H);
    HT7017_SetFreq(pHT7017, 0x0074);
    HT7017_SetMode(pHT7017, 0x00);
    HT7017_SetAdcCon(pHT7017, 0x1000);
    HT7017_SetWP(pHT7017, HT7017_WP_ENABLE);

#if 1
    if (g_tParam.HT7017CalibFlag[HT7017_INDEX_01])
    {
        user_ht7017_calib(HT7017_INDEX_01, HT7017_CH_I1, 1);
        user_ht7017_calib(HT7017_INDEX_01, HT7017_CH_V, 1);
        g_tParam.HT7017CalibFlag[HT7017_INDEX_01] = 0;
        SaveParam();
    }
    else
    {
        user_ht7017_calib(HT7017_INDEX_01, HT7017_CH_I1, 0);
        user_ht7017_calib(HT7017_INDEX_01, HT7017_CH_V, 0);
    }
#endif

    // ====== 配置芯片2：输入交流端 (IRANGE_SW = LOW) ======
    // 芯片2 上电后未复位，需要软复位并重新配置
    HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    pHT7017->inited = 0;                       // 重置标志，允许 SWREST
    HT7017_SWREST(pHT7017);
    pHT7017->delay_us(10);
    // 读取芯片2 的 DEVICE_ID 确认通信正常
    {
        uint8_t id2[3] = {0};
        uint8_t r = HT7017_Read(pHT7017, 0x1C, id2, 3);  // DEVICE_ID = 0x1C
        if (r == 0 && id2[0] == 0x70 && id2[1] == 0x53 && id2[2] == 0x21)
        {
            pHT7017->inited = 1;
            HT7017_SetWP(pHT7017, HT7017_WP_40H_45H);
            HT7017_SetFreq(pHT7017, 0x0074);
            HT7017_SetMode(pHT7017, 0x00);
            HT7017_SetAdcCon(pHT7017, 0x1000);
            HT7017_SetWP(pHT7017, HT7017_WP_ENABLE);
        }
    }
    // 注：芯片2 暂不校准，因 g_tParam 中 DcCurrOffset/AcCurrOffset 等字段
    //     声明为 [1]，需扩展数组后才能保存两片芯片的校准值

    // 切回芯片1
    HAL_GPIO_WritePin(IRANGE_SW_GPIO_Port, IRANGE_SW_Pin, GPIO_PIN_SET);
    HAL_Delay(5);

    MoveAverageFilter_Init(&MAF_DCVolt[HT7017_INDEX_01]);
    MoveAverageFilter_Init(&MAF_DCCurr[HT7017_INDEX_01]);
    MoveAverageFilter_Init(&MAF_DCVolt[HT7017_INDEX_02]);
    MoveAverageFilter_Init(&MAF_DCCurr[HT7017_INDEX_02]);

    return res;
}

/* ************************************ 文件结束 **************************************/
