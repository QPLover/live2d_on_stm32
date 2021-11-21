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

/*
 * lcd resolution and pixel format info
 */
#define LCD_WIDTH   320
#define LCD_HEIGHT  480
#define LCD_PIXEL_FORMAT    RTGRAPHIC_PIXEL_FORMAT_RGB565
#define LCD_PIXEL_SIZE_BYTE  2

/* this address use the max address of keep D/C pin to low */
#define LCD_PORT_ADDR    (0x60000000 | (0x00 << 26) | (0xffff * 2))
#define LCD_DATA_BUS_WIDTH  16

#define LCD_RST_GPIO_NUM     GET_PIN(D, 6)

#define LCD_BACKLIGHT_USING_GPIO
#define LCD_BL_GPIO_NUM     GET_PIN(B, 1)

/* do not modify the code below! */
typedef struct
{
    volatile rt_uint16_t lcd_cmd;
    volatile rt_uint16_t lcd_data;
} stm32_lcd_port;
#define LCD_PORT    ((stm32_lcd_port *) LCD_PORT_ADDR)

#define LCD_PIXEL_NUM (LCD_WIDTH * LCD_HEIGHT)
#define LCD_BUF_SIZE (LCD_PIXEL_NUM * LCD_PIXEL_SIZE_BYTE)
#define LCD_NUM_OF_TX (LCD_BUF_SIZE / (LCD_DATA_BUS_WIDTH / 8))

#endif /* DRIVERS_LCD_PORT_H_ */
