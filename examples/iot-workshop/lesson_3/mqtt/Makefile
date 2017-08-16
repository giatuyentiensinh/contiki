all: mqtt-example

CONTIKI = ../../../..
DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
APPS += mqtt

# Linker size optimization
SMALL = 1

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include
