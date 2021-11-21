/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-04     QP       the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#ifdef BSP_USING_LCD_8080
#include <lcd_port.h>

#define DRV_DEBUG
#define LOG_TAG             "lcd.port_ili9481"
#include <drv_log.h>

#define LCD_WR_REG(__reg_val) LCD_PORT->lcd_cmd = __reg_val
#define LCD_WR_DATA(__data_val) LCD_PORT->lcd_data = __data_val

RT_SECTION(".ram_ex") rt_uint8_t _lcd_framebuffer[LCD_BUF_SIZE];

extern void stm32_8080_lcd_clear_gram(void);

void stm32_8080_lcd_init(void)
{
    /* Read LCD ID */
    LCD_PORT->lcd_cmd = 0xbf;
    LOG_D("LCD ID 1 is %x", (rt_uint32_t)LCD_PORT->lcd_data);
    LOG_D("LCD ID 2 is %x", (rt_uint32_t)LCD_PORT->lcd_data);
    LOG_D("LCD ID 3 is %x", (rt_uint32_t)LCD_PORT->lcd_data);
    LOG_D("LCD ID 4 is %x", (rt_uint32_t)LCD_PORT->lcd_data);
    LOG_D("LCD ID 5 is %x", (rt_uint32_t)LCD_PORT->lcd_data);
    LOG_D("LCD ID 6 is %x", (rt_uint32_t)LCD_PORT->lcd_data);

    /* ILI9481 initialize */
    LCD_WR_REG(0x11);
    rt_thread_mdelay(200);
    LCD_WR_REG(0x11);
    rt_thread_mdelay(200);
    //2 ok20200803
    LCD_WR_REG(0xD0);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x42);
    LCD_WR_DATA(0x1b);

    LCD_WR_REG(0xD1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x0c);

    LCD_WR_REG(0xD2);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x11);

    LCD_WR_REG(0xC0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x11);

    LCD_WR_REG(0xC5);
    LCD_WR_DATA(0x02);

    LCD_WR_REG(0xC8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x22);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);

    ///20210809
    ///1
    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xF6);
    LCD_WR_DATA(0x84);  ///0x80
    LCD_WR_REG(0xF3);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x1A);
    LCD_WR_REG(0xF7);
    LCD_WR_DATA(0x80);

    ///2
    //LCD_WR_REG(0xF0);
    //LCD_WR_DATA(0x01);
    //LCD_WR_REG(0xF6);
    //LCD_WR_DATA(0x80);
    //LCD_WR_REG(0xF3);
    //LCD_WR_DATA(0x40);
    //LCD_WR_DATA(0x0A);
    //LCD_WR_REG(0xF7);
    //LCD_WR_DATA(0x80);

    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x0A);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);

    //LCD_WR_REG(0xB3);
    //LCD_WR_DATA(0x02);
    //LCD_WR_DATA(0x03);
    //LCD_WR_DATA(0x03);
    //LCD_WR_DATA(0x21);

    rt_thread_mdelay(120);

    /* Set display window full screen */
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA((320 - 1) >> 8);
    LCD_WR_DATA((320 - 1) & 0xff);

    LCD_WR_REG(0x2B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA((480 - 1) >> 8);
    LCD_WR_DATA((480 - 1) & 0xff);

    LCD_WR_REG(0x2C);

    /* Clear GRAM */
    stm32_8080_lcd_clear_gram();

    LCD_WR_REG(0x29);
    LCD_WR_REG(0x3C);
}

#endif /* BSP_USING_LCD_8080 */
