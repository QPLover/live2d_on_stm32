/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-04     QP       the first version
 */
#ifndef DRIVERS_LCD_PORT_H_
#define DRIVERS_LCD_PORT_H_

#include <rtdef.h>

typedef struct
{
    volatile rt_uint16_t lcd_cmd;
    volatile rt_uint16_t lcd_data;
}LCD_PortTypeDef;

#define LCD_BaseAddr    (0x60000000 | (0x00 << 26) | (0xffff * 2))
#define LCD_Port    ((LCD_PortTypeDef *) LCD_BaseAddr)

#define LCD_BACKLIGHT_USING_GPIO
#define LCD_BL_GPIO_NUM     GET_PIN(B, 1)
#define LCD_RST_GPIO_NUM     GET_PIN(D, 6)

#define LCD_WR_REG(__reg_val) LCD_Port->lcd_cmd = __reg_val
#define LCD_WR_DATA(__data_val) LCD_Port->lcd_data = __data_val

#endif /* DRIVERS_LCD_PORT_H_ */
