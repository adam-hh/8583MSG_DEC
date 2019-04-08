# Makefile for cygwin gcc
# Nate Lawson <nate@rootlabs.com>

PCAP_PATH = ../lib/x64
CFLAGS = -shared -O -Wall -m64 -I ../include -I ../include/adamhh -I ./
CC=x86_64-w64-mingw32-gcc

OBJS = javaapi.o loadNpcapDlls.o list.o print.o callback.o userbuff.o 
LIBS = -L ${PCAP_PATH} -lwpcap

all: ${OBJS}
	${CC} ${CFLAGS} -o npcapdump.dll ${OBJS} ${LIBS}

clean:
	rm -f ${OBJS} npcapdump.dll

.c.o:
	${CC} ${CFLAGS} -c -o $*.o $<
