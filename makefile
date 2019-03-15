#Top makefile for 8583dec project
#by adamhh 409520259@qq.com
OBJS = 8583_functions.o 8583_main.o
CC = gcc
CFLAGS = -Wall -0 -g
EXECF = output/8583

$(EXECF) : $(OBJS)
	$(CC) $(OBJS) -o $(EXECF)

8583_functions.o : 8583_functions.c 8583.h
	$(CC) -c 8583_functions.c -o 8583_functions.o

8583_main.o : 8583_main.c 8583.h
	$(CC) -c 8583_main.c -o 8583_main.o

.PHONY : clean
clean:
	rm *.o

.PHONY : cleanall
cleanall:
	rm *.o
	rm $(EXECF)
