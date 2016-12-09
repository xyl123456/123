PREFIX=mipsel-openwrt-linux-

CC=$(PREFIX)gcc
#LD=$(PREFIX)ld
#STRIP=$(PREFIX)strip

LFLAGS=-lpthread

SOURCE=$(shell ls *.c)
TARGET=$(notdir $(shell pwd))

.PHONY:all

all:clean $(TARGET)

$(TARGET):$(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE) $(LFLAGS)

clean:
	-rm -rf $(TARGET) *.o

