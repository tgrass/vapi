CC=mipsel-openwrt-linux-uclibc-gcc
#CC = gcc
CFLAGS = -lm
OBJECT = main.o utils.o system.o wlan.o neighbour.o radio.o interface.o qos.o
TARGET = vapi
STAGING_DIR = /home/tarena/openwrt/trunk/staging_dir/

export STAGING_DIR

all: $(OBJECT)
	$(CC) $(OBJECT) $(CFLAGS) -o $(TARGET)

clean:
	@rm -rf $(OBJECT)

distclean: $(clean)
	@rm -f $(TARGET)

