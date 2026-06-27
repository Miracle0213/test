/**
 * Copyright (C) STW Electronics Co. Ltd. 2021-2026 All rights reserved
 *
 * @file      bsp_ht7017.c
 * @brief     none
 * @version   1.0.0
 * @author    Wei-JiaBao
 * @date      2022/3/9
 */

#include "bsp.h"

#ifdef USB_RTOS
#include "cmsis_os2.h"
#endif

//宏定义对应的寄存器地址
#define HT7017_REG_SPL_I1 0x00
#define HT7017_REG_SPL_I2 0x01
#define HT7017_REG_SPL_V 0x02
#define HT7017_REG_I_DC 0x03
#define HT7017_REG_V_DC 0x04
// 0X05
#define HT7017_REG_RMS_I1 0x06
#define HT7017_REG_RMS_I2 0x07
#define HT7017_REG_RMS_V 0x08
#define HT7017_REG_FREQ_U 0x09

#define HT7017_REG_POWER_P 0x0A  //第一通道的有功功率寄存器
#define HT7017_REG_POWER_Q 0x0B  //第二通道的无功功率寄存器


#define HT7017_REG_CHIP_ID 0x1B  //芯片ID
#define HT7017_REG_DEVICE_ID 0x1C  //设备ID
#define HT7017_REG_WP 0x32
#define HT7017_REG_SREST 0x33
#define HT7017_REG_EMU_CFG 0x40
#define HT7017_REG_FREQ_CFG 0x41  //时钟配置寄存器
#define HT7017_REG_MODULE_EN 0x42
#define HT7017_REG_ANAEN 0x43
#define HT7017_REG_ADC_CON 0x59
#define HT7017_REG_I2_GAIN 0x5B
#define HT7017_REG_I1_DC_OFFSET 0x5C
#define HT7017_REG_I2_DC_OFFSET 0x5D
#define HT7017_REG_V_DC_OFFSET 0x5E
#define HT7017_REG_I1_RMS_OFFSET 0x69
#define HT7017_REG_I2_RMS_OFFSET 0x6A
#define HT7017_REG_V_RMS_OFFSET 0x6B

uint8_t HT7017_Init(ht7017_handle_t *handle)  //HT7017初始化
{
    uint8_t res = 0, id[3] = { 0 };

    if (handle == NULL) /* check handle */
    {
        return 1; /* return error */
    }
    if (handle->debug_print == NULL) /* check debug_print */
    {
        return 2; /* return error */
    }
    if (handle->uart_init == NULL) /* check uart_init */
    {
        handle->debug_print("ht7017: uart_init is null.\n"); /* uart_init is null */

        return 3; /* return error */
    }
    if (handle->uart_deinit == NULL) /* check uart_init */
    {
        handle->debug_print("ht7017: uart_deinit is null.\n"); /* uart_deinit is null */

        return 4; /* return error */
    }
    if (handle->uart_read == NULL) /* check uart_read */
    {
        handle->debug_print("ht7017: uart_read is null.\n"); /* uart_read is null */

        return 5; /* return error */
    }
    if (handle->uart_write == NULL) /* check uart_write */
    {
        handle->debug_print("ht7017: uart_write is null.\n"); /* uart_write is null */

        return 6; /* return error */
    }
    if (handle->delay_us == NULL) /* check delay_ms */
    {
        handle->debug_print("ht7017: delay_us is null.\n"); /* delay_ms is null */

        return 7; /* return error */
    }

    if (handle->uart_init()) /* uart init */
    {
        handle->debug_print("ht7017: uart init failed.\n"); /* uart init failed */

        return 8; /* return error */
    }

    res = HT7017_SWREST(handle);
    handle->delay_us(10);

    res = HT7017_Read(handle, HT7017_REG_DEVICE_ID, (uint8_t *) &id[0], 3);
    if (res) /* check result */
    {
        handle->debug_print("ht7017: read id failed.\n"); /* read id failed */
        handle->uart_deinit(); /* uart deinit */

        return 9; /* return error */
    }
    if ((id[0] != 0x70) || (id[1] != 0x53) || (id[2] != 0x21)) /* check id */
    {
        handle->debug_print("ht7017: id is error.\n"); /* id is error */
        handle->uart_deinit(); /* uart deinit */

        return 10; /* return error */
    }
    handle->inited = 1; /* flag finish initialization */

    return 0; /* success return 0 */
}

uint8_t HT7017_DeInit(ht7017_handle_t *handle)
{
    volatile uint8_t res, prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* return error */
    }
    if (handle->inited != 1) /* check handle initialization */
    {
        return 3; /* return error */
    }

    // res = handle->uart_read(ht7017_REG_ENABLE, (uint8_t *)&prev, 1);       /* read enable */
    // if (res)                                                                                  /* check result */
    // {
    //     handle->debug_print("ht7017: read register failed.\n");                             /* read register failed */

    //     return 1;                                                                             /* return error */
    // }
    // prev &= ~(1 << 0);                                                                        /* disable */
    // if (handle->uart_write(ht7017_REG_ENABLE, (uint8_t *)&prev, 1))        /* write enable */
    // {
    //      handle->debug_print("ht7017: write register failed.\n");                           /* write register failed */

    //     return 1;                                                                             /* return error */
    // }
    //

    if (handle->uart_deinit()) /* uart deinit */
    {
        handle->debug_print("ht7017: uart deinit failed.\n"); /* uart deinit failed */

        return 1; /* return error */
    }
    handle->inited = 0; /* flag close */

    return 0;
}


//HT7017写数据
uint8_t HT7017_Write(ht7017_handle_t *handle, uint8_t reg, const uint8_t *data, uint16_t len)
{

    uint8_t res = 0;
    uint8_t buf[32] = { 0 };
    uint8_t check_sum = 0;

    buf[0] = 0x6A;
    buf[1] = reg | 0x80;

    check_sum = buf[0] + buf[1];

    for (int i = 0; i < len; ++i)
    {
        buf[2 + i] = *(data + i);
        check_sum += buf[2 + i];
    }

    buf[len + 2] = ~check_sum;

    if (handle->uart_write(&buf[0], len + 3, 100) != 0)
    {
        //        return 1;
        res = 1;
        return res;
    }

    if (handle->uart_read(&buf[0], 1, 100) != 0)
    {
        res = 2;
        return res;
    }
    handle->delay_us(1);

    if (buf[0] != 0x54)
    {
        res = 3;
        return res;
    }

    return res;
}


//HT7017读数据
uint8_t HT7017_Read(ht7017_handle_t *handle, uint8_t reg, uint8_t *data, uint16_t len)
{
    uint8_t res = 0;
    uint8_t buf[32] = { 0 };
    uint8_t check_sum = 0;

    buf[0] = 0x6A;
    buf[1] = reg;

    check_sum = buf[0] + buf[1];

    if (handle->uart_write(&buf[0], 2, 1000) != 0)
    {
        res = 1;
        return res;
    }

    // DATA + CHECKSUM = len+1
    if (handle->uart_read(&buf[0], len + 1, 1000) != 0)
    {
        res = 2;
        return res;
    }

    for (int i = 0; i < len; ++i)
    {
        data[i] = *(buf + i);
        check_sum += data[i];
    }
    check_sum = ~check_sum;
    if (buf[len] != check_sum)
    {
        res = 3;
        return res;
    }

    return res;
}

//HT7017连续读取数据
uint8_t HT7017_MultipleRead(ht7017_handle_t *handle, uint8_t regStart, uint8_t regEnd, uint8_t *data, uint16_t len)
{
    uint8_t res = 0;
    uint8_t buf[32] = { 0 };
    uint8_t check_sum;

    buf[0] = 0x6A;
    buf[1] = 0xC6;
    buf[2] = regStart;
    buf[3] = regEnd;

    check_sum = buf[0] + buf[1] + buf[2] + buf[3];
    buf[4] = ~check_sum;

    if (handle->uart_write(&buf[0], 5, 1000) != 0)
    {
        res = 1;
        return res;
    }

    // DATA + CHECKSUM = len + 1
    if (handle->uart_read(&buf[0], 1, 1000) != 0)
    {
        res = 2;
        return res;
    }

    if (buf[0] != 0x54)
    {
        res = 3;
        return res;
    }
    else
    {
        if (handle->uart_read(&buf[0], len + 1, 1000) != 0)
        {
            res = 4;
            return res;
        }

        check_sum = 0;
        for (int i = 0; i < len; ++i)
        {
            data[i] = *(buf + i);
            check_sum += data[i];
        }

        check_sum = ~check_sum;
        if (buf[len] != check_sum)
        {
            res = 5;
            return res;
        }
    }

    return res;
}

// 获取期间ID信息，仅用于调试
uint8_t ID_buf[3];
uint32_t ID_data = 0;
uint8_t HT7017_Get_ID(ht7017_handle_t *handle)
{

    uint8_t res = 0;
    res = HT7017_Read(handle, HT7017_REG_CHIP_ID, &ID_buf[0], 3);
    ID_data = ID_buf[0] << 16 | ID_buf[1] << 8 | ID_buf[2];
    return res;
}


uint8_t HT7017_SWREST(ht7017_handle_t *handle)
{
    uint8_t buf[2], res = 0;

    buf[0] = 0;
    buf[1] = 0x55;

    res = HT7017_Write(handle, HT7017_REG_SREST, &buf[0], 2);

    return res;
}

uint8_t HT7017_SetWP(ht7017_handle_t *handle, uint8_t wp)
{
    uint8_t buf[2], res = 0;

    buf[0] = 0;
    buf[1] = wp;

    res = HT7017_Write(handle, HT7017_REG_WP, &buf[0], 2);

    return res;
}

uint8_t HT7017_SetMode(ht7017_handle_t *handle, uint8_t mode)
{
    uint8_t buf[2], res = 0;

    buf[0] = 0;
    buf[1] = mode;

    res = HT7017_Write(handle, HT7017_REG_MODULE_EN, &buf[0], 2);

    return res;
}

uint8_t HT7017_SetFreq(ht7017_handle_t *handle, uint16_t freq)
{
    uint8_t buf[2], res = 0;

    buf[0] = 0x00;
    buf[1] = (uint8_t) freq & 0xff;

    res = HT7017_Write(handle, HT7017_REG_ADC_CON, &buf[0], 2);

    return res;
}

uint8_t HT7017_SetAdcCon(ht7017_handle_t *handle, uint16_t con)
{
    uint8_t buf[2], res = 0;

    buf[0] = con >> 8;
    buf[1] = (uint8_t) con & 0xff;

    res = HT7017_Write(handle, HT7017_REG_ADC_CON, &buf[0], 2);

    return res;
}

uint8_t HT7017_SetDCOffset(ht7017_handle_t *handle, uint8_t ch, int32_t offset)
{
    uint8_t buf[2], reg = 0, res = 0;

    if (ch == HT7017_CH_I1)
        reg = HT7017_REG_I1_DC_OFFSET;
    else if (ch == HT7017_CH_I2)
        reg = HT7017_REG_I2_DC_OFFSET;
    else if (ch == HT7017_CH_V)
        reg = HT7017_REG_V_DC_OFFSET;
    else
        return 4;
    offset = ~offset;

    // 先高位在低位
    buf[0] = (offset >> 8) & 0xff;
    buf[1] = offset & 0xff;

    res = HT7017_Write(handle, reg, &buf[0], 2);

    return res;
}

uint8_t HT7017_SetACOffset(ht7017_handle_t *handle, uint8_t ch, uint32_t offset)
{
    uint8_t buf[2], reg = 0, res = 0;

    if (ch == HT7017_CH_I1)
        reg = HT7017_REG_I1_RMS_OFFSET;
    else if (ch == HT7017_CH_I2)
        reg = HT7017_REG_I2_RMS_OFFSET;
    else if (ch == HT7017_CH_V)
        reg = HT7017_REG_V_RMS_OFFSET;
    else
        return 4;

    //	offset = (offset * offset) / 32768;

    // 先高位在低位
    buf[0] = (offset >> 8) & 0xff;
    buf[1] = offset & 0xff;

    res = HT7017_Write(handle, reg, &buf[0], 2);

    return res;
}

/**
 *	获取HT7017测量通道的ADC值
 *
 * @param handle HT7017句柄指针
 * @param ch 测量通道
 * @param dc 直流电压值指针
 * @return
 */
uint8_t HT7017_GetSpl(ht7017_handle_t *handle, uint8_t ch, int32_t *spl)
{
    uint8_t buf[3], reg = 0, res = 0;
    //    uint8_t _sign = 0;

    if (ch == HT7017_CH_I1)
        reg = HT7017_REG_SPL_I1;
    else if (ch == HT7017_CH_I2)
        reg = HT7017_REG_SPL_I2;
    else if (ch == HT7017_CH_V)
        reg = HT7017_REG_SPL_V;
    else
        return 4;

    res = HT7017_Read(handle, reg, &buf[0], 3);

#if 1
    //        _sign = buf[0] & 0x80;
    //        *spl = (buf[0] & 0x3F) << 16 | buf[1] << 8 | buf[2];
    //
    //        if (_sign)
    //            *spl |= (int32_t)0xFFFC0000;


    *spl = buf[0] << 16 | buf[1] << 8 | buf[2];
    if ((*spl & 0xFC0000) == 0xFC0000)  //如果最高位为1，则是负数
    {
        *spl |= 0xFFFC0000;  //高位补1
    }
#endif

    return res;
}


/**
 *	获取HT7017测量通道的有功功率
 *
 * @param handle HT7017句柄指针
 * @param dc 有功功率指针
 * @return
 */
// 0xff 0xbf 0xf9  0x00FFBFF9 16760825
uint8_t HT7017_GetPower_P(ht7017_handle_t *handle, int32_t *dc)
{
    uint8_t buf[3], reg = 0, res = 0;

    reg = HT7017_REG_POWER_P;  //有功功率寄存器

    res = HT7017_Read(handle, reg, &buf[0], 3);

    //    *dc = buf[0] << 16 | buf[1] << 8 | buf[2];
    if (res == 0)
    {
        *dc = buf[0] << 16 | buf[1] << 8 | buf[2];
        if ((*dc & 0x800000) == 0x800000)  //如果最高位为1，则是负数
        {
            *dc |= 0xFF000000;  //高位补1
        }
    }

    return res;
}


/**
 *	获取HT7017测量通道的无功功率
 *
 * @param handle HT7017句柄指针
 * @param dc 无功功率指针
 * @return
 */
// 0xff 0xbf 0xf9  0x00FFBFF9 16760825
uint8_t HT7017_GetPower_Q(ht7017_handle_t *handle, int32_t *dc)
{
    uint8_t buf[3], reg = 0, res = 0;

    reg = HT7017_REG_POWER_Q;  //无功功率寄存器

    res = HT7017_Read(handle, reg, &buf[0], 3);

    //    *dc = buf[0] << 16 | buf[1] << 8 | buf[2];
    if (res == 0)
    {
        *dc = buf[0] << 16 | buf[1] << 8 | buf[2];
        if ((*dc & 0x800000) == 0x800000)  //如果最高位为1，则是负数
        {
            *dc |= 0xFF000000;  //高位补1
        }
    }

    return res;
}


/**
 *	获取HT7017测量通道的直流电压值
 *
 * @param handle HT7017句柄指针
 * @param ch 测量通道
 * @param dc 直流电压 ADC码指针
 * @return
 */
// 0xff 0xbf 0xf9  0x00FFBFF9 16760825
uint8_t HT7017_GetDcCode(ht7017_handle_t *handle, uint8_t ch, int32_t *dc)
{
    uint8_t buf[3], reg = 0, res = 0;

    if ((ch == HT7017_CH_I1) || (ch == HT7017_CH_I2))
        reg = HT7017_REG_I_DC;  //电流寄存器
    else if (ch == HT7017_CH_V)
        reg = HT7017_REG_V_DC;  //电压寄存器
    else
        return 4;  //返回错误类型
    res = HT7017_Read(handle, reg, &buf[0], 3);

    //    *dc = buf[0] << 16 | buf[1] << 8 | buf[2];
    if (res == 0)
    {
        *dc = buf[0] << 16 | buf[1] << 8 | buf[2];
        if ((*dc & 0x800000) == 0x800000)  //如果最高位为1，则是负数
        {
            *dc |= 0xFF000000;  //高位补1
        }
    }

    return res;
}

/**
 *	获取HT7017测量通道的RMS电压值
 * @param handle HT7017句柄指针
 * @param ch 测量通道
 * @param rms RMS ADC码指针
 * @return
 */
uint8_t HT7017_GetRmsCode(ht7017_handle_t *handle, uint8_t ch, uint32_t *rms)
{
    uint8_t buf[3], reg = 0, res = 0;

    if (ch == HT7017_CH_I1)
        reg = HT7017_REG_RMS_I1;
    else if (ch == HT7017_CH_I2)
        reg = HT7017_REG_RMS_I2;
    else if (ch == HT7017_CH_V)
        reg = HT7017_REG_RMS_V;
    else
        return 4;

    res = HT7017_Read(handle, reg, &buf[0], 3);
    if (res == 0)
    {
        *rms = buf[0] << 16 | buf[1] << 8 | buf[2];
    }

    return res;
}

uint8_t HT7017_GetValue(ht7017_handle_t *handle, int32_t *idc, int32_t *udc)
{
    uint8_t buf[18], res = 0;

    res = HT7017_MultipleRead(handle, HT7017_REG_I_DC, HT7017_REG_V_DC, &buf[0], 6);

    if (res == 0)
    {
        *idc = buf[0] << 16 | buf[1] << 8 | buf[2];
        if ((*idc & 0x800000) == 0x800000)  //如果最高位为1，则是负数
        {
            *idc |= 0xFF000000;  //高位补1
        }

        *udc = buf[3] << 16 | buf[4] << 8 | buf[5];
        if ((*udc & 0x800000) == 0x800000)  //如果最高位为1，则是负数
        {
            *udc |= 0xFF000000;  //高位补1
        }
    }

    return res;
}

uint8_t HT7017_GetAllValue(ht7017_handle_t *handle, int32_t *idc, int32_t *udc, uint32_t *i1rms, uint32_t *i2rms, uint32_t *urms)
{
    uint8_t buf[18], res = 0;

    res = HT7017_MultipleRead(handle, HT7017_REG_I_DC, HT7017_REG_RMS_V, &buf[0], 18);

    if (res == 0)
    {
        *idc = buf[0] << 16 | buf[1] << 8 | buf[2];
        if ((*idc & 0x800000) == 0x800000)  //如果最高位为1，则是负数
        {
            *idc |= 0xFF000000;  //高位补1
        }

        *udc = buf[3] << 16 | buf[4] << 8 | buf[5];
        if ((*udc & 0x800000) == 0x800000)  //如果最高位为1，则是负数
        {
            *udc |= 0xFF000000;  //高位补1
        }

        *i1rms = buf[9] << 16 | buf[10] << 8 | buf[11];
        *i2rms = buf[12] << 16 | buf[13] << 8 | buf[14];
        *urms = buf[15] << 16 | buf[16] << 8 | buf[17];
    }

    return res;
}
