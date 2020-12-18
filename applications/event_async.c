#include <rtthread.h> 
#include <rtdevice.h>
#include "board.h"
#include <multi_button.h>
#include "wavplayer.h"

#define BUTTON_PIN_0 rt_pin_get("PF.0")
#define BUTTON_PIN_1 rt_pin_get("PF.1")

#define NUM_OF_SONGS    (2u)

static struct button btn_0;
static struct button btn_1;

static uint32_t cnt_0 = 0;
static uint32_t cnt_1 = 0;

static char *table[2] =
{
    "wav_1.wav",
    "wav_2.wav",
};

void saia_channels_set(uint8_t channels);
void saia_volume_set(rt_uint8_t volume);
uint8_t saia_volume_get(void);

static uint8_t button_read_pin_0(void) 
{
    return rt_pin_read(BUTTON_PIN_0);
}

static uint8_t button_read_pin_1(void) 
{
    return rt_pin_read(BUTTON_PIN_1);
}

static void button_0_callback(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch(btn_event_val)
    {
    case SINGLE_CLICK:
        if (cnt_0 == 1) {
            saia_volume_set(30);
        }else if (cnt_0 == 2) {
            saia_volume_set(50);
        }else {
            saia_volume_set(100);
            cnt_0 = 0;
        }
        cnt_0++;
        rt_kprintf("vol=%d\n", saia_volume_get());
        rt_kprintf("button 0 single click\n");
    break; 

    case DOUBLE_CLICK:
        if (cnt_0 == 1) {
            saia_channels_set(1);
        }else {
            saia_channels_set(2);
            cnt_0 = 0;
        }
        cnt_0++;
        rt_kprintf("button 0 double click\n");
    break; 

    case LONG_RRESS_START:
        rt_kprintf("button 0 long press start\n");
    break; 

    case LONG_PRESS_HOLD:
        rt_kprintf("button 0 long press hold\n");
    break; 
    }
}

static void button_1_callback(void *btn)
{
    uint32_t btn_event_val;
    
    btn_event_val = get_button_event((struct button *)btn);
    
    switch(btn_event_val)
    {
    case SINGLE_CLICK:
        wavplayer_play(table[(cnt_1++) % NUM_OF_SONGS]);
        rt_kprintf("button 1 single click\n");
    break; 

    case DOUBLE_CLICK:
        rt_kprintf("button 1 double click\n");
    break; 

    case LONG_RRESS_START:
        rt_kprintf("button 1 long press start\n");
    break; 

    case LONG_PRESS_HOLD:
        rt_kprintf("button 1 long press hold\n");
    break; 
    }
}

static void btn_thread_entry(void* p)
{
    while(1)
    {
        /* 5ms */
        rt_thread_delay(RT_TICK_PER_SECOND/200);
        button_ticks(); 
    }
}

static int multi_button_test(void)
{
    rt_thread_t thread = RT_NULL;

    /* Create background ticks thread */
    thread = rt_thread_create("btn", btn_thread_entry, RT_NULL, 1024, 10, 10);
    if(thread == RT_NULL)
    {
        return RT_ERROR; 
    }
    rt_thread_startup(thread);

    /* low level drive */
    rt_pin_mode  (BUTTON_PIN_0, PIN_MODE_INPUT_PULLUP); 
    button_init  (&btn_0, button_read_pin_0, PIN_LOW);
    button_attach(&btn_0, SINGLE_CLICK,     button_0_callback);
    button_attach(&btn_0, DOUBLE_CLICK,     button_0_callback);
    button_attach(&btn_0, LONG_RRESS_START, button_0_callback);
    button_attach(&btn_0, LONG_PRESS_HOLD,  button_0_callback);
    button_start (&btn_0);

    rt_pin_mode  (BUTTON_PIN_1, PIN_MODE_INPUT_PULLUP); 
    button_init  (&btn_1, button_read_pin_1, PIN_LOW);
    button_attach(&btn_1, SINGLE_CLICK,     button_1_callback);
    button_attach(&btn_1, DOUBLE_CLICK,     button_1_callback);
    button_attach(&btn_1, LONG_RRESS_START, button_1_callback);
    button_attach(&btn_1, LONG_PRESS_HOLD,  button_1_callback);
    button_start (&btn_1);

    return RT_EOK; 
}
INIT_APP_EXPORT(multi_button_test); 
