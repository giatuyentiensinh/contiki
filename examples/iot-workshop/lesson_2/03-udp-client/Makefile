all: udp-client udp-client-sensor
CONTIKI=../../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
CONTIKI_TARGET_SOURCEFILES += dht22.c

include $(CONTIKI)/Makefile.include

turn-port: $(CONTIKI)/examples/iot-workshop/lesson_2/03-udp-client/javacode
	javac javacode/*.java && java javacode.UDPServer

clean-java-class: $(CONTIKI)/examples/iot-workshop/lesson_2/03-udp-client/javacode
	rm -rf javacode/*.class
