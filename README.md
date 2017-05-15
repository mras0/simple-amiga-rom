# Simple Amiga ROM

## Introduction

Meant as a simple starting point for exploring classic Amiga/68000 development. Based on discussion here: http://eab.abime.net/showthread.php?t=87087

Currently in a rough state and only tested with VBCC/VLINK/VASM and WinUAE in a basic A500 mode.

The code is kept minimal in terms of dependencies, but this means that it doesn't necessarily follow standard / good practices. Most likely it only works on my exact configuration.

## Building

Get a working setup of VBCC and friends. For example from http://www.pouet.net/prod.php?which=65625

In a command prompt go to the source directory and run:

    set VBCC=path_to_vbcc_executables
    m.cmd

Which should produce `out\rom.bin` that can be used as a ROM file for WinUAE.
