#Top makefile for 8583dec project
#by adamhh 409520259@qq.com
CFLAGS = -shared -Wall -O -m64
CC=x86_64-w64-mingw32-gcc

OBJS = native.o 8583_functions.o

all: ${OBJS}
	${CC} ${CFLAGS} -o native.dll ${OBJS}

clean:
	rm -f ${OBJS} native.dll

.c.o:
	${CC} -Wall -O -c -o $*.o $<

