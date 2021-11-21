/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-02     QP       the first version
 * 2021-11-21     QP       use dma to transfer framebuffer data to lcd gram
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#ifdef BSP_USING_LCD_8080
#include <lcd_port.h>

#define DRV_DEBUG
#define LOG_TAG             "drv.lcd_8080"
#include <drv_log.h>

static SRAM_HandleTypeDef hsram;
static DMA_HandleTypeDef hdma;

/* stm32 lcd dirver class */
struct drv_lcd_device
{
    struct rt_device parent;

    struct rt_device_graphic_info lcd_info;

    struct rt_semaphore lcd_lock;

    rt_uint32_t dma_tx_blk_num;
    rt_uint32_t dma_tx_last_blk_size;
};

#define LCD_DEVICE(dev)     (struct drv_lcd_device*)(dev)

/* stm32 lcd dirver private class */
struct drv_lcd_device _lcd;

/* lcd framebuffer, defined on lcd_port.c */
extern rt_uint8_t _lcd_framebuffer[];

/* lcd initialization function, defined on lcd_port.c */
extern void stm32_8080_lcd_init(void);

/*
 * nothing to do it
 */
static rt_err_t drv_lcd_init(struct rt_device *device)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);
    /* nothing, right now */
    lcd = lcd;
    return RT_EOK;
}

/*
 * lcd control function, refresh lcd or get lcd infomation
 */
static rt_err_t drv_lcd_control(struct rt_device *device, int cmd, void *args)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);

    switch (cmd)
    {
    case RTGRAPHIC_CTRL_RECT_UPDATE:
    {
        rt_uint32_t dma_tx_start_pos = 0;
#if LCD_DATA_BUS_WIDTH == 16
        rt_uint16_t *_framebuffer = (rt_uint16_t *)lcd->lcd_info.framebuffer;
#elif LCD_DATA_BUS_WIDTH == 8
        rt_uint8_t *_framebuffer = (rt_uint8_t *)lcd->lcd_info.framebuffer;
#else
        rt_uint32_t *_framebuffer = (rt_uint32_t *)lcd->lcd_info.framebuffer;
#endif
        /* transmit framebuffer data to lcd gram */
        for(int i = lcd->dma_tx_blk_num; i > 0; i--)
        {
            if (i == 1)
            {
                /* transmit the last framebuffer data block */
                HAL_DMA_Start_IT(&hdma,
                                 (rt_uint32_t)&_framebuffer[dma_tx_start_pos],
                                 (rt_uint32_t)&LCD_PORT->lcd_data,
                                 lcd->dma_tx_last_blk_size);
            }
            else
            {
                /* transmit framebuffer data block, size is 65535 */
                HAL_DMA_Start_IT(&hdma,
                                 (rt_uint32_t)&_framebuffer[dma_tx_start_pos],
                                 (rt_uint32_t)&LCD_PORT->lcd_data,
                                 0xffff);

                dma_tx_start_pos += 0xffff;
            }
            rt_sem_take(&_lcd.lcd_lock, RT_TICK_PER_SECOND / 20);
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

static void lcd_dma_tx_finish_callback(DMA_HandleTypeDef *_hdma)
{
    rt_sem_release(&_lcd.lcd_lock);
}

void DMA1_Stream6_IRQHandler(void)
{
    rt_interrupt_enter();

    HAL_DMA_IRQHandler(&hdma);

    rt_interrupt_leave();
}

/*
 * fmc lcd/sram port initialization
 */
static int lcd_port_init(void)
{
    FMC_NORSRAM_TimingTypeDef Timing = { 0 };
    FMC_NORSRAM_TimingTypeDef ExtTiming = { 0 };

    /*
     * Perform the SRAM memory initialization sequence
     */
    hsram.Instance = FMC_NORSRAM_DEVICE;
    hsram.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
    /* hsram.Init */
    hsram.Init.NSBank = FMC_NORSRAM_BANK1;
    hsram.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
    hsram.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
#if LCD_DATA_BUS_WIDTH == 16
    hsram.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
#elif LCD_DATA_BUS_WIDTH == 8
    hsram.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_8;
#else
    hsram.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_32;
#endif
    hsram.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
    hsram.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
    hsram.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
    hsram.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
    hsram.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
    hsram.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
    hsram.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
    hsram.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
    hsram.Init.PageSize = FMC_PAGE_SIZE_NONE;
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

    HAL_SRAM_Init(&hsram, &Timing, &ExtTiming);
    return RT_EOK;
}

INIT_BOARD_EXPORT(lcd_port_init);

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

static int lcd_dma_init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure DMA request hdma on DMA1_Stream6 */
    hdma.Instance = DMA1_Stream6;
    hdma.Init.Request = DMA_REQUEST_MEM2MEM;
    hdma.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma.Init.MemInc = DMA_MINC_DISABLE;
#if LCD_DATA_BUS_WIDTH == 16
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
#elif LCD_DATA_BUS_WIDTH == 8
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
#else
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
#endif
    hdma.Init.Mode = DMA_NORMAL;
    hdma.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma) != HAL_OK)
    {
       return RT_ERROR;
    }

    HAL_DMA_RegisterCallback(&hdma, HAL_DMA_XFER_CPLT_CB_ID, lcd_dma_tx_finish_callback);

    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

    return RT_EOK;
}

static int drv_lcd_8080_hw_init(void)
{
    struct rt_device *device = &_lcd.parent;

    /* memset _lcd to zero */
    rt_memset(&_lcd, 0x00, sizeof(_lcd));

    /* config LCD dev info */
    _lcd.lcd_info.height = LCD_HEIGHT;
    _lcd.lcd_info.width = LCD_WIDTH;
#if LCD_PIXEL_FORMAT == RTGRAPHIC_PIXEL_FORMAT_RGB565
    _lcd.lcd_info.bits_per_pixel = 16;
#elif LCD_PIXEL_FORMAT == RTGRAPHIC_PIXEL_FORMAT_RGB888
    _lcd.lcd_info.bits_per_pixel = 24;
#endif
    _lcd.lcd_info.pixel_format = LCD_PIXEL_FORMAT;
    _lcd.lcd_info.framebuffer = _lcd_framebuffer;

    /*
     * calculate number of transmit blocks for dma
     */
    rt_uint32_t lcd_dma_tx_blk_num = LCD_NUM_OF_TX / 0xffff;
    rt_uint32_t lcd_dma_tx_last_blk_size = LCD_NUM_OF_TX % 0xffff;

    if (lcd_dma_tx_blk_num == 0)
        lcd_dma_tx_blk_num += 1;

    if (lcd_dma_tx_last_blk_size)
    {
        _lcd.dma_tx_blk_num = lcd_dma_tx_blk_num + 1;
        _lcd.dma_tx_last_blk_size = lcd_dma_tx_last_blk_size;
    }
    else
    {
        _lcd.dma_tx_blk_num = lcd_dma_tx_blk_num;
        _lcd.dma_tx_last_blk_size = 0xffff;
    }

    /* init lcd_lock semaphore */
    rt_sem_init(&_lcd.lcd_lock, "lcd_lock", 0, RT_IPC_FLAG_FIFO);

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

    /* initialize transmission dma */
    lcd_dma_init();

    /* reset lcd */
    rt_pin_mode(LCD_RST_GPIO_NUM, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_RST_GPIO_NUM, PIN_LOW);
    rt_thread_mdelay(10);
    rt_pin_write(LCD_RST_GPIO_NUM, PIN_HIGH);
    rt_thread_mdelay(20);

    /* lcd initialize */
    stm32_8080_lcd_init();

    return RT_EOK;
}

INIT_DEVICE_EXPORT(drv_lcd_8080_hw_init);

/*
 * transmit data from clean framebuffer into lcd gram to clear lcd screen
 */
void stm32_8080_lcd_clear_gram(void)
{
    _lcd.parent.control(&_lcd.parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
}

#endif /* BSP_USING_LCD_8080 */
