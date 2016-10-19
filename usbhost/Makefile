CC:=gcc
LD:=gcc

############################################
# EDIT THESE TO REFLECT LOCATION OF libusb #
############################################
INCLUDES:=-I/usr/local/Cellar/libusb/1.0.20/include/libusb-1.0
LIBS:=-L/usr/local/Cellar/libusb/1.0.20/lib

CFLAGS:=-std=c99 -Wall $(INCLUDES)
LDFLAGS:=$(LIBS) -lusb-1.0

OBJS:=main.o efm32_comm.o usb_helpers.o debug.o

.PHONY : debug
debug : CFLAGS += -DDEBUG
debug : usb_host

.PHONY : nodebug
nodebug : CFLAGS += -DNODEBUG
nodebug : usb_host

.PHONY : run
run : usb_host
	./usb_host

usb_host : $(OBJS)
	@echo "Linking target: $@"
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o : %.c
	@echo "Compiling source: $@"
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean :
	-rm -rf usb_host *.o