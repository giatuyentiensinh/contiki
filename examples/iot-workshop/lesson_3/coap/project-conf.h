#ifndef __PROJECT_ERBIUM_CONF_H__
#define __PROJECT_ERBIUM_CONF_H__

/* The masks below converts the Port number and Pin number to base and mask values */
#define EXAMPLE_PORT_BASE              GPIO_PORT_TO_BASE(GPIO_A_NUM)
#define EXAMPLE_PIN_MASK               GPIO_PIN_MASK(5)

/* IP buffer size must match all other hops, in particular the border router. */
#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE           256

/* Disabling RDC and CSMA for demo purposes. Core updates often
   require more memory. */
/* For projects, optimize memory and enable RDC and CSMA again. */
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC              nullrdc_driver

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC              nullmac_driver

/* Disabling TCP on CoAP nodes. */
#undef UIP_CONF_TCP
#define UIP_CONF_TCP                   0

/* Increase rpl-border-router IP-buffer when using more than 64. */
#undef REST_MAX_CHUNK_SIZE
#define REST_MAX_CHUNK_SIZE            96

/* Multiplies with chunk size, be aware of memory constraints. */
#undef COAP_MAX_OPEN_TRANSACTIONS
#define COAP_MAX_OPEN_TRANSACTIONS     4

/* Filtering .well-known/core per query can be disabled to save space. */
#undef COAP_LINK_FORMAT_FILTERING
#define COAP_LINK_FORMAT_FILTERING     0
#undef COAP_PROXY_OPTION_PROCESSING
#define COAP_PROXY_OPTION_PROCESSING   0

/* Enable client-side support for COAP observe */
#define COAP_OBSERVE_CLIENT 1
#endif /* __PROJECT_ERBIUM_CONF_H__ */
