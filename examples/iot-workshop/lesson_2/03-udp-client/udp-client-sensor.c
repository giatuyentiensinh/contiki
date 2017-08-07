#include "contiki.h"
#include "lib/random.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-udp-packet.h"
#include <stdio.h>
#include <string.h>

#include "dev/uart1.h"
#include "dev/serial-line.h"
#include "net/ipv6/uip-ds6-route.h"
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
#include "dev/sht25.h"

#define DEBUG DEBUG_FULL
#include "net/ip/uip-debug.h"

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678
/*---------------------------------------------------------------------------*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/*---------------------------------------------------------------------------*/

#define SEND_INTERVAL   (15 * CLOCK_SECOND)

static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;

struct my_msg_t {
  uint16_t adc1;
  uint16_t adc2;
  uint16_t adc3;
  uint16_t temp;
  uint16_t battery;
  uint16_t sensor_temp;
  uint16_t sensor_humi;
};

static struct my_msg_t msg;
/*---------------------------------------------------------------------------*/
PROCESS(udp_client_sensor_process, "UDP client sensor process");
AUTOSTART_PROCESSES(&udp_client_sensor_process);
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
  char *str;
  if(uip_newdata()) {
    str = uip_appdata;
    str[uip_datalen()] = '\0';
    PRINTF("DATA recv " ANSI_COLOR_GREEN "'%s'" ANSI_COLOR_RESET "\n", str);
  }
}
/*---------------------------------------------------------------------------*/
static void
send_packet(void)
{
  msg.adc1        = adc_zoul.value(ZOUL_SENSORS_ADC1);
  msg.adc2        = adc_zoul.value(ZOUL_SENSORS_ADC2);
  msg.adc3        = adc_zoul.value(ZOUL_SENSORS_ADC3);
  msg.temp        = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
  msg.battery     = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
  msg.sensor_temp = sht25.value(SHT25_VAL_TEMP);
  msg.sensor_humi = sht25.value(SHT25_VAL_HUM);

  PRINTF("DATA send " ANSI_COLOR_BLUE "adc1: %u, adc2: %u, adc3: %u, temp: %u, battery: %u, sensor_temp: %u, sensor_humi: %u" ANSI_COLOR_RESET "\n", 
    msg.adc1, msg.adc2, msg.adc3, msg.temp, msg.battery, msg.sensor_temp, msg.sensor_humi);

  msg.adc1        = UIP_HTONS(msg.adc1);
  msg.adc2        = UIP_HTONS(msg.adc2);
  msg.adc3        = UIP_HTONS(msg.adc3);
  msg.temp        = UIP_HTONS(msg.temp);
  msg.battery     = UIP_HTONS(msg.battery);
  msg.sensor_temp = UIP_HTONS(msg.sensor_temp);
  msg.sensor_humi = UIP_HTONS(msg.sensor_humi);

  uip_udp_packet_sendto(client_conn, &msg, sizeof(msg),
                        &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
}
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Server IPv6 addresses:\n" ANSI_COLOR_RED);
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }
  PRINTF(ANSI_COLOR_RESET);
}
/*---------------------------------------------------------------------------*/
static void     
set_global_address(void)
{
  uip_ipaddr_t ipaddr;

  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);  
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_sensor_process, ev, data)
{
  static struct etimer periodic;

  PROCESS_BEGIN();
  PROCESS_PAUSE();

  set_global_address();

  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
  
  PRINTF("Server address: ");
  PRINT6ADDR(&server_ipaddr);
  PRINTF("\n");

  print_local_addresses();

  /* Activate the sensors */
  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);
  sht25.configure(SHT25_RESOLUTION, SHT2X_RES_14T_12RH);
  uart_set_input(1, serial_line_input_byte);

  /* new connection with remote host */
  client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL); 
  if(client_conn == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(client_conn, UIP_HTONS(UDP_CLIENT_PORT)); 

  PRINTF("Created a connection with the server ");
  PRINT6ADDR(&client_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n",
  UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));

  etimer_set(&periodic, SEND_INTERVAL);
  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }

    if(ev == serial_line_event_message && data != NULL) {
      send_packet();      
    }

    if(etimer_expired(&periodic)) {
      etimer_reset(&periodic);
      send_packet();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
