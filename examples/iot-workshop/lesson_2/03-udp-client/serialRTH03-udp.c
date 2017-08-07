#include "contiki.h"
#include "sys/ctimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-udp-packet.h"
#include "net/ipv6/uip-ds6-route.h"
#include <stdio.h>
#include <string.h>

#include "dev/uart1.h"
#include "dev/serial-line.h"
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"

#define DEBUG DEBUG_FULL
#include "net/ip/uip-debug.h"

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678
#define UDP_EXAMPLE_ID  190
/*---------------------------------------------------------------------------*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/*---------------------------------------------------------------------------*/
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

static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "NCT UDP client with Arduino process");
AUTOSTART_PROCESSES(&udp_client_process);
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
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Client IPv6 addresses: " ANSI_COLOR_RED);
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
      /* hack to make address "final" */
      if (state == ADDR_TENTATIVE)
    	  uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
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
PROCESS_THREAD(udp_client_process, ev, data)
{
  PROCESS_BEGIN();
  PROCESS_PAUSE();

  set_global_address();

  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);

  PRINTF("Server address: ");
  PRINT6ADDR(&server_ipaddr);
  PRINTF("\n");

  // active uart1
  uart_set_input(1, serial_line_input_byte);
  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);

  print_local_addresses();

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
  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event)
      tcpip_handler();
    if(ev == serial_line_event_message && data != NULL) {
      char *str = data;
      msg.adc1        = adc_zoul.value(ZOUL_SENSORS_ADC1);
      msg.adc2        = adc_zoul.value(ZOUL_SENSORS_ADC2);
      msg.adc3        = adc_zoul.value(ZOUL_SENSORS_ADC3);
      msg.temp        = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
      msg.battery     = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);

    	uint16_t tmp[2], index = 0, i;
    	PRINTF("Data received from Arduino " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET ".\n", str);

      if (strlen(str) < 3)
        continue;

    	for (i = 0; i < strlen(str); i++)
    		if (str[i] == '\'') {
    			i++;
    			tmp[index++] = 1000 * (str[i] - '0') + 100 * (str[i+1] -'0') + 10 * (str[i+3] - '0') + (str[i+4] - '0');
    			i += 5;
    		}
    	msg.sensor_humi = tmp[0];
    	msg.sensor_temp = tmp[1];

      PRINTF("\nDATA send " ANSI_COLOR_BLUE "adc1: %u, adc2: %u, adc3: %u, temp: %u, battery: %u, sensor_temp: %u, sensor_humi: %u" ANSI_COLOR_RESET "\n", 
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
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
