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

extern void stm32_8080_lcd_init(void);
//extern void stm32_8080_lcd_config(rt_uint32_t pixel_format);
//extern void stm32_8080_display_on(void);
//extern void stm32_8080_display_off(void);

SRAM_HandleTypeDef hsram1;

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

static int LCD_Init(void)
{
    rt_pin_mode(LCD_RST_GPIO_NUM, PIN_MODE_OUTPUT);

    /* Reset LCD */
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
