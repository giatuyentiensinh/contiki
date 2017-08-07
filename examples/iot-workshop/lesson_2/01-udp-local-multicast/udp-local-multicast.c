#include "contiki.h"

/* The following libraries add IP/IPv6 support */
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"

/* This is quite handy, allows to print IPv6 related stuff in a readable way */
#include "net/ip/uip-debug.h"

/* The simple UDP library API */
#include "simple-udp.h"

/* Library used to read the metadata in the packets */
#include "net/packetbuf.h"

/* Plus sensors to send data */
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"

/* And this you should be familiar with from the basic lessons... */
#include "sys/etimer.h"
#include "dev/leds.h"

#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/*---------------------------------------------------------------------------*/
#define SEND_INTERVAL (15 * CLOCK_SECOND)
#define UDP_PORT 5678
/*---------------------------------------------------------------------------*/
/* The structure used in the Simple UDP library to create an UDP connection */
static struct simple_udp_connection mcast_connection;
/*---------------------------------------------------------------------------*/
/* This data structure is used to store the packet content (payload) */
struct my_msg_t {
  uint16_t adc1;
  uint16_t adc2;
  uint16_t adc3;
  uint16_t temperature;
  uint16_t battery;
};
/*---------------------------------------------------------------------------*/
/* Create a structure and pointer to store the data to be sent as payload */
static struct my_msg_t msg;
/*---------------------------------------------------------------------------*/
PROCESS(mcast_example_process, "UDP multicast example process");
AUTOSTART_PROCESSES(&mcast_example_process);
/*---------------------------------------------------------------------------*/
/* This is the receiver callback, we tell the Simple UDP library to call this
 * function each time we receive a packet
 */
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  /* Create a pointer to the received data, adjust to the expected structure */
  struct my_msg_t *msgPtr = (struct my_msg_t *) data;
  leds_toggle(LEDS_GREEN);
  printf("\n***\nMessage from: " ANSI_COLOR_RED);

  /* Converts to readable IPv6 address */
  uip_debug_ipaddr_print(sender_addr);
  printf(ANSI_COLOR_RESET);

  printf("\nData received on port %d from port %d with length %d\n",
         receiver_port, sender_port, datalen);
         
  printf(ANSI_COLOR_GREEN);
  /* Print the received data */
  printf("ADC1: %d, ADC2: %d, ADC3: %d, core temp: %d.%u, batt: %u\n",
          msgPtr->adc1, msgPtr->adc2, msgPtr->adc3, 
          msgPtr->temperature / 1000, msgPtr->temperature % 1000, msgPtr->battery);
  printf(ANSI_COLOR_RESET);
}
/*---------------------------------------------------------------------------*/
static void
take_readings(void)
{
  uint32_t aux;

  msg.adc1  = adc_zoul.value(ZOUL_SENSORS_ADC1);
  msg.adc2  = adc_zoul.value(ZOUL_SENSORS_ADC2);
  msg.adc3  = adc_zoul.value(ZOUL_SENSORS_ADC3);
  msg.temperature  = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
  aux = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
  msg.battery = (uint16_t) aux;

  printf(ANSI_COLOR_BLUE);
  /* Print the sensor data */
  printf("ADC1: %d, ADC2: %d, ADC3: %d, core temp: %u.%u, batt: %u\n",
          msg.adc1, msg.adc2, msg.adc3, 
          msg.temperature / 1000, msg.temperature % 1000, msg.battery);
  printf(ANSI_COLOR_RESET);
}
/*---------------------------------------------------------------------------*/
static void
print_radio_values(void)
{
  radio_value_t aux;
  printf("\n* Radio parameters:\n");

  NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &aux);
  printf("   Channel %u", aux);

  NETSTACK_RADIO.get_value(RADIO_CONST_CHANNEL_MIN, &aux);
  printf(" (Min: %u, ", aux);

  NETSTACK_RADIO.get_value(RADIO_CONST_CHANNEL_MAX, &aux);
  printf("Max: %u)\n", aux);

  NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &aux);
  printf("   Tx Power %3d dBm", aux);

  NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MIN, &aux);
  printf(" (Min: %3d dBm, ", aux);

  NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MAX, &aux);
  printf("Max: %3d dBm)\n", aux);

  /* This value is set in contiki-conf.h and can be changed */
  printf("   PAN ID: 0x%02X\n", IEEE802154_CONF_PANID);
}
/*---------------------------------------------------------------------------*/
static void
set_radio_default_parameters(void)
{
  NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, EXAMPLE_TX_POWER);
  // NETSTACK_RADIO.set_value(RADIO_PARAM_PAN_ID, EXAMPLE_PANID);
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, EXAMPLE_CHANNEL);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mcast_example_process, ev, data)
{
  static struct etimer periodic_timer;

  /* Data container used to store the IPv6 addresses */
  uip_ipaddr_t addr;

  PROCESS_BEGIN();

  /* Alternatively if you want to change the channel or transmission power, this
   * are the functions to use.  You can also change these values in runtime.
   * To check what are the regular platform values, comment out the function
   * below, so the print_radio_values() function shows the default.
   */
  set_radio_default_parameters();

  /* This blocks prints out the radio constants (minimum and maximum channel,
   * transmission power and current PAN ID (more or less like a subnet)
   */
  print_radio_values();

  /* Create the UDP connection.  This function registers a UDP connection and
   * attaches a callback function to it. The callback function will be
   * called for incoming packets. The local UDP port can be set to 0 to indicate
   * that an ephemeral UDP port should be allocated. The remote IP address can
   * be NULL, to indicate that packets from any IP address should be accepted.
   */
  simple_udp_register(&mcast_connection, UDP_PORT, NULL,
                      UDP_PORT, receiver);

  /* Activate the sensors */
  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);

  etimer_set(&periodic_timer, SEND_INTERVAL);

  while(1) {
    /* Wait for a fixed time */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    printf("\n***\nSending packet to multicast adddress ");

    /* Create a link-local multicast address to all nodes */
    uip_create_linklocal_allnodes_mcast(&addr);
    uip_debug_ipaddr_print(&addr);
    printf("\n");

    /* Take sensor readings and store into the message structure */
    take_readings();

    /* Send the multicast packet to all devices */
    simple_udp_sendto(&mcast_connection, &msg, sizeof(msg), &addr);
    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
