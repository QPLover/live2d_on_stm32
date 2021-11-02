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

#define DRV_DEBUG
#define LOG_TAG             "drv.lcd_8080"
#include <drv_log.h>

typedef struct
{
    volatile rt_uint16_t lcd_cmd;
    volatile rt_uint16_t lcd_data;
}LCD_PortTypeDef;

SRAM_HandleTypeDef hsram1;
#define LCD_Base    (0x60000000 | (0x00 << 26) | (0xffff * 2))
#define LCD_Port    ((LCD_PortTypeDef *) LCD_Base)
#define LCD_RST     GET_PIN(D, 6)

#define LCD_WR_REG(__reg_val) LCD_Port->lcd_cmd = __reg_val
#define LCD_WR_DATA(__data_val) LCD_Port->lcd_data = __data_val
#define LCD_WriteReg(__reg_val, __data_val) \
        do{                                     \
            LCD_Port->lcd_cmd = __reg_val;           \
            LCD_Port->lcd_data = __data_val;         \
        }while(0)                               \


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
    ExtTiming.AddressSetupTime = 15;
    ExtTiming.AddressHoldTime = 15;
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
    rt_pin_mode(LCD_RST, PIN_MODE_OUTPUT);

    /* Reset LCD */
    rt_pin_write(LCD_RST, PIN_LOW);
    rt_thread_mdelay(10);
    rt_pin_write(LCD_RST, PIN_HIGH);
    rt_thread_mdelay(20);

    /* Read LCD ID */
    LCD_Port->lcd_cmd = 0xbf;
    LOG_D("LCD ID 1 is %x", (rt_uint32_t)LCD_Port->lcd_data);
    LOG_D("LCD ID 2 is %x", (rt_uint32_t)LCD_Port->lcd_data);
    LOG_D("LCD ID 3 is %x", (rt_uint32_t)LCD_Port->lcd_data);
    LOG_D("LCD ID 4 is %x", (rt_uint32_t)LCD_Port->lcd_data);
    LOG_D("LCD ID 5 is %x", (rt_uint32_t)LCD_Port->lcd_data);
    LOG_D("LCD ID 6 is %x", (rt_uint32_t)LCD_Port->lcd_data);

    /* ILI9481 initialize */

    return RT_EOK;
}

INIT_APP_EXPORT(LCD_Init);
