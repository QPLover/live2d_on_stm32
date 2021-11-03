/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-02     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

typedef struct
{
    volatile rt_uint16_t lcd_cmd;
    volatile rt_uint16_t lcd_data;
}LCD_PortTypeDef;

#define LCD_Base    (0x60000000 | (0x00 << 26) | (0xffff * 2))
#define LCD_Port    ((LCD_PortTypeDef *) LCD_Base)

#define LCD_WR_DATA(__data_val) LCD_Port->lcd_data = __data_val

int main(void)
{
    int count = 1;

    while (count++)
    {
        for(int i = 0; i < (320 * 480); i++)
            LCD_WR_DATA(0xf800);

        rt_thread_mdelay(200);

        for(int i = 0; i < (320 * 480); i++)
            LCD_WR_DATA(0x07e0);

        rt_thread_mdelay(200);

        for(int i = 0; i < (320 * 480); i++)
            LCD_WR_DATA(0x001f);

        rt_thread_mdelay(200);
    }

    return RT_EOK;
}
