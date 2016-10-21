################
# DEFINITIONS
################

# This will be the name of the executable
TARGET:=ytelse_usbhost

# Directories
OBJDIR:=build
SRCDIR:=src

################
# TOOLCHAIN
################

CC:=gcc
LD:=gcc

################
# FLAGS
################

LIBUSB_PATH:=/usr/local/Cellar/libusb/1.0.20

INCLUDES:=-I$(LIBUSB_PATH)/include/libusb-1.0 -Isrc
LIBS:=-L$(LIBUSB_PATH)/lib

CFLAGS:=-std=c99 -Wall $(INCLUDES)
LDFLAGS:=$(LIBS) -lusb-1.0

################
# FILES
################

CSRC:= $(addprefix $(SRCDIR)/, \
main.c \
usb_helpers.c \
efm32_comm.c \
callbacks.c \
debug.c \
)

C_FILES := $(notdir $(CSRC))
C_PATHS := $(dir $(CSRC))
C_OBJS := $(addprefix $(OBJDIR)/, $(C_FILES:.c=.o))
OBJS = $(C_OBJS)

vpath %.c $(C_PATHS)

################
# BUILD RULES
################

.PHONY : debug
debug : CFLAGS += -DDEBUG
debug : $(TARGET)

.PHONY : nodebug
nodebug : CFLAGS += -DNODEBUG
nodebug : $(TARGET)

.PHONY : run
run : $(TARGET)
	./$(TARGET)

$(TARGET) : $(OBJS)
	@echo "Linking target: $@"
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o : %.c | $(OBJDIR)
	@echo "Compiling source: $@"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR) :
	mkdir $@

.PHONY : clean
clean :
	-rm -rf $(OBJDIR) $(TARGET)