/*---------------------------------------------------------------------------*/
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/* Exercise 2, 4 */
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

/* IP buffer size must match all other hops, in particular the border router. */
#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE           256

/* Exercise 3 */
#undef REST_MAX_CHUNK_SIZE
#define REST_MAX_CHUNK_SIZE            120

/* Disabling TCP on CoAP nodes. */
#undef UIP_CONF_TCP
#define UIP_CONF_TCP                   0

/* Exercise 4 */
#define NETSTACK_CONF_NETWORK sicslowpan_driver
#define NETSTACK_CONF_MAC     csma_driver
#define NETSTACK_CONF_RDC     contikimac_driver

#endif /* PROJECT_CONF_H_ */
/*---------------------------------------------------------------------------*/
