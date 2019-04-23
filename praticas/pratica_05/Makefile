# # Para qual plataforma vamos compilar?
TARGET = srf06-cc26xx
BOARD = launchpad/cc1350

# # Qual nossa aplicação/projeto?
CONTIKI_PROJECT = mqttsn-client

# # Quais outros arquivos precisamos compilar?
# PROJECT_SOURCEFILES += mqtt-sn.c cetic-6lbr-client.c mdns_utils.c uip_utils.c
PROJECT_SOURCEFILES += mqtt-sn.c uip_utils.c

# # Há um arquivo com configurações globais do contiki?
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

all: $(CONTIKI_PROJECT)

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS+= -DUIP_CONF_IPV6_RPL
CONTIKI_WITH_IPV6 = 1
CFLAGS += -DUIP_CONF_ND6_SEND_NS=1
CFLAGS += -g

CONTIKI=../../contiki

include $(CONTIKI)/Makefile.include



# Forçar o Contiki a usar o compilador do CCS.
# Você deve ajustar a variável CC_PREFIX para apontar ao diretório do CCS.

#CC_PREFIX = /export/tools/ti_ccs/ccs830/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin
#CC_PREFIX = /opt/ti/ccsv7/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin
#CC_PREFIX = /opt/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin
#CC_PREFIX = /opt/ti/ccsv9/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin
CC_PREFIX = /opt/ti/ccsv7/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin

CC      = $(CC_PREFIX)/arm-none-eabi-gcc
CPP     = $(CC_PREFIX)/arm-none-eabi-cpp
LD      = $(CC_PREFIX)/arm-none-eabi-gcc
AR      = $(CC_PREFIX)/arm-none-eabi-ar
OBJCOPY = $(CC_PREFIX)/arm-none-eabi-objcopy
OBJDUMP = $(CC_PREFIX)/arm-none-eabi-objdump
NM      = $(CC_PREFIX)/arm-none-eabi-nm
SIZE    = $(CC_PREFIX)/arm-none-eabi-size
