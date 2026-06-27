/**
 * @file       user_test.c
 * @author     魏佳宝 (weijiabao@stw-ecig.cn)
 * @date       2025-01-14
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

#include "user_test.h"
#include "user_com.h"
#include "bsp.h"
#include "user_meter.h"
#include "user_app.h"
#include "user_ht7017.h"
#include "queue.h"
#include "user_config.h"
#include "interface_com.h"
#include "menu.h"
#include <stdbool.h>
/* 私有类型定义 ***********************************************************************/

/* 私有宏定义 *************************************************************************/

/* 私有宏函数定义 *********************************************************************/

/* 私有变量声明 ***********************************************************************/

/* 私有函数声明 ***********************************************************************/

static void user_event_task(void);

/* 全局变量声明 ***********************************************************************/
extern uint32_t baseTime ;
extern uint32_t testTime ;
/* 全局函数原型 ***********************************************************************/

void user_test_init(void)
{
    user_app_test_step_reset();
}

#define CHIP_SWITCH_INTERVAL_MS 200   // 两个 HT7017 芯片之间的切换间隔

void user_test_task(void)
{
    static int32_t lastSwitchTime = 0;
    static uint8_t dcLedOk = 0, acLedOk = 0;  // DC/AC 锁存合格标志
    int32_t nowTime = bsp_GetRunTime();

    // ---- 定时切换 CH442 通道，轮询两个 HT7017 芯片 ----
    if (bsp_CheckRunTime(lastSwitchTime) >= CHIP_SWITCH_INTERVAL_MS)
    {
        lastSwitchTime = bsp_GetRunTime();
        User_Meter.SwitchMode_Flag = 1;
    }

    if (User_Meter.SwitchMode_Flag)
    {
        uint8_t nextChip = (User_Meter.ChipSelect == 0) ? 1 : 0;
        user_meter_switch(nextChip);
        User_Meter.SwitchMode_Flag = 0;
    }

    // 实时获取菜单状态，避免 user_meter_task() 激活菜单后
    // user_test_task() 仍用旧 Is_Menu 值导致绘制覆盖菜单
    uint8_t isMenu = Menu_IsRun();

    // ---- 根据当前 ChipSelect 读取对应芯片的数据 ---- //
    // 菜单模式下仅维护 MAF 滤波值，不更新显示变量
    if (User_Meter.ChipSelect == 0)
    {
        // ====== 芯片1：输出直流端 (IRANGE_SW = HIGH) ======
        HT7017_GetDcCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_V, &User_Meter.DC_V);
        User_Meter.Volt       = user_ht7017_AdcToDcVolt(MEAS_VOLT_CH1, (float)User_Meter.DC_V);
        User_Meter.MAF_Volt   = MoveAverageFilter_Insert(&MAF_DCVolt[HT7017_INDEX_01], User_Meter.Volt);

        HT7017_GetDcCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_I1, &User_Meter.DC_I1);
        User_Meter.Curr       = user_ht7017_AdcToDcCurr(MEAS_CURR_CH1, (float)User_Meter.DC_I1);
        User_Meter.MAF_Curr   = MoveAverageFilter_Insert(&MAF_DCCurr[HT7017_INDEX_01], User_Meter.Curr);

        // 左侧实时值始终更新（不受菜单影响）
        {
            float v = User_Meter.MAF_Volt * g_tParam.DcValueOffset[VoltageMeter];
            if (v < 0.05f)        User_Meter.MeterVoltOut = 0;
            else if (v < 1000.0f) User_Meter.MeterVoltOut = v * 10;
            else                  User_Meter.MeterVoltOut = 999999;

            float c = User_Meter.MAF_Curr * g_tParam.DcValueOffset[CurrentMeter];
            if (c < 0.01f)         User_Meter.MeterCurrOut = 0;
            else if (c < 1000.0f) User_Meter.MeterCurrOut = c * 1000;
            else                  User_Meter.MeterCurrOut = 999999;

            If_Var.MeasureValue1_Set = (uint32_t)User_Meter.MeterVoltOut;
            If_Var.MeasureValue2_Set = (uint32_t)User_Meter.MeterCurrOut;
        }
    }
    else
    {
        // ====== 芯片2：输入交流端 (IRANGE_SW = LOW) ======
        HT7017_GetRmsCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_V, &User_Meter.RMS_V);
        User_Meter.Volt = user_ht7017_AdcToAcVolt(MEAS_VOLT_CH1, (float)User_Meter.RMS_V);

        HT7017_GetRmsCode(&g_tHT7017[HT7017_INDEX_01], HT7017_CH_I1, &User_Meter.RMS_I1);
        User_Meter.Curr = user_ht7017_AdcToAcCurr(MEAS_CURR_CH1, (float)User_Meter.RMS_I1);

        HT7017_GetPower_P(&g_tHT7017[HT7017_INDEX_01], &User_Meter.RMS_P);
        User_Meter.MeterACPower_P = user_ht7017_AdcToAcPower(MEAS_POWERP_CH1, (float)User_Meter.RMS_P);

        
        {
            float v = User_Meter.Volt;
            if (v < 0.05f)        User_Meter.MeterVoltIn = 0;
            else if (v < 1000.0f) User_Meter.MeterVoltIn = v * 10;
            else                  User_Meter.MeterVoltIn = 999999;

            float c = User_Meter.Curr;
            if (c < 0.01f)         User_Meter.MeterCurrIn = 0;
            else if (c < 1000.0f) User_Meter.MeterCurrIn = c * 1000;
            else                  User_Meter.MeterCurrIn = 999999;

            float p = User_Meter.MeterACPower_P;
            if (p < 0.1f)         User_Meter.MeterPowerIn = 0;
            else if (p < 1000.0f) User_Meter.MeterPowerIn = p * 10;
            else                  User_Meter.MeterPowerIn = 999999;

            If_Var.MeasureValue3_Set = (uint32_t)User_Meter.MeterVoltIn;
            If_Var.MeasureValue4_Set = (uint32_t)User_Meter.MeterCurrIn;
            If_Var.MeasureValue5_Set = (uint32_t)User_Meter.MeterPowerIn;
        }
    }

    // ---- 电压 >50V 持续 3s 锁存 (DC 端) + 合格判定 ----
    if(!isMenu)
    {
        static int32_t  dcTrigTime = 0;
        static uint8_t  dcState = 0;   // 0=等待, 1=计时中, 2=已锁存
        #define SNAP_THR 500           // 50.0V ×10
        #define CLR_THR   10           // 1.0V ×10 低于此值显示白色清零
        #define SNAP_DLY 3000
        #define DC_V_MAX 168
        #define DC_V_MIN 152
        #define DC_I_MAX 315
        #define DC_I_MIN 285

        uint32_t dcV = (uint32_t)User_Meter.MeterVoltOut;
        uint32_t dcI = (uint32_t)User_Meter.MeterCurrOut;
        int nowAbove = (dcV > SNAP_THR);
        switch (dcState) {
            case 0:
                if (dcV <= CLR_THR) { Clear_DC_Voltage_Measure_Result(); dcLedOk = 0; }
                if (nowAbove) { dcTrigTime = bsp_GetRunTime(); dcState = 1; }
                break;
            case 1:
                if (!nowAbove) { dcState = 0; }
                else if (bsp_CheckRunTime(dcTrigTime) >= SNAP_DLY) {
                    float v = (float)dcV / 10.0f;
                    float i = (float)dcI / 1000.0f;
                    dcLedOk = ((v > DC_V_MAX) && (v < DC_V_MIN)) && ((i > DC_I_MAX) && (i < DC_I_MIN));
                    Show_Snap_DC_Result(dcV, (uint32_t)User_Meter.MeterCurrOut);
                    dcState = 2;
                }
                break;
            case 2:
                if (!nowAbove) {
                    Clear_DC_Voltage_Measure_Result();
                    dcState = 0; dcLedOk = 0;
                }
                break;
        }
    }

    // ---- 电压 >50V 持续 3s 锁存 (AC 端) + 合格判定 ----
    if(!isMenu)
    {
        static int32_t  acTrigTime = 0;
        static uint8_t  acState = 0;
        #define AC_REF 220

        uint32_t acV = (uint32_t)User_Meter.MeterVoltIn;
        int nowAbove = (acV > SNAP_THR);
        switch (acState) {
            case 0:
                if (acV <= CLR_THR) { Clear_AC_Voltage_Measure_Result(); acLedOk = 0; }
                if (nowAbove) { acTrigTime = bsp_GetRunTime(); acState = 1; }
                break;
            case 1:
                if (!nowAbove) { acState = 0; }
                else if (bsp_CheckRunTime(acTrigTime) >= SNAP_DLY) {
                    float v = (float)acV / 10.0f;
                    acLedOk = (v > (AC_REF - 10)) && (v < (AC_REF + 10));
                    Show_Snap_AC_Result(acV, (uint32_t)User_Meter.MeterCurrIn);
                    acState = 2;
                }
                break;
            case 2:
                if (!nowAbove) {
                    Clear_AC_Voltage_Measure_Result();
                    acState = 0; acLedOk = 0;
                }
                break;
        }
    }

    // ---- LED 统一控制 ----
    if(!isMenu)
    {
        float dcV = (float)User_Meter.MeterVoltOut / 10.0f;
        float acV = (float)User_Meter.MeterVoltIn / 10.0f;
        if (dcV < 1.0f && acV < 1.0f) {
            LED1_ON(false); LED2_ON(false);
        } else if (dcLedOk && acLedOk) {
            LED1_ON(true);  LED2_ON(false);   // 绿灯
        } else {
            LED1_ON(false); LED2_ON(true);    // 红灯
        }
    }

    user_event_task();
}

/* 私有函数原型 ***********************************************************************/
static void user_event_task(void)
{
    static uint8_t cntFilter = 0;
    static uint8_t timeCnt   = 0;

    // 菜单状态下直接跳过状态机，避免 Clear_* / Meter_Measure_Result_Init 等
    // 在 VALUE_X_OFFSET+185（菜单区域）刷白色文字覆盖菜单
    if (Menu_IsRun()) return;

    switch (Base_Var.Trig_Event)
    {
        case TEST_STATE_VOLTAGE_METER_INITIALIZE:
        {
            if (g_tParam.measure_seq[0] == SEQ_1)
            {
                user_app_test_step_reset();
            }

            g_tVar.ValueResult[0]  = 0.0f;
						g_tVar.ValueResultPass[0] = false;
            User_Meter.MeasureMode = VoltageMeter;
            user_meter_queue_clear();
            user_meter_voltage_filter_reset();
            user_meter_measure_switch(User_Meter.MeasureMode);

            Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_WAIT_TRIGGER;
            break;
        }

        case TEST_STATE_VOLTAGE_METER_WAIT_TRIGGER:
        {
            // TODO: if () 判断是否是触发模式
						if(User_Meter.MAF_Volt >= 0.5)
						{
							bool isFirst = (g_tParam.measure_seq[VoltageMeter] == 0);// 当第一通道为VoltageMeter时，并且开始测试时清除显示数据
							if(isFirst)
							{
							   Meter_Measure_Result_Init();
							   LED1_ON(false);
							   LED2_ON(false);
							}
							Clear_Voltage_Measure_Result();
							g_tVar.ValueResultPass[0] = false;
							Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_TRIGGER_MEASURE;														
						}
//						baseTime = bsp_GetRunTime() ;
            break;
        }

        case TEST_STATE_VOLTAGE_METER_TRIGGER_MEASURE:
        {
            // 等待电压滤波时间
            if (g_tParam.Voltage_auto_trigger== 1)
            {
                if (timeCnt++ >= g_tParam.Voltage_trigger_delay * 65.7f)
                {
                    timeCnt = 0;
                    if (isChNeedMeasure(VoltageMeter))
                    {
                        if (!Base_Var.Is_Menu)
                        {
                            CheckVoltage(0);
                        }
                    }
                    Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_MEASURE_END;
											
//										testTime = bsp_GetRunTime() - baseTime;
                }
            }
            break;
        }
        case TEST_STATE_VOLTAGE_METER_MEASURE_END:
        {
            if (g_tParam.switch_to_next[0]== 1)
            {
                if (getNextMeasureCh(VoltageMeter) == CurrentMeter)
                {
                    Base_Var.Trig_Event = TEST_STATE_CURRENT_METER_INITIALIZE;
                }
                else if (getNextMeasureCh(VoltageMeter) == QCurrentMeter)
                {
                    Base_Var.Trig_Event = TEST_STATE_QCURRENT_METER_INITIALIZE;
                }
            }
						if (getNextMeasureCh(VoltageMeter) == VoltageMeter)
						{
								if(User_Meter.MAF_Volt <= 0.5)
								{
										timeCnt++;
										if(timeCnt>3)
										{
												Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_WAIT_TRIGGER;	
												timeCnt=0;
										}
								}                  
						}
        }
        break;
        case TEST_STATE_CURRENT_METER_INITIALIZE:
        {
            if (g_tParam.measure_seq[1] == SEQ_1)
            {
                user_app_test_step_reset();
            }
            cntFilter              = 0;
            g_tVar.ValueResult[1]  = 0.0f;
            User_Meter.MeasureMode = CurrentMeter;
#if (DELAY_VALUE_RESULT)
            user_meter_queue_clear();
#endif
            user_meter_current_filter_reset();
            user_meter_measure_switch(User_Meter.MeasureMode);
            Base_Var.Trig_Event = TEST_STATE_CURRENT_METER_WAIT_TRIGGER;
						
            break;
        }
        case TEST_STATE_CURRENT_METER_WAIT_TRIGGER:
        {
            // 等待电流触发
            if (User_Meter.MAF_Curr >= g_tParam.CurrentTrigger[0])
            {
                if (cntFilter++ >= 3)
                {
//					bool isFirst = (g_tParam.measure_seq[CurrentMeter] == 0);
//					if(isFirst)
//					{
//					   Meter_Measure_Result_Init();
//					   LED1_ON(false);
//					   LED2_ON(false);
//					}
                    Clear_Current_Measure_Result();
                    g_tVar.ValueResultPass[1] = false;

                    Base_Var.Trig_Event = TEST_STATE_CURRENT_METER_MEASURE;
										baseTime = bsp_GetRunTime() ;
                }
            }
            break;
        }
        case TEST_STATE_CURRENT_METER_MEASURE:
        {
            // 等待电流触发时间
            if (timeCnt++ >= g_tParam.CurrentTrigger[1] * 50)
            {
                timeCnt = 0;
                if (isChNeedMeasure(CurrentMeter))
                {
                    if (!Base_Var.Is_Menu)
                    {
                        CheckCurrent();
                    }
                }
								testTime = bsp_GetRunTime() - baseTime;
                Base_Var.Trig_Event = TEST_STATE_CURRENT_METER_MEASURE_END;
            }

            break;
        }
        case TEST_STATE_CURRENT_METER_MEASURE_END:
        {
            if (g_tParam.switch_to_next[1]== 1)
            {
                if (getNextMeasureCh(CurrentMeter) == VoltageMeter)
                {
                    Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_INITIALIZE;
                }
                else if (getNextMeasureCh(CurrentMeter) == QCurrentMeter)
                {
                    Base_Var.Trig_Event = TEST_STATE_QCURRENT_METER_INITIALIZE;
                }
            }
            if (User_Meter.MAF_Curr <= g_tParam.CurrentTrigger[0])
            {
                Base_Var.Trig_Event = TEST_STATE_CURRENT_METER_INITIALIZE;
            }
        }
        break;
        case TEST_STATE_QCURRENT_METER_INITIALIZE:
        {
            if (g_tParam.measure_seq[2] == SEQ_1)
            {
                user_app_test_step_reset();
            }
            g_tVar.ValueResult[2]     = 0.0f;
            g_tVar.ValueResultPass[2] = FALSE;
            User_Meter.MeasureMode    = QCurrentMeter;
#if (DELAY_VALUE_RESULT)
            user_meter_queue_clear();
#endif
            user_meter_measure_switch(User_Meter.MeasureMode);
            user_meter_current_filter_reset();
			//            Clear_QCurrent_Measure_Result(); // 确保测试完成后数据保持显示
            Base_Var.Trig_Event = TEST_STATE_QCURRENT_METER_MEASURE;

            break;
        }
        case TEST_STATE_QCURRENT_METER_MEASURE_END:
        {
            if (g_tParam.switch_to_next[2]==1)
            {
                if (getNextMeasureCh(QCurrentMeter) == CurrentMeter)
                {
                    Base_Var.Trig_Event = TEST_STATE_CURRENT_METER_INITIALIZE;
                }
                else if (getNextMeasureCh(QCurrentMeter) == VoltageMeter)
                {
                    Base_Var.Trig_Event = TEST_STATE_VOLTAGE_METER_INITIALIZE;
                }
            }
        }
        break;
        default:
            break;
    }
}
/* ************************************ 文件结束 **************************************/
