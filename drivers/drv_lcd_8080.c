/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-02     QP       the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#ifdef BSP_USING_LCD_8080
#include <lcd_port.h>

#define DRV_DEBUG
#define LOG_TAG             "drv.lcd_8080"
#include <drv_log.h>

#define LCD_DEVICE(dev)     (struct drv_lcd_device*)(dev)

SRAM_HandleTypeDef hsram1;

struct drv_lcd_device
{
    struct rt_device parent;

    struct rt_device_graphic_info lcd_info;
};

struct drv_lcd_device _lcd;

#define LCD_PIXEL_NUM (LCD_WIDTH * LCD_HEIGHT)
#define LCD_BUF_SIZE (LCD_PIXEL_NUM * 4)
RT_SECTION(".ram_ex") rt_uint8_t _lcd_framebuffer[LCD_BUF_SIZE];

extern void stm32_8080_lcd_init(void);
//extern void stm32_8080_lcd_config(rt_uint32_t pixel_format);
//extern void stm32_8080_display_on(void);
//extern void stm32_8080_display_off(void);

static rt_err_t drv_lcd_init(struct rt_device *device)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);
    /* nothing, right now */
    lcd = lcd;
    return RT_EOK;
}

static rt_err_t drv_lcd_control(struct rt_device *device, int cmd, void *args)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);

    switch (cmd)
    {
    case RTGRAPHIC_CTRL_RECT_UPDATE:
    {
        rt_uint16_t color_rgb565;
        rt_uint32_t color_index = 0;

        /* update */
        for (int i = 0; i < LCD_PIXEL_NUM; i++)
        {
            color_rgb565 = (lcd->lcd_info.framebuffer[color_index + 2] >> 3) \
                           | (lcd->lcd_info.framebuffer[color_index + 1] >> 2) << 5 \
                           | (lcd->lcd_info.framebuffer[color_index] >> 3) << 11;
            LCD_WR_DATA(color_rgb565);
            color_index += 4;
        }
    }
    break;

    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *)args;

        RT_ASSERT(info != RT_NULL);
        info->pixel_format  = lcd->lcd_info.pixel_format;
        info->bits_per_pixel = 24;
        info->width         = lcd->lcd_info.width;
        info->height        = lcd->lcd_info.height;
        info->framebuffer   = lcd->lcd_info.framebuffer;
    }
    break;
    }

    return RT_EOK;
}

static int LCD_Port_Init(void)
{
    FMC_NORSRAM_TimingTypeDef Timing = { 0 };
    FMC_NORSRAM_TimingTypeDef ExtTiming = { 0 };

    /** Perform the SRAM1 memory initialization sequence
     */
    hsram1.Instance = FMC_NORSRAM_DEVICE;
    hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
    /* hsram1.Init */
    hsram1.Init.NSBank = FMC_NORSRAM_BANK1;
    hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
    hsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
    hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
    hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
    hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
    hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
    hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
    hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
    hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
    hsram1.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
    hsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;
    /* Timing */
    Timing.AddressSetupTime = 15;
    Timing.AddressHoldTime = 15;
    Timing.DataSetupTime = 80;
    Timing.BusTurnAroundDuration = 15;
    Timing.CLKDivision = 16;
    Timing.DataLatency = 17;
    Timing.AccessMode = FMC_ACCESS_MODE_A;
    /* ExtTiming */
    ExtTiming.AddressSetupTime = 3;
    ExtTiming.AddressHoldTime = 3;
    ExtTiming.DataSetupTime = 15;
    ExtTiming.BusTurnAroundDuration = 15;
    ExtTiming.CLKDivision = 16;
    ExtTiming.DataLatency = 17;
    ExtTiming.AccessMode = FMC_ACCESS_MODE_A;

    return (int) HAL_SRAM_Init(&hsram1, &Timing, &ExtTiming);
}

INIT_BOARD_EXPORT(LCD_Port_Init);

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops lcd_ops =
{
    drv_lcd_init,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    drv_lcd_control
};
#endif

static int LCD_Init(void)
{
    struct rt_device *device = &_lcd.parent;

    /* memset _lcd to zero */
    rt_memset(&_lcd, 0x00, sizeof(_lcd));

    /* config LCD dev info */
    _lcd.lcd_info.height = LCD_HEIGHT;
    _lcd.lcd_info.width = LCD_WIDTH;
    _lcd.lcd_info.bits_per_pixel = 24;
    _lcd.lcd_info.pixel_format = RTGRAPHIC_PIXEL_FORMAT_ABGR888;
    _lcd.lcd_info.framebuffer = &_lcd_framebuffer[0];

    /* memset buff to 0x00 */
    rt_memset(_lcd.lcd_info.framebuffer, 0x00, LCD_BUF_SIZE);

    device->type    = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    device->ops     = &lcd_ops;
#else
    device->init    = drv_lcd_init;
    device->control = drv_lcd_control;
#endif

    /* register lcd device */
    rt_device_register(device, "lcd", RT_DEVICE_FLAG_RDWR);

    /* Reset LCD */
    rt_pin_mode(LCD_RST_GPIO_NUM, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_RST_GPIO_NUM, PIN_LOW);
    rt_thread_mdelay(10);
    rt_pin_write(LCD_RST_GPIO_NUM, PIN_HIGH);
    rt_thread_mdelay(20);

    /* LCD initialize */
    stm32_8080_lcd_init();

    return RT_EOK;
}

INIT_DEVICE_EXPORT(LCD_Init);
#endif /* BSP_USING_LCD_8080 */
