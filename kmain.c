#include "microknight.h"
#include "hw.h"

//
// Screen definitions
//
#define SCREENW 640U // Must be multiple of 16
#define SCREENH 256U // Don't increase this too much :)

// Note: The following only works because we know that the bss and data segments will end up in chip mem!
UWORD screen[SCREENW*SCREENH/16];
UWORD copperlist[] = {
    bpl1pth, 0x0000,
    bpl1ptl, 0x0000,
    0xffff,  0xfffe,
};

void screen_init() {
    //
    // Setup a 1-bpl screen using hires mode if SCREENW>320
    //
    const UWORD xres    = SCREENW>320?4:8; // 8=lores
    const UWORD xstart  = 0x81+(320-(xres==4?SCREENW/2:SCREENW))/2;
    const UWORD ystart  = 0x2c+(256-SCREENH)/2; // first scanline
    const UWORD dfstart = xstart/2-xres;

    BPLCON0  = (1<<12)|(xres==4?BPLCON0_HIRES:0)|BPLCON0_COLOR; // BPU=1
    BPLCON1  = 0;
    BPLCON2  = 0;
    BPLCON3  = 0;
    BPL1MOD  = 0;
    BPL2MOD  = 0;
    BPLPT[0] = (ULONG)screen;
    DIWSTRT  = (ystart<<8)|xstart;
    DIWSTOP  = (UWORD)((ystart+SCREENH)<<8)|((xstart+(xres==4?SCREENW/2:SCREENW))&0xff);
    DDFSTRT  = dfstart;
    DDFSTOP  = dfstart+xres*(SCREENW/16-8/xres);
    // Use some coder colors
    COLOR[0] = 0x006d;
    COLOR[1] = 0x0fff;


    //
    // Update copperlist with pointer to the screen buffer
    //
    copperlist[1] = ((ULONG)screen)>>16;
    copperlist[3] = ((ULONG)screen)&0xffff;
    COP1LC = (ULONG)copperlist;

}

void screen_put_string(int x, int y, const char* str) {
    UBYTE* c = ((UBYTE*)screen) + x + y * SCREENW;
    for (; *str; ++str, ++c) {
        const UBYTE* s = microknight[(*str&0x7f)-0x20];
        UBYTE* d = c;
        for (int r = 0; r < 8; ++r) {
            *d = *s++;
            d += SCREENW>>3;
        }
    }
}

void hexstring(ULONG num, char* buf, int num_nibbles) {
    char* d = &buf[num_nibbles];
    while (d > buf) {
        *--d = "0123456789abcdef"[num&0xf];
        num>>=4;
    }
}

static void delay1s(void) {
    extern void dly_100us(void);
    int i;
    for(i=0; i<10000; i++) dly_100us();
}

volatile ULONG frame_counter = 0;

void level3_interrupt_handler(void) {
    if (INTREQR & INTF_VERTB) {
        INTREQ = INTF_VERTB; // Acknowledge interrupt
        INTREQ = INTF_VERTB; // Twice to ensure compatability (probably not needed in this exact case)
        ++frame_counter;
    }
}

extern unsigned char _bss_end; // See rom.ld for memory map, but for now from _bss_end onwards is free to use

static print(const char* text) {
    static int y=0;
    screen_put_string(0, y, text);
    y += 10;
}    

void kmain(void) {
    extern void maprom_enable(register uint32_t asm("d0"));
    extern void maprom_disable(void);
    uint32_t map_rom = 0x0FF00000;
    uint32_t entry = 0;
    FATFS FatFs;
    
    // Setup screen
    screen_init();

    // Enable DMA to show our screen
    DMACON = DMAF_SETCLR|DMAF_MASTER|DMAF_RASTER|DMAF_COPPER;

    // Enable VBLANK interrupt
    // the initialization code has setup the appropriate vector
    // and calls level3_interrupt_handler
    INTENA = INTF_SETCLR | INTF_INTEN | INTF_VERTB;

    // Print status information
    print("Searching for KICK.ROM...");
    delay1s();
   
    pf_mount(&Fatfs);
    if (pf_open("KICK.ROM") != FR_OK) {
        print("Fatal Error: File not found!");
        while(1);
    }
    
    if(Fatfs.fsize ==  256 * 1024) {
        map_rom = map_rom + 768 * 1024;
    } else if(Fatfs.fsize ==  512 * 1024) {
        map_rom = map_rom + 512 * 1024;
    } else if(Fatfs.fsize != 1024 * 1024) {
        print("Fatal Error: ROM size is not 256KB, 512KB or 1MB!");
        while(1);
    }
    
    print("Loading ROM into high memory...");
    delay1s();
    
    maprom_disable();
    if(pf_read((void*)map_rom,  Fatfs.fsize, &bytes_read)) {
        print("Fatal Error: Error reading ROM image!");
        while(1);
    }
    if(bytes_read != Fatfs.fsize) {
        print("Fatal Error: Incomplete read!");
        while(1);
    }
    print("Copying ROM into low memory...");
    delay1s();
    
    if(Fatfs.fsize ==  256 * 1024) {
        memcpy(0xFC0000, (void*)map_rom, 256 * 1024);
        entry = 0xFC0004;
    } else if(Fatfs.fsize ==  512 * 1024) {
        memcpy(0xF80000, (void*)map_rom, 512 * 1024);
        entry = 0xF80004;
    } else {
        memcpy(0xF80000, (void*)(map_rom + 512 * 1024), 512 * 1024);
        memcpy(0xE00000, (void*)map_rom, 512 * 1024);
        entry = 0xE00004;
    }
    print("Done!");
    delay1s(); delay1s(); delay1s();
    
    maprom_enable(entry);
}
