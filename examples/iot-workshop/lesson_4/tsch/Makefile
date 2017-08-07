CONTIKI_PROJECT = node
all: $(CONTIKI_PROJECT)

CONTIKI=../../../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CONTIKI_WITH_IPV6 = 1

MODULES += core/net/mac/tsch

include $(CONTIKI)/Makefile.include
