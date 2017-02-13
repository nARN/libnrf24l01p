CC     = avr-gcc
AR     = avr-ar
RANLIB = avr-ranlib 

MCU       = atmega328p
CPU_SPEED = 16000000UL
CFLAGS    = -mmcu=$(MCU) -DF_CPU=$(CPU_SPEED) -std=c99  -Os -w 

INCLUDEDIR = ../include/nrf24l01+/
LIBDIR     = ../lib/

LIBNAME  = libnrf24l01+.a
LIB_SRCS = radio.c spi.c
LIB_OBJS = $(LIB_SRCS:.c=.o) 

default: $(LIB_OBJS)
	avr-ar rcs $(LIBNAME) $^
	mkdir -p $(INCLUDEDIR)
	mkdir -p $(LIBDIR)
	cp *.h $(INCLUDEDIR)
	mv $(LIBNAME) $(LIBDIR)

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f *.o

