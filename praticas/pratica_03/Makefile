# Para qual plataforma vamos compilar?
TARGET = srf06-cc26xx
BOARD = launchpad/cc1350

# Qual nossa aplicação/projeto?
CONTIKI_PROJECT = cliente-udp-sensor

# Quais outros arquivos precisamos compilar?
PROJECT_SOURCEFILES += uip_utils.c mdns_utils.c

# Há um arquivo com configurações globais do contiki?
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

all: $(CONTIKI_PROJECT)

CONTIKI = ../../contiki
CONTIKI_WITH_IPV6 = 1
CFLAGS += -g

include $(CONTIKI)/Makefile.include

# Forçar o Contiki a usar o compilador do CCS.
# Você deve ajustar a variável CC_PREFIX para apontar ao diretório do CCS.

#CC_PREFIX = /opt/ti/ccsv7/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin
#CC_PREFIX = /opt/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin
CC_PREFIX = /opt/ti/ccsv7/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin
CC      = $(CC_PREFIX)/arm-none-eabi-gcc
CPP     = $(CC_PREFIX)/arm-none-eabi-cpp
LD      = $(CC_PREFIX)/arm-none-eabi-gcc
AR      = $(CC_PREFIX)/arm-none-eabi-ar
OBJCOPY = $(CC_PREFIX)/arm-none-eabi-objcopy
OBJDUMP = $(CC_PREFIX)/arm-none-eabi-objdump
NM      = $(CC_PREFIX)/arm-none-eabi-nm
SIZE    = $(CC_PREFIX)/arm-none-eabi-size
