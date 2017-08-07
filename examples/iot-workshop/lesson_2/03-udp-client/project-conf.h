#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nullrdc_driver
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK       1

/* Define as minutes */
#define RPL_CONF_DEFAULT_LIFETIME_UNIT   60

/* 10 minutes lifetime of routes */
#define RPL_CONF_DEFAULT_LIFETIME        10
#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME 1
#endif
