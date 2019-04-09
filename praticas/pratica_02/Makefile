TARGET = srf06-cc26xx
BOARD = launchpad/cc1350

CONTIKI_PROJECT = sensor-timer

all: $(CONTIKI_PROJECT)

CONTIKI = ../../contiki
CFLAGS += -g
include $(CONTIKI)/Makefile.include

# Force Contiki to USE CCS's compiler instead of system compiler
# You may need to adjust CC_PREFIX to point to CCS compiler binaries folder

CC_PREFIX = /opt/ti/ccsv7/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin
#CC_PREFIX = /export/tools/ti_ccs/gcc-arm-none-eabi-5_4-2016q3/bin
CC      = $(CC_PREFIX)/arm-none-eabi-gcc
CPP     = $(CC_PREFIX)/arm-none-eabi-cpp
LD      = $(CC_PREFIX)/arm-none-eabi-gcc
AR      = $(CC_PREFIX)/arm-none-eabi-ar
OBJCOPY = $(CC_PREFIX)/arm-none-eabi-objcopy
OBJDUMP = $(CC_PREFIX)/arm-none-eabi-objdump
NM      = $(CC_PREFIX)/arm-none-eabi-nm
SIZE    = $(CC_PREFIX)/arm-none-eabi-size
