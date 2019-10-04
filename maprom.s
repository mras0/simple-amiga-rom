******************************************************************
**
** Program   : MapEmuTOS
** Version   : 0.1 (Mar. 2019)
** Author    : flype
** Copyright : (c) APOLLO-Team, 2016-2019.
**
** Compiler  : Amiga/Atari HiSoft Devpac 3.18
**             VASM -Ftos mapemutos.s -o maptos.prg
**
******************************************************************

  MACHINE  MC68020
  XREF _maprom_enable
  XREF _maprom_disable

_maprom_disable:
	move.w   #$b00b,$dff3fe      ; maprom off
	tst.w    $dff002             ; sync
	rts                          ; and return

_maprom_enable:
	move.w   #$0001,$dff3fe      ; maprom on
	tst.w    $dff002             ; sync

	bchg.b   #1,$bfe001          ; ciaapra -> LED
	tst.w    $dff002             ; sync

	move.l   d0,a0               ; load start of ROM entry point
	reset                        ; reset instruction
	jmp      (a0)                ; and kick-start the ROM (get it?)
	nop                          ; pad the prefetch just in case
	nop

;=================================================================

	CNOP     0,4
ROMFILE:
	INCBIN   "emutos.rom"

;=================================================================
