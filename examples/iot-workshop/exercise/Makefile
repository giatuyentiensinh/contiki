CONTIKI_PROJECT = exercise1 exercise2 exercise3 exercise4
all: $(CONTIKI_PROJECT)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
CONTIKI_TARGET_SOURCEFILES += dht22.c

REST_RESOURCES_DIR = ./resources_ex3
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c' ! -name 'res-plugtest*'))
PROJECTDIRS += $(REST_RESOURCES_DIR)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

APPS += er-coap
APPS += rest-engine

CONTIKI_WITH_IPV6 = 1
CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
