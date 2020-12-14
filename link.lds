/* Define the flash max size */
__max_flash_size = 768k;

__comm_ram_size = 84k;
__heap_ram_size = 16k;

__base = 0x10000000;

__comm_vma = 0x17000;
__heap_vma = __comm_vma + __comm_ram_size;

__ram1_vma = 0x50000;

MEMORY
{
    init            : org = __base,             len = 512
    flash(rx)       : org = __base + 512,       len = __max_flash_size
    comm(rx)        : org = __comm_vma,         len = __comm_ram_size

    data            : org = 0x11000,            len = 16k
    stack           : org = 0x15000,            len = 8k
    heap            : org = __heap_vma,         len = __heap_ram_size
    ram1(rx)        : org = __ram1_vma,         len = 0x7a00
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .comm : {
        KEEP(*(.vector))
        *(.text*)
        *(.rodata*)
        *(.srodata*)
        *(.rela*)
        *(.data*)
	    *(.sdata*)
        . = ALIGN(512);
    } > comm AT > flash

    .flash : {
        . = ALIGN(512);
    } > flash

    .bss (NOLOAD):
    {
        __bss_start = .;
        *(COMMON)
        *(.bss)
        *(.sbss)
        *(.buf*)
        __bss_end = .;
    } > data
    __bss_size = __bss_end - __bss_start;

    .stack (NOLOAD) : {
        __irq_stack_start = .;
        . = 0x2000;
        __irq_stack = .;
    } > stack
    __irq_stack_size = __irq_stack - __irq_stack_start;

    .heap : {
        __heap_start = .;
        . = 0x4000;
        __heap_end = .;
    } > heap

    .ram1 __ram1_vma : {
        *(.drv_text*)
        *(.sys_text*)
        . = ALIGN(512);
    } > ram1 AT > flash

    .rti : {
        . = ALIGN(4);
        /* section information for initial */
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;

        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;

        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;

        . = ALIGN(512);
    } > ram1 AT > flash
}

/* Calc the lma */
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
__ram1_lma = LOADADDR(.ram1);
__ram1_size = SIZEOF(.ram1) + SIZEOF(.rti);
