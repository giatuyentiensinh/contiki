CONTIKI_PROJECT = node
all: $(CONTIKI_PROJECT)
CONTIKI=../../../..
CONTIKI_WITH_IPV6 = 1
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"


MODULES += core/net/mac/tsch

include $(CONTIKI)/Makefile.include
