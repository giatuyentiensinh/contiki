all: dtls-client dtls-server

CONTIKI=../../..

#CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

APPS += tinydtls/aes tinydtls/sha2 tinydtls/ecc tinydtls
#APPS += tinydtls/aes  tinydtls/ecc tinydtls

CONTIKI_WITH_IPV6 = 1
UIP_CONF_RPL = 1

# To try to optimize the compilation (original from TinyDTLS).
CFLAGS += -ffunction-sections
LDFLAGS += -Wl,--gc-sections,--undefined=_reset_vector__,--undefined=InterruptVectors,--undefined=_copy_data_init__,--undefined=_clear_bss_init__,--undefined=_end_of_init__

CFLAGS += -DSHA2_USE_INTTYPES_H

include $(CONTIKI)/Makefile.include