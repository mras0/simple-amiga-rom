.PHONY: all clean

OUTDIR:=out
ROM:=$(OUTDIR)/rom.bin
OBJS:=$(addprefix $(OUTDIR)/,init.o kmain.o)

ifeq ($(AS),as)
	AS=vasm
endif
AFLAGS+=-quiet -phxass -x -Fvobj
LINK:=vlink -brawbin1
ifeq ($(CC),cc)
	CC:=vc
endif

ifeq (,$(findstring gcc,$(CC)))
	# assume vbcc
	CFLAGS+=-c99 -warn=-1 -dontwarn=79 -dontwarn=166 -dontwarn=168 -dontwarn=81 -maxerrors=0 -speed -O=1023
else
	CFLAGS+=-nostartfiles -nostdlib -fomit-frame-pointer -Wall -Wextra -pedantic -O3 -m68000 -mregparm=2
endif

all: $(ROM)

$(OUTDIR)/%.o: %.s
	$(AS) $(AFLAGS) -o $@ $<

$(OUTDIR)/%.o: %.c $(wildcard *.h)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJS): | $(OUTDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(ROM): $(OBJS)
	$(LINK) -T rom.ld -o $@ $^

clean:
	-rm -f $(OBJS) $(ROM)
