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

/* PainterEngine mempool */
RT_SECTION(".ram_ex") rt_uint8_t px_cache[0x800000];

int main(void)
{
    int count = 1;
    rt_device_t lcd_dev;
    struct rt_device_graphic_info lcd_info;
    px_surface lcd_surface;
    px_memorypool mp;

    lcd_dev = rt_device_find("lcd");
    rt_device_open(lcd_dev, 0);
    rt_device_control(lcd_dev, RTGRAPHIC_CTRL_GET_INFO, &lcd_info);
    LOG_D("lcd width %d, height %d, framebuffer addr %x", lcd_info.width, lcd_info.height, lcd_info.framebuffer);

    mp = MP_Create(px_cache, (px_uint)0x800000);

    lcd_surface.height = lcd_info.height;
    lcd_surface.width = lcd_info.width;
    lcd_surface.surfaceBuffer = (px_color *)lcd_info.framebuffer;
    lcd_surface.MP = &mp;
    lcd_surface.limit_left = 0;
    lcd_surface.limit_top = 0;
    lcd_surface.limit_right = lcd_info.width - 1;
    lcd_surface.limit_bottom = lcd_info.height - 1;
    PX_memdwordset(lcd_surface.surfaceBuffer, 0, lcd_info.height * lcd_info.width);

    LOG_D("lcd width %d, height %d, framebuffer addr %x", lcd_surface.width, lcd_surface.height, lcd_surface.surfaceBuffer);

    PX_SurfaceClear(&lcd_surface, 0, 0, 320 - 1, 480 - 1, PX_COLOR(0xff, 0x00, 0x00, 0x00));
    rt_device_control(lcd_dev, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);

    while (count++)
    {
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0xff, 0x00, 0x00));
        rt_device_control(lcd_dev, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(200);
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0x00, 0xff, 0x00));
        rt_device_control(lcd_dev, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(200);
        PX_GeoDrawCircle(&lcd_surface, 320 / 2, 480 / 2, 100, 5, PX_COLOR(0xff, 0x00, 0x00, 0xff));
        rt_device_control(lcd_dev, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(200);
   }

    return RT_EOK;
}
