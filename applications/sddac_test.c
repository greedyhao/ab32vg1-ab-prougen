#include "board.h"

#define DAC_OBUF_SIZE   576
#define BUFSZ   1024
#define SOUND_DEVICE_NAME    "sound0"    /* Audio 设备名称 */
static rt_device_t snd_dev;              /* Audio 设备句柄 */

const unsigned short SinData[48]  = {

0x0000,
0x10b5,
0x2121,
0x30fc,
0x4000,
0x4dec,
0x5a82,
0x658d,
0x6eda,
0x7642,
0x7ba3,
0x7ee8,
0x7fff,
0x7ee8,
0x7ba3,
0x7642,
0x6eda,
0x658d,
0x5a82,
0x4dec,
0x4000,
0x30fc,
0x2121,
0x10b5,
0x0000,
0xef4b,
0xdedf,
0xcf04,
0xc000,
0xb214,
0xa57e,
0x9a73,
0x9126,
0x89be,
0x845d,
0x8118,
0x8000,
0x8118,
0x845d,
0x89be,
0x9126,
0x9a73,
0xa57e,
0xb214,
0xc000,
0xcf04,
0xdedf,
0xef4b
};

void adpll_init(uint8_t out_spr);
void dac_start(void);
void sddac_test(void)
{
   uint8_t *buff1 = (uint8_t *)rt_malloc(512*4);

   unsigned int i,j;
   //pll_init();
   //AudioPll2Init();
   //AudioPll2Init();
   rt_kprintf("T0\n");
   // AuPllInit();
   adpll_init(0);
   rt_kprintf("T1\n");
   dac_start();
   rt_kprintf("T2\n");

   AUBUFSIZE      = (512-1);
   AUBUFSTARTADDR = DMA_ADR(buff1);

   j = 0;
   for(i=0;i<256;i++){
      AUBUFDATA = SinData[j]<<16 | SinData[j];
      //AUBUFDATA = SinData[j]<<16 | 0;
      if(++j==48)j=0;
   }
   // DACDIGCON0  = BIT(0) | BIT(1) | BIT(10); // (0x01<<2)
   DACDIGCON0  = BIT(0) | BIT(10); // (0x01<<2)
   DACVOLCON   = 0x7fff; // -60DB
   DACVOLCON  |= BIT(20);
   i = 512*4;
   rt_kprintf("t1%08x\n",PLL1CON);
   rt_kprintf("t2%08x\n",PLL1DIV);
   //while(i--){
   while(1){
      while(AUBUFCON & BIT(8));

      //rt_kprintf("t3\n");
      AUBUFDATA  =  SinData[j]<<16 | SinData[j];
      //AUBUFDATA = SinData[j]<<16 | 0;
      if(++j==48) j = 0;
   }

   //simend();
   while(1);
}
MSH_CMD_EXPORT(sddac_test, "sddac_test");

void audio_test(void)
{
   rt_err_t ret = RT_ERROR;

   snd_dev = rt_device_find(SOUND_DEVICE_NAME);
   ret = rt_device_open(snd_dev, RT_DEVICE_OFLAG_WRONLY);
   if (ret == RT_EOK) {
      rt_kprintf("sound0 open successful\n");
   }

   rt_device_write(snd_dev, 0, SinData, 48);
   rt_device_close(snd_dev);
}
MSH_CMD_EXPORT(audio_test, "audio_test");

