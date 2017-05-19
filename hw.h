#ifndef HW_H
#define HW_H

typedef unsigned char  UBYTE;
typedef unsigned short UWORD;
typedef unsigned long  ULONG;

#define HARDWARE_REGISTER(type, byte_offset) ((volatile type*)(byte_offset))

//
// CIA
//
#define CIAAPRA *HARDWARE_REGISTER(UBYTE, 0xbfe001)

//
// Custom
//
#define CUSTOM_BASE 0xdff000
#define CUSTOMWR(byte_offset) *HARDWARE_REGISTER(const UWORD, CUSTOM_BASE+byte_offset)
#define CUSTOMWW(byte_offset) *HARDWARE_REGISTER(UWORD, CUSTOM_BASE+byte_offset)
#define CUSTOMLR(byte_offset) *HARDWARE_REGISTER(const ULONG, CUSTOM_BASE+byte_offset)
#define CUSTOMLW(byte_offset) *HARDWARE_REGISTER(ULONG, CUSTOM_BASE+byte_offset)

#define VPOSR   ((CUSTOMLR(4)>>8)&0x1ff)

#define INTENAR CUSTOMWR(0x01c)
#define INTREQR CUSTOMWR(0x01e)
#define INTENA  CUSTOMWW(0x09a)
#define INTREQ  CUSTOMWW(0x09c)
#define INTF_SETCLR	0x8000
#define INTF_INTEN  0x4000
#define INTF_VERTB  0x0020

#define COP1LC  CUSTOMLW(0x080)
#define DIWSTRT CUSTOMWW(0x08e)
#define DIWSTOP CUSTOMWW(0x090)
#define DDFSTRT CUSTOMWW(0x092)
#define DDFSTOP CUSTOMWW(0x094)

#define DMACON  CUSTOMWW(0x096)
#define DMAF_SETCLR  0x8000
#define DMAF_MASTER  0x0200
#define DMAF_RASTER  0x0100
#define DMAF_COPPER  0x0080

#define bpl1pth 0x0e0
#define bpl1ptl 0x0e2
#define BPLPT   HARDWARE_REGISTER(ULONG, CUSTOM_BASE+bpl1pth)

#define BPLCON0 CUSTOMWW(0x100)
#define BPLCON0_HIRES 0x8000
#define BPLCON0_COLOR 0x0200

#define BPLCON1 CUSTOMWW(0x102)
#define BPLCON2 CUSTOMWW(0x104)
#define BPLCON3 CUSTOMWW(0x106)
#define BPL1MOD CUSTOMWW(0x108)
#define BPL2MOD CUSTOMWW(0x10a)

#define COLOR   HARDWARE_REGISTER(UWORD, CUSTOM_BASE+0x180)

#endif
