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
#include <PX_Kernel.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include <sdram_port.h>
#include <lcd_port.h>

rt_uint8_t *px_cache;

static void lcd_refresh(px_surface *lcd_framebuffer)
{
    px_color color_argb888;
    rt_uint16_t color_rgb565;

    if (lcd_framebuffer->height == 480 && lcd_framebuffer->width == 320)
    {
        for (int i = 0; i < (320 * 480); i++)
        {
            color_argb888 = lcd_framebuffer->surfaceBuffer[i];
            color_rgb565 = (color_argb888._argb.r >> 3) << 11 \
                           | (color_argb888._argb.g >> 2) << 5 \
                           | (color_argb888._argb.b >> 3);
            LCD_WR_DATA(color_rgb565);
        }
    }
    else
    {
        LOG_E("Framebuffer size is not equal lcd resolution");
        LOG_E("framebuffer height %d, width %d", lcd_framebuffer->height, lcd_framebuffer->width);
    }
}

int main(void)
{
    int count = 1;
    px_surface lcd_surface;
    px_memorypool mp;
    px_cache = (rt_uint8_t *)0XC0000000;

    mp = MP_Create(px_cache, (px_uint)0x2000000);
    PX_SurfaceCreate(&mp, 320, 480, &lcd_surface);
    PX_SurfaceClear(&lcd_surface, 0, 0, 320 - 1, 480 - 1, PX_COLOR(0xff, 0x00, 0x00, 0x00));
    lcd_refresh(&lcd_surface);

    while (count++)
    {
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0xff, 0x00, 0x00));
        lcd_refresh(&lcd_surface);
        rt_thread_mdelay(200);
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0xff, 0xff, 0x00));
        lcd_refresh(&lcd_surface);
        rt_thread_mdelay(200);
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0x00, 0xff, 0x00));
        lcd_refresh(&lcd_surface);
        rt_thread_mdelay(200);
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0x00, 0xff, 0xff));
        lcd_refresh(&lcd_surface);
        rt_thread_mdelay(200);
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0x00, 0x00, 0xff));
        lcd_refresh(&lcd_surface);
        rt_thread_mdelay(200);
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0xff, 0x00, 0xff));
        lcd_refresh(&lcd_surface);
        rt_thread_mdelay(200);
   }

    return RT_EOK;
}
