#include "contiki.h"
#include "node-id.h"
#include "net/rpl/rpl.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ip/uip-udp-packet.h"
#include "net/mac/tsch/tsch.h"
#include "net/rpl/rpl-private.h"
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"
#include "button-sensor.h"
#define CONFIG_WAIT_TIME 5
#define UDP_CLIENT_HOST 5678
#define UDP_SERVER_HOST 8765
/*---------------------------------------------------------------------------*/
PROCESS(node_process, "RPL Node");
AUTOSTART_PROCESSES(&node_process, &sensors_process);
/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *server_conn;
static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;
static uip_ipaddr_t ipaddr;
static int seq_id;
/*---------------------------------------------------------------------------*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
  char *appdata;
  if(uip_newdata()) {
    appdata = (char *)uip_appdata;
    appdata[uip_datalen()] = 0;
    PRINTF(ANSI_COLOR_GREEN "DATA recv: %s\n" ANSI_COLOR_RESET, appdata);
  }
}
/*---------------------------------------------------------------------------*/
static void
send_packet()
{
  char buf[50];
  seq_id++;
  PRINTF(ANSI_COLOR_BLUE "DATA send msg from %u to %d 'Hello %d'\n" ANSI_COLOR_RESET, ipaddr.u8[15],
         server_ipaddr.u8[sizeof(server_ipaddr.u8) - 1], seq_id);
  sprintf(buf, "Hello %d from the client %d\n", seq_id, ipaddr.u8[15]);
  uip_udp_packet_sendto(client_conn, buf, strlen(buf),
                        &server_ipaddr, UIP_HTONS(UDP_SERVER_HOST));
}
/*---------------------------------------------------------------------------*/
static void
print_network_status(void)
{
  int i;
  uint8_t state;
  uip_ds6_defrt_t *default_route;
#if RPL_WITH_STORING
  uip_ds6_route_t *route;
#endif /* RPL_WITH_STORING */
#if RPL_WITH_NON_STORING
  rpl_ns_node_t *link;
#endif /* RPL_WITH_NON_STORING */
  PRINTF(ANSI_COLOR_RED "--- Network status ---\n");
  /* Our IPv6 addresses */
  PRINTF("- Server IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINTF("-- ");
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }

  /* Our default route */
  PRINTF("- Default route:\n");
  default_route = uip_ds6_defrt_lookup(uip_ds6_defrt_choose());
  if(default_route != NULL) {
    PRINTF("-- ");
    PRINT6ADDR(&default_route->ipaddr);
    PRINTF(" (lifetime: %lu seconds)\n", (unsigned long)default_route->lifetime.interval);
  } else {
    PRINTF("-- None\n");
  }

#if RPL_WITH_STORING
  /* Our routing entries */
  PRINTF("- Routing entries (%u in total):\n", uip_ds6_route_num_routes());
  route = uip_ds6_route_head();
  while(route != NULL) {
    PRINTF("-- ");
    PRINT6ADDR(&route->ipaddr);
    PRINTF(" via ");
    PRINT6ADDR(uip_ds6_route_nexthop(route));
    PRINTF(" (lifetime: %lu seconds)\n", (unsigned long)route->state.lifetime);
    route = uip_ds6_route_next(route);
  }
#endif

#if RPL_WITH_NON_STORING
  /* Our routing links */
  PRINTF("- Routing links (%u in total):\n", rpl_ns_num_nodes());
  link = rpl_ns_node_head();
  while(link != NULL) {
    uip_ipaddr_t child_ipaddr;
    uip_ipaddr_t parent_ipaddr;
    rpl_ns_get_node_global_addr(&child_ipaddr, link);
    rpl_ns_get_node_global_addr(&parent_ipaddr, link->parent);
    PRINTF("-- ");
    PRINT6ADDR(&child_ipaddr);
    if(link->parent == NULL) {
      memset(&parent_ipaddr, 0, sizeof(parent_ipaddr));
      PRINTF(" --- DODAG root ");
    } else {
      PRINTF(" to ");
      PRINT6ADDR(&parent_ipaddr);
    }
    PRINTF(" (lifetime: %lu seconds)\n", (unsigned long)link->lifetime);
    link = rpl_ns_node_next(link);
  }
#endif

  PRINTF("----------------------\n" ANSI_COLOR_RESET);
}
/*---------------------------------------------------------------------------*/
static void
net_init(uip_ipaddr_t *br_prefix)
{
  uip_ipaddr_t global_ipaddr;
  if(br_prefix) {
    memcpy(&global_ipaddr, br_prefix, 16);
    uip_ds6_set_addr_iid(&global_ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&global_ipaddr, 0, ADDR_AUTOCONF);
    rpl_set_root(RPL_DEFAULT_INSTANCE, &global_ipaddr);
    rpl_set_prefix(rpl_get_any_dag(), br_prefix, 64);
    rpl_repair_root(RPL_DEFAULT_INSTANCE);
  }
  NETSTACK_MAC.on();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_process, ev, data)
{
  static struct etimer et;
  static struct etimer periodic;

  PROCESS_BEGIN();

  static int is_coordinator = 0;
  static enum { role_6ln, role_6dr } node_role;
  node_role = role_6ln;

  SENSORS_ACTIVATE(button_sensor);
  etimer_set(&et, CLOCK_SECOND * CONFIG_WAIT_TIME);

  while(!etimer_expired(&et)) {
    printf("Init: current role: " ANSI_COLOR_RED " %s " ANSI_COLOR_RESET ". Will start in %u seconds. Press user button to toggle mode.\n",
           node_role == role_6ln ? "6ln" : "6dr", CONFIG_WAIT_TIME);
    PROCESS_WAIT_EVENT_UNTIL(((ev == sensors_event) &&
                              (data == &button_sensor) && button_sensor.value(0) > 0)
                             || etimer_expired(&et));
    if(ev == sensors_event && data == &button_sensor && button_sensor.value(0) > 0) {
      node_role = (node_role + 1) % 2;
      etimer_restart(&et);
    }
  }

  printf(ANSI_COLOR_RED "Init: node starting with role %s\n" ANSI_COLOR_RESET,
         node_role == role_6ln ? "6ln" : "6dr");

  tsch_set_pan_secured(0);
  is_coordinator = node_role > role_6ln;

  if(is_coordinator) {
    uip_ipaddr_t prefix;
    uip_ip6addr(&prefix, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    net_init(&prefix);
    server_conn = udp_new(NULL, UIP_HTONS(UDP_CLIENT_HOST), NULL);
    if(server_conn == NULL) {
      PRINTF("No UDP connection available, exiting the process!\n");
      PROCESS_EXIT();
    }
    udp_bind(server_conn, UIP_HTONS(UDP_SERVER_HOST));
    PRINTF("Created a server connection with remote address ");
    PRINT6ADDR(&server_conn->ripaddr);
    PRINTF(" local/remote port %u/%u\n", UIP_HTONS(server_conn->lport),
         UIP_HTONS(server_conn->rport));

  } else {
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    net_init(NULL);
    // hard code
    uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0x212, 0x4b00, 0x9df, 0x4f53);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

    client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_HOST), NULL);
    if(client_conn == NULL) {
      PRINTF("No UDP connection available, exiting the process!\n");
      PROCESS_EXIT();
    }
    udp_bind(client_conn, UIP_HTONS(UDP_CLIENT_HOST));
    PRINTF("Created a connection with the server ");
    PRINT6ADDR(&client_conn->ripaddr);
    PRINTF(" local/remote port %u/%u\n",
        UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));
  }

  /* Print out routing tables every minute */
  etimer_set(&periodic, 15 * 128UL);

  if (is_coordinator)
    while(1) {
      PROCESS_YIELD();
      if (ev == tcpip_event)
        tcpip_handler();
      if (etimer_expired(&periodic)) {
        etimer_reset(&periodic);
        print_network_status();
      }
    }
  else
    while(1) {
      PROCESS_YIELD();
      if (ev == tcpip_event)
        tcpip_handler();
      if (ev == sensors_event && data == &button_sensor)
        send_packet();
      if (etimer_expired(&periodic)) {
        etimer_reset(&periodic);
        send_packet();
        print_network_status();
      }
    }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
