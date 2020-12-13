/*
 * Copyright (c) 2020-2020, Bluetrum Development Team
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2020-12-12     greedyhao    first implementation
 */

#include <board.h>

#define DBG_TAG              "drv.sound"
#define DBG_LVL              DBG_INFO
#include <rtdbg.h>

#define TX_FIFO_SIZE         (2048)

struct sound_device
{
    struct rt_audio_device audio;
    struct rt_audio_configure replay_config;
    rt_uint8_t *tx_fifo;
    rt_uint8_t volume;
};

static struct sound_device snd_dev = {0};

static rt_err_t sound_getcaps(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    struct sound_device *sound = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    sound = (struct sound_device *)audio->parent.user_data; (void)sound; 

    return RT_EOK; 
}

static rt_err_t sound_configure(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    struct sound_device *sound = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    sound = (struct sound_device *)audio->parent.user_data; (void)sound; 

    return RT_EOK; 
}

static rt_err_t sound_init(struct rt_audio_device *audio)
{
    struct sound_device *sound = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    sound = (struct sound_device *)audio->parent.user_data; (void)sound; 

    return RT_EOK; 
}

static rt_err_t sound_start(struct rt_audio_device *audio, int stream)
{
    struct sound_device *sound = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    sound = (struct sound_device *)audio->parent.user_data; (void)sound; 

    return RT_EOK;
}

static rt_err_t sound_stop(struct rt_audio_device *audio, int stream)
{
    struct sound_device *sound = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    sound = (struct sound_device *)audio->parent.user_data; (void)sound;  

    return RT_EOK;
}

rt_size_t sound_transmit(struct rt_audio_device *audio, const void *writeBuf, void *readBuf, rt_size_t size)
{
    struct sound_device *sound = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    sound = (struct sound_device *)audio->parent.user_data; (void)sound; 

    return size; 
}

static void sound_buffer_info(struct rt_audio_device *audio, struct rt_audio_buf_info *info)
{
    struct sound_device *sound = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    sound = (struct sound_device *)audio->parent.user_data;

    /**
     *               TX_FIFO
     * +----------------+----------------+
     * |     block1     |     block2     |
     * +----------------+----------------+
     *  \  block_size  /
     */
    info->buffer      = sound->tx_fifo;
    info->total_size  = TX_FIFO_SIZE;
    info->block_size  = TX_FIFO_SIZE / 2;
    info->block_count = 2;
}

static struct rt_audio_ops ops =
{
    .getcaps     = sound_getcaps,
    .configure   = sound_configure,
    .init        = sound_init,
    .start       = sound_start,
    .stop        = sound_stop,
    .transmit    = sound_transmit, 
    .buffer_info = sound_buffer_info,
};

static int rt_hw_sound_init(void)
{
    rt_uint8_t *tx_fifo = RT_NULL; 
    static struct sound_device sound = {0};

    /* 分配 DMA 搬运 buffer */ 
    tx_fifo = rt_calloc(1, TX_FIFO_SIZE); 
    if(tx_fifo == RT_NULL)
    {
        return -RT_ENOMEM;
    }

    rt_memset(tx_fifo, 0, TX_FIFO_SIZE);
    sound.tx_fifo = tx_fifo;

    /* init default configuration */
    {
        snd_dev.replay_config.samplerate = 44100;
        snd_dev.replay_config.channels   = 2;
        snd_dev.replay_config.samplebits = 16;
        snd_dev.volume                   = 55;
    }

    /* register sound device */
    sound.audio.ops = &ops;
    rt_audio_register(&sound.audio, "sound0", RT_DEVICE_FLAG_WRONLY, &sound);

    return RT_EOK; 
}
INIT_DEVICE_EXPORT(rt_hw_sound_init);
