# Simple Amiga ROM

## Introduction

Meant as a simple starting point for exploring classic Amiga/68000 development. Based on discussion here: http://eab.abime.net/showthread.php?t=87087

The code is kept minimal in terms of dependencies, but this means that it doesn't necessarily follow standard / good practices. Most likely it only works on my exact configuration.

## Building

### For Windows

Get a working setup of vbcc and friends. For example from http://www.pouet.net/prod.php?which=65625

In a command prompt go to the source directory and run:

    set VBCC=path_to_vbcc_executables
    m.cmd

Which should produce `out\rom.bin` that can be used as a ROM file for WinUAE.

### For \*nix like environments

Get vbcc/vasm and vlink working. Possibly using Bebbos toolchain scripts: https://github.com/bebbo/amigaos-cross-toolchain (tested with revision e0e3762)

Run `make` to build `out/rom.bin`. Experimentally you can try doing `CC=m68k-amigaos-gcc make` to build the C source using the GCC cross compiler.

## Testing

### WinUAE

* Start WinUAE and in the Quickstart tab select "Model=A500/Configuration=(most common)".
* Go to the Miscellaneous tab and deselect "Show GUI on startup".
* On the Hardware->ROM tab select `out\rom.bin`
* Save the configuration to a file.
* You can now test using `winuae -config=path_to_config.uae`

### FS-UAE

Run `fs-uae --kickstart_file=path_to_rom.bin`
