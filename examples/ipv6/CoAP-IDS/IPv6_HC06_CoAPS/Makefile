all: coaps-client coaps-server
# use target "er-plugtest-server" explicitly when requried 

CONTIKI=../../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# automatically build RESTful resources
# RAFS: Original code 
REST_RESOURCES_DIR = ./resources
ifndef TARGET
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c'))
else
ifeq ($(TARGET), native)
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c'))
else
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c' ! -name 'res-plugtest*'))
endif
endif

PROJECTDIRS += $(REST_RESOURCES_DIR)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)


# REST Engine shall use Erbium CoAP implementation
APPS += rest-engine
APPS += tinydtls/aes tinydtls/sha2 tinydtls/ecc tinydtls 
APPS += er-coaps



CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include


# Taken from the tinydtls example Makefile
# They try to optimize more the memory allocation of the program.
########################################################################

LDFLAGS += -Wl,--gc-sections,--undefined=_reset_vector__,--undefined=InterruptVectors,--undefined=_copy_data_init__,--undefined=_clear_bss_init__,--undefined=_end_of_init__

CFLAGS += -DSHA2_USE_INTTYPES_H

# Those are for letting communication between Cooja and the real world
# Though the real world are other VM.. bleh
########################################################################

# border router rules
# RAFS: Original code (Need deep analyze )
$(CONTIKI)/tools/tunslip6:	$(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 -p 60002 aaaa::1/64

connect-router-native:	$(CONTIKI)/examples/ipv6/native-border-router/border-router.native
	sudo $(CONTIKI)/exmples/ipv6/native-border-router/border-router.native -a 127.0.0.1 -p 60002 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0 

# Load the extern TinyDTLS repository to Contiki!
# TODO: Requires to be better integrated to this Maefile 
#       Ex: "make all TARGET=xxx"  should be enough
########################################################################
APPS_NAME=tinydtls
APPS_URL=git://github.com/rfuentess/TinyDTLS.git
APPS_VERSION=Contiki

APPS_DIR=$(CONTIKI)/apps/$(APPS_NAME)

TinyDTLS-git: 
	test -d "$(APPS_DIR)" || git clone "$(APPS_URL)" "$(APPS_DIR)"; \
	cd "$(APPS_DIR)" && git checkout -f "$(APPS_VERSION)"

TinyDTLS-clean:
	rm -rf "$(APPS_DIR)"