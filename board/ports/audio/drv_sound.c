/*
 * Copyright (c) 2020-2020, Bluetrum Development Team
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2020-12-12     greedyhao    first implementation
 */

#include <board.h>

// #define DBG_TAG              "drv.snd_dev"
// #define DBG_LVL              DBG_INFO
// #include <rtdbg.h>
#define LOG_D(...) rt_kprintf(__VA_ARGS__)
#define LOG_I(...) rt_kprintf(__VA_ARGS__)
#define LOG_E(...) rt_kprintf(__VA_ARGS__)

#define TX_FIFO_SIZE         (2048)

struct sound_device
{
    struct rt_audio_device audio;
    struct rt_audio_configure replay_config;
    rt_uint8_t *tx_fifo;
    rt_uint8_t volume;
};

static struct sound_device snd_dev = {0};

// static void virtualplay(void *p)
// {
//     struct sound_device *sound = (struct sound_device *)p;

//     // while(1)
//     // {
//         /* tick = TX_DMA_FIFO_SIZE/2 * 1000ms / 44100 / 4 ≈ 5.8 */
//         rt_thread_mdelay(6);
//         rt_audio_tx_complete(&sound->audio);

//     //     if(sound->endflag == 1)
//     //     {
//     //         break;
//     //     }
//     // }
// }

static rt_err_t sound_getcaps(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    rt_err_t result = RT_EOK;
    struct sound_device *snd_dev = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    snd_dev = (struct sound_device *)audio->parent.user_data;

    switch (caps->main_type)
    {
    case AUDIO_TYPE_QUERY: /* qurey the types of hw_codec device */
    {
        switch (caps->sub_type)
        {
        case AUDIO_TYPE_QUERY:
            caps->udata.mask = AUDIO_TYPE_OUTPUT | AUDIO_TYPE_MIXER;
            break;

        default:
            result = -RT_ERROR;
            break;
        }

        break;
    }

    case AUDIO_TYPE_OUTPUT: /* Provide capabilities of OUTPUT unit */
    {
        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
            caps->udata.config.samplerate   = snd_dev->replay_config.samplerate;
            caps->udata.config.channels     = snd_dev->replay_config.channels;
            caps->udata.config.samplebits   = snd_dev->replay_config.samplebits;
            break;

        case AUDIO_DSP_SAMPLERATE:
            caps->udata.config.samplerate   = snd_dev->replay_config.samplerate;
            break;

        case AUDIO_DSP_CHANNELS:
            caps->udata.config.channels     = snd_dev->replay_config.channels;
            break;

        case AUDIO_DSP_SAMPLEBITS:
            caps->udata.config.samplebits   = snd_dev->replay_config.samplebits;
            break;

        default:
            result = -RT_ERROR;
            break;
        }

        break;
    }

    case AUDIO_TYPE_MIXER: /* report the Mixer Units */
    {
        switch (caps->sub_type)
        {
        case AUDIO_MIXER_QUERY:
            caps->udata.mask = AUDIO_MIXER_VOLUME;
            break;

        case AUDIO_MIXER_VOLUME:
            // caps->udata.value =  es8388_volume_get();
            break;

        default:
            result = -RT_ERROR;
            break;
        }

        break;
    }

    default:
        result = -RT_ERROR;
        break;
    }

    return RT_EOK; 
}

static rt_err_t sound_configure(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    rt_kprintf("sound_configure\n");
    rt_err_t result = RT_EOK;
    struct sound_device *snd_dev = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    snd_dev = (struct sound_device *)audio->parent.user_data;

    switch (caps->main_type)
    {
    case AUDIO_TYPE_MIXER:
    {
        switch (caps->sub_type)
        {
        case AUDIO_MIXER_VOLUME:
        {
            rt_uint8_t volume = caps->udata.value;

            // es8388_volume_set(volume);
            snd_dev->volume = volume;
            LOG_D("set volume %d", volume);
            break;
        }

        default:
            result = -RT_ERROR;
            break;
        }

        break;
    }

    case AUDIO_TYPE_OUTPUT:
    {
        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
        {
            // /* set samplerate */
            // SAIA_Frequency_Set(caps->udata.config.samplerate);
            // /* set channels */
            // SAIA_Channels_Set(caps->udata.config.channels);

            /* save configs */
            snd_dev->replay_config.samplerate = caps->udata.config.samplerate;
            snd_dev->replay_config.channels   = caps->udata.config.channels;
            snd_dev->replay_config.samplebits = caps->udata.config.samplebits;
            LOG_D("set samplerate %d", snd_dev->replay_config.samplerate);
            break;
        }

        case AUDIO_DSP_SAMPLERATE:
        {
            // SAIA_Frequency_Set(caps->udata.config.samplerate);
            snd_dev->replay_config.samplerate = caps->udata.config.samplerate;
            LOG_D("set samplerate %d", snd_dev->replay_config.samplerate);
            break;
        }

        case AUDIO_DSP_CHANNELS:
        {
            // SAIA_Channels_Set(caps->udata.config.channels);
            snd_dev->replay_config.channels   = caps->udata.config.channels;
            LOG_D("set channels %d", snd_dev->replay_config.channels);
            break;
        }

        case AUDIO_DSP_SAMPLEBITS:
        {
            /* not support */
            snd_dev->replay_config.samplebits = caps->udata.config.samplebits;
            break;
        }

        default:
            result = -RT_ERROR;
            break;
        }

        break;
    }

    default:
        break;
    }

    return RT_EOK; 
}

static rt_err_t sound_init(struct rt_audio_device *audio)
{
    struct sound_device *snd_dev = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    snd_dev = (struct sound_device *)audio->parent.user_data;

    /* set default params */
    // SAIA_Frequency_Set(snd_dev->replay_config.samplerate);
    // SAIA_Channels_Set(snd_dev->replay_config.channels);

    return RT_EOK; 
}

static rt_err_t sound_start(struct rt_audio_device *audio, int stream)
{
    struct sound_device *snd_dev = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    snd_dev = (struct sound_device *)audio->parent.user_data;

    if (stream == AUDIO_STREAM_REPLAY)
    {
        LOG_D("open sound device");
        // es8388_start(ES_MODE_DAC);
        // HAL_SAI_Transmit_DMA(&SAI1A_Handler, snd_dev->tx_fifo, TX_FIFO_SIZE / 2);
    }

    return RT_EOK;
}

static rt_err_t sound_stop(struct rt_audio_device *audio, int stream)
{
    struct sound_device *snd_dev = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    snd_dev = (struct sound_device *)audio->parent.user_data; 

    // if (stream == AUDIO_STREAM_REPLAY)
    // {
        // HAL_SAI_DMAStop(&SAI1A_Handler);
        // es8388_stop(ES_MODE_DAC);
        LOG_D("close sound device");
    // }

    return RT_EOK;
}

rt_size_t sound_transmit(struct rt_audio_device *audio, const void *writeBuf, void *readBuf, rt_size_t size)
{
    rt_kprintf("sound_transmit\n");
    struct sound_device *snd_dev = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    snd_dev = (struct sound_device *)audio->parent.user_data;

    return size; 
}

static void sound_buffer_info(struct rt_audio_device *audio, struct rt_audio_buf_info *info)
{
    struct sound_device *snd_dev = RT_NULL;

    RT_ASSERT(audio != RT_NULL); 
    snd_dev = (struct sound_device *)audio->parent.user_data;

    /**
     *               TX_FIFO
     * +----------------+----------------+
     * |     block1     |     block2     |
     * +----------------+----------------+
     *  \  block_size  /
     */
    info->buffer      = snd_dev->tx_fifo;
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
    static struct sound_device snd_dev = {0};

    /* 分配 DMA 搬运 buffer */ 
    tx_fifo = rt_calloc(1, TX_FIFO_SIZE); 
    if(tx_fifo == RT_NULL)
    {
        return -RT_ENOMEM;
    }

    rt_memset(tx_fifo, 0, TX_FIFO_SIZE);
    snd_dev.tx_fifo = tx_fifo;

    /* init default configuration */
    {
        snd_dev.replay_config.samplerate = 44100;
        snd_dev.replay_config.channels   = 2;
        snd_dev.replay_config.samplebits = 16;
        snd_dev.volume                   = 55;
    }

    /* register snd_dev device */
    snd_dev.audio.ops = &ops;
    rt_audio_register(&snd_dev.audio, "sound0", RT_DEVICE_FLAG_WRONLY, &snd_dev);

    return RT_EOK; 
}
INIT_DEVICE_EXPORT(rt_hw_sound_init);
