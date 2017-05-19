#include "microknight.h"
#include "hw.h"

//
// Screen definitions
//
#define SCREENW 640 // Must be multiple of 16
#define SCREENH 256 // Don't increase this too much :)

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
    const int xres    = SCREENW>320?4:8; // 8=lores
    const int xstart  = 0x81+(320-(xres==4?SCREENW/2:SCREENW))/2;
    const int ystart  = 0x2c+(256-SCREENH)/2; // first scanline
    const int dfstart = xstart/2-xres;

    BPLCON0  = (1<<12)|(xres==4?BPLCON0_HIRES:0)|BPLCON0_COLOR; // BPU=1
    BPLCON1  = 0;
    BPLCON2  = 0;
    BPLCON3  = 0;
    BPL1MOD  = 0;
    BPL2MOD  = 0;
    BPLPT[0] = (ULONG)screen;
    DIWSTRT  = (ystart<<8)|xstart;
    DIWSTOP  = ((ystart+SCREENH)<<8)|((xstart+(xres==4?SCREENW/2:SCREENW))&0xff);
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

volatile ULONG frame_counter = 0;

void level3_interrupt_handler(void) {
    if (INTREQR & INTF_VERTB) {
        INTREQ = INTF_VERTB; // Acknowledge interrupt
        INTREQ = INTF_VERTB; // Twice to ensure compatability (probably not needed in this exact case)
        ++frame_counter;
    }
}

extern unsigned char _bss_end; // See rom.ld for memory map, but for now from _bss_end onwards is free to use

void kmain(void) {
    // Setup screen
    screen_init();

    // Print some information
    char buf[] = "End of BSS:  $........";
    hexstring((ULONG)&_bss_end, buf+sizeof(buf)-9, 8);
    screen_put_string(0, 0, buf);

    char buf2[] = "Free memory: $........";
    hexstring((512<<10)-(ULONG)&_bss_end, buf2+sizeof(buf2)-9, 8);
    screen_put_string(0, 1, buf2);

    // Enable DMA to show our screen
    DMACON = DMAF_SETCLR|DMAF_MASTER|DMAF_RASTER|DMAF_COPPER;

    // Enable VBLANK interrupt
    // the initialization code has setup the appropriate vector
    // and calls level3_interrupt_handler
    INTENA = INTF_SETCLR | INTF_INTEN | INTF_VERTB;

    // Run main loop for ever
    for (;;) {
        const ULONG current_frame = frame_counter;
        // Blink LED every 32th frame
        if ((current_frame&31)==0) CIAAPRA ^= 2;

        char text[] = "Frame $........";
        hexstring(current_frame, text+sizeof(text)-9, 8);
        screen_put_string(SCREENW/8-sizeof(text), SCREENH/8-1, text);

        // Wait for next frame
        while (frame_counter == current_frame)
            ;
    }
}
