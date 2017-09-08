# This is a custom Makefile for the original TinyDTLS example. 
# The reason behind this custom (instead of the original) is due the changes  
# between Contiki 2.6 and the Contiki 3.0

all: dtls-client dtls-server

CONTIKI=../../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

APPS += tinydtls/aes tinydtls/sha2 tinydtls/ecc tinydtls
#APPS += tinydtls/aes  tinydtls/ecc tinydtls

CONTIKI_WITH_IPV6 = 1
UIP_CONF_RPL = 1

# To try to optimize the compilation (original from TinyDTLS).
CFLAGS += -ffunction-sections
LDFLAGS += -Wl,--gc-sections,--undefined=_reset_vector__,--undefined=InterruptVectors,--undefined=_copy_data_init__,--undefined=_clear_bss_init__,--undefined=_end_of_init__

CFLAGS += -DSHA2_USE_INTTYPES_H

(CONTIKI)/tools/tunslip6:	$(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 -p 60001 aaaa::1/64

connect-router-native:	$(CONTIKI)/examples/ipv6/native-border-router/border-router.native
	sudo $(CONTIKI)/exmples/ipv6/native-border-router/border-router.native -a 127.0.0.1 -p 60001 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0 
	
# Load the extern TinyDTLS repository to Contiki!
# TODO: Requires to be better integrated to this Makefile 
#       Ex: "make all TARGET=xxx"  should be enough
########################################################################
APPS_NAME=tinydtls

 APPS_URL=https://github.com/rfuentess/TinyDTLS.git
 APPS_VERSION=Contiki

# This enable another fork (used succesfully) but require add support to the mote.
# Similar to this: https://github.com/rfuentess/TinyDTLS/tree/Contiki/platform-specific
# APPS_URL=https://github.com/cetic/tinydtls.git
# APPS_VERSION=6lbr

APPS_DIR=$(CONTIKI)/apps/$(APPS_NAME)

TinyDTLS-git: 
	test -d "$(APPS_DIR)" || git clone "$(APPS_URL)" "$(APPS_DIR)"; \
	cd "$(APPS_DIR)" && git checkout -f "$(APPS_VERSION)"

TinyDTLS-clean:
	rm -rf "$(APPS_DIR)"

include $(CONTIKI)/Makefile.include