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
#include <dfs_posix.h>
#include <PX_Kernel.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* PainterEngine使用的内存池（位于外部SDRAM中） */
RT_SECTION(".ram_ex") rt_uint8_t px_cache[1024 * 1024 * 16];

px_surface lcd_surface; //LCD帧缓冲对象
px_memorypool px_mp;    //内存池对象
PX_LiveFramework live2d;    //Live2d渲染器对象

/*
 * 文件内存池对象结构体
 */
typedef struct
{
    unsigned char *buffer;
    unsigned int size;
}PX_IO_Data;

/*
 * 从SD卡加载文件到内存池
 */
PX_IO_Data PX_LoadFileToIOData(const char path[])
{
    PX_IO_Data io;
    struct stat filestate;
    int pf;
    int filesize;

    pf = open(path, O_RDONLY);
    if (pf == -1)
    {
        LOG_D("Open live2d file err");
        goto _ERROR;
    }

    fstat(pf, &filestate);
    filesize = filestate.st_size;
    LOG_D("Live2d file size %u", filesize);

    io.buffer = (unsigned char *)MP_Malloc(&px_mp, filesize + 1);
    if (!io.buffer)
    {
        LOG_D("Get mem space err");
        goto _ERROR;
    }

    read(pf, io.buffer, filesize);

    close(pf);

    io.buffer[filesize]='\0';
    io.size=filesize;
    return io;
    _ERROR:
    io.buffer=0;
    io.size=0;
    return io;
}

/*
  * 释放文件内存池
 */
void PX_FreeIOData(PX_IO_Data *io)
{
    if (io->size&&io->buffer)
    {
        MP_Free(&px_mp, io->buffer);
        io->size=0;
        io->buffer=0;
    }
}

/*
 * 主线程，Live2d渲染器在此运行
 */
int main(void)
{
    int count = 1;
    int anim_index = 1;
    char anim_action[16];
    rt_uint32_t now, lastUpdateTime, elapsed;
    rt_device_t lcd_dev;    //LCD设备对象
    struct rt_device_graphic_info lcd_info;    //LCD参数结构体
    PX_IO_Data Live2d_file; //存放Live2d文件的内存空间的对象

    /* 获取LCD设备对象并获取LCD参数 */
    lcd_dev = rt_device_find("lcd");
    rt_device_open(lcd_dev, RT_NULL);
    rt_device_control(lcd_dev, RTGRAPHIC_CTRL_GET_INFO, &lcd_info);
    LOG_D("LCD width %d, height %d, framebuffer addr %x", lcd_info.width, lcd_info.height, lcd_info.framebuffer);

    /* 初始化PainterEngine的内存池 */
    px_mp = MP_Create(px_cache, sizeof px_cache);

    /* 初始化LCD帧缓冲对象 */
    lcd_surface.height = lcd_info.height;
    lcd_surface.width = lcd_info.width;
    lcd_surface.surfaceBuffer = (px_color *)lcd_info.framebuffer;
    lcd_surface.MP = &px_mp;
    lcd_surface.limit_left = 0;
    lcd_surface.limit_top = 0;
    lcd_surface.limit_right = lcd_info.width - 1;
    lcd_surface.limit_bottom = lcd_info.height - 1;
    LOG_D("LCD surface width %d, height %d, framebuffer addr %x", lcd_surface.width, lcd_surface.height, lcd_surface.surfaceBuffer);

    /* 等待SD卡挂载 */
    rt_thread_mdelay(1000);

    /* 初始化Live2d渲染器 */
    if (PX_LiveFrameworkInitialize(&px_mp, &live2d, lcd_info.width, lcd_info.height) == PX_FALSE)
    {
        LOG_D("Live2d framework init fail");
    }

    /* 从SD卡加载Live2d文件并导入渲染器中 */
    Live2d_file = PX_LoadFileToIOData("/sdcard/release.live");
    if (PX_LiveFrameworkImport(&px_mp, &live2d, Live2d_file.buffer, Live2d_file.size) == PX_FALSE)
    {
        LOG_D("Live2d framework import file fail");
    }
    PX_FreeIOData(&Live2d_file);

    /* 开启Live2d动画播放 */
    PX_LiveFrameworkPlay(&live2d);
    if (PX_LiveFrameworkPlayAnimationByName(&live2d, "action01") == PX_FALSE)   //从第一个动画开始播放
    {
        LOG_D("Live2d set animation fail");
    }

    //取得时间(毫秒)
    lastUpdateTime = rt_tick_get_millisecond();

    while (count++)
    {
        //计算当前时间
        now = rt_tick_get_millisecond();
        //计算上次更新到现在经过的时间
        elapsed = now - lastUpdateTime;
        lastUpdateTime = now;

        if (!(count % 20))  //每个动画执行20次渲染
        {
            anim_index += 1;
            if(anim_index > 32) //一共32个动画
            {
                anim_index = 1;
            }
            if (anim_index < 10)
            {
                sprintf(anim_action, "action0%u", anim_index);
            }
            else
            {
                sprintf(anim_action, "action%u", anim_index);
            }
            LOG_D(anim_action);
            if (PX_LiveFrameworkPlayAnimationByName(&live2d, anim_action) == PX_FALSE)  //选择动画
            {
                LOG_D("Live2d set animation fail");
            }
        }

        /* 执行Live2d动画渲染并刷新显示屏 */
        PX_SurfaceClear(&lcd_surface, 0, 0, 320 - 1, 480 - 1, PX_COLOR(0xff, 0x00, 0x00, 0x00));    //清空LCD的帧缓冲
        PX_LiveFrameworkRender(&lcd_surface, &live2d, lcd_surface.width / 2, lcd_surface.height / 2, PX_ALIGN_CENTER, elapsed); //执行Live2d动画渲染
        rt_device_control(lcd_dev, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);    //刷屏

        /* 每次渲染后延时5ms为其它任务留出执行时间 */
        rt_thread_mdelay(5);
    }

    return RT_EOK;
}
