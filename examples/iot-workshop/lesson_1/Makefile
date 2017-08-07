CONTIKI_PROJECT = 01-hello-world 02-led-and-button 03-timers 04-1-onboard-sensor 04-2-adc 05-1-gpio
all: $(CONTIKI_PROJECT)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CONTIKI_TARGET_SOURCEFILES += motion-sensor.c

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
