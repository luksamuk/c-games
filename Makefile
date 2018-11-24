CC       := cc
CFLAGS   := -Wall -g
LDFLAGS  :=
OUTFLAG  := -o
SOURCES  := $(wildcard *.c)
BINARIES := $(SOURCES:%.c=%.out)

.PHONY: all clean

all: $(BINARIES)

%.out: %.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(OUTFLAG) $@

clean:
	rm -rf *~ *.out

