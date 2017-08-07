#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/* Comment this out to use Radio Duty Cycle (RDC) and save energy */
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC          nullrdc_driver

#undef IEEE802154_CONF_PANID
#define IEEE802154_CONF_PANID      0xABCD

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          4
#endif

/* The following are Zoul (RE-Mote, etc) specific */
#undef CC2538_RF_CONF_CHANNEL
#define CC2538_RF_CONF_CHANNEL     26


#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE       256

#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW    60
#endif

#define EXAMPLE_TX_POWER  0xFF
#define EXAMPLE_CHANNEL   15
#define EXAMPLE_PANID     0xBEEF

/*---------------------------------------------------------------------------*/
#endif /* PROJECT_CONF_H_ */
