#include <rtthread.h>

uint16_t os_spiflash_read(void *buf, uint32_t addr, uint16_t len);
void os_spiflash_program(void *buf, uint32_t addr, uint16_t len);
void os_spiflash_erase(uint32_t addr);

// Flash size 0x100000
// #define FLASH_TEST_START  0xFE000
#define FLASH_TEST_START  0xFFE00
#define FLASH_TEST_END    0x100000
uint8_t spi_buf[256];
uint8_t read_buf[256];

void print_r(void *buf, uint32_t size)
{
    uint8_t *ptr = buf;
    uint32_t cnt = 0;

    while (cnt++ != size) {
        if ((cnt - 1) % 16 == 0) {
            rt_kprintf("\n");
        }
        rt_kprintf("%02x ", *ptr++);
    }
    rt_kprintf("\n");
}

void spi_test(void)
{
    static uint32_t w_addr = FLASH_TEST_START;
    static uint8_t tag_idx = 0;
    const uint8_t tag_tab[] = {0x00,0xFF,0x55,0xAA};
    static uint32_t ok_cnt = 0, err_cnt = 0;
    if (FLASH_TEST_START == w_addr) {
        rt_memset(spi_buf, tag_tab[tag_idx], 256);       //往spi_buf里放示例数,一共256字节
        rt_kprintf("tag = 0x%X\n", tag_tab[tag_idx]);
        print_r(spi_buf, 16);
        os_spiflash_erase(FLASH_TEST_START);          //擦除的256k LAST 8K
        os_spiflash_erase(FLASH_TEST_START + 0X1000);
    }
    os_spiflash_program(spi_buf, w_addr, 256);
    w_addr += 256;
    rt_kprintf("[0x%X]",w_addr);
    if (FLASH_TEST_END == w_addr) {   //read
        //read and cmpare
        for (uint32_t r_addr = FLASH_TEST_START; r_addr < FLASH_TEST_END; r_addr += 256) {
            rt_memset(read_buf, 0xCC,256);
            os_spiflash_read(read_buf, r_addr,256);
            if (rt_memcmp(read_buf, spi_buf,256) == 0) {
                ok_cnt += 1;
            } else {
                err_cnt += 1;
                rt_kprintf("ERR, buf:\n");
                print_r(spi_buf, 256);
                rt_kprintf("-----------------------------------");
                print_r(read_buf, 256);
           }
        }
        rt_kprintf("[OK size = %d kb, Err= %d times]\n", ok_cnt/4,err_cnt);
        //REST PARAM
        w_addr = FLASH_TEST_START;
        tag_idx++;
        if (tag_idx == sizeof(tag_tab)) {
            tag_idx = 0;
        }
    }
}
MSH_CMD_EXPORT(spi_test, spi_test);
