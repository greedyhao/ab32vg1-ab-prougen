#include "board.h"

#define BUFSZ   1024
#define SOUND_DEVICE_NAME    "sound0"    /* Audio 设备名称 */
static rt_device_t snd_dev;              /* Audio 设备句柄 */

struct RIFF_HEADER_DEF
{
    char riff_id[4];     // 'R','I','F','F'
    uint32_t riff_size;
    char riff_format[4]; // 'W','A','V','E'
};

struct WAVE_FORMAT_DEF
{
    uint16_t FormatTag;
    uint16_t Channels;
    uint32_t SamplesPerSec;
    uint32_t AvgBytesPerSec;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
};

struct FMT_BLOCK_DEF
{
    char fmt_id[4];    // 'f','m','t',' '
    uint32_t fmt_size;
    struct WAVE_FORMAT_DEF wav_format;
};

struct DATA_BLOCK_DEF
{
    char data_id[4];     // 'R','I','F','F'
    uint32_t data_size;
};

struct wav_info
{
    struct RIFF_HEADER_DEF header;
    struct FMT_BLOCK_DEF   fmt_block;
    struct DATA_BLOCK_DEF  data_block;
};

static void example_audio(int argc, char**argv)
{
    int fd = -1;
    rt_err_t ret = RT_EOK;
    uint8_t *buffer = NULL;
    struct wav_info *info = NULL;
    struct rt_audio_caps caps = {0};

    info = (struct wav_info *) rt_malloc(sizeof(struct wav_info));
    rt_memset(info, 0x00, sizeof(struct wav_info));

    snd_dev = rt_device_find(SOUND_DEVICE_NAME);
    if (snd_dev != RT_NULL) {
        rt_kprintf("find sound0      \n");
    }
    /* 以只写方式打开 Audio 播放设备 */
    ret = rt_device_open(snd_dev, RT_DEVICE_OFLAG_WRONLY);
    if (ret == RT_EOK) {
        rt_kprintf("sound0 open successful\n");
    }

    info->fmt_block.wav_format.SamplesPerSec = 44100;
    info->fmt_block.wav_format.Channels = 2;
    // /* 设置采样率、通道、采样位数等音频参数信息 */
    caps.main_type               = AUDIO_TYPE_OUTPUT;                           /* 输出类型（播放设备 ）*/
    caps.sub_type                = AUDIO_DSP_PARAM;                             /* 设置所有音频参数信息 */
    caps.udata.config.samplerate = info->fmt_block.wav_format.SamplesPerSec;    /* 采样率 */
    caps.udata.config.channels   = info->fmt_block.wav_format.Channels;         /* 采样通道 */
    caps.udata.config.samplebits = 16;                                          /* 采样位数 */
    rt_device_control(snd_dev, AUDIO_CTL_CONFIGURE, &caps);

    // while (1)
    // {
    //     int length;

    //     // /* 从文件系统读取 wav 文件的音频数据 */
    //     // length = read(fd, buffer, BUFSZ);

    //     // if (length <= 0)
    //     //     break;

    //     /* 向 Audio 设备写入音频数据 */
    //     rt_device_write(snd_dev, 0, buffer, length);
    // }
    uint8_t num = 0x00;
    rt_device_write(snd_dev, 0, &num, 1);

    /* 关闭 Audio 设备 */
    rt_device_close(snd_dev);
}
MSH_CMD_EXPORT(example_audio, "example audio");
