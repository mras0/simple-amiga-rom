__initial_sp = 0x010000 ;

MEMORY {
    /* 512K Chip memory (minus interrupt vectors and stack) */
    ram: org=__initial_sp, len=0x80000-__initial_sp
    /* 256K ROM */
    rom: org=0xfc0000,    len=0x3fff0
    /* See extended discussion at http://eab.abime.net/showthread.php?t=87087 */
    vec: org=0xfffff0,    len=0x00010
}

SECTIONS {
    .rom: {
        *(init)
        *(CODE)
        . = ALIGN(16);
        __rom_data = . ;
    } >rom
    .vecs: {
        *(vectors)
    } >vec
    .data: {
        __data_beg = . ;
        *(DATA)
        . = ALIGN(16);
        __data_end = . ;
    } >ram AT>rom
    .bss (NOLOAD): {
        __bss_beg = . ;
        *(BSS)
        . = ALIGN(16);
        __bss_end = . ;
    } >ram
}
